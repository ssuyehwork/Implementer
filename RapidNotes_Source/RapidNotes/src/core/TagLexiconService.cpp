// MSVC Rebuild Trigger 2026-08-28
#include "TagLexiconService.h"
#include "../meta/DatabaseManager.h"
#include <QDateTime>
#include <mutex>

namespace QuarkMeta {

TagLexiconService& TagLexiconService::instance() {
    static TagLexiconService s_instance;
    return s_instance;
}

QStringList TagLexiconService::querySuggestions(const QString& prefix, int limit) const {
    QStringList results;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return results;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    sqlite3_stmt* stmt = nullptr;

    QString cleanPrefix = prefix.trimmed();
    if (cleanPrefix.isEmpty()) {
        const char* sql = "SELECT name FROM tags ORDER BY use_count DESC, last_used DESC LIMIT ?";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, limit);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                const wchar_t* wname = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 0));
                if (wname) results << QString::fromWCharArray(wname);
            }
            sqlite3_finalize(stmt);
        }
    } else {
        const char* sql = "SELECT name FROM tags WHERE name LIKE ? ORDER BY use_count DESC, last_used DESC LIMIT ?";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            QString pattern = cleanPrefix + "%";
            sqlite3_bind_text16(stmt, 1, pattern.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 2, limit);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                const wchar_t* wname = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 0));
                if (wname) results << QString::fromWCharArray(wname);
            }
            sqlite3_finalize(stmt);
        }
    }
    return results;
}

QList<TagGroup> TagLexiconService::getAllTagGroups() const {
    QList<TagGroup> results;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return results;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, name, color, sort_order FROM tag_groups ORDER BY sort_order ASC, id ASC";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            TagGroup tg;
            tg.id = sqlite3_column_int(stmt, 0);
            const wchar_t* wname = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 1));
            const wchar_t* wcolor = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 2));
            if (wname) tg.name = QString::fromWCharArray(wname);
            if (wcolor) tg.colorHex = QString::fromWCharArray(wcolor);
            tg.sortOrder = sqlite3_column_int(stmt, 3);

            sqlite3_stmt* itemStmt = nullptr;
            const char* itemSql = "SELECT tag_name FROM tag_group_items WHERE group_id = ? ORDER BY tag_name ASC";
            if (sqlite3_prepare_v2(db, itemSql, -1, &itemStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(itemStmt, 1, tg.id);
                while (sqlite3_step(itemStmt) == SQLITE_ROW) {
                    const wchar_t* wtag = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(itemStmt, 0));
                    if (wtag) {
                        TagEntry te;
                        te.name = QString::fromWCharArray(wtag);
                        te.groupId = tg.id;
                        tg.tags << te;
                    }
                }
                sqlite3_finalize(itemStmt);
            }
            results.append(tg);
        }
        sqlite3_finalize(stmt);
    }
    return results;
}

QStringList TagLexiconService::getAllTagNames() const {
    return getAllMasterTags();
}

QStringList TagLexiconService::getAllMasterTags() const {
    QStringList results;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return results;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT name FROM tags ORDER BY last_used DESC, name ASC";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const wchar_t* wname = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 0));
            if (wname) results << QString::fromWCharArray(wname);
        }
        sqlite3_finalize(stmt);
    }
    return results;
}

bool TagLexiconService::addTag(const QString& tagName, int groupId, const QString& colorHex) {
    QString cleanName = tagName.trimmed();
    if (cleanName.isEmpty()) return false;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    const char* sql =
        "INSERT INTO tags (name, color, last_used, use_count) VALUES (?, ?, ?, 1) "
        "ON CONFLICT(name) DO UPDATE SET last_used = excluded.last_used, use_count = use_count + 1;";

    sqlite3_stmt* stmt = nullptr;
    bool success = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt, 1, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt, 2, colorHex.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 3, now);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
            sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
        }
        sqlite3_finalize(stmt);
    }

    if (success && groupId > 0) {
        const char* groupSql = "INSERT INTO tag_group_items (group_id, tag_name) VALUES (?, ?) ON CONFLICT(group_id, tag_name) DO NOTHING;";
        if (sqlite3_prepare_v2(db, groupSql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, groupId);
            sqlite3_bind_text16(stmt, 2, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }

    if (success) emit lexiconChanged();
    return success;
}

bool TagLexiconService::addTags(const QStringList& tags, int groupId, const QString& colorHex) {
    QStringList cleanTags;
    for (const QString& tag : tags) {
        QString clean = tag.trimmed();
        if (!clean.isEmpty()) {
            cleanTags.append(clean);
        }
    }
    if (cleanTags.isEmpty()) return false;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    SqlTransaction trans(db);

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    const char* sql =
        "INSERT INTO tags (name, color, last_used, use_count) VALUES (?, ?, ?, 1) "
        "ON CONFLICT(name) DO UPDATE SET last_used = excluded.last_used, use_count = use_count + 1;";

    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        for (const QString& cleanName : cleanTags) {
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);

            sqlite3_bind_text16(stmt, 1, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text16(stmt, 2, colorHex.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmt, 3, now);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
        success = true;
    }

    if (success && groupId > 0) {
        const char* groupSql = "INSERT INTO tag_group_items (group_id, tag_name) VALUES (?, ?) ON CONFLICT(group_id, tag_name) DO NOTHING;";
        if (sqlite3_prepare_v2(db, groupSql, -1, &stmt, nullptr) == SQLITE_OK) {
            for (const QString& cleanName : cleanTags) {
                sqlite3_reset(stmt);
                sqlite3_clear_bindings(stmt);

                sqlite3_bind_int(stmt, 1, groupId);
                sqlite3_bind_text16(stmt, 2, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(stmt);
            }
            sqlite3_finalize(stmt);
        }
    }

    if (success) {
        bool res = trans.commit();
        if (res) {
            sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
            emit lexiconChanged();
        }
        return res;
    }

    return false;
}

bool TagLexiconService::renameTag(const QString& oldName, const QString& newName) {
    QString cleanOld = oldName.trimmed();
    QString cleanNew = newName.trimmed();
    if (cleanOld.isEmpty() || cleanNew.isEmpty() || cleanOld == cleanNew) return false;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    SqlTransaction trans(db);

    sqlite3_stmt* stmt1 = nullptr;
    sqlite3_stmt* stmt2 = nullptr;

    if (sqlite3_prepare_v2(db, "UPDATE tags SET name = ? WHERE name = ?", -1, &stmt1, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt1, 1, cleanNew.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt1, 2, cleanOld.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt1);
        sqlite3_finalize(stmt1);
    }

    if (sqlite3_prepare_v2(db, "UPDATE tag_group_items SET tag_name = ? WHERE tag_name = ?", -1, &stmt2, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt2, 1, cleanNew.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt2, 2, cleanOld.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt2);
        sqlite3_finalize(stmt2);
    }

    bool res = trans.commit();
    if (res) {
        sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
        emit lexiconChanged();
    }
    return res;
}

bool TagLexiconService::deleteTag(const QString& tagName) {
    QString cleanName = tagName.trimmed();
    if (cleanName.isEmpty()) return false;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    SqlTransaction trans(db);

    sqlite3_stmt* stmt1 = nullptr;
    sqlite3_stmt* stmt2 = nullptr;

    if (sqlite3_prepare_v2(db, "DELETE FROM tags WHERE name = ?", -1, &stmt1, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt1, 1, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt1);
        sqlite3_finalize(stmt1);
    }

    if (sqlite3_prepare_v2(db, "DELETE FROM tag_group_items WHERE tag_name = ?", -1, &stmt2, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt2, 1, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt2);
        sqlite3_finalize(stmt2);
    }

    bool res = trans.commit();
    if (res) {
        sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
        emit lexiconChanged();
    }
    return res;
}

bool TagLexiconService::setTagColor(const QString& tagName, const QString& colorHex) {
    QString cleanName = tagName.trimmed();
    if (cleanName.isEmpty()) return false;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE tags SET color = ? WHERE name = ?";
    bool ok = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt, 1, colorHex.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt, 2, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        ok = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        if (ok) sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    }
    if (ok) emit lexiconChanged();
    return ok;
}

int TagLexiconService::createGroup(const QString& groupName, const QString& colorHex) {
    QString cleanName = groupName.trimmed();
    if (cleanName.isEmpty()) return -1;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return -1;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO tag_groups (name, color, sort_order) VALUES (?, ?, (SELECT IFNULL(MAX(sort_order), 0) + 1 FROM tag_groups))";
    int groupId = -1;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt, 1, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text16(stmt, 2, colorHex.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            groupId = static_cast<int>(sqlite3_last_insert_rowid(db));
            sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
        }
        sqlite3_finalize(stmt);
    }
    if (groupId > 0) emit lexiconChanged();
    return groupId;
}

bool TagLexiconService::renameGroup(int groupId, const QString& newName) {
    QString cleanName = newName.trimmed();
    if (groupId <= 0 || cleanName.isEmpty()) return false;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE tag_groups SET name = ? WHERE id = ?";
    bool ok = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt, 1, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, groupId);
        ok = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        if (ok) sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    }
    if (ok) emit lexiconChanged();
    return ok;
}

bool TagLexiconService::deleteGroup(int groupId) {
    if (groupId <= 0) return false;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    SqlTransaction trans(db);

    sqlite3_stmt* stmt1 = nullptr;
    sqlite3_stmt* stmt2 = nullptr;
    bool ok1 = false, ok2 = false;

    if (sqlite3_prepare_v2(db, "DELETE FROM tag_groups WHERE id = ?", -1, &stmt1, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt1, 1, groupId);
        ok1 = (sqlite3_step(stmt1) == SQLITE_DONE);
        sqlite3_finalize(stmt1);
    }

    if (sqlite3_prepare_v2(db, "DELETE FROM tag_group_items WHERE group_id = ?", -1, &stmt2, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt2, 1, groupId);
        ok2 = (sqlite3_step(stmt2) == SQLITE_DONE);
        sqlite3_finalize(stmt2);
    }

    if (ok1 && ok2) {
        bool res = trans.commit();
        if (res) {
            sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
            emit lexiconChanged();
        }
        return res;
    }
    return false;
}

bool TagLexiconService::moveTagToGroup(const QString& tagName, int targetGroupId) {
    removeTagFromGroup(tagName, -1);
    if (targetGroupId > 0) {
        return addTagToGroup(tagName, targetGroupId);
    }
    return true;
}

bool TagLexiconService::addTagToGroup(const QString& tagName, int targetGroupId) {
    QString cleanName = tagName.trimmed();
    if (cleanName.isEmpty() || targetGroupId <= 0) return false;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    addTag(cleanName);

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO tag_group_items (group_id, tag_name) VALUES (?, ?) ON CONFLICT(group_id, tag_name) DO NOTHING;";
    bool ok = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, targetGroupId);
        sqlite3_bind_text16(stmt, 2, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        ok = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        if (ok) sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    }
    if (ok) emit lexiconChanged();
    return ok;
}

bool TagLexiconService::removeTagFromGroup(const QString& tagName, int groupId) {
    QString cleanName = tagName.trimmed();
    if (cleanName.isEmpty()) return false;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    sqlite3_stmt* stmt = nullptr;
    bool ok = false;
    if (groupId <= 0) {
        const char* sql = "DELETE FROM tag_group_items WHERE tag_name = ?";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text16(stmt, 1, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
    } else {
        const char* sql = "DELETE FROM tag_group_items WHERE group_id = ? AND tag_name = ?";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, groupId);
            sqlite3_bind_text16(stmt, 2, cleanName.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
    }
    if (ok) {
        sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
        emit lexiconChanged();
    }
    return ok;
}

} // namespace QuarkMeta
