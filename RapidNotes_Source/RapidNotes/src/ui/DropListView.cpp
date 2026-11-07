#include "DropListView.h"
#include "../core/ModelContract.h"
#include "ColumnItemDelegate.h"
#include "ViewDragDropHelper.h"
#include <QMouseEvent>

namespace QuarkMeta {

DropListView::DropListView(QWidget* parent) : QListView(parent) {
    setDragEnabled(true);
    DragDropEventFilter::install(this);
}

void DropListView::startDrag(Qt::DropActions supportedActions) {
    ViewDragDropHelper::executeStartDrag(this, supportedActions);
}

void DropListView::mousePressEvent(QMouseEvent* event) {
    if (event && event->button() == Qt::LeftButton) {
        QModelIndex idx = indexAt(event->pos());
        if (idx.isValid() && idx.data(SectionHeaderRole).toBool()) {
            emit sectionHeaderClicked(idx);
            event->accept();
            return;
        }
        if (!idx.isValid()) {
            emit blankSpaceClicked();
        }
    }
    QListView::mousePressEvent(event);
}

void DropListView::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event && event->button() == Qt::LeftButton) {
        QModelIndex idx = indexAt(event->pos());
        if (idx.isValid() && idx.data(SectionHeaderRole).toBool()) {
            event->accept();
            return;
        }
        if (!idx.isValid()) {
            emit blankSpaceDoubleClicked();
            event->accept();
            return;
        }
    }
    QListView::mouseDoubleClickEvent(event);
}

} // namespace QuarkMeta
