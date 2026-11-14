#include "AssociationCascadeService.h"
#include "../core/DatabaseManager.h"
#include "LibraryService.h"
#include "LibrarySyncService.h"
#include "MarkedFolderService.h"
#include "LibraryItemDao.h"
#include <QDir>
#include <QDateTime>
#include <sqlite3.h>

namespace RapidNotes {

AssociationCascadeService& AssociationCascadeService::instance() {
    static AssociationCascadeService s_inst;
    return s_inst;
}

QString AssociationCascadeService::escapeLikePattern(const QString& str) {
    QString res = str;
    res.replace('^', "^^");
    res.replace('%', "^%");
    res.replace('_', "^_");
    return res;
}

AssetAssociationSnapshot AssociationCascadeService::findCascadeAssociations(const QStringList& targetPaths) {
    AssetAssociationSnapshot snapshot;
    if (targetPaths.isEmpty()) return snapshot;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return snapshot;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    for (const QString& target : targetPaths) {
        if (target.trimmed().isEmpty()) continue;
        QString clean = QDir::cleanPath(target);
        QString tFwd = clean;
        tFwd.replace('\\', '/');
        QString tBak = clean;
        tBak.replace('/', '\\');

        QString pFwd = escapeLikePattern(tFwd) + "/%";
        QString pBak = escapeLikePattern(tBak) + "\\%";


        // 2. 查询库中匹配项
        const char* sqlLib = "SELECT category_id, path FROM library_category_paths "
                             "WHERE path = ? COLLATE NOCASE OR path = ? COLLATE NOCASE "
                             "   OR path LIKE ? ESCAPE '^' COLLATE NOCASE OR path LIKE ? ESCAPE '^' COLLATE NOCASE;";
        sqlite3_stmt* stmtLib = nullptr;
        if (sqlite3_prepare_v2(db, sqlLib, -1, &stmtLib, nullptr) == SQLITE_OK) {
            std::string sFwd = tFwd.toStdString();
            std::string sBak = tBak.toStdString();
            std::string sPFwd = pFwd.toStdString();
            std::string sPBak = pBak.toStdString();

            sqlite3_bind_text(stmtLib, 1, sFwd.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmtLib, 2, sBak.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmtLib, 3, sPFwd.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmtLib, 4, sPBak.c_str(), -1, SQLITE_TRANSIENT);

            while (sqlite3_step(stmtLib) == SQLITE_ROW) {
                LibraryCategoryPathEntry entry;
                entry.categoryId = sqlite3_column_int(stmtLib, 0);
                const char* p = reinterpret_cast<const char*>(sqlite3_column_text(stmtLib, 1));
                entry.path = p ? QString::fromUtf8(p) : QString();

                bool alreadyAdded = false;
                for (const auto& existing : snapshot.libraryEntries) {
                    if (existing.categoryId == entry.categoryId && existing.path.compare(entry.path, Qt::CaseInsensitive) == 0) {
                        alreadyAdded = true;
                        break;
                    }
                }
                if (!alreadyAdded) {
                    snapshot.libraryEntries.append(entry);
                }
            }
            sqlite3_finalize(stmtLib);
        }

        // 3. 查询标记目录中匹配项
        const char* sqlMark = "SELECT id, folder_path, is_marked, updated_at FROM marked_folders "
                              "WHERE folder_path = ? COLLATE NOCASE OR folder_path = ? COLLATE NOCASE "
                              "   OR folder_path LIKE ? ESCAPE '^' COLLATE NOCASE OR folder_path LIKE ? ESCAPE '^' COLLATE NOCASE;";
        sqlite3_stmt* stmtMark = nullptr;
        if (sqlite3_prepare_v2(db, sqlMark, -1, &stmtMark, nullptr) == SQLITE_OK) {
            std::string sFwd = tFwd.toStdString();
            std::string sBak = tBak.toStdString();
            std::string sPFwd = pFwd.toStdString();
            std::string sPBak = pBak.toStdString();

            sqlite3_bind_text(stmtMark, 1, sFwd.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmtMark, 2, sBak.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmtMark, 3, sPFwd.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmtMark, 4, sPBak.c_str(), -1, SQLITE_TRANSIENT);

            while (sqlite3_step(stmtMark) == SQLITE_ROW) {
                MarkedFolderRecord rec;
                rec.id = sqlite3_column_int(stmtMark, 0);
                const char* p = reinterpret_cast<const char*>(sqlite3_column_text(stmtMark, 1));
                rec.folderPath = p ? QString::fromUtf8(p) : QString();
                rec.isMarked = (sqlite3_column_int(stmtMark, 2) != 0);
                rec.updatedAt = sqlite3_column_int64(stmtMark, 3);

                bool alreadyAdded = false;
                for (const auto& existing : snapshot.markedFolders) {
                    if (existing.id == rec.id) {
                        alreadyAdded = true;
                        break;
                    }
                }
                if (!alreadyAdded) {
                    snapshot.markedFolders.append(rec);
                }
            }
            sqlite3_finalize(stmtMark);
        }
    }

    return snapshot;
}

bool AssociationCascadeService::cleanCascadeAssociations(const QStringList& targetPaths) {
    if (targetPaths.isEmpty()) return true;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    bool libChanged = false;
    bool markChanged = false;
    QStringList removedLibPaths;
    bool ok = false;

    // 🟢 限定锁与数据库事务的作用域：在发射信号和调用外部服务前必须彻底释放锁！
    {
        std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
        SqlTransaction trans(db);

        const char* sqlDelLibCat = "DELETE FROM library_category_paths "
                                   "WHERE path = ? COLLATE NOCASE OR path = ? COLLATE NOCASE "
                                   "   OR path LIKE ? ESCAPE '^' COLLATE NOCASE OR path LIKE ? ESCAPE '^' COLLATE NOCASE;";
        sqlite3_stmt* stmtDelLibCat = nullptr;
        sqlite3_prepare_v2(db, sqlDelLibCat, -1, &stmtDelLibCat, nullptr);

        const char* sqlDelLibItems = "DELETE FROM library_items "
                                     "WHERE file_path = ? COLLATE NOCASE OR file_path = ? COLLATE NOCASE "
                                     "   OR file_path LIKE ? ESCAPE '^' COLLATE NOCASE OR file_path LIKE ? ESCAPE '^' COLLATE NOCASE;";
        sqlite3_stmt* stmtDelLibItems = nullptr;
        sqlite3_prepare_v2(db, sqlDelLibItems, -1, &stmtDelLibItems, nullptr);

        const char* sqlDelMark = "DELETE FROM marked_folders "
                                 "WHERE folder_path = ? COLLATE NOCASE OR folder_path = ? COLLATE NOCASE "
                                 "   OR folder_path LIKE ? ESCAPE '^' COLLATE NOCASE OR folder_path LIKE ? ESCAPE '^' COLLATE NOCASE;";
        sqlite3_stmt* stmtDelMark = nullptr;
        sqlite3_prepare_v2(db, sqlDelMark, -1, &stmtDelMark, nullptr);

        for (const QString& target : targetPaths) {
            if (target.trimmed().isEmpty()) continue;
            QString clean = QDir::cleanPath(target);
            QString tFwd = clean;
            tFwd.replace('\\', '/');
            QString tBak = clean;
            tBak.replace('/', '\\');

            QString pFwd = escapeLikePattern(tFwd) + "/%";
            QString pBak = escapeLikePattern(tBak) + "\\%";

            std::string sFwd = tFwd.toStdString();
            std::string sBak = tBak.toStdString();
            std::string sPFwd = pFwd.toStdString();
            std::string sPBak = pBak.toStdString();

            if (stmtDelLibCat) {
                sqlite3_bind_text(stmtDelLibCat, 1, sFwd.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtDelLibCat, 2, sBak.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtDelLibCat, 3, sPFwd.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtDelLibCat, 4, sPBak.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(stmtDelLibCat);
                if (sqlite3_changes(db) > 0) libChanged = true;
                sqlite3_reset(stmtDelLibCat);
            }

            if (stmtDelLibItems) {
                sqlite3_bind_text(stmtDelLibItems, 1, sFwd.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtDelLibItems, 2, sBak.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtDelLibItems, 3, sPFwd.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtDelLibItems, 4, sPBak.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(stmtDelLibItems);
                sqlite3_reset(stmtDelLibItems);
            }

            if (stmtDelMark) {
                sqlite3_bind_text(stmtDelMark, 1, sFwd.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtDelMark, 2, sBak.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtDelMark, 3, sPFwd.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtDelMark, 4, sPBak.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(stmtDelMark);
                if (sqlite3_changes(db) > 0) markChanged = true;
                sqlite3_reset(stmtDelMark);
            }
            removedLibPaths.append(clean);
        }

        if (stmtDelLibCat) sqlite3_finalize(stmtDelLibCat);
        if (stmtDelLibItems) sqlite3_finalize(stmtDelLibItems);
        if (stmtDelMark) sqlite3_finalize(stmtDelMark);

        ok = trans.commit();
        sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    } // 🟢 离开花括号，lock 彻底析构，全局互斥锁归还

    // 🟢 在锁外发射信号与调度外部服务，彻底杜绝同线程重入死锁
    if (ok) {
        if (libChanged) {
            LibrarySyncService::instance().removePaths(removedLibPaths);
            emit LibraryService::instance().libraryChanged();
        }
        if (markChanged) {
            emit MarkedFolderService::instance().markedFoldersChanged();
        }
    }

    return ok;
}

bool AssociationCascadeService::restoreAssociations(const AssetAssociationSnapshot& snapshot) {
    if (snapshot.isEmpty()) return true;

    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    QStringList pathsToSync;
    bool ok = false;

    // 🟢 限定锁与数据库事务的作用域：在发射信号和调用外部服务前必须彻底释放锁！
    {
        std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
        SqlTransaction trans(db);

        // 恢复库分类关系
        if (!snapshot.libraryEntries.isEmpty()) {
            const char* sql = "INSERT OR IGNORE INTO library_category_paths (category_id, path) VALUES (?, ?);";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                for (const auto& ent : snapshot.libraryEntries) {
                    sqlite3_bind_int(stmt, 1, ent.categoryId);
                    std::string pStd = ent.path.toStdString();
                    sqlite3_bind_text(stmt, 2, pStd.c_str(), -1, SQLITE_TRANSIENT);

                    sqlite3_step(stmt);
                    sqlite3_reset(stmt);
                    pathsToSync.append(ent.path);
                }
                sqlite3_finalize(stmt);
            }
        }

        // 3. 恢复标记目录
        if (!snapshot.markedFolders.isEmpty()) {
            const char* sql = "INSERT OR REPLACE INTO marked_folders (id, folder_path, is_marked, updated_at) VALUES (?, ?, ?, ?);";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                for (const auto& mark : snapshot.markedFolders) {
                    if (mark.id > 0) {
                        sqlite3_bind_int(stmt, 1, mark.id);
                    } else {
                        sqlite3_bind_null(stmt, 1);
                    }
                    QByteArray pathUtf8 = mark.folderPath.toUtf8();
                    sqlite3_bind_text(stmt, 2, pathUtf8.constData(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_int(stmt, 3, mark.isMarked ? 1 : 0);
                    sqlite3_bind_int64(stmt, 4, mark.updatedAt > 0 ? mark.updatedAt : QDateTime::currentMSecsSinceEpoch());

                    sqlite3_step(stmt);
                    sqlite3_reset(stmt);
                }
                sqlite3_finalize(stmt);
            }
        }

        ok = trans.commit();
        sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    } // 🟢 离开花括号，lock 彻底析构，全局互斥锁归还

    // 🟢 在锁外发射信号与调度外部服务，彻底杜绝同线程重入死锁
    if (ok) {
        if (!snapshot.libraryEntries.isEmpty()) {
            LibrarySyncService::instance().syncPaths(pathsToSync);
            emit LibraryService::instance().libraryChanged();
        }
        if (!snapshot.markedFolders.isEmpty()) {
            emit MarkedFolderService::instance().markedFoldersChanged();
        }
    }

    return ok;
}

} // namespace RapidNotes
