#include "ColorWheel.h"
#include "../ToolTipOverlay.h"
#include <QPainter>
#include <QPainterPath>
#include <QConicalGradient>
#include <QRadialGradient>
#include <QMouseEvent>
#include <QtMath>

namespace QuarkMeta {

ColorWheel::ColorWheel(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

QSize ColorWheel::sizeHint() const {
    return QSize(160, 160);
}

QSize ColorWheel::minimumSizeHint() const {
    return QSize(80, 80);
}

double ColorWheel::wheelRadius() const {
    int side = qMin(width() - 16, 160);
    side = qMax(side, 40);
    return side / 2.0 - 12.0; // 留 12px 容纳手柄 A 的外半径与描边线
}

QPointF ColorWheel::getCenterPoint() const {
    return QPointF(width() / 2.0, height() / 2.0);
}

QPointF ColorWheel::getHandleAPosition() const {
    QPointF center = getCenterPoint();
    double radius = wheelRadius();
    double rad = qDegreesToRadians(double(m_hue));
    return QPointF(center.x() + radius * qCos(rad), center.y() - radius * qSin(rad));
}

QPointF ColorWheel::getHandleCPosition() const {
    QPointF center = getCenterPoint();
    double radius = wheelRadius();
    double r = (1.0 - double(m_precision) / 100.0) * radius;
    double rad = qDegreesToRadians(double(m_hue));
    return QPointF(center.x() + r * qCos(rad), center.y() - r * qSin(rad));
}

void ColorWheel::setValues(int hue, int precision) {
    int newHue = qBound(0, hue, 359);
    int newPrecision = qBound(0, precision, 100);
    if (m_hue != newHue || m_precision != newPrecision) {
        m_hue = newHue;
        m_precision = newPrecision;
        update();
    }
}

void ColorWheel::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPointF center = getCenterPoint();
    double radius = wheelRadius();
    if (radius <= 0) return;

    // 1. 在圆形剪裁区域绘制色轮盘面
    QPainterPath clipPath;
    clipPath.addEllipse(center, radius, radius);

    p.save();
    p.setClipPath(clipPath);

    QConicalGradient conicalGrad(center, 0);
    conicalGrad.setColorAt(0.0,         QColor(255, 0, 0));
    conicalGrad.setColorAt(60.0 / 360,  QColor(255, 255, 0));
    conicalGrad.setColorAt(120.0 / 360, QColor(0, 255, 0));
    conicalGrad.setColorAt(180.0 / 360, QColor(0, 255, 255));
    conicalGrad.setColorAt(240.0 / 360, QColor(0, 0, 255));
    conicalGrad.setColorAt(300.0 / 360, QColor(255, 0, 255));
    conicalGrad.setColorAt(1.0,         QColor(255, 0, 0));

    p.setPen(Qt::NoPen);
    p.setBrush(conicalGrad);
    p.drawEllipse(center, radius, radius);

    QRadialGradient radialGrad(center, radius);
    radialGrad.setColorAt(0.0, QColor("#B0B0B0"));
    radialGrad.setColorAt(1.0, QColor(176, 176, 176, 0));
    p.setBrush(radialGrad);
    p.drawEllipse(center, radius, radius);

    p.restore();

    // 绘制色轮外圈细保护边线
    p.setPen(QPen(QColor(60, 60, 60), 1));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(center, radius, radius);

    // 计算手柄坐标：B (圆心), A (边缘), C (连线中间)
    QPointF posB = center;
    QPointF posA = getHandleAPosition();
    QPointF posC = getHandleCPosition();

    // 2. 绘制连线：从 C 到 A 的白色连线 (宽 2px)
    p.save();
    QPen linePen(Qt::white, 2.0);
    linePen.setCapStyle(Qt::RoundCap);
    p.setPen(linePen);
    p.drawLine(posB, posA);
    p.restore();

    // 3. 绘制点 C (中心点，半径 1，纯白实心)
    p.save();
    p.setPen(QPen(Qt::white, 2));
    p.setBrush(Qt::white);
    p.drawEllipse(posB, 1.0, 1.0); // C点半径值
    p.restore();

    // 辅助闭包：绘制点 A (外缘点，半径 2，实心彩色)
    auto drawHandleA = [&]() {
        p.save();
        QColor pureHueColor = QColor::fromHsv(m_hue, 255, 255);
        p.setPen(QPen(Qt::white, 2));
        p.setBrush(pureHueColor);
        p.drawEllipse(posA, 6.0, 6.0); // A点半径值
        p.restore();
    };

    // 辅助闭包：绘制点 B (滑杆点，半径 1.5，实心当前拾取色)
    auto drawHandleC = [&]() {
        p.save();
        double t = 1.0 - double(m_precision) / 100.0;
        QColor pureHueColor = QColor::fromHsv(m_hue, 255, 255);
        int r = qRound(176 + (pureHueColor.red() - 176) * t);
        int g = qRound(176 + (pureHueColor.green() - 176) * t);
        int b = qRound(176 + (pureHueColor.blue() - 176) * t);
        QColor currentColor(r, g, b);

        p.setPen(QPen(Qt::white, 2));
        p.setBrush(currentColor);
        p.drawEllipse(posC, 4, 4); // B点半径值
        p.restore();
    };

    // 当精准度 == 0 时 C 与 A 重合，按要求“C 与 A 重合时让 A 绘制在上层”
    if (m_precision == 0) {
        drawHandleC();
        drawHandleA();
    } else {
        drawHandleA();
        drawHandleC();
    }
}

void ColorWheel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF pos = event->position();
        QPointF posA = getHandleAPosition();
        QPointF posC = getHandleCPosition();

        double distToA = QLineF(pos, posA).length();
        double distToC = QLineF(pos, posC).length();

        // 判定手柄点击命中 (保留 10px 交互容差热区)
        bool hitA = (distToA <= 10.0);
        bool hitC = (distToC <= 10.0);

        if (hitC && (!hitA || distToC < distToA)) {
            m_dragTarget = DragTarget::DragC;
        } else {
            m_dragTarget = DragTarget::DragA;
        }

        handleDragMove(pos);
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void ColorWheel::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragTarget != DragTarget::None) {
        handleDragMove(event->position());
        event->accept();
    } else {
        updateCursorShape(event->pos());
        QWidget::mouseMoveEvent(event);
    }
}

void ColorWheel::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_dragTarget != DragTarget::None) {
        m_dragTarget = DragTarget::None;
        ToolTipOverlay::hideTip();
        updateCursorShape(event->pos());
        emit changed(m_hue, m_precision);
        event->accept();
    } else {
        QWidget::mouseReleaseEvent(event);
    }
}

void ColorWheel::handleDragMove(const QPointF& mousePos) {
    QPointF center = getCenterPoint();
    double radius = wheelRadius();
    if (radius <= 0) return;

    if (m_dragTarget == DragTarget::DragA) {
        double dx = mousePos.x() - center.x();
        double dy = center.y() - mousePos.y(); // 笛卡尔坐标系 Y 向上
        double dist = qSqrt(dx * dx + dy * dy);

        if (dist > 0.001) {
            double rad = qAtan2(dy, dx);
            double deg = qRadiansToDegrees(rad);
            if (deg < 0) deg += 360.0;
            m_hue = qRound(deg) % 360;
        }
        update();
        ToolTipOverlay::instance()->showText(QCursor::pos(), QString("色相: %1°").arg(m_hue), 0);
    } else if (m_dragTarget == DragTarget::DragC) {
        QPointF posA = getHandleAPosition();
        QPointF vecBA = posA - center;
        double lenBA = QLineF(center, posA).length();

        if (lenBA > 0.001) {
            QPointF vecBM = mousePos - center;
            // 鼠标点 M 在连线 BA 上的点积投影
            double dot = (vecBM.x() * vecBA.x() + vecBM.y() * vecBA.y()) / lenBA;
            double clampedDist = qBound(0.0, dot, lenBA);

            // 比例：精准度 0 在边缘(A点)，100 在圆心(B点)
            m_precision = qBound(0, qRound(100.0 * (1.0 - clampedDist / lenBA)), 100);
        }
        update();
        ToolTipOverlay::instance()->showText(QCursor::pos(), QString("精准度: %1").arg(m_precision), 0);
    }
}

void ColorWheel::updateCursorShape(const QPoint& pos) {
    QPointF posF(pos);
    QPointF posA = getHandleAPosition();
    QPointF posC = getHandleCPosition();

    double distA = QLineF(posF, posA).length();
    double distC = QLineF(posF, posC).length();

    if (distA <= 10.0 || distC <= 10.0) {
        setCursor(Qt::PointingHandCursor);
    } else {
        unsetCursor();
    }
}

} // namespace QuarkMeta
