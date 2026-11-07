#include "ColumnBlankCanvasWidget.h"
#include "ColumnViewWidget.h"
#include "ContentPanel.h"
#include <QAbstractItemView>
#include <QMimeData>
#include <QPainter>
#include <QUrl>

namespace QuarkMeta {

ColumnBlankCanvasWidget::ColumnBlankCanvasWidget(ColumnViewWidget* columnView, ContentPanel* contentPanel, QWidget* parent)
    : QWidget(parent), m_columnView(columnView), m_contentPanel(contentPanel) {
    setObjectName("ColumnBlankCanvasWidget");
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &ColumnBlankCanvasWidget::onContextMenuRequested);
}

void ColumnBlankCanvasWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_columnView) {
        m_columnView->goUpColumn();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void ColumnBlankCanvasWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        m_isDragHover = true;
        update();
    }
}

void ColumnBlankCanvasWidget::dragLeaveEvent(QDragLeaveEvent* event) {
    m_isDragHover = false;
    update();
    QWidget::dragLeaveEvent(event);
}

void ColumnBlankCanvasWidget::dropEvent(QDropEvent* event) {
    m_isDragHover = false;
    update();
    if (m_contentPanel && m_columnView && m_columnView->rightmostPane()) {
        QString targetDir = m_columnView->rightmostPane()->currentPath();
        QStringList paths;
        for (const QUrl& url : event->mimeData()->urls()) {
            paths << url.toLocalFile();
        }
        if (!paths.isEmpty()) {
            m_contentPanel->onPathsDropped(paths, QModelIndex(), targetDir);
            event->acceptProposedAction();
        }
    }
}

void ColumnBlankCanvasWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    if (m_isDragHover) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        QColor highlightColor("#3498db");
        highlightColor.setAlphaF(0.35f);
        painter.fillRect(rect(), highlightColor);
        painter.setPen(QPen(QColor("#3498db"), 2, Qt::DashLine));
        painter.drawRect(rect().adjusted(1, 1, -1, -1));
    }
}

void ColumnBlankCanvasWidget::onContextMenuRequested(const QPoint& pos) {
    if (m_contentPanel) {
        QPoint globalPos = mapToGlobal(pos);
        QAbstractItemView* view = m_contentPanel->activeItemView();
        if (view && view->viewport()) {
            QPoint viewPos = view->viewport()->mapFromGlobal(globalPos);
            m_contentPanel->onCustomContextMenuRequested(view, viewPos);
        } else {
            m_contentPanel->onCustomContextMenuRequested(nullptr, globalPos);
        }
    }
}

} // namespace QuarkMeta
