#pragma once

#include "FramelessDialog.h"
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>
#include <QDateTime>

namespace QuarkMeta {

/**
 * @brief 批量操作进度对话框
 */
class BatchProgressDialog : public FramelessDialog {
    Q_OBJECT

public:
    explicit BatchProgressDialog(const QString& title, QWidget* parent = nullptr)
        : FramelessDialog(title, parent) {
        setVisibleButtons(Close);
        setFixedSize(400, 150);

        auto* layout = new QVBoxLayout(m_contentArea);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(10);

        m_statusLabel = new QLabel("正在准备...", this);
        m_statusLabel->setObjectName("ProgressStatusLabel");
        layout->addWidget(m_statusLabel);

        m_progressBar = new QProgressBar(this);
        m_progressBar->setFixedHeight(8);
        m_progressBar->setTextVisible(false);
        m_progressBar->setObjectName("ProgressDialogBar");
        layout->addWidget(m_progressBar);

        layout->addStretch();
    }

    void setStatus(const QString& text) {
        m_statusLabel->setText(text);
    }

    void setRange(int min, int max) {
        m_progressBar->setRange(min, max);
    }

    void setValue(int value) {
        m_progressBar->setValue(value);
    }

    /**
     * @brief 跨线程安全更新进度
     */
    Q_INVOKABLE void updateProgress(int current, int total, const QString& fileName) {
        if (total <= 0) return;

        qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (m_lastUpdateTime > 0 && now - m_lastUpdateTime < 50 && current < total) {
            return;
        }
        m_lastUpdateTime = now;

        m_progressBar->setRange(0, total);
        m_progressBar->setValue(current);

        double percent = (double)current / (double)total * 100.0;
        QString status = QString("[%1/%2] %3% - %4")
                            .arg(current)
                            .arg(total)
                            .arg(QString::number(percent, 'f', 1))
                            .arg(fileName);
        m_statusLabel->setText(status);
    }

private:
    QLabel* m_statusLabel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    qint64 m_lastUpdateTime = 0;
};

} // namespace QuarkMeta
