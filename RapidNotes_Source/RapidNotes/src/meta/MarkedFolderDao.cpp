#include "MarkedFolderDao.h"
#include "DatabaseManager.h"
#include "Logger.h"
#include <QDir>
#include <QDateTime>

namespace QuarkMeta {

bool MarkedFolderDao::initTable() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS marked_folders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            folder_path TEXT UNIQUE NOT NULL COLLATE NOCASE,
            is_marked INTEGER DEFAULT 1,
            updated_at INTEGER DEFAULT 0
        );
        CREATE INDEX IF NOT EXISTS idx_marked_folders_path ON marked_folders(folder_path);
        CREATE INDEX IF NOT EXISTS idx_marked_folders_marked ON marked_folders(is_marked);
    )";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) {
            Logger::log(QString("[MarkedFolderDao] initTable error: %1").arg(errMsg));
            sqlite3_free(errMsg);
        }
        return false;
    }
    return true;
}

int MarkedFolderDao::getMarkedCount() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return 0;

    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());
    const char* sql = "SELECT COUNT(*) FROM marked_folders WHERE is_marked = 1;";
    sqlite3_stmt* stmt = nullptr;
    int count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return count;
}

bool MarkedFolderDao::addMarkedFolder(const QString& folderPath) {
    if (folderPath.isEmpty()) return false;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    QString cleanPath = QDir::cleanPath(folderPath);
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());

    // 检查限制：已标记数量达到 50 条且当前未被标记
    const char* countSql = "SELECT COUNT(*) FROM marked_folders WHERE is_marked = 1;";
    sqlite3_stmt* countStmt = nullptr;
    int activeCount = 0;
    if (sqlite3_prepare_v2(db, countSql, -1, &countStmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(countStmt) == SQLITE_ROW) {
            activeCount = sqlite3_column_int(countStmt, 0);
        }
    }
    if (countStmt) sqlite3_finalize(countStmt);

    // 双级淘汰：如果总数达到 50，优先清理 is_marked = 0 的最旧记录；若无则清理最旧记录
    const char* totalSql = "SELECT COUNT(*) FROM marked_folders;";
    sqlite3_stmt* totalStmt = nullptr;
    int totalCount = 0;
    if (sqlite3_prepare_v2(db, totalSql, -1, &totalStmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(totalStmt) == SQLITE_ROW) {
            totalCount = sqlite3_column_int(totalStmt, 0);
        }
    }
    if (totalStmt) sqlite3_finalize(totalStmt);

    if (totalCount >= 50) {
        const char* evictSql = R"(
            DELETE FROM marked_folders WHERE id IN (
                SELECT id FROM marked_folders ORDER BY is_marked ASC, updated_at ASC LIMIT 1
            );
        )";
        sqlite3_exec(db, evictSql, nullptr, nullptr, nullptr);
    }

    const char* upsertSql = R"(
        INSERT INTO marked_folders (folder_path, is_marked, updated_at)
        VALUES (?, 1, ?)
        ON CONFLICT(folder_path) DO UPDATE SET
            is_marked = 1,
            updated_at = excluded.updated_at;
    )";

    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(db, upsertSql, -1, &stmt, nullptr) == SQLITE_OK) {
        QByteArray pathUtf8 = cleanPath.toUtf8();
        sqlite3_bind_text(stmt, 1, pathUtf8.constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 2, now);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return success;
}

bool MarkedFolderDao::unmarkFolder(const QString& folderPath) {
    if (folderPath.isEmpty()) return false;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    QString cleanPath = QDir::cleanPath(folderPath);
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());
    const char* sql = "UPDATE marked_folders SET is_marked = 0, updated_at = ? WHERE folder_path = ?;";
    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, now);
        QByteArray pathUtf8 = cleanPath.toUtf8();
        sqlite3_bind_text(stmt, 2, pathUtf8.constData(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return success;
}

bool MarkedFolderDao::unmarkAllFolders() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());
    const char* sql = "UPDATE marked_folders SET is_marked = 0, updated_at = ?;";
    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, now);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return success;
}

bool MarkedFolderDao::removeFolderRecord(const QString& folderPath) {
    if (folderPath.isEmpty()) return false;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    QString cleanPath = QDir::cleanPath(folderPath);
    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());
    const char* sql = "DELETE FROM marked_folders WHERE folder_path = ?;";
    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        QByteArray pathUtf8 = cleanPath.toUtf8();
        sqlite3_bind_text(stmt, 1, pathUtf8.constData(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return success;
}

bool MarkedFolderDao::clearAllFolderRecords() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, "DELETE FROM marked_folders;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) {
            sqlite3_free(errMsg);
        }
        return false;
    }
    return true;
}

bool MarkedFolderDao::isMarked(const QString& folderPath) {
    if (folderPath.isEmpty()) return false;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    QString cleanPath = QDir::cleanPath(folderPath);
    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());
    const char* sql = "SELECT is_marked FROM marked_folders WHERE folder_path = ? LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;
    bool marked = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        QByteArray pathUtf8 = cleanPath.toUtf8();
        sqlite3_bind_text(stmt, 1, pathUtf8.constData(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            marked = (sqlite3_column_int(stmt, 0) == 1);
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return marked;
}

QStringList MarkedFolderDao::getAllMarkedFolders() {
    QStringList result;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return result;

    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());
    const char* sql = "SELECT folder_path FROM marked_folders ORDER BY updated_at DESC;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* txt = sqlite3_column_text(stmt, 0);
            if (txt) {
                result.append(QString::fromUtf8(reinterpret_cast<const char*>(txt)));
            }
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return result;
}

QStringList MarkedFolderDao::getActiveMarkedFolders() {
    QStringList result;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return result;

    std::lock_guard<std::mutex> lock(DatabaseManager::getGlobalMutex());
    const char* sql = "SELECT folder_path FROM marked_folders WHERE is_marked = 1 ORDER BY updated_at DESC;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* txt = sqlite3_column_text(stmt, 0);
            if (txt) {
                result.append(QString::fromUtf8(reinterpret_cast<const char*>(txt)));
            }
        }
    }
    if (stmt) sqlite3_finalize(stmt);
    return result;
}

} // namespace QuarkMeta
