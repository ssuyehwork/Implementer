#pragma once
#include "sqlite3.h"
#include "../util/VolumePathResolver.h"

namespace QuarkMeta {

class DatabaseMigrator {
public:
    static bool ensureActivated(sqlite3* db) {
        const char* sqlCreateMetadata =
            "CREATE TABLE IF NOT EXISTS metadata ("
            "  folder_id TEXT PRIMARY KEY, "
            "  path TEXT UNIQUE, "
            "  rating INTEGER, "
            "  color TEXT, "
            "  pinned INTEGER"
            ");";
        return sqlite3_exec(db, sqlCreateMetadata, nullptr, nullptr, nullptr) == SQLITE_OK;
    }
};

} // namespace QuarkMeta
