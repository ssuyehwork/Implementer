#include "PanelMediator.h"
#include "NavPanel.h"
#include "FavoritePanel.h"
#include "LibraryPanel.h"
#include "MarkedPanel.h"
#include "ContentPanel.h"
#include "MetaPanel.h"
#include "FilterPanel.h"
#include "AddressBar.h"
#include "SearchController.h"
#include "TitleBarWidget.h"
#include "TabBarWidget.h"
#include "PanelLayoutManager.h"
#include "AppShortcutController.h"

#include "ActivePaneCoordinator.h"
#include "MetaPanelPresenter.h"
#include "MetaEditController.h"
#include "QuickLookController.h"
#include "ShortcutActionRouter.h"
#include "TabBarCoordinator.h"
#include "NavigationCoordinator.h"
#include "FilterStatsBinder.h"

namespace QuarkMeta {

static PanelMediator* s_panelMediatorInstance = nullptr;

PanelMediator* PanelMediator::instance() {
    return s_panelMediatorInstance;
}

PanelMediator::PanelMediator(const PanelMediatorComponents& components, QObject* parent)
    : QObject(parent),
      m_navPanel(components.navPanel),
      m_favoritePanel(components.favoritePanel),
      m_libraryPanel(components.libraryPanel),
      m_markedPanel(components.markedPanel),
      m_contentPanel(components.contentPanel),
      m_metaPanel(components.metaPanel),
      m_filterPanel(components.filterPanel),
      m_addressBar(components.addressBar),
      m_searchController(components.searchController),
      m_titleBar(components.titleBar),
      m_layoutManager(components.layoutManager),
      m_shortcutController(components.shortcutController) {
    s_panelMediatorInstance = this;
}

PanelMediator::~PanelMediator() {
    if (s_panelMediatorInstance == this) {
        s_panelMediatorInstance = nullptr;
    }
}

ContentPanel* PanelMediator::activeContentPanel() const {
    return m_activeCoordinator ? m_activeCoordinator->activePane() : m_contentPanel.data();
}

void PanelMediator::locateItem(const QString& fullPath) {
    if (m_navCoordinator) {
        m_navCoordinator->locateItem(fullPath);
    }
}

void PanelMediator::distributeBindPane(ContentPanel* pane) {
    if (!pane) return;

    // 统一规则三：按固定先后顺序对窗格依次进行逐窗格绑定
    if (m_activeCoordinator) {
        m_activeCoordinator->bindPane(pane);
    }
    if (m_quickLookController) {
        m_quickLookController->bindPane(pane);
    }
    if (m_metaPresenter) {
        m_metaPresenter->bindPane(pane);
    }
    if (m_navCoordinator) {
        m_navCoordinator->bindPane(pane);
    }

    // 动态副窗格递归分发
    connect(pane, &ContentPanel::secondaryPaneCreated, this, [this](ContentPanel* secondaryPane) {
        distributeBindPane(secondaryPane);
    });
}

void PanelMediator::setupConnections() {
    ContentPanel* contentPanel = m_contentPanel.data();

    // 1. 创建各专业协调器
    m_activeCoordinator = new ActivePaneCoordinator(contentPanel, this);
    m_metaPresenter = new MetaPanelPresenter(m_activeCoordinator, m_metaPanel.data(), contentPanel, this);
    m_metaEditController = new MetaEditController(m_activeCoordinator, m_metaPanel.data(), this);
    m_quickLookController = new QuickLookController(m_activeCoordinator, m_metaPanel.data(), this);
    m_shortcutRouter = new ShortcutActionRouter(
        m_activeCoordinator,
        m_shortcutController.data(),
        m_titleBar.data(),
        m_layoutManager.data(),
        m_filterPanel.data(),
        this
    );

    TabBarWidget* tabBar = m_titleBar ? m_titleBar->tabBar() : nullptr;

    m_navCoordinator = new NavigationCoordinator(
        m_activeCoordinator,
        m_navPanel.data(),
        m_favoritePanel.data(),
        m_libraryPanel.data(),
        m_markedPanel.data(),
        m_addressBar.data(),
        tabBar,
        m_searchController.data(),
        this
    );

    m_tabBarCoordinator = new TabBarCoordinator(
        m_activeCoordinator,
        tabBar,
        [this]() {
            if (m_navCoordinator) m_navCoordinator->refreshCurrent();
        },
        this
    );

    m_filterStatsBinder = new FilterStatsBinder(
        m_activeCoordinator,
        m_filterPanel.data(),
        m_metaPanel.data(),
        this
    );

    // 2. 状态栏消息转发
    connect(m_activeCoordinator, &ActivePaneCoordinator::statusMessageRequested, this, &PanelMediator::statusMessageRequested);
    connect(m_navCoordinator, &NavigationCoordinator::statusMessageRequested, this, &PanelMediator::statusMessageRequested);

    // 3. activeContentPanelChanged 接收顺序保全契约
    // 协调器激活信号转发给 PanelMediator::activeContentPanelChanged
    connect(m_activeCoordinator, &ActivePaneCoordinator::activePaneChanged, this, &PanelMediator::activeContentPanelChanged);

    // 严格保持的原有接收顺序：
    // ① 搜索控制器 setActiveContentPanel（在初始化阶段只对 contentPanel bindContentPanel 一次）
    if (m_searchController) {
        if (contentPanel) {
            m_searchController->bindContentPanel(contentPanel);
        }
        connect(this, &PanelMediator::activeContentPanelChanged, this, [this](ContentPanel* activePanel) {
            if (m_searchController) {
                m_searchController->setActiveContentPanel(activePanel);
            }
        });
    }

    // ② 窗格高亮同步（已由 ActivePaneCoordinator 内部监听 activePaneChanged 自动完成）

    // ③ 元数据面板刷新
    if (m_metaPanel) {
        connect(this, &PanelMediator::activeContentPanelChanged, this, [this](ContentPanel* activePanel) {
            if (m_metaPresenter) {
                m_metaPresenter->updateMetaPanel(activePanel);
            }
        });
    }

    // ④ 筛选面板绑定
    if (m_filterPanel) {
        connect(this, &PanelMediator::activeContentPanelChanged, this, [this](ContentPanel* activePanel) {
            if (m_filterStatsBinder && activePanel) {
                m_filterStatsBinder->bindFilterToActivePanel(activePanel);
            }
        });
    }

    // 4. 执行各协调器的全局信号装配 (wire)
    m_activeCoordinator->wire();
    m_metaPresenter->wire();
    m_metaEditController->wire();
    m_quickLookController->wire();
    m_shortcutRouter->wire();
    m_tabBarCoordinator->wire();
    m_navCoordinator->wire();
    m_filterStatsBinder->wire();

    // 5. 统一窗格绑定（主窗格及后续副窗格均走统一分发）
    if (contentPanel) {
        distributeBindPane(contentPanel);
    }
}

} // namespace QuarkMeta
