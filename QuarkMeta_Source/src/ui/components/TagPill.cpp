#include "TagPill.h"
#include "../UiHelper.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QFontMetrics>

namespace QuarkMeta {

TagPill::TagPill(const QString& text, QWidget* parent) : QWidget(parent), m_text(text) {
    setFixedHeight(22);
    setAttribute(Qt::WA_StyledBackground, true);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 0, 4, 0);
    layout->setSpacing(4);

    m_label = new QLabel(text, this);
    m_label->setObjectName("TagPillLabel");
    
    // 显眼突出“×”号：增大图标、提升常规状态对比度与悬停反馈 
    m_closeBtn = new QPushButton(this); 
    m_closeBtn->setFixedSize(16, 16); 
    m_closeBtn->setCursor(Qt::PointingHandCursor); 
    m_closeBtn->setIcon(UiHelper::getIcon("clear-2.svg", QColor("#FFFFFF"), 14));
    m_closeBtn->setIconSize(QSize(12, 12)); 
    m_closeBtn->setObjectName("TagPillCloseBtn");

    layout->addWidget(m_label);
    layout->addWidget(m_closeBtn);

    connect(m_closeBtn, &QPushButton::clicked, [this]() {
        emit deleteRequested(m_text);
    });

    setData(text);
}

void TagPill::setData(const QString& text) {
    m_text = text;
    setProperty("tagText", text);
    m_label->setText(text);
    QFontMetrics fm(m_label->font());
    setFixedWidth(fm.horizontalAdvance(text) + 32);
}

void TagPill::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor("#2D2D30"));
    painter.setPen(QPen(QColor("#3E3E42"), 1));
    // 严格绘制 4 像素标准圆角
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 4, 4);
}

} // namespace QuarkMeta
