#include "components/StyledCheckBox.h"
#include <QPainter>
#include <QPainterPath>

namespace QuarkMeta {

StyledCheckBox::StyledCheckBox(QWidget* parent) : QCheckBox(parent) {
    setFixedSize(15, 15);
}

void StyledCheckBox::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool checked = isChecked();

    QRectF rect(0.5, 0.5, width() - 1.0, height() - 1.0);
    QColor borderColor = checked ? QColor("#378ADD") : QColor("#444444");

    painter.setPen(QPen(borderColor, 1.0));
    painter.setBrush(QColor("#1E1E1E"));
    painter.drawRoundedRect(rect, 2.0, 2.0);

    if (checked) {
        QPen pen(QColor("#378ADD"), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        QPolygonF checkMark;
        checkMark << QPointF(2.5, 7.5)
                  << QPointF(5.5, 11.0)
                  << QPointF(12.0, 3.5);
        painter.drawPolyline(checkMark);
    }
}

} // namespace QuarkMeta
