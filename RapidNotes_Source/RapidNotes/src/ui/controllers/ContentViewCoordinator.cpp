#include "ContentViewCoordinator.h"
#include "../ContentPanel.h"

namespace RapidNotes {

ContentViewCoordinator::ContentViewCoordinator(ContentPanel* panel)
    : QObject(panel), m_panel(panel) {
}

QList<QAbstractItemView*> ContentViewCoordinator::currentActiveViews() const {
    if (!m_panel) return {};
    QAbstractItemView* view = m_panel->activeItemView();
    return view ? QList<QAbstractItemView*>{view} : QList<QAbstractItemView*>{};
}

QAbstractItemView* ContentViewCoordinator::activeItemView() const {
    return m_panel ? m_panel->activeItemView() : nullptr;
}

QModelIndexList ContentViewCoordinator::getSelectedIndexes() const {
    QAbstractItemView* view = activeItemView();
    if (view && view->selectionModel()) {
        return view->selectionModel()->selectedIndexes();
    }
    return {};
}

QStringList ContentViewCoordinator::getSelectedPaths() const {
    return m_panel ? m_panel->getSelectedPaths() : QStringList();
}

void ContentViewCoordinator::restoreSelections(const QSet<QString>& selectedPaths, bool isPendingEdit) {
    Q_UNUSED(selectedPaths);
    Q_UNUSED(isPendingEdit);
}

QModelIndex ContentViewCoordinator::toSourceIndex(const QModelIndex& idx, const QAbstractItemModel* target) {
    Q_UNUSED(target);
    return idx;
}

QSet<int> ContentViewCoordinator::calculateVisibleSourceRows(QAbstractItemView* view, const QAbstractItemModel* targetDiskModel) {
    Q_UNUSED(view);
    Q_UNUSED(targetDiskModel);
    return {};
}

void ContentViewCoordinator::applyFilterStateToAllViews(const FilterState& state) {
    Q_UNUSED(state);
}

void ContentViewCoordinator::refreshVisibleThumbnails() {
}

void ContentViewCoordinator::updateGridSize(int zoomLevel) {
    Q_UNUSED(zoomLevel);
}

void ContentViewCoordinator::installActivationFilters() {
}

} // namespace RapidNotes
