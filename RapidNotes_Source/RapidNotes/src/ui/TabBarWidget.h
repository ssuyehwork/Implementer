#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QString>
#include <QEvent>
#include <QResizeEvent>
#include <QScrollArea>
#include <QWheelEvent>

namespace QuarkMeta {

class HoverEventFilter;

struct PaneTreeNode {
    bool isSplitter = false;
    Qt::Orientation orientation = Qt::Horizontal;
    QString path;
    bool isPrimary = false;
    QList<PaneTreeNode> children;
};

struct TabSplitState {
    Qt::Orientation orientation = Qt::Horizontal;
    QStringList panePaths;
    int primaryIndex = 0;
    int activePaneIndex = 0;
    bool isSplit = false;
    PaneTreeNode rootNode;
};

struct TabInfo {
    QString id;
    QString title;
    QString url;
    QString color;
    QString iconKey;
    bool active = false;
    TabSplitState splitState;
};

class TabItemButton : public QPushButton {
    Q_OBJECT
public:
    explicit TabItemButton(int index, QWidget* parent = nullptr);
    int index() const { return m_index; }
    void setIndex(int index) { m_index = index; }

    void setTabTitle(const QString& title);
    void setTabIcon(const QIcon& icon);
    void setActive(bool active);

signals:
    void tabClicked(int index);
    void middleClicked(int index);
    void closeClicked(int index);
    void customContextMenuRequested(int index, const QPoint& globalPos);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateElidedTitle();

    int m_index = -1;
    QPoint m_dragStartPos;
    bool m_isDragging = false;
    QString m_rawTitle;
    QLabel* m_iconLabel = nullptr;
    QLabel* m_titleLabel = nullptr;
    QPushButton* m_btnClose = nullptr;
};

class TabBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit TabBarWidget(QWidget* parent = nullptr, HoverEventFilter* hoverFilter = nullptr);
    ~TabBarWidget() override = default;

    void addTab(const QString& title = "此电脑", const QString& url = "computer://", bool switchToNew = true);
    void closeTab(int index);
    void closeOtherTabs(int index);
    void closeRightTabs(int index);
    void duplicateTab(int index);
    void mergeTab(int sourceIndex, int targetIndex);
    void splitTab(int tabIndex);
    void restoreLastClosedTab();
    void setCurrentIndex(int index, bool forceNotify = false);
    int currentIndex() const { return m_currentIndex; }
    int tabCount() const { return m_tabs.size(); }
    QString tabTitle(int index) const {
        if (index >= 0 && index < m_tabs.size()) return m_tabs[index].title;
        return QString();
    }
    QString tabUrl(int index) const {
        if (index >= 0 && index < m_tabs.size()) return m_tabs[index].url;
        return QString();
    }
    TabSplitState tabSplitState(int index) const {
        if (index >= 0 && index < m_tabs.size()) return m_tabs[index].splitState;
        return TabSplitState();
    }
    void setTabSplitState(int index, const TabSplitState& state) {
        if (index >= 0 && index < m_tabs.size()) {
            m_tabs[index].splitState = state;
        }
    }
    void updateSplitTabTitle(const TabSplitState& state);
    void updateCurrentTabTitle(const QString& title, const QString& url);
    void openOrFocusTab(const QString& path);
    void openInNewTab(const QString& path);
    void openInExistingTab(int tabIndex, const QString& path);

    void saveStateToConfig();
    bool restoreStateFromConfig();

    void selectNextTab();
    void selectPreviousTab();

signals:
    void tabAboutToChange(int oldIndex);
    void currentTabChanged(int index, const QString& url);
    void tabClosed(int index);
    void newTabRequested();
    void refreshRequested();
    void mergeRequested(int sourceIndex, int targetIndex);
    void splitRequested(int tabIndex);
    void folderDroppedOnTab(int tabIndex, const QString& folderPath);
    void tabColorChanged(int index, const QString& url, const QString& hexColor);
    void tabIconChanged(int index, const QString& url, const QString& iconKey);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void showTabContextMenu(int index, const QPoint& globalPos);
    void updateTabsUiState();
    void applyDiffUpdate();

    QHBoxLayout* m_mainLayout = nullptr;
    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_tabsContainer = nullptr;
    QHBoxLayout* m_tabsContainerLayout = nullptr;
    QHBoxLayout* m_tabsLayout = nullptr;
    QPushButton* m_btnNewTab = nullptr;
    HoverEventFilter* m_hoverFilter = nullptr;

    QList<TabInfo> m_tabs;
    QList<TabInfo> m_closedTabsHistory;
    QList<TabItemButton*> m_tabWidgets;
    int m_currentIndex = -1;
    bool m_isInitializing = true;
};

} // namespace QuarkMeta
