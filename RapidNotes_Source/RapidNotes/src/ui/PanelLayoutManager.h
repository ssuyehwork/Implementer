#pragma once

#include <QObject>
#include <QSplitter>
#include <QMainWindow>
#include <QMenu>
#include <QPoint>
#include <QPointer>

namespace QuarkMeta {

class NavPanel;
class FavoritePanel;
class SidebarContainerWidget;
class ContentPanel;
class MetaPanel;
class FilterPanel;

class PanelLayoutManager : public QObject {
    Q_OBJECT

public:
    explicit PanelLayoutManager(QMainWindow* mainWindow,
                                QSplitter* mainSplitter,
                                NavPanel* navPanel,
                        SidebarContainerWidget* sidebarContainer,
                                ContentPanel* contentPanel,
                                MetaPanel* metaPanel,
                                FilterPanel* filterPanel,
                                QObject* parent = nullptr);
    ~PanelLayoutManager() override = default;

    void initLayout();
    void resetSplitterLayout();
    void setPanelVisible(const QString& panelId, bool visible);
    bool isPanelVisible(const QString& panelId) const;
    bool isImmersiveMode() const;
    void toggleImmersiveMode();
    void populatePanelMenu(QMenu* menu);
    void showPanelContextMenu(const QPoint& globalPos);
    void updateDynamicMinimumSize();
    void saveLayoutState();

private:
    void savePreImmersiveState();
    void restorePreImmersiveState();

signals:
    void layoutResetCompleted();
    void panelVisibilityChanged(const QString& panelId, bool visible);

private:
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QSplitter> m_mainSplitter;

    QPointer<NavPanel> m_navPanel;
    QPointer<SidebarContainerWidget> m_sidebarContainer;
    QPointer<ContentPanel> m_contentPanel;
    QPointer<MetaPanel> m_metaPanel;
    QPointer<FilterPanel> m_filterPanel;

    // 🚀【物理基准】：面板基准 230px，QSplitter 句柄宽度 5px（旧版本原始机制，不叠加额外 margin）
    static constexpr int kBasePanelWidth = 230;
    static constexpr int kContentBaseWidth = 230;
    static constexpr int kSplitterHandleWidth = 5;
    static constexpr int kWindowAbsoluteMinWidth = 710; // 顶栏与三栏视界物理绝对下限 (3 * 230px + 10px + 10px = 710px)
};

} // namespace QuarkMeta