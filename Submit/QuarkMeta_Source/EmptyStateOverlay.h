#pragma once

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>
#include <utility>

#include "UiHelper.h"

namespace QuarkMeta {

class EmptyStateOverlay final : public QWidget {
public:
    explicit EmptyStateOverlay(QWidget* parent, std::function<void()> onReset)
        : QWidget(parent) {
        setObjectName("EmptyFilterOverlay");
        setAttribute(Qt::WA_StyledBackground, true);

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(12);
        layout->setAlignment(Qt::AlignCenter);

        auto* iconLabel = new QLabel(this);
        iconLabel->setPixmap(UiHelper::getIcon("filter", QColor("#888888"), 48).pixmap(48, 48));
        iconLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(iconLabel);

        m_titleLabel = new QLabel(this);
        m_titleLabel->setObjectName("EmptyFilterTitleLabel");
        m_titleLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(m_titleLabel);

        m_subLabel = new QLabel("未显示内容是因为筛选器或搜索过滤了结果", this);
        m_subLabel->setObjectName("EmptyFilterSubLabel");
        m_subLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(m_subLabel);

        m_resetButton = new QPushButton("重置筛选与搜索 或 后退 (Ctrl + Space)", this);
        m_resetButton->setObjectName("EmptyFilterResetBtn");
        m_resetButton->setFocusPolicy(Qt::NoFocus);
        m_resetButton->setCursor(Qt::PointingHandCursor);
        m_resetButton->setFixedHeight(32);
        m_resetButton->setMinimumWidth(160);
        QObject::connect(m_resetButton, &QPushButton::clicked, this,
                         [onReset = std::move(onReset)]() {
            if (onReset) onReset();
        });
        layout->addWidget(m_resetButton, 0, Qt::AlignCenter);

        hide();
    }

    void updateState(int visibleCount, int fullCount, int hiddenCount) {
        if (visibleCount == 0) {
            if (fullCount > 0) {
                m_titleLabel->setText(QString("当前共有 %1 个项目，已过滤隐藏 %2 个项目").arg(fullCount).arg(hiddenCount));
                m_subLabel->setText("未显示内容是因为筛选器或搜索过滤了结果");
                m_subLabel->show();
                m_resetButton->setText("重置筛选与搜索 或 后退 (Ctrl + Space)");
                m_resetButton->adjustSize();
            } else {
                m_titleLabel->setText("该文件夹为空");
                m_subLabel->hide();
                m_resetButton->setText("后退 (Ctrl + Space)");
                m_resetButton->adjustSize();
            }

            if (parentWidget()) setGeometry(parentWidget()->rect());
            show();
            raise();
        } else {
            hide();
        }
    }

private:
    QLabel* m_titleLabel = nullptr;
    QLabel* m_subLabel = nullptr;
    QPushButton* m_resetButton = nullptr;
};

} // namespace QuarkMeta
