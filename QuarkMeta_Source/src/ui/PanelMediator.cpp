#include "PanelMediator.h"
#include "NavPanel.h"
#include "FavoritePanel.h"
#include "LibraryPanel.h"
#include "MarkedPanel.h"
#include "../meta/HistoryService.h"
#include "../meta/LibrarySyncService.h"
#include "ContentPanel.h"
#include "ColumnViewWidget.h"
#include "controllers/ContentPaneSplitManager.h"
#include "controllers/PaneActivationTracker.h"
#include "controllers/ContextMenuFactory.h"
#include "MetaPanel.h"
#include "FilterPanel.h"
#include "AddressBar.h"
#include "SearchController.h"
#include "TitleBarWidget.h"
#include "TabBarWidget.h"
#include "PanelLayoutManager.h"
#include "AppShortcutController.h"
#include "QuickLookWindow.h"
#include "ToolTipOverlay.h"
#include "Logger.h"
#include <QApplication>
#include <QElapsedTimer>
#include "../core/NavigationService.h"
#include "../core/TrashService.h"
#include "../meta/MarkedFolderService.h"
#include "../core/CoreEngine.h"
#include "../core/CentralEventHub.h"
#include "../core/VolumeOnlineManager.h"
#include "../core/ModelContract.h"
#include "../core/AppConfig.h"
#include "../util/ShellHelper.h"
#include "../meta/MetadataManager.h"
#include "../meta/FavoriteDao.h"
#include "../meta/FavoriteService.h"
#include "../meta/LibraryDao.h"
#include "../meta/SmartFolderDao.h"
#include "UiHelper.h"
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QCursor>

namespace QuarkMeta {

static PanelMediator* s_panelMediatorInstance = nullptr;

PanelMediator* PanelMediator::instance() {
    return s_panelMediatorInstance;
}

PanelMediator::PanelMediator(const PanelMediatorComponents& components, QObject* parent)
    : QObject(parent),
      m_navPanel(components.navPanel),
      m_favoritePanel(components.favoritePanel),
      m_libraryPanel(components.libraryPanel),
      m_markedPanel(components.markedPanel),
      m_contentPanel(components.contentPanel),
      m_metaPanel(components.metaPanel),
      m_filterPanel(components.filterPanel),
      m_addressBar(components.addressBar),
      m_searchController(components.searchController),
      m_titleBar(components.titleBar),
      m_layoutManager(components.layoutManager),
      m_shortcutController(components.shortcutController) {
    s_panelMediatorInstance = this;
}

PanelMediator::~PanelMediator() {
    if (s_panelMediatorInstance == this) {
        s_panelMediatorInstance = nullptr;
    }
}

void PanelMediator::setupConnections() {
    NavPanel* navPanel = m_navPanel;

    // 🚀【Tab页在新标签中打开路由】：从 NavigationService 接收在 Tab 栏打开请求，解耦控制器下钻
    connect(&NavigationService::instance(), &NavigationService::requestOpenInNewTab, this, [this](const QString& url) {
        if (m_titleBar && m_titleBar->tabBar()) {
            m_titleBar->tabBar()->openInNewTab(url);
        }
    });
    FavoritePanel* favoritePanel = m_favoritePanel;
    LibraryPanel* libraryPanel = m_libraryPanel;
    MarkedPanel* markedPanel = m_markedPanel;
    ContentPanel* contentPanel = m_contentPanel;
    MetaPanel* metaPanel = m_metaPanel;
    FilterPanel* filterPanel = m_filterPanel;
    AddressBar* addressBar = m_addressBar;
    SearchController* searchController = m_searchController;
    TitleBarWidget* titleBar = m_titleBar;
    PanelLayoutManager* layoutManager = m_layoutManager;
    AppShortcutController* shortcutController = m_shortcutController;

    // 0. TitleBar 与各组件的高阶编排及 UI 状态恢复/持久化
    if (titleBar) {
        if (titleBar->tabBar()) {
            // 切出旧 Tab 前：快照导出旧 Tab 的完整分屏树
            connect(titleBar->tabBar(), &TabBarWidget::tabAboutToChange, this, [this, titleBar, contentPanel](int oldIndex) {
                ContentPanel* root = contentPanel ? contentPanel->rootPane() : nullptr;
                if (root && root->splitManager()) {
                    TabSplitState state = root->splitManager()->exportSplitState();
                    titleBar->tabBar()->setTabSplitState(oldIndex, state);
                }
            });

            // 切换到新 Tab：还原新 Tab 的专属分屏树并通知 NavigationService
            connect(titleBar->tabBar(), &TabBarWidget::currentTabChanged, this, [this, titleBar, contentPanel](int index, const QString& url) {
                ContentPanel* root = contentPanel ? contentPanel->rootPane() : nullptr;
                if (root && root->splitManager() && titleBar->tabBar()) {
                    TabSplitState state = titleBar->tabBar()->tabSplitState(index);
                    root->splitManager()->restoreSplitState(state);
                }
                NavigationService::instance().navigateTo(url);
            });

            connect(titleBar->tabBar(), &TabBarWidget::refreshRequested, this, [this, contentPanel]() {
                ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
                if (target && (target->getCurrentCategoryType() == "library" || target->getCurrentCategoryType() == "smart_folder")) {
                    target->refreshAll();
                } else {
                    NavigationService::instance().refresh();
                }
            });

            connect(titleBar->tabBar(), &TabBarWidget::mergeRequested, this, [this, titleBar, contentPanel](int sourceIndex, int targetIndex) {
                ContentPanel* root = contentPanel ? contentPanel->rootPane() : nullptr;
                if (root && root->splitManager() && titleBar->tabBar()) {
                    TabSplitState state = root->splitManager()->exportSplitState();
                    titleBar->tabBar()->setTabSplitState(titleBar->tabBar()->currentIndex(), state);

                    TabSplitState srcState = titleBar->tabBar()->tabSplitState(sourceIndex);
                    TabSplitState tgtState = titleBar->tabBar()->tabSplitState(targetIndex);

                    int srcCount = (srcState.isSplit && !srcState.panePaths.isEmpty()) ? srcState.panePaths.size() : 1;
                    int tgtCount = (tgtState.isSplit && !tgtState.panePaths.isEmpty()) ? tgtState.panePaths.size() : 1;

                    if (srcCount + tgtCount > ContentPanel::kMaxPanes) {
                        ToolTipOverlay::instance()->showToast(QString("窗格数量超出上限%1窗格，不支持合并").arg(ContentPanel::kMaxPanes), 2000, QColor("#e81123"));
                        return;
                    }

                    titleBar->tabBar()->mergeTab(sourceIndex, targetIndex);
                }
            });

            connect(titleBar->tabBar(), &TabBarWidget::splitRequested, this, [this, titleBar, contentPanel](int tabIndex) {
                ContentPanel* root = contentPanel ? contentPanel->rootPane() : nullptr;
                if (root && root->splitManager() && titleBar->tabBar()) {
                    TabSplitState state = root->splitManager()->exportSplitState();
                    titleBar->tabBar()->setTabSplitState(titleBar->tabBar()->currentIndex(), state);
                    titleBar->tabBar()->splitTab(tabIndex);
                }
            });

            connect(titleBar->tabBar(), &TabBarWidget::folderDroppedOnTab, this, [this, titleBar, contentPanel](int tabIndex, const QString& folderPath) {
                if (!titleBar || !titleBar->tabBar()) return;
                titleBar->tabBar()->setCurrentIndex(tabIndex, true);
                ContentPanel* root = contentPanel ? contentPanel->rootPane() : nullptr;
                if (root) {
                    if (root->paneCount() >= ContentPanel::kMaxPanes) {
                        ToolTipOverlay::instance()->showToast(QString("窗格数量超出上限%1窗格，不支持继续分屏").arg(ContentPanel::kMaxPanes), 2000, QColor("#e81123"));
                        return;
                    }
                    root->splitPane(Qt::Horizontal, folderPath);
                }
            });

            connect(titleBar->tabBar(), &TabBarWidget::tabColorChanged, this, [](int /*index*/, const QString& url, const QString& hexColor) {
                if (!url.isEmpty() && !url.startsWith("computer://")) {
                    AppCommand cmd;
                    cmd.type = AppCommandType::SetColor;
                    cmd.targetPaths = {url};
                    cmd.params["color"] = hexColor;
                    CoreEngine::instance().executeCommand(cmd);
                }
            });

            connect(titleBar->tabBar(), &TabBarWidget::tabIconChanged, this, [](int /*index*/, const QString& url, const QString& iconKey) {
                if (!url.isEmpty()) {
                    FavoriteDao::updateFavorite(url, iconKey, "");
                }
            });

        }
        if (layoutManager) {
            connect(titleBar, &TitleBarWidget::layoutMenuRequested, layoutManager, [layoutManager](const QPoint& pos) {
                layoutManager->showPanelContextMenu(pos);
            });
        }
        if (contentPanel) {
            connect(titleBar, &TitleBarWidget::viewModeRequested, this, [this, contentPanel](TitleBarWidget::ViewModeOption option) {
                ContentPanel::ViewMode targetMode = ContentPanel::GridView;
                if (option == TitleBarWidget::JustifiedViewMode) targetMode = ContentPanel::JustifiedViewMode;
                else if (option == TitleBarWidget::GridViewMode) targetMode = ContentPanel::GridView;
                else if (option == TitleBarWidget::ListViewMode) targetMode = ContentPanel::ListView;
                else if (option == TitleBarWidget::ColumnViewMode) targetMode = ContentPanel::ColumnView;

                ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
                if (target) target->setViewMode(targetMode);
            });

            connect(titleBar, &TitleBarWidget::createItemRequested, this, [this, contentPanel](const QString& type) {
                ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
                if (target) target->createNewItem(type);
            });

            // 缩放级别初始化与双向同步 + 持久化
            int initZoom = AppConfig::instance().getValue("UI/GridZoomLevel", 96).toInt();
            int boundZoom = qBound(30, initZoom, 230);
            titleBar->setZoomLevel(boundZoom);
            contentPanel->setZoomLevel(boundZoom);

            connect(titleBar, &TitleBarWidget::zoomLevelChanged, this, [this, contentPanel](int value) {
                ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
                if (target) target->setZoomLevel(value);
                AppConfig::instance().setValue("UI/GridZoomLevel", value);
            });

            connect(contentPanel, &ContentPanel::zoomLevelChanged, this, [titleBar](int level) {
                if (titleBar) titleBar->setZoomLevel(level);
                AppConfig::instance().setValue("UI/GridZoomLevel", level);
            });
        }
    }

    // 快捷键沉浸模式切换与新建文件夹下沉
    if (shortcutController) {
        if (layoutManager) {
            connect(shortcutController, &AppShortcutController::toggleImmersiveRequested, layoutManager, [layoutManager]() {
                layoutManager->toggleImmersiveMode();
            });
        }
        connect(shortcutController, &AppShortcutController::createNewFolderRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                target->createNewItem("folder");
            }
        });

        connect(shortcutController, &AppShortcutController::createNewMdRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                target->createNewItem("md");
            }
        });

        connect(shortcutController, &AppShortcutController::extractContentRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                QStringList selectedPaths = target->getSelectedPaths();
                if (!selectedPaths.isEmpty()) {
                    ContextMenuFactory::extractContentToClipboard(selectedPaths.first());
                } else {
                    ToolTipOverlay::instance()->showToast("未选择任何可提取内容的文件", 1200, QColor("#e81123"));
                }
            }
        });

        connect(shortcutController, &AppShortcutController::toggleRecursiveRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target && !target->currentPath().isEmpty() && target->currentPath() != "computer://") {
                bool nextState = !target->isRecursive();
                target->setIsRecursive(nextState);
                target->loadDirectory(target->currentPath(), nextState);
            }
        });

        if (filterPanel) {
            connect(shortcutController, &AppShortcutController::resetFilterRequested, filterPanel, [filterPanel]() {
                filterPanel->clearAllFilters(true);
            });
        }

        connect(shortcutController, &AppShortcutController::moveToTrashRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                QStringList sel = target->getSelectedPaths();
                if (!sel.isEmpty()) {
                    TrashService::instance().moveToTrash(sel, target);
                }
            }
        });

        connect(shortcutController, &AppShortcutController::toggleMarkedRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                QStringList sel = target->getSelectedPaths();
                if (!sel.isEmpty()) {
                    QuarkMeta::MarkedFolderService::instance().toggleMarked(sel);
                }
            }
        });

        connect(shortcutController, &AppShortcutController::transferToMarkedRequested, this, []() {
            QuarkMeta::MarkedFolderService::instance().executeTransferToMarked({});
        });

        connect(shortcutController, &AppShortcutController::copyNamesEnumeratedRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                QStringList sel = target->getSelectedPaths();
                if (!sel.isEmpty()) {
                    ContextMenuFactory::copyNamesToClipboard(sel, ContextMenuFactory::NameDelimiter::Enumeration, true);
                }
            }
        });
    }

    // 搜索控制器与 ContentPanel 绑定及状态更新
    if (searchController) {
        if (contentPanel) {
            searchController->bindContentPanel(contentPanel);
        }
        connect(searchController, &SearchController::searchExecuted, this, [this]() {
            emit statusMessageRequested("搜索已完成");
        });
    }

    // 1. 路径变更与导航驱动
    connect(&NavigationService::instance(), &NavigationService::currentUrlChanged, this,
            [this, contentPanel, addressBar, navPanel, filterPanel, searchController, titleBar](const QString& url, const QString& displayPath, bool recursive) {
        if (searchController) {
            searchController->clearSearch(true);
        }
        ContentPanel* targetPanel = (m_activeContentPanel && m_activeContentPanel->isVisible()) ? m_activeContentPanel.data() : contentPanel;
        if (targetPanel) {
            targetPanel->search("");
        }
        if (filterPanel) {
            filterPanel->clearAllFilters();
            filterPanel->clearStats();
            filterPanel->setMirrorSource(false);
        }

        if (addressBar) addressBar->setPath(displayPath);
        if (navPanel) navPanel->selectPath(url == "computer://" ? "" : url);
        if (titleBar && titleBar->tabBar()) {
            titleBar->tabBar()->updateCurrentTabTitle(displayPath, url);
        }

        if (targetPanel) {
            QString goUpChild = NavigationService::instance().takePendingGoUpChildPath();
            if (!goUpChild.isEmpty() && url != "computer://" && url != "trash://") {
                targetPanel->setPendingSelectPath(goUpChild, false);
            }

            if (url == "computer://") {
                targetPanel->loadDirectory("computer://");
            } else if (url == "trash://") {
                targetPanel->loadCategory("trash");
            } else {
                targetPanel->loadDirectory(url, recursive);
            }
        }
    });

    if (navPanel) {
        connect(navPanel, &NavPanel::directorySelected, &NavigationService::instance(), [](const QString& path) {
            NavigationService::instance().navigateTo(path);
        });

        connect(navPanel, &NavPanel::requestLocateFile, this, [this, contentPanel](const QString& path) {
            ContentPanel* target = (m_activeContentPanel && m_activeContentPanel->isVisible()) ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                target->setPendingSelectPath(path, false);
            }
            NavigationService::instance().navigateTo(QFileInfo(path).absolutePath());
        });

        connect(navPanel, &NavPanel::requestOpenTrash, &NavigationService::instance(), []() {
            NavigationService::instance().navigateTo("trash://");
        });

        if (favoritePanel) {
            connect(navPanel, &NavPanel::requestAddFavorite, favoritePanel, [favoritePanel](const QString& path) {
                if (FavoriteService::instance().isFavorite(path)) {
                    ToolTipOverlay::instance()->showToast("该文件夹已在收藏夹中，请勿重复添加", 2000, QColor("#e81123"));
                } else {
                    favoritePanel->addFavoriteItem(path);
                    favoritePanel->saveFavorites();
                    ToolTipOverlay::instance()->showToast("已成功添加至收藏夹", 1500, QColor("#2ecc71"));
                }
            });
            connect(navPanel, &NavPanel::requestRemoveFavorite, favoritePanel, [favoritePanel](const QString& path) {
                favoritePanel->removeFavoriteItem(path);
                favoritePanel->saveFavorites();
                ToolTipOverlay::instance()->showToast("已从收藏夹移除", 1500, QColor("#e74c3c"));
            });
        }
    }

    if (favoritePanel) {
        connect(favoritePanel, &FavoritePanel::directorySelected, &NavigationService::instance(), [](const QString& path, bool recursive) {
            NavigationService::instance().navigateTo(path, true, recursive);
        });

        connect(favoritePanel, &FavoritePanel::requestLocateFile, this, [this, contentPanel](const QString& path) {
            ContentPanel* target = (m_activeContentPanel && m_activeContentPanel->isVisible()) ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                target->setPendingSelectPath(path, false);
            }
            NavigationService::instance().navigateTo(QFileInfo(path).absolutePath());
        });
    }

        if (markedPanel) {
            connect(markedPanel, &MarkedPanel::requestLocateFolder, this, [](const QString& path) {
                NavigationService::instance().navigateTo(path);
            });
        }

        if (libraryPanel) {
            connect(libraryPanel, &LibraryPanel::categoryPathsSelected, this, [this, contentPanel, filterPanel, addressBar, titleBar](const QStringList& paths, const QString& name, int categoryId) {
                ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
                if (filterPanel) {
                    filterPanel->clearAllFilters(true);
                }
                if (target) {
                    target->search("");
                    target->applyViewTarget(QString("library://%1").arg(categoryId), "library", name, categoryId, false);
                    target->setLastLoadedLibraryPaths(paths);
                    target->loadLibraryPaths(paths);
                }
                if (addressBar) {
                    addressBar->setPath("库 → " + name);
                }
                if (titleBar && titleBar->tabBar()) {
                    titleBar->tabBar()->updateCurrentTabTitle(name, "library://" + QString::number(categoryId));
                }
            });

            connect(libraryPanel, &LibraryPanel::smartFolderSelected, this, [this, contentPanel, filterPanel, addressBar, titleBar](const SmartFolderRecord& record) {
                ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
                if (target) {
                    target->loadSmartFolder(record, record.name);
                }
                if (addressBar) {
                    addressBar->setPath("库 → " + record.name);
                }
                if (filterPanel) {
                    filterPanel->clearAllFilters(false);
                }
                if (titleBar && titleBar->tabBar()) {
                    titleBar->tabBar()->updateCurrentTabTitle(record.name, "smartfolder://" + QString::number(record.id));
                }
            });

            connect(libraryPanel, &LibraryPanel::smartFolderPreviewRequested, this, [this, contentPanel, filterPanel, addressBar, titleBar](const SmartFolderRecord& record) {
                QString displayName = record.name.isEmpty() ? "智能文件夹预览" : record.name;
                ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
                if (target) {
                    target->loadSmartFolder(record, displayName);
                }
                if (addressBar) {
                    addressBar->setPath("库 → " + displayName);
                }
                if (filterPanel) {
                    filterPanel->clearAllFilters(false);
                }
                if (titleBar && titleBar->tabBar()) {
                    titleBar->tabBar()->updateCurrentTabTitle(displayName, "smartfolder://" + QString::number(record.id));
                }
            });
        }

    if (contentPanel) {
        connect(contentPanel, &ContentPanel::directorySelected, &NavigationService::instance(), [](const QString& path) {
            NavigationService::instance().navigateTo(path);
        });

        ContentPanel* rootPanel = contentPanel ? contentPanel->rootPane() : nullptr;
        if (rootPanel && rootPanel->splitManager()) {
            connect(rootPanel->splitManager(), &ContentPaneSplitManager::layoutChanged, this, [this, titleBar, rootPanel]() {
                if (titleBar && titleBar->tabBar()) {
                    TabSplitState state = rootPanel->splitManager()->exportSplitState();
                    titleBar->tabBar()->updateSplitTabTitle(state);
                }
            });
        }

        if (contentPanel->columnView()) {
            connect(contentPanel->columnView(), &ColumnViewWidget::pathNavigated, this, [filterPanel](const QString& path) {
                if (filterPanel) {
                    filterPanel->clearAllFilters(false);
                }
                NavigationService::instance().navigateTo(path);
            });
        }

        if (favoritePanel) {
            connect(contentPanel, &ContentPanel::requestAddFavorite, favoritePanel, [favoritePanel](const QStringList& paths) {
                for (const QString& p : paths) {
                    favoritePanel->addFavoriteItem(p);
                }
                favoritePanel->saveFavorites();
            });
            connect(contentPanel, &ContentPanel::requestRemoveFavorite, favoritePanel, [favoritePanel](const QStringList& paths) {
                for (const QString& p : paths) {
                    favoritePanel->removeFavoriteItem(p);
                }
                favoritePanel->saveFavorites();
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

    // 安装全应用窗格激活事件过滤器
    if (!m_activationTracker) {
        m_activationTracker = new PaneActivationTracker(this);
        qApp->installEventFilter(m_activationTracker);

        connect(m_activationTracker, &PaneActivationTracker::paneInteracted, this, [this](ContentPanel* panel) {
            if (panel && m_activeContentPanel != panel) {
                if (m_activeContentPanel) {
                    m_activeContentPanel->setActivePane(false);
                }
                m_activeContentPanel = panel;
                m_activeContentPanel->setActivePane(true);
                emit activeContentPanelChanged(m_activeContentPanel);
            }
        });
    }

    // 2. 内容面板 (主/副窗格) 统一全功能绑定
    m_activeContentPanel = contentPanel;

    auto updateMetaPanelFromPanel = [metaPanel](ContentPanel* panel) {
        if (!panel || !metaPanel) return;
        QStringList paths = panel->getSelectedPaths();
        metaPanel->setSelectedPaths(paths);

        if (paths.isEmpty()) {
            metaPanel->setImagePreview(QPixmap());
            metaPanel->updateInfo("-", "-", "-", "-", "-", "-", "-", false, 0, 0);
            metaPanel->setRating(0, false);
            metaPanel->setColor(QString(""), false);
            metaPanel->setTags(QStringList());
            metaPanel->setNote(QString(""));
            metaPanel->setURL(QString(""));
            metaPanel->setPalettes({});
        } else if (paths.size() == 1) {
            QModelIndexList selectedIndices = panel->getSelectedIndexes();
            QModelIndex idx = selectedIndices.isEmpty() ? QModelIndex() : selectedIndices.first();

            QString path = paths.first();
            QFileInfo fi(path);

            QString name = idx.isValid() ? idx.sibling(idx.row(), static_cast<int>(FileListColumn::Name)).data(Qt::DisplayRole).toString() : fi.fileName();
            QString type = idx.isValid() ? ((idx.data(TypeRole).toString() == "folder") ? "文件夹" : idx.sibling(idx.row(), static_cast<int>(FileListColumn::Type)).data(Qt::DisplayRole).toString() + " 文件") : (fi.isDir() ? "文件夹" : fi.suffix().toUpper() + " 文件");
            QString sizeStr = idx.isValid() ? idx.sibling(idx.row(), static_cast<int>(FileListColumn::Size)).data(Qt::DisplayRole).toString() : "-";
            QString mtimeStr = idx.isValid() ? idx.sibling(idx.row(), static_cast<int>(FileListColumn::ModifiedDate)).data(Qt::DisplayRole).toString() : "-";
            bool encrypted = idx.isValid() ? idx.data(EncryptedRole).toBool() : false;

            auto meta = MetadataManager::instance().getMeta(path.toStdWString());

            metaPanel->updateInfo(
                name, type, sizeStr, "-", mtimeStr, "-",
                path, encrypted, meta.width, meta.height
            );

            QVector<QPair<QColor, float>> qPalettes;
            qPalettes.reserve(static_cast<int>(meta.palettes.size()));
            for (const auto& entry : meta.palettes) {
                qPalettes.append(qMakePair(entry.color, entry.ratio));
            }

            if (idx.isValid()) {
                int rating = idx.data(RatingRole).toInt();
                QString color = idx.data(ColorRole).toString();
                QStringList tags = idx.data(TagsRole).toStringList();
                QString note = idx.data(NoteRole).toString();
                QString url = idx.data(UrlRole).toString();

                int finalRating = rating > 0 ? rating : meta.rating;
                QString finalColor = !color.isEmpty() ? color : QString::fromStdWString(meta.manualColor);
                QStringList finalTagsList = !tags.isEmpty() ? tags : meta.tags;
                QString finalNote = !note.isEmpty() ? note : QString::fromStdWString(meta.note);
                QString finalUrl = !url.isEmpty() ? url : QString::fromStdWString(meta.url);

                metaPanel->setRating(finalRating, false);
                metaPanel->setColor(finalColor, false);
                metaPanel->setTags(finalTagsList);
                metaPanel->setNote(finalNote);
                metaPanel->setURL(finalUrl);
                metaPanel->setPalettes(qPalettes);

                QVariant decData = idx.data(Qt::DecorationRole);
                bool hasThumb = idx.data(HasThumbnailRole).toBool();
                if (hasThumb) {
                    QPixmap previewPixmap;
                    if (decData.canConvert<QIcon>()) {
                        previewPixmap = decData.value<QIcon>().pixmap(128, 128);
                    } else if (decData.canConvert<QPixmap>()) {
                        previewPixmap = decData.value<QPixmap>();
                    }
                    metaPanel->setImagePreview(previewPixmap);
                } else {
                    if (decData.canConvert<QIcon>()) {
                        metaPanel->setDefaultIconPreview(decData.value<QIcon>());
                    } else {
                        metaPanel->setImagePreview(QPixmap());
                    }
                }
            } else {
                metaPanel->setRating(meta.rating, false);
                metaPanel->setColor(QString::fromStdWString(meta.manualColor), false);
                metaPanel->setTags(meta.tags);
                metaPanel->setNote(QString::fromStdWString(meta.note));
                metaPanel->setURL(QString::fromStdWString(meta.url));
                metaPanel->setPalettes(qPalettes);
                metaPanel->setImagePreview(QPixmap());
            }
        } else if (paths.size() > 1) {
            QModelIndexList selectedIndices = panel->getSelectedIndexes();
            QMap<QString, QModelIndex> pathToIndexMap;
            for (const QModelIndex& idx : selectedIndices) {
                if (idx.isValid()) {
                    QString p = idx.data(PathRole).toString();
                    if (!p.isEmpty()) {
                        pathToIndexMap[p] = idx;
                    }
                }
            }

            bool firstItem = true;
            bool sameRating = true;
            bool sameColor = true;
            bool sameNote = true;
            bool sameUrl = true;

            int commonRating = 0;
            QString commonColor;
            QString commonNote;
            QString commonUrl;

            QSet<QString> commonTags;
            bool commonTagsInitialized = false;

            QList<MetaPreviewItem> multiPreviewItems;
            int previewStartIdx = qMax(0, paths.size() - MetaPanel::kMultiPreviewMaxCards);

            for (int i = 0; i < paths.size(); ++i) {
                const QString& path = paths[i];
                QModelIndex idx = pathToIndexMap.value(path, QModelIndex());

                int itemRating = 0;
                QString itemColor;
                QStringList itemTags;
                QString itemNote;
                QString itemUrl;

                auto meta = MetadataManager::instance().getMeta(path.toStdWString());

                if (idx.isValid()) {
                    int r = idx.data(RatingRole).toInt();
                    QString c = idx.data(ColorRole).toString();
                    QStringList t = idx.data(TagsRole).toStringList();
                    QString n = idx.data(NoteRole).toString();
                    QString u = idx.data(UrlRole).toString();

                    itemRating = r > 0 ? r : meta.rating;
                    itemColor = !c.isEmpty() ? c : QString::fromStdWString(meta.manualColor);
                    itemTags = !t.isEmpty() ? t : meta.tags;
                    itemNote = !n.isEmpty() ? n : QString::fromStdWString(meta.note);
                    itemUrl = !u.isEmpty() ? u : QString::fromStdWString(meta.url);

                    if (i >= previewStartIdx && multiPreviewItems.size() < MetaPanel::kMultiPreviewMaxCards) {
                        MetaPreviewItem item;
                        QVariant decData = idx.data(Qt::DecorationRole);
                        bool hasThumb = idx.data(HasThumbnailRole).toBool();
                        if (hasThumb) {
                            item.hasThumbnail = true;
                            if (decData.canConvert<QIcon>()) {
                                item.thumbnail = decData.value<QIcon>().pixmap(128, 128);
                            } else if (decData.canConvert<QPixmap>()) {
                                item.thumbnail = decData.value<QPixmap>();
                            }
                        } else {
                            item.hasThumbnail = false;
                            if (decData.canConvert<QIcon>()) {
                                item.icon = decData.value<QIcon>();
                            }
                        }
                        if ((item.hasThumbnail && !item.thumbnail.isNull()) || (!item.hasThumbnail && !item.icon.isNull())) {
                            multiPreviewItems.append(item);
                        }
                    }
                } else {
                    itemRating = meta.rating;
                    itemColor = QString::fromStdWString(meta.manualColor);
                    itemTags = meta.tags;
                    itemNote = QString::fromStdWString(meta.note);
                    itemUrl = QString::fromStdWString(meta.url);

                    if (i >= previewStartIdx && multiPreviewItems.size() < MetaPanel::kMultiPreviewMaxCards) {
                        MetaPreviewItem item;
                        item.hasThumbnail = false;
                        QFileInfo fi(path);
                        if (fi.isDir()) {
                            item.icon = UiHelper::getIcon("folder", QColor("#3498db"), 128);
                        } else {
                            item.icon = UiHelper::getIcon("file_eps", QColor("#888888"), 128);
                        }
                        if (!item.icon.isNull()) {
                            multiPreviewItems.append(item);
                        }
                    }
                }

                if (firstItem) {
                    firstItem = false;
                    commonRating = itemRating;
                    commonColor = itemColor;
                    commonNote = itemNote;
                    commonUrl = itemUrl;

                    commonTags = QSet<QString>(itemTags.begin(), itemTags.end());
                    commonTagsInitialized = true;
                } else {
                    if (sameRating && itemRating != commonRating) sameRating = false;
                    if (sameColor && itemColor.compare(commonColor, Qt::CaseInsensitive) != 0) sameColor = false;
                    if (sameNote && itemNote != commonNote) sameNote = false;
                    if (sameUrl && itemUrl != commonUrl) sameUrl = false;

                    if (commonTagsInitialized) {
                        QSet<QString> currentSet(itemTags.begin(), itemTags.end());
                        commonTags.intersect(currentSet);
                    }
                }
            }

            metaPanel->setRating(sameRating ? commonRating : 0, false);
            metaPanel->setColor(sameColor ? commonColor : QString(""), false);
            metaPanel->setNote(sameNote ? commonNote : QString(""));
            metaPanel->setURL(sameUrl ? commonUrl : QString(""));
            metaPanel->setTags(commonTags.values());

            metaPanel->setPalettes({});
            metaPanel->setMultiSelectionPreview(multiPreviewItems);
        }
    };

    auto wireContentPanel = std::make_shared<std::function<void(ContentPanel*)>>();
    *wireContentPanel = [this, addressBar, favoritePanel, metaPanel, updateMetaPanelFromPanel, wireContentPanel](ContentPanel* panel) {
        if (!panel) return;

        // 1. 文件双击/空格/回车/右键菜单统一文件激活动作
        connect(panel, &ContentPanel::fileActivated, this, [this](const QString& path) {
            if (path.isEmpty() || !QFile::exists(path)) {
                return;
            }
            // 无论能否在 QuickLook 中预览，凡是被双击/空格/回车尝试激活的文件全量记录历史
            HistoryService::instance().recordFileAccess(path);

            if (!UiHelper::canPreviewFile(path)) {
                return;
            }

            AppCommand cmd;
            cmd.type = AppCommandType::RecordAccess;
            cmd.targetPaths << path;
            CoreEngine::instance().executeCommand(cmd);

            QuickLookWindow::instance().preview(path);
        });

        // 2. 收藏夹请求
        if (favoritePanel) {
            connect(panel, &ContentPanel::requestAddFavorite, favoritePanel, [favoritePanel](const QStringList& paths) {
                int addedCount = 0;
                int duplicateCount = 0;
                for (const QString& p : paths) {
                    if (FavoriteService::instance().isFavorite(p)) {
                        duplicateCount++;
                    } else {
                        favoritePanel->addFavoriteItem(p);
                        addedCount++;
                    }
                }
                if (addedCount > 0) {
                    favoritePanel->saveFavorites();
                    if (duplicateCount == 0) {
                        ToolTipOverlay::instance()->showToast("已成功添加至收藏夹", 1500, QColor("#2ecc71"));
                    } else {
                        ToolTipOverlay::instance()->showToast(QString("已添加 %1 个项目 (其余 %2 个已在收藏夹中)").arg(addedCount).arg(duplicateCount), 2000, QColor("#3498db"));
                    }
                } else if (duplicateCount > 0) {
                    ToolTipOverlay::instance()->showToast("该文件夹已在收藏夹中，请勿重复添加", 2000, QColor("#e81123"));
                }
            });
            connect(panel, &ContentPanel::requestRemoveFavorite, favoritePanel, [favoritePanel](const QStringList& paths) {
                for (const QString& p : paths) {
                    favoritePanel->removeFavoriteItem(p);
                }
                favoritePanel->saveFavorites();
                ToolTipOverlay::instance()->showToast("已从收藏夹移除", 1500, QColor("#e74c3c"));
            });
        }

        // 3. 状态栏与焦点响应 (仅当信息来自当前激活窗格时才刷到底栏)
        connect(panel, &ContentPanel::statusBarMessageReady, this, [this, panel](const QString& message) {
            if (m_activeContentPanel == panel || (!m_activeContentPanel && panel == m_contentPanel.data())) {
                emit statusMessageRequested(message);
            }
        });

        connect(panel, &ContentPanel::directorySelected, &NavigationService::instance(), [](const QString& path) {
            NavigationService::instance().navigateTo(path);
        });

        connect(panel, &ContentPanel::panelActivated, this, [this, addressBar](ContentPanel* activePanel) {
            if (m_activeContentPanel != activePanel) {
                m_activeContentPanel = activePanel;
                emit activeContentPanelChanged(activePanel);
            }
            if (addressBar) {
                addressBar->setPath(activePanel->displayPath());
            }
        });

        // 4. 元数据面板选中项 & 模型数据变动同步
        if (metaPanel) {
            connect(panel, &ContentPanel::selectionChanged, metaPanel, [this, panel, updateMetaPanelFromPanel](const QStringList&) {
                if (m_activeContentPanel == panel || (!m_activeContentPanel && panel == m_contentPanel.data())) {
                    updateMetaPanelFromPanel(panel);
                }
            });

            if (panel->model()) {
                connect(panel->model(), &QAbstractItemModel::dataChanged, metaPanel,
                        [this, panel, updateMetaPanelFromPanel](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
                    if (!roles.isEmpty() && !roles.contains(RatingRole) && !roles.contains(ColorRole) && !roles.contains(TagsRole) && !roles.contains(NoteRole) && !roles.contains(UrlRole)) {
                        return;
                    }

                    auto& debounceState = m_panelMetaDebounceMap[panel];
                    if (!debounceState.timer) {
                        debounceState.timer = new QTimer(panel);
                        debounceState.timer->setSingleShot(true);
                        connect(panel, &QObject::destroyed, this, [this, panel]() {
                            m_panelMetaDebounceMap.remove(panel);
                        });
                        connect(debounceState.timer, &QTimer::timeout, this, [this, panel, updateMetaPanelFromPanel]() {
                            auto it = m_panelMetaDebounceMap.find(panel);
                            if (it == m_panelMetaDebounceMap.end()) return;

                            ContentPanel* activeP = m_activeContentPanel ? m_activeContentPanel.data() : m_contentPanel.data();
                            if (activeP == panel) {
                                QSet<QString> selSet;
                                for (const QString& sp : panel->getSelectedPaths()) {
                                    selSet.insert(QDir::cleanPath(sp).toLower());
                                }

                                bool hasOverlap = selSet.isEmpty();
                                if (!hasOverlap) {
                                    for (const QString& cp : it->pendingCleanPaths) {
                                        if (selSet.contains(cp)) {
                                            hasOverlap = true;
                                            break;
                                        }
                                    }
                                }

                                if (hasOverlap) {
                                    updateMetaPanelFromPanel(panel);
                                }
                            }
                            it->pendingCleanPaths.clear();
                        });
                    }

                    QAbstractItemModel* model = panel->model();
                    int startRow = topLeft.row();
                    int endRow = bottomRight.row();
                    for (int r = startRow; r <= endRow; ++r) {
                        QModelIndex idx = model->index(r, 0, topLeft.parent());
                        if (idx.isValid()) {
                            QString path = idx.data(PathRole).toString();
                            if (!path.isEmpty()) {
                                debounceState.pendingCleanPaths.insert(QDir::cleanPath(path).toLower());
                            }
                        }
                    }

                    if (!debounceState.timer->isActive()) {
                        debounceState.timer->start(30);
                    }
                });
            }
        }

        // 5. 递归绑定动态创建的副窗格
        connect(panel, &ContentPanel::secondaryPaneCreated, this, [wireContentPanel](ContentPanel* pane) {
            if (*wireContentPanel) {
                (*wireContentPanel)(pane);
            }
        });
    };

    if (contentPanel) {
        (*wireContentPanel)(contentPanel);

        if (searchController) {
            connect(this, &PanelMediator::activeContentPanelChanged, this, [searchController](ContentPanel* activePanel) {
                searchController->setActiveContentPanel(activePanel);
            });
        }

        connect(&LibrarySyncService::instance(), &LibrarySyncService::libraryItemsSynced, this, [this, libraryPanel, contentPanel]() {
            if (libraryPanel) {
                libraryPanel->refreshSmartFolderCounts();
            }
            if (contentPanel) {
                ContentPanel* root = contentPanel->rootPane();
                if (root) {
                    for (ContentPanel* pane : root->panes()) {
                        if (pane) {
                            pane->refreshSmartFolderIfShowing();
                        }
                    }
                }
            }
        });

        if (metaPanel) {
            connect(this, &PanelMediator::activeContentPanelChanged, this, [contentPanel, updateMetaPanelFromPanel](ContentPanel* activePanel) {
                ContentPanel* root = contentPanel ? contentPanel->rootPane() : nullptr;
                if (root) {
                    root->setActivePane(root == activePanel);
                    for (ContentPanel* pane : root->panes()) {
                        if (pane && pane != root) {
                            pane->setActivePane(pane == activePanel);
                        }
                    }
                }
                updateMetaPanelFromPanel(activePanel);
            });
        }
    }

    connect(&QuickLookWindow::instance(), &QuickLookWindow::prevRequested, this, [this, contentPanel]() {
        ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
        if (!target) return;
        QString curPath = QuickLookWindow::instance().currentPath();
        int delta = -1;
        while (true) {
            QString prev = target->getAdjacentFilePath(curPath, delta);
            if (prev.isEmpty()) break;
            if (!QFileInfo(prev).isDir() && UiHelper::canPreviewFile(prev)) {
                QuickLookWindow::instance().preview(prev);
                target->selectAndScrollToPath(prev);
                break;
            }
            delta--;
        }
    });

    connect(&QuickLookWindow::instance(), &QuickLookWindow::nextRequested, this, [this, contentPanel]() {
        ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
        if (!target) return;
        QString curPath = QuickLookWindow::instance().currentPath();
        int delta = 1;
        while (true) {
            QString next = target->getAdjacentFilePath(curPath, delta);
            if (next.isEmpty()) break;
            if (!QFileInfo(next).isDir() && UiHelper::canPreviewFile(next)) {
                QuickLookWindow::instance().preview(next);
                target->selectAndScrollToPath(next);
                break;
            }
            delta++;
        }
    });

    // QuickLook 改星级 -> 同步更新内容面板卡片并弹出 Toast 提示
    connect(&QuickLookWindow::instance(), &QuickLookWindow::ratingRequested, this, [this, metaPanel](int rating) {
        QString curPath = QuickLookWindow::instance().currentPath();
        if (curPath.isEmpty()) return;

        AppCommand cmd;
        cmd.type = AppCommandType::SetRating;
        cmd.targetPaths << curPath;
        cmd.params["rating"] = rating;
        CoreEngine::instance().executeCommand(cmd);

        if (metaPanel) metaPanel->setRating(rating, false);

        ToolTipOverlay::instance()->showRatingToast(
            rating, &QuickLookWindow::instance()
        );
    });

    // QuickLook 改颜色 -> 同步更新内容面板卡片并弹出 Toast 提示
    connect(&QuickLookWindow::instance(), &QuickLookWindow::colorRequested, this, [this, metaPanel](const QString& color) {
        QString curPath = QuickLookWindow::instance().currentPath();
        if (curPath.isEmpty()) return;

        AppCommand cmd;
        cmd.type = AppCommandType::SetColor;
        cmd.targetPaths << curPath;
        cmd.params["color"] = color;
        CoreEngine::instance().executeCommand(cmd);

        if (metaPanel) metaPanel->setColor(color, false);

        ToolTipOverlay::instance()->showColorToast(
            color, &QuickLookWindow::instance()
        );
    });

    connect(&QuickLookWindow::instance(), &QuickLookWindow::deleteRequested, this, [this, contentPanel](const QString& path) {
        if (path.isEmpty()) return;
        ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
        if (!target) return;

        // 提前计算下一个有效文件（若末项则计算备选的上一个有效文件），防止物理删除后寻址失败
        QString nextTarget;

        int delta = 1;
        while (true) {
            QString candidate = target->getAdjacentFilePath(path, delta);
            if (candidate.isEmpty()) break;
            if (!QFileInfo(candidate).isDir() && UiHelper::canPreviewFile(candidate)) {
                nextTarget = candidate;
                break;
            }
            delta++;
        }

        if (nextTarget.isEmpty()) {
            delta = -1;
            while (true) {
                QString candidate = target->getAdjacentFilePath(path, delta);
                if (candidate.isEmpty()) break;
                if (!QFileInfo(candidate).isDir() && UiHelper::canPreviewFile(candidate)) {
                    nextTarget = candidate;
                    break;
                }
                delta--;
            }
        }

        // 锁定 QuickLook 失焦关闭门禁，防止刷新与对话框导致预览被误关
        QuickLookWindow::instance().setIgnoreDeactivate(true);

        bool deleteOk = TrashService::instance().moveToTrash({path}, target);

        if (deleteOk) {
            target->refreshAll();

            if (!nextTarget.isEmpty() && QFile::exists(nextTarget)) {
                QuickLookWindow::instance().preview(nextTarget);
                target->selectAndScrollToPath(nextTarget);
            } else {
                QuickLookWindow::instance().closePreview();
            }
        }

        QTimer::singleShot(200, &QuickLookWindow::instance(), []() {
            QuickLookWindow::instance().setIgnoreDeactivate(false);
        });
    });

    // 4. 统计与过滤联动 (动态支持多分栏焦点切换)
    if (filterPanel) {
        // 用于管理当前绑定面板的连接断开，防止后台窗格发送统计冲刷侧边栏
        auto activeStatsConn = std::make_shared<QMetaObject::Connection>();

        auto bindFilterToActivePanel = [this, filterPanel, activeStatsConn](ContentPanel* activePanel) {
            if (!activePanel) return;

            // 1. 严格解绑上一个窗格的统计信号，杜绝后台非焦点窗格冲刷侧边栏
            if (*activeStatsConn) {
                QObject::disconnect(*activeStatsConn);
            }

            // 2. 绑定新焦点窗格的统计就绪信号
            *activeStatsConn = connect(activePanel, &ContentPanel::directoryStatsReady, filterPanel, [filterPanel](const ScanStats& stats) {
                filterPanel->populateStats(stats);
                AppEvent ev;
                ev.type = AppEventType::FilterStateChanged;
                CentralEventHub::instance().publishEvent(ev);
            });

            // 3. 切换焦点的瞬间：反向刷出新焦点窗格已有的 FilterState 与统计数据给 FilterPanel
            filterPanel->blockSignals(true);
            filterPanel->syncUIFromFilterState();
            filterPanel->blockSignals(false);

            // 触发当前焦点窗格重新计算并广播统计
            activePanel->recalculateAndEmitStats();
        };

        // 绑定初始主面板
        if (contentPanel) {
            bindFilterToActivePanel(contentPanel);
        }

        // 焦点分栏切换时动态绑定并同步刷出状态
        connect(this, &PanelMediator::activeContentPanelChanged, this, [this, filterPanel, bindFilterToActivePanel](ContentPanel* newActivePanel) {
            if (newActivePanel) {
                bindFilterToActivePanel(newActivePanel);
            }
        });

        // 筛选条件变动时精准应用至当前激活分栏
        connect(filterPanel, &FilterPanel::filterChanged, this, [this](const FilterState& state) {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : m_contentPanel.data();
            if (target) {
                target->applyFilters(state);
            }
        });
    }

    // 5. 地址栏路径跳转与刷新
    if (addressBar) {
        connect(addressBar, &AddressBar::pathChanged, &NavigationService::instance(), [](const QString& path) {
            NavigationService::instance().navigateTo(path);
        });

        connect(addressBar, &AddressBar::refreshRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target && (target->getCurrentCategoryType() == "library" || target->getCurrentCategoryType() == "smart_folder")) {
                target->refreshAll();
            } else {
                NavigationService::instance().refresh();
            }
        });

        if (favoritePanel) {
            connect(addressBar, &AddressBar::requestAddFavorite, favoritePanel, [favoritePanel](const QString& path) {
                if (favoritePanel->containsPath(path)) {
                    favoritePanel->removeFavoriteItem(path);
                    favoritePanel->saveFavorites();
                    ToolTipOverlay::instance()->showToast("已从收藏夹移除", 1500, QColor("#e74c3c"));
                } else {
                    favoritePanel->addFavoriteItem(path);
                    favoritePanel->saveFavorites();
                    ToolTipOverlay::instance()->showToast("已成功添加至收藏夹", 1500, QColor("#2ecc71"));
                }
            });
            connect(addressBar, &AddressBar::requestRemoveFavorite, favoritePanel, [favoritePanel](const QString& path) {
                favoritePanel->removeFavoriteItem(path);
                favoritePanel->saveFavorites();
                ToolTipOverlay::instance()->showToast("已从收藏夹移除", 1500, QColor("#e74c3c"));
            });
        }
    }

    // 6. 响应元数据面板解耦信号 -> 驱动 CoreEngine 与当前激活 ContentPanel 同步
    if (metaPanel && contentPanel) {
        auto activeOrRootPanel = [this, contentPanel]() -> ContentPanel* {
            return m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
        };

        connect(metaPanel, &MetaPanel::ratingChanged, this, [](const QStringList& paths, int rating) {
            if (paths.isEmpty()) return;
            AppCommand cmd;
            cmd.type = AppCommandType::SetRating;
            cmd.targetPaths = paths;
            cmd.params["rating"] = rating;
            CoreEngine::instance().executeCommand(cmd);
        });

        connect(metaPanel, &MetaPanel::colorChanged, this, [](const QStringList& paths, const QString& hexColor) {
            if (paths.isEmpty()) return;
            AppCommand cmd;
            cmd.type = AppCommandType::SetColor;
            cmd.targetPaths = paths;
            cmd.params["color"] = hexColor;
            CoreEngine::instance().executeCommand(cmd);
        });

        connect(metaPanel, &MetaPanel::primaryColorChanged, this, [](const QString& path, const QColor& color) {
            if (path.isEmpty()) return;
            AppCommand cmd;
            cmd.type = AppCommandType::SetColor;
            cmd.targetPaths = {path};
            cmd.params["color"] = color.name(QColor::HexRgb);
            CoreEngine::instance().executeCommand(cmd);
        });

        connect(metaPanel, &MetaPanel::tagsAddRequested, this, [](const QStringList& paths, const QStringList& newTags) {
            if (!paths.isEmpty() && !newTags.isEmpty()) {
                if (paths.size() > 50) {
                    CoreEngine::instance().addTagsToPathsAsync(paths, newTags, nullptr);
                } else {
                    AppCommand cmd;
                    cmd.type = AppCommandType::AddTag;
                    cmd.targetPaths = paths;
                    cmd.params["tags"] = newTags;
                    CoreEngine::instance().executeCommand(cmd);
                }
            }
        });

        connect(metaPanel, &MetaPanel::tagsRemoveRequested, this, [](const QStringList& paths, const QStringList& removeTags) {
            if (!paths.isEmpty() && !removeTags.isEmpty()) {
                AppCommand cmd;
                cmd.type = AppCommandType::RemoveTag;
                cmd.targetPaths = paths;
                cmd.params["tags"] = removeTags;
                CoreEngine::instance().executeCommand(cmd);
            }
        });

        if (filterPanel) {
            connect(metaPanel, &MetaPanel::searchByColor, filterPanel, [filterPanel](const QColor& color) {
                filterPanel->selectColor(color);
            });
        }

        connect(metaPanel, &MetaPanel::renameRequested, this, [activeOrRootPanel](const QString& oldPath, const QString& newPath) {
            ContentPanel* target = activeOrRootPanel();
            if (ShellHelper::renameItem(oldPath, newPath)) {
                if (target) {
                    target->migrateModelCache(oldPath, newPath);
                    target->refreshAll();
                }
            } else {
                if (target) {
                    target->updateItemMetadata(oldPath);
                }
            }
        });

        connect(metaPanel, &MetaPanel::noteEdited, this, [](const QStringList& paths, const QString& newNote) {
            if (!paths.isEmpty()) {
                AppCommand cmd;
                cmd.type = AppCommandType::SetNote;
                cmd.targetPaths = paths;
                cmd.params["note"] = newNote;
                CoreEngine::instance().executeCommand(cmd);
            }
        });

        connect(metaPanel, &MetaPanel::linkEdited, this, [](const QStringList& paths, const QString& newLink) {
            if (!paths.isEmpty()) {
                AppCommand cmd;
                cmd.type = AppCommandType::SetURL;
                cmd.targetPaths = paths;
                cmd.params["url"] = newLink;
                CoreEngine::instance().executeCommand(cmd);
            }
        });
    }

    // 7. 全局事件总线 CentralEventHub 增量通知响应
    connect(&CentralEventHub::instance(), &CentralEventHub::eventOccurred, this, [this, contentPanel, metaPanel](const QuarkMeta::AppEvent& event) {
        ContentPanel* activeOrRoot = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
        if (!activeOrRoot) return;

        if (event.type == QuarkMeta::AppEventType::MetadataUpdated) {
            if (!event.targetPath.isEmpty()) {
                activeOrRoot->updateItemMetadata(event.targetPath);
                if (contentPanel && contentPanel != activeOrRoot) {
                    contentPanel->updateItemMetadata(event.targetPath);
                }
                if (metaPanel) {
                    QString targetClean = QDir::cleanPath(event.targetPath);
                    for (const QString& p : activeOrRoot->getSelectedPaths()) {
                        if (QString::compare(QDir::cleanPath(p), targetClean, Qt::CaseInsensitive) == 0) {
                            if (event.payload.contains("field") && event.payload["field"].toString() == "color") {
                                QString newColor = event.payload.value("value").toString();
                                metaPanel->setColor(newColor, false);
                            } else if (event.payload.contains("field") && event.payload["field"].toString() == "rating") {
                                int newRating = event.payload.value("value").toInt();
                                metaPanel->setRating(newRating, false);
                            }
                            break;
                        }
                    }
                }
            } else if (!event.paths.isEmpty()) {
                activeOrRoot->updateItemsMetadata(event.paths);
                if (contentPanel && contentPanel != activeOrRoot) {
                    contentPanel->updateItemsMetadata(event.paths);
                }
            } else {
                activeOrRoot->refreshAll();
                if (contentPanel && contentPanel != activeOrRoot) {
                    contentPanel->refreshAll();
                }
            }
            // 🚀 拔除直调：统计重算统一由模型的 dataChanged Roles 门禁经 50ms 定时器防抖触发
        } else if (event.type == QuarkMeta::AppEventType::ItemsDeleted ||
                   event.type == QuarkMeta::AppEventType::ItemsRenamed ||
                   event.type == QuarkMeta::AppEventType::UndoRedoPerformed) {
            activeOrRoot->refreshAll();
            if (contentPanel && contentPanel != activeOrRoot) {
                contentPanel->refreshAll();
            }
        }
    });
}

} // namespace QuarkMeta
