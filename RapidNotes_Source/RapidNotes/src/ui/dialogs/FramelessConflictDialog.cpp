#include "dialogs/FramelessConflictDialog.h"
#include "UiHelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace QuarkMeta {

FramelessConflictDialog::FramelessConflictDialog(const QString& title, const QString& message, QWidget* parent)
    : FramelessDialog(title, parent) {
    setVisibleButtons(Close);
    resize(460, 200);
    setMinimumSize(420, 180);

    auto* layout = new QVBoxLayout(m_contentArea);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(18);

    auto* msgLayout = new QHBoxLayout();
    msgLayout->setSpacing(15);

    auto* iconLbl = new QLabel();
    iconLbl->setPixmap(UiHelper::getIcon("alert", QColor("#FF9800"), 32).pixmap(32, 32));
    msgLayout->addWidget(iconLbl, 0, Qt::AlignTop);

    auto* lbl = new QLabel(message);
    lbl->setObjectName("FramelessConfirmLabel");
    lbl->setWordWrap(true);
    lbl->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    msgLayout->addWidget(lbl, 1);

    layout->addLayout(msgLayout, 1);

    auto* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addStretch();

    auto* btnAutoRename = new QPushButton("同存");
    btnAutoRename->setIcon(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 14));
    btnAutoRename->setFixedHeight(32);
    btnAutoRename->setCursor(Qt::PointingHandCursor);
    btnAutoRename->setObjectName("FramelessBtnOk");
    connect(btnAutoRename, &QPushButton::clicked, this, [this]() {
        m_choice = AutoRename;
        accept();
    });

    auto* btnReplace = new QPushButton("替代");
    btnReplace->setIcon(UiHelper::getIcon("copy", QColor("#EEEEEE"), 14));
    btnReplace->setFixedHeight(32);
    btnReplace->setCursor(Qt::PointingHandCursor);
    btnReplace->setObjectName("FramelessBtnOk");
    connect(btnReplace, &QPushButton::clicked, this, [this]() {
        m_choice = Replace;
        accept();
    });

    auto* btnCancel = new QPushButton("取消");
    btnCancel->setIcon(UiHelper::getIcon("close", QColor("#EEEEEE"), 14));
    btnCancel->setFixedHeight(32);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setObjectName("FramelessBtnCancel");
    connect(btnCancel, &QPushButton::clicked, this, [this]() {
        m_choice = Cancel;
        reject();
    });

    btnLayout->addWidget(btnAutoRename);
    btnLayout->addWidget(btnReplace);
    btnLayout->addWidget(btnCancel);

    layout->addLayout(btnLayout);
}

} // namespace QuarkMeta
