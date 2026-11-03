#include "DatabaseManager.h"
#include "DriveMetaDao.h"
#include "SmartFolderDao.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <windows.h>
#include "../util/AppDirectoryInitializer.h"

namespace QuarkMeta {

SqlTransaction::SqlTransaction(sqlite3* db) : m_db(db) {
    if (m_db) {
        m_isNested = (sqlite3_get_autocommit(m_db) == 0);
        if (m_isNested) {
            qWarning() << "[SqlTransaction] ⚠️ 警告：检测到数据库连接已处于未提交事务中 (autocommit=0)，本事务判定为嵌套事务/假事务！若上层事务泄漏，后续提交将全部静默失效！";
        } else {
            int rc = sqlite3_exec(m_db, "BEGIN IMMEDIATE TRANSACTION", nullptr, nullptr, nullptr);
            if (rc != SQLITE_OK) {
                qCritical() << "[SqlTransaction] ❌ BEGIN IMMEDIATE TRANSACTION 失败，错误码:" << rc << sqlite3_errmsg(m_db);
            }
        }
    }
}

SqlTransaction::~SqlTransaction() {
    if (m_db && !m_committed) {
        if (m_isNested) {
            qWarning() << "[SqlTransaction] ⚠️ 嵌套事务析构时未显式 commit()，已被跳过，依赖外层事务决定";
        } else {
            qWarning() << "[SqlTransaction] ⚠️ 事务析构时未提交，执行自动 ROLLBACK 回滚！";
            sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
        }
    }
}

bool SqlTransaction::commit() {
    if (m_db && !m_committed) {
        if (m_isNested) {
            qWarning() << "[SqlTransaction] ⚠️ 嵌套事务调用 commit()：仅标记局部提交，实际未向 SQLite 下发 COMMIT 指令！";
            m_committed = true;
            return true;
        }
        int rc = sqlite3_exec(m_db, "COMMIT", nullptr, nullptr, nullptr);
        if (rc == SQLITE_OK) {
            m_committed = true;
            return true;
        } else {
            qCritical() << "[SqlTransaction] ❌ 真实 COMMIT 失败，错误码:" << rc << sqlite3_errmsg(m_db);
        }
    }
    return false;
}

void SqlTransaction::rollback() {
    if (m_db && !m_committed) {
        if (!m_isNested) {
            qWarning() << "[SqlTransaction] 显式执行真实 ROLLBACK";
            sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
        } else {
            qWarning() << "[SqlTransaction] ⚠️ 嵌套事务调用 rollback()，无法单独向 SQLite 下发 ROLLBACK，请注意外层一致性！";
        }
        m_committed = true;
    }
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::DatabaseManager(QObject* parent) : QObject(parent) {
    if (QCoreApplication::instance()) {
        this->moveToThread(QCoreApplication::instance()->thread());
    }
}

DatabaseManager::~DatabaseManager() {
    shutdown();
}

QString DatabaseManager::getAppDir() {
    return QCoreApplication::applicationDirPath();
}

QString DatabaseManager::getGlobalDbPath() {
    QString dir = QDir::toNativeSeparators(getAppDir() + "/.QuarkMeta");
    return QDir::toNativeSeparators(dir + "/QuarkMeta.store");
}

QString DatabaseManager::getBackupDir() {
    QString dir = QDir::toNativeSeparators(getAppDir() + "/.QuarkMeta/backups");
    QDir().mkpath(dir);
    return dir;
}

void DatabaseManager::performDailyBackup() {
    QString dbPath = getGlobalDbPath();
    if (!QFile::exists(dbPath)) return;

    QString backupDir = getBackupDir();
    QString todayStr = QDate::currentDate().toString("yyyyMMdd");
    QString todayBackupPath = QDir::toNativeSeparators(backupDir + QString("/QuarkMeta_%1.store.bak").arg(todayStr));

    // 当天已存在备份副本，直接跳过，零磁盘写入磨损
    if (QFile::exists(todayBackupPath)) {
        return;
    }

    // 执行按天快照复制
    QFile::copy(dbPath, todayBackupPath);
    qInfo() << "[DatabaseManager] 已成功创建每日数据库快照备份:" << todayBackupPath;

    // 清理超过 7 天的历史快照文件
    QDir bDir(backupDir);
    QStringList filters;
    filters << "QuarkMeta_*.store.bak";
    QFileInfoList backupFiles = bDir.entryInfoList(filters, QDir::Files, QDir::Time); // 按修改时间降序排序

    if (backupFiles.size() > 7) {
        for (int i = 7; i < backupFiles.size(); ++i) {
            QFile::remove(backupFiles[i].absoluteFilePath());
            qInfo() << "[DatabaseManager] 已自动清理过期历史快照:" << backupFiles[i].fileName();
        }
    }
}

bool DatabaseManager::tryRecoverFromBackup() {
    QString backupDir = getBackupDir();
    QDir bDir(backupDir);
    QStringList filters;
    filters << "QuarkMeta_*.store.bak";
    QFileInfoList backupFiles = bDir.entryInfoList(filters, QDir::Files, QDir::Time);

    if (backupFiles.isEmpty()) return false;

    QString dbPath = getGlobalDbPath();
    // 🚨 严密清理：不仅移除 .store 主文件，必须同步彻底移除残留的 -wal 与 -shm，防止旧快照与新 WAL 合并回放导致数据库污染
    QFile::remove(dbPath);
    QFile::remove(dbPath + "-wal");
    QFile::remove(dbPath + "-shm");

    for (const auto& bFile : backupFiles) {
        if (QFile::copy(bFile.absoluteFilePath(), dbPath)) {
            qCritical() << "[DatabaseManager] 🚨 警告：已从自动备份文件自愈恢复数据库快照:" << bFile.fileName()
                        << "，原 -wal 与 -shm 日志已完全清除，确保快照纯净装载！";
            return true;
        }
    }
    return false;
}

bool DatabaseManager::loadDb(const std::wstring& diskPath, DbConnection& conn) {
    QString qDiskPath = QDir::toNativeSeparators(QString::fromStdWString(diskPath));
    QFileInfo fileInfo(qDiskPath);
    
    QDir().mkpath(fileInfo.absolutePath());
    SetFileAttributesW(fileInfo.absolutePath().toStdWString().c_str(), FILE_ATTRIBUTE_HIDDEN);

    std::wstring nativeWPath = qDiskPath.toStdWString();

    int rc = sqlite3_open16(nativeWPath.c_str(), &conn.diskDb);
    if (rc != SQLITE_OK || !conn.diskDb) {
        qCritical() << "[DatabaseManager] 无法打开/创建全局数据库:" << qDiskPath;
        if (conn.diskDb) {
            sqlite3_close_v2(conn.diskDb);
            conn.diskDb = nullptr;
        }
        return false;
    }

    sqlite3_busy_timeout(conn.diskDb, 25000);
    sqlite3_exec(conn.diskDb, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    sqlite3_exec(conn.diskDb, "PRAGMA journal_mode = WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(conn.diskDb, "PRAGMA synchronous = NORMAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(conn.diskDb, "PRAGMA temp_store = MEMORY;", nullptr, nullptr, nullptr);

    SmartFolderDao::registerColorFunctions(conn.diskDb);

    constexpr int kSchemaVersion = 1;
    int currentVersion = 0;
    sqlite3_stmt* verStmt = nullptr;
    if (sqlite3_prepare_v2(conn.diskDb, "PRAGMA user_version;", -1, &verStmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(verStmt) == SQLITE_ROW) {
            currentVersion = sqlite3_column_int(verStmt, 0);
        }
        sqlite3_finalize(verStmt);
    } else {
        logSqlError("PRAGMA user_version read", conn.diskDb);
    }

    if (currentVersion != kSchemaVersion) {
        char* err = nullptr;
        if (sqlite3_exec(conn.diskDb, "BEGIN TRANSACTION;", nullptr, nullptr, &err) == SQLITE_OK) {
            sqlite3_exec(conn.diskDb, "DROP TABLE IF EXISTS item_palettes;", nullptr, nullptr, nullptr);
            sqlite3_exec(conn.diskDb, "DROP TABLE IF EXISTS file_tags;", nullptr, nullptr, nullptr);
            sqlite3_exec(conn.diskDb, "DROP TABLE IF EXISTS library_items;", nullptr, nullptr, nullptr);
            if (sqlite3_exec(conn.diskDb, "COMMIT;", nullptr, nullptr, &err) != SQLITE_OK) {
                logSqlError("Version upgrade transaction commit", conn.diskDb, "COMMIT;");
                if (err) sqlite3_free(err);
                sqlite3_close_v2(conn.diskDb);
                conn.diskDb = nullptr;
                return false;
            }
        } else {
            logSqlError("Version upgrade transaction begin", conn.diskDb, "BEGIN TRANSACTION;");
            if (err) sqlite3_free(err);
            sqlite3_close_v2(conn.diskDb);
            conn.diskDb = nullptr;
            return false;
        }
    }

    const char* schema = R"(
        -- ===================================================================
        -- 1. 基础字典表：标签管理
        -- ===================================================================
        CREATE TABLE IF NOT EXISTS tags (
            name TEXT PRIMARY KEY COLLATE NOCASE,   -- 忽略大小写
            color TEXT DEFAULT '',
            pinned INTEGER DEFAULT 0,
            use_count INTEGER DEFAULT 0,            -- 使用频次
            last_used INTEGER DEFAULT 0             -- 最后使用时间
        );
        -- 索引：覆盖按热门使用量排序、按最后使用时间排序
        CREATE INDEX IF NOT EXISTS idx_tags_use_count ON tags(use_count DESC);
        CREATE INDEX IF NOT EXISTS idx_tags_last_used ON tags(last_used DESC);

        -- ===================================================================
        -- 2. 标签组及多对多关联
        -- ===================================================================
        CREATE TABLE IF NOT EXISTS tag_groups (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,              -- 组名禁止重复
            color TEXT DEFAULT '',
            sort_order INTEGER DEFAULT 0
        );
        CREATE INDEX IF NOT EXISTS idx_tag_groups_sort ON tag_groups(sort_order ASC);

        CREATE TABLE IF NOT EXISTS tag_group_items (
            group_id INTEGER,
            tag_name TEXT COLLATE NOCASE,
            PRIMARY KEY (group_id, tag_name),
            FOREIGN KEY (group_id) REFERENCES tag_groups(id) ON DELETE CASCADE,
            FOREIGN KEY (tag_name) REFERENCES tags(name) ON DELETE CASCADE ON UPDATE CASCADE
        );
        CREATE INDEX IF NOT EXISTS idx_tag_group_items_tag ON tag_group_items(tag_name);

        -- ===================================================================
        -- 3. 全局文件索引表 (Sidecar File_ID 主键契约)
        -- ===================================================================
        CREATE TABLE IF NOT EXISTS library_items (
            file_id TEXT PRIMARY KEY,               -- 持久化唯一标识 (UUID/Base36)
            meta_file_id TEXT DEFAULT '',           -- .QuarkMeta.json 自身 File_ID 凭据
            file_path TEXT NOT NULL UNIQUE COLLATE NOCASE, -- 绝对物理路径
            file_name TEXT NOT NULL,                -- 文件名（含后缀）
            base_name TEXT NOT NULL,                -- 纯文件名（不含后缀，搜索用）
            extension TEXT NOT NULL COLLATE NOCASE, -- 后缀（如 png、md）
            file_size INTEGER DEFAULT 0,
            is_dir INTEGER DEFAULT 0,
            is_empty INTEGER DEFAULT 0,
            is_hidden INTEGER DEFAULT 0,
            is_managed INTEGER DEFAULT 0,
            encrypted INTEGER DEFAULT 0,
            pinned INTEGER DEFAULT 0,
            rating INTEGER DEFAULT 0,               -- 评级 0~5
            color_hex TEXT DEFAULT '',              -- 标记颜色
            auto_color TEXT DEFAULT '',             -- 自动调色板特征 Hex
            width INTEGER DEFAULT 0,
            height INTEGER DEFAULT 0,
            thumb_status INTEGER DEFAULT 0,
            note TEXT DEFAULT '',                   -- 备注
            url TEXT DEFAULT '',                    -- 关联网址
            sha256 TEXT DEFAULT '',
            created_at INTEGER DEFAULT 0,
            updated_at INTEGER DEFAULT 0,
            added_at INTEGER DEFAULT 0,
            sync_state INTEGER DEFAULT 0,
            synced_at INTEGER DEFAULT 0
        );
        -- 为物理路径及智能文件夹的高频过滤字段建立 B-Tree 索引
        CREATE INDEX IF NOT EXISTS idx_items_meta_file_id ON library_items(meta_file_id);
        CREATE INDEX IF NOT EXISTS idx_items_rating ON library_items(rating);
        CREATE INDEX IF NOT EXISTS idx_items_ext ON library_items(extension);
        CREATE INDEX IF NOT EXISTS idx_items_color ON library_items(color_hex);
        CREATE INDEX IF NOT EXISTS idx_items_updated ON library_items(updated_at DESC);

        -- ===================================================================
        -- 4. 主色调调色板拆分表
        -- ===================================================================
        CREATE TABLE IF NOT EXISTS item_palettes (
            file_id TEXT NOT NULL,
            idx INTEGER NOT NULL,
            color_hex TEXT DEFAULT '',
            ratio REAL DEFAULT 0,
            PRIMARY KEY (file_id, idx),
            FOREIGN KEY (file_id) REFERENCES library_items(file_id) ON DELETE CASCADE ON UPDATE CASCADE
        );

        -- ===================================================================
        -- 5. 文件与标签的多对多解耦关联表
        -- ===================================================================
        CREATE TABLE IF NOT EXISTS file_tags (
            file_id TEXT NOT NULL,
            tag_name TEXT NOT NULL COLLATE NOCASE,
            PRIMARY KEY (file_id, tag_name),
            FOREIGN KEY (file_id) REFERENCES library_items(file_id) ON DELETE CASCADE ON UPDATE CASCADE,
            FOREIGN KEY (tag_name) REFERENCES tags(name) ON DELETE CASCADE ON UPDATE CASCADE
        );
        CREATE INDEX IF NOT EXISTS idx_file_tags_tag ON file_tags(tag_name);

        -- ===================================================================
        -- 6. 回收站表（补全高频检索与恢复索引）
        -- ===================================================================
        CREATE TABLE IF NOT EXISTS disk_trash (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_id TEXT NOT NULL,
            trash_path TEXT NOT NULL,
            original_path TEXT NOT NULL,
            drive_letter TEXT NOT NULL,
            file_name TEXT NOT NULL,
            is_folder INTEGER DEFAULT 0,
            file_size INTEGER DEFAULT 0,
            created_at INTEGER DEFAULT 0,
            deleted_at INTEGER DEFAULT 0
        );
        CREATE UNIQUE INDEX IF NOT EXISTS idx_disk_trash_file_id ON disk_trash(file_id);
        CREATE INDEX IF NOT EXISTS idx_disk_trash_orig_path ON disk_trash(original_path);
        CREATE INDEX IF NOT EXISTS idx_disk_trash_deleted ON disk_trash(deleted_at DESC);

        -- ===================================================================
        -- 7. 驱动器与智能文件夹元数据
        -- ===================================================================
        CREATE TABLE IF NOT EXISTS drive_metadata (
            drive_path TEXT PRIMARY KEY,
            rating INTEGER DEFAULT 0,
            color TEXT DEFAULT '',
            pinned INTEGER DEFAULT 0,
            note TEXT DEFAULT '',
            url TEXT DEFAULT '',
            updated_at INTEGER DEFAULT 0
        );

        CREATE TABLE IF NOT EXISTS smart_folders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            match_mode TEXT DEFAULT 'ALL',
            icon_key TEXT DEFAULT 'folder_filled',
            color_hex TEXT DEFAULT '#888888',
            sort_order INTEGER DEFAULT 0,
            rules_json TEXT DEFAULT '[]',
            created_at INTEGER
        );
        CREATE INDEX IF NOT EXISTS idx_smart_folders_sort ON smart_folders(sort_order ASC);
    )";

    char* errMsg = nullptr;
    if (sqlite3_exec(conn.diskDb, schema, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        logSqlError("Schema creation", conn.diskDb, schema);
        if (errMsg) sqlite3_free(errMsg);
        sqlite3_close_v2(conn.diskDb);
        conn.diskDb = nullptr;
        return false;
    }

    if (currentVersion != kSchemaVersion) {
        char* verErr = nullptr;
        QString setVerSql = QString("PRAGMA user_version = %1;").arg(kSchemaVersion);
        if (sqlite3_exec(conn.diskDb, setVerSql.toUtf8().constData(), nullptr, nullptr, &verErr) != SQLITE_OK) {
            logSqlError("PRAGMA user_version set", conn.diskDb, setVerSql);
            if (verErr) sqlite3_free(verErr);
        }
    }

    // 🚨 核心加固：立即执行 TRUNCATE 检查点，将表结构瞬间从 -wal 固化回 QuarkMeta.store 主文件！
    sqlite3_wal_checkpoint_v2(conn.diskDb, nullptr, SQLITE_CHECKPOINT_TRUNCATE, nullptr, nullptr);

    conn.diskPath = nativeWPath;
    qInfo() << "[DatabaseManager] 全局表结构已成功固化到物理数据库:" << qDiskPath;
    return true;
}

void DatabaseManager::closeDb(DbConnection& conn) {
    if (conn.diskDb) {
        sqlite3_close_v2(conn.diskDb);
        conn.diskDb = nullptr;
    }
}

bool DatabaseManager::initLocked() {
    if (m_isInitialized && m_globalDb.diskDb) {
        return true;
    }

    AppDirectoryInitializer::initializeStoragePath(getAppDir());

    std::wstring globalPath = getGlobalDbPath().toStdWString();
    if (!loadDb(globalPath, m_globalDb)) {
        qWarning() << "[DatabaseManager] 主数据库装载失败，尝试从 7 天自动快照自愈恢复...";
        if (tryRecoverFromBackup()) {
            if (!loadDb(globalPath, m_globalDb)) {
                return false;
            }
        } else {
            return false;
        }
    }

    m_isInitialized = true;
    performDailyBackup();
    return true;
}

bool DatabaseManager::init() { 
    bool needTableInit = false;
    {
        std::lock_guard<std::mutex> lock(m_initMutex);
        if (!m_isInitialized || !m_globalDb.diskDb) {
            if (initLocked()) {
                needTableInit = true;
            } else {
                return false;
            }
        }
    } 
    if (needTableInit) { 
        DriveMetaDao::initTable(); 
    } 
    return true; 
}

void DatabaseManager::shutdown() {
    std::lock_guard<std::mutex> lock(m_initMutex);
    std::lock_guard<std::mutex> dbLock(m_globalDbMutex);
    closeDb(m_globalDb);
    m_isInitialized = false;
}

sqlite3* DatabaseManager::getGlobalDb() { 
    bool needTableInit = false;
    sqlite3* db = nullptr;
    {
        std::lock_guard<std::mutex> lock(m_initMutex);
        if (!m_isInitialized || !m_globalDb.diskDb) { 
            if (initLocked()) {
                needTableInit = true;
            }
        } 
        db = m_globalDb.diskDb;
    }
    if (needTableInit) { 
        DriveMetaDao::initTable(); 
    } 
    return db; 
}

void DatabaseManager::logSqlError(const char* location, sqlite3* db, const QString& sql) {
    const char* errmsg = db ? sqlite3_errmsg(db) : "Unknown error (null db)";
    if (!sql.isEmpty()) {
        QString truncatedSql = sql.left(120);
        qWarning() << QString("[DatabaseError][%1] %2 | SQL: %3").arg(location, errmsg, truncatedSql);
    } else {
        qWarning() << QString("[DatabaseError][%1] %2").arg(location, errmsg);
    }
}

} // namespace QuarkMeta