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
     * @brief 统一批量添加至收藏夹并触发全局统一交互反馈 (SSOT)
     */
    void addFavorites(const QStringList& paths, int parentId = 0);

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
