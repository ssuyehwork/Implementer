#pragma once

#include <QObject>
#include <QPointer>
#include "ContentPanel.h"

namespace QuarkMeta {

class NavPanel;
class FavoritePanel;
class LibraryPanel;
class MarkedPanel;
class ContentPanel;
class MetaPanel;
class FilterPanel;
class AddressBar;
class SearchController;
class TitleBarWidget;
class PanelLayoutManager;
class AppShortcutController;

class ActivePaneCoordinator;
class MetaPanelPresenter;
class MetaEditController;
class QuickLookController;
class ShortcutActionRouter;
class TabBarCoordinator;
class NavigationCoordinator;
class FilterStatsBinder;

/**
 * @brief 面板中介者组件依赖包
 */
struct PanelMediatorComponents {
    NavPanel* navPanel = nullptr;
    FavoritePanel* favoritePanel = nullptr;
    LibraryPanel* libraryPanel = nullptr;
    MarkedPanel* markedPanel = nullptr;
    ContentPanel* contentPanel = nullptr;
    MetaPanel* metaPanel = nullptr;
    FilterPanel* filterPanel = nullptr;
    AddressBar* addressBar = nullptr;
    SearchController* searchController = nullptr;
    TitleBarWidget* titleBar = nullptr;
    PanelLayoutManager* layoutManager = nullptr;
    AppShortcutController* shortcutController = nullptr;
};

/**
 * @brief 面板中介者 (瘦身重构版)
 * 作为各专职协调器的装配中枢与对外门面，完全冻结对外公开接口契约。
 */
class PanelMediator : public QObject {
    Q_OBJECT

public:
    explicit PanelMediator(const PanelMediatorComponents& components, QObject* parent = nullptr);
    ~PanelMediator() override;

    static PanelMediator* instance();

    ContentPanel* activeContentPanel() const;

    /**
     * @brief 建立各面板间的信号槽连接
     */
    void setupConnections();

    /**
     * @brief 统一在主视图中定位目标项目 (SSOT)
     * @param fullPath 目标文件或文件夹完整物理路径
     */
    void locateItem(const QString& fullPath);

signals:
    /**
     * @brief 统一向 MainWindow 发送状态栏消息更新请求
     */
    void statusMessageRequested(const QString& message);

    /**
     * @brief 当前激活的内容面板改变广播信号
     */
    void activeContentPanelChanged(ContentPanel* panel);

private:
    void distributeBindPane(ContentPanel* pane);

    QPointer<NavPanel> m_navPanel;
    QPointer<FavoritePanel> m_favoritePanel;
    QPointer<LibraryPanel> m_libraryPanel;
    QPointer<MarkedPanel> m_markedPanel;
    QPointer<ContentPanel> m_contentPanel;
    QPointer<MetaPanel> m_metaPanel;
    QPointer<FilterPanel> m_filterPanel;
    QPointer<AddressBar> m_addressBar;
    QPointer<SearchController> m_searchController;
    QPointer<TitleBarWidget> m_titleBar;
    QPointer<PanelLayoutManager> m_layoutManager;
    QPointer<AppShortcutController> m_shortcutController;

    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    MetaPanelPresenter* m_metaPresenter = nullptr;
    MetaEditController* m_metaEditController = nullptr;
    QuickLookController* m_quickLookController = nullptr;
    ShortcutActionRouter* m_shortcutRouter = nullptr;
    TabBarCoordinator* m_tabBarCoordinator = nullptr;
    NavigationCoordinator* m_navCoordinator = nullptr;
    FilterStatsBinder* m_filterStatsBinder = nullptr;
};

} // namespace QuarkMeta
