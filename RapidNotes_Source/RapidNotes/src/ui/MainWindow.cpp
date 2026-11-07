#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "MainWindow.h"
#include "TitleBarWidget.h"
#include "TabBarWidget.h"
#include "NavBarWidget.h"
#include "DriveBarWidget.h"
#include "UiHelper.h"
#include "SearchHistoryPanel.h"
#include "AppShortcutController.h"
#include "LocalShortcutInterceptor.h"
#include "PanelMediator.h"
#include "SearchController.h"
#include "NavPanel.h"
#include "SidebarContainerWidget.h"
#include "FavoritePanel.h"
#include "LibraryPanel.h"
#include "ContentPanel.h"
#include "MetaPanel.h"
#include "FilterPanel.h"
#include "TrayController.h"
#include "HoverEventFilter.h"
#include "FramelessWindowHelper.h"
#include "PanelLayoutManager.h"
#include "AddressBar.h"
#include "ToolTipOverlay.h"
#include "TaskProgressStrip.h"
#include "../core/AppConfig.h"
#include "../core/NavigationService.h"
#include "../core/CoreController.h"
#include "../core/ModelContract.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QTimer>
#include <QCloseEvent>
#include <QPushButton>
#include <QMenu>
#include <QActionGroup>
#include <QSignalBlocker>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

constexpr int kLayoutEdgeMargin = 5;
constexpr int kStatusBarHorizontalMargin = 12;

MainWindow::~MainWindow() = default;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    setMinimumHeight(700); // 宽度由 PanelLayoutManager::updateDynamicMinimumSize() 动态管理
    setWindowTitle("QuarkMeta");

    m_hoverFilter = new HoverEventFilter(this);
    m_isPinned = AppConfig::instance().getValue("MainWindow/AlwaysOnTop", false).toBool();

    ToolTipOverlay::instance();

    initUi();

    // 挂载无边框助手（必须在几何属性 restoreGeometry 恢复前完成挂载）
    m_framelessHelper = FramelessWindowHelper::apply(this, m_titleBarWidget);

    // 恢复窗口位置与几何尺寸
    QByteArray savedGeom = AppConfig::instance().getValue("MainWindow/Geometry").toByteArray();
    if (!savedGeom.isEmpty()) {
        restoreGeometry(savedGeom);
    } else {
        resize(1180, 800);
    }

    // 关键修正 4：几何尺寸恢复后，主动检测 Win32 最大化状态并对齐标题栏按钮
#ifdef Q_OS_WIN
    if (m_titleBarWidget) {
        m_titleBarWidget->setWindowMaximized(::IsZoomed(reinterpret_cast<HWND>(winId())));
    }
#endif

    if (m_isPinned) {
        FramelessWindowHelper::setAlwaysOnTop(this, true);
    }

    m_trayController = new TrayController(this);
    m_trayController->show();
}

void MainWindow::initUi() {
    QWidget* centralC = new QWidget(this);
    centralC->setObjectName("CentralWidget");
    QVBoxLayout* mainL = new QVBoxLayout(centralC);
    mainL->setContentsMargins(0, 0, 0, 0);
    mainL->setSpacing(0);

    // 1. 顶层栏组件组装
    setupTopBars(centralC);

    // 2. 5 大 Panel 核心主体组装
    QWidget* bodyWrapper = setupCentralPanels(centralC);

    // 3. 底部状态栏组装
    setupStatusBar(centralC);

    // 4. 悬浮线条进度条组装（作为 centralC 子控件悬浮，不进入布局）
    m_taskProgressStrip = new TaskProgressStrip(centralC);

    // 5. 控制器与中介者组装（组件依赖图绑定）
    setupControllersAndMediators();

    // 6. 组装至顶层主布局
    mainL->addWidget(m_titleBarWidget);
    mainL->addWidget(m_driveBarWidget);
    mainL->addWidget(m_navBarWidget);
    mainL->addWidget(bodyWrapper, 1);
    mainL->addWidget(m_statusBarWidget);

    // 7. 设置悬浮进度条对齐锚点
    m_taskProgressStrip->setAnchors(bodyWrapper, m_statusBarWidget);

    setCentralWidget(centralC);
}

void MainWindow::setupTopBars(QWidget* parentWidget) {
    m_titleBarWidget = new TitleBarWidget(parentWidget, m_hoverFilter);
    m_navBarWidget   = new NavBarWidget(parentWidget, m_hoverFilter);
    m_driveBarWidget = new DriveBarWidget(parentWidget);

    m_addressBar       = m_navBarWidget->addressBar();
    m_searchController = m_navBarWidget->searchController();

    // 置顶状态初始化与配置持久化（依赖顶层窗口本体资源）
    m_titleBarWidget->setPinned(m_isPinned);
    connect(m_titleBarWidget, &TitleBarWidget::pinToggled, this, [this](bool pinned) {
        m_isPinned = pinned;
        FramelessWindowHelper::setAlwaysOnTop(this, pinned);
        AppConfig::instance().setValue("MainWindow/AlwaysOnTop", pinned);
    });

    // 顶层子部件间的纯 UI 布局显隐联动与持久化恢复
    bool driveBarVis = AppConfig::instance().getValue("MainWindow/DriveBarVisible", true).toBool();
    m_titleBarWidget->setDriveBarVisible(driveBarVis);
    if (m_driveBarWidget) m_driveBarWidget->setVisible(driveBarVis);

    connect(m_titleBarWidget, &TitleBarWidget::driveBarToggleRequested, this, [this](bool visible) {
        if (m_driveBarWidget) m_driveBarWidget->setVisible(visible);
        AppConfig::instance().setValue("MainWindow/DriveBarVisible", visible);
    });
}

QWidget* setupCentralPanels(QWidget* parentWidget);
QWidget* MainWindow::setupCentralPanels(QWidget* parentWidget) {
    QWidget* bodyWrapper = new QWidget(parentWidget);
    bodyWrapper->setObjectName("BodyWrapper");
    m_bodyLayout = new QVBoxLayout(bodyWrapper);
    m_bodyLayout->setContentsMargins(kLayoutEdgeMargin, 0, kLayoutEdgeMargin, kLayoutEdgeMargin);
    m_bodyLayout->setSpacing(0);

    m_mainSplitter = new QSplitter(Qt::Horizontal, bodyWrapper);
    m_mainSplitter->setHandleWidth(5);
    m_mainSplitter->setChildrenCollapsible(false);

    m_navPanel      = new NavPanel(this);      m_navPanel->setObjectName("SidebarContainer");
    m_sidebarContainer = new SidebarContainerWidget(this); m_sidebarContainer->setObjectName("SidebarContainerWidget");
    m_favoritePanel = m_sidebarContainer->favoritePanel();
    m_contentPanel  = new ContentPanel(this);  m_contentPanel->setObjectName("EditorContainer");
    m_metaPanel     = new MetaPanel(this);     m_metaPanel->setObjectName("MetadataContainer");
    m_filterPanel   = new FilterPanel(this);   m_filterPanel->setObjectName("FilterContainer");

    m_mainSplitter->addWidget(m_navPanel);
    m_mainSplitter->addWidget(m_sidebarContainer);
    m_mainSplitter->addWidget(m_contentPanel);
    m_mainSplitter->addWidget(m_metaPanel);
    m_mainSplitter->addWidget(m_filterPanel);

    m_bodyLayout->addWidget(m_mainSplitter);

    return bodyWrapper;
}

void MainWindow::setupControllersAndMediators() {
    m_panelLayoutManager = new PanelLayoutManager(this, m_mainSplitter, m_navPanel, m_sidebarContainer, m_contentPanel, m_metaPanel, m_filterPanel, this);
    m_panelLayoutManager->initLayout();

    m_shortcutController = new AppShortcutController(this, m_searchController, this);
    connect(m_shortcutController, &AppShortcutController::togglePinRequested, this, [this]() {
        if (m_titleBarWidget) {
            bool nextState = !m_titleBarWidget->isPinned();
            m_titleBarWidget->setPinned(nextState);
            FramelessWindowHelper::setAlwaysOnTop(this, nextState);
            AppConfig::instance().setValue("MainWindow/AlwaysOnTop", nextState);
        }
    });

    PanelMediatorComponents components;
    components.navPanel = m_navPanel;
    components.favoritePanel = m_favoritePanel;
    components.libraryPanel = m_sidebarContainer ? m_sidebarContainer->libraryPanel() : nullptr;
    components.markedPanel = m_sidebarContainer ? m_sidebarContainer->markedPanel() : nullptr;
    components.contentPanel = m_contentPanel;
    components.metaPanel = m_metaPanel;
    components.filterPanel = m_filterPanel;
    components.addressBar = m_addressBar;
    components.searchController = m_searchController;
    components.titleBar = m_titleBarWidget;
    components.layoutManager = m_panelLayoutManager;
    components.shortcutController = m_shortcutController;

    m_panelMediator = new PanelMediator(components, this);
    m_panelMediator->setupConnections();

    connect(m_panelMediator, &PanelMediator::statusMessageRequested, this, [this](const QString& msg) {
        if (m_statusLeft) m_statusLeft->setText(msg);
    });

    connect(m_panelLayoutManager, &PanelLayoutManager::panelVisibilityChanged, this, [this](const QString&, bool) {
        updateStatusBarButtonHighlights();
    });
    connect(m_panelLayoutManager, &PanelLayoutManager::layoutResetCompleted, this, [this]() {
        updateStatusBarButtonHighlights();
    });
    updateStatusBarButtonHighlights();
}

void MainWindow::setupStatusBar(QWidget* parentWidget) {
    m_statusBarWidget = new QWidget(parentWidget);
    m_statusBarWidget->setObjectName("StatusBar");
    m_statusBarWidget->setFixedHeight(32);
    QHBoxLayout* statusL = new QHBoxLayout(m_statusBarWidget);
    statusL->setContentsMargins(kStatusBarHorizontalMargin, 0, kStatusBarHorizontalMargin, 0);
    statusL->setSpacing(0);

    m_statusLeft = new QLabel("就绪中...", m_statusBarWidget);
    m_statusLeft->setObjectName("StatusBarLeft");
    statusL->addWidget(m_statusLeft);
    statusL->addStretch(1);

    connect(m_contentPanel, &ContentPanel::statusBarMessageReady, this, [this](const QString& msg) {
        if (m_statusLeft) m_statusLeft->setText(msg);
    });

    auto updateStatus = [this]() {
        m_statusLeft->setText(CoreController::instance().statusText());
        m_statusLeft->setProperty("indexing", CoreController::instance().isIndexing());
        m_statusLeft->style()->unpolish(m_statusLeft);
        m_statusLeft->style()->polish(m_statusLeft);
    };
    connect(&CoreController::instance(), &CoreController::statusTextChanged, this, updateStatus);
    connect(&CoreController::instance(), &CoreController::isIndexingChanged, this, updateStatus);
    updateStatus();

    auto createStatusBtn = [this](const QString& iconKey, const QString& tip) -> QPushButton* {
        QPushButton* btn = new QPushButton(m_statusBarWidget);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setAttribute(Qt::WA_Hover);
        btn->setFixedSize(28, 20);
        btn->setCheckable(true);
        btn->setIcon(UiHelper::getIcon(iconKey, QColor("#EEEEEE"), 26));
        btn->setIconSize(QSize(27, 24));
        btn->setObjectName("StatusBarControlBtn");
        btn->setProperty("tooltipText", tip);
        if (m_hoverFilter) {
            btn->installEventFilter(m_hoverFilter);
        }
        return btn;
    };

    auto createSquareStatusBtn = [this](const QString& iconKey, const QString& tip) -> QPushButton* {
        QPushButton* btn = new QPushButton(m_statusBarWidget);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setAttribute(Qt::WA_Hover);
        btn->setFixedSize(22, 22);
        btn->setCheckable(true);
        btn->setIcon(UiHelper::getIcon(iconKey, QColor("#EEEEEE"), 18));
        btn->setIconSize(QSize(18, 18));
        btn->setObjectName("StatusBarControlBtn");
        btn->setProperty("tooltipText", tip);
        if (m_hoverFilter) {
            btn->installEventFilter(m_hoverFilter);
        }
        return btn;
    };

    m_btnToggleSortOrder = createSquareStatusBtn("arrow_down_long", "排序方向 (降序)");
    m_btnToggleSortOrder->setCheckable(false);
    m_btnToggleColumn    = createSquareStatusBtn("column_view", "分栏(C)");
    m_btnToggleJustified = createSquareStatusBtn("resize2", "自适应(A)");
    m_btnToggleGrid      = createSquareStatusBtn("gridgapm", "网格(G)");
    m_btnToggleList      = createSquareStatusBtn("list_ul", "列表(L)");

    m_btnToggleFilter   = createStatusBtn("隐藏筛选器", "切换筛选器面板 (显示/隐藏)");
    m_btnToggleMeta     = createStatusBtn("隐藏元数据面板", "切换元数据面板 (显示/隐藏)");
    m_btnContentPanel   = createStatusBtn("内容面板", "单独内容面板 (Tab 沉浸模式)");
    m_btnToggleFavorite = createStatusBtn("隐藏收藏栏", "切换收藏栏 (显示/隐藏)");
    m_btnToggleNav      = createStatusBtn("隐藏目录导航", "切换目录导航 (显示/隐藏)");
    m_btnPresetLayout   = createStatusBtn("显示收藏栏+内容面板+筛选器", "三栏预设 (右键可切换导航/收藏栏)");
    m_btnResetLayout    = createStatusBtn("重置分栏", "重置分栏");

    m_btnPresetLayout->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_btnPresetLayout, &QPushButton::customContextMenuRequested, this, [this](const QPoint& pos) {
        QMenu menu;
        UiHelper::applyMenuStyle(&menu);

        QString currentLeft = AppConfig::instance().getValue("MainWindow/PresetLeftPanel", "favorite").toString();

        QAction* actFav = menu.addAction(UiHelper::getIcon("显示收藏栏+内容面板+筛选器", QColor("#EEEEEE")), "显示 收藏栏 + 内容面板 + 筛选器");
        actFav->setCheckable(true);
        actFav->setChecked(currentLeft == "favorite");

        QAction* actNav = menu.addAction(UiHelper::getIcon("显示收藏栏+内容面板+筛选器", QColor("#EEEEEE")), "显示 目录导航 + 内容面板 + 筛选器");
        actNav->setCheckable(true);
        actNav->setChecked(currentLeft == "nav");

        QActionGroup* group = new QActionGroup(&menu);
        group->addAction(actFav);
        group->addAction(actNav);
        group->setExclusive(true);

        connect(actFav, &QAction::triggered, this, [this]() {
            AppConfig::instance().setValue("MainWindow/PresetLeftPanel", "favorite");
            applyPresetLayout("favorite");
        });

        connect(actNav, &QAction::triggered, this, [this]() {
            AppConfig::instance().setValue("MainWindow/PresetLeftPanel", "nav");
            applyPresetLayout("nav");
        });

        menu.exec(m_btnPresetLayout->mapToGlobal(pos));
    });

    connect(m_btnToggleFilter, &QPushButton::clicked, this, [this]() {
        if (!m_panelLayoutManager) return;
        m_panelLayoutManager->setPanelVisible("nav", true);
        m_panelLayoutManager->setPanelVisible("favorite", true);
        m_panelLayoutManager->setPanelVisible("content", true);
        m_panelLayoutManager->setPanelVisible("meta", true);
        m_panelLayoutManager->setPanelVisible("filter", false);
        updateStatusBarButtonHighlights();
    });

    connect(m_btnToggleMeta, &QPushButton::clicked, this, [this]() {
        if (!m_panelLayoutManager) return;
        m_panelLayoutManager->setPanelVisible("nav", true);
        m_panelLayoutManager->setPanelVisible("favorite", true);
        m_panelLayoutManager->setPanelVisible("content", true);
        m_panelLayoutManager->setPanelVisible("meta", false);
        m_panelLayoutManager->setPanelVisible("filter", true);
        updateStatusBarButtonHighlights();
    });

    connect(m_btnContentPanel, &QPushButton::clicked, this, [this]() {
        if (!m_panelLayoutManager) return;
        m_panelLayoutManager->setPanelVisible("nav", false);
        m_panelLayoutManager->setPanelVisible("favorite", false);
        m_panelLayoutManager->setPanelVisible("content", true);
        m_panelLayoutManager->setPanelVisible("meta", false);
        m_panelLayoutManager->setPanelVisible("filter", false);
        updateStatusBarButtonHighlights();
    });

    connect(m_btnToggleFavorite, &QPushButton::clicked, this, [this]() {
        if (!m_panelLayoutManager) return;
        m_panelLayoutManager->setPanelVisible("nav", true);
        m_panelLayoutManager->setPanelVisible("favorite", false);
        m_panelLayoutManager->setPanelVisible("content", true);
        m_panelLayoutManager->setPanelVisible("meta", true);
        m_panelLayoutManager->setPanelVisible("filter", true);
        updateStatusBarButtonHighlights();
    });

    connect(m_btnToggleNav, &QPushButton::clicked, this, [this]() {
        if (!m_panelLayoutManager) return;
        m_panelLayoutManager->setPanelVisible("nav", false);
        m_panelLayoutManager->setPanelVisible("favorite", true);
        m_panelLayoutManager->setPanelVisible("content", true);
        m_panelLayoutManager->setPanelVisible("meta", true);
        m_panelLayoutManager->setPanelVisible("filter", true);
        updateStatusBarButtonHighlights();
    });

    connect(m_btnPresetLayout, &QPushButton::clicked, this, [this]() {
        QString presetLeft = AppConfig::instance().getValue("MainWindow/PresetLeftPanel", "favorite").toString();
        applyPresetLayout(presetLeft);
    });

    auto getActivePanel = [this]() -> ContentPanel* {
        if (m_panelMediator && m_panelMediator->activeContentPanel()) {
            return m_panelMediator->activeContentPanel();
        }
        return m_contentPanel;
    };

    connect(m_btnToggleSortOrder, &QPushButton::clicked, this, [this, getActivePanel]() {
        ContentPanel* target = getActivePanel();
        if (target) {
            Qt::SortOrder current = target->currentSortOrder();
            Qt::SortOrder next = (current == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
            target->setSortOrder(next);
            updateStatusBarButtonHighlights();
        }
    });

    connect(m_btnToggleColumn, &QPushButton::clicked, this, [this, getActivePanel]() {
        ContentPanel* target = getActivePanel();
        if (target) {
            target->setViewMode(ContentPanel::ColumnView);
            updateStatusBarButtonHighlights();
        }
    });

    connect(m_btnToggleJustified, &QPushButton::clicked, this, [this, getActivePanel]() {
        ContentPanel* target = getActivePanel();
        if (target) {
            target->setViewMode(ContentPanel::JustifiedViewMode);
            updateStatusBarButtonHighlights();
        }
    });

    connect(m_btnToggleGrid, &QPushButton::clicked, this, [this, getActivePanel]() {
        ContentPanel* target = getActivePanel();
        if (target) {
            target->setViewMode(ContentPanel::GridView);
            updateStatusBarButtonHighlights();
        }
    });

    connect(m_btnToggleList, &QPushButton::clicked, this, [this, getActivePanel]() {
        ContentPanel* target = getActivePanel();
        if (target) {
            target->setViewMode(ContentPanel::ListView);
            updateStatusBarButtonHighlights();
        }
    });

    if (m_contentPanel) {
        connect(m_contentPanel, &ContentPanel::viewModeChanged, this, [this](ContentPanel::ViewMode) {
            updateStatusBarButtonHighlights();
        });
    }

    connect(m_btnResetLayout, &QPushButton::clicked, this, [this]() {
        if (!m_panelLayoutManager) return;
        m_panelLayoutManager->setPanelVisible("nav", true);
        m_panelLayoutManager->setPanelVisible("favorite", true);
        m_panelLayoutManager->setPanelVisible("content", true);
        m_panelLayoutManager->setPanelVisible("meta", true);
        m_panelLayoutManager->setPanelVisible("filter", true);
        m_panelLayoutManager->resetSplitterLayout();
        updateStatusBarButtonHighlights();
    });

    QFrame* sepLineSort = new QFrame(m_statusBarWidget);
    sepLineSort->setFrameShape(QFrame::VLine);
    sepLineSort->setFixedWidth(1);
    sepLineSort->setFixedHeight(14);
    sepLineSort->setObjectName("MenuSeparatorLine");

    QFrame* sepLine = new QFrame(m_statusBarWidget);
    sepLine->setFrameShape(QFrame::VLine);
    sepLine->setFixedWidth(1);
    sepLine->setFixedHeight(14);
    sepLine->setObjectName("MenuSeparatorLine");

    statusL->setSpacing(4);
    statusL->addWidget(m_btnToggleSortOrder);
    statusL->addWidget(sepLineSort);
    statusL->addWidget(m_btnToggleColumn);
    statusL->addWidget(m_btnToggleJustified);
    statusL->addWidget(m_btnToggleGrid);
    statusL->addWidget(m_btnToggleList);
    statusL->addWidget(sepLine);
    statusL->addWidget(m_btnResetLayout);
    statusL->addWidget(m_btnPresetLayout);
    statusL->addWidget(m_btnToggleNav);
    statusL->addWidget(m_btnToggleFavorite);
    statusL->addWidget(m_btnContentPanel);
    statusL->addWidget(m_btnToggleMeta);
    statusL->addWidget(m_btnToggleFilter);
}

void MainWindow::applyPresetLayout(const QString& leftPanel) {
    if (!m_panelLayoutManager) return;
    if (leftPanel == "nav") {
        m_panelLayoutManager->setPanelVisible("nav", true);
        m_panelLayoutManager->setPanelVisible("favorite", false);
    } else {
        m_panelLayoutManager->setPanelVisible("favorite", true);
        m_panelLayoutManager->setPanelVisible("nav", false);
    }
    m_panelLayoutManager->setPanelVisible("content", true);
    m_panelLayoutManager->setPanelVisible("meta", false);
    m_panelLayoutManager->setPanelVisible("filter", true);
    updateStatusBarButtonHighlights();
}

void MainWindow::updateStatusBarButtonHighlights() {
    if (!m_panelLayoutManager) return;

    bool isImm = m_panelLayoutManager->isImmersiveMode();
    bool navVis = m_panelLayoutManager->isPanelVisible("nav");
    bool favVis = m_panelLayoutManager->isPanelVisible("favorite");
    bool metaVis = m_panelLayoutManager->isPanelVisible("meta");
    bool filterVis = m_panelLayoutManager->isPanelVisible("filter");

    QSignalBlocker b1(m_btnToggleFilter);
    QSignalBlocker b2(m_btnToggleMeta);
    QSignalBlocker b3(m_btnContentPanel);
    QSignalBlocker b4(m_btnToggleFavorite);
    QSignalBlocker b5(m_btnToggleNav);
    QSignalBlocker b6(m_btnPresetLayout);
    QSignalBlocker b7(m_btnResetLayout);
    QSignalBlocker b8(m_btnToggleJustified);
    QSignalBlocker b9(m_btnToggleGrid);
    QSignalBlocker b10(m_btnToggleList);
    QSignalBlocker b11(m_btnToggleSortOrder);
    QSignalBlocker b12(m_btnToggleColumn);

    ContentPanel* targetPanel = (m_panelMediator && m_panelMediator->activeContentPanel()) ? m_panelMediator->activeContentPanel() : m_contentPanel;
    if (targetPanel) {
        ContentPanel::ViewMode mode = targetPanel->currentViewMode();
        if (m_btnToggleColumn)    m_btnToggleColumn->setChecked(mode == ContentPanel::ColumnView);
        if (m_btnToggleJustified) m_btnToggleJustified->setChecked(mode == ContentPanel::JustifiedViewMode);
        if (m_btnToggleGrid)      m_btnToggleGrid->setChecked(mode == ContentPanel::GridView);
        if (m_btnToggleList)      m_btnToggleList->setChecked(mode == ContentPanel::ListView);

        Qt::SortOrder sortOrd = targetPanel->currentSortOrder();
        if (m_btnToggleSortOrder) {
            bool isAsc = (sortOrd == Qt::AscendingOrder);
            m_btnToggleSortOrder->setIcon(UiHelper::getIcon(isAsc ? "arrow_up_long" : "arrow_down_long", QColor("#EEEEEE"), 18));
            m_btnToggleSortOrder->setProperty("tooltipText", isAsc ? "升序 状态中" : "降序 状态中");
        }
    }

    if (m_btnToggleFilter)   m_btnToggleFilter->setChecked(false);
    if (m_btnToggleMeta)     m_btnToggleMeta->setChecked(false);
    if (m_btnContentPanel)   m_btnContentPanel->setChecked(false);
    if (m_btnToggleFavorite) m_btnToggleFavorite->setChecked(false);
    if (m_btnToggleNav)      m_btnToggleNav->setChecked(false);
    if (m_btnPresetLayout)   m_btnPresetLayout->setChecked(false);
    if (m_btnResetLayout)    m_btnResetLayout->setChecked(false);

    if (isImm || (!navVis && !favVis && !metaVis && !filterVis)) {
        if (m_btnContentPanel) m_btnContentPanel->setChecked(true);
    } else if (navVis && favVis && metaVis && filterVis) {
        if (m_btnResetLayout) m_btnResetLayout->setChecked(true);
    } else if (navVis && favVis && metaVis && !filterVis) {
        if (m_btnToggleFilter) m_btnToggleFilter->setChecked(true);
    } else if (navVis && favVis && !metaVis && filterVis) {
        if (m_btnToggleMeta) m_btnToggleMeta->setChecked(true);
    } else if (navVis && !favVis && metaVis && filterVis) {
        if (m_btnToggleFavorite) m_btnToggleFavorite->setChecked(true);
    } else if (!navVis && favVis && metaVis && filterVis) {
        if (m_btnToggleNav) m_btnToggleNav->setChecked(true);
    } else if (((favVis && !navVis) || (navVis && !favVis)) && !metaVis && filterVis) {
        if (m_btnPresetLayout) m_btnPresetLayout->setChecked(true);
    }
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);

    // 关键修正 5：在实际展示事件中再次核实同步最大化图标（结合 0ms 单次定时器处理 DWM 异步延迟）
#ifdef Q_OS_WIN
    if (m_titleBarWidget) {
        m_titleBarWidget->setWindowMaximized(::IsZoomed(reinterpret_cast<HWND>(winId())));
        QTimer::singleShot(0, this, [this]() {
            if (m_titleBarWidget) {
                m_titleBarWidget->setWindowMaximized(::IsZoomed(reinterpret_cast<HWND>(winId())));
            }
        });
    }
#endif

    if (!m_panelsInitialized) {
        m_panelsInitialized = true;
        if (m_navPanel) m_navPanel->deferredInit();

        bool restored = false;
        if (m_titleBarWidget && m_titleBarWidget->tabBar()) {
            restored = m_titleBarWidget->tabBar()->restoreStateFromConfig();
        }

        if (!restored) {
            QString lastPath = AppConfig::instance().getValue("MainWindow/LastPath", "computer://").toString();
            bool isValid = lastPath.contains("://") || QDir(lastPath).exists();
            NavigationService::instance().navigateTo(isValid ? lastPath : "computer://");
        }

        QTimer::singleShot(500, []() {
            ToolTipOverlay::instance()->silentWarmup();
        });
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W && (event->modifiers() & Qt::ControlModifier)) {
        close();
        event->accept();
        return;
    }
    setAttribute(Qt::WA_Hover);
    QMainWindow::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::onStatusBarStatsUpdated(int fileCount, int folderCount, int totalCount) {
    Q_UNUSED(fileCount); Q_UNUSED(folderCount); Q_UNUSED(totalCount);
}

void MainWindow::updateStatusBar() {
    onStatusBarStatsUpdated(0, 0, 0);
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::ActivationChange) {
        if (isActiveWindow()) {
            if (QWidget::mouseGrabber()) {
                QWidget::mouseGrabber()->releaseMouse();
            }
#ifdef Q_OS_WIN
            if (testAttribute(Qt::WA_WState_Created)) {
                ::ReleaseCapture();
            }
#endif
            unsetCursor();
        }
    } else if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            if (m_searchController && m_searchController->historyPanel()) {
                m_searchController->historyPanel()->hide();
            }
        }
        if (m_titleBarWidget) {
#ifdef Q_OS_WIN
            m_titleBarWidget->setWindowMaximized(::IsZoomed(reinterpret_cast<HWND>(winId())));
#else
            m_titleBarWidget->setWindowMaximized(isMaximized());
#endif
        }
        if (m_bodyLayout) {
            m_bodyLayout->setContentsMargins(kLayoutEdgeMargin, 0, kLayoutEdgeMargin, kLayoutEdgeMargin);
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    AppConfig::instance().setValue("MainWindow/LastPath", NavigationService::instance().currentUrl());
    AppConfig::instance().setValue("MainWindow/Geometry", saveGeometry());
    if (m_driveBarWidget) {
        AppConfig::instance().setValue("MainWindow/DriveBarVisible", m_driveBarWidget->isVisible());
    }
    if (m_panelLayoutManager) {
        m_panelLayoutManager->saveLayoutState();
    }
    if (m_titleBarWidget && m_titleBarWidget->tabBar()) {
        m_titleBarWidget->tabBar()->saveStateToConfig();
    }
    AppConfig::instance().sync();
    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
    if (m_framelessHelper && m_framelessHelper->handleNativeEvent(message, result)) {
        return true;
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

} // namespace QuarkMeta
