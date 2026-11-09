#include "FavoritePanel.h"
#include "controllers/ContentKeyHandler.h"
#include <QKeyEvent>
#include "UiHelper.h"
#include "ShellIconManager.h"
#include "WindowsShellThumbnailProvider.h"
#include "../util/DiskMediaExtractor.h"
#include "ColorPicker.h"
#include "ToolTipOverlay.h"
#include "PresetTagsDialog.h"
#include "dialogs/FramelessConfirmDialog.h"
#include "delegates/FavoriteItemDelegate.h"
#include "controllers/FavoriteTreeDropHandler.h"
#include "../meta/FavoriteService.h"
#include "../meta/MarkedFolderService.h"
#include "../meta/LibraryService.h"
#include "../meta/MetadataManager.h"
#include "../meta/DriveMetaDao.h"
#include "../core/ClipboardService.h"
#include "../core/NavigationService.h"
#include "TabBarWidget.h"
#include "ContentPanel.h"
#include "controllers/ContextMenuFactory.h"
#include "ThumbnailPipelineService.h"
#include <QThreadPool>
#include <QPainter>
#include <QPainterPath>
#include "../core/AppConfig.h"
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QWidgetAction>
#include <QGridLayout>
#include <QFileInfo>
#include <QDir>
#include <QHeaderView>
#include <QCoreApplication>
#include <QtConcurrent>
#include <QPointer>
#include <QTimer>

namespace QuarkMeta {

FavoritePanel::FavoritePanel(QWidget* parent)
    : QFrame(parent) {
    setObjectName("FavoriteContainer");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(230);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    initUi();
    loadFavorites();
}

bool FavoritePanel::eventFilter(QObject* watched, QEvent* event) {
    if ((watched == m_favoriteView || (m_favoriteView && watched == m_favoriteView->viewport())) 
        && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEv = static_cast<QKeyEvent*>(event);
        if (keyEv->key() == Qt::Key_F2 && keyEv->modifiers() == Qt::NoModifier) {
            QModelIndex curIdx = m_favoriteView->currentIndex();
            if (curIdx.isValid()) {
                m_favoriteView->setFocus();
                m_favoriteView->edit(curIdx);
                keyEv->accept();
                return true;
            }
        }
        if (keyEv->modifiers() & Qt::ControlModifier) {
            if (keyEv->key() == Qt::Key_Home) {
                if (ContentKeyHandler::navigateViewFirstItem(m_favoriteView)) return true;
            } else if (keyEv->key() == Qt::Key_End) {
                if (ContentKeyHandler::navigateViewLastItem(m_favoriteView)) return true;
            }
        }
    }
    return QFrame::eventFilter(watched, event);
}

void FavoritePanel::setFocusHighlight(bool visible) {
    Q_UNUSED(visible);
}

void FavoritePanel::initUi() {
    m_favoriteView = new DropTreeView(this);
    m_favoriteView->setObjectName("FavoriteTreeView");
    m_favoriteView->setFocusPolicy(Qt::StrongFocus);
    m_favoriteView->setHeaderHidden(true);
    if (m_favoriteView->header()) {
        m_favoriteView->header()->setStretchLastSection(true);
        m_favoriteView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    }
    m_favoriteView->setIndentation(0);
    m_favoriteView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_favoriteView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_favoriteView->setDragEnabled(true);
    m_favoriteView->setAcceptDrops(true);
    m_favoriteView->setDropIndicatorShown(true);
    m_favoriteView->setDefaultDropAction(Qt::MoveAction);
    m_favoriteView->setDragDropMode(QAbstractItemView::DragDrop);
    m_favoriteView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_favoriteView->installEventFilter(this);
    if (m_favoriteView->viewport()) {
        m_favoriteView->viewport()->installEventFilter(this);
    }
    m_favoriteView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_favoriteView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_favoriteModel = new QStandardItemModel(this);
    m_favoriteView->setModel(m_favoriteModel);
    m_delegate = new FavoriteItemDelegate(this);
    m_favoriteView->setItemDelegate(m_delegate);

    m_mainLayout->addWidget(m_favoriteView, 1);

    // 启用树内拖拽重排序底座
    m_favoriteView->setInternalReorderEnabled(true);

    m_favoriteView->setInternalDragValidator([](const QModelIndex& index) {
        if (!index.isValid()) return false;
        int nodeId = index.data(Qt::UserRole + 6).toInt();
        return nodeId > 0;
    });

    m_favoriteView->setInternalDropValidator([this](const QList<QPersistentModelIndex>& sources, const QModelIndex& target, InternalDropZone zone) -> bool {
        return FavoriteTreeDropHandler::validateDrop(sources, target, zone, m_favoriteModel);
    });

    // 启用外部拖入精确指示定位与区域判定
    m_favoriteView->setExternalDropPositioning(true);
    m_favoriteView->setDropZoneResolver([](const QModelIndex& target, InternalDropZone rawZone, bool inUpperHalf) -> InternalDropZone {
        if (!target.isValid()) return rawZone;
        bool isVirtualCategory = target.data(Qt::UserRole + 7).toBool();
        if (!isVirtualCategory) {
            // 真实收藏项不可作为放进容器：落在中间 50% 时按鼠标上半/下半转为 Above 或 Below
            if (rawZone == InternalDropZone::OnItem) {
                return inUpperHalf ? InternalDropZone::Above : InternalDropZone::Below;
            }
        }
        return rawZone;
    });

    connect(m_favoriteView, &DropTreeView::internalMoveRequested, this, &FavoritePanel::onInternalMoveRequested);
    connect(m_favoriteView, &QTreeView::clicked, this, &FavoritePanel::onFavoriteClicked);
    connect(m_favoriteView, &QWidget::customContextMenuRequested, this, &FavoritePanel::onFavoriteContextMenu);
    connect(m_favoriteView, &DropTreeView::pathsDroppedAt, this, &FavoritePanel::onPathsDroppedAt);

    connect(m_favoriteModel, &QStandardItemModel::rowsMoved, this, &FavoritePanel::markOrderDirty, Qt::DirectConnection);
    connect(m_favoriteModel, &QStandardItemModel::rowsInserted, this, &FavoritePanel::markOrderDirty, Qt::DirectConnection);
    connect(m_favoriteModel, &QStandardItemModel::rowsRemoved, this, &FavoritePanel::markOrderDirty, Qt::DirectConnection);

    // 🚀【行内编辑即时落库】：用户重命名编辑完成后，即时更新 SQLite
    connect(m_favoriteModel, &QStandardItemModel::itemChanged, this, [this](QStandardItem* item) {
        if (!item || m_isLoading) return;
        int nodeId = item->data(Qt::UserRole + 6).toInt();
        if (nodeId > 0) {
            QString name = item->text();
            QString iconKey = item->data(Qt::UserRole + 2).toString();
            QString colorHex = item->data(Qt::UserRole + 3).toString();
            FavoriteService::instance().updateFavoriteNode(nodeId, name, iconKey, colorHex);
        }
    });

    connect(&FavoriteService::instance(), &FavoriteService::favoriteChanged, this, [this](const QString& path, bool isFav) {
        Q_UNUSED(path);
        Q_UNUSED(isFav);
        loadFavorites();
    });
    connect(&FavoriteService::instance(), &FavoriteService::favoritesReloaded, this, [this]() {
        loadFavorites();
    });

    // 🚀【系统图标异步回写】：当后台线程完成文件真实扩展名/程序图标提取时，即时刷新收藏项图标
    connect(&IconLoadNotifier::instance(), &IconLoadNotifier::iconLoaded, this, &FavoritePanel::refreshItemIcons, Qt::QueuedConnection);
}

void FavoritePanel::markOrderDirty() {
    if (m_isLoading) return;
    saveFavorites();
    m_orderDirty = false;
}

void FavoritePanel::flushOrder() {
    if (!m_orderDirty) return;
    saveFavorites();
    m_orderDirty = false;
}

void FavoritePanel::filterTreeNodes(const QString& keyword) {
    if (!m_favoriteModel || !m_favoriteView) return;

    if (m_delegate) {
        m_delegate->setFilterKeyword(keyword.trimmed());
    }

    std::function<bool(QStandardItem*)> filterRecursive = [&](QStandardItem* item) -> bool {
        if (!item) return false;

        bool matchesSelf = keyword.trimmed().isEmpty() || item->text().contains(keyword.trimmed(), Qt::CaseInsensitive);
        bool anyChildMatches = false;

        for (int i = 0; i < item->rowCount(); ++i) {
            QStandardItem* child = item->child(i);
            if (filterRecursive(child)) {
                anyChildMatches = true;
                m_favoriteView->setRowHidden(i, item->index(), false);
            } else {
                m_favoriteView->setRowHidden(i, item->index(), true);
            }
        }

        bool visible = matchesSelf || anyChildMatches;
        if (!keyword.trimmed().isEmpty() && visible && item->hasChildren()) {
            m_favoriteView->expand(item->index());
        }
        return visible;
    };

    for (int i = 0; i < m_favoriteModel->rowCount(); ++i) {
        QStandardItem* topItem = m_favoriteModel->item(i);
        if (!topItem) continue;
        if (filterRecursive(topItem)) {
            m_favoriteView->setRowHidden(i, QModelIndex(), false);
        } else {
            m_favoriteView->setRowHidden(i, QModelIndex(), true);
        }
    }

    if (m_favoriteView->viewport()) {
        m_favoriteView->viewport()->update();
    }
}

void FavoritePanel::onInternalMoveRequested(const QList<QPersistentModelIndex>& sources, const QModelIndex& target, InternalDropZone zone) {
    if (FavoriteTreeDropHandler::executeMove(sources, target, zone, m_favoriteModel, m_favoriteView)) {
        saveFavorites();
        m_orderDirty = false;
    }
}

void FavoritePanel::onFavoriteClicked(const QModelIndex& index) {
    if (!index.isValid()) return;

    // 🚀【防抢焦机制】：联动导航后，延迟夺回键盘焦点给收藏栏
    QTimer::singleShot(0, this, [this, index]() {
        if (m_favoriteView) {
            m_favoriteView->setFocus(Qt::MouseFocusReason);
            m_favoriteView->setCurrentIndex(index);
        }
    });

    bool isVirtual = index.data(Qt::UserRole + 7).toBool();
    if (isVirtual) {
        if (m_favoriteView) {
            if (m_favoriteView->isExpanded(index)) {
                m_favoriteView->collapse(index);
            } else {
                m_favoriteView->expand(index);
            }
        }
        return;
    }

    QString path = index.data(Qt::UserRole + 1).toString();
    if (path.isEmpty()) return;

    QFileInfo fi(path);
    if (!fi.exists()) {
        int recId = index.data(Qt::UserRole + 6).toInt();
        QString msg = QString("无法找到路径：%1\n该文件或文件夹可能已被移动、重命名或删除。\n\n是否将其从收藏夹中移除？").arg(path);
        FramelessConfirmDialog dlg("提示", msg, FramelessConfirmDialog::OkCancel, "alert_warning", QColor("#e74c3c"), this);
        if (dlg.exec() == QDialog::Accepted) {
            if (recId > 0) {
                FavoriteService::instance().removeFavoriteById(recId);
            } else {
                FavoriteService::instance().removeFavorite(path);
            }
            loadFavorites();
        }
        return;
    }

    if (fi.isDir()) {
        bool isAutoRec = index.data(Qt::UserRole + 9).toBool();
        emit directorySelected(path, isAutoRec);
    } else {
        emit requestLocateFile(path);
    }
}

void FavoritePanel::onFavoriteContextMenu(const QPoint& pos) {
    QModelIndex index = m_favoriteView->indexAt(pos);

    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    if (!index.isValid()) {
        auto* sortMenu = menu.addMenu(UiHelper::getIcon("list_ul", QColor("#AAAAAA")), "排列");
        UiHelper::applyMenuStyle(sortMenu);
        QAction* sortAsc = sortMenu->addAction("按名称 (A→Z)");
        connect(sortAsc, &QAction::triggered, this, [this]() { sortItemsByName(true); });
        QAction* sortDesc = sortMenu->addAction("按名称 (Z→A)");
        connect(sortDesc, &QAction::triggered, this, [this]() { sortItemsByName(false); });
        QAction* sortManual = sortMenu->addAction("手动");
        connect(sortManual, &QAction::triggered, this, [this]() { loadFavorites(); });

        menu.exec(m_favoriteView->viewport()->mapToGlobal(pos));
        return;
    }

    QString path = index.data(Qt::UserRole + 1).toString();
    QString curIconKey = index.data(Qt::UserRole + 2).toString();
    QString curColorHex = index.data(Qt::UserRole + 3).toString();
    int nodeId = index.data(Qt::UserRole + 6).toInt();
    bool isVirtual = index.data(Qt::UserRole + 7).toBool();

    if (curIconKey.isEmpty()) curIconKey = "folder_filled";
    if (curColorHex.isEmpty()) curColorHex = "#888888";

    QFileInfo fi(path);
    bool isFolder = isVirtual ? true : fi.isDir();
    bool isItemRemoved = false;

    // 缓存图标按钮指针，以便在换色时动态刷新子菜单图标色彩
    QList<QPair<QPushButton*, QString>> iconButtons;

    if (isFolder) {
        // 1. 打开文件夹
        if (!path.isEmpty() && !isVirtual) {
            QAction* actOpenFolder = menu.addAction(UiHelper::getIcon("folder", QColor("#EEEEEE"), 18), "打开文件夹");
            connect(actOpenFolder, &QAction::triggered, this, [this, path]() {
                emit directorySelected(path, false);
            });
        }

        // 2. 标签页级联子菜单
        if (!path.isEmpty() && !isVirtual) {
            ContextMenuFactory::buildTabCascadeMenu(&menu, path, this);

            // 在“资源管理器”中显示
            ContextMenuFactory::buildShowInExplorerAction(&menu, path, this);

            bool isAutoRec = index.data(Qt::UserRole + 9).toBool();
            QAction* actAutoRec = menu.addAction(UiHelper::getIcon("layers", QColor("#EEEEEE"), 18),
                                                 isAutoRec ? "关闭自动递归" : "开启自动递归");
            connect(actAutoRec, &QAction::triggered, this, [this, nodeId, isAutoRec, index]() {
                bool nextState = !isAutoRec;
                FavoriteService::instance().updateAutoRecursive(nodeId, nextState);
                if (m_favoriteModel) {
                    QStandardItem* item = m_favoriteModel->itemFromIndex(index);
                    if (item) {
                        item->setData(nextState, Qt::UserRole + 9);
                    }
                }
                if (m_favoriteView && m_favoriteView->viewport()) {
                    m_favoriteView->viewport()->update();
                }
            });

            menu.addSeparator();
        }

        QPersistentModelIndex persistentIdx(index);
        ContextMenuFactory::buildIconPickerMenu(&menu, curIconKey, curColorHex,
            [this, persistentIdx](const QString& iconKey) {
                if (!persistentIdx.isValid()) return;
                QStandardItem* item = m_favoriteModel ? m_favoriteModel->itemFromIndex(persistentIdx) : nullptr;
                if (!item) return;

                QString colorHex = item->data(Qt::UserRole + 3).toString();
                if (colorHex.isEmpty()) colorHex = "#888888";

                QIcon newIcon = UiHelper::getIcon(iconKey, QColor(colorHex), 18);
                item->setIcon(newIcon);
                item->setData(iconKey, Qt::UserRole + 2);

                if (m_favoriteView && m_favoriteView->viewport()) {
                    m_favoriteView->viewport()->update();
                }
            },
            [this, persistentIdx](const QString& hexColor) {
                if (!persistentIdx.isValid()) return;
                QStandardItem* item = m_favoriteModel ? m_favoriteModel->itemFromIndex(persistentIdx) : nullptr;
                if (!item) return;

                QString finalColor = hexColor.isEmpty() ? "#888888" : hexColor.toUpper();
                QString iconKey = item->data(Qt::UserRole + 2).toString();
                if (iconKey.isEmpty()) iconKey = "folder_filled";
                QString targetPath = item->data(Qt::UserRole + 1).toString();

                QIcon newIcon = UiHelper::getIcon(iconKey, QColor(finalColor), 18);
                item->setIcon(newIcon);
                item->setData(finalColor, Qt::UserRole + 3);

                if (!targetPath.isEmpty() && !targetPath.startsWith("virtual_cat_")) {
                    AppCommand cmd;
                    cmd.type = AppCommandType::SetColor;
                    cmd.targetPaths = {targetPath};
                    cmd.params["color"] = finalColor;
                    CoreEngine::instance().executeCommand(cmd);
                }

                if (m_favoriteView && m_favoriteView->viewport()) {
                    m_favoriteView->viewport()->update();
                }
            }
        );
    }

    // 复制名称 与 复制完整路径
    ContextMenuFactory::buildCopyNameAction(&menu, {path}, this);
    ContextMenuFactory::buildCopyPathAction(&menu, {path}, this);

    if (isFolder && !path.isEmpty() && !isVirtual) {
        LibraryService::instance().buildLibraryAction(&menu, QStringList{path}, this);

        // 粘贴到此文件夹
        QAction* actPaste = menu.addAction(UiHelper::getIcon("paste", QColor("#EEEEEE"), 18), "粘贴到此文件夹");
        actPaste->setEnabled(ClipboardService::instance().canPaste(path));
        connect(actPaste, &QAction::triggered, this, [this, path]() {
            ClipboardService::instance().executePaste(path, this);
        });
    }

    // 3. 重命名 (直接唤起行内编辑框)
    QAction* renameAct = menu.addAction(UiHelper::getIcon("rename-1.svg", QColor("#EEEEEE")), "重命名");
    connect(renameAct, &QAction::triggered, this, [this, index]() {
        if (m_favoriteView && index.isValid()) {
            m_favoriteView->edit(index);
        }
    });

    // 4. 删除 / 取消收藏
    if (isVirtual) {
        QAction* removeAct = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE")), "删除");
        connect(removeAct, &QAction::triggered, this, [this, nodeId, &isItemRemoved]() {
            isItemRemoved = true;
            FavoriteService::instance().removeFavoriteById(nodeId);
        });
    } else {
        MarkedFolderService::instance().buildMarkAction(&menu, path, this);
        QAction* favAct = FavoriteService::instance().buildFavoriteAction(&menu, path, this);
        if (favAct) {
            connect(favAct, &QAction::triggered, this, [&isItemRemoved]() {
                isItemRemoved = true;
            });
        }
    }

    menu.addSeparator();

    // 5. 排列子菜单
    auto* sortMenu = menu.addMenu(UiHelper::getIcon("list_ul", QColor("#AAAAAA")), "排列");
    UiHelper::applyMenuStyle(sortMenu);
    QAction* sortAsc = sortMenu->addAction("按名称 (A→Z)");
    connect(sortAsc, &QAction::triggered, this, [this]() { sortItemsByName(true); });
    QAction* sortDesc = sortMenu->addAction("按名称 (Z→A)");
    connect(sortDesc, &QAction::triggered, this, [this]() { sortItemsByName(false); });
    QAction* sortManual = sortMenu->addAction("手动");
    connect(sortManual, &QAction::triggered, this, [this]() { loadFavorites(); });

    // 阻塞展示菜单
    menu.exec(m_favoriteView->viewport()->mapToGlobal(pos));

    // 🚀【失焦退出机制】：菜单自然关闭后物理数据库持久化！
    if (isFolder && !isItemRemoved && index.isValid()) {
        QStandardItem* item = m_favoriteModel->itemFromIndex(index);
        if (item) {
            QString finalPath = item->data(Qt::UserRole + 1).toString();
            QString finalIconKey = item->data(Qt::UserRole + 2).toString();
            QString finalColorHex = item->data(Qt::UserRole + 3).toString();
            QString finalName = item->text();
            FavoriteService::instance().updateFavoriteNode(nodeId, finalName, finalIconKey, finalColorHex);
        }
    }
}

void FavoritePanel::onPathsDroppedAt(const QStringList& paths, const QModelIndex& target, InternalDropZone zone) {
    if (paths.isEmpty()) return;

    int parentId = 0;
    int beforeSiblingId = 0;

    if (zone == InternalDropZone::Blank) {
        // Blank：父节点 0，之前的兄弟 0（追加到根级末尾）
        parentId = 0;
        beforeSiblingId = 0;
    } else if (zone == InternalDropZone::OnItem) {
        // OnItem（此时目标必定是虚拟文件夹）：父节点为目标节点编号，之前的兄弟 0（追加到该文件夹末尾）
        if (target.isValid()) {
            parentId = target.data(Qt::UserRole + 6).toInt();
        }
        beforeSiblingId = 0;
    } else if (zone == InternalDropZone::Above) {
        // Above：父节点为目标的模型父节点编号（根级为 0），之前的兄弟为目标自身的节点编号
        if (target.isValid()) {
            QModelIndex pIdx = target.parent();
            if (pIdx.isValid()) {
                parentId = pIdx.data(Qt::UserRole + 6).toInt();
            } else {
                parentId = 0;
            }
            beforeSiblingId = target.data(Qt::UserRole + 6).toInt();
        }
    } else if (zone == InternalDropZone::Below) {
        // Below：父节点同上，之前的兄弟为目标在模型中同一父级下、紧邻其后的那一行的节点编号，没有则为 0
        if (target.isValid()) {
            QModelIndex pIdx = target.parent();
            if (pIdx.isValid()) {
                parentId = pIdx.data(Qt::UserRole + 6).toInt();
            } else {
                parentId = 0;
            }

            int nextRow = target.row() + 1;
            int totalRows = m_favoriteModel ? (pIdx.isValid() ? m_favoriteModel->rowCount(pIdx) : m_favoriteModel->rowCount()) : 0;
            if (nextRow < totalRows && m_favoriteModel) {
                QModelIndex nextSiblingIdx = m_favoriteModel->index(nextRow, 0, pIdx);
                if (nextSiblingIdx.isValid()) {
                    beforeSiblingId = nextSiblingIdx.data(Qt::UserRole + 6).toInt();
                }
            }
        }
    }

    FavoriteService::instance().addFavoritesAt(paths, parentId, beforeSiblingId);
}

void FavoritePanel::refreshItemIcons() {
    if (!m_favoriteModel || m_isLoading) return;

    std::function<void(QStandardItem*)> updateIconsRecursive = [&](QStandardItem* parentItem) {
        int count = parentItem ? parentItem->rowCount() : m_favoriteModel->rowCount();
        for (int i = 0; i < count; ++i) {
            QStandardItem* item = parentItem ? parentItem->child(i) : m_favoriteModel->item(i);
            if (!item) continue;

            bool isDir = item->data(Qt::UserRole + 4).toBool();
            bool hasCustomThumb = item->data(Qt::UserRole + 5).toBool();
            bool isVirtual = item->data(Qt::UserRole + 7).toBool();

            // 仅对非文件夹、且尚未被自定义大缩略图接管的普通文件更新真实系统文件图标
            if (!isDir && !isVirtual && !hasCustomThumb) {
                QString itemPath = item->data(Qt::UserRole + 1).toString();
                if (!itemPath.isEmpty()) {
                    QIcon newIcon = ShellIconManager::getFileIcon(itemPath);
                    if (!newIcon.isNull()) {
                        item->setIcon(newIcon);
                    }
                }
            }

            if (item->hasChildren()) {
                updateIconsRecursive(item);
            }
        }
    };

    updateIconsRecursive(nullptr);
    if (m_favoriteView && m_favoriteView->viewport()) {
        m_favoriteView->viewport()->update();
    }
}

void FavoritePanel::updateItemThumbnail(const QString& path, const QPixmap& pix) {
    if (!m_favoriteModel || pix.isNull()) return;

    QString cleanTarget = QDir::toNativeSeparators(QDir::cleanPath(path));

    std::function<bool(QStandardItem*)> findAndUpdate = [&](QStandardItem* parentItem) -> bool {
        int rowCount = parentItem ? parentItem->rowCount() : m_favoriteModel->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QStandardItem* item = parentItem ? parentItem->child(i) : m_favoriteModel->item(i);
            if (!item) continue;

            QString itemPath = QDir::toNativeSeparators(QDir::cleanPath(item->data(Qt::UserRole + 1).toString()));
            if (QString::compare(itemPath, cleanTarget, Qt::CaseInsensitive) == 0) {
                item->setIcon(QIcon(pix));
                item->setData(true, Qt::UserRole + 5);
                if (m_favoriteView && m_favoriteView->viewport()) {
                    m_favoriteView->viewport()->update();
                }
                return true;
            }

            if (findAndUpdate(item)) return true;
        }
        return false;
    };

    findAndUpdate(nullptr);
}

void FavoritePanel::loadFavorites() {
    if (!m_favoriteModel) return;
    m_isLoading = true;
    m_favoriteModel->clear();

    auto list = FavoriteService::instance().getAllFavorites();

    QStringList pathsToExtract;
    QMap<int, QStandardItem*> itemMap;

    // First Pass: Create QStandardItems
    for (const auto& rec : list) {
        bool isVirtual = (rec.nodeType == FavoriteNodeType::VirtualCategory);
        QString nativePath = isVirtual ? rec.path : QDir::toNativeSeparators(QDir::cleanPath(rec.path));

        QColor itemColor = QColor(rec.colorHex);
        if (!itemColor.isValid()) itemColor = QColor("#888888");

        QString iconKey = rec.iconKey.isEmpty() ? "folder_filled" : rec.iconKey;
        if (iconKey == "folder") iconKey = "folder_filled";

        bool isDir = isVirtual ? true : QFileInfo(nativePath).isDir();
        QIcon icon;

        if (isDir) {
            icon = UiHelper::getIcon(iconKey, itemColor, 18);
        } else {
            icon = ShellIconManager::getFileIcon(nativePath);
            QString ext = QFileInfo(nativePath).suffix().toLower();
            if (ColorPaletteEngine::isGraphicsFile(ext) || ext == "psd" || ext == "ai" || ext == "eps" || ext == "pdf" || ext == "svg") {
                pathsToExtract << nativePath;
            }
        }

        QString displayName = rec.name;
        if (displayName.isEmpty() && !isVirtual) {
            displayName = QFileInfo(nativePath).fileName();
        }

        QStandardItem* item = new QStandardItem(icon, displayName);
        item->setData(nativePath, Qt::UserRole + 1);
        item->setData(iconKey, Qt::UserRole + 2);
        item->setData(rec.colorHex, Qt::UserRole + 3);
        item->setData(isDir, Qt::UserRole + 4);
        item->setData(false, Qt::UserRole + 5);
        item->setData(rec.id, Qt::UserRole + 6);
        item->setData(isVirtual, Qt::UserRole + 7);
        item->setData(rec.parentId, Qt::UserRole + 8);
        item->setData(rec.isAutoRecursive, Qt::UserRole + 9);

        itemMap.insert(rec.id, item);
    }

    // Second Pass: Build Tree Hierarchy
    for (const auto& rec : list) {
        if (!itemMap.contains(rec.id)) continue;
        QStandardItem* item = itemMap.value(rec.id);

        if (rec.parentId > 0 && itemMap.contains(rec.parentId)) {
            itemMap.value(rec.parentId)->appendRow(item);
        } else {
            m_favoriteModel->appendRow(item);
        }
    }

    if (m_favoriteView) {
        m_favoriteView->expandAll();
    }

    m_isLoading = false;

    // 🚀 【时序对齐】：新建节点完成后，找到对应节点并唤起行内重命名编辑框
    if (m_pendingEditNodeId > 0 && m_favoriteView) {
        int targetNodeId = m_pendingEditNodeId;
        m_pendingEditNodeId = 0;

        std::function<QModelIndex(QStandardItem*)> findIndexByNodeId = [&](QStandardItem* parentItem) -> QModelIndex {
            int rowCount = parentItem ? parentItem->rowCount() : m_favoriteModel->rowCount();
            for (int i = 0; i < rowCount; ++i) {
                QStandardItem* item = parentItem ? parentItem->child(i) : m_favoriteModel->item(i);
                if (!item) continue;
                if (item->data(Qt::UserRole + 6).toInt() == targetNodeId) {
                    return item->index();
                }
                QModelIndex childIdx = findIndexByNodeId(item);
                if (childIdx.isValid()) return childIdx;
            }
            return QModelIndex();
        };

        QModelIndex targetIndex = findIndexByNodeId(nullptr);
        if (targetIndex.isValid()) {
            m_favoriteView->setCurrentIndex(targetIndex);
            m_favoriteView->edit(targetIndex);
        }
    }

    if (!pathsToExtract.isEmpty()) {
        QPointer<FavoritePanel> weakThis(this);
        for (const QString& path : pathsToExtract) {
            (void)QtConcurrent::run(ThumbnailPipelineService::instance().decodePool(), [weakThis, path]() {
                if (!weakThis) return;
                // 磁盘缓存只有 230 一档；收藏面板要 128，从 230 缩小，不单独写盘
                QImage img = DiskMediaExtractor::getCapsuleThumbnail(path, DiskMediaExtractor::kThumbSize);
                if (!img.isNull()) {
                    if (img.width() > 128 || img.height() > 128) {
                        img = img.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    }
                    QPixmap pix = QPixmap::fromImage(img);
                    QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis, path, pix]() {
                        if (weakThis) {
                            weakThis->updateItemThumbnail(path, pix);
                        }
                    }, Qt::QueuedConnection);
                }
            });
        }
    }
}

void FavoritePanel::saveFavorites() {
    if (!m_favoriteModel || m_isLoading) return;

    QList<NodeOrderEntry> entries;
    int orderCounter = 1;

    std::function<void(QStandardItem*, int)> traverseAndCollect = [&](QStandardItem* parentItem, int parentId) {
        int rowCount = parentItem ? parentItem->rowCount() : m_favoriteModel->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QStandardItem* item = parentItem ? parentItem->child(i) : m_favoriteModel->item(i);
            if (!item) continue;

            int nodeId = item->data(Qt::UserRole + 6).toInt();
            if (nodeId > 0) {
                NodeOrderEntry entry;
                entry.id = nodeId;
                entry.parentId = parentId;
                entry.sortOrder = orderCounter++;
                entries.append(entry);

                traverseAndCollect(item, nodeId);
            }
        }
    };

    traverseAndCollect(nullptr, 0);

    if (!entries.isEmpty()) {
        FavoriteService::instance().saveFavoritesOrder(entries);
    }
}

bool FavoritePanel::containsPath(const QString& path) const {
    return FavoriteService::instance().isFavorite(path);
}

void FavoritePanel::removeFavoriteItem(const QString& path) {
    FavoriteService::instance().removeFavorite(path);
}

void FavoritePanel::addFavoriteItem(const QString& path, int parentId) {
    FavoriteService::instance().addFavorite(path, parentId);
}

void FavoritePanel::addVirtualCategory(const QString& name, int parentId) {
    FavoriteService::instance().addVirtualCategory(name, parentId);
}

void FavoritePanel::createAndEditCategory(int parentId) {
    int newId = FavoriteService::instance().addVirtualCategory("新建文件夹", parentId);
    if (newId > 0) {
        m_pendingEditNodeId = newId;
    }
}

void FavoritePanel::sortItemsByName(bool ascending) {
    if (!m_favoriteModel) return;

    std::function<void(QStandardItem*)> sortChildren = [&](QStandardItem* parentItem) {
        int rowCount = parentItem ? parentItem->rowCount() : m_favoriteModel->rowCount();
        if (rowCount <= 1) return;

        QList<QStandardItem*> childItems;
        for (int i = rowCount - 1; i >= 0; --i) {
            QStandardItem* item = parentItem ? parentItem->takeRow(i).first() : m_favoriteModel->takeRow(i).first();
            if (item) childItems.prepend(item);
        }

        std::sort(childItems.begin(), childItems.end(), [ascending](QStandardItem* a, QStandardItem* b) {
            return ascending ? (a->text().localeAwareCompare(b->text()) < 0)
                              : (a->text().localeAwareCompare(b->text()) > 0);
        });

        for (QStandardItem* item : childItems) {
            if (parentItem) {
                parentItem->appendRow(item);
            } else {
                m_favoriteModel->appendRow(item);
            }
            sortChildren(item);
        }
    };

    sortChildren(nullptr);
    saveFavorites();
    m_orderDirty = false;
}

} // namespace QuarkMeta
