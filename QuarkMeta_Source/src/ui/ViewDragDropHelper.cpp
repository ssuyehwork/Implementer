#include "ViewDragDropHelper.h"
#include "../core/ModelContract.h"
#include <QMimeData>
#include <QDrag>
#include <QPixmap>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QItemSelectionModel>
#include <QApplication>

namespace QuarkMeta {

DragDropEventFilter::DragDropEventFilter(QAbstractItemView* targetView, QObject* parent)
    : QObject(parent ? parent : targetView), m_targetView(targetView) {
}

void DragDropEventFilter::install(QAbstractItemView* view) {
    if (!view) return;
    view->setAcceptDrops(true);
    auto* filter = new DragDropEventFilter(view, view);
    view->installEventFilter(filter);
    if (view->viewport()) {
        view->viewport()->installEventFilter(filter);
    }

    // 🚀【核心修复】：将事件过滤器的 pathsDropped 动态信号直接桥接至宿主视图的 pathsDropped 信号
    QObject::connect(filter, SIGNAL(pathsDropped(QStringList,QModelIndex,Qt::DropAction)),
                     view, SIGNAL(pathsDropped(QStringList,QModelIndex,Qt::DropAction)));
}

void DragDropEventFilter::clearDropHighlight() {
    if (m_currentHoverDropIdx.isValid() && m_targetView && m_targetView->model()) {
        const_cast<QAbstractItemModel*>(m_targetView->model())->setData(m_currentHoverDropIdx, false, IsDropTargetRole);
        m_currentHoverDropIdx = QModelIndex();
        if (m_targetView->viewport()) m_targetView->viewport()->update();
    }
}

bool DragDropEventFilter::eventFilter(QObject* watched, QEvent* event) {
    if (!m_targetView) return QObject::eventFilter(watched, event);

    if (event->type() == QEvent::DragEnter) {
        auto* dragEvent = static_cast<QDragEnterEvent*>(event);
        if (dragEvent->mimeData() && dragEvent->mimeData()->hasFormat("application/x-quarkmeta-tree-nodes")) {
            return false;
        }
        if (ViewDragDropHelper::handleDragEnter(m_targetView, dragEvent)) {
            return true;
        }
    } else if (event->type() == QEvent::DragMove) {
        auto* moveEvent = static_cast<QDragMoveEvent*>(event);
        if (moveEvent->mimeData() && moveEvent->mimeData()->hasFormat("application/x-quarkmeta-tree-nodes")) {
            return false;
        }
        QModelIndex hoverIdx = m_targetView->indexAt(moveEvent->position().toPoint());
        if (hoverIdx.isValid()) {
            int nodeId = hoverIdx.data(Qt::UserRole + 1).toInt();
            QString typeStr = hoverIdx.data(TypeRole).toString();
            bool isTargetable = !hoverIdx.data(SectionHeaderRole).toBool() &&
                                (typeStr == "folder" || typeStr == "category" ||
                                 nodeId > 0 || nodeId == -2 ||
                                 hoverIdx.data(Qt::UserRole + 2).toBool());
            if (isTargetable) {
                ViewDragDropHelper::setHoverTarget(m_targetView, hoverIdx);
            } else {
                ViewDragDropHelper::clearHover(m_targetView);
            }
        } else {
            ViewDragDropHelper::clearHover(m_targetView);
        }
        if (ViewDragDropHelper::handleDragMove(m_targetView, moveEvent)) {
            return true;
        }
    } else if (event->type() == QEvent::DragLeave) {
        clearDropHighlight();
        ViewDragDropHelper::clearHover(m_targetView);
        return false;
    } else if (event->type() == QEvent::Drop) {
        clearDropHighlight();
        auto* dropEv = static_cast<QDropEvent*>(event);
        if (dropEv->mimeData() && dropEv->mimeData()->hasFormat("application/x-quarkmeta-tree-nodes")) {
            return false;
        }
        QStringList paths;
        QModelIndex targetIdx;
        Qt::DropAction action = Qt::CopyAction;
        if (ViewDragDropHelper::handleDrop(m_targetView, dropEv, paths, targetIdx, &action)) {
            emit pathsDropped(paths, targetIdx, action);
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

QAbstractItemView* ViewDragDropHelper::s_hoverView = nullptr;
QPersistentModelIndex ViewDragDropHelper::s_hoverIndex;
QPoint ViewDragDropHelper::s_lastDragStartPos;

bool ViewDragDropHelper::isDropTarget(const QWidget* widget, const QModelIndex& index) {
    if (!widget || !s_hoverView || !s_hoverIndex.isValid()) return false;
    if (widget == s_hoverView || widget == s_hoverView->viewport()) {
        return s_hoverIndex == index;
    }
    return false;
}

void ViewDragDropHelper::setHoverTarget(QAbstractItemView* view, const QModelIndex& index) {
    if (!view) return;
    if (s_hoverView != view || s_hoverIndex != index) {
        QAbstractItemView* oldView = s_hoverView;
        s_hoverView = view;
        s_hoverIndex = index;
        if (oldView && oldView->viewport()) oldView->viewport()->update();
        if (view->viewport()) view->viewport()->update();
    }
}

void ViewDragDropHelper::clearHover(QAbstractItemView* view) {
    if (view && s_hoverView != view) return;
    QAbstractItemView* oldView = s_hoverView;
    s_hoverView = nullptr;
    s_hoverIndex = QPersistentModelIndex();
    if (oldView && oldView->viewport()) {
        oldView->viewport()->update();
    }
}

bool ViewDragDropHelper::handleDragEnter(QAbstractItemView* /*view*/, QDragEnterEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        return true;
    }
    return false;
}

bool ViewDragDropHelper::handleDragMove(QAbstractItemView* /*view*/, QDragMoveEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        return true;
    }
    return false;
}

bool ViewDragDropHelper::handleDrop(QAbstractItemView* view, QDropEvent* event, QStringList& outPaths, QModelIndex& outTargetIdx, Qt::DropAction* outAction, QPoint* outStartPos) {
    outPaths.clear();
    outTargetIdx = QModelIndex();
    if (outStartPos) *outStartPos = s_lastDragStartPos;
    if (outAction) *outAction = event->dropAction();

    if (event->mimeData() && event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl& url : urls) {
            QString localPath = url.toLocalFile();
            if (!localPath.isEmpty()) {
                outPaths.append(QDir::toNativeSeparators(localPath));
            }
        }
        outTargetIdx = view->indexAt(event->position().toPoint());
        if (!outPaths.isEmpty()) {
            event->acceptProposedAction();
            clearHover(view);
            return true;
        }
    }
    clearHover(view);
    return false;
}

void ViewDragDropHelper::executeStartDrag(QAbstractItemView* view, Qt::DropActions supportedActions) {
    s_lastDragStartPos = QCursor::pos();
    if (!view || !view->selectionModel()) return;
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) return;

    QList<QUrl> urls;
    for (const QModelIndex& idx : indexes) {
        if (idx.column() != static_cast<int>(FileListColumn::Name)) continue;

        QString path = idx.data(PathRole).toString();
        if (path.isEmpty()) {
            path = idx.data(Qt::UserRole + 1).toString();
        }

        if (!path.isEmpty() && QFileInfo::exists(path)) {
            urls.append(QUrl::fromLocalFile(path));
        }
    }

    if (urls.isEmpty()) return;

    QMimeData* mimeData = new QMimeData();
    mimeData->setUrls(urls);

    QDrag* drag = new QDrag(view);
    drag->setMimeData(mimeData);

    QPixmap pixmap(1, 1);
    pixmap.fill(Qt::transparent);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(0, 0));

    bool isCtrl = (QApplication::keyboardModifiers() & Qt::ControlModifier);
    Qt::DropAction defaultAction = isCtrl ? Qt::CopyAction : Qt::MoveAction;

    drag->exec(supportedActions | Qt::CopyAction | Qt::MoveAction, defaultAction);
    s_lastDragStartPos = QPoint();
}

} // namespace QuarkMeta
