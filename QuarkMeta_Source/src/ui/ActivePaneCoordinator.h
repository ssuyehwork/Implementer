#pragma once

#include <QObject>
#include <QPointer>
#include "ContentPanel.h"

namespace QuarkMeta {

class PaneActivationTracker;

/**
 * @brief 激活窗格协调器 (SSOT)
 * 唯一持有当前激活窗格，统一提供 currentTarget() 与 isCurrentTarget()，
 * 并负责 PaneActivationTracker 的创建与全生命周期事件过滤。
 */
class ActivePaneCoordinator : public QObject {
    Q_OBJECT
public:
    explicit ActivePaneCoordinator(ContentPanel* rootPanel, QObject* parent = nullptr);
    ~ActivePaneCoordinator() override = default;

    ContentPanel* currentTarget() const;
    bool isCurrentTarget(ContentPanel* panel) const;
    ContentPanel* activePane() const { return m_activeContentPanel.data(); }
    ContentPanel* rootPane() const { return m_rootContentPanel.data(); }

    void activate(ContentPanel* panel);
    void bindPane(ContentPanel* pane);
    void wire();

signals:
    void activePaneChanged(ContentPanel* panel);
    void statusMessageRequested(const QString& message);

private:
    void syncPaneHighlights(ContentPanel* activePanel);

    QPointer<ContentPanel> m_rootContentPanel;
    QPointer<ContentPanel> m_activeContentPanel;
    PaneActivationTracker* m_activationTracker = nullptr;
};

} // namespace QuarkMeta
