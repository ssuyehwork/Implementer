#pragma once

#include <QAbstractItemView>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QStringList>
#include <QModelIndex>

namespace QuarkMeta {

/**
 * @brief Helper class for unified drag-and-drop behavior across item views.
 */
class DragDropEventFilter : public QObject {
    Q_OBJECT

public:
    explicit DragDropEventFilter(QAbstractItemView* targetView, QObject* parent = nullptr);
    ~DragDropEventFilter() override = default;

    static void install(QAbstractItemView* view);

signals:
    void pathsDropped(const QStringList& paths, const QModelIndex& targetIndex, Qt::DropAction action);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void clearDropHighlight();

    QAbstractItemView* m_targetView = nullptr;
    QPersistentModelIndex m_currentHoverDropIdx;
};

class ViewDragDropHelper {
public:
    static bool handleDragEnter(QAbstractItemView* view, QDragEnterEvent* event);
    static bool handleDragMove(QAbstractItemView* view, QDragMoveEvent* event);
    static bool handleDrop(QAbstractItemView* view, QDropEvent* event, QStringList& outPaths, QModelIndex& outTargetIdx, Qt::DropAction* outAction = nullptr, QPoint* outStartPos = nullptr);
    static void executeStartDrag(QAbstractItemView* view, Qt::DropActions supportedActions);
    static QPoint lastDragStartPos() { return s_lastDragStartPos; }

    static bool isDropTarget(const QWidget* widget, const QModelIndex& index);
    static void setHoverTarget(QAbstractItemView* view, const QModelIndex& index);
    static void clearHover(QAbstractItemView* view = nullptr);

private:
    static QAbstractItemView* s_hoverView;
    static QPersistentModelIndex s_hoverIndex;
    static QPoint s_lastDragStartPos;
};

} // namespace QuarkMeta
