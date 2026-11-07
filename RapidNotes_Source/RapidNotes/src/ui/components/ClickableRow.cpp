#include <QStyle>
#include "components/ClickableRow.h"
#include "components/StyledCheckBox.h"
#include <QMouseEvent>

namespace QuarkMeta {

ClickableRow::ClickableRow(StyledCheckBox* cb, QWidget* parent)
    : QWidget(parent), m_cb(cb) {
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_StyledBackground);
}

void ClickableRow::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        QPoint local = m_cb->mapFromGlobal(e->globalPosition().toPoint());
        if (!m_cb->rect().contains(local)) {
            m_cb->setChecked(!m_cb->isChecked());
        }
    }
    QWidget::mousePressEvent(e);
}

void ClickableRow::enterEvent(QEnterEvent* e) {
    setObjectName("ClickableRowActive");
    style()->unpolish(this);
    style()->polish(this);
    QWidget::enterEvent(e);
}

void ClickableRow::leaveEvent(QEvent* e) {
    setObjectName("ClickableRowNormal");
    style()->unpolish(this);
    style()->polish(this);
    QWidget::leaveEvent(e);
}

} // namespace QuarkMeta
