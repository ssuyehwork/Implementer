#pragma once
#include "../FramelessDialogBase.h"
#include <QColor>

namespace QuarkMeta {

class FramelessConfirmDialog : public FramelessDialog {
    Q_OBJECT
public:
    enum ButtonType { OkOnly, OkCancel };
    explicit FramelessConfirmDialog(const QString& title, const QString& message, 
                                   ButtonType type = OkCancel, const QString& iconName = "", 
                                   const QColor& iconColor = Qt::white, QWidget* parent = nullptr,
                                   const QString& okText = "确定");
};

} // namespace QuarkMeta
