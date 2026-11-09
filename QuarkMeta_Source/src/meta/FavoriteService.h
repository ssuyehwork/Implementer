#pragma once

#include <QObject>
#include <QString>
#include <QMenu>
#include <QAction>
#include "FavoriteDao.h"

namespace QuarkMeta {

class FavoriteService : public QObject {
    Q_OBJECT
public:
    static FavoriteService& instance();

    bool isFavorite(const QString& path) const;
    bool addFavorite(const QString& path, int parentId = 0);
    int addVirtualCategory(const QString& name, int parentId = 0);
    bool removeFavorite(const QString& path);
    bool removeFavoriteById(int id);
    bool toggleFavorite(const QString& path);

    /**
     * @brief 获取所有收藏项记录（下沉封装 FavoriteDao）
     */
    QList<FavoriteRecord> getAllFavorites() const;

    /**
     * @brief 更新收藏项节点属性（名称、图标、色彩）
     */
    bool updateFavoriteNode(int id, const QString& name, const QString& iconKey, const QString& colorHex);

    /**
     * @brief 更新文件夹自动递归状态
     */
    bool updateAutoRecursive(int id, bool autoRecursive);

    /**
     * @brief 批量更新收藏项父子层级与自定义排序
     */
    bool saveFavoritesOrder(const QList<NodeOrderEntry>& entries);

    /**
     * @brief 统一批量添加至收藏夹并触发全局统一交互反馈 (SSOT)
     */
    void addFavorites(const QStringList& paths, int parentId = 0);

    /**
     * @brief 在指定位置批量添加至收藏夹并触发全局统一交互反馈 (SSOT)
     */
    void addFavoritesAt(const QStringList& paths, int parentId = 0, int beforeSiblingId = 0);

    /**
     * @brief 统一批量从收藏夹移除并触发全局统一交互反馈 (SSOT)
     */
    void removeFavorites(const QStringList& paths);

    QAction* buildFavoriteAction(QMenu* parentMenu, const QString& path, QObject* receiver = nullptr);

signals:
    void favoriteChanged(const QString& path, bool isFavorite);
    void favoritesReloaded();

private:
    explicit FavoriteService(QObject* parent = nullptr);
    ~FavoriteService() override = default;
    Q_DISABLE_COPY_MOVE(FavoriteService)
};

} // namespace QuarkMeta
