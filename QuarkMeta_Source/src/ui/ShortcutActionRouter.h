#pragma once

#include <QObject>
#include <QPointer>
#include "AppShortcutController.h"
#include "TitleBarWidget.h"
#include "PanelLayoutManager.h"
#include "FilterPanel.h"

namespace QuarkMeta {

class ActivePaneCoordinator;

/**
 * @brief 快捷键与标题栏动作路由器
 * 负责处理 AppShortcutController 快捷键信号与 TitleBarWidget 界面控制动作。
 */
class ShortcutActionRouter : public QObject {
    Q_OBJECT
public:
    explicit ShortcutActionRouter(ActivePaneCoordinator* activeCoordinator,
                                  AppShortcutController* shortcutController,
                                  TitleBarWidget* titleBar,
                                  PanelLayoutManager* layoutManager,
                                  FilterPanel* filterPanel,
                                  QObject* parent = nullptr);
    ~ShortcutActionRouter() override = default;

    void wire();

private:
    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<AppShortcutController> m_shortcutController;
    QPointer<TitleBarWidget> m_titleBar;
    QPointer<PanelLayoutManager> m_layoutManager;
    QPointer<FilterPanel> m_filterPanel;
};

} // namespace QuarkMeta
