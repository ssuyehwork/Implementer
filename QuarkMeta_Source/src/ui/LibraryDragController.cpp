#include "LibraryDragController.h"
#include "../meta/LibraryDao.h"
#include "../meta/SmartFolderDao.h"
#include <QTimer>
#include <algorithm>

namespace QuarkMeta {

LibraryDragController::LibraryDragController(DropTreeView* treeView, QStandardItemModel* model, QObject* parent)
    : QObject(parent), m_treeView(treeView), m_model(model) {
    if (m_model) {
        connect(m_model, &QStandardItemModel::rowsMoved, this, &LibraryDragController::markOrderDirty, Qt::DirectConnection);
        connect(m_model, &QStandardItemModel::rowsInserted, this, &LibraryDragController::markOrderDirty, Qt::DirectConnection);
        connect(m_model, &QStandardItemModel::rowsRemoved, this, &LibraryDragController::markOrderDirty, Qt::DirectConnection);
    }
    if (m_treeView) {
        connect(m_treeView, &DropTreeView::internalMoveRequested, this, &LibraryDragController::onInternalMoveRequested);
    }
}

void LibraryDragController::setupDragDropValidators() {
    if (!m_treeView) return;

    m_treeView->setInternalReorderEnabled(true);

    // 拖拽起始校验：仅允许 Category 和 SmartFolder（重构前为 nodeId > 0 且 type == "smart_folder" || type != "section"）
    m_treeView->setInternalDragValidator([](const QModelIndex& index) {
        if (!index.isValid()) return false;
        LibraryNodeKind kind = static_cast<LibraryNodeKind>(index.data(NodeKindRole).toInt());
        return kind == LibraryNodeKind::Category || kind == LibraryNodeKind::SmartFolder;
    });

    // 放置校验
    m_treeView->setInternalDropValidator([this](const QList<QPersistentModelIndex>& sources, const QModelIndex& target, InternalDropZone zone) -> bool {
        if (sources.isEmpty() || !m_model) return false;

        bool isSmartFolderDrag = false;
        for (const QPersistentModelIndex& psrc : sources) {
            if (!psrc.isValid()) return false;
            LibraryNodeKind k = static_cast<LibraryNodeKind>(psrc.data(NodeKindRole).toInt());
            if (k != LibraryNodeKind::Category && k != LibraryNodeKind::SmartFolder) return false;
            if (k == LibraryNodeKind::SmartFolder) {
                isSmartFolderDrag = true;
            }
        }

        // 智能文件夹同组隔离守卫
        if (isSmartFolderDrag) {
            QStandardItem* sfHeader = findNode(m_model->invisibleRootItem(), LibraryNodeKind::SectionSmartFolder);
            if (!sfHeader) return false;

            if (zone == InternalDropZone::OnItem) {
                if (!target.isValid()) return false;
                LibraryNodeKind targetKind = static_cast<LibraryNodeKind>(target.data(NodeKindRole).toInt());
                return targetKind == LibraryNodeKind::SectionSmartFolder;
            } else if (zone == InternalDropZone::Above || zone == InternalDropZone::Below) {
                if (!target.isValid()) return false;
                LibraryNodeKind targetKind = static_cast<LibraryNodeKind>(target.data(NodeKindRole).toInt());
                if (targetKind != LibraryNodeKind::SmartFolder) return false;
                for (const QPersistentModelIndex& psrc : sources) {
                    if (psrc.isValid() && psrc == target) return false;
                }
                return true;
            }
            return false;
        }

        auto isSourceOrDescendant = [](const QModelIndex& idx, const QList<QPersistentModelIndex>& srcList) -> bool {
            QModelIndex cur = idx;
            while (cur.isValid()) {
                for (const QPersistentModelIndex& psrc : srcList) {
                    if (psrc.isValid() && psrc == cur) return true;
                }
                cur = cur.parent();
            }
            return false;
        };

        QStandardItem* sectionItem = findNode(m_model->invisibleRootItem(), LibraryNodeKind::SectionCustom);
        if (!sectionItem) return false;

        QStandardItem* targetParentItem = nullptr;
        int destRow = 0;

        if (zone == InternalDropZone::OnItem) {
            if (!target.isValid()) return false;
            LibraryNodeKind targetKind = static_cast<LibraryNodeKind>(target.data(NodeKindRole).toInt());
            if (targetKind == LibraryNodeKind::SectionQuickAccess) {
                return true; // 允许拖拽放置到快速访问标头上以置顶
            }
            if (targetKind != LibraryNodeKind::Category) return false;

            if (isSourceOrDescendant(target, sources)) return false;

            targetParentItem = m_model->itemFromIndex(target);
            if (targetParentItem) destRow = targetParentItem->rowCount();
        } else if (zone == InternalDropZone::Above || zone == InternalDropZone::Below) {
            if (!target.isValid()) return false;
            LibraryNodeKind targetKind = static_cast<LibraryNodeKind>(target.data(NodeKindRole).toInt());
            if (targetKind != LibraryNodeKind::Category) return false;

            for (const QPersistentModelIndex& psrc : sources) {
                if (psrc.isValid() && psrc == target) return false;
            }
            if (target.parent().isValid() && isSourceOrDescendant(target.parent(), sources)) return false;

            targetParentItem = target.parent().isValid() ? m_model->itemFromIndex(target.parent()) : sectionItem;
            destRow = (zone == InternalDropZone::Above) ? target.row() : target.row() + 1;
        } else if (zone == InternalDropZone::Blank) {
            targetParentItem = sectionItem;
            destRow = sectionItem->rowCount();
        }

        if (!targetParentItem) return false;

        bool anyChanged = false;
        for (const QPersistentModelIndex& psrc : sources) {
            if (!psrc.isValid()) continue;
            QStandardItem* srcItem = m_model->itemFromIndex(psrc);
            if (!srcItem) continue;

            QStandardItem* srcParentItem = srcItem->parent() ? srcItem->parent() : sectionItem;
            int srcRow = srcItem->row();

            if (srcParentItem != targetParentItem) {
                anyChanged = true;
                break;
            } else {
                if (zone == InternalDropZone::OnItem || zone == InternalDropZone::Blank) {
                    if (srcRow != targetParentItem->rowCount() - 1) {
                        anyChanged = true;
                        break;
                    }
                } else if (zone == InternalDropZone::Above || zone == InternalDropZone::Below) {
                    if (srcRow != destRow && srcRow != destRow - 1) {
                        anyChanged = true;
                        break;
                    }
                }
            }
        }

        return anyChanged;
    });
}

void LibraryDragController::markOrderDirty() {
    if (m_isLoading) return;
    if (m_orderDirty) return;
    m_orderDirty = true;
    QTimer::singleShot(0, this, &LibraryDragController::flushOrder);
}

void LibraryDragController::flushOrder() {
    if (!m_orderDirty) return;
    saveLibraryOrder();
    m_orderDirty = false;
}

void LibraryDragController::saveExpandedStates(QStandardItem* pItem, QMap<int, bool>& expandedMap) {
    int rCount = pItem ? pItem->rowCount() : (m_model ? m_model->rowCount() : 0);
    for (int i = 0; i < rCount; ++i) {
        QStandardItem* child = pItem ? pItem->child(i) : m_model->item(i);
        if (!child) continue;
        LibraryNodeKind kind = static_cast<LibraryNodeKind>(child->data(NodeKindRole).toInt());
        if (kind == LibraryNodeKind::Category) {
            int catId = child->data(CategoryIdRole).toInt();
            if (catId > 0 && m_treeView) {
                expandedMap[catId] = m_treeView->isExpanded(child->index());
            }
        }
        saveExpandedStates(child, expandedMap);
    }
}

void LibraryDragController::restoreExpandedStates(QStandardItem* pItem, const QMap<int, bool>& expandedMap) {
    int rCount = pItem ? pItem->rowCount() : (m_model ? m_model->rowCount() : 0);
    for (int i = 0; i < rCount; ++i) {
        QStandardItem* child = pItem ? pItem->child(i) : m_model->item(i);
        if (!child) continue;
        LibraryNodeKind kind = static_cast<LibraryNodeKind>(child->data(NodeKindRole).toInt());
        if (kind == LibraryNodeKind::Category) {
            int catId = child->data(CategoryIdRole).toInt();
            if (catId > 0 && expandedMap.contains(catId) && m_treeView) {
                m_treeView->setExpanded(child->index(), expandedMap.value(catId));
            }
        }
        restoreExpandedStates(child, expandedMap);
    }
}

void LibraryDragController::onInternalMoveRequested(const QList<QPersistentModelIndex>& sources, const QModelIndex& target, InternalDropZone zone) {
    if (sources.isEmpty() || !m_model) return;

    bool isSmartFolderMove = false;
    for (const auto& psrc : sources) {
        if (psrc.isValid() && static_cast<LibraryNodeKind>(psrc.data(NodeKindRole).toInt()) == LibraryNodeKind::SmartFolder) {
            isSmartFolderMove = true;
            break;
        }
    }

    if (isSmartFolderMove) {
        QStandardItem* sfHeader = findNode(m_model->invisibleRootItem(), LibraryNodeKind::SectionSmartFolder);
        if (!sfHeader) return;

        int destRow = sfHeader->rowCount();
        if (zone == InternalDropZone::Above && target.isValid()) {
            destRow = target.row();
        } else if (zone == InternalDropZone::Below && target.isValid()) {
            destRow = target.row() + 1;
        }

        QList<QStandardItem*> itemsToMove;
        for (const auto& psrc : sources) {
            if (psrc.isValid()) {
                QStandardItem* item = m_model->itemFromIndex(psrc);
                if (item) itemsToMove.append(item);
            }
        }
        if (itemsToMove.isEmpty()) return;

        QList<QStandardItem*> movedItemsList;
        for (QStandardItem* item : itemsToMove) {
            int srcRow = item->row();
            QList<QStandardItem*> rowItems = sfHeader->takeRow(srcRow);
            if (rowItems.isEmpty()) continue;

            int insertRow = destRow;
            if (srcRow < insertRow) insertRow--;
            sfHeader->insertRow(insertRow, rowItems);
            destRow = insertRow + 1;
            movedItemsList.append(rowItems.first());
        }

        if (m_treeView && m_treeView->selectionModel()) {
            m_treeView->selectionModel()->clearSelection();
            for (QStandardItem* movedItem : movedItemsList) {
                m_treeView->selectionModel()->select(movedItem->index(), QItemSelectionModel::Select | QItemSelectionModel::Rows);
            }
        }

        saveLibraryOrder();
        m_orderDirty = false;
        return;
    }

    QStandardItem* sectionItem = findNode(m_model->invisibleRootItem(), LibraryNodeKind::SectionCustom);
    if (!sectionItem) return;

    QStandardItem* parentItem = nullptr;
    int destRow = 0;

    if (zone == InternalDropZone::OnItem) {
        if (target.isValid()) {
            LibraryNodeKind targetKind = static_cast<LibraryNodeKind>(target.data(NodeKindRole).toInt());
            if (targetKind == LibraryNodeKind::SectionQuickAccess) {
                // 拖拽到“快速访问”标头：将选中的分类全量触发置顶
                for (const QPersistentModelIndex& pidx : sources) {
                    if (pidx.isValid()) {
                        LibraryNodeKind srcKind = static_cast<LibraryNodeKind>(pidx.data(NodeKindRole).toInt());
                        if (srcKind == LibraryNodeKind::Category) {
                            int srcId = pidx.data(CategoryIdRole).toInt();
                            if (srcId > 0) {
                                LibraryDao::setCategoryPinned(srcId, true);
                            }
                        }
                    }
                }
                emit requestReloadLibrary();
                return;
            }

            parentItem = m_model->itemFromIndex(target);
            if (parentItem) {
                destRow = parentItem->rowCount();
            }
        }
    } else if (zone == InternalDropZone::Above) {
        if (target.isValid()) {
            parentItem = (target.parent().isValid() && static_cast<LibraryNodeKind>(target.parent().data(NodeKindRole).toInt()) == LibraryNodeKind::Category)
                         ? m_model->itemFromIndex(target.parent())
                         : sectionItem;
            destRow = target.row();
        }
    } else if (zone == InternalDropZone::Below) {
        if (target.isValid()) {
            parentItem = (target.parent().isValid() && static_cast<LibraryNodeKind>(target.parent().data(NodeKindRole).toInt()) == LibraryNodeKind::Category)
                         ? m_model->itemFromIndex(target.parent())
                         : sectionItem;
            destRow = target.row() + 1;
        }
    } else if (zone == InternalDropZone::Blank) {
        parentItem = sectionItem;
        destRow = sectionItem->rowCount();
    }

    if (!parentItem) {
        parentItem = sectionItem;
        destRow = sectionItem->rowCount();
    }

    // 1. 记录展开状态
    QMap<int, bool> expandedMap;
    saveExpandedStates(sectionItem, expandedMap);

    // 2. 转换 itemsToMove
    QList<QStandardItem*> itemsToMove;
    for (const QPersistentModelIndex& pidx : sources) {
        if (pidx.isValid()) {
            QStandardItem* item = m_model->itemFromIndex(pidx);
            if (item) itemsToMove.append(item);
        }
    }
    if (itemsToMove.isEmpty()) return;

    QList<QStandardItem*> movedItemsList;
    for (QStandardItem* item : itemsToMove) {
        QStandardItem* srcParent = item->parent() ? item->parent() : sectionItem;
        int srcRow = item->row();

        QList<QStandardItem*> rowItems = srcParent->takeRow(srcRow);
        if (rowItems.isEmpty()) continue;

        int insertRow = destRow;
        if (srcParent == parentItem && srcRow < insertRow) {
            insertRow--;
        }

        parentItem->insertRow(insertRow, rowItems);
        destRow = insertRow + 1;
        movedItemsList.append(rowItems.first());
    }

    // 3. 恢复展开状态
    restoreExpandedStates(sectionItem, expandedMap);

    // 4. 若放入某分类内部，展开该分类
    if (zone == InternalDropZone::OnItem && target.isValid() && static_cast<LibraryNodeKind>(target.data(NodeKindRole).toInt()) == LibraryNodeKind::Category) {
        m_treeView->setExpanded(target, true);
    }

    // 5. 恢复选中状态
    if (m_treeView && m_treeView->selectionModel()) {
        m_treeView->selectionModel()->clearSelection();
        for (QStandardItem* movedItem : movedItemsList) {
            m_treeView->selectionModel()->select(movedItem->index(), QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    }

    // 6. 落库保存
    saveLibraryOrder();
    m_orderDirty = false;
}

void LibraryDragController::saveLibraryOrder() {
    if (!m_model || m_isLoading) return;

    QStandardItem* sectionItem = findNode(m_model->invisibleRootItem(), LibraryNodeKind::SectionCustom);
    if (!sectionItem) return;

    QList<NodeOrderEntry> entries;
    int orderCounter = 1;

    std::function<void(QStandardItem*, int)> traverseAndCollect = [&](QStandardItem* parentItem, int parentId) {
        int rowCount = parentItem->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QStandardItem* child = parentItem->child(i);
            if (!child) continue;
            LibraryNodeKind kind = static_cast<LibraryNodeKind>(child->data(NodeKindRole).toInt());
            if (kind == LibraryNodeKind::Category) {
                int catId = child->data(CategoryIdRole).toInt();
                if (catId > 0) {
                    NodeOrderEntry entry;
                    entry.id = catId;
                    entry.parentId = parentId;
                    entry.sortOrder = orderCounter++;
                    entries.append(entry);

                    traverseAndCollect(child, catId);
                }
            }
        }
    };

    traverseAndCollect(sectionItem, 0);

    if (!entries.isEmpty()) {
        LibraryDao::updateNodesParentAndOrder(entries);
    }

    // 持久化智能文件夹自定义排序
    QStandardItem* sfHeader = findNode(m_model->invisibleRootItem(), LibraryNodeKind::SectionSmartFolder);
    if (sfHeader) {
        QList<QPair<int, int>> sfOrderList;
        int sfOrder = 1;
        for (int i = 0; i < sfHeader->rowCount(); ++i) {
            QStandardItem* sfChild = sfHeader->child(i);
            if (!sfChild) continue;
            LibraryNodeKind kind = static_cast<LibraryNodeKind>(sfChild->data(NodeKindRole).toInt());
            if (kind == LibraryNodeKind::SmartFolder) {
                int sfId = sfChild->data(SmartFolderIdRole).toInt();
                if (sfId > 0) {
                    sfOrderList.append(qMakePair(sfId, sfOrder++));
                }
            }
        }
        if (!sfOrderList.isEmpty()) {
            SmartFolderDao::updateSmartFolderOrder(sfOrderList);
        }
    }
}

void LibraryDragController::sortCategoriesByName(bool ascending) {
    if (!m_model) return;

    QStandardItem* sectionItem = findNode(m_model->invisibleRootItem(), LibraryNodeKind::SectionCustom);
    if (!sectionItem) return;

    std::function<void(QStandardItem*)> sortChildren = [&](QStandardItem* parentItem) {
        int rowCount = parentItem->rowCount();
        if (rowCount <= 1) return;

        QList<QStandardItem*> childItems;
        for (int i = rowCount - 1; i >= 0; --i) {
            QStandardItem* item = parentItem->takeRow(i).first();
            if (item) childItems.prepend(item);
        }

        std::sort(childItems.begin(), childItems.end(), [ascending](QStandardItem* a, QStandardItem* b) {
            return ascending ? (a->text().localeAwareCompare(b->text()) < 0)
                              : (a->text().localeAwareCompare(b->text()) > 0);
        });

        for (QStandardItem* item : childItems) {
            parentItem->appendRow(item);
            sortChildren(item);
        }
    };

    sortChildren(sectionItem);
    saveLibraryOrder();
    m_orderDirty = false;
}

} // namespace QuarkMeta
