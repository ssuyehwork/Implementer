#include "TrashRepository.h"
#include "DatabaseManager.h"
#include <sqlite3.h>

namespace QuarkMeta {

TrashRepository& TrashRepository::instance() {
    static TrashRepository inst;
    return inst;
}

TrashRepository::TrashRepository(QObject* parent)
    : QObject(parent) {
}

bool TrashRepository::getDiskTrashRecordByPath(const std::wstring& originalPath, int& outId, QString& outTrashPath) const {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    const char* sql = "SELECT id, trash_path FROM disk_trash WHERE original_path = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt, 1, originalPath.c_str(), -1, SQLITE_TRANSIENT);
        bool found = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            outId = sqlite3_column_int(stmt, 0);
            const wchar_t* wPath = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 1));
            if (wPath) outTrashPath = QString::fromWCharArray(wPath);
            found = true;
        }
        sqlite3_finalize(stmt);
        return found;
    }
    return false;
}

} // namespace QuarkMeta
