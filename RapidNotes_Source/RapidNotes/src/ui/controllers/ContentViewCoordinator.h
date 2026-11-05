#pragma once

#include <QObject>
#include <QAbstractItemView>
#include <QModelIndexList>
#include <QSet>
#include <QString>
#include <QSortFilterProxyModel>
#include "../FilterStateModel.h"

namespace RapidNotes {

class ContentPanel;
class DiskItemModel;

/**
 * @brief 视图与视口协调器：负责 ContentPanel 的多视图选区收集与视口协调
 */
class ContentViewCoordinator : public QObject {
    Q_OBJECT

public:
    explicit ContentViewCoordinator(ContentPanel* panel);
    ~ContentViewCoordinator() override = default;

    QList<QAbstractItemView*> currentActiveViews() const;
    QAbstractItemView* activeItemView() const;

    QModelIndexList getSelectedIndexes() const;
    QStringList getSelectedPaths() const;
    void restoreSelections(const QSet<QString>& selectedPaths, bool isPendingEdit);

    static QModelIndex toSourceIndex(const QModelIndex& idx, const QAbstractItemModel* target);
    static QSet<int> calculateVisibleSourceRows(QAbstractItemView* view, const QAbstractItemModel* targetDiskModel);

    void applyFilterStateToAllViews(const FilterState& state);
    void refreshVisibleThumbnails();
    void updateGridSize(int zoomLevel);
    void installActivationFilters();

private:
    ContentPanel* m_panel = nullptr;
    QSet<QObject*> m_filteredObjects;
};

} // namespace RapidNotes
