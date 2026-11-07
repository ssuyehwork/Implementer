#include "FavoriteService.h"
#include "../ui/UiHelper.h"
#include "../ui/ToolTipOverlay.h"
#include <QDir>
#include <QFileInfo>
#include <QCursor>

namespace QuarkMeta {

FavoriteService& FavoriteService::instance() {
    static FavoriteService s_instance;
    return s_instance;
}

FavoriteService::FavoriteService(QObject* parent) : QObject(parent) {
    FavoriteDao::initTable();
}

bool FavoriteService::isFavorite(const QString& path) const {
    if (path.isEmpty()) return false;
    QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(path));
    return FavoriteDao::containsPath(cleanPath);
}

bool FavoriteService::addFavorite(const QString& path, int parentId) {
    QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(path));
    if (cleanPath.isEmpty()) return false;

    QFileInfo fi(cleanPath);
    if (!fi.exists()) return false;

    bool existsAlready = FavoriteDao::containsPath(cleanPath);
    if (existsAlready) {
        ToolTipOverlay::instance()->showText(QCursor::pos(), "该文件夹已在收藏夹中，请勿重复添加", 2000, QColor("#e81123"));
        return false;
    }

    bool isDir = fi.isDir();
    QString finalColorHex = "#888888";

    if (isDir) {
        bool isDriveRoot = fi.isRoot() || cleanPath.endsWith(":\\") || cleanPath.endsWith(":/") || (cleanPath.length() == 2 && cleanPath.endsWith(':'));
        if (isDriveRoot) {
            finalColorHex = "#378ADD";
        }
    }

    bool ok = FavoriteDao::addFavorite(cleanPath, parentId, "folder_filled", finalColorHex);
    if (ok) {
        emit favoriteChanged(cleanPath, true);
    }
    return ok;
}

int FavoriteService::addVirtualCategory(const QString& name, int parentId) {
    int newId = FavoriteDao::addVirtualCategory(name, parentId, "folder_filled", "#888888");
    if (newId > 0) {
        emit favoritesReloaded();
    }
    return newId;
}

bool FavoriteService::removeFavoriteById(int id) {
    bool ok = FavoriteDao::removeFavoriteById(id);
    if (ok) {
        emit favoritesReloaded();
    }
    return ok;
}

bool FavoriteService::removeFavorite(const QString& path) {
    QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(path));
    if (cleanPath.isEmpty() || !FavoriteDao::containsPath(cleanPath)) return false;

    bool ok = FavoriteDao::removeFavorite(cleanPath);
    if (ok) {
        emit favoriteChanged(cleanPath, false);
    }
    return ok;
}

bool FavoriteService::toggleFavorite(const QString& path) {
    if (isFavorite(path)) {
        return removeFavorite(path);
    } else {
        return addFavorite(path);
    }
}

QAction* FavoriteService::buildFavoriteAction(QMenu* parentMenu, const QString& path, QObject* receiver) {
    if (!parentMenu || path.isEmpty()) return nullptr;

    bool fav = isFavorite(path);
    QIcon favIcon = fav ? UiHelper::getIcon("close", QColor("#EEEEEE"), 18) : UiHelper::getIcon("Favorites.svg", QColor("#EEEEEE"), 18);
    QString text = fav ? "从收藏夹移除" : "添加至收藏夹";

    QAction* action = parentMenu->addAction(favIcon, text);
    connect(action, &QAction::triggered, receiver ? receiver : parentMenu, [this, path]() {
        toggleFavorite(path);
    });
    return action;
}

} // namespace QuarkMeta
