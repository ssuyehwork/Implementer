#include "dialogs/FramelessInputDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

namespace QuarkMeta {

FramelessInputDialog::FramelessInputDialog(const QString& title, const QString& label,
                                           const QString& initial, QWidget* parent)
    : FramelessDialog(title, parent)
{
    setVisibleButtons(Close);
    resize(500, 210);
    setMinimumSize(400, 190);

    auto* layout = new QVBoxLayout(m_contentArea);
    layout->setContentsMargins(20, 15, 20, 20);
    layout->setSpacing(7);

    auto* lbl = new QLabel(label);
    lbl->setObjectName("FramelessInputLabel");
    layout->addWidget(lbl);

    m_edit = new QLineEdit(initial);
    m_edit->setMinimumHeight(38);
    m_edit->setObjectName("FramelessInputEdit");
    layout->addWidget(m_edit);

    connect(m_edit, &QLineEdit::returnPressed, this, &QDialog::accept);

    layout->addStretch();

    auto* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    auto* btnCancel = new QPushButton("取消");
    btnCancel->setFixedSize(80, 32);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setObjectName("FramelessBtnCancel");
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(btnCancel);

    auto* btnOk = new QPushButton("确定");
    btnOk->setFixedSize(80, 32);
    btnOk->setCursor(Qt::PointingHandCursor);
    btnOk->setObjectName("FramelessBtnOk");
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(btnOk);

    layout->addLayout(btnLayout);

    m_edit->setFocus();
    m_edit->selectAll();
}

void FramelessInputDialog::showEvent(QShowEvent* event) {
    FramelessDialog::showEvent(event);
    QTimer::singleShot(50, m_edit, qOverload<>(&QWidget::setFocus));
}

void FramelessInputDialog::setEchoMode(QLineEdit::EchoMode mode) {
    if (m_edit) {
        m_edit->setEchoMode(mode);
    }
}

} // namespace QuarkMeta
