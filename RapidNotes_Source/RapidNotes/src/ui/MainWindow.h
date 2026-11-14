#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QToolBar>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QSystemTrayIcon>
#include <QSet>
#include <QSlider>
#include <QProgressBar>
#include <QDateTime>

#include "FramelessDialog.h"
#include "TagSelectorOverlay.h"
#include <QPointer>

namespace RapidNotes {

class TrayController;
class HoverEventFilter;
class AddressBar;
class TaskProgressStrip;
class SearchController; 
class TitleBarWidget;
class SettingsBarWidget;
class NavBarWidget;
class NavPanel;
class SidebarContainerWidget;
class ContentPanel;
class MetaPanel;
class FilterPanel;
class SearchHistoryPanel;
class AppShortcutController;
class PanelMediator;
class PanelLayoutManager;
class FramelessWindowHelper;

/**
 * @brief 主窗口类
 * 负责窗口生命周期、物理属性、顶层组件组装（Composition Root）与布局管理
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    PanelLayoutManager* layoutManager() const { return m_panelLayoutManager; }

public slots:
    void onStatusBarStatsUpdated(int fileCount, int folderCount, int totalCount);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void changeEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
    void closeEvent(QCloseEvent* event) override;

private:
    void initUi();
    
    // 拆分的私有阶段初始化函数
    void setupTopBars(QWidget* parentWidget);
    QWidget* setupCentralPanels(QWidget* parentWidget);
    void setupControllersAndMediators();
    void setupStatusBar(QWidget* parentWidget);

    void updateStatusBar();
    void applyPresetLayout(const QString& leftPanel);
    void updateStatusBarButtonHighlights();

    TitleBarWidget* m_titleBarWidget = nullptr;
    NavBarWidget* m_navBarWidget = nullptr;
    SettingsBarWidget* m_settingsBarWidget = nullptr;
    QVBoxLayout* m_bodyLayout = nullptr;

    // 导航与搜索组件句柄
    AddressBar* m_addressBar = nullptr;
    SearchController* m_searchController = nullptr;

    // 5 大核心面板与 Splitter
    NavPanel* m_navPanel = nullptr;
    SidebarContainerWidget* m_sidebarContainer = nullptr;
    ContentPanel* m_contentPanel = nullptr;
    MetaPanel* m_metaPanel = nullptr;
    FilterPanel* m_filterPanel = nullptr;

    QSplitter* m_mainSplitter = nullptr;

    // 状态管理
    bool m_isPinned = false;
    bool m_panelsInitialized = false;

    // 底部状态栏与悬浮进度条
    QLabel* m_statusLeft = nullptr;
    QWidget* m_statusBarWidget = nullptr;
    TaskProgressStrip* m_taskProgressStrip = nullptr;

    QPushButton* m_btnToggleFilter = nullptr;
    QPushButton* m_btnToggleMeta = nullptr;
    QPushButton* m_btnContentPanel = nullptr;
    QPushButton* m_btnToggleNav = nullptr;
    QPushButton* m_btnPresetLayout = nullptr;
    QPushButton* m_btnResetLayout = nullptr;
    QPushButton* m_btnToggleSortOrder = nullptr;
    QPushButton* m_btnToggleGrid = nullptr;
    QPushButton* m_btnToggleList = nullptr;

    // 系统托盘控制器
    TrayController* m_trayController = nullptr;
    HoverEventFilter*     m_hoverFilter     = nullptr;

    // 模块化控制器与中介者
    AppShortcutController* m_shortcutController = nullptr;
    PanelMediator* m_panelMediator = nullptr;
    PanelLayoutManager*       m_panelLayoutManager = nullptr;
    FramelessWindowHelper*    m_framelessHelper = nullptr;
};

} // namespace RapidNotes

using RapidNotes::MainWindow;

