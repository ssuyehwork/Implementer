#include "HistoryDao.h"
#include "DatabaseManager.h"
#include <sqlite3.h>
#include <QDir>
#include <QFileInfo>

namespace QuarkMeta {

bool HistoryDao::initTable() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "CREATE TABLE IF NOT EXISTS file_history ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "file_path TEXT NOT NULL UNIQUE COLLATE NOCASE, "
                      "accessed_at DATETIME DEFAULT CURRENT_TIMESTAMP);";

    char* errMsgs = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsgs);
    if (rc != SQLITE_OK) {
        if (errMsgs) sqlite3_free(errMsgs);
        return false;
    }

    sqlite3_exec(db, "CREATE INDEX IF NOT EXISTS idx_history_accessed_at ON file_history(accessed_at DESC);", nullptr, nullptr, nullptr);
    return true;
}

bool HistoryDao::addOrUpdateHistory(const QString& rawPath) {
    if (rawPath.isEmpty()) return false;
    QString cleanPath = QDir::toNativeSeparators(QFileInfo(rawPath).absoluteFilePath());

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "INSERT INTO file_history (file_path, accessed_at) "
                      "VALUES (?, CURRENT_TIMESTAMP) "
                      "ON CONFLICT(file_path) DO UPDATE SET accessed_at = CURRENT_TIMESTAMP;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string pathStd = cleanPath.toStdString();
    sqlite3_bind_text(stmt, 1, pathStd.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    const char* cleanSql = "DELETE FROM file_history WHERE id NOT IN ("
                           "  SELECT id FROM file_history ORDER BY accessed_at DESC LIMIT 100"
                           ");";
    sqlite3_exec(db, cleanSql, nullptr, nullptr, nullptr);

    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

QList<HistoryRecord> HistoryDao::getAllHistory() {
    QList<HistoryRecord> list;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return list;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "SELECT id, file_path, accessed_at FROM file_history ORDER BY accessed_at DESC LIMIT 100;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        HistoryRecord rec;
        rec.id = sqlite3_column_int(stmt, 0);
        const char* pathStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* dateStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        if (pathStr) rec.filePath = QString::fromUtf8(pathStr);
        if (dateStr) rec.accessedAt = QDateTime::fromString(QString::fromUtf8(dateStr), Qt::ISODate);
        list.append(rec);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool HistoryDao::removeHistory(const QString& rawPath) {
    if (rawPath.isEmpty()) return false;
    QString cleanPath = QDir::toNativeSeparators(QFileInfo(rawPath).absoluteFilePath());

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "DELETE FROM file_history WHERE file_path = ? COLLATE NOCASE;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string pathStd = cleanPath.toStdString();
    sqlite3_bind_text(stmt, 1, pathStd.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool HistoryDao::clearAllHistory() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    char* errMsgs = nullptr;
    int rc = sqlite3_exec(db, "DELETE FROM file_history;", nullptr, nullptr, &errMsgs);
    if (rc != SQLITE_OK) {
        if (errMsgs) sqlite3_free(errMsgs);
        return false;
    }
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return true;
}

bool HistoryDao::updateHistoryPaths(const QStringList& oldPaths, const QStringList& newPaths) {
    if (oldPaths.size() != newPaths.size()) return false;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* updateSql = "UPDATE OR IGNORE file_history SET file_path = ?, accessed_at = CURRENT_TIMESTAMP WHERE file_path = ? COLLATE NOCASE;";
    const char* delSql = "DELETE FROM file_history WHERE file_path = ? COLLATE NOCASE;";

    sqlite3_stmt* stmtUp = nullptr;
    sqlite3_stmt* stmtDel = nullptr;

    if (sqlite3_prepare_v2(db, updateSql, -1, &stmtUp, nullptr) != SQLITE_OK) return false;
    if (sqlite3_prepare_v2(db, delSql, -1, &stmtDel, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmtUp);
        return false;
    }

    for (int i = 0; i < oldPaths.size(); ++i) {
        QString oldP = QDir::toNativeSeparators(QFileInfo(oldPaths[i]).absoluteFilePath());
        QString newP = QDir::toNativeSeparators(QFileInfo(newPaths[i]).absoluteFilePath());

        std::string oldStd = oldP.toStdString();
        std::string newStd = newP.toStdString();

        sqlite3_bind_text(stmtUp, 1, newStd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtUp, 2, oldStd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmtUp);
        sqlite3_reset(stmtUp);

        sqlite3_bind_text(stmtDel, 1, oldStd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmtDel);
        sqlite3_reset(stmtDel);
    }

    sqlite3_finalize(stmtUp);
    sqlite3_finalize(stmtDel);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return true;
}

} // namespace QuarkMeta
