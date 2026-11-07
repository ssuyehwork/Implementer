#include "LibraryItemDao.h"
#include "DatabaseManager.h"
#include "MetadataManager.h"
#include <QFileInfo>
#include <QDir>
#include <QUuid>
#include <QDateTime>
#include <QDebug>

namespace QuarkMeta {

std::vector<ItemRecord> LibraryItemDao::getItemsByPaths(const QStringList& paths) {
    std::vector<ItemRecord> result;
    if (paths.isEmpty()) return result;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return result;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    constexpr int kBatchSize = 500;
    for (int i = 0; i < paths.size(); i += kBatchSize) {
        int count = qMin(kBatchSize, paths.size() - i);
        QStringList chunkPaths;
        chunkPaths.reserve(count);
        for (int j = 0; j < count; ++j) {
            chunkPaths.append(QDir::toNativeSeparators(paths[i + j]));
        }

        // 1. 构建主表 IN (?, ?, ...) 查询
        QStringList placeholders;
        placeholders.reserve(count);
        for (int j = 0; j < count; ++j) {
            placeholders.append("?");
        }

        QString sql = QString("SELECT file_id, meta_file_id, file_path, file_name, base_name, extension, file_size, is_dir, is_empty, "
                              "is_hidden, is_managed, encrypted, pinned, rating, color_hex, auto_color, width, height, "
                              "thumb_status, note, url, sha256, created_at, updated_at, added_at "
                              "FROM library_items WHERE file_path IN (%1) COLLATE NOCASE;")
                      .arg(placeholders.join(','));

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.toUtf8().constData(), -1, &stmt, nullptr) != SQLITE_OK) {
            DatabaseManager::logSqlError("LibraryItemDao::getItemsByPaths prepare main", db, sql);
            continue;
        }

        for (int j = 0; j < count; ++j) {
            sqlite3_bind_text(stmt, j + 1, chunkPaths[j].toUtf8().constData(), -1, SQLITE_TRANSIENT);
        }

        QMap<QString, ItemRecord> chunkItemMap;
        QStringList chunkFileIds;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ItemRecord rec;
            rec.fileId = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            rec.metaFileId = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            rec.path = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            rec.filename = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            rec.suffix = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
            rec.size = sqlite3_column_int64(stmt, 6);
            rec.isDir = (sqlite3_column_int(stmt, 7) != 0);
            rec.isEmpty = (sqlite3_column_int(stmt, 8) != 0);
            rec.isHidden = (sqlite3_column_int(stmt, 9) != 0);
            rec.isManaged = (sqlite3_column_int(stmt, 10) != 0);
            rec.encrypted = (sqlite3_column_int(stmt, 11) != 0);
            rec.pinned = (sqlite3_column_int(stmt, 12) != 0);
            rec.rating = sqlite3_column_int(stmt, 13);
            rec.manualColor = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 14)));
            rec.autoColor = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 15)));
            rec.width = sqlite3_column_int(stmt, 16);
            rec.height = sqlite3_column_int(stmt, 17);
            rec.thumbStatus = sqlite3_column_int(stmt, 18);
            rec.note = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 19)));
            rec.url = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 20)));
            rec.sha256 = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 21)));
            rec.ctime = sqlite3_column_int64(stmt, 22);
            rec.mtime = sqlite3_column_int64(stmt, 23);
            rec.added_at = sqlite3_column_int64(stmt, 24);

            if (!rec.fileId.isEmpty()) {
                chunkItemMap.insert(rec.fileId, rec);
                chunkFileIds.append(rec.fileId);
            }
        }
        sqlite3_finalize(stmt);

        if (chunkFileIds.isEmpty()) {
            continue;
        }

        // 2. 构建 file_tags 动态 IN 查询
        QStringList idPlaceholders;
        idPlaceholders.reserve(chunkFileIds.size());
        for (int j = 0; j < chunkFileIds.size(); ++j) {
            idPlaceholders.append("?");
        }

        QString tagSql = QString("SELECT file_id, tag_name FROM file_tags WHERE file_id IN (%1);")
                         .arg(idPlaceholders.join(','));

        if (sqlite3_prepare_v2(db, tagSql.toUtf8().constData(), -1, &stmt, nullptr) == SQLITE_OK) {
            for (int j = 0; j < chunkFileIds.size(); ++j) {
                sqlite3_bind_text(stmt, j + 1, chunkFileIds[j].toUtf8().constData(), -1, SQLITE_TRANSIENT);
            }
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                QString fileId = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
                QString tagName = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                if (chunkItemMap.contains(fileId)) {
                    chunkItemMap[fileId].tags.append(tagName);
                }
            }
            sqlite3_finalize(stmt);
        } else {
            DatabaseManager::logSqlError("LibraryItemDao::getItemsByPaths prepare tags", db, tagSql);
        }

        // 3. 构建 item_palettes 动态 IN 查询
        QString palSql = QString("SELECT file_id, color_hex, ratio FROM item_palettes WHERE file_id IN (%1) ORDER BY idx ASC;")
                         .arg(idPlaceholders.join(','));

        if (sqlite3_prepare_v2(db, palSql.toUtf8().constData(), -1, &stmt, nullptr) == SQLITE_OK) {
            for (int j = 0; j < chunkFileIds.size(); ++j) {
                sqlite3_bind_text(stmt, j + 1, chunkFileIds[j].toUtf8().constData(), -1, SQLITE_TRANSIENT);
            }
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                QString fileId = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
                QString colorHex = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                float ratio = static_cast<float>(sqlite3_column_double(stmt, 2));
                if (chunkItemMap.contains(fileId)) {
                    chunkItemMap[fileId].palettes.push_back({QColor(colorHex), ratio});
                }
            }
            sqlite3_finalize(stmt);
        } else {
            DatabaseManager::logSqlError("LibraryItemDao::getItemsByPaths prepare palettes", db, palSql);
        }

        for (const auto& item : chunkItemMap) {
            result.push_back(item);
        }
    }

    return result;
}

QStringList LibraryItemDao::getAllLibraryPaths() {
    QStringList paths;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return paths;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    const char* sql = "SELECT file_path FROM library_items;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            paths.append(QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
        }
        sqlite3_finalize(stmt);
    } else {
        DatabaseManager::logSqlError("LibraryItemDao::getAllLibraryPaths prepare", db, sql);
    }
    return paths;
}

std::vector<ItemRecord> LibraryItemDao::getAllLibraryItems() {
    QStringList paths = getAllLibraryPaths();
    return getItemsByPaths(paths);
}

bool LibraryItemDao::upsertItem(const ItemRecord& record) {
    return upsertItems({record});
}

bool LibraryItemDao::upsertItems(const std::vector<ItemRecord>& records) {
    if (records.empty()) return true;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    SqlTransaction tx(db);

    const char* queryIdSql = "SELECT file_id FROM library_items WHERE file_path = ? COLLATE NOCASE;";
    const char* upsertSql = "INSERT INTO library_items ("
                            "file_id, meta_file_id, file_path, file_name, base_name, extension, file_size, is_dir, is_empty, "
                            "is_hidden, is_managed, encrypted, pinned, rating, color_hex, auto_color, width, height, "
                            "thumb_status, note, url, sha256, created_at, updated_at, added_at, sync_state, synced_at) "
                            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0, ?) "
                            "ON CONFLICT(file_id) DO UPDATE SET "
                            "meta_file_id=excluded.meta_file_id, file_path=excluded.file_path, file_name=excluded.file_name, "
                            "base_name=excluded.base_name, extension=excluded.extension, file_size=excluded.file_size, "
                            "is_dir=excluded.is_dir, is_empty=excluded.is_empty, is_hidden=excluded.is_hidden, "
                            "is_managed=excluded.is_managed, encrypted=excluded.encrypted, pinned=excluded.pinned, "
                            "rating=excluded.rating, color_hex=excluded.color_hex, auto_color=excluded.auto_color, "
                            "width=excluded.width, height=excluded.height, thumb_status=excluded.thumb_status, "
                            "note=excluded.note, url=excluded.url, sha256=excluded.sha256, created_at=excluded.created_at, "
                            "updated_at=excluded.updated_at, added_at=excluded.added_at, sync_state=0, synced_at=excluded.synced_at;";

    const char* delTagsSql = "DELETE FROM file_tags WHERE file_id = ?;";
    const char* tagDictSql = "INSERT OR IGNORE INTO tags(name) VALUES(?);";
    const char* insTagSql = "INSERT OR REPLACE INTO file_tags(file_id, tag_name) VALUES(?, ?);";

    const char* delPalSql = "DELETE FROM item_palettes WHERE file_id = ?;";
    const char* insPalSql = "INSERT INTO item_palettes(file_id, idx, color_hex, ratio) VALUES(?, ?, ?, ?);";

    sqlite3_stmt* stmtQueryId = nullptr;
    sqlite3_stmt* stmtUpsert = nullptr;
    sqlite3_stmt* stmtDelTags = nullptr;
    sqlite3_stmt* stmtTagDict = nullptr;
    sqlite3_stmt* stmtInsTag = nullptr;
    sqlite3_stmt* stmtDelPal = nullptr;
    sqlite3_stmt* stmtInsPal = nullptr;

    auto cleanupStmts = [&]() {
        if (stmtQueryId) sqlite3_finalize(stmtQueryId);
        if (stmtUpsert) sqlite3_finalize(stmtUpsert);
        if (stmtDelTags) sqlite3_finalize(stmtDelTags);
        if (stmtTagDict) sqlite3_finalize(stmtTagDict);
        if (stmtInsTag) sqlite3_finalize(stmtInsTag);
        if (stmtDelPal) sqlite3_finalize(stmtDelPal);
        if (stmtInsPal) sqlite3_finalize(stmtInsPal);
    };

    if (sqlite3_prepare_v2(db, queryIdSql, -1, &stmtQueryId, nullptr) != SQLITE_OK ||
        sqlite3_prepare_v2(db, upsertSql, -1, &stmtUpsert, nullptr) != SQLITE_OK ||
        sqlite3_prepare_v2(db, delTagsSql, -1, &stmtDelTags, nullptr) != SQLITE_OK ||
        sqlite3_prepare_v2(db, tagDictSql, -1, &stmtTagDict, nullptr) != SQLITE_OK ||
        sqlite3_prepare_v2(db, insTagSql, -1, &stmtInsTag, nullptr) != SQLITE_OK ||
        sqlite3_prepare_v2(db, delPalSql, -1, &stmtDelPal, nullptr) != SQLITE_OK ||
        sqlite3_prepare_v2(db, insPalSql, -1, &stmtInsPal, nullptr) != SQLITE_OK) {
        DatabaseManager::logSqlError("LibraryItemDao::upsertItems prepare", db, upsertSql);
        cleanupStmts();
        return false;
    }

    long long now = QDateTime::currentSecsSinceEpoch();

    for (const auto& record : records) {
        QString fileId = record.fileId;
        if (fileId.isEmpty()) {
            sqlite3_reset(stmtQueryId);
            sqlite3_clear_bindings(stmtQueryId);
            sqlite3_bind_text(stmtQueryId, 1, record.path.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmtQueryId) == SQLITE_ROW) {
                fileId = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmtQueryId, 0)));
            }
        }
        if (fileId.isEmpty()) {
            fileId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        }

        QFileInfo info(record.path);
        QString baseName = info.completeBaseName();
        QString ext = info.suffix();

        sqlite3_reset(stmtUpsert);
        sqlite3_clear_bindings(stmtUpsert);

        sqlite3_bind_text(stmtUpsert, 1, fileId.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtUpsert, 2, record.metaFileId.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtUpsert, 3, record.path.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtUpsert, 4, record.filename.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtUpsert, 5, baseName.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtUpsert, 6, ext.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmtUpsert, 7, record.size);
        sqlite3_bind_int(stmtUpsert, 8, record.isDir ? 1 : 0);
        sqlite3_bind_int(stmtUpsert, 9, record.isEmpty ? 1 : 0);
        sqlite3_bind_int(stmtUpsert, 10, record.isHidden ? 1 : 0);
        sqlite3_bind_int(stmtUpsert, 11, record.isManaged ? 1 : 0);
        sqlite3_bind_int(stmtUpsert, 12, record.encrypted ? 1 : 0);
        sqlite3_bind_int(stmtUpsert, 13, record.pinned ? 1 : 0);
        sqlite3_bind_int(stmtUpsert, 14, record.rating);
        sqlite3_bind_text(stmtUpsert, 15, record.manualColor.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtUpsert, 16, record.autoColor.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmtUpsert, 17, record.width);
        sqlite3_bind_int(stmtUpsert, 18, record.height);
        sqlite3_bind_int(stmtUpsert, 19, record.thumbStatus);
        sqlite3_bind_text(stmtUpsert, 20, record.note.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtUpsert, 21, record.url.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtUpsert, 22, record.sha256.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmtUpsert, 23, record.ctime);
        sqlite3_bind_int64(stmtUpsert, 24, record.mtime);
        sqlite3_bind_int64(stmtUpsert, 25, record.added_at > 0 ? record.added_at : now);
        sqlite3_bind_int64(stmtUpsert, 26, now);

        if (sqlite3_step(stmtUpsert) != SQLITE_DONE) {
            DatabaseManager::logSqlError("LibraryItemDao::upsertItems step upsert", db, upsertSql);
            cleanupStmts();
            return false;
        }

        // 重建 file_tags
        sqlite3_reset(stmtDelTags);
        sqlite3_clear_bindings(stmtDelTags);
        sqlite3_bind_text(stmtDelTags, 1, fileId.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmtDelTags);

        for (const QString& tag : record.tags) {
            if (tag.trimmed().isEmpty()) continue;

            sqlite3_reset(stmtTagDict);
            sqlite3_clear_bindings(stmtTagDict);
            sqlite3_bind_text(stmtTagDict, 1, tag.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmtTagDict);

            sqlite3_reset(stmtInsTag);
            sqlite3_clear_bindings(stmtInsTag);
            sqlite3_bind_text(stmtInsTag, 1, fileId.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmtInsTag, 2, tag.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmtInsTag);
        }

        // 重建 item_palettes
        sqlite3_reset(stmtDelPal);
        sqlite3_clear_bindings(stmtDelPal);
        sqlite3_bind_text(stmtDelPal, 1, fileId.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmtDelPal);

        for (size_t i = 0; i < record.palettes.size(); ++i) {
            sqlite3_reset(stmtInsPal);
            sqlite3_clear_bindings(stmtInsPal);
            sqlite3_bind_text(stmtInsPal, 1, fileId.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmtInsPal, 2, static_cast<int>(i));
            sqlite3_bind_text(stmtInsPal, 3, record.palettes[i].first.name().toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmtInsPal, 4, record.palettes[i].second);
            sqlite3_step(stmtInsPal);
        }
    }

    cleanupStmts();
    return tx.commit();
}

bool LibraryItemDao::removeItemsByPaths(const QStringList& paths) {
    if (paths.isEmpty()) return true;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    SqlTransaction tx(db);

    const char* sql = "DELETE FROM library_items WHERE file_path = ? COLLATE NOCASE;";
    sqlite3_stmt* stmt = nullptr;
    for (const QString& path : paths) {
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, path.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        } else {
            DatabaseManager::logSqlError("LibraryItemDao::removeItemsByPaths prepare", db, sql);
        }
    }
    return tx.commit();
}

bool LibraryItemDao::renameItemPath(const QString& oldPath, const QString& newPath) {
    return updatePath(oldPath, newPath);
}

bool LibraryItemDao::updatePath(const QString& oldPath, const QString& newPath) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || oldPath.isEmpty() || newPath.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    QFileInfo info(newPath);
    const char* sql = "UPDATE library_items SET file_path = ?, file_name = ?, base_name = ?, extension = ?, sync_state = 0 WHERE file_path = ? COLLATE NOCASE;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, newPath.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, info.fileName().toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, info.completeBaseName().toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, info.suffix().toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, oldPath.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return ok;
    } else {
        DatabaseManager::logSqlError("LibraryItemDao::updatePath prepare", db, sql);
    }
    return false;
}

bool LibraryItemDao::updatePathPrefix(const QString& oldPrefix, const QString& newPrefix) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || oldPrefix.isEmpty() || newPrefix.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    QString cleanOld = QDir::toNativeSeparators(oldPrefix);
    QString cleanNew = QDir::toNativeSeparators(newPrefix);

    const char* sql = "UPDATE library_items SET file_path = ? || SUBSTR(file_path, ?) WHERE file_path LIKE ? || '%' COLLATE NOCASE;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, cleanNew.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, cleanOld.length() + 1);
        sqlite3_bind_text(stmt, 3, cleanOld.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return ok;
    } else {
        DatabaseManager::logSqlError("LibraryItemDao::updatePathPrefix prepare", db, sql);
    }
    return false;
}

bool LibraryItemDao::markSyncStateByPath(const QString& path, int syncState) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || path.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    const char* sql = "UPDATE library_items SET sync_state = ? WHERE file_path = ? COLLATE NOCASE;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, syncState);
        sqlite3_bind_text(stmt, 2, path.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return ok;
    } else {
        DatabaseManager::logSqlError("LibraryItemDao::markSyncStateByPath prepare", db, sql);
    }
    return false;
}

} // namespace QuarkMeta
