#pragma once

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPaintEvent>

namespace QuarkMeta {

class ColorDot : public QWidget {
public:
    explicit ColorDot(const QColor& color, QWidget* parent = nullptr)
        : QWidget(parent), m_color(color) {
        setFixedSize(10, 10);
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }

    void setColor(const QColor& color) {
        if (m_color != color) {
            m_color = color;
            update();
        }
    }

    QColor color() const { return m_color; }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::NoPen);
        painter.setBrush(m_color);
        painter.drawEllipse(rect());
    }

private:
    QColor m_color;
};

} // namespace QuarkMeta
