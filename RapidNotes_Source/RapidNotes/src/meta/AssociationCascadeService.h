#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QList>
#include "FavoriteDao.h"
#include "MarkedFolderDao.h"

namespace QuarkMeta {

// 库分类路径关联快照项
struct LibraryCategoryPathEntry {
    int categoryId = 0;
    QString path;
};

// 聚合业务关联快照
struct AssetAssociationSnapshot {
    QList<FavoriteRecord> favorites;
    QVector<LibraryCategoryPathEntry> libraryEntries;
    QList<MarkedFolderRecord> markedFolders;

    bool isEmpty() const {
        return favorites.isEmpty() && libraryEntries.isEmpty() && markedFolders.isEmpty();
    }
};

/**
 * @brief 资产删除级联清理与撤销恢复服务 (SSOT)
 */
class AssociationCascadeService {
public:
    static AssociationCascadeService& instance();

    /**
     * @brief 查找给定路径列表及其子路径涉及的所有收藏、库和标记记录
     */
    AssetAssociationSnapshot findCascadeAssociations(const QStringList& targetPaths);

    /**
     * @brief 级联清理给定路径及其子路径在收藏夹、库、标记中的关联
     */
    bool cleanCascadeAssociations(const QStringList& targetPaths);

    /**
     * @brief 还原之前捕获的关联快照
     */
    bool restoreAssociations(const AssetAssociationSnapshot& snapshot);

private:
    AssociationCascadeService() = default;
    ~AssociationCascadeService() = default;
    AssociationCascadeService(const AssociationCascadeService&) = delete;
    AssociationCascadeService& operator=(const AssociationCascadeService&) = delete;

    static QString escapeLikePattern(const QString& str);
};

} // namespace QuarkMeta
