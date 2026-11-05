#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPointer>
#include <QShowEvent>

class NoteModel;
class CleanListView;
class DropTreeView;
class CategoryModel;
class QSortFilterProxyModel;

namespace RapidNotes {

class CategoryTabBarWidget;
class CategoryPaneSplitManager;
class PanelLayoutManager;
class MainPanelMediator;

/**
 * @brief 面向数据库分类的独立数据管理大窗口 (MainWindow)
 * 仅用于管理 notes 文本数据，严格不涉及物理磁盘路径
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

    static MainWindow& instance();
    void showAndActivate();

    void setCurrentCategory(int categoryId, const QString& categoryName);

signals:
    void dataChanged();

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void initUI();
    void updateAddressCategory(const QString& categoryName);
    void loadNotesForCategory(int categoryId);

    QWidget* m_centralWidget = nullptr;
    QWidget* m_topToolBar = nullptr;
    QLabel* m_addressCategoryLabel = nullptr;
    QPushButton* m_btnRefresh = nullptr;

    CategoryTabBarWidget* m_tabBar = nullptr;
    QSplitter* m_mainSplitter = nullptr;
    QWidget* m_sidebarPanel = nullptr;
    DropTreeView* m_categoryTree = nullptr;
    CategoryModel* m_categoryModel = nullptr;

    QWidget* m_contentPanel = nullptr;
    CleanListView* m_noteListView = nullptr;
    NoteModel* m_noteModel = nullptr;
    QSortFilterProxyModel* m_proxyModel = nullptr;

    PanelLayoutManager* m_layoutManager = nullptr;
    MainPanelMediator* m_mediator = nullptr;

    int m_currentCategoryId = -1;
};

} // namespace RapidNotes
