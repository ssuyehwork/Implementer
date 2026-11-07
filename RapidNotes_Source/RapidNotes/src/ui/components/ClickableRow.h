#pragma once
#include <QWidget>

namespace QuarkMeta {

class StyledCheckBox;

class ClickableRow : public QWidget {
    Q_OBJECT
public:
    explicit ClickableRow(StyledCheckBox* cb, QWidget* parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent* e) override;
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;
private:
    StyledCheckBox* m_cb;
};

} // namespace QuarkMeta
