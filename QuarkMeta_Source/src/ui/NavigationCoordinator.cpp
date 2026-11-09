#include "NavigationCoordinator.h"
#include "ActivePaneCoordinator.h"
#include "ColumnViewWidget.h"
#include "../core/NavigationService.h"
#include "../meta/FavoriteService.h"
#include "../meta/LibrarySyncService.h"
#include "../core/CentralEventHub.h"
#include "../core/VolumeOnlineManager.h"
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

namespace QuarkMeta {

NavigationCoordinator::NavigationCoordinator(ActivePaneCoordinator* activeCoordinator,
                                             NavPanel* navPanel,
                                             FavoritePanel* favoritePanel,
                                             LibraryPanel* libraryPanel,
                                             MarkedPanel* markedPanel,
                                             AddressBar* addressBar,
                                             TabBarWidget* tabBar,
                                             SearchController* searchController,
                                             QObject* parent)
    : QObject(parent),
      m_activeCoordinator(activeCoordinator),
      m_navPanel(navPanel),
      m_favoritePanel(favoritePanel),
      m_libraryPanel(libraryPanel),
      m_markedPanel(markedPanel),
      m_addressBar(addressBar),
      m_tabBar(tabBar),
      m_searchController(searchController) {
}

void NavigationCoordinator::showLocation(const QString& addressText, const QString& tabTitle, const QString& tabUrl) {
    if (m_addressBar) {
        m_addressBar->setPath(addressText);
    }
    if (m_tabBar) {
        m_tabBar->updateCurrentTabTitle(tabTitle, tabUrl);
    }
}

void NavigationCoordinator::locateItem(const QString& fullPath) {
    if (fullPath.isEmpty() || fullPath.contains("://")) return;

    QFileInfo fi(fullPath);
    QString targetDir = fi.absolutePath();
    ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
    if (!target) return;

    target->setPendingSelectPath(fullPath, false);

    if (m_searchController) {
        m_searchController->clearSearch(true);
    }
    target->search("");

    QString currentNative = QDir::toNativeSeparators(QDir::cleanPath(target->currentPath()));
    QString targetDirNative = QDir::toNativeSeparators(QDir::cleanPath(targetDir));

    bool isSameDir = (QString::compare(currentNative, targetDirNative, Qt::CaseInsensitive) == 0);

    if (isSameDir && !target->isRecursive()) {
        target->restoreSelections();
    } else if (isSameDir && target->isRecursive()) {
        target->loadDirectory(targetDir, false);
    } else {
        NavigationService::instance().navigateTo(targetDir, true, false);
    }

    if (m_navPanel) {
        m_navPanel->selectPath(targetDirNative);
    }
}

void NavigationCoordinator::refreshCurrent() {
    ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
    if (target && (target->getCurrentCategoryType() == "library" || target->getCurrentCategoryType() == "smart_folder")) {
        target->refreshAll();
    } else {
        NavigationService::instance().refresh();
    }
}

void NavigationCoordinator::bindPane(ContentPanel* pane) {
    if (!pane) return;

    // 遵循统一规则二：主窗格与动态副窗格统一在此连接且仅连接一次
    connect(pane, &ContentPanel::directorySelected, &NavigationService::instance(), [](const QString& path) {
        NavigationService::instance().navigateTo(path);
    });

    if (m_favoritePanel) {
        connect(pane, &ContentPanel::requestAddFavorite, this, [](const QStringList& paths) {
            FavoriteService::instance().addFavorites(paths);
        });
        connect(pane, &ContentPanel::requestRemoveFavorite, this, [](const QStringList& paths) {
            FavoriteService::instance().removeFavorites(paths);
        });
    }

    if (pane->columnView()) {
        connect(pane->columnView(), &ColumnViewWidget::pathNavigated, this, [this](const QString& path) {
            NavigationService::instance().navigateTo(path);
        });
    }
}

void NavigationCoordinator::wire() {
    connect(&NavigationService::instance(), &NavigationService::requestLocateItem, this, &NavigationCoordinator::locateItem);

    connect(&NavigationService::instance(), &NavigationService::requestOpenInNewTab, this, [this](const QString& url) {
        if (m_tabBar) {
            m_tabBar->openInNewTab(url);
        }
    });

    if (m_navPanel) {
        connect(m_navPanel, &NavPanel::directorySelected, &NavigationService::instance(), [](const QString& path) {
            NavigationService::instance().navigateTo(path);
        });
        connect(m_navPanel, &NavPanel::requestLocateFile, this, &NavigationCoordinator::locateItem);
        connect(m_navPanel, &NavPanel::requestOpenTrash, &NavigationService::instance(), []() {
            NavigationService::instance().navigateTo("trash://");
        });
        if (m_favoritePanel) {
            connect(m_navPanel, &NavPanel::requestAddFavorite, this, [](const QString& path) {
                FavoriteService::instance().addFavorites({path});
            });
            connect(m_navPanel, &NavPanel::requestRemoveFavorite, this, [](const QString& path) {
                FavoriteService::instance().removeFavorites({path});
            });
        }
    }

    if (m_favoritePanel) {
        connect(m_favoritePanel, &FavoritePanel::directorySelected, &NavigationService::instance(), [](const QString& path, bool recursive) {
            NavigationService::instance().navigateTo(path, true, recursive);
        });
        connect(m_favoritePanel, &FavoritePanel::requestLocateFile, this, &NavigationCoordinator::locateItem);
    }

    if (m_markedPanel) {
        connect(m_markedPanel, &MarkedPanel::requestLocateFolder, this, [](const QString& path) {
            NavigationService::instance().navigateTo(path);
        });
    }

    if (m_addressBar) {
        connect(m_addressBar, &AddressBar::pathChanged, &NavigationService::instance(), [](const QString& path) {
            NavigationService::instance().navigateTo(path);
        });
        connect(m_addressBar, &AddressBar::refreshRequested, this, &NavigationCoordinator::refreshCurrent);

        if (m_favoritePanel) {
            connect(m_addressBar, &AddressBar::requestAddFavorite, this, [](const QString& path) {
                if (FavoriteService::instance().isFavorite(path)) {
                    FavoriteService::instance().removeFavorites({path});
                } else {
                    FavoriteService::instance().addFavorites({path});
                }
            });
            connect(m_addressBar, &AddressBar::requestRemoveFavorite, this, [](const QString& path) {
                FavoriteService::instance().removeFavorites({path});
            });
        }
    }

    connect(&VolumeOnlineManager::instance(), &VolumeOnlineManager::volumeStateChanged, this,
            [](const QString& driveLetter, bool isOnline) {
        if (!isOnline) {
            QString current = NavigationService::instance().currentUrl();
            if (current.contains(driveLetter + ":", Qt::CaseInsensitive)) {
                QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
                NavigationService::instance().navigateTo(desktopPath);
            }
        }
    });

    if (m_libraryPanel) {
        connect(m_libraryPanel, &LibraryPanel::categoryPathsSelected, this, [this](const QStringList& paths, const QString& name, int categoryId) {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                target->setActivePane(true);
                target->search("");
                target->applyViewTarget(QString("library://%1").arg(categoryId), "library", name, categoryId, false);
                target->setLastLoadedLibraryPaths(paths);
                target->loadLibraryPaths(paths);
            }
            showLocation("库 → " + name, name, "library://" + QString::number(categoryId));
        });

        connect(m_libraryPanel, &LibraryPanel::smartFolderSelected, this, [this](const SmartFolderRecord& record) {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                target->setActivePane(true);
                target->loadSmartFolder(record, record.name);
            }
            showLocation("库 → " + record.name, record.name, "smartfolder://" + QString::number(record.id));
        });

        connect(m_libraryPanel, &LibraryPanel::smartFolderPreviewRequested, this, [this](const SmartFolderRecord& record) {
            QString displayName = record.name.isEmpty() ? "智能文件夹预览" : record.name;
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                target->setActivePane(true);
                target->loadSmartFolder(record, displayName);
            }
            showLocation("库 → " + displayName, displayName, "smartfolder://" + QString::number(record.id));
        });
    }

    connect(&NavigationService::instance(), &NavigationService::currentUrlChanged, this,
            [this](const QString& url, const QString& displayPath, bool recursive) {
        if (m_searchController) {
            m_searchController->clearSearch(true);
        }
        ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
        if (target) {
            target->setActivePane(true);
            target->search("");
        }

        showLocation(displayPath, displayPath, url);

        if (m_navPanel) {
            m_navPanel->selectPath(url == "computer://" ? "" : url);
        }

        if (target) {
            QString targetChild = NavigationService::instance().takePendingTargetChildPath(url);
            if (!targetChild.isEmpty() && url != "computer://" && url != "trash://") {
                target->setPendingSelectPath(targetChild, false);
            }

            if (url == "computer://") {
                target->loadDirectory("computer://");
            } else if (url == "trash://") {
                target->loadCategory("trash");
            } else {
                target->loadDirectory(url, recursive);
            }
        }
    });

    if (m_searchController) {
        connect(m_searchController, &SearchController::searchExecuted, this, [this]() {
            emit statusMessageRequested("搜索已完成");
        });
    }

    connect(&LibrarySyncService::instance(), &LibrarySyncService::libraryItemsSynced, this, [this]() {
        if (m_libraryPanel) {
            m_libraryPanel->refreshSmartFolderCounts();
        }
        ContentPanel* root = m_activeCoordinator ? m_activeCoordinator->rootPane() : nullptr;
        if (root) {
            for (ContentPanel* pane : root->panes()) {
                if (pane) {
                    pane->refreshSmartFolderIfShowing();
                }
            }
        }
    });

    connect(&CentralEventHub::instance(), &CentralEventHub::eventOccurred, this, [this](const QuarkMeta::AppEvent& event) {
        if (event.type == QuarkMeta::AppEventType::ItemsDeleted ||
            event.type == QuarkMeta::AppEventType::ItemsRenamed ||
            event.type == QuarkMeta::AppEventType::UndoRedoPerformed) {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                target->refreshAll();
            }
            ContentPanel* root = m_activeCoordinator ? m_activeCoordinator->rootPane() : nullptr;
            if (root && root != target) {
                root->refreshAll();
            }
        }
    });
}

} // namespace QuarkMeta
