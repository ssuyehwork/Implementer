#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

namespace QuarkMeta {

class QuickLookMinimap;

class QuickLookGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit QuickLookGraphicsView(QWidget* parent = nullptr);
    void setPixmap(const QPixmap& pixmap);
    void fitImage();
    void setZoomOriginal();
    void rotateClockwise();
    void flipHorizontal();
    void clear();
    bool isFitMode() const { return m_isFitMode; }

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void updateCursor();
    void updateMinimap();

    QGraphicsScene* m_scene = nullptr;
    QGraphicsPixmapItem* m_pixmapItem = nullptr;
    double m_currentScale = 1.0;
    bool m_isFitMode = true;
    QuickLookMinimap* m_minimap = nullptr;
};

} // namespace QuarkMeta
