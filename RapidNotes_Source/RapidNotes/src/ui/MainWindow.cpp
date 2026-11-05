#include "MainWindow.h"
#include "CategoryTabBarWidget.h"
#include "controllers/CategoryPaneSplitManager.h"
#include "controllers/PanelLayoutManager.h"
#include "controllers/MainPanelMediator.h"
#include "CleanListView.h"
#include "DropTreeView.h"
#include "IconHelper.h"
#include "QuickNoteDelegate.h"
#include "../models/NoteModel.h"
#include "../models/CategoryModel.h"
#include "../core/DatabaseManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QSortFilterProxyModel>

namespace RapidNotes {

MainWindow& MainWindow::instance() {
    static MainWindow s_inst;
    return s_inst;
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setObjectName("RapidNotesMainWindow");
    setWindowTitle("RapidNotes - 数据管理中枢");
    resize(1100, 720);
    setStyleSheet("background-color: #1e1e1e; color: #ffffff;");

    initUI();
}

void MainWindow::initUI() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    auto* rootLayout = new QVBoxLayout(m_centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 1. 顶部地址栏（面向文本数据，仅显示当前分类名称，不包含物理路径）
    m_topToolBar = new QWidget(this);
    m_topToolBar->setFixedHeight(40);
    m_topToolBar->setStyleSheet("background-color: #252526; border-bottom: 1px solid #333333;");

    auto* topLayout = new QHBoxLayout(m_topToolBar);
    topLayout->setContentsMargins(12, 4, 12, 4);
    topLayout->setSpacing(8);

    auto* iconCategory = new QLabel(m_topToolBar);
    iconCategory->setPixmap(IconHelper::getIcon("category", "#4a90e2", 18).pixmap(18, 18));
    topLayout->addWidget(iconCategory);

    auto* addressTitle = new QLabel("当前分类:", m_topToolBar);
    addressTitle->setStyleSheet("color: #888888; font-size: 12px;");
    topLayout->addWidget(addressTitle);

    m_addressCategoryLabel = new QLabel("全部数据", m_topToolBar);
    m_addressCategoryLabel->setStyleSheet("color: #4facfe; font-weight: bold; font-size: 13px;");
    topLayout->addWidget(m_addressCategoryLabel);

    topLayout->addStretch(1);

    m_btnRefresh = new QPushButton(m_topToolBar);
    m_btnRefresh->setFixedSize(28, 28);
    m_btnRefresh->setIcon(IconHelper::getIcon("refresh", "#aaaaaa", 16));
    m_btnRefresh->setToolTip("刷新数据");
    m_btnRefresh->setCursor(Qt::PointingHandCursor);
    m_btnRefresh->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 4px; } QPushButton:hover { background-color: rgba(255, 255, 255, 0.1); }");
    connect(m_btnRefresh, &QPushButton::clicked, this, [this]() {
        loadNotesForCategory(m_currentCategoryId);
        emit dataChanged();
    });
    topLayout->addWidget(m_btnRefresh);

    rootLayout->addWidget(m_topToolBar);

    // 2. 分类标签页栏 (CategoryTabBarWidget)
    m_tabBar = new CategoryTabBarWidget(this);
    rootLayout->addWidget(m_tabBar);

    // 3. 主分割器（左侧分类树 + 右侧笔记内容列表）
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setHandleWidth(1);
    m_mainSplitter->setStyleSheet("QSplitter::handle { background-color: #2D2D2D; }");

    // 左侧分类面板
    m_sidebarPanel = new QWidget(m_mainSplitter);
    m_sidebarPanel->setMinimumWidth(180);
    auto* sidebarLayout = new QVBoxLayout(m_sidebarPanel);
    sidebarLayout->setContentsMargins(4, 4, 4, 4);

    m_categoryTree = new DropTreeView(m_sidebarPanel);
    m_categoryModel = new CategoryModel(this);
    m_categoryTree->setModel(m_categoryModel);
    m_categoryTree->setHeaderHidden(true);
    m_categoryTree->setStyleSheet("QTreeView { background-color: #1e1e1e; border: none; color: #ccc; } QTreeView::item:hover { background-color: #2a2d2e; } QTreeView::item:selected { background-color: #094771; }");
    sidebarLayout->addWidget(m_categoryTree);
    m_mainSplitter->addWidget(m_sidebarPanel);

    // 右侧笔记列表面板
    m_contentPanel = new QWidget(m_mainSplitter);
    auto* contentLayout = new QVBoxLayout(m_contentPanel);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    m_noteListView = new CleanListView(m_contentPanel);
    m_noteModel = new NoteModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_noteModel);
    m_noteListView->setModel(m_proxyModel);
    m_noteListView->setItemDelegate(new QuickNoteDelegate(this));
    m_noteListView->setStyleSheet("QListView { background-color: #181818; border: none; color: #ffffff; }");
    contentLayout->addWidget(m_noteListView);
    m_mainSplitter->addWidget(m_contentPanel);

    rootLayout->addWidget(m_mainSplitter, 1);

    // 4. 布局管理器与大脑中介者
    m_layoutManager = new PanelLayoutManager(this, m_mainSplitter, m_sidebarPanel, m_contentPanel, nullptr, this);
    m_layoutManager->initLayout();

    m_mediator = new MainPanelMediator(m_tabBar, nullptr, m_layoutManager, this);
    m_mediator->setupConnections();

    connect(m_mediator, &MainPanelMediator::categorySelected, this, [this](int categoryId) {
        loadNotesForCategory(categoryId);
    });

    connect(m_categoryTree, &QTreeView::clicked, this, [this](const QModelIndex& index) {
        if (!index.isValid()) return;
        int catId = index.data(Qt::UserRole).toInt();
        QString catName = index.data(Qt::DisplayRole).toString();
        setCurrentCategory(catId, catName);
    });

    // 默认添加第一个标签页：全部数据
    m_tabBar->addTab(-100, "全部数据", "database", "#4a90e2");
    loadNotesForCategory(-100);
}

void MainWindow::updateAddressCategory(const QString& categoryName) {
    if (m_addressCategoryLabel) {
        m_addressCategoryLabel->setText(categoryName);
    }
}

void MainWindow::loadNotesForCategory(int categoryId) {
    m_currentCategoryId = categoryId;
    QString catName = "全部数据";
    if (categoryId > 0) {
        auto catMap = DatabaseManager::instance().getCategoryById(categoryId);
        if (!catMap.isEmpty()) catName = catMap.value("name").toString();
    } else if (categoryId == -1) {
        catName = "未分类";
    }
    updateAddressCategory(catName);

    // 从数据库读取分类下的笔记列表并回填 NoteModel
    QList<QVariantMap> notes;
    if (categoryId > 0) {
        notes = DatabaseManager::instance().getNotesByCategory(categoryId);
    } else if (categoryId == -1) {
        notes = DatabaseManager::instance().getUncategorizedNotes();
    } else {
        notes = DatabaseManager::instance().getAllNotes();
    }
    if (m_noteModel) {
        m_noteModel->setNotes(notes);
    }
}

void MainWindow::setCurrentCategory(int categoryId, const QString& categoryName) {
    m_currentCategoryId = categoryId;
    updateAddressCategory(categoryName);
    if (m_tabBar) {
        m_tabBar->updateTabCategory(m_tabBar->currentTabId(), categoryId, categoryName);
    }
    loadNotesForCategory(categoryId);
}

void MainWindow::showAndActivate() {
    showNormal();
    raise();
    activateWindow();
    loadNotesForCategory(m_currentCategoryId);
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QMainWindow::closeEvent(event);
}

} // namespace RapidNotes
