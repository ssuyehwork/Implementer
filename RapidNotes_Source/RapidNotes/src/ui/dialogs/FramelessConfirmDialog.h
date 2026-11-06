#pragma once
#include "../FramelessDialog.h"
#include <QColor>

namespace RapidNotes {

class FramelessConfirmDialog : public FramelessDialog {
    Q_OBJECT
public:
    enum ButtonType { OkOnly, OkCancel };
    explicit FramelessConfirmDialog(const QString& title, const QString& message,
                                   ButtonType type = OkCancel, const QString& iconName = "",
                                   const QColor& iconColor = Qt::white, QWidget* parent = nullptr,
                                   const QString& okText = "确定")
        : FramelessDialog(title, parent) {
        Q_UNUSED(message); Q_UNUSED(type); Q_UNUSED(iconName); Q_UNUSED(iconColor); Q_UNUSED(okText);
    }
};

} // namespace RapidNotes
