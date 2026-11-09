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

bool FavoriteDao::addFavoritesAt(const QList<NewFavoriteItem>& items, int parentId, int beforeSiblingId) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || items.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    // a. BEGIN
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        return false;
    }

    // b. 读取全部行（id、parent_id、sort_order，按 sort_order 升序、id 升序），按父节点分组
    const char* selectSql = "SELECT id, parent_id, sort_order FROM favorites ORDER BY sort_order ASC, id ASC;";
    sqlite3_stmt* selectStmt = nullptr;
    if (sqlite3_prepare_v2(db, selectSql, -1, &selectStmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    QMap<int, QList<int>> parentToChildren;
    QMap<int, int> currentSortOrder;
    QMap<int, int> currentParentId;

    while (sqlite3_step(selectStmt) == SQLITE_ROW) {
        int rId = sqlite3_column_int(selectStmt, 0);
        int rParentId = sqlite3_column_int(selectStmt, 1);
        int rSortOrder = sqlite3_column_int(selectStmt, 2);

        parentToChildren[rParentId].append(rId);
        currentSortOrder[rId] = rSortOrder;
        currentParentId[rId] = rParentId;
    }
    sqlite3_finalize(selectStmt);

    // c. 逐个插入新行（名称规则、字段与原 addFavorite 一致；排序号先写 0），取得新编号
    const char* insertSql = "INSERT INTO favorites (parent_id, node_type, path, name, icon_key, color_hex, sort_order, created_at) "
                            "VALUES (?, 1, ?, ?, ?, ?, 0, ?);";
    sqlite3_stmt* insertStmt = nullptr;
    if (sqlite3_prepare_v2(db, insertSql, -1, &insertStmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    QList<int> newlyInsertedIds;
    qint64 nowSecs = QDateTime::currentSecsSinceEpoch();

    for (const auto& item : items) {
        QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(item.path));
        QFileInfo fi(cleanPath);
        QString name = item.name.isEmpty() ? (fi.fileName().isEmpty() ? cleanPath : fi.fileName()) : item.name;

        std::string pathStd = cleanPath.toStdString();
        std::string nameStd = name.toStdString();
        std::string iconStd = item.iconKey.toStdString();
        std::string colorStd = item.colorHex.toStdString();

        sqlite3_bind_int(insertStmt, 1, parentId);
        sqlite3_bind_text(insertStmt, 2, pathStd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 3, nameStd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 4, iconStd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 5, colorStd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(insertStmt, 6, nowSecs);

        if (sqlite3_step(insertStmt) == SQLITE_DONE) {
            int newId = static_cast<int>(sqlite3_last_insert_rowid(db));
            if (newId > 0) {
                newlyInsertedIds.append(newId);
                currentParentId[newId] = parentId;
                currentSortOrder[newId] = 0;
            }
        }
        sqlite3_reset(insertStmt);
    }
    sqlite3_finalize(insertStmt);

    if (newlyInsertedIds.isEmpty()) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    // d. 在 parentId 的子节点列表中，把新编号按给定顺序连续插入到 beforeSiblingId 之前；beforeSiblingId 为 0 或不在该父级下，则追加到末尾
    QList<int>& targetChildren = parentToChildren[parentId];
    int insertIndex = -1;
    if (beforeSiblingId > 0) {
        insertIndex = targetChildren.indexOf(beforeSiblingId);
    }

    if (insertIndex >= 0) {
        for (int i = 0; i < newlyInsertedIds.size(); ++i) {
            targetChildren.insert(insertIndex + i, newlyInsertedIds[i]);
        }
    } else {
        targetChildren.append(newlyInsertedIds);
    }

    // e. 从父节点 0 起做深度优先遍历，按遍历顺序重新赋值排序号（从 1 开始）；遍历不到的行不动
    int orderCounter = 1;
    QMap<int, int> newSortOrders;
    std::function<void(int)> dfs = [&](int currentPid) {
        const QList<int> children = parentToChildren.value(currentPid);
        for (int childId : children) {
            newSortOrders[childId] = orderCounter++;
            dfs(childId);
        }
    };
    dfs(0);

    // f. 只对 (parent_id, sort_order) 与原值不同的行，以及新行，执行一次预编译语句的更新，不得对未变化的行写库
    const char* updateSql = "UPDATE favorites SET parent_id = ?, sort_order = ? WHERE id = ?;";
    sqlite3_stmt* updateStmt = nullptr;
    if (sqlite3_prepare_v2(db, updateSql, -1, &updateStmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    for (auto it = newSortOrders.constBegin(); it != newSortOrders.constEnd(); ++it) {
        int id = it.key();
        int assignedOrder = it.value();
        bool isNew = newlyInsertedIds.contains(id);

        int origOrder = currentSortOrder.value(id, 0);
        int origParent = currentParentId.value(id, 0);
        int targetParent = (isNew ? parentId : origParent);

        if (isNew || origOrder != assignedOrder || origParent != targetParent) {
            sqlite3_bind_int(updateStmt, 1, targetParent);
            sqlite3_bind_int(updateStmt, 2, assignedOrder);
            sqlite3_bind_int(updateStmt, 3, id);
            sqlite3_step(updateStmt);
            sqlite3_reset(updateStmt);
        }
    }
    sqlite3_finalize(updateStmt);

    // g. COMMIT，之后只做一次 wal_checkpoint
    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return true;
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
