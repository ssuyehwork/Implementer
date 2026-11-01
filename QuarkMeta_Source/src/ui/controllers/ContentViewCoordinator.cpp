#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "ContentViewCoordinator.h"
#include "../ContentPanel.h"
#include "../DropTreeView.h"
#include "../DropListView.h"
#include "../ColumnViewWidget.h"
#include "../JustifiedView.h"
#include "../models/DiskItemModel.h"
#include "../models/FilterProxyModel.h"
#include "../../core/CoreController.h"
#include <QHeaderView>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QDir>

namespace QuarkMeta {

ContentViewCoordinator::ContentViewCoordinator(ContentPanel* panel)
    : QObject(panel), m_panel(panel) {
}

QList<QAbstractItemView*> ContentViewCoordinator::currentActiveViews() const {
    QList<QAbstractItemView*> views;
    if (!m_panel) return views;

    auto mode = m_panel->currentViewMode();
    if (mode == ContentPanel::ColumnView) {
        if (m_panel->columnView() && m_panel->columnView()->activePane()) {
            if (m_panel->columnView()->activePane()->listView()) {
                views << m_panel->columnView()->activePane()->listView();
            }
        }
    } else if (mode == ContentPanel::ListView) {
        if (m_panel->dropTreeView()) views << m_panel->dropTreeView();
    } else { // GridView / JustifiedViewMode
        if (m_panel->gridView()) views << m_panel->gridView();
    }
    return views;
}

QAbstractItemView* ContentViewCoordinator::activeItemView() const {
    if (!m_panel) return nullptr;

    auto mode = m_panel->currentViewMode();
    if (mode == ContentPanel::ColumnView) {
        if (m_panel->columnView() && m_panel->columnView()->activePane()) {
            return m_panel->columnView()->activePane()->listView();
        }
        return nullptr;
    }

    if (mode == ContentPanel::ListView) {
        return m_panel->dropTreeView();
    }

    return m_panel->gridView();
}

QModelIndex ContentViewCoordinator::toSourceIndex(const QModelIndex& idx, const QAbstractItemModel* target) {
    if (!idx.isValid() || idx.data(SectionHeaderRole).toBool()) return QModelIndex();

    QModelIndex cur = idx;
    while (cur.isValid() && cur.model() != target) {
        auto* proxy = qobject_cast<const QAbstractProxyModel*>(cur.model());
        if (!proxy) break;
        cur = proxy->mapToSource(cur);
    }

    if (cur.isValid() && cur.model() == target) {
        return cur;
    }
    return QModelIndex();
}


QModelIndexList ContentViewCoordinator::getSelectedIndexes() const {
    QModelIndexList res;
    if (!m_panel) return res;

    if (m_panel->currentViewMode() == ContentPanel::ColumnView) {
        if (m_panel->columnView() && m_panel->columnView()->activePane()) {
            auto* view = m_panel->columnView()->activePane()->listView();
            if (view && view->selectionModel() && view->selectionModel()->hasSelection()) {
                for (const auto& idx : view->selectionModel()->selectedIndexes()) {
                    if (idx.column() == static_cast<int>(FileListColumn::Name)) res.append(idx);
                }
            }
        }
        return res;
    }

    QList<QAbstractItemView*> views = currentActiveViews();
    for (auto* view : views) {
        if (view && view->selectionModel() && view->selectionModel()->hasSelection()) {
            for (const auto& idx : view->selectionModel()->selectedIndexes()) {
                if (idx.column() == static_cast<int>(FileListColumn::Name)) {
                    res.append(idx);
                }
            }
        }
    }
    return res;
}

QStringList ContentViewCoordinator::getSelectedPaths() const {
    QStringList paths;
    for (const auto& idx : getSelectedIndexes()) {
        if (idx.column() == static_cast<int>(FileListColumn::Name)) {
            QString p = idx.data(PathRole).toString();
            if (!p.isEmpty()) paths << p;
        }
    }
    return paths;
}

void ContentViewCoordinator::applyFilterStateToAllViews(const FilterState& state) {
    if (!m_panel) return;

    m_panel->applyFilters(state);
}

void ContentViewCoordinator::restoreSelections(const QSet<QString>& selectedPaths, bool isPendingEdit) {
    if (!m_panel || selectedPaths.isEmpty()) return;

    if (m_panel->currentViewMode() == ContentPanel::ColumnView) {
        if (m_panel->columnView()) {
            QString firstPath = *selectedPaths.begin();
            ColumnViewPane* targetPane = m_panel->columnView()->findPaneForPath(firstPath);
            if (targetPane) {
                targetPane->setPendingSelectPaths(selectedPaths, isPendingEdit);
            }
        }
        return;
    }

    QList<QAbstractItemView*> views = currentActiveViews();
    for (auto* view : views) {
        if (!view || !view->selectionModel() || !view->model()) continue;
        QAbstractItemModel* viewModeModel = view->model();

        QItemSelection sel;
        QModelIndex lastIdx;

        auto matchesAny = [&selectedPaths](const QString& itemPath) {
            QString nItem = QDir::toNativeSeparators(QDir::cleanPath(itemPath));
            for (const QString& sp : selectedPaths) {
                QString nTarget = QDir::toNativeSeparators(QDir::cleanPath(sp));
                if (QString::compare(nItem, nTarget, Qt::CaseInsensitive) == 0) {
                    return true;
                }
            }
            return false;
        };

        int total = viewModeModel->rowCount();
        for (int r = 0; r < total; ++r) {
            QModelIndex idx = viewModeModel->index(r, 0);
            if (idx.data(SectionHeaderRole).toBool()) continue;
            QString p = idx.data(PathRole).toString();
            if (matchesAny(p)) {
                sel.select(idx, idx);
                lastIdx = idx;
            }
        }

        if (lastIdx.isValid()) {
            view->setFocus();
            view->setCurrentIndex(lastIdx);
            view->selectionModel()->select(sel, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            view->scrollTo(lastIdx, QAbstractItemView::EnsureVisible);
            if (m_panel) {
                emit m_panel->selectionChanged(m_panel->getSelectedPaths());
            }

            if (isPendingEdit) {
                QPointer<QAbstractItemView> weakView(view);
                QTimer::singleShot(0, m_panel, [weakView, lastIdx]() {
                    if (weakView && lastIdx.isValid()) {
                        weakView->setFocus();
                        weakView->setCurrentIndex(lastIdx);
                        weakView->edit(lastIdx);
                    }
                });
            }
        }
    }
}

void ContentViewCoordinator::refreshVisibleThumbnails() {
    if (!m_panel || !m_panel->diskModel() || CoreController::isShuttingDown()) return;

    QList<QAbstractItemView*> views = currentActiveViews();
    QSet<int> visibleRows;

    for (auto* view : views) {
        if (!view || !view->viewport()) continue;
        QAbstractItemModel* model = view->model();
        if (!model || model->rowCount() == 0) continue;

        auto* jv = qobject_cast<JustifiedView*>(view);
        if (jv) {
            if (!jv->isLayoutReady()) return;
            int scrollY = jv->verticalScrollBar() ? jv->verticalScrollBar()->value() : 0;
            int vpH = jv->viewport()->height();
            int bufferH = static_cast<int>(vpH * 1.5);
            QList<int> rangeRows = jv->rowsInRange(qMax(0, scrollY - bufferH), scrollY + vpH + bufferH);
            for (int r : rangeRows) {
                QModelIndex idx = model->index(r, 0);
                QModelIndex srcIdx = toSourceIndex(idx, m_panel->diskModel());
                if (srcIdx.isValid()) visibleRows.insert(srcIdx.row());
            }
            continue;
        }

        QSet<int> rows = calculateVisibleSourceRows(view, m_panel->diskModel());
        visibleRows.unite(rows);
    }

    if (!visibleRows.isEmpty()) {
        m_panel->diskModel()->loadThumbnailsForRows(visibleRows.values());
    }
}

QSet<int> ContentViewCoordinator::calculateVisibleSourceRows(QAbstractItemView* view, const QAbstractItemModel* targetDiskModel) {
    QSet<int> visibleRows;
    if (!view || !view->viewport() || !targetDiskModel) return visibleRows;
    QAbstractItemModel* model = view->model();
    if (!model || model->rowCount() == 0) return visibleRows;

    QRect vpRect = view->viewport()->rect();
    QModelIndex topIdx = view->indexAt(vpRect.topLeft());
    QModelIndex btmIdx = view->indexAt(vpRect.bottomRight());

    int top = topIdx.isValid() ? qMax(0, topIdx.row() - 15) : 0;
    int bottom = btmIdx.isValid() ? qMin(model->rowCount() - 1, btmIdx.row() + 15) : model->rowCount() - 1;

    for (int r = top; r <= bottom; ++r) {
        QModelIndex idx = model->index(r, 0);
        if (idx.data(SectionHeaderRole).toBool()) continue;
        QModelIndex srcIdx = toSourceIndex(idx, targetDiskModel);
        if (srcIdx.isValid()) visibleRows.insert(srcIdx.row());
    }
    return visibleRows;
}

void ContentViewCoordinator::updateGridSize(int zoomLevel) {
    if (!m_panel) return;
    if (m_panel->gridView()) {
        auto* jv = qobject_cast<JustifiedView*>(m_panel->gridView());
        if (jv) jv->setTargetRowHeight(zoomLevel);
    }
    if (m_panel->dropTreeView()) {
        int iconSz = qMax(16, zoomLevel - 8);
        m_panel->dropTreeView()->setIconSize(QSize(iconSz, iconSz));
        if (auto* header = qobject_cast<ContentHeaderView*>(m_panel->dropTreeView()->header())) {
            header->setZoomLevel(zoomLevel);
        }
        m_panel->dropTreeView()->doItemsLayout();
    }
}

void ContentViewCoordinator::installActivationFilters() {
    if (!m_panel) return;

    auto installOnce = [this](QObject* obj) {
        if (!obj || m_filteredObjects.contains(obj)) return;
        obj->installEventFilter(m_panel);
        m_filteredObjects.insert(obj);
    };

    for (QAbstractItemView* view : currentActiveViews()) {
        if (!view) continue;
        installOnce(view);
        installOnce(view->viewport());
    }
}

} // namespace QuarkMeta
