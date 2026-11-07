#include "dialogs/FramelessConfirmDialog.h"
#include "UiHelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace QuarkMeta {

FramelessConfirmDialog::FramelessConfirmDialog(const QString& title, const QString& message,
                                               ButtonType type, const QString& iconName,
                                               const QColor& iconColor, QWidget* parent,
                                               const QString& okText)
    : FramelessDialog(title, parent)
{
    setVisibleButtons(Close);
    resize(420, 180);
    setMinimumSize(380, 160);

    auto* layout = new QVBoxLayout(m_contentArea);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(15);

    auto* msgLayout = new QHBoxLayout();
    msgLayout->setSpacing(15);

    if (!iconName.isEmpty()) {
        auto* iconLbl = new QLabel();
        iconLbl->setPixmap(UiHelper::getIcon(iconName, iconColor, 32).pixmap(32, 32));
        msgLayout->addWidget(iconLbl, 0, Qt::AlignTop);
    }

    auto* lbl = new QLabel(message);
    lbl->setObjectName("FramelessConfirmLabel");
    lbl->setWordWrap(true);
    lbl->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    msgLayout->addWidget(lbl, 1);

    layout->addLayout(msgLayout, 1);

    auto* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);
    btnLayout->addStretch();

    if (type == OkCancel) {
        auto* btnCancel = new QPushButton("取消");
        btnCancel->setFixedSize(85, 30);
        btnCancel->setCursor(Qt::PointingHandCursor);
        btnCancel->setObjectName("FramelessBtnCancel");
        connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
        btnLayout->addWidget(btnCancel);
    }

    auto* btnOk = new QPushButton(okText.isEmpty() ? "确定" : okText);
    int okBtnWidth = (btnOk->text().length() > 2) ? 95 : 85;
    btnOk->setFixedSize(okBtnWidth, 30);
    btnOk->setCursor(Qt::PointingHandCursor);
    btnOk->setObjectName("FramelessBtnOk");
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(btnOk);

    layout->addLayout(btnLayout);
}

} // namespace QuarkMeta
