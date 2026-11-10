#pragma once

#include "../FramelessDialog.h"
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <atomic>
#include <QPointer>
#include <QCoreApplication>

namespace QuarkMeta {

class ProgressDialog : public FramelessDialog {
    Q_OBJECT

public:
    explicit ProgressDialog(const QString& title, QWidget* parent = nullptr)
        : FramelessDialog(title, parent) {
        setFixedSize(440, 160);

        auto* layout = new QVBoxLayout(m_contentArea);
        layout->setContentsMargins(20, 15, 20, 15);
        layout->setSpacing(12);

        m_statusLabel = new QLabel("正在准备...", this);
        m_statusLabel->setObjectName("ProgressStatusLabel");
        layout->addWidget(m_statusLabel);

        m_progressBar = new QProgressBar(this);
        m_progressBar->setObjectName("ProgressDialogBar");
        m_progressBar->setFixedHeight(8);
        m_progressBar->setTextVisible(false);
        layout->addWidget(m_progressBar);

        auto* btnLayout = new QHBoxLayout();
        btnLayout->setContentsMargins(0, 5, 0, 0);
        btnLayout->setSpacing(10);
        btnLayout->addStretch();

        m_btnPause = new QPushButton("暂停", this);
        m_btnPause->setObjectName("ProgressPauseBtn");
        m_btnPause->setFocusPolicy(Qt::NoFocus);
        m_btnPause->setFixedSize(75, 28);
        connect(m_btnPause, &QPushButton::clicked, this, &ProgressDialog::togglePause);
        btnLayout->addWidget(m_btnPause);

        m_btnCancel = new QPushButton("取消", this);
        m_btnCancel->setObjectName("ProgressCancelBtn");
        m_btnCancel->setFocusPolicy(Qt::NoFocus);
        m_btnCancel->setFixedSize(75, 28);
        connect(m_btnCancel, &QPushButton::clicked, this, &ProgressDialog::cancel);
        btnLayout->addWidget(m_btnCancel);

        layout->addLayout(btnLayout);
    }

    bool isPaused() const { return m_isPaused.load(); }
    bool isCanceled() const { return m_isCanceled.load(); }

    Q_INVOKABLE void updateProgress(int current, int total, const QString& fileName) {
        if (total <= 0) return;
        m_progressBar->setRange(0, total);
        m_progressBar->setValue(current);

        double percent = (double)current / total * 100.0;
        QString status = QString("[%1/%2] %3% - %4")
                            .arg(current)
                            .arg(total)
                            .arg(QString::number(percent, 'f', 1))
                            .arg(fileName);
        m_statusLabel->setText(status);
    }

signals:
    void pauseToggled(bool isPaused);
    void cancelRequested();

public slots:
    void togglePause() {
        bool next = !m_isPaused.load();
        m_isPaused.store(next);
        m_btnPause->setText(next ? "继续" : "暂停");
        emit pauseToggled(next);
    }

    void cancel() {
        m_isCanceled.store(true);
        m_statusLabel->setText("正在取消操作...");
        m_btnPause->setEnabled(false);
        m_btnCancel->setEnabled(false);
        emit cancelRequested();
    }

private:
    QLabel* m_statusLabel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QPushButton* m_btnPause = nullptr;
    QPushButton* m_btnCancel = nullptr;
    std::atomic<bool> m_isPaused{false};
    std::atomic<bool> m_isCanceled{false};
};

} // namespace QuarkMeta
