#pragma once

#include "../FramelessDialog.h"
#include "../ToolTipOverlay.h"
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QClipboard>
#include <QFileInfo>

namespace QuarkMeta {

class FileHashDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit FileHashDialog(const QString& filePath, const QString& hashValue, QWidget* parent = nullptr)
        : FramelessDialog("文件哈希值", parent), m_hash(hashValue) {
        setVisibleButtons(Close);
        setFixedSize(460, 230);

        auto* layout = new QVBoxLayout(m_contentArea);
        layout->setContentsMargins(20, 15, 20, 15);
        layout->setSpacing(10);

        QString fileName = QFileInfo(filePath).fileName();
        auto* nameLabel = new QLabel(QString("文件: %1").arg(fileName), this);
        nameLabel->setObjectName("FileHashDialogFileName");
        layout->addWidget(nameLabel);

        auto* hashTitleLabel = new QLabel("SHA-256 哈希值:", this);
        hashTitleLabel->setObjectName("FileHashDialogTitleLabel");
        layout->addWidget(hashTitleLabel);

        m_hashEdit = new QTextEdit(this);
        m_hashEdit->setObjectName("FileHashDialogEdit");
        m_hashEdit->setReadOnly(true);
        m_hashEdit->setPlainText(m_hash);
        m_hashEdit->setLineWrapMode(QTextEdit::WidgetWidth);
        m_hashEdit->setFixedHeight(60);
        layout->addWidget(m_hashEdit);

        auto* btnLayout = new QHBoxLayout();
        btnLayout->setContentsMargins(0, 5, 0, 0);
        btnLayout->setSpacing(10);
        btnLayout->addStretch();

        auto* btnCopy = new QPushButton("复制哈希值", this);
        btnCopy->setObjectName("FileHashDialogCopyBtn");
        btnCopy->setFocusPolicy(Qt::NoFocus);
        btnCopy->setFixedSize(90, 28);
        btnCopy->setCursor(Qt::PointingHandCursor);
        connect(btnCopy, &QPushButton::clicked, this, [this]() {
            QApplication::clipboard()->setText(m_hash);
            ToolTipOverlay::instance()->showToast("已复制哈希值", 1200, QColor("#2ecc71"));
        });
        btnLayout->addWidget(btnCopy);

        auto* btnClose = new QPushButton("关闭", this);
        btnClose->setObjectName("FileHashDialogCloseBtn");
        btnClose->setFocusPolicy(Qt::NoFocus);
        btnClose->setFixedSize(75, 28);
        btnClose->setCursor(Qt::PointingHandCursor);
        connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
        btnLayout->addWidget(btnClose);

        layout->addLayout(btnLayout);
    }

private:
    QString m_hash;
    QTextEdit* m_hashEdit = nullptr;
};

} // namespace QuarkMeta
