#pragma once

#include <QObject>
#include <QStandardItemModel>
#include <QPointer>
#include "DropTreeView.h"
#include "LibraryNodeKind.h"

namespace QuarkMeta {

/**
 * @brief 库树拖拽与排序控制器
 * 收拢内部拖拽验证、放置有效性判定、层级移动操作、展开与选中状态恢复、手动排序与排序落库。
 */
class LibraryDragController : public QObject {
    Q_OBJECT
public:
    explicit LibraryDragController(DropTreeView* treeView, QStandardItemModel* model, QObject* parent = nullptr);
    ~LibraryDragController() override = default;

    void setupDragDropValidators();

    void markOrderDirty();
    void flushOrder();
    void saveLibraryOrder();
    void sortCategoriesByName(bool ascending);

    void setIsLoading(bool loading) { m_isLoading = loading; }

public slots:
    void onInternalMoveRequested(const QList<QPersistentModelIndex>& sources, const QModelIndex& target, InternalDropZone zone);

signals:
    void requestReloadLibrary();

private:
    void saveExpandedStates(QStandardItem* pItem, QMap<int, bool>& expandedMap);
    void restoreExpandedStates(QStandardItem* pItem, const QMap<int, bool>& expandedMap);

    QPointer<DropTreeView> m_treeView;
    QPointer<QStandardItemModel> m_model;
    bool m_isLoading = false;
    bool m_orderDirty = false;
};

} // namespace QuarkMeta
