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

namespace RapidNotes {

/**
 * @brief 分类标签页多窗格状态树节点
 * 仅用于记录数据库分类 ID (notes.category_id)
 */
struct CategoryPaneTreeNode {
    bool isSplitter = false;
    Qt::Orientation orientation = Qt::Horizontal;
    int categoryId = -1;
    QString categoryName;
    bool isPrimary = false;
    QList<CategoryPaneTreeNode> children;
};

/**
 * @brief 单个分类标签页的分屏状态
 */
struct CategoryTabSplitState {
    Qt::Orientation orientation = Qt::Horizontal;
    QList<int> paneCategoryIds;
    int primaryIndex = 0;
    int activePaneIndex = 0;
    bool isSplit = false;
    CategoryPaneTreeNode rootNode;
};

/**
 * @brief 分类标签元信息
 */
struct CategoryTabInfo {
    int tabId = -1;
    int categoryId = -1;
    QString title;
    QString iconName;
    QString colorHex;
    QWidget* tabButton = nullptr;
    QPushButton* closeButton = nullptr;
    QLabel* titleLabel = nullptr;
    QLabel* iconLabel = nullptr;
    CategoryTabSplitState splitState;
};

/**
 * @brief 面向数据库分类的标签页栏组件
 * 由 QuarkMeta TabBarWidget 裁剪迁移，仅承载分类，彻底排除物理路径
 */
class CategoryTabBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit CategoryTabBarWidget(QWidget* parent = nullptr);
    ~CategoryTabBarWidget() override = default;

    int currentTabId() const { return m_currentTabId; }
    int currentCategoryId() const;
    int tabCount() const { return m_tabs.size(); }

    int addTab(int categoryId, const QString& title, const QString& iconName = "folder", const QString& colorHex = "");
    void closeTab(int tabId);
    void activateTab(int tabId);
    void updateTabTitle(int tabId, const QString& title);
    void updateTabCategory(int tabId, int categoryId, const QString& title);

    CategoryTabSplitState getTabSplitState(int tabId) const;
    void setTabSplitState(int tabId, const CategoryTabSplitState& state);

signals:
    void tabChanged(int tabId, int categoryId);
    void tabClosed(int tabId);
    void newTabRequested();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void rebuildTabLayout();
    void updateTabStyles();
    void ensureActiveTabVisible();

    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_scrollContainer = nullptr;
    QHBoxLayout* m_tabLayout = nullptr;
    QPushButton* m_btnNewTab = nullptr;

    QList<CategoryTabInfo> m_tabs;
    int m_currentTabId = -1;
    int m_nextTabId = 1;
};

} // namespace RapidNotes
