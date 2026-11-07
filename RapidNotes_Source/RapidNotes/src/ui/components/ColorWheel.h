#pragma once

#include <QWidget>
#include <QColor>

namespace QuarkMeta {

class ColorWheel : public QWidget {
    Q_OBJECT
public:
    explicit ColorWheel(QWidget* parent = nullptr);
    ~ColorWheel() override = default;

    int hue() const { return m_hue; }
    int precision() const { return m_precision; }

    void setValues(int hue, int precision);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void changed(int hue, int precision);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    enum class DragTarget {
        None,
        DragA,
        DragC
    };

    double wheelRadius() const;
    QPointF getCenterPoint() const;
    QPointF getHandleAPosition() const;
    QPointF getHandleCPosition() const;

    void handleDragMove(const QPointF& mousePos);
    void updateCursorShape(const QPoint& pos);

    int m_hue = 0;           // 0~359
    int m_precision = 50;    // 0~100 (0在边缘, 100在圆心)
    DragTarget m_dragTarget = DragTarget::None;
};

} // namespace QuarkMeta
