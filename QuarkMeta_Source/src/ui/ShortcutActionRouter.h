#pragma once

#include <QObject>
#include <QPointer>
#include "AppShortcutController.h"
#include "TitleBarWidget.h"
#include "PanelLayoutManager.h"
#include "FilterPanel.h"

namespace QuarkMeta {

class ActivePaneCoordinator;
class SearchController;
class ContentPanel;

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
                                  SearchController* searchController = nullptr,
                                  QObject* parent = nullptr);
    ~ShortcutActionRouter() override = default;

    void wire();
    void bindPane(ContentPanel* pane);
    void triggerUnifiedReset();

private:
    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<AppShortcutController> m_shortcutController;
    QPointer<TitleBarWidget> m_titleBar;
    QPointer<PanelLayoutManager> m_layoutManager;
    QPointer<FilterPanel> m_filterPanel;
    QPointer<SearchController> m_searchController;
};

} // namespace QuarkMeta
