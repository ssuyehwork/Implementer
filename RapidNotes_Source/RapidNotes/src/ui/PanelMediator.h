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
class PaneActivationTracker;

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
 * @brief 面板中介者
 * 负责各面板与控制器之间的信号槽连接与协同动作，彻底解耦 MainWindow
 */
class PanelMediator : public QObject {
    Q_OBJECT

public:
    explicit PanelMediator(const PanelMediatorComponents& components, QObject* parent = nullptr);
    ~PanelMediator() override;

    static PanelMediator* instance();

    ContentPanel* activeContentPanel() const { return m_activeContentPanel.data(); }

    /**
     * @brief 建立各面板间的信号槽连接
     */
    void setupConnections();

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
    QPointer<NavPanel> m_navPanel;
    QPointer<FavoritePanel> m_favoritePanel;
    QPointer<LibraryPanel> m_libraryPanel;
    QPointer<MarkedPanel> m_markedPanel;
    QPointer<ContentPanel> m_contentPanel;
    QPointer<ContentPanel> m_activeContentPanel;
    QPointer<MetaPanel> m_metaPanel;
    QPointer<FilterPanel> m_filterPanel;
    QPointer<AddressBar> m_addressBar;
    QPointer<SearchController> m_searchController;
    QPointer<TitleBarWidget> m_titleBar;
    QPointer<PanelLayoutManager> m_layoutManager;
    QPointer<AppShortcutController> m_shortcutController;
    PaneActivationTracker* m_activationTracker = nullptr;

    struct PanelMetaDebounceState {
        QTimer* timer = nullptr;
        QSet<QString> pendingCleanPaths;
    };
    QMap<ContentPanel*, PanelMetaDebounceState> m_panelMetaDebounceMap;
};

} // namespace QuarkMeta
