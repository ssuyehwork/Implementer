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
#include "components/PanelTitleBar.h"
#include "QuickPreview.h"
#include "PasswordVerifyDialog.h"
#include "CategoryPasswordDialog.h"
#include "FilterPanel.h"
#include "Pendingtasks.h"
#include "MetaPanel.h"
#include "../core/TodoRepository.h"
#include "NoteFilterBridge.h"
#include "NoteFilterStatsLoader.h"
#include "NoteEditWindow.h"
#include "StringUtils.h"
#include "TitleEditorDialog.h"
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
    m_splitter->setObjectName("QuickWindow_m_splitter");
    m_splitter->setHandleWidth(kSplitterHandleWidth);
    m_splitter->setChildrenCollapsible(false);

    // 列表容器：强制绑定 [320px, 500px] SSOT 尺寸约束
    m_listWrapper = new QFrame();
    m_listWrapper->setMinimumWidth(kListMinWidth);
    m_listWrapper->setMaximumWidth(kListMaxWidth);
    m_listWrapper->setObjectName("ListContainer");
    auto* listWrapperLayout = new QVBoxLayout(m_listWrapper);
    listWrapperLayout->setContentsMargins(0, 0, 0, 0);
    listWrapperLayout->setSpacing(0);

    auto* listFocusLine = new QWidget();
    listFocusLine->setFixedHeight(1);
    listFocusLine->setObjectName("QuickWindow_m_listFocusLine_5");
    listFocusLine->hide();
    listWrapperLayout->addWidget(listFocusLine);

    m_listTitleBar = new RapidNotes::PanelTitleBar("最近访问", "list_ul", m_listWrapper);
    listWrapperLayout->addWidget(m_listTitleBar);

    m_listStack = new QStackedWidget();
    m_listStack->setMinimumWidth(95);
    listWrapperLayout->addWidget(m_listStack);
    ToolTipOverlay::instance()->setAnchorWidget(m_listStack);

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

    // 分类侧边栏：强制绑定 [230px, 300px] SSOT 尺寸约束
    m_sidebarWrapper = new QFrame();
    m_sidebarWrapper->setObjectName("SidebarContainer");
    m_sidebarWrapper->setMinimumWidth(kSidePanelMinWidth);
    m_sidebarWrapper->setMaximumWidth(kSidePanelMaxWidth);
    auto* sidebarLayout = new QVBoxLayout(m_sidebarWrapper);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    m_sidebarFocusLine = new QWidget();
    m_sidebarFocusLine->setFixedHeight(1);
    m_sidebarFocusLine->setObjectName("QuickWindow_m_sidebarFocusLine_6");
    m_sidebarFocusLine->hide();
    sidebarLayout->addWidget(m_sidebarFocusLine);

    m_sidebarTitleBar = new RapidNotes::PanelTitleBar("分类", "category", m_sidebarWrapper);
    sidebarLayout->addWidget(m_sidebarTitleBar);

    m_libraryPanel = new RapidNotes::LibraryPanel(this);
    m_libraryPanel->setMaximumWidth(QWIDGETSIZE_MAX);
    if (auto* libTree = m_libraryPanel->treeView()) {
        libTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
        libTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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

    // 筛选器面板：强制绑定 [230px, 300px] SSOT 尺寸约束
    m_filterWrapper = new QFrame();
    m_filterWrapper->setObjectName("FilterWrapper");
    m_filterWrapper->setMinimumWidth(kSidePanelMinWidth);
    m_filterWrapper->setMaximumWidth(kSidePanelMaxWidth);
    auto* fwLayout = new QVBoxLayout(m_filterWrapper);
    fwLayout->setContentsMargins(0, 0, 0, 0);
    fwLayout->setSpacing(0);

    m_filterPanel = new RapidNotes::FilterPanel(this);
    m_filterPanel->setObjectName("QuickWindow_m_filterPanel_9");
    m_filterStatsLoader = new RapidNotes::NoteFilterStatsLoader(this);
    connect(m_filterStatsLoader, &RapidNotes::NoteFilterStatsLoader::statsReady,
            m_filterPanel, &RapidNotes::FilterPanel::populateStats);
    connect(m_filterPanel, &RapidNotes::FilterPanel::filterChanged, this,
            [this](const RapidNotes::FilterState&) { refreshData(); });
    fwLayout->addWidget(m_filterPanel);
    m_filterWrapper->hide();

    // 元数据面板：强制绑定 [230px, 300px] SSOT 尺寸约束
    m_metaWrapper = new QFrame();
    m_metaWrapper->setObjectName("MetadataContainer");
    m_metaWrapper->setMinimumWidth(kSidePanelMinWidth);
    m_metaWrapper->setMaximumWidth(kSidePanelMaxWidth);
    auto* metaLayout = new QVBoxLayout(m_metaWrapper);
    metaLayout->setContentsMargins(0, 0, 0, 0);
    metaLayout->setSpacing(0);
    metaLayout->addWidget(new RapidNotes::PanelTitleBar("元数据", "all_data", m_metaWrapper));
    m_metaPanel = new RapidNotes::MetaPanel(this);
    metaLayout->addWidget(m_metaPanel, 1);
    m_metaWrapper->hide();

    m_metaTimer = new QTimer(this);
    m_metaTimer->setSingleShot(true);
    m_metaTimer->setInterval(80);
    connect(m_metaTimer, &QTimer::timeout, this, &QuickWindow::updateMetaPanel);

    connect(m_metaPanel, &RapidNotes::MetaPanel::ratingChanged, this, [this](const QList<int>& ids, int rating) {
        if (m_mediator) m_mediator->requestSetRating(ids, rating);
    });
    connect(m_metaPanel, &RapidNotes::MetaPanel::colorChanged, this, [this](const QList<int>& ids, const QString& hex) {
        if (m_mediator) m_mediator->requestSetColor(ids, hex);
    });
    connect(m_metaPanel, &RapidNotes::MetaPanel::tagsAddRequested, this, [this](const QList<int>& ids, const QStringList& tags) {
        if (m_mediator) m_mediator->requestAddTags(ids, tags);
    });
    connect(m_metaPanel, &RapidNotes::MetaPanel::tagsRemoveRequested, this, [this](const QList<int>& ids, const QStringList& tags) {
        if (m_mediator) m_mediator->requestRemoveTags(ids, tags);
    });
    connect(m_metaPanel, &RapidNotes::MetaPanel::noteEdited, this, [this](const QList<int>& ids, const QString& note) {
        if (m_mediator) for (int id : ids) m_mediator->requestSetNoteRemark(id, note);
    });
    connect(m_metaPanel, &RapidNotes::MetaPanel::linkEdited, this, [this](const QList<int>& ids, const QString& url) {
        if (m_mediator) for (int id : ids) m_mediator->requestSetNoteURL(id, url);
    });
    connect(m_metaPanel, &RapidNotes::MetaPanel::titleEdited, this, [this](int id, const QString& title) {
        if (m_mediator) m_mediator->requestRenameNote(id, title);
    });

    // 待处理任务面板 (Pendingtasks)：强制绑定 [250px, 300px] SSOT 尺寸约束
    m_pendingWrapper = new QFrame();
    m_pendingWrapper->setObjectName("PendingContainer");
    m_pendingWrapper->setMinimumWidth(kPendingMinWidth);
    m_pendingWrapper->setMaximumWidth(kPendingMaxWidth);
    auto* pendingLayout = new QVBoxLayout(m_pendingWrapper);
    pendingLayout->setContentsMargins(0, 0, 0, 0);
    pendingLayout->setSpacing(0);
    pendingLayout->addWidget(new RapidNotes::PanelTitleBar("Pendingtasks", "list_ul", m_pendingWrapper));
    m_pendingPanel = new RapidNotes::Pendingtasks(this);
    pendingLayout->addWidget(m_pendingPanel, 1);
    m_pendingPanel->deferredInit();
    connect(m_pendingPanel, &RapidNotes::Pendingtasks::noteSelected, this, &QuickWindow::locateNote);
    connect(m_pendingPanel, &RapidNotes::Pendingtasks::previewRequested, this,
            [this](const QVariantMap& note, const QString& stageName) {
        QuickPreview::instance()->showPreview(note, QCursor::pos(), stageName, m_pendingPanel);
    });
    m_pendingWrapper->hide();

    connect(&RapidNotes::TodoRepository::instance(), &RapidNotes::TodoRepository::todoChanged, this, [this]() {
        if (m_currentFilterType == "tasks") scheduleRefresh();
    });

    // 分栏顺序：列表 → 元数据 → 筛选器 → 分类 → 待处理
    m_splitter->addWidget(m_listWrapper);
    m_splitter->addWidget(m_metaWrapper);
    m_splitter->addWidget(m_filterWrapper);
    m_splitter->addWidget(m_sidebarWrapper);
    m_splitter->addWidget(m_pendingWrapper);

    connect(m_splitter, &QSplitter::splitterMoved, this, [this](int pos, int index) {
        Q_UNUSED(pos); Q_UNUSED(index);
        this->updateFocusLines();
        for (int i = 0; i < m_splitter->count(); ++i) {
            QWidget* w = m_splitter->widget(i);
            const int size = m_splitter->sizes().at(i);
            if (w->isHidden()) continue;
            if (w == m_filterWrapper) {
                m_filterWidth = qBound(kSidePanelMinWidth, size, kSidePanelMaxWidth);
            } else if (w == m_sidebarWrapper) {
                m_sidebarWidth = qBound(kSidePanelMinWidth, size, kSidePanelMaxWidth);
            } else if (w == m_pendingWrapper) {
                m_pendingWidth = qBound(kPendingMinWidth, size, kPendingMaxWidth);
            } else if (w == m_metaWrapper) {
                m_metaWidth = qBound(kSidePanelMinWidth, size, kSidePanelMaxWidth);
            }
        }
    });

    for (int i = 0; i < 5; ++i) {
        m_splitter->setCollapsible(i, false);
        m_splitter->setStretchFactor(i, i == 0 ? 1 : 0);
    }
    m_splitter->setSizes({kListMinWidth, kSidePanelDefaultWidth, kSidePanelDefaultWidth, kSidePanelDefaultWidth, kPendingMinWidth});
    leftLayout->addWidget(m_splitter);

    auto* bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(2, 0, 0, 5);
    bottomLayout->setSpacing(10);

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
    customToolbar->setFixedWidth(kToolbarWidth);

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

    m_btnStatusFilter  = createToolBtn("隐藏筛选器", "#aaaaaa", "隐藏筛选器：显示分类栏和列表");
    m_btnStatusNav     = createToolBtn("隐藏目录导航", "#aaaaaa", "隐藏分类栏：显示列表和筛选器（右键切换收起方式）");
    m_btnStatusContent = createToolBtn("内容面板", "#aaaaaa", "内容面板：只显示列表（Tab 沉浸模式）");
    m_btnStatusReset   = createToolBtn("重置分栏", "#aaaaaa", "重置分栏：所有面板全部显示");
    for (QPushButton* btn : {m_btnStatusFilter, m_btnStatusNav, m_btnStatusContent, m_btnStatusReset}) {
        btn->setCheckable(true);
    }
    connect(m_btnStatusFilter, &QPushButton::clicked, this, [this]() { setPanelsVisible(true, false); });
    connect(m_btnStatusNav, &QPushButton::clicked, this, [this]() { setPanelsVisible(false, true); });
    connect(m_btnStatusContent, &QPushButton::clicked, this, [this]() { setPanelsVisible(false, false, 0, 0); });
    connect(m_btnStatusReset, &QPushButton::clicked, this, [this]() {
        m_sidebarWidth = kSidePanelDefaultWidth;
        m_filterWidth  = kSidePanelDefaultWidth;
        m_pendingWidth = kPendingMinWidth;
        m_metaWidth    = kSidePanelDefaultWidth;
        setPanelsVisible(true, true, 1, 1);
    });

    m_btnTogglePending = createToolBtn("隐藏收藏栏", "#aaaaaa", "显示/隐藏待处理任务面板 (Pendingtasks)");
    m_btnTogglePending->setCheckable(true);
    connect(m_btnTogglePending, &QPushButton::clicked, this, &QuickWindow::togglePending);

    m_btnToggleMeta = createToolBtn("隐藏元数据面板", "#aaaaaa", "显示/隐藏元数据面板");
    m_btnToggleMeta->setCheckable(true);
    connect(m_btnToggleMeta, &QPushButton::clicked, this, &QuickWindow::toggleMeta);

    QPushButton* btnRefresh = createToolBtn("refresh", "#aaaaaa", "刷新", "qw_refresh");
    btnRefresh->setObjectName("btnRefresh");
    connect(btnRefresh, &QPushButton::clicked, this, &QuickWindow::refreshData);

    m_btnAutoCat = createToolBtn("switch_off", "#aaaaaa", "剪贴板自动归档到当前分类");
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
    toolLayout->addWidget(btnMin, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnPin, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(btnAdd, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(m_btnStatusFilter, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(m_btnStatusNav, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(m_btnStatusContent, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(m_btnStatusReset, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(m_btnToggleMeta, 0, Qt::AlignHCenter);
    toolLayout->addSpacing(4);
    toolLayout->addWidget(m_btnTogglePending, 0, Qt::AlignHCenter);
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

    QPushButton* btnLogo = createToolBtn("RapidNotes-1.svg", "#3A90FF", "RapidNotes");
    btnLogo->setFixedSize(28, 28);
    btnLogo->setCursor(Qt::ArrowCursor);
    toolLayout->addWidget(btnLogo, 0, Qt::AlignHCenter);

    containerLayout->addWidget(customToolbar);
    m_mainLayout->addWidget(container);

    QTimer::singleShot(500, []() {
        QuickPreview::instance();
    });

    setMinimumHeight(700);

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
        scheduleMetaUpdate();
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
    updateStatusBarButtonHighlights();
    refreshData();
    applyListTheme("");
    updateShortcuts();
    setupAppLock();

    m_idleLockTimer = new QTimer(this);
    m_idleLockTimer->setInterval(5000);
    connect(m_idleLockTimer, &QTimer::timeout, this, &QuickWindow::checkIdleLock);
    m_idleLockTimer->start();
}