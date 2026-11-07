#pragma once

#include <QObject>
#include <QAbstractItemView>
#include <QModelIndexList>
#include <QSet>
#include <QString>
#include <QSortFilterProxyModel>
#include "FilterStateModel.h"

namespace QuarkMeta {

class ContentPanel;
class DiskItemModel;

/**
 * @brief 视图与视口协调器：负责 ContentPanel 的多视图选区收集、视口几何探测与分组折叠高度维护
 */
class ContentViewCoordinator : public QObject {
    Q_OBJECT

public:
    explicit ContentViewCoordinator(ContentPanel* panel);
    ~ContentViewCoordinator() override = default;

    // 视图探测与代理模型归一化
    QList<QAbstractItemView*> currentActiveViews() const;
    QAbstractItemView* activeItemView() const;

    // 选区与焦点计算
    QModelIndexList getSelectedIndexes() const;
    QStringList getSelectedPaths() const;
    void restoreSelections(const QSet<QString>& selectedPaths, bool isPendingEdit);

    static QModelIndex toSourceIndex(const QModelIndex& idx, const QAbstractItemModel* target);
    static QSet<int> calculateVisibleSourceRows(QAbstractItemView* view, const QAbstractItemModel* targetDiskModel);

    // 统一 FilterState 广播
    void applyFilterStateToAllViews(const FilterState& state);

    // 缩略图视口行号探测与触发
    void refreshVisibleThumbnails();

    // 视图缩放几何适配
    void updateGridSize(int zoomLevel);

    // 激活态事件过滤器统一安装入口：修复 panelActivated 从未被真正触发的问题
    void installActivationFilters();

private:
    ContentPanel* m_panel = nullptr;
    QSet<QObject*> m_filteredObjects;
};

} // namespace QuarkMeta
