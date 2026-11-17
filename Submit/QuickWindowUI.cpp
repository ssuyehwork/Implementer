#include "AppLockWidget.h"
#include "ToolTipOverlay.h"
#include "QuickWindow.h"
#include "RecentNotesWindow.h"
#include "SearchLineEdit.h"
#include "CleanListView.h"
#include "ClickableLineEdit.h"
#include "CategoryLockWidget.h"
#include "DropTreeView.h"
#include "LibraryPanel.h"
#include "QuickPreview.h"
#include "PasswordVerifyDialog.h"
#include "CategoryPasswordDialog.h"
#include "QuickFilterPanel.h"
#include "NoteEditWindow.h"
#include "StringUtils.h"
#include "TitleEditorDialog.h"
#include "AdvancedTagSelector.h"
#include "IconHelper.h"
#include "QuickNoteDelegate.h"
#include "FramelessDialog.h"
#include "controllers/QuickMediator.h"
#include "controllers/QuickShortcutHandler.h"
#include "controllers/QuickContextMenuBuilder.h"
#include "controllers/QuickPasteDeliveryService.h"
#include "../models/NoteModel.h"
#include "../meta/LibraryService.h"
#include "../core/DatabaseManager.h"
#include "../core/ModelContract.h"
#include "../core/TagCodec.h"
#include "../core/ClipboardMonitor.h"
#include "../core/FileStorageHelper.h"
#include "../core/ActionRecorder.h"
#include "../core/ShortcutManager.h"
#include "../core/HotkeyManager.h"
#include "../core/KeyboardHook.h"

#include <QDebug>
#include <QPushButton>
#include <QGuiApplication>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QScreen>
#include <QKeyEvent>
#include <QGraphicsDropShadowEffect>
#include <QSettings>
#include <QMenu>
#include <QWindow>
#include <QShortcut>
#include <QKeySequence>
#include <QClipboard>
#include <QMimeData>
#include <QTimer>
#include <QApplication>
#include <QElapsedTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QUrl>
#include <QBuffer>
#include <QScrollBar>
#include <QWheelEvent>
#include <QRegularExpression>
#include <QImage>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QLineEdit>
#include <QColorDialog>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRandomGenerator>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

void QuickWindow::initUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);

    auto* container = new QWidget();
    container->setObjectName("QuickWindow_container");
    container->setMouseTracking(true);

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(7);
    shadow->setColor(QColor(0, 0, 0, 90));
    shadow->setOffset(0, 2);
    container->setGraphicsEffect(shadow);

    auto* containerLayout = new QHBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    auto* leftContent = new QWidget();
    leftContent->setObjectName("leftContent");
    leftContent->setMouseTracking(true);
    auto* leftLayout = new QVBoxLayout(leftContent);
    leftLayout->setContentsMargins(10, 10, 10, 5);
    leftLayout->setSpacing(8);

    m_searchEdit = new SearchLineEdit();
    m_searchEdit->setObjectName("QuickWindow_m_searchEdit");
    m_searchEdit->setFixedHeight(32);
    m_searchEdit->setPlaceholderText("搜索灵感 (双击查看历史)");
    m_searchEdit->setClearButtonEnabled(true);
    leftLayout->addWidget(m_searchEdit);

    m_splitter = new QSplitter(Qt::Horizontal);
    m_splitter->setHandleWidth(4);
    m_splitter->setChildrenCollapsible(false);

    m_listWrapper = new QWidget();
    auto* listWrapperLayout = new QVBoxLayout(m_listWrapper);
    listWrapperLayout->setContentsMargins(0, 0, 0, 0);
    listWrapperLayout->setSpacing(0);

    m_listFocusLine = new QWidget();
    m_listFocusLine->setFixedHeight(1);
    m_listFocusLine->setObjectName("QuickWindow_m_listFocusLine_5");
    m_listFocusLine->hide();
    listWrapperLayout->addWidget(m_listFocusLine);

    m_listStack = new QStackedWidget();
    m_listStack->setMinimumWidth(95);
    listWrapperLayout->addWidget(m_listStack);

    m_listView = new CleanListView();
    m_listView->setObjectName("QuickWindow_m_listView");
    m_listView->setDragEnabled(true);
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_listView->setIconSize(QSize(28, 28));
    m_listView->setAlternatingRowColors(true);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setMouseTracking(true);
    m_listView->setItemDelegate(new QuickNoteDelegate(this));
    m_model = new NoteModel(this);
    m_listView->setModel(m_model);
    m_listView->setAcceptDrops(true);
    m_listView->setDropIndicatorShown(true);

    connect(m_listView, &CleanListView::internalMoveRequested, this, [this](const QList<int>& ids, int row){
        if (m_currentFilterType == "recently_visited" || m_currentFilterType == "trash") {
            ToolTipOverlay::instance()->showText(QCursor::pos(), "<b style='color: #e67e22;'>[!] 当前视图不支持手动排序</b>");
            return;
        }
        if (m_mediator) {
            m_mediator->requestMoveNotesToRow(ids, row, m_currentFilterType, m_currentFilterValue);
        }
    });

    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_lockWidget = new CategoryLockWidget(this);
    connect(m_lockWidget, &CategoryLockWidget::unlocked, this, [this](){
        refreshData();
    });
    connect(m_lockWidget, &CategoryLockWidget::escPressed, this, [this](){
        this->setFocus();
    });
    connect(m_listView, &QListView::customContextMenuRequested, this, &QuickWindow::showListContextMenu);
    connect(m_listView, &QListView::doubleClicked, this, [this](const QModelIndex& index){
        activateNote(index);
    });

    connect(m_listView->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected) {
        if (!selected.isEmpty() && m_listView->hasFocus()) {
            m_bottomStackedWidget->setCurrentIndex(1);
        }
    });
    connect(m_listView, &QListView::clicked, [this](){
         m_bottomStackedWidget->setCurrentIndex(1);
    });

    // 侧边栏容器：彻底卸载双树，统一挂载 LibraryPanel 单一真相源
    m_sidebarWrapper = new QWidget();
    m_sidebarWrapper->setMinimumWidth(230);
    auto* sidebarLayout = new QVBoxLayout(m_sidebarWrapper);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    m_sidebarFocusLine = new QWidget();
    m_sidebarFocusLine->setFixedHeight(1);
    m_sidebarFocusLine->setObjectName("QuickWindow_m_sidebarFocusLine_6");
    m_sidebarFocusLine->hide();
    sidebarLayout->addWidget(m_sidebarFocusLine);

    m_libraryPanel = new RapidNotes::LibraryPanel(this);
    m_libraryPanel->setMaximumWidth(QWIDGETSIZE_MAX); // 允许随用户拖拽分割条自适应加宽
    if (auto* libTree = m_libraryPanel->treeView()) {
        libTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
        libTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        // 拦截原生右键菜单信号，接管为 QuickWindow 全功能菜单
        disconnect(libTree, &QWidget::customContextMenuRequested, nullptr, nullptr);
        connect(libTree, &QWidget::customContextMenuRequested, this, &QuickWindow::showSidebarMenu);
    }

    connect(m_libraryPanel, &RapidNotes::LibraryPanel::filterConditionSelected, this,
            [this](const QString& filterType, const QVariant& filterValue, const QString& displayName, const QString& colorHex) {
        m_currentFilterType = filterType;
        m_currentFilterValue = filterValue;
        m_currentCategoryColor = colorHex.isEmpty() ? "#4a90e2" : colorHex;

        if (m_currentFilterType == "category" && m_currentFilterValue.toInt() > 0) {
            StringUtils::recordRecentCategory(m_currentFilterValue.toInt());
        }

        updatePartitionStatus(displayName);
        applyListTheme(m_currentCategoryColor);
        m_currentPage = 1;
        refreshData();
        m_bottomStackedWidget->setCurrentIndex(0);
    });

    sidebarLayout->addWidget(m_libraryPanel, 1);

    m_listStack->addWidget(m_listView);
    m_listStack->addWidget(m_lockWidget);

    m_filterWrapper = new QFrame();
    m_filterWrapper->setObjectName("FilterWrapper");
    m_filterWrapper->setMinimumWidth(163);
    auto* fwLayout = new QVBoxLayout(m_filterWrapper);
    fwLayout->setContentsMargins(0, 0, 0, 0);
    fwLayout->setSpacing(0);

    m_filterPanel = new QuickFilterPanel(this);
    m_filterPanel->setObjectName("QuickWindow_m_filterPanel_9");
    connect(m_filterPanel, &QuickFilterPanel::filterChanged, this, &QuickWindow::refreshData);
    fwLayout->addWidget(m_filterPanel);
    m_filterWrapper->hide();

    m_splitter->addWidget(m_listWrapper);
    m_splitter->addWidget(m_filterWrapper);
    m_splitter->addWidget(m_sidebarWrapper);

    connect(m_splitter, &QSplitter::splitterMoved, this, [this](int pos, int index) {
        Q_UNUSED(pos); Q_UNUSED(index);
        this->updateFocusLines();
        if (m_filterWrapper && !m_filterWrapper->isHidden()) {
            int w = m_splitter->sizes().at(1);
            if (w >= 163) m_filterWidth = w;
        }
        if (m_sidebarWrapper && !m_sidebarWrapper->isHidden()) {
            int w = m_splitter->sizes().at(2);
            if (w >= 230) m_sidebarWidth = w;
        }
    });
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);
    m_splitter->setCollapsible(2, false);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 0);
    m_splitter->setStretchFactor(2, 0);
    m_splitter->setSizes({480, 163, 230});
    leftLayout->addWidget(m_splitter);

    auto* bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(2, 0, 0, 5);
    bottomLayout->setSpacing(10);

    m_statusLabel = new QLabel("当前分类: 全部数据");
    m_statusLabel->setObjectName("QuickWindow_m_statusLabel_10");
    m_statusLabel->setFixedHeight(32);
    bottomLayout->addWidget(m_statusLabel);

    m_bottomStackedWidget = new QStackedWidget();
    m_bottomStackedWidget->setFixedHeight(32);

    m_catSearchEdit = new SearchLineEdit();
    m_catSearchEdit->setHistoryKey("CategoryFilterHistory");
    m_catSearchEdit->setHistoryTitle("分类筛选历史");
    m_catSearchEdit->setPlaceholderText("筛选分类...");
    m_catSearchEdit->setClearButtonEnabled(true);

    QAction* filterIconAction = new QAction(IconHelper::getIcon("filter_funnel", "#888"), "", m_catSearchEdit);
    m_catSearchEdit->addAction(filterIconAction, QLineEdit::LeadingPosition);
    m_catSearchEdit->setObjectName("QuickWindow_m_catSearchEdit");

    connect(m_catSearchEdit, &QLineEdit::textChanged, this, [this](const QString& text){
        if (m_libraryPanel) {
            m_libraryPanel->filterTreeNodes(text);
        }
    });

    connect(m_catSearchEdit, &QLineEdit::returnPressed, this, [this](){
        QString text = m_catSearchEdit->text().trimmed();
        if (!text.isEmpty()) {
            m_catSearchEdit->addHistoryEntry(text);
        }
    });

    m_tagEdit = new ClickableLineEdit();
    m_tagEdit->setObjectName("QuickWindow_m_tagEdit");
    m_tagEdit->setPlaceholderText("输入标签添加... (双击显示历史)");
    connect(m_tagEdit, &QLineEdit::returnPressed, this, &QuickWindow::handleTagInput);
    connect(m_tagEdit, &ClickableLineEdit::doubleClicked, this, [this](){
        this->openTagSelector();
    });

    m_bottomStackedWidget->addWidget(m_catSearchEdit);
    m_bottomStackedWidget->addWidget(m_tagEdit);
    bottomLayout->addWidget(m_bottomStackedWidget, 1);
    leftLayout->addLayout(bottomLayout);
    containerLayout->addWidget(leftContent);

    QWidget* customToolbar = new QWidget(this);
    customToolbar->setObjectName("customToolbar");
    customToolbar->setMouseTracking(true);
    customToolbar->setAttribute(Qt::WA_Hover);
    customToolbar->setFixedWidth(36);

    QVBoxLayout* toolLayout = new QVBoxLayout(customToolbar);
    toolLayout->setContentsMargins(4, 8, 4, 8);
    toolLayout->setSpacing(0);

    auto getScHint = [](const QString& id) -> QString {
        QKeySequence seq = ShortcutManager::instance().getShortcut(id);
        if (seq.isEmpty()) return "";
        QString keyText = seq.toString(QKeySequence::NativeText);
        keyText.replace("+", " + ");
        return QString(" （%1）").arg(keyText);
    };

    auto createToolBtn = [this, getScHint](QString iconName, QString color, QString tooltip, QString scId = "", int rotate = 0) {
        QPushButton* btn = new QPushButton();
        QIcon icon = IconHelper::getIcon(iconName, color);
        if (rotate != 0) {
            QPixmap pix = icon.pixmap(28, 28);
            QTransform trans;
            trans.rotate(rotate);
            btn->setIcon(QIcon(pix.transformed(trans, Qt::SmoothTransformation)));
        } else {
            btn->setIcon(icon);
        }
        btn->setIconSize(QSize(18, 18));
        btn->setFixedSize(24, 24);

        QString fullTip = tooltip;
        if (!scId.isEmpty()) fullTip += getScHint(scId);
        btn->setProperty("tooltipText", fullTip);

        btn->setCursor(Qt::PointingHandCursor);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setMouseTracking(true);
        btn->setAttribute(Qt::WA_Hover);
        btn->installEventFilter(this);
        return btn;
    };

    QPushButton* btnClose = createToolBtn("close", "#FFFFFF", "关闭", "qw_close");
    btnClose->setObjectName("btnClose");
    connect(btnClose, &QPushButton::clicked, this, &QuickWindow::hide);

    QPushButton* btnMin = createToolBtn("minimize", "#aaaaaa", "最小化");
    btnMin->setObjectName("btnMin");
    connect(btnMin, &QPushButton::clicked, this, &QuickWindow::showMinimized);

    QPushButton* btnPin = createToolBtn("pin_tilted", "#aaaaaa", "置顶", "qw_stay_on_top");
    btnPin->setCheckable(true);
    btnPin->setObjectName("btnPin");
    if (windowFlags() & Qt::WindowStaysOnTopHint) {
        btnPin->setChecked(true);
        btnPin->setIcon(IconHelper::getIcon("pin_vertical", "#FF551C"));
    }
    connect(btnPin, &QPushButton::toggled, this, &QuickWindow::toggleStayOnTop);

    QPushButton* btnAdd = createToolBtn("add", "#aaaaaa", "新建数据", "qw_new_idea");
    btnAdd->setObjectName("btnAdd");
    QMenu* addMenu = new QMenu(this);
    IconHelper::setupMenu(addMenu);
    addMenu->addAction(IconHelper::getIcon("add", "#aaaaaa", 18), "新建数据", [this](){ this->doNewIdea(); });
    QMenu* createByLineMenu = addMenu->addMenu(IconHelper::getIcon("list_ul", "#aaaaaa", 18), "按行创建数据");
    createByLineMenu->addAction("从复制的内容创建", [this](){ this->doCreateByLine(true); });
    createByLineMenu->addAction("从选中数据创建", [this](){ this->doCreateByLine(false); });
    btnAdd->setMenu(addMenu);
    connect(btnAdd, &QPushButton::clicked, [btnAdd](){ btnAdd->showMenu(); });

    QPushButton* btnMainWindow = createToolBtn("window", "#aaaaaa", "打开主界面");
    btnMainWindow->setObjectName("btnMainWindow");
    connect(btnMainWindow, &QPushButton::clicked, this, [this](){ emit toggleMainWindowRequested(); });

    QPushButton* btnSidebar = createToolBtn("eye", "#41F2F2", "显示/隐藏侧边栏", "qw_sidebar");
    btnSidebar->setObjectName("btnSidebar");
    btnSidebar->setCheckable(true);
    btnSidebar->setChecked(true);
    connect(btnSidebar, &QPushButton::clicked, this, &QuickWindow::toggleSidebar);

    QPushButton* btnFilter = createToolBtn("filter", "#f1c40f", "显示/隐藏高级筛选", "qw_filter");
    btnFilter->setObjectName("btnFilter");
    btnFilter->setCheckable(true);
    btnFilter->setChecked(false);
    connect(btnFilter, &QPushButton::clicked, this, &QuickWindow::toggleFilter);

    QPushButton* btnRefresh = createToolBtn("refresh", "#aaaaaa", "刷新", "qw_refresh");
    btnRefresh->setObjectName("btnRefresh");
    connect(btnRefresh, &QPushButton::clicked, this, &QuickWindow::refreshData);

    m_btnToggleAll = createToolBtn("sidebar_open_filled", "#3A90FF", "联动显示/隐藏面板", "qw_toggle_all_panels");
    m_btnToggleAll->setObjectName("btnToggleAll");
    m_btnToggleAll->setCheckable(true);
    connect(m_btnToggleAll, &QPushButton::clicked, this, &QuickWindow::toggleAllPanels);

    m_btnAutoCat = createToolBtn("switch_on", "#aaaaaa", "剪贴板自动归档到当前分类");
    m_btnAutoCat->setObjectName("btnAutoCat");
    m_btnAutoCat->setCheckable(true);
    m_btnAutoCat->setChecked(DatabaseManager::instance().isAutoCategorizeEnabled());
    updateAutoCategorizeButton();
    connect(m_btnAutoCat, &QPushButton::clicked, this, [this](bool checked){
        auto& db = DatabaseManager::instance();
        db.setAutoCategorizeEnabled(checked);
        if (checked) {
            int catId = db.activeCategoryId();
            QString catName = (catId > 0) ? db.getCategoryNameById(catId) : "全部数据";
            ToolTipOverlay::instance()->showText(QCursor::pos(), QString("<b style='color: #00A650;'>[OK] 自动归档已开启 （%1）</b>").arg(catName));
        } else {
            ToolTipOverlay::instance()->showText(QCursor::pos(), "<b style='color: #aaaaaa;'>[OFF] 自动归档已关闭</b>");
        }
    });

    QPushButton* btnLock = createToolBtn("lock_secure", "#aaaaaa", "锁定应用", "qw_lock_app");
    btnLock->setObjectName("btnLock");
    connect(btnLock, &QPushButton::clicked, this, &QuickWindow::doGlobalLock);

    toolLayout->addWidget(btnClose, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(m_btnMax, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnMin, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnPin, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnAdd, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnMainWindow, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnSidebar, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnFilter, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(m_btnToggleAll, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnRefresh, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(m_btnAutoCat, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnLock, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);

    m_btnNavDir = createToolBtn("nav_first", "#aaaaaa", "切换布局方向");
    m_btnNavDir->setObjectName("btnNavDir");
    connect(m_btnNavDir, &QPushButton::clicked, this, &QuickWindow::toggleLayoutDirection);
    toolLayout->addWidget(m_btnNavDir, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addStretch();

    QPushButton* btnPrev = createToolBtn("nav_prev", "#aaaaaa", "上一页", "qw_prev_page", 90);
    btnPrev->setObjectName("btnPrev");
    btnPrev->setFixedSize(28, 20);
    connect(btnPrev, &QPushButton::clicked, [this](){
        if (m_currentPage > 1) { m_currentPage--; refreshData(); }
    });

    m_pageInput = new QLineEdit("1");
    m_pageInput->setObjectName("pageInput");
    m_pageInput->setAlignment(Qt::AlignCenter);
    m_pageInput->setFixedSize(24, 20);
    m_pageInput->installEventFilter(this);
    connect(m_pageInput, &QLineEdit::returnPressed, [this](){
        int p = m_pageInput->text().toInt();
        if (p > 0 && p <= m_totalPages) { m_currentPage = p; refreshData(); }
    });

    QLabel* totalLabel = new QLabel("1");
    totalLabel->setObjectName("totalLabel");
    totalLabel->setAlignment(Qt::AlignCenter);

    QPushButton* btnNext = createToolBtn("nav_next", "#aaaaaa", "下一页", "qw_next_page", 90);
    btnNext->setObjectName("btnNext");
    btnNext->setFixedSize(28, 20);
    connect(btnNext, &QPushButton::clicked, [this](){
        if (m_currentPage < m_totalPages) { m_currentPage++; refreshData(); }
    });

    toolLayout->addWidget(btnPrev, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(6);
    toolLayout->addWidget(m_pageInput, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(6);
    toolLayout->addWidget(totalLabel, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(6);
    toolLayout->addWidget(btnNext, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(20);

    QLabel* verticalTitle = new QLabel("懒\n人\n笔\n记");
    verticalTitle->setAlignment(Qt::AlignCenter);
    verticalTitle->setObjectName("QuickWindow_verticalTitle_17");
    toolLayout->addWidget(verticalTitle, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(12);

    QPushButton* btnLogo = createToolBtn("zap", "#3A90FF", "RapidNotes");
    btnLogo->setFixedSize(28, 28);
    btnLogo->setCursor(Qt::ArrowCursor);
    toolLayout->addWidget(btnLogo, 0, Qt::AlignHCenter);

    containerLayout->addWidget(customToolbar);
    m_mainLayout->addWidget(container);

    QTimer::singleShot(500, []() {
        QuickPreview::instance();
    });

    setMinimumSize(400, 700);

    auto* preview = QuickPreview::instance();
    connect(preview, &QuickPreview::editRequested, this, [this](int id){
        auto* prev = QuickPreview::instance();
        if (!prev->caller() || prev->caller()->window() != this) return;
        this->doEditNote(id);
    });

    m_listView->installEventFilter(this);
    if (m_libraryPanel && m_libraryPanel->treeView()) {
        m_libraryPanel->treeView()->installEventFilter(this);
    }
    m_searchEdit->installEventFilter(this);
    m_catSearchEdit->installEventFilter(this);
    m_tagEdit->installEventFilter(this);

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    connect(m_searchTimer, &QTimer::timeout, this, &QuickWindow::refreshData);
    connect(m_searchEdit, &QLineEdit::textChanged, [this](const QString& text){
        Q_UNUSED(text);
        m_currentPage = 1;
        m_searchTimer->start(300);
    });

    connect(m_searchEdit, &QLineEdit::returnPressed, [this](){
        QString text = m_searchEdit->text().trimmed();
        if (text.isEmpty()) return;
        m_searchEdit->addHistoryEntry(text);
        m_searchTimer->stop();
        refreshData();
    });

    connect(m_listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](){
        auto selected = m_listView->selectionModel()->selectedIndexes();
        if (selected.isEmpty()) {
            m_bottomStackedWidget->setCurrentIndex(0);
            m_tagEdit->setEnabled(false);
        } else {
            if (m_listView->hasFocus()) {
                m_bottomStackedWidget->setCurrentIndex(1);
            }
            m_tagEdit->setEnabled(true);
            m_tagEdit->setPlaceholderText(selected.size() == 1 ? "标签... (双击显示历史)" : "批量添加标签... (双击显示历史)");
            auto* p = QuickPreview::instance();
            if (p->isVisible()) {
                updatePreviewContent();
            }
        }
    });

    setupShortcuts();
    connect(&ShortcutManager::instance(), &ShortcutManager::shortcutsChanged, this, &QuickWindow::updateShortcuts);
    updateAppLockStatus();
    restoreState();

    updateLayoutWidth();
    updateToggleAllIcon();
    refreshData();
    applyListTheme("");
    updateShortcuts();
    setupAppLock();

    m_idleLockTimer = new QTimer(this);
    m_idleLockTimer->setInterval(5000);
    connect(m_idleLockTimer, &QTimer::timeout, this, &QuickWindow::checkIdleLock);
    m_idleLockTimer->start();
}
