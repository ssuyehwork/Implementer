#pragma once
#include "../FramelessDialogBase.h"
#include <QColor>

namespace QuarkMeta {

class ColorPicker;

class FramelessColorPicker : public FramelessDialog {
    Q_OBJECT
public:
    explicit FramelessColorPicker(const QString& title, QWidget* parent = nullptr);
    void setCurrentColor(const QColor& color);
    QColor selectedColor() const { return m_selectedColor; }

private:
    ColorPicker* m_picker;
    QColor m_selectedColor;
};

} // namespace QuarkMeta
