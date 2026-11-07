#include "DriveMetaDao.h"
#include "DatabaseManager.h"
#include "MetadataManager.h"
#include "sqlite3.h"
#include <QDateTime>

namespace QuarkMeta {

bool DriveMetaDao::initTable() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql =
        "CREATE TABLE IF NOT EXISTS drive_metadata ("
        "  drive_path TEXT PRIMARY KEY,"
        "  rating INTEGER DEFAULT 0,"
        "  color TEXT DEFAULT '',"
        "  pinned INTEGER DEFAULT 0,"
        "  note TEXT DEFAULT '',"
        "  url TEXT DEFAULT '',"
        "  updated_at INTEGER DEFAULT 0"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

std::unordered_map<std::wstring, DriveMetaRecord> DriveMetaDao::getAllDriveMeta() {
    std::unordered_map<std::wstring, DriveMetaRecord> result;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return result;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "SELECT drive_path, rating, color, pinned, note, url FROM drive_metadata;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            DriveMetaRecord rec;
            const wchar_t* pPath = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 0));
            if (pPath) rec.drivePath = MetadataManager::normalizePath(pPath);
            rec.rating = sqlite3_column_int(stmt, 1);
            const wchar_t* pColor = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 2));
            if (pColor) rec.color = pColor;
            rec.pinned = (sqlite3_column_int(stmt, 3) != 0);
            const wchar_t* pNote = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 4));
            if (pNote) rec.note = pNote;
            const wchar_t* pUrl = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 5));
            if (pUrl) rec.url = pUrl;

            result[rec.drivePath] = rec;
        }
        sqlite3_finalize(stmt);
    }
    return result;
}

DriveMetaRecord DriveMetaDao::getDriveMeta(const std::wstring& drivePath) {
    std::wstring normPath = MetadataManager::normalizePath(drivePath);
    auto all = getAllDriveMeta();
    auto it = all.find(normPath);
    if (it != all.end()) return it->second;
    DriveMetaRecord defaultRec;
    defaultRec.drivePath = normPath;
    return defaultRec;
}

bool DriveMetaDao::saveDriveMeta(const DriveMetaRecord& record) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    std::wstring normPath = MetadataManager::normalizePath(record.drivePath);

    const char* sql =
        "INSERT INTO drive_metadata (drive_path, rating, color, pinned, note, url, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?) "
        "ON CONFLICT(drive_path) DO UPDATE SET "
        "  rating=excluded.rating, "
        "  color=excluded.color, "
        "  pinned=excluded.pinned, "
        "  note=excluded.note, "
        "  url=excluded.url, "
        "  updated_at=excluded.updated_at;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    sqlite3_bind_text16(stmt, 1, normPath.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, record.rating);
    sqlite3_bind_text16(stmt, 3, record.color.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, record.pinned ? 1 : 0);
    sqlite3_bind_text16(stmt, 5, record.note.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(stmt, 6, record.url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 7, now);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        // 🚨 核心保障：写完立即触发 PASSIVE 检查点，确保持久化写入 inspiration.db 主文件！
        sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
        return true;
    }
    return false;
}

} // namespace QuarkMeta