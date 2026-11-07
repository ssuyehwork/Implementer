#pragma once

#include "../core/ItemRecord.h"
#include <QStringList>
#include <vector>

namespace QuarkMeta {

class LibrarySyncService;

class LibraryItemDao {
public:
    // 按路径列表查询库条目记录（自动装配 file_tags 与 item_palettes）
    static std::vector<ItemRecord> getItemsByPaths(const QStringList& paths);

    // 查询当前库表中存储的所有成员路径对应的 ItemRecord 记录
    static std::vector<ItemRecord> getAllLibraryItems();

private:
    friend class LibrarySyncService;
    friend class LibraryValidationService;

    // 查询当前库表中存储的所有成员路径列表
    static QStringList getAllLibraryPaths();

    // 写接口锁定仅供 LibrarySyncService 友元调用
    static bool upsertItem(const ItemRecord& record);
    static bool upsertItems(const std::vector<ItemRecord>& records);
    static bool removeItemsByPaths(const QStringList& paths);
    static bool renameItemPath(const QString& oldPath, const QString& newPath);
    static bool updatePath(const QString& oldPath, const QString& newPath);
    static bool updatePathPrefix(const QString& oldPrefix, const QString& newPrefix);
    static bool markSyncStateByPath(const QString& path, int syncState);
};

} // namespace QuarkMeta
