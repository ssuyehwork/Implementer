#include "QuickLookGraphicsView.h"
#include "QuickLookMinimap.h"
#include "QuickLookWindow.h"
#include "../core/AppConfig.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>

namespace QuarkMeta {

QuickLookGraphicsView::QuickLookGraphicsView(QWidget* parent) : QGraphicsView(parent) {
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    m_pixmapItem = new QGraphicsPixmapItem();
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    m_scene->addItem(m_pixmapItem);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setObjectName("QLGraphicsView");


    m_minimap = new QuickLookMinimap(this);

    connect(m_minimap, &QuickLookMinimap::centerRequested, this, [this](double xRatio, double yRatio) {
        if (!m_pixmapItem || m_pixmapItem->pixmap().isNull()) return;
        QRectF totalRect = m_pixmapItem->boundingRect();
        QPointF targetCenter(xRatio * totalRect.width(), yRatio * totalRect.height());
        centerOn(targetCenter);
        updateMinimap();
    });
}

void QuickLookGraphicsView::setPixmap(const QPixmap& pixmap) {
    m_pixmapItem->setPixmap(pixmap);
    m_scene->setSceneRect(m_pixmapItem->boundingRect());

    if (m_minimap) {
        m_minimap->setPixmap(pixmap);
    }

    bool defaultFit = AppConfig::instance().getValue("QuickLook/ImageFitMode", false).toBool();
    if (defaultFit) {
        fitImage();
    } else {
        setZoomOriginal();
    }
    updateMinimap();
}

void QuickLookGraphicsView::clear() {
    m_pixmapItem->setPixmap(QPixmap());
    m_scene->setSceneRect(QRectF());
    resetTransform();
    m_currentScale = 1.0;
    m_isFitMode = false;
    if (m_minimap) m_minimap->clear();
    updateCursor();
}

void QuickLookGraphicsView::fitImage() {
    if (!m_pixmapItem || m_pixmapItem->pixmap().isNull()) return;

    resetTransform();
    m_scene->setSceneRect(m_pixmapItem->boundingRect());
    fitInView(m_pixmapItem, Qt::KeepAspectRatio);

    m_currentScale = transform().m11();
    m_isFitMode = true;
    AppConfig::instance().setValue("QuickLook/ImageFitMode", true);
    AppConfig::instance().sync();
    updateCursor();
}

void QuickLookGraphicsView::setZoomOriginal() {
    if (!m_pixmapItem || m_pixmapItem->pixmap().isNull()) return;

    resetTransform();
    m_scene->setSceneRect(m_pixmapItem->boundingRect());
    m_currentScale = 1.0;
    m_isFitMode = false;
    AppConfig::instance().setValue("QuickLook/ImageFitMode", false);
    AppConfig::instance().sync();
    updateCursor();
}

void QuickLookGraphicsView::rotateClockwise() {
    rotate(90);
    updateCursor();
}

void QuickLookGraphicsView::flipHorizontal() {
    scale(-1, 1);
    updateCursor();
}

void QuickLookGraphicsView::wheelEvent(QWheelEvent* event) {
    if (!m_pixmapItem || m_pixmapItem->pixmap().isNull()) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    double factor = 1.15;
    if (event->angleDelta().y() < 0) {
        factor = 1.0 / factor;
    }

    double newScale = m_currentScale * factor;
    if (newScale < 0.1) {
        factor = 0.1 / m_currentScale;
        newScale = 0.1;
    } else if (newScale > 10.0) {
        factor = 10.0 / m_currentScale;
        newScale = 10.0;
    }

    if (qFuzzyCompare(newScale, m_currentScale)) {
        return;
    }

    m_isFitMode = false;
    scale(factor, factor);
    m_currentScale = newScale;
    updateCursor();
    updateMinimap();
}

void QuickLookGraphicsView::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QuickLookWindow::instance().closePreview();
        event->accept();
    } else {
        QGraphicsView::mouseDoubleClickEvent(event);
    }
}

void QuickLookGraphicsView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    if (m_isFitMode) {
        fitImage();
    }
    updateMinimap();
}

void QuickLookGraphicsView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        bool exceeds = (m_pixmapItem->boundingRect().width() * m_currentScale > viewport()->width()) ||
                       (m_pixmapItem->boundingRect().height() * m_currentScale > viewport()->height());
        if (exceeds) {
            setCursor(Qt::ClosedHandCursor);
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void QuickLookGraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    QGraphicsView::mouseReleaseEvent(event);
    updateCursor();
}

void QuickLookGraphicsView::mouseMoveEvent(QMouseEvent* event) {
    QGraphicsView::mouseMoveEvent(event);
    if (event->buttons() & Qt::LeftButton) {
        updateMinimap();
    }
}

void QuickLookGraphicsView::updateMinimap() {
    if (!m_minimap || !m_pixmapItem || m_pixmapItem->pixmap().isNull()) {
        if (m_minimap) m_minimap->hide();
        return;
    }

    QRectF totalRect = m_pixmapItem->boundingRect();
    QRectF visibleRect = mapToScene(viewport()->rect()).boundingRect();

    bool exceedsHorizontal = visibleRect.width() < totalRect.width() * 0.99;
    bool exceedsVertical = visibleRect.height() < totalRect.height() * 0.99;

    if (exceedsHorizontal || exceedsVertical) {
        m_minimap->updateViewportRect(visibleRect, totalRect);

        int mx = viewport()->width() - m_minimap->width() - 20;
        int my = viewport()->height() - m_minimap->height() - 20;
        m_minimap->move(mx, my);

        m_minimap->show();
        m_minimap->raise();
    } else {
        m_minimap->hide();
    }
}

void QuickLookGraphicsView::updateCursor() {
    if (!m_pixmapItem || m_pixmapItem->pixmap().isNull()) {
        setCursor(Qt::ArrowCursor);
        return;
    }

    bool exceedsHorizontal = m_pixmapItem->boundingRect().width() * m_currentScale > viewport()->width();
    bool exceedsVertical = m_pixmapItem->boundingRect().height() * m_currentScale > viewport()->height();

    if (exceedsHorizontal || exceedsVertical) {
        setCursor(Qt::OpenHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

} // namespace QuarkMeta
