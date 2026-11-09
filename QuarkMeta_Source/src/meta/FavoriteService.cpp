#include "FavoriteService.h"
#include "../ui/UiHelper.h"
#include "../ui/ToolTipOverlay.h"
#include <QDir>
#include <QFileInfo>
#include <QCursor>
#include <QColor>

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

QList<FavoriteRecord> FavoriteService::getAllFavorites() const {
    return FavoriteDao::getAllFavorites();
}

bool FavoriteService::updateFavoriteNode(int id, const QString& name, const QString& iconKey, const QString& colorHex) {
    return FavoriteDao::updateFavoriteNode(id, name, iconKey, colorHex);
}

bool FavoriteService::updateAutoRecursive(int id, bool autoRecursive) {
    return FavoriteDao::updateAutoRecursive(id, autoRecursive);
}

bool FavoriteService::saveFavoritesOrder(const QList<NodeOrderEntry>& entries) {
    return FavoriteDao::updateNodesParentAndOrder(entries);
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


void FavoriteService::addFavorites(const QStringList& paths, int parentId) {
    addFavoritesAt(paths, parentId, 0);
}

void FavoriteService::addFavoritesAt(const QStringList& paths, int parentId, int beforeSiblingId) {
    if (paths.isEmpty()) return;

    int duplicateCount = 0;
    QList<FavoriteDao::NewFavoriteItem> itemsToAdd;

    for (const QString& path : paths) {
        if (isFavorite(path)) {
            duplicateCount++;
        } else {
            QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(path));
            if (cleanPath.isEmpty()) continue;

            QFileInfo fi(cleanPath);
            if (!fi.exists()) continue;

            bool isDir = fi.isDir();
            QString finalColorHex = "#888888";
            if (isDir) {
                bool isDriveRoot = fi.isRoot() || cleanPath.endsWith(":\\") || cleanPath.endsWith(":/") || (cleanPath.length() == 2 && cleanPath.endsWith(':'));
                if (isDriveRoot) {
                    finalColorHex = "#378ADD";
                }
            }

            FavoriteDao::NewFavoriteItem item;
            item.path = cleanPath;
            item.name = fi.fileName().isEmpty() ? cleanPath : fi.fileName();
            item.iconKey = "folder_filled";
            item.colorHex = finalColorHex;
            itemsToAdd.append(item);
        }
    }

    int addedCount = 0;
    if (!itemsToAdd.isEmpty()) {
        if (FavoriteDao::addFavoritesAt(itemsToAdd, parentId, beforeSiblingId)) {
            addedCount = itemsToAdd.size();
        }
    }

    if (addedCount > 0) {
        emit favoritesReloaded();
        if (duplicateCount == 0) {
            ToolTipOverlay::instance()->showToast("已成功添加至收藏夹", 1500, QColor("#2ecc71"));
        } else {
            ToolTipOverlay::instance()->showToast(QString("已添加 %1 个项目 (其余 %2 个已在收藏夹中)").arg(addedCount).arg(duplicateCount), 2000, QColor("#3498db"));
        }
    } else if (duplicateCount > 0) {
        ToolTipOverlay::instance()->showToast("该文件夹已在收藏夹中，请勿重复添加", 2000, QColor("#e81123"));
    }
}

void FavoriteService::removeFavorites(const QStringList& paths) {
    if (paths.isEmpty()) return;

    int removedCount = 0;
    for (const QString& path : paths) {
        if (isFavorite(path)) {
            QString cleanPath = QDir::toNativeSeparators(QDir::cleanPath(path));
            if (FavoriteDao::removeFavorite(cleanPath)) {
                removedCount++;
            }
        }
    }

    if (removedCount > 0) {
        emit favoritesReloaded();
        ToolTipOverlay::instance()->showToast("已从收藏夹移除", 1500, QColor("#e74c3c"));
    }
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
