#include "ActivePaneCoordinator.h"
#include "controllers/PaneActivationTracker.h"
#include <QApplication>

namespace QuarkMeta {

ActivePaneCoordinator::ActivePaneCoordinator(ContentPanel* rootPanel, QObject* parent)
    : QObject(parent), m_rootContentPanel(rootPanel), m_activeContentPanel(rootPanel) {
    if (m_rootContentPanel) {
        m_rootContentPanel->setActivePane(true);
    }
}

ContentPanel* ActivePaneCoordinator::currentTarget() const {
    if (m_activeContentPanel && m_activeContentPanel->isVisible()) {
        return m_activeContentPanel.data();
    }
    return m_rootContentPanel.data();
}

bool ActivePaneCoordinator::isCurrentTarget(ContentPanel* panel) const {
    return panel && (panel == currentTarget());
}

void ActivePaneCoordinator::activate(ContentPanel* panel) {
    if (!panel || m_activeContentPanel == panel) {
        return;
    }
    if (m_activeContentPanel) {
        m_activeContentPanel->setActivePane(false);
    }
    m_activeContentPanel = panel;
    m_activeContentPanel->setActivePane(true);

    emit activePaneChanged(m_activeContentPanel.data());
}

void ActivePaneCoordinator::syncPaneHighlights(ContentPanel* activePanel) {
    ContentPanel* root = m_rootContentPanel ? m_rootContentPanel->rootPane() : nullptr;
    if (root) {
        root->setActivePane(root == activePanel);
        for (ContentPanel* pane : root->panes()) {
            if (pane && pane != root) {
                pane->setActivePane(pane == activePanel);
            }
        }
    }
}

void ActivePaneCoordinator::bindPane(ContentPanel* pane) {
    if (!pane) return;

    // 状态栏消息转发（仅当来自当前目标窗格时才转发）
    connect(pane, &ContentPanel::statusBarMessageReady, this, [this, pane](const QString& message) {
        if (isCurrentTarget(pane)) {
            emit statusMessageRequested(message);
        }
    });

    // 窗格自身激活信号接入
    connect(pane, &ContentPanel::panelActivated, this, [this](ContentPanel* activePanel) {
        activate(activePanel);
    });
}

void ActivePaneCoordinator::wire() {
    if (!m_activationTracker) {
        m_activationTracker = new PaneActivationTracker(this);
        qApp->installEventFilter(m_activationTracker);

        connect(m_activationTracker, &PaneActivationTracker::paneInteracted, this, [this](ContentPanel* panel) {
            activate(panel);
        });
    }

    // 窗格高亮同步作为 activePaneChanged 接收者无条件执行
    connect(this, &ActivePaneCoordinator::activePaneChanged, this, [this](ContentPanel* activePanel) {
        syncPaneHighlights(activePanel);
    });
}

} // namespace QuarkMeta
