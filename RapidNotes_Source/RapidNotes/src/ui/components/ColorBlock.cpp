#include "ColorBlock.h"
#include "../ToolTipOverlay.h"
#include <QPainter>
#include <QPen>
#include <QCursor>

namespace QuarkMeta {

ColorBlock::ColorBlock(const QColor& color, QWidget* parent)
    : QWidget(parent), m_color(color) {
    setFixedSize(15, 15);
    setCursor(Qt::PointingHandCursor);
}

void ColorBlock::setChecked(bool checked) {
    m_checked = checked;
    update();
}

void ColorBlock::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(1, 1, -1, -1);
    if (m_checked) {
        painter.setPen(QPen(QColor("#378ADD"), 1.5));
        painter.setBrush(m_color);
        painter.drawRoundedRect(r, 2.0, 2.0);
    } else {
        painter.setPen(m_hovered ? QPen(QColor("#AAAAAA"), 1.0) : Qt::NoPen);
        painter.setBrush(m_color);
        painter.drawRoundedRect(r, 2.0, 2.0);
    }
}

void ColorBlock::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_color);
    }
}

void ColorBlock::enterEvent(QEnterEvent*) {
    m_hovered = true;
    update();
    QString tip = QString("颜色: %1").arg(m_color.name().toUpper());
    if (m_count >= 0) {
        tip += QString("\n匹配项: %1").arg(m_count);
    }
    ToolTipOverlay::instance()->showText(QCursor::pos(), tip, 0);
}

void ColorBlock::leaveEvent(QEvent*) {
    m_hovered = false;
    update();
    ToolTipOverlay::hideTip();
}

} // namespace QuarkMeta
