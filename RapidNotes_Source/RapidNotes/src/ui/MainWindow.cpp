#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "MainWindow.h"
#include "TitleBarWidget.h"
#include "TabBarWidget.h"
#include "NavBarWidget.h"

#include "UiHelper.h"
#include "SearchHistoryPanel.h"
#include "AppShortcutController.h"
#include "LocalShortcutInterceptor.h"
#include "PanelMediator.h"
#include "SearchController.h"

#include "SidebarContainerWidget.h"

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

namespace RapidNotes {

static MainWindow* s_mainWindowInstance = nullptr;

constexpr int kLayoutEdgeMargin = 5;
constexpr int kStatusBarHorizontalMargin = 12;

MainWindow::~MainWindow() {
    if (s_mainWindowInstance == this) {
        s_mainWindowInstance = nullptr;
    }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    s_mainWindowInstance = this;
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    setMinimumHeight(700); // 宽度由 PanelLayoutManager::updateDynamicMinimumSize() 动态管理
    setWindowTitle("RapidNotes");

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
    // m_driveBarWidget removed

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
    bool driveBarVis = AppConfig::instance().getValue("MainWindow/DriveBarVisible", false).toBool();
    m_titleBarWidget->setDriveBarVisible(driveBarVis);

    connect(m_titleBarWidget, &TitleBarWidget::driveBarToggleRequested, this, [this](bool visible) {
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

    m_sidebarContainer = new SidebarContainerWidget(this); m_sidebarContainer->setObjectName("SidebarContainerWidget");
    m_contentPanel  = new ContentPanel(this);  m_contentPanel->setObjectName("EditorContainer");
    m_metaPanel     = new MetaPanel(this);     m_metaPanel->setObjectName("MetadataContainer");
    m_filterPanel   = new FilterPanel(this);   m_filterPanel->setObjectName("FilterContainer");

    m_mainSplitter->addWidget(m_sidebarContainer);
    m_mainSplitter->addWidget(m_contentPanel);
    m_mainSplitter->addWidget(m_metaPanel);
    m_mainSplitter->addWidget(m_filterPanel);

    m_bodyLayout->addWidget(m_mainSplitter);

    return bodyWrapper;
}

void MainWindow::setupControllersAndMediators() {
    m_panelLayoutManager = new PanelLayoutManager(this, m_mainSplitter,  m_sidebarContainer, m_contentPanel, m_metaPanel, m_filterPanel, this);
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
    components.libraryPanel = m_sidebarContainer ? m_sidebarContainer->libraryPanel() : nullptr;
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

    if (m_statusLeft) {
        m_statusLeft->setText("就绪");
    }

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
    m_btnToggleGrid      = createSquareStatusBtn("gridgapm", "网格(G)");
    m_btnToggleList      = createSquareStatusBtn("list_ul", "列表(L)");

    m_btnToggleFilter   = createStatusBtn("隐藏筛选器", "切换筛选器面板 (显示/隐藏)");
    m_btnToggleMeta     = createStatusBtn("隐藏元数据面板", "切换元数据面板 (显示/隐藏)");
    m_btnContentPanel   = createStatusBtn("内容面板", "单独内容面板 (Tab 沉浸模式)");
    m_btnResetLayout    = createStatusBtn("重置分栏", "重置分栏");

    connect(m_btnToggleFilter, &QPushButton::clicked, this, [this]() {
        if (!m_panelLayoutManager) return;
        bool cur = m_panelLayoutManager->isPanelVisible("filter");
        m_panelLayoutManager->setPanelVisible("filter", !cur);
        updateStatusBarButtonHighlights();
    });

    connect(m_btnToggleMeta, &QPushButton::clicked, this, [this]() {
        if (!m_panelLayoutManager) return;
        bool cur = m_panelLayoutManager->isPanelVisible("meta");
        m_panelLayoutManager->setPanelVisible("meta", !cur);
        updateStatusBarButtonHighlights();
    });

    connect(m_btnContentPanel, &QPushButton::clicked, this, [this]() {
        if (!m_panelLayoutManager) return;
        m_panelLayoutManager->toggleImmersiveMode();
        updateStatusBarButtonHighlights();
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
        m_panelLayoutManager->setPanelVisible("sidebar", true);
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
    
    
    statusL->addWidget(m_btnToggleGrid);
    statusL->addWidget(m_btnToggleList);
    statusL->addWidget(sepLine);
    statusL->addWidget(m_btnResetLayout);
    
    
    
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
    QSignalBlocker b7(m_btnResetLayout);
    QSignalBlocker b9(m_btnToggleGrid);
    QSignalBlocker b10(m_btnToggleList);
    QSignalBlocker b11(m_btnToggleSortOrder);

    ContentPanel* targetPanel = (m_panelMediator && m_panelMediator->activeContentPanel()) ? m_panelMediator->activeContentPanel() : m_contentPanel;
    if (targetPanel) {
        ContentPanel::ViewMode mode = targetPanel->currentViewMode();
        if (m_btnToggleGrid)      m_btnToggleGrid->setChecked(mode == ContentPanel::GridView);
        if (m_btnToggleList)      m_btnToggleList->setChecked(mode == ContentPanel::ListView);

        Qt::SortOrder sortOrd = targetPanel->currentSortOrder();
        if (m_btnToggleSortOrder) {
            bool isAsc = (sortOrd == Qt::AscendingOrder);
            m_btnToggleSortOrder->setIcon(UiHelper::getIcon(isAsc ? "arrow_up_long" : "arrow_down_long", QColor("#EEEEEE"), 18));
            m_btnToggleSortOrder->setProperty("tooltipText", isAsc ? "升序 状态中" : "降序 状态中");
        }
    }

    if (m_btnToggleFilter)   m_btnToggleFilter->setChecked(filterVis);
    if (m_btnToggleMeta)     m_btnToggleMeta->setChecked(metaVis);
    if (m_btnContentPanel)   m_btnContentPanel->setChecked(isImm);
    if (m_btnResetLayout)    m_btnResetLayout->setChecked(false);
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

MainWindow& MainWindow::instance() {
    static MainWindow s_instance;
    if (s_mainWindowInstance) {
        return *s_mainWindowInstance;
    }
    return s_instance;
}

void MainWindow::showAndActivate() {
    showNormal();
    raise();
    activateWindow();
}

} // namespace RapidNotes
