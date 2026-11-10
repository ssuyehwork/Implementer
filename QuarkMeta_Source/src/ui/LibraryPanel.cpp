#include "LibraryPanel.h"
#include "controllers/ContentKeyHandler.h"
#include "Logger.h"
#include <QKeyEvent>
#include <QDebug>
#include "UiHelper.h"
#include "ToolTipOverlay.h"
#include "PresetTagsDialog.h"
#include "dialogs/SmartFolderDialog.h"
#include "LibraryDragController.h"
#include "LibraryModelBuilder.h"
#include "LibraryMenuBuilder.h"
#include "../meta/SmartFolderCountService.h"
#include "../meta/LibraryDao.h"
#include "../meta/SmartFolderDao.h"
#include "../meta/LibraryService.h"
#include "../core/PresetTagService.h"
#include "../core/AppConfig.h"
#include <QHeaderView>
#include <QTimer>
#include <QMouseEvent>
#include <QApplication>

namespace QuarkMeta {

void LibraryTreeView::setHoverState(const QPersistentModelIndex& index, SectionHoverArea area) {
    if (m_hoveredIndex == index && m_hoveredArea == area) {
        return;
    }

    QPersistentModelIndex oldIdx = m_hoveredIndex;
    m_hoveredIndex = index;
    m_hoveredArea = area;

    // 同步给属性，供委托无缝访问
    setProperty("_libTreeHoveredIndex", QVariant::fromValue<QModelIndex>(m_hoveredIndex));
    setProperty("_libTreeHoveredArea", static_cast<int>(m_hoveredArea));

    // 状态发生物理改变时，仅定向刷新旧行和新行视口区域
    if (viewport()) {
        if (oldIdx.isValid()) {
            QRect oldRect = visualRect(oldIdx);
            if (oldRect.isValid()) viewport()->update(oldRect);
        }
        if (m_hoveredIndex.isValid()) {
            QRect newRect = visualRect(m_hoveredIndex);
            if (newRect.isValid()) viewport()->update(newRect);
        }
    }
}

void LibraryTreeView::updateHoverState() {
    if (!viewport()) return;
    QPoint mousePos = viewport()->mapFromGlobal(QCursor::pos());
    if (!viewport()->rect().contains(mousePos)) {
        setHoverState(QPersistentModelIndex(), SectionHoverArea::None);
        return;
    }

    QModelIndex idx = indexAt(mousePos);
    if (!idx.isValid()) {
        setHoverState(QPersistentModelIndex(), SectionHoverArea::None);
        return;
    }

    LibraryNodeKind kind = static_cast<LibraryNodeKind>(idx.data(NodeKindRole).toInt());
    if (isSectionKind(kind)) {
        QRect rowR = visualRect(idx);
        int countVal = idx.data(CountRole).toInt();
        SectionHoverArea area = LibraryItemDelegate::hitTestSectionRow(rowR, kind, countVal, font(), mousePos);
        setHoverState(QPersistentModelIndex(idx), area);
    } else {
        setHoverState(QPersistentModelIndex(idx), SectionHoverArea::Row);
    }
}

void LibraryTreeView::mouseMoveEvent(QMouseEvent* event) {
    DropTreeView::mouseMoveEvent(event);

    QPoint pos = event->pos();
    QModelIndex idx = indexAt(pos);
    if (!idx.isValid()) {
        setHoverState(QPersistentModelIndex(), SectionHoverArea::None);
        return;
    }

    LibraryNodeKind kind = static_cast<LibraryNodeKind>(idx.data(NodeKindRole).toInt());
    if (isSectionKind(kind)) {
        QRect rowR = visualRect(idx);
        int countVal = idx.data(CountRole).toInt();
        SectionHoverArea area = LibraryItemDelegate::hitTestSectionRow(rowR, kind, countVal, font(), pos);
        setHoverState(QPersistentModelIndex(idx), area);
    } else {
        setHoverState(QPersistentModelIndex(idx), SectionHoverArea::Row);
    }
}

bool LibraryTreeView::viewportEvent(QEvent* event) {
    if (event->type() == QEvent::Leave) {
        setHoverState(QPersistentModelIndex(), SectionHoverArea::None);
    }
    return DropTreeView::viewportEvent(event);
}

void LibraryTreeView::scrollContentsBy(int dx, int dy) {
    DropTreeView::scrollContentsBy(dx, dy);
    updateHoverState();
}

LibraryPanel::LibraryPanel(QWidget* parent) : QFrame(parent) {
    setObjectName("LibraryContainer");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(230);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_countService = new SmartFolderCountService(this);

    initUi();
    loadLibrary();
}

void LibraryPanel::initUi() {
    m_treeView = new LibraryTreeView(this);
    m_treeView->setObjectName("LibraryTreeView");
    m_treeView->setFocusPolicy(Qt::StrongFocus);
    m_treeView->setHeaderHidden(true);
    if (m_treeView->header()) {
        m_treeView->header()->setStretchLastSection(true);
        m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    }
    m_treeView->setIndentation(0);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setDragEnabled(true);
    m_treeView->setAcceptDrops(true);
    m_treeView->setDropIndicatorShown(true);
    m_treeView->setDefaultDropAction(Qt::MoveAction);
    m_treeView->setDragDropMode(QAbstractItemView::DragDrop);
    m_treeView->setMouseTracking(true);
    m_treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_treeView->installEventFilter(this);
    if (m_treeView->viewport()) {
        m_treeView->viewport()->setMouseTracking(true);
        m_treeView->viewport()->installEventFilter(this);
    }

    m_model = new QStandardItemModel(this);
    m_treeView->setModel(m_model);

    m_delegate = new LibraryItemDelegate(this);
    m_treeView->setItemDelegate(m_delegate);

    m_dragController = new LibraryDragController(m_treeView, m_model, this);
    m_dragController->setupDragDropValidators();

    connect(m_dragController, &LibraryDragController::requestReloadLibrary, this, &LibraryPanel::loadLibrary);

    connect(m_delegate, &LibraryItemDelegate::requestAddCategory, this, [this](LibraryNodeKind kind) {
        if (kind == LibraryNodeKind::SectionSmartFolder) {
            createNewSmartFolder();
        } else {
            createAndEditCategory(0);
        }
    });

    connect(m_delegate, &LibraryItemDelegate::requestToggleSection, this, &LibraryPanel::toggleSectionByNodeKind);

    m_mainLayout->addWidget(m_treeView, 1);

    connect(m_treeView, &QTreeView::clicked, this, &LibraryPanel::onCategoryClicked);
    connect(m_treeView, &QWidget::customContextMenuRequested, this, &LibraryPanel::onCategoryContextMenu);
    connect(m_treeView, &DropTreeView::pathsDropped, this, &LibraryPanel::onPathsDroppedToCategory);

    connect(&LibraryService::instance(), &LibraryService::libraryChanged, this, [this]() {
        QTimer::singleShot(0, this, [this]() {
            loadLibrary();
        });
    });

    // 智能文件夹异步计数回到主线程后更新视图
    connect(m_countService, &SmartFolderCountService::countsReady, this, [this](const QMap<int, int>& results) {
        if (!m_model) return;
        m_isUpdatingCounts = true;
        QStandardItem* sfHeader = findNode(m_model->invisibleRootItem(), LibraryNodeKind::SectionSmartFolder);
        for (auto it = results.constBegin(); it != results.constEnd(); ++it) {
            int sfId = it.key();
            int newCount = it.value();

            if (sfHeader) {
                QStandardItem* sfItem = findNode(sfHeader, LibraryNodeKind::SmartFolder, sfId);
                if (sfItem) {
                    int oldCount = sfItem->data(CountRole).toInt();
                    if (oldCount != newCount) {
                        sfItem->setData(newCount, CountRole);
                        sfItem->setData(newCount, Qt::UserRole + 9);
                    }
                }
            }
        }
        m_isUpdatingCounts = false;
    });

    // 行内重命名编辑提交后落库
    connect(m_model, &QStandardItemModel::itemChanged, this, [this](QStandardItem* item) {
        if (!item || m_isLoading || m_isUpdatingCounts) return;
        LibraryNodeKind kind = static_cast<LibraryNodeKind>(item->data(NodeKindRole).toInt());
        if (kind == LibraryNodeKind::SmartFolder) {
            int sfId = item->data(SmartFolderIdRole).toInt();
            if (sfId > 0) {
                SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(sfId);
                if (rec.id > 0 && rec.name != item->text()) {
                    rec.name = item->text();
                    SmartFolderDao::updateSmartFolder(rec);
                }
            }
        } else if (kind == LibraryNodeKind::Category) {
            int catId = item->data(CategoryIdRole).toInt();
            if (catId > 0) {
                QString name = item->text();
                QString iconKey = item->data(IconKeyRole).toString();
                QString colorHex = item->data(ColorHexRole).toString();
                LibraryDao::updateCategoryNode(catId, name, iconKey, colorHex);
            }
        }
    });
}

void LibraryPanel::loadLibrary() {
    if (!m_model) return;

    // 记录刷新前选中的分类或智能文件夹节点 ID 与节点种类，刷新后精准恢复高亮选中
    int savedSelectedId = 0;
    LibraryNodeKind savedSelectedKind = LibraryNodeKind::SectionCustom;
    bool hasSelection = false;

    if (m_treeView && m_treeView->currentIndex().isValid()) {
        hasSelection = true;
        savedSelectedKind = static_cast<LibraryNodeKind>(m_treeView->currentIndex().data(NodeKindRole).toInt());
        if (savedSelectedKind == LibraryNodeKind::SmartFolder) {
            savedSelectedId = m_treeView->currentIndex().data(SmartFolderIdRole).toInt();
        } else {
            savedSelectedId = m_treeView->currentIndex().data(CategoryIdRole).toInt();
        }
    }

    m_isLoading = true;
    if (m_dragController) m_dragController->setIsLoading(true);

    m_model->clear();

    // 委派给 LibraryModelBuilder 组装整个模型
    LibraryModelBuilder::BuildResult buildRes = LibraryModelBuilder::populateModel(m_model, m_countService);

    // 重建树后先 expandAll()，再按保存的折叠状态设置三大标头行的展开/折叠
    if (m_treeView) {
        m_treeView->expandAll();

        bool qaCollapsed = AppConfig::instance().getValue(sectionCollapsedConfigKey(LibraryNodeKind::SectionQuickAccess), false).toBool();
        if (buildRes.quickAccessHeader) {
            m_treeView->setExpanded(buildRes.quickAccessHeader->index(), !qaCollapsed);
        }

        bool sfCollapsed = AppConfig::instance().getValue(sectionCollapsedConfigKey(LibraryNodeKind::SectionSmartFolder), false).toBool();
        if (buildRes.smartFolderHeader) {
            m_treeView->setExpanded(buildRes.smartFolderHeader->index(), !sfCollapsed);
        }

        bool sectionCollapsed = AppConfig::instance().getValue(sectionCollapsedConfigKey(LibraryNodeKind::SectionCustom), false).toBool();
        if (buildRes.customSectionHeader) {
            m_treeView->setExpanded(buildRes.customSectionHeader->index(), !sectionCollapsed);
        }

        m_treeView->updateHoverState();
    }

    m_isLoading = false;
    if (m_dragController) m_dragController->setIsLoading(false);

    // 触发智能文件夹异步后台计算
    if (!buildRes.smartFolders.isEmpty() && m_countService) {
        m_countService->requestCountsImmediately(buildRes.smartFolders);
    }

    // 恢复树节点的选中高亮状态：严格同时校验 kind 与 ID，严禁把分类的高亮挂到智能文件夹上
    if (hasSelection && savedSelectedId != 0 && m_treeView) {
        QStandardItem* targetItem = findNode(m_model->invisibleRootItem(), savedSelectedKind, savedSelectedId);
        if (targetItem) {
            m_treeView->setCurrentIndex(targetItem->index());
        }
    }

    // 延迟进入行内编辑（新建分类场景）
    if (m_pendingEditNodeId > 0 && m_treeView) {
        int targetNodeId = m_pendingEditNodeId;
        m_pendingEditNodeId = 0;
        QTimer::singleShot(50, this, [this, targetNodeId]() {
            if (!m_model || !m_treeView) return;
            QStandardItem* targetItem = findNode(m_model->invisibleRootItem(), LibraryNodeKind::Category, targetNodeId);
            if (targetItem) {
                QPersistentModelIndex pIdx(targetItem->index());
                if (!pIdx.isValid() || pIdx.model() != m_model) return;

                QModelIndex parentIdx = targetItem->parent() ? targetItem->parent()->index() : QModelIndex();
                if (parentIdx.isValid()) {
                    m_treeView->expand(parentIdx);
                }

                // 必须先 setCurrentIndex，且使用持久化校验后的索引，再 focus 和 edit
                m_treeView->setCurrentIndex(pIdx);
                m_treeView->setFocus(Qt::OtherFocusReason);
                m_treeView->edit(pIdx);
            }
        });
    }
}

void LibraryPanel::saveLibraryOrder() {
    if (m_dragController) {
        m_dragController->saveLibraryOrder();
    }
}

void LibraryPanel::sortCategoriesByName(bool ascending) {
    if (m_dragController) {
        m_dragController->sortCategoriesByName(ascending);
    }
}

void LibraryPanel::invalidateSmartFolderCache(int smartFolderId) {
    if (m_countService) {
        m_countService->invalidate(smartFolderId);
    }
}

void LibraryPanel::refreshSmartFolderCounts() {
    if (m_countService) {
        m_countService->requestCountsDebounced();
    }
}

void LibraryPanel::createNewSmartFolder() {
    SmartFolderDialog dlg(this);
    connect(&dlg, &SmartFolderDialog::rulesPreviewRequested, this, &LibraryPanel::smartFolderPreviewRequested);
    if (dlg.exec() == QDialog::Accepted) {
        SmartFolderRecord rec = dlg.getRecord();
        int id = SmartFolderDao::addSmartFolder(rec);
        if (id > 0) {
            invalidateSmartFolderCache(id);
            loadLibrary();
        }
    }
}

void LibraryPanel::toggleSectionByNodeKind(LibraryNodeKind kind) {
    if (!m_model || !m_treeView) return;
    QStandardItem* sectionItem = findNode(m_model->invisibleRootItem(), kind);
    if (!sectionItem) return;
    QModelIndex idx = sectionItem->index();
    bool isExpanded = m_treeView->isExpanded(idx);
    m_treeView->setExpanded(idx, !isExpanded);

    QString configKey = sectionCollapsedConfigKey(kind);
    if (!configKey.isEmpty()) {
        AppConfig::instance().setValue(configKey, isExpanded);
    }

    if (m_treeView) {
        m_treeView->updateHoverState();
    }
}

void LibraryPanel::toggleSection() {
    toggleSectionByNodeKind(LibraryNodeKind::SectionCustom);
}

bool LibraryPanel::eventFilter(QObject* watched, QEvent* event) {
    if ((watched == m_treeView || (m_treeView && watched == m_treeView->viewport())) 
        && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEv = static_cast<QKeyEvent*>(event);
        if (keyEv->key() == Qt::Key_F2 && keyEv->modifiers() == Qt::NoModifier) {
            QModelIndex curIdx = m_treeView->currentIndex();
            if (curIdx.isValid()) {
                LibraryNodeKind kind = static_cast<LibraryNodeKind>(curIdx.data(NodeKindRole).toInt());
                // 仅允许用户自定义分类与智能文件夹（isUserNodeKind）进入行内编辑，屏蔽系统固定项和分组标头
                if (isUserNodeKind(kind)) {
                    m_treeView->setFocus();
                    m_treeView->edit(curIdx);
                    keyEv->accept();
                    return true;
                }
            }
        }
        if (keyEv->modifiers() & Qt::ControlModifier) {
            if (keyEv->key() == Qt::Key_Home) {
                if (ContentKeyHandler::navigateViewFirstItem(m_treeView)) return true;
            } else if (keyEv->key() == Qt::Key_End) {
                if (ContentKeyHandler::navigateViewLastItem(m_treeView)) return true;
            }
        }
    }
    return QFrame::eventFilter(watched, event);
}

void LibraryPanel::onCategoryClicked(const QModelIndex& index) {
    if (!index.isValid()) return;

    // 🚀【防崩与持久化索引契约】：跨事件循环 (QTimer::singleShot) 绝不能捕获临时 QModelIndex，必须使用 QPersistentModelIndex
    QPersistentModelIndex pIndex(index);
    QTimer::singleShot(0, this, [this, pIndex]() {
        if (!m_treeView || !m_model) return;
        // 关键守卫：若在延迟期间 model 被重置/重新加载，pIndex 会自动失效，严禁执行野指针解引用！
        if (!pIndex.isValid() || pIndex.model() != m_model) return;

        m_treeView->setCurrentIndex(pIndex);
        m_treeView->setFocus(Qt::MouseFocusReason);
    });

    LibraryNodeKind kind = static_cast<LibraryNodeKind>(index.data(NodeKindRole).toInt());
    if (isSectionKind(kind)) {
        return;
    }

    QString nodeName = index.data(Qt::DisplayRole).toString();
    if (kind == LibraryNodeKind::SmartFolder) {
        int smartFolderId = index.data(SmartFolderIdRole).toInt();
        SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(smartFolderId);
        emit smartFolderSelected(rec);
        return;
    }

    int catId = index.data(CategoryIdRole).toInt();
    QStringList paths = LibraryService::instance().getCategoryPaths(catId);
    emit categoryPathsSelected(paths, nodeName, catId);
}

void LibraryPanel::filterTreeNodes(const QString& keyword) {
    if (!m_model || !m_treeView) return;

    std::function<bool(QStandardItem*)> filterRecursive = [&](QStandardItem* item) -> bool {
        if (!item) return false;
        LibraryNodeKind kind = static_cast<LibraryNodeKind>(item->data(NodeKindRole).toInt());

        bool matchesSelf = keyword.isEmpty() || item->text().contains(keyword, Qt::CaseInsensitive);
        bool anyChildMatches = false;

        for (int i = 0; i < item->rowCount(); ++i) {
            QStandardItem* child = item->child(i);
            if (filterRecursive(child)) {
                anyChildMatches = true;
                m_treeView->setRowHidden(i, item->index(), false);
            } else {
                m_treeView->setRowHidden(i, item->index(), true);
            }
        }

        if (isSectionKind(kind)) {
            return true;
        }

        bool visible = matchesSelf || anyChildMatches;
        if (!keyword.isEmpty() && visible && item->hasChildren()) {
            m_treeView->expand(item->index());
        }
        return visible;
    };

    for (int i = 0; i < m_model->rowCount(); ++i) {
        QStandardItem* topItem = m_model->item(i);
        if (!topItem) continue;
        LibraryNodeKind kind = static_cast<LibraryNodeKind>(topItem->data(NodeKindRole).toInt());
        if (isSystemKind(kind)) {
            m_treeView->setRowHidden(i, QModelIndex(), false);
        } else if (isSectionKind(kind)) {
            filterRecursive(topItem);
            m_treeView->setRowHidden(i, QModelIndex(), false);
            if (!keyword.isEmpty()) {
                m_treeView->expand(topItem->index());
            } else {
                QString cfgKey = sectionCollapsedConfigKey(kind);
                bool sectionCollapsed = AppConfig::instance().getValue(cfgKey, false).toBool();
                m_treeView->setExpanded(topItem->index(), !sectionCollapsed);
            }
        }
    }
}

void LibraryPanel::onCategoryContextMenu(const QPoint& pos) {
    if (!m_treeView) return;
    QModelIndex index = m_treeView->indexAt(pos);

    LibraryMenuBuilder::MenuActions actions;
    actions.onNewCategory = [this]() {
        createAndEditCategory(0);
    };
    actions.onNewSmartFolder = [this]() {
        createNewSmartFolder();
    };
    actions.onSortByName = [this](bool asc) {
        sortCategoriesByName(asc);
    };
    actions.onSortManual = [this]() {
        loadLibrary();
    };
    actions.onClearQuickAccess = [this]() {
        // 清空快速访问快捷方式（既有空实现，保持原样并在报告中说明）
    };
    actions.onEditSmartFolderRule = [this](int smartFolderId) {
        SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(smartFolderId);
        SmartFolderDialog dlg(rec, this);
        connect(&dlg, &SmartFolderDialog::rulesPreviewRequested, this, &LibraryPanel::smartFolderPreviewRequested);
        if (dlg.exec() == QDialog::Accepted) {
            SmartFolderRecord updatedRec = dlg.getRecord();
            if (SmartFolderDao::updateSmartFolder(updatedRec)) {
                invalidateSmartFolderCache(smartFolderId);
                loadLibrary();
            }
        }
    };
    actions.onSmartFolderIconChanged = [this](int smartFolderId, const QString& iconKey) {
        SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(smartFolderId);
        if (rec.id > 0) {
            rec.iconKey = iconKey;
            if (SmartFolderDao::updateSmartFolder(rec)) {
                loadLibrary();
            }
        }
    };
    actions.onSmartFolderColorChanged = [this](int smartFolderId, const QString& hexColor) {
        SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(smartFolderId);
        if (rec.id > 0) {
            rec.colorHex = hexColor.isEmpty() ? "#888888" : hexColor.toUpper();
            if (SmartFolderDao::updateSmartFolder(rec)) {
                loadLibrary();
            }
        }
    };
    actions.onDeleteSmartFolder = [this](int smartFolderId) {
        if (SmartFolderDao::removeSmartFolderById(smartFolderId)) {
            invalidateSmartFolderCache(smartFolderId);
            loadLibrary();
        }
    };
    actions.onPinCategory = [this](int catId, bool pin) {
        onPinCategoryTriggered(catId, pin);
    };
    actions.onNewSubCategory = [this](int catId) {
        createAndEditCategory(catId);
    };
    actions.onSetPresetTags = [this](int catId) {
        PresetTagsDialog dlg(catId, this, true);
        dlg.exec();
    };
    actions.onCategoryIconChanged = [this](const QPersistentModelIndex& persistentIdx, const QString& iconKey) {
        if (!persistentIdx.isValid()) return;
        QStandardItem* item = m_model ? m_model->itemFromIndex(persistentIdx) : nullptr;
        if (!item) return;

        QString colorHex = item->data(ColorHexRole).toString();
        if (colorHex.isEmpty()) colorHex = "#888888";

        QIcon newIcon = UiHelper::getIcon(iconKey, QColor(colorHex), 18);
        item->setIcon(newIcon);
        item->setData(iconKey, IconKeyRole);
        item->setData(iconKey, Qt::UserRole + 2);

        int catId = item->data(CategoryIdRole).toInt();
        LibraryDao::updateCategoryNode(catId, item->text(), iconKey, colorHex);
    };
    actions.onCategoryColorChanged = [this](const QPersistentModelIndex& persistentIdx, const QString& hexColor) {
        if (!persistentIdx.isValid()) return;
        QStandardItem* item = m_model ? m_model->itemFromIndex(persistentIdx) : nullptr;
        if (!item) return;

        QString finalColor = hexColor.isEmpty() ? "#888888" : hexColor.toUpper();
        QString iconKey = item->data(IconKeyRole).toString();
        if (iconKey.isEmpty()) iconKey = "folder_filled";

        QIcon newIcon = UiHelper::getIcon(iconKey, QColor(finalColor), 18);
        item->setIcon(newIcon);
        item->setData(finalColor, ColorHexRole);
        item->setData(finalColor, Qt::UserRole + 3);

        int catId = item->data(CategoryIdRole).toInt();
        LibraryDao::updateCategoryNode(catId, item->text(), iconKey, finalColor);
    };
    actions.onRename = [this](const QModelIndex& renameIdx) {
        if (m_treeView) {
            m_treeView->setFocus();
            m_treeView->edit(renameIdx);
        }
    };
    actions.onDeleteCategory = [this](int catId) {
        LibraryService::instance().removeCategory(catId);
    };

    auto menu = LibraryMenuBuilder::buildContextMenu(index, actions, this);
    if (menu) {
        menu->exec(m_treeView->viewport()->mapToGlobal(pos));
    }
}

void LibraryPanel::onPathsDroppedToCategory(const QStringList& paths, const QModelIndex& target) {
    if (paths.isEmpty()) return;

    // 拖拽到空白处时默认为“未分类” (kSystemUncategorizedId)，拖到具体节点则根据其 kind 和 ID 处理
    int targetId = kSystemUncategorizedId;
    LibraryNodeKind kind = LibraryNodeKind::SystemUncategorized;

    if (target.isValid()) {
        kind = static_cast<LibraryNodeKind>(target.data(NodeKindRole).toInt());
        if (kind == LibraryNodeKind::SmartFolder) {
            targetId = target.data(SmartFolderIdRole).toInt();
        } else {
            targetId = target.data(CategoryIdRole).toInt();
        }
    }

    // 拖到固定系统分类(-1 全部数据) 或 三大分组标头行时：静默返回，不做任何处理
    if (kind == LibraryNodeKind::SystemAll || isSectionKind(kind)) {
        return;
    }

    // 只接受普通分类或未分类 (-2)
    if (kind == LibraryNodeKind::Category || kind == LibraryNodeKind::PinnedMirror || kind == LibraryNodeKind::SystemUncategorized) {
        bool ok = LibraryService::instance().addPathsToCategory(targetId, paths);
        if (!ok) {
            ToolTipOverlay::instance()->showToast("归入失败", 1500, QColor("#e81123"));
            return;
        }

        if (kind == LibraryNodeKind::SystemUncategorized) {
            // 目标为未分类时：不执行预设标签，直接显示成功提示
            ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目归入未分类").arg(paths.size()), 1500, QColor("#2ecc71"));
            return;
        }

        int appliedTagCount = PresetTagService::instance().applyPresetTagsForDroppedItems(targetId, paths);

        if (appliedTagCount > 0) {
            ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目关联至当前分类，并自动应用了 %2 个预设标签").arg(paths.size()).arg(appliedTagCount), 2000, QColor("#2ecc71"));
        } else {
            ToolTipOverlay::instance()->showToast(QString("已关联 %1 个路径到当前分类").arg(paths.size()), 1500, QColor("#2ecc71"));
        }
    }
}

void LibraryPanel::onPinCategoryTriggered(int nodeId, bool pin) {
    if (nodeId <= 0) return;
    LibraryDao::setCategoryPinned(nodeId, pin);
    loadLibrary();
}

void LibraryPanel::createAndEditCategory(int parentId) {
    LibraryNodeKind targetHeaderKind = (parentId == kParentIdSmartFolderSection) ? LibraryNodeKind::SectionSmartFolder : LibraryNodeKind::SectionCustom;
    QString targetConfigKey = sectionCollapsedConfigKey(targetHeaderKind);

    if (m_treeView && m_model) {
        QStandardItem* targetHeaderItem = findNode(m_model->invisibleRootItem(), targetHeaderKind);
        if (targetHeaderItem) {
            m_treeView->setExpanded(targetHeaderItem->index(), true);
            AppConfig::instance().setValue(targetConfigKey, false);
        }
    }

    QString defaultName = (parentId == kParentIdSmartFolderSection) ? "新建智能文件夹" : "新建分类";
    int newId = LibraryService::instance().createCategory(defaultName, parentId);
    if (newId > 0) {
        m_pendingEditNodeId = newId;
    }
}

} // namespace QuarkMeta
