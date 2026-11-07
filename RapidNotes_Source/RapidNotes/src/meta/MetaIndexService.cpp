#include "MetaIndexService.h"
#include "DatabaseManager.h"
#include "QuarkMetaJson.h"
#include "QuarkMetaJsonStore.h"
#include "LibraryService.h"
#include "../core/TaskProgressService.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <chrono>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

namespace {
std::wstring normalizePath(const std::wstring& path) {
    if (path.empty()) return L"";
    QString qp = QDir::toNativeSeparators(QDir::cleanPath(QString::fromStdWString(path))).toLower();
    if (qp.length() == 2 && qp.endsWith(':')) qp += '\\';
    return qp.toStdWString();
}
} // namespace

MetaIndexService& MetaIndexService::instance() {
    static MetaIndexService inst;
    return inst;
}

MetaIndexService::MetaIndexService(QObject* parent) : QObject(parent) {
    m_dbPath = DatabaseManager::getGlobalDbPath().toStdWString();

    if (auto* app = QCoreApplication::instance()) {
        connect(app, &QCoreApplication::aboutToQuit, this, [this]() {
            shutdown();
        });
    }
}

MetaIndexService::~MetaIndexService() {
    shutdown();
}

void MetaIndexService::init() {
    if (m_initialized.exchange(true)) return;

    // 1. 初始化独占写连接
    if (!openDatabase(m_dbPath, &m_writeDb, false)) {
        qWarning() << "[MetaIndexService] Failed to open write database, index will be disabled.";
        return;
    }

    if (!initSchema(m_writeDb)) {
        qWarning() << "[MetaIndexService] Schema initialization failed for inspiration.db.";
    }

    // 2. 初始化独立只读连接
    openDatabase(m_dbPath, &m_readDb, true);

    // 3. 启动后台低优先级消费线程
    m_workerThread = std::thread(&MetaIndexService::workerThreadLoop, this);
}

void MetaIndexService::shutdown() {
    if (!m_initialized.load()) return;
    m_stopping.store(true);
    m_cv.notify_all();

    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }

    {
        std::lock_guard<std::mutex> lock(m_readMutex);
        if (m_readDb) {
            sqlite3_close(m_readDb);
            m_readDb = nullptr;
        }
    }

    if (m_writeDb) {
        sqlite3_close(m_writeDb);
        m_writeDb = nullptr;
    }

    m_initialized.store(false);
}

bool MetaIndexService::openDatabase(const std::wstring& dbPath, sqlite3** outDb, bool readOnly) {
    int flags = readOnly ? (SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX)
                         : (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX);

    if (sqlite3_open_v2(QString::fromStdWString(dbPath).toUtf8().constData(), outDb, flags, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_busy_timeout(*outDb, 5000);

    // 工业级轻量参数调优：WAL、NORMAL 刷盘、内存临时表与缓存
    sqlite3_exec(*outDb, "PRAGMA journal_mode = WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(*outDb, "PRAGMA synchronous = NORMAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(*outDb, "PRAGMA temp_store = MEMORY;", nullptr, nullptr, nullptr);
    sqlite3_exec(*outDb, "PRAGMA cache_size = -32000;", nullptr, nullptr, nullptr); // 32MB
    return true;
}

bool MetaIndexService::initSchema(sqlite3* db) {
    const char* schema = R"(
        CREATE TABLE IF NOT EXISTS meta_version (
            version INTEGER PRIMARY KEY
        );
        CREATE TABLE IF NOT EXISTS folders_index (
            folder_path TEXT PRIMARY KEY,
            mtime INTEGER NOT NULL,
            size INTEGER NOT NULL,
            indexed_at INTEGER NOT NULL
        );
        CREATE TABLE IF NOT EXISTS items_index (
            path TEXT PRIMARY KEY,
            folder_path TEXT NOT NULL,
            name TEXT NOT NULL,
            type TEXT NOT NULL,
            rating INTEGER DEFAULT 0,
            color TEXT DEFAULT '',
            tags TEXT DEFAULT '',
            note TEXT DEFAULT '',
            url TEXT DEFAULT '',
            width INTEGER DEFAULT 0,
            height INTEGER DEFAULT 0,
            auto_color TEXT DEFAULT '',
            file_size INTEGER DEFAULT 0,
            file_mtime INTEGER DEFAULT 0
        );
        CREATE INDEX IF NOT EXISTS idx_items_folder ON items_index(folder_path);
        CREATE INDEX IF NOT EXISTS idx_items_name ON items_index(name);
        CREATE INDEX IF NOT EXISTS idx_items_rating ON items_index(rating);
        CREATE INDEX IF NOT EXISTS idx_items_color ON items_index(color);
        CREATE INDEX IF NOT EXISTS idx_items_type ON items_index(type);
        CREATE INDEX IF NOT EXISTS idx_items_mtime ON items_index(file_mtime DESC);

        CREATE VIRTUAL TABLE IF NOT EXISTS items_fts USING fts5(
            name,
            note,
            tags,
            tokenize = 'unicode61'
        );

        CREATE TRIGGER IF NOT EXISTS idx_items_ai AFTER INSERT ON items_index BEGIN
            INSERT INTO items_fts(rowid, name, note, tags) VALUES (new.rowid, new.name, new.note, new.tags);
        END;
        CREATE TRIGGER IF NOT EXISTS idx_items_ad AFTER DELETE ON items_index BEGIN
            DELETE FROM items_fts WHERE rowid = old.rowid;
        END;
        CREATE TRIGGER IF NOT EXISTS idx_items_au AFTER UPDATE ON items_index BEGIN
            DELETE FROM items_fts WHERE rowid = old.rowid;
            INSERT INTO items_fts(rowid, name, note, tags) VALUES (new.rowid, new.name, new.note, new.tags);
        END;
    )";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, schema, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

void MetaIndexService::enqueueFolder(const std::wstring& folderPath) {
    if (folderPath.empty() || m_stopping.load()) return;
    init();

    std::wstring norm = normalizePath(folderPath);
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_pendingFolders[norm] = folderPath;
    }
    m_cv.notify_one();
}

void MetaIndexService::enqueueCategoryIndexing(int categoryId, const QStringList& paths) {
    if (categoryId <= 0 || paths.isEmpty() || m_stopping.load()) return;
    init();

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_pendingCategoryTasks.push_back({categoryId, paths});
    }
    m_cv.notify_one();
}

void MetaIndexService::workerThreadLoop() {
#ifdef Q_OS_WIN
    // 操作系统级后台低优先级让步：不争抢前台 I/O 和 CPU
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif

    while (!m_stopping.load()) {
        std::unordered_map<std::wstring, std::wstring> currentBatch;
        std::vector<CategoryTask> categoryBatch;

        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            // 300ms 防抖沉淀窗口
            m_cv.wait_for(lock, std::chrono::milliseconds(300), [this]() {
                return m_stopping.load();
            });

            if (m_stopping.load()) break;
            if (m_pendingFolders.empty() && m_pendingCategoryTasks.empty()) {
                m_cv.wait(lock, [this]() {
                    return !m_pendingFolders.empty() || !m_pendingCategoryTasks.empty() || m_stopping.load();
                });
                if (m_stopping.load()) break;
            }

            currentBatch.swap(m_pendingFolders);
            categoryBatch.swap(m_pendingCategoryTasks);
        }

        // 1. 处理分类路径后台批量索引任务 (现统一由 LibrarySyncService 自动同步)
        if (!categoryBatch.empty()) {
            int totalPaths = 0;
            for (const auto& task : categoryBatch) {
                totalPaths += task.paths.size();
            }
            int jobId = TaskProgressService::instance().createJob("正在归入库分类...", totalPaths);
            int processedPaths = 0;

            for (const auto& task : categoryBatch) {
                if (m_stopping.load()) break;
                if (!task.paths.isEmpty()) {
                    LibraryService::instance().addPathsToCategory(task.categoryId, task.paths);
                    processedPaths += task.paths.size();
                    TaskProgressService::instance().updateProgress(jobId, processedPaths, totalPaths, QString("已归入 %1 / %2 项").arg(processedPaths).arg(totalPaths));
                }
            }
            TaskProgressService::instance().finishJob(jobId);
        }

        if (currentBatch.empty() || !m_writeDb) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // 2. 显式大事务处理已落盘文件夹批次
        sqlite3_exec(m_writeDb, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

        for (const auto& kv : currentBatch) {
            if (m_stopping.load()) break;
            processFolder(m_writeDb, kv.first, kv.second);
        }

        sqlite3_exec(m_writeDb, "COMMIT;", nullptr, nullptr, nullptr);

        // 批次完成，主动让出 10ms CPU 保证前台丝滑
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MetaIndexService::processFolder(sqlite3* db, const std::wstring& normFolder, const std::wstring& rawFolder) {
    QString jsonPath = QString::fromStdWString(rawFolder) + "/.QuarkMeta.json";
    QFileInfo fi(jsonPath);

    if (!fi.exists()) {
        // .QuarkMeta.json 不存在（被自愈清理），抹除该文件夹下所有旧索引
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, "DELETE FROM items_index WHERE folder_path = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, QString::fromStdWString(normFolder).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        if (sqlite3_prepare_v2(db, "DELETE FROM folders_index WHERE folder_path = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, QString::fromStdWString(normFolder).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        return;
    }

    qint64 currentMtime = fi.lastModified().toMSecsSinceEpoch();
    qint64 currentSize = fi.size();

    // 1. 新鲜度检查：mtime 和 size 没变，直接跳过整个文件夹
    sqlite3_stmt* checkStmt = nullptr;
    bool needUpdate = true;
    if (sqlite3_prepare_v2(db, "SELECT mtime, size FROM folders_index WHERE folder_path = ?;", -1, &checkStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(checkStmt, 1, QString::fromStdWString(normFolder).toUtf8().constData(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(checkStmt) == SQLITE_ROW) {
            qint64 dbMtime = sqlite3_column_int64(checkStmt, 0);
            qint64 dbSize = sqlite3_column_int64(checkStmt, 1);
            if (dbMtime == currentMtime && dbSize == currentSize) {
                needUpdate = false;
            }
        }
        sqlite3_finalize(checkStmt);
    }

    if (!needUpdate) return;

    // 2. 读磁盘 JSON 并解析
    QuarkMetaJson jsonReader(rawFolder);
    if (!jsonReader.load()) return;

    // 3. 增量更新该文件夹的条目
    sqlite3_stmt* delStmt = nullptr;
    if (sqlite3_prepare_v2(db, "DELETE FROM items_index WHERE folder_path = ?;", -1, &delStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(delStmt, 1, QString::fromStdWString(normFolder).toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_step(delStmt);
        sqlite3_finalize(delStmt);
    }

    const char* insertSql = R"(
        INSERT INTO items_index (path, folder_path, name, type, rating, color, tags, note, url, width, height, auto_color, file_size, file_mtime)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ON CONFLICT(path) DO UPDATE SET
            folder_path=excluded.folder_path,
            name=excluded.name,
            type=excluded.type,
            rating=excluded.rating,
            color=excluded.color,
            tags=excluded.tags,
            note=excluded.note,
            url=excluded.url,
            width=excluded.width,
            height=excluded.height,
            auto_color=excluded.auto_color,
            file_size=excluded.file_size,
            file_mtime=excluded.file_mtime;
    )";

    sqlite3_stmt* insStmt = nullptr;
    if (sqlite3_prepare_v2(db, insertSql, -1, &insStmt, nullptr) == SQLITE_OK) {
        for (const auto& kv : jsonReader.items()) {
            const std::wstring& fileName = kv.first;
            const ItemMeta& meta = kv.second;

            QString fullPath = QDir(QString::fromStdWString(rawFolder)).filePath(QString::fromStdWString(fileName));
            std::wstring normItemPath = normalizePath(fullPath.toStdWString());

            QString tagsJoined;
            for (const auto& t : meta.tags) {
                if (!tagsJoined.isEmpty()) tagsJoined.append(" ");
                tagsJoined.append(QString::fromStdWString(t));
            }

            sqlite3_bind_text(insStmt, 1, QString::fromStdWString(normItemPath).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insStmt, 2, QString::fromStdWString(normFolder).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insStmt, 3, QString::fromStdWString(fileName).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insStmt, 4, QString::fromStdWString(meta.type).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(insStmt, 5, meta.rating);
            sqlite3_bind_text(insStmt, 6, QString::fromStdWString(meta.color).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insStmt, 7, tagsJoined.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insStmt, 8, QString::fromStdWString(meta.note).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insStmt, 9, QString::fromStdWString(meta.url).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(insStmt, 10, meta.width);
            sqlite3_bind_int(insStmt, 11, meta.height);
            sqlite3_bind_text(insStmt, 12, QString::fromStdWString(meta.autoColor).toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(insStmt, 13, meta.size);
            sqlite3_bind_int64(insStmt, 14, meta.modificationTime);

            sqlite3_step(insStmt);
            sqlite3_reset(insStmt);
        }
        sqlite3_finalize(insStmt);
    }

    // 4. 更新文件夹索引时间戳
    const char* folderSql = R"(
        INSERT INTO folders_index (folder_path, mtime, size, indexed_at)
        VALUES (?, ?, ?, ?)
        ON CONFLICT(folder_path) DO UPDATE SET
            mtime=excluded.mtime,
            size=excluded.size,
            indexed_at=excluded.indexed_at;
    )";
    sqlite3_stmt* fStmt = nullptr;
    if (sqlite3_prepare_v2(db, folderSql, -1, &fStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(fStmt, 1, QString::fromStdWString(normFolder).toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(fStmt, 2, currentMtime);
        sqlite3_bind_int64(fStmt, 3, currentSize);
        sqlite3_bind_int64(fStmt, 4, QDateTime::currentMSecsSinceEpoch());
        sqlite3_step(fStmt);
        sqlite3_finalize(fStmt);
    }
}

QStringList MetaIndexService::searchGlobal(const QString& keyword, const QString& scopeFolderPath, int limit) {
    QStringList results;
    if (keyword.trimmed().isEmpty()) return results;
    init();

    std::lock_guard<std::mutex> lock(m_readMutex);
    if (!m_readDb) return results;

    std::string sql;
    bool hasScope = !scopeFolderPath.trimmed().isEmpty();
    if (hasScope) {
        sql = "SELECT i.path FROM items_index i JOIN items_fts f ON i.rowid = f.rowid WHERE i.folder_path LIKE ? AND f MATCH ? LIMIT ?;";
    } else {
        sql = "SELECT i.path FROM items_index i JOIN items_fts f ON i.rowid = f.rowid WHERE f MATCH ? LIMIT ?;";
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_readDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return results;
    }

    QString cleanKeyword = keyword.trimmed();
    cleanKeyword.replace("\"", "");
    QString ftsQuery = QString("\"%1\"*").arg(cleanKeyword);
    QByteArray ftsQueryUtf8 = ftsQuery.toUtf8();

    int idx = 1;
    if (hasScope) {
        QString scopePattern = QString::fromStdWString(normalizePath(scopeFolderPath.toStdWString())) + "%";
        sqlite3_bind_text(stmt, idx++, scopePattern.toUtf8().constData(), -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_text(stmt, idx++, ftsQueryUtf8.constData(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, idx++, limit);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* p = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (p) {
            results.append(QString::fromUtf8(p));
        }
    }
    sqlite3_finalize(stmt);
    return results;
}

} // namespace QuarkMeta
