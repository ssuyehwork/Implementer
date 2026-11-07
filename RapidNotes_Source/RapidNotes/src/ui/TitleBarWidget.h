#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

namespace QuarkMeta {

class HoverEventFilter;
class TabBarWidget;

/**
 * @brief 独立标题栏组件
 * 封装 LOGO、多标签页、缩放滑杆、排列视图菜单、新建菜单、盘符折叠按钮、布局重置、窗口控制按钮(置顶/最小化/最大化/关闭)
 * 纯 View 部件：完全不依赖 ContentPanel/PanelLayoutManager 的指针或头文件，且不泄漏内部控件指针。
 */
class TitleBarWidget : public QWidget {
    Q_OBJECT

public:
    enum ViewModeOption {
        JustifiedViewMode,
        GridViewMode,
        ListViewMode,
        ColumnViewMode
    };
    Q_ENUM(ViewModeOption)

    explicit TitleBarWidget(QWidget* parent = nullptr, HoverEventFilter* hoverFilter = nullptr);
    ~TitleBarWidget() override = default;

    TabBarWidget* tabBar() const { return m_tabBar; }

    bool isPinned() const;
    void setPinned(bool pinned);
    void setZoomLevel(int value);
    Q_INVOKABLE void setWindowMaximized(bool maximized);
    void setViewModeOption(ViewModeOption mode);
    void setDriveBarVisible(bool visible);

signals:
    void driveBarToggleRequested(bool visible);
    void pinToggled(bool pinned);
    void zoomLevelChanged(int value);
    void viewModeRequested(TitleBarWidget::ViewModeOption mode);
    void createItemRequested(const QString& type);
    void layoutMenuRequested(const QPoint& globalPos);

private:
    void initUi(HoverEventFilter* hoverFilter);
    void setupViewMenu();
    void setupCreateMenu();

    QHBoxLayout* m_layout = nullptr;
    QLabel* m_logoLabel = nullptr;
    TabBarWidget* m_tabBar = nullptr;
    QWidget* m_rightToolContainer = nullptr;

    QPushButton* m_btnViewMenu = nullptr;
    QSlider* m_sizeSlider = nullptr;

    QPushButton* m_btnToggleDriveBar = nullptr;
    QPushButton* m_btnLayout = nullptr;
    QPushButton* m_btnCreate = nullptr;
    QPushButton* m_btnPinTop = nullptr;
    QPushButton* m_btnMin = nullptr;
    QPushButton* m_btnMax = nullptr;
    QPushButton* m_btnClose = nullptr;

    ViewModeOption m_currentViewMode = GridViewMode;

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

} // namespace QuarkMeta
