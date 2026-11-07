#include "FavoriteDao.h"
#include "DatabaseManager.h"
#include <sqlite3.h>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QRandomGenerator>

namespace QuarkMeta {

bool FavoriteDao::initTable() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "CREATE TABLE IF NOT EXISTS favorites ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "parent_id INTEGER DEFAULT 0, "
                      "node_type INTEGER DEFAULT 1, "
                      "path TEXT UNIQUE NOT NULL, "
                      "name TEXT, "
                      "icon_key TEXT DEFAULT 'folder_filled', "
                      "color_hex TEXT DEFAULT '#888888', "
                      "sort_order INTEGER DEFAULT 0, "
                      "created_at INTEGER);";

    char* errMsgs = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsgs);
    if (rc != SQLITE_OK) {
        if (errMsgs) sqlite3_free(errMsgs);
        return false;
    }

    // 数据库增量平滑迁移 (Add parent_id, node_type & preset_tags columns if upgrading from old schema)
    sqlite3_exec(db, "ALTER TABLE favorites ADD COLUMN parent_id INTEGER DEFAULT 0;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "ALTER TABLE favorites ADD COLUMN node_type INTEGER DEFAULT 1;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "ALTER TABLE favorites ADD COLUMN preset_tags TEXT DEFAULT '';", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "ALTER TABLE favorites ADD COLUMN is_auto_recursive INTEGER DEFAULT 0;", nullptr, nullptr, nullptr);

    return true;
}

QList<FavoriteRecord> FavoriteDao::getAllFavorites() {
    QList<FavoriteRecord> list;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return list;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "SELECT id, parent_id, node_type, path, name, icon_key, color_hex, sort_order, preset_tags, is_auto_recursive FROM favorites ORDER BY sort_order ASC, id ASC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        FavoriteRecord rec;
        rec.id = sqlite3_column_int(stmt, 0);
        rec.parentId = sqlite3_column_int(stmt, 1);
        rec.nodeType = static_cast<FavoriteNodeType>(sqlite3_column_int(stmt, 2));
        const char* pathStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* nameStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const char* iconStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        const char* colorStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        rec.sortOrder = sqlite3_column_int(stmt, 7);
        const char* tagsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        rec.isAutoRecursive = (sqlite3_column_int(stmt, 9) != 0);

        if (pathStr) rec.path = QString::fromUtf8(pathStr);
        if (nameStr) rec.name = QString::fromUtf8(nameStr);
        if (iconStr) rec.iconKey = QString::fromUtf8(iconStr);
        if (colorStr) rec.colorHex = QString::fromUtf8(colorStr);
        if (tagsStr && strlen(tagsStr) > 0) {
            rec.presetTags = QString::fromUtf8(tagsStr).split(',', Qt::SkipEmptyParts);
        }

        list.append(rec);
    }
    sqlite3_finalize(stmt);
    return list;
}

int FavoriteDao::addVirtualCategory(const QString& name, int parentId, const QString& iconKey, const QString& colorHex) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return 0;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    quint32 randVal = QRandomGenerator::global()->generate() % 1000;
    QString virtPath = QString("virtual_cat_%1_%2").arg(static_cast<qlonglong>(QDateTime::currentMSecsSinceEpoch())).arg(randVal);
    const char* sql = "INSERT INTO favorites (parent_id, node_type, path, name, icon_key, color_hex, sort_order, created_at) "
                      "VALUES (?, 0, ?, ?, ?, ?, (SELECT COALESCE(MAX(sort_order), 0) + 1 FROM favorites), ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;

    std::string pathStd = virtPath.toStdString();
    std::string nameStd = name.toStdString();
    std::string iconStd = iconKey.toStdString();
    std::string colorStd = colorHex.toStdString();

    sqlite3_bind_int(stmt, 1, parentId);
    sqlite3_bind_text(stmt, 2, pathStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, nameStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, iconStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, colorStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 6, QDateTime::currentSecsSinceEpoch());

    int newId = 0;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        newId = static_cast<int>(sqlite3_last_insert_rowid(db));
    }
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return newId;
}

bool FavoriteDao::addFavorite(const QString& path, int parentId, const QString& iconKey, const QString& colorHex) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(path));
    QFileInfo fi(cleanPath);
    QString name = fi.fileName().isEmpty() ? cleanPath : fi.fileName();

    const char* sql = "INSERT INTO favorites (parent_id, node_type, path, name, icon_key, color_hex, sort_order, created_at) "
                      "VALUES (?, 1, ?, ?, ?, ?, (SELECT COALESCE(MAX(sort_order), 0) + 1 FROM favorites), ?)"
                      "ON CONFLICT(path) DO UPDATE SET parent_id = excluded.parent_id;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string pathStd = cleanPath.toStdString();
    std::string nameStd = name.toStdString();
    std::string iconStd = iconKey.toStdString();
    std::string colorStd = colorHex.toStdString();

    sqlite3_bind_int(stmt, 1, parentId);
    sqlite3_bind_text(stmt, 2, pathStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, nameStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, iconStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, colorStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 6, QDateTime::currentSecsSinceEpoch());

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool FavoriteDao::updateAutoRecursive(int id, bool autoRecursive) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "UPDATE favorites SET is_auto_recursive = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, autoRecursive ? 1 : 0);
    sqlite3_bind_int(stmt, 2, id);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool FavoriteDao::updatePresetTags(int id, const QStringList& tags) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "UPDATE favorites SET preset_tags = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string tagsStd = tags.join(",").toStdString();
    sqlite3_bind_text(stmt, 1, tagsStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, id);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool FavoriteDao::updateFavoriteNode(int id, const QString& name, const QString& iconKey, const QString& colorHex) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "UPDATE favorites SET name = ?, icon_key = ?, color_hex = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string nameStd = name.toStdString();
    std::string iconStd = iconKey.toStdString();
    std::string colorStd = colorHex.toStdString();

    sqlite3_bind_text(stmt, 1, nameStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, iconStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, colorStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, id);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool FavoriteDao::updateNodeParentAndOrder(int id, int newParentId, int sortOrder) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "UPDATE favorites SET parent_id = ?, sort_order = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, newParentId);
    sqlite3_bind_int(stmt, 2, sortOrder);
    sqlite3_bind_int(stmt, 3, id);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool FavoriteDao::updateNodesParentAndOrder(const QList<NodeOrderEntry>& entries) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || entries.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    const char* sql = "UPDATE favorites SET parent_id = ?, sort_order = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    for (const auto& entry : entries) {
        if (entry.id <= 0) continue;
        sqlite3_bind_int(stmt, 1, entry.parentId);
        sqlite3_bind_int(stmt, 2, entry.sortOrder);
        sqlite3_bind_int(stmt, 3, entry.id);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return true;
}

bool FavoriteDao::removeFavoriteById(int id) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "WITH RECURSIVE cnt(x) AS ("
                      "  SELECT ? UNION ALL SELECT id FROM favorites, cnt WHERE favorites.parent_id = cnt.x"
                      ") DELETE FROM favorites WHERE id IN (SELECT x FROM cnt);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, id);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool FavoriteDao::removeFavorite(const QString& path) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(path));
    const char* sql = "DELETE FROM favorites WHERE path = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string pathStd = cleanPath.toStdString();
    sqlite3_bind_text(stmt, 1, pathStd.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool FavoriteDao::updateFavorite(const QString& path, const QString& iconKey, const QString& colorHex) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(path));
    const char* sql = "UPDATE favorites SET icon_key = ?, color_hex = ? WHERE path = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string pathStd = cleanPath.toStdString();
    std::string iconStd = iconKey.toStdString();
    std::string colorStd = colorHex.toStdString();

    sqlite3_bind_text(stmt, 1, iconStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, colorStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pathStd.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool FavoriteDao::containsPath(const QString& path) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(path));
    const char* sql = "SELECT COUNT(*) FROM favorites WHERE path = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string pathStd = cleanPath.toStdString();
    sqlite3_bind_text(stmt, 1, pathStd.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = (sqlite3_column_int(stmt, 0) > 0);
    }
    sqlite3_finalize(stmt);
    return exists;
}

bool FavoriteDao::updateSortOrders(const QList<QPair<int, int>>& orders) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "UPDATE favorites SET sort_order = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    for (const auto& pair : orders) {
        sqlite3_bind_int(stmt, 1, pair.second);
        sqlite3_bind_int(stmt, 2, pair.first);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return true;
}

} // namespace QuarkMeta
