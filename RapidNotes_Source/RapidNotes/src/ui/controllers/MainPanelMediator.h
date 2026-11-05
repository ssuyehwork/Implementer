#pragma once

#include <QObject>
#include <QPointer>

class NoteModel;
class CategoryModel;
class QListView;
class QTreeView;
class QLineEdit;

namespace RapidNotes {

class CategoryTabBarWidget;
class CategoryPaneSplitManager;
class PanelLayoutManager;

/**
 * @brief 主窗口中介者 (PanelMediator)
 * 纯分类中介者，解耦各个视图面板、搜索框与标签栏，协调业务信号流动
 */
class MainPanelMediator : public QObject {
    Q_OBJECT

public:
    explicit MainPanelMediator(CategoryTabBarWidget* tabBar,
                               CategoryPaneSplitManager* splitManager,
                               PanelLayoutManager* layoutManager,
                               QObject* parent = nullptr);
    ~MainPanelMediator() override = default;

    void setupConnections();

signals:
    void categorySelected(int categoryId);
    void searchRequested(const QString& keyword);
    void refreshAllRequested();

public slots:
    void onCategoryClicked(int categoryId);
    void onSearchTextChanged(const QString& keyword);
    void onTabSwitched(int tabId, int categoryId);

private:
    QPointer<CategoryTabBarWidget> m_tabBar;
    QPointer<CategoryPaneSplitManager> m_splitManager;
    QPointer<PanelLayoutManager> m_layoutManager;
};

} // namespace RapidNotes
