#pragma once

#include <QWidget>
#include <QColor>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEnterEvent>

namespace QuarkMeta {

class ColorBlock : public QWidget {
    Q_OBJECT
public:
    explicit ColorBlock(const QColor& color, QWidget* parent = nullptr);
    ~ColorBlock() override = default;

    void setChecked(bool checked);
    bool isChecked() const { return m_checked; }
    void setCount(int count) { m_count = count; }
    QColor color() const { return m_color; }

signals:
    void clicked(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QColor m_color;
    bool m_checked = false;
    bool m_hovered = false;
    int m_count = 0;
};

} // namespace QuarkMeta
