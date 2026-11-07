#include "DiskTrashService.h"
#include "../meta/DatabaseManager.h"
#include "../meta/QuarkMetaJson.h"
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QUuid>
#include <QDebug>
#include <mutex>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

bool DiskTrashService::moveToDiskTrash(const QStringList& paths) {
    if (paths.isEmpty()) return true;

    bool allOk = true;
    for (const QString& p : paths) {
        QFileInfo info(p);
        QString drive = info.absolutePath().left(3); // e.g. "C:/"
        QString trashDir = QDir::toNativeSeparators(drive + ".QuarkMeta/disk_trash");

#ifdef Q_OS_WIN
        QDir().mkpath(trashDir);
        SetFileAttributesW(QDir::toNativeSeparators(drive + ".QuarkMeta").toStdWString().c_str(), FILE_ATTRIBUTE_HIDDEN);
#else
        QDir().mkpath(trashDir);
#endif

        bool isFolder = info.isDir();
        qint64 fileSize = info.size();
        qint64 createdAt = info.birthTime().isValid() ? info.birthTime().toMSecsSinceEpoch() : info.lastModified().toMSecsSinceEpoch();

        QString fileId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        QString itemContainerDir = trashDir + "/" + fileId;
        QDir().mkpath(itemContainerDir);

        QString dest = QDir::toNativeSeparators(itemContainerDir + "/" + info.fileName());

        if (QFile::rename(p, dest)) {
            // 物理移动成功后，漫游元数据至回收站专属 .QuarkMeta.json (同时物理擦除原目录中的孤儿元数据)
            QuarkMetaJson::roamItemMetadata(p, dest, true);

            sqlite3* db = DatabaseManager::instance().getGlobalDb();
            if (!db) {
                allOk = false;
                continue;
            }

            std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
            SqlTransaction trans(db);
            sqlite3_stmt* stmt = nullptr;
            const char* sql = "INSERT INTO disk_trash (file_id, trash_path, original_path, drive_letter, file_name, is_folder, file_size, created_at, deleted_at) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

            if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                QString driveLetter = drive.left(1).toUpper();
                sqlite3_bind_text16(stmt, 1, fileId.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text16(stmt, 2, dest.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text16(stmt, 3, p.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text16(stmt, 4, driveLetter.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text16(stmt, 5, info.fileName().toStdWString().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt, 6, isFolder ? 1 : 0);
                sqlite3_bind_int64(stmt, 7, fileSize);
                sqlite3_bind_int64(stmt, 8, createdAt);
                sqlite3_bind_int64(stmt, 9, QDateTime::currentMSecsSinceEpoch());

                if (sqlite3_step(stmt) == SQLITE_DONE) {
                    trans.commit();
                } else {
                    qWarning() << "[DiskTrashService] 插入 disk_trash 失败:" << sqlite3_errmsg(db);
                    allOk = false;
                }
                sqlite3_finalize(stmt);
            } else {
                qWarning() << "[DiskTrashService] 准备 SQL 失败:" << sqlite3_errmsg(db);
                allOk = false;
            }
        } else {
            qWarning() << "[DiskTrashService] 移动到回收站失败:" << p;
            allOk = false;
        }
    }
    return allOk;
}

bool DiskTrashService::restoreFromDiskTrash(int id, const QString& trashPath) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    QString originalPath;
    QString actualTrashPath = trashPath;
    qint64 trashCreatedAt = 0;
    sqlite3_stmt* stmt = nullptr;
    const char* sqlSel = "SELECT original_path, created_at, trash_path FROM disk_trash WHERE id = ?";
    if (sqlite3_prepare_v2(db, sqlSel, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const wchar_t* wOrig = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 0));
            if (wOrig) {
                originalPath = QString::fromWCharArray(wOrig);
            }
            trashCreatedAt = sqlite3_column_int64(stmt, 1);

            if (actualTrashPath.isEmpty()) {
                const wchar_t* wTrash = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 2));
                if (wTrash) {
                    actualTrashPath = QString::fromWCharArray(wTrash);
                }
            }
        }
        sqlite3_finalize(stmt);
    }

    if (originalPath.isEmpty() || actualTrashPath.isEmpty()) {
        return false;
    }

    QDir().mkpath(QFileInfo(originalPath).absolutePath());

    QString targetPath = originalPath;
    if (QFile::exists(originalPath)) {
        QFileInfo existingInfo(originalPath);
        qint64 diskCreatedAt = existingInfo.birthTime().isValid() ? existingInfo.birthTime().toMSecsSinceEpoch() : existingInfo.lastModified().toMSecsSinceEpoch();

        if (trashCreatedAt < diskCreatedAt) {
            QString baseDir = existingInfo.absolutePath();
            QString baseName = existingInfo.completeBaseName();
            QString suffix = existingInfo.suffix();
            QString newDiskPath;
            int counter = 1;
            do {
                QString candidateName = suffix.isEmpty() ? QString("%1-%2").arg(baseName).arg(counter) : QString("%1-%2.%3").arg(baseName).arg(counter).arg(suffix);
                newDiskPath = baseDir + "/" + candidateName;
                counter++;
            } while (QFile::exists(newDiskPath));

            QFile::rename(originalPath, newDiskPath);
            targetPath = originalPath;
        } else {
            QFileInfo trashInfo(originalPath);
            QString baseDir = trashInfo.absolutePath();
            QString baseName = trashInfo.completeBaseName();
            QString suffix = trashInfo.suffix();
            int counter = 1;
            do {
                QString candidateName = suffix.isEmpty() ? QString("%1-%2").arg(baseName).arg(counter) : QString("%1-%2.%3").arg(baseName).arg(counter).arg(suffix);
                targetPath = baseDir + "/" + candidateName;
                counter++;
            } while (QFile::exists(targetPath));
        }
    }

    if (QFile::rename(actualTrashPath, targetPath)) {
        // 从回收站反向漫游元数据回真实目标物理目录
        QuarkMetaJson::roamItemMetadata(actualTrashPath, targetPath, true);

        QDir(QFileInfo(actualTrashPath).absolutePath()).removeRecursively();

        SqlTransaction trans(db);
        sqlite3_stmt* delStmt = nullptr;
        const char* sqlDel = "DELETE FROM disk_trash WHERE id = ?";
        bool success = false;
        if (sqlite3_prepare_v2(db, sqlDel, -1, &delStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(delStmt, 1, id);
            if (sqlite3_step(delStmt) == SQLITE_DONE) {
                trans.commit();
                success = true;
            }
            sqlite3_finalize(delStmt);
        }
        return success;
    }

    return false;
}

bool DiskTrashService::restoreToDirectory(const QString& trashPath, const QString& targetDir) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    QFileInfo info(trashPath);
    QString dest = QDir(targetDir).filePath(info.fileName());
    QDir().mkpath(targetDir);

    bool moved = QFile::rename(trashPath, dest);
    if (!moved) {
        if (info.isDir()) {
            moved = QFile::copy(trashPath, dest);
            if (moved) QDir(trashPath).removeRecursively();
        } else {
            moved = QFile::copy(trashPath, dest) && QFile::remove(trashPath);
        }
    }

    if (moved) {
        // 还原至指定目录时反向漫游元数据
        QuarkMetaJson::roamItemMetadata(trashPath, dest, true);

        QDir(QFileInfo(trashPath).absolutePath()).removeRecursively();

        std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
        SqlTransaction trans(db);
        sqlite3_stmt* delStmt = nullptr;
        const char* sqlDel = "DELETE FROM disk_trash WHERE trash_path = ?";
        if (sqlite3_prepare_v2(db, sqlDel, -1, &delStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text16(delStmt, 1, trashPath.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(delStmt) == SQLITE_DONE) {
                trans.commit();
            }
            sqlite3_finalize(delStmt);
        }
        return true;
    }
    return false;
}

bool DiskTrashService::permanentlyDeleteDiskTrash(int id, const QString& trashPath) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    QFileInfo info(trashPath);
    bool physicalOk = false;
    if (info.exists()) {
        if (info.isDir()) {
            physicalOk = QDir(trashPath).removeRecursively();
        } else {
            physicalOk = QFile::remove(trashPath);
        }
    } else {
        physicalOk = true;
    }

    if (physicalOk) {
        QDir containerDir = QFileInfo(trashPath).absoluteDir();
        if (containerDir.exists()) {
            containerDir.removeRecursively();
        }

        std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
        SqlTransaction trans(db);
        sqlite3_stmt* delStmt = nullptr;
        const char* sqlDel = "DELETE FROM disk_trash WHERE id = ?";
        bool success = false;
        if (sqlite3_prepare_v2(db, sqlDel, -1, &delStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(delStmt, 1, id);
            if (sqlite3_step(delStmt) == SQLITE_DONE) {
                trans.commit();
                success = true;
            }
            sqlite3_finalize(delStmt);
        }
        return success;
    }

    return false;
}

bool DiskTrashService::restoreAllDiskTrash() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    struct TrashItem {
        int id;
        QString trashPath;
    };
    std::vector<TrashItem> items;

    {
        std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
        sqlite3_stmt* stmt = nullptr;
        const char* sql = "SELECT id, trash_path FROM disk_trash";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                const wchar_t* wPath = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 1));
                if (wPath) {
                    items.push_back({id, QString::fromWCharArray(wPath)});
                }
            }
            sqlite3_finalize(stmt);
        }
    }

    bool allOk = true;
    for (const auto& item : items) {
        if (!restoreFromDiskTrash(item.id, item.trashPath)) {
            allOk = false;
        }
    }
    return allOk;
}

bool DiskTrashService::getLatestTrashItemByOriginalPath(const QString& originalPath, int& outId, QString& outTrashPath) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db || originalPath.isEmpty()) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, trash_path FROM disk_trash WHERE original_path = ? ORDER BY deleted_at DESC LIMIT 1";
    bool found = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text16(stmt, 1, originalPath.toStdWString().c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            outId = sqlite3_column_int(stmt, 0);
            const wchar_t* wTrash = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 1));
            if (wTrash) {
                outTrashPath = QString::fromWCharArray(wTrash);
            }
            found = true;
        }
        sqlite3_finalize(stmt);
    }
    return found;
}

bool DiskTrashService::emptyDiskTrash() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    struct TrashItem {
        int id;
        QString trashPath;
    };
    std::vector<TrashItem> items;

    {
        std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());
        sqlite3_stmt* stmt = nullptr;
        const char* sql = "SELECT id, trash_path FROM disk_trash";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                const wchar_t* wPath = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 1));
                if (wPath) {
                    items.push_back({id, QString::fromWCharArray(wPath)});
                }
            }
            sqlite3_finalize(stmt);
        }
    }

    bool allOk = true;
    for (const auto& item : items) {
        if (!permanentlyDeleteDiskTrash(item.id, item.trashPath)) {
            allOk = false;
        }
    }
    return allOk;
}

} // namespace QuarkMeta