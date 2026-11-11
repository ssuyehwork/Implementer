#include "LibraryDao.h"
#include "../core/DatabaseManager.h"
#include <sqlite3.h>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QRandomGenerator>

namespace RapidNotes {

bool LibraryDao::initTable() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) {
        qWarning() << "[LibraryDao] 数据库句柄不可用，安全跳过 initTable。";
        return false;
    }

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sqlCat = "CREATE TABLE IF NOT EXISTS library_categories ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "parent_id INTEGER DEFAULT 0, "
                         "name TEXT NOT NULL, "
                         "icon_key TEXT DEFAULT 'Data-Classification.svg', "
                         "color_hex TEXT DEFAULT '#888888', "
                         "sort_order INTEGER DEFAULT 0, "
                         "preset_tags TEXT DEFAULT '', "
                         "created_at INTEGER);";

    const char* sqlPaths = "CREATE TABLE IF NOT EXISTS library_category_paths ("
                           "category_id INTEGER, "
                           "path TEXT NOT NULL COLLATE NOCASE, "
                           "PRIMARY KEY(category_id, path));";

    const char* sqlPathsIdx = "CREATE INDEX IF NOT EXISTS idx_category_paths_path ON library_category_paths(path COLLATE NOCASE);";

    char* errMsgs = nullptr;
    sqlite3_exec(db, sqlCat, nullptr, nullptr, &errMsgs);
    sqlite3_exec(db, sqlPaths, nullptr, nullptr, &errMsgs);
    sqlite3_exec(db, sqlPathsIdx, nullptr, nullptr, &errMsgs);
    sqlite3_exec(db, "CREATE INDEX IF NOT EXISTS idx_cat_paths_nocase ON library_category_paths(path COLLATE NOCASE);", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "ALTER TABLE library_categories ADD COLUMN preset_tags TEXT DEFAULT '';", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "ALTER TABLE library_categories ADD COLUMN is_pinned INTEGER DEFAULT 0;", nullptr, nullptr, nullptr);
    return true;
}

QList<LibraryCategoryRecord> LibraryDao::getAllCategories() {
    QList<LibraryCategoryRecord> list;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return list;

    {
        std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

        const char* sql = "SELECT id, parent_id, name, icon_key, color_hex, sort_order, preset_tags, is_pinned FROM library_categories ORDER BY sort_order ASC, id ASC;";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            LibraryCategoryRecord rec;
            rec.id = sqlite3_column_int(stmt, 0);
            rec.parentId = sqlite3_column_int(stmt, 1);
            const char* nameStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* iconStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            const char* colorStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            rec.sortOrder = sqlite3_column_int(stmt, 5);
            const char* tagsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            rec.isPinned = (sqlite3_column_int(stmt, 7) != 0);

            if (nameStr) rec.name = QString::fromUtf8(nameStr);
            if (iconStr) rec.iconKey = QString::fromUtf8(iconStr);
            if (colorStr) rec.colorHex = QString::fromUtf8(colorStr);
            if (tagsStr && strlen(tagsStr) > 0) {
                rec.presetTags = QString::fromUtf8(tagsStr).split(',', Qt::SkipEmptyParts);
            }

            list.append(rec);
        }
        sqlite3_finalize(stmt);
    }

    return list;
}

LibraryCategoryRecord LibraryDao::getCategoryById(int id) {
    LibraryCategoryRecord rec;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return rec;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    const char* sql = "SELECT id, parent_id, name, icon_key, color_hex, sort_order, preset_tags, is_pinned FROM library_categories WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            rec.id = sqlite3_column_int(stmt, 0);
            rec.parentId = sqlite3_column_int(stmt, 1);
            const char* nameStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* iconStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            const char* colorStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            rec.sortOrder = sqlite3_column_int(stmt, 5);
            const char* tagsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            rec.isPinned = (sqlite3_column_int(stmt, 7) != 0);

            if (nameStr) rec.name = QString::fromUtf8(nameStr);
            if (iconStr) rec.iconKey = QString::fromUtf8(iconStr);
            if (colorStr) rec.colorHex = QString::fromUtf8(colorStr);
            if (tagsStr && strlen(tagsStr) > 0) {
                rec.presetTags = QString::fromUtf8(tagsStr).split(',', Qt::SkipEmptyParts);
            }
        }
        sqlite3_finalize(stmt);
    }
    return rec;
}

QStringList LibraryDao::getPresetTagsWithAncestors(int id) {
    QStringList resultTags;
    if (id <= 0) return resultTags;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return resultTags;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "WITH RECURSIVE cat_ancestors(id, parent_id, preset_tags) AS ("
                      "  SELECT id, parent_id, preset_tags FROM library_categories WHERE id = ?"
                      "  UNION ALL "
                      "  SELECT c.id, c.parent_id, c.preset_tags FROM library_categories c "
                      "  JOIN cat_ancestors a ON c.id = a.parent_id"
                      ") SELECT preset_tags FROM cat_ancestors;";

    sqlite3_stmt* stmt = nullptr;
    QSet<QString> seenLower;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* tagsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            if (tagsStr && strlen(tagsStr) > 0) {
                QStringList rawList = QString::fromUtf8(tagsStr).split(',', Qt::SkipEmptyParts);
                for (const QString& tag : rawList) {
                    QString clean = tag.trimmed();
                    if (clean.isEmpty()) continue;
                    QString lower = clean.toLower();
                    if (!seenLower.contains(lower)) {
                        seenLower.insert(lower);
                        resultTags.append(clean);
                    }
                }
            }
        }
        sqlite3_finalize(stmt);
    }

    return resultTags;
}

int LibraryDao::addCategory(const QString& name, int parentId, const QString& iconKey, const QString& colorHex) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return 0;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "INSERT INTO library_categories (parent_id, name, icon_key, color_hex, sort_order, created_at) "
                      "VALUES (?, ?, ?, ?, (SELECT COALESCE(MAX(sort_order), 0) + 1 FROM library_categories), ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;

    std::string nameStd = name.toStdString();
    std::string iconStd = iconKey.toStdString();
    std::string colorStd = colorHex.toStdString();

    sqlite3_bind_int(stmt, 1, parentId);
    sqlite3_bind_text(stmt, 2, nameStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, iconStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, colorStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 5, QDateTime::currentSecsSinceEpoch());

    int newId = 0;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        newId = static_cast<int>(sqlite3_last_insert_rowid(db));
    }
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return newId;
}

bool LibraryDao::setCategoryPinned(int id, bool pinned) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "UPDATE library_categories SET is_pinned = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, pinned ? 1 : 0);
    sqlite3_bind_int(stmt, 2, id);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool LibraryDao::updatePathPrefix(const QString& oldPrefix, const QString& newPrefix) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || oldPrefix.isEmpty() || newPrefix.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    QString cleanOld = QDir::toNativeSeparators(oldPrefix);
    QString cleanNew = QDir::toNativeSeparators(newPrefix);

    const char* sql = "UPDATE library_category_paths SET path = ? || SUBSTR(path, ?) WHERE path LIKE ? || '%' COLLATE NOCASE;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, cleanNew.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, cleanOld.length() + 1);
        sqlite3_bind_text(stmt, 3, cleanOld.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return ok;
    }
    return false;
}

bool LibraryDao::updateCategoryNode(int id, const QString& name, const QString& iconKey, const QString& colorHex) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "UPDATE library_categories SET name = ?, icon_key = ?, color_hex = ? WHERE id = ?;";
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

bool LibraryDao::updateNodeParentAndOrder(int id, int newParentId, int sortOrder) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "UPDATE library_categories SET parent_id = ?, sort_order = ? WHERE id = ?;";
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

bool LibraryDao::updateNodesParentAndOrder(const QList<NodeOrderEntry>& entries) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || entries.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    const char* sql = "UPDATE library_categories SET parent_id = ?, sort_order = ? WHERE id = ?;";
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

bool LibraryDao::removePathsFromLibrary(const QStringList& paths) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || paths.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    SqlTransaction trans(db);

    // 1. 从分类关系表中移除 (带 COLLATE NOCASE 确保大小写不敏感匹配)
    const char* sql = "DELETE FROM library_category_paths WHERE path = ? COLLATE NOCASE;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    for (const QString& p : paths) {
        std::string pStd = QDir::toNativeSeparators(QDir::cleanPath(p)).toStdString();
        sqlite3_bind_text(stmt, 1, pStd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);

    // 2. 在同个事务中级联清理实体索引表 library_items，杜绝异步断层和幽灵复活
    const char* sqlItems = "DELETE FROM library_items WHERE file_path = ? COLLATE NOCASE;";
    sqlite3_stmt* stmtItems = nullptr;
    if (sqlite3_prepare_v2(db, sqlItems, -1, &stmtItems, nullptr) == SQLITE_OK) {
        for (const QString& p : paths) {
            std::string pStd = QDir::toNativeSeparators(QDir::cleanPath(p)).toStdString();
            sqlite3_bind_text(stmtItems, 1, pStd.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmtItems);
            sqlite3_reset(stmtItems);
        }
        sqlite3_finalize(stmtItems);
    }

    bool ok = trans.commit();
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return ok;
}

bool LibraryDao::removeCategoryById(int id) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "WITH RECURSIVE cnt(x) AS ("
                      "  SELECT ? UNION ALL SELECT id FROM library_categories, cnt WHERE library_categories.parent_id = cnt.x"
                      ") DELETE FROM library_categories WHERE id IN (SELECT x FROM cnt);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, id);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    const char* sqlCleanPaths = "DELETE FROM library_category_paths WHERE category_id IN ("
                                "  WITH RECURSIVE cnt(x) AS ("
                                "    SELECT ? UNION ALL SELECT id FROM library_categories, cnt WHERE library_categories.parent_id = cnt.x"
                                "  ) SELECT x FROM cnt"
                                ");";
    if (sqlite3_prepare_v2(db, sqlCleanPaths, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

bool LibraryDao::addPathsToCategory(int id, const QStringList& paths) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id == 0 || paths.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    SqlTransaction trans(db);

    // 1. 先清除被操作路径已有的所有旧分类关联 (强制忽略大小写，彻底确保删除旧关联 100% 命中)
    const char* delPathsSql = "DELETE FROM library_category_paths WHERE path = ? COLLATE NOCASE;";
    sqlite3_stmt* stmtDelP = nullptr;
    if (sqlite3_prepare_v2(db, delPathsSql, -1, &stmtDelP, nullptr) == SQLITE_OK) {
        for (const QString& p : paths) {
            std::string pStd = QDir::toNativeSeparators(QDir::cleanPath(p)).toStdString();
            sqlite3_bind_text(stmtDelP, 1, pStd.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmtDelP);
            sqlite3_reset(stmtDelP);
        }
        sqlite3_finalize(stmtDelP);
    }

    // 2. 当 id > 0 或 id == -2 时，都向 library_category_paths 写入一行（category_id 取传入的 id，-2 表示未分类）
    if (id > 0 || id == -2) {
        const char* sql = "INSERT OR IGNORE INTO library_category_paths (category_id, path) VALUES (?, ?);";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            for (const QString& p : paths) {
                std::string pStd = QDir::toNativeSeparators(QDir::cleanPath(p)).toStdString();
                sqlite3_bind_int(stmt, 1, id);
                sqlite3_bind_text(stmt, 2, pStd.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(stmt);
                sqlite3_reset(stmt);
            }
            sqlite3_finalize(stmt);
        }
    }

    bool ok = trans.commit();
    if (ok) {
        sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    }
    return ok;
}

bool LibraryDao::removePathsFromCategory(int id, const QStringList& paths) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0 || paths.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "DELETE FROM library_category_paths WHERE category_id = ? AND path = ? COLLATE NOCASE;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    for (const QString& p : paths) {
        QString cleanP = QDir::toNativeSeparators(QDir::cleanPath(p));
        std::string pStd = cleanP.toStdString();
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_bind_text(stmt, 2, pStd.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return true;
}

bool LibraryDao::updatePresetTags(int id, const QStringList& tags) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id <= 0) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "UPDATE library_categories SET preset_tags = ? WHERE id = ?;";
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

QStringList LibraryDao::getCategoryPaths(int id) {
    QStringList rawPaths;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || id == 0) return rawPaths;

    {
        std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

        if (id == -1) {
            const char* sql = "SELECT DISTINCT path FROM library_category_paths;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const char* pStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    if (pStr) rawPaths.append(QString::fromUtf8(pStr));
                }
                sqlite3_finalize(stmt);
            }
        } else if (id == -2) {
            const char* sql = "SELECT DISTINCT path FROM library_category_paths WHERE category_id <= 0;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const char* pStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    if (pStr) rawPaths.append(QString::fromUtf8(pStr));
                }
                sqlite3_finalize(stmt);
            }
        } else {
            const char* sql = "WITH RECURSIVE cat_tree(x) AS ("
                              "  SELECT ? UNION ALL SELECT id FROM library_categories, cat_tree WHERE library_categories.parent_id = cat_tree.x"
                              ") SELECT DISTINCT path FROM library_category_paths WHERE category_id IN (SELECT x FROM cat_tree);";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, id);
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const char* pStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    if (pStr) rawPaths.append(QString::fromUtf8(pStr));
                }
                sqlite3_finalize(stmt);
            }
        }
    }

    return rawPaths;
}

} // namespace RapidNotes
