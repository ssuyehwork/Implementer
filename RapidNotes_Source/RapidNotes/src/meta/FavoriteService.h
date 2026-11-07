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
