#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "ContentPanel.h"
#include "AlphabetIndexBar.h"
#include "EmptyStateOverlay.h"
#include <QtConcurrent/QtConcurrent>
#include "models/SectionProxyModel.h"
#include "ContentHeaderWidget.h"
#include "controllers/ContentContextMenu.h"
#include "controllers/ContentKeyHandler.h"
#include "controllers/ContentSortController.h"
#include "controllers/ContentDataLoader.h"
#include "controllers/ContentFileOpsHandler.h"
#include "controllers/ContentViewCoordinator.h"
#include "controllers/ContentPaneSplitManager.h"
#include "controllers/ContentChangeWatcher.h"
#include "workers/ContentStatsWorker.h"
#include "DropJustifiedView.h"
#include "DropTreeView.h"
#include "DropListView.h"
#include "ColumnViewWidget.h"
#include "ThumbnailDelegate.h"
#include "TreeItemDelegate.h"
#include "CardLayoutEngine.h"
#include "UiHelper.h"
#include "ToolTipOverlay.h"
#include "../meta/LibraryService.h"
#include "../meta/LibrarySyncService.h"

#include "../core/AppConfig.h"
#include "../core/CoreEngine.h"
#include "Logger.h"
#include <QElapsedTimer>
#include "../core/CoreController.h"
#include "../core/TrashService.h"
#include "../core/PermanentDeleteService.h"
#include "../core/ClipboardService.h"
#include "../meta/MediaExtractorPipeline.h"
#include "../util/ThumbnailPipelineService.h"
#include "../core/NavigationService.h"
#include "../core/NavigationHistoryService.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QHeaderView>
#include <QScrollBar>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QSignalBlocker>
#include <QStyle>
#include <QDebug>

namespace QuarkMeta {

namespace {

// 将面板子控件上的 Ctrl+V 统一路由到面板快捷键处理器。
// 输入框会由 ContentKeyHandler 放行，继续执行原生文本粘贴。
class PanelClipboardShortcutFilter final : public QObject {
public:
    explicit PanelClipboardShortcutFilter(ContentPanel* panel)
        : QObject(panel), m_panel(panel) {
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (!m_panel || !event || event->type() != QEvent::KeyPress) {
            return false;
        }

        auto* widget = qobject_cast<QWidget*>(watched);
        if (!widget) {
            return false;
        }

        QWidget* header = m_panel->headerWidget();
        QWidget* viewStack = m_panel->viewStack();
        const bool belongsToPanel =
            (header && (widget == header || header->isAncestorOf(widget))) ||
            (viewStack && (widget == viewStack || viewStack->isAncestorOf(widget)));
        if (!belongsToPanel) {
            return false;
        }

        auto* keyEvent = static_cast<QKeyEvent*>(event);
        const Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
        if (keyEvent->key() != Qt::Key_V ||
            !(modifiers & Qt::ControlModifier) ||
            (modifiers & Qt::ShiftModifier)) {
            return false;
        }

        return m_panel->keyHandler() &&
               m_panel->keyHandler()->handleEvent(watched, event);
    }

private:
    ContentPanel* m_panel = nullptr;
};

} // namespace

QTreeView* ContentPanel::treeView() const {
    return static_cast<QTreeView*>(m_treeView);
}

ContentPanel::ContentPanel(QWidget* parent) : QFrame(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setObjectName("EditorContainer");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(kMinPaneWidth);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_diskModel = new DiskItemModel(this);
    m_model = m_diskModel;
    m_model->setCurrentPath(m_currentPath);

    // 统计重算防抖定时器 (50ms)：兼顾实时响应与批量修改时的去噪
    m_statsDebounceTimer = new QTimer(this);
    m_statsDebounceTimer->setSingleShot(true);
    m_statsDebounceTimer->setInterval(50);
    connect(m_statsDebounceTimer, &QTimer::timeout, this, &ContentPanel::recalculateAndEmitStats);

    // 核心架构闭环：监听底层模型元数据变更，自动防抖驱动统计重算、右侧索引栏刷新与筛选器同步
    connect(m_diskModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex&, const QModelIndex&, const QList<int>& roles) {
        if (roles.isEmpty() ||
            roles.contains(Qt::DisplayRole) ||
            roles.contains(Qt::EditRole) ||
            roles.contains(RatingRole) ||
            roles.contains(ColorRole) ||
            roles.contains(TagsRole) ||
            roles.contains(NoteRole) ||
            roles.contains(UrlRole) ||
            roles.contains(PinnedRole) ||
            roles.contains(IsLockedRole) ||
            roles.contains(HasThumbnailRole)) {
            updateAlphabetIndexBar();
            if (m_statsDebounceTimer) {
                m_statsDebounceTimer->start();
            }
        }
    });

    // 监听重命名完成信号
    connect(m_diskModel, &DiskItemModel::itemRenamed, this,
            [this](const QString& /*oldPath*/, const QString& newPath) {
                setPendingSelectPath(newPath, false);
                setPendingSelectPaths({newPath});
                QMetaObject::invokeMethod(this, [this]() {
                    refreshAll();
                }, Qt::QueuedConnection);
            });

    m_visibleTimer = new QTimer(this);
    m_visibleTimer->setSingleShot(true);
    m_visibleTimer->setInterval(60);
    connect(m_visibleTimer, &QTimer::timeout, this, &ContentPanel::refreshVisibleThumbnails);

    m_sortController = new ContentSortController(this);
    connect(m_sortController, &ContentSortController::sortCriteriaChanged, this, [this](SortType type, Qt::SortOrder order) {
        applySort();
        updateAlphabetIndexBar();
        if (m_treeView && m_treeView->header()) {
            FileListColumn colEnum = ContentSortController::columnForSortType(type);
            m_treeView->header()->setSortIndicator(static_cast<int>(colEnum), order);
        }
    });

    m_dataLoader = new ContentDataLoader(this);
    m_fileOpsHandler = new ContentFileOpsHandler(this);
    m_statsWorker = new ContentStatsWorker(this);
    m_splitManager = new ContentPaneSplitManager(this);
    m_viewCoordinator = new ContentViewCoordinator(this);
    m_changeWatcher = new ContentChangeWatcher(this);
    setAcceptDrops(true);

    connect(m_statsWorker, &ContentStatsWorker::statsReady, this, [this](const ScanStats& stats) {
        if (m_proxyModel) {
            m_proxyModel->setCachedDuplicatePaths(stats.duplicatePaths);
        }
        emit directoryStatsReady(stats);
    });

    m_zoomLevel = AppConfig::instance().getValue("UI/GridZoomLevel", 96).toInt();
    m_currentFilter.showHidden = AppConfig::instance().getValue("ContentPanel/ShowHidden", false).toBool();

    connect(&TrashService::instance(), &TrashService::trashOperationCompleted, this, &ContentPanel::refreshAll);
    connect(&PermanentDeleteService::instance(), &PermanentDeleteService::permanentDeleteCompleted, this, &ContentPanel::refreshAll);
    connect(&ClipboardService::instance(), &ClipboardService::pasteCompleted, this, [this](const QString& dir) {
        if (m_isRecursive || m_currentPath == dir) refreshAll();
    });
    connect(&LibraryService::instance(), &LibraryService::libraryChanged, this, &ContentPanel::refreshAll);

    m_keyHandler = new ContentKeyHandler(this);

    connect(this, &ContentPanel::closePaneRequested, this, [this]() {
        closePane(this);
    });

    initUi();
    updateGridSize();

    int savedMode = AppConfig::instance().getValue("ContentPanel/ViewMode", static_cast<int>(GridView)).toInt();
    setViewMode(static_cast<ViewMode>(savedMode));

    if (qApp) {
        qApp->installEventFilter(new PanelClipboardShortcutFilter(this));
    }
}

void ContentPanel::initUi() {
    // ── 顶部 Header 区域（顶格停靠，绝不产生纵向空洞） ──
    m_headerWidget = new ContentHeaderWidget(this);
    m_headerWidget->setFilterState(m_currentFilter);

    connect(m_headerWidget, &ContentHeaderWidget::splitViewRequested, this, [this]() {
        if (isSplitMode()) {
            if (isSecondaryPane()) {
                emit closePaneRequested();
            } else {
                ContentPanel* secPanel = secondaryContentPanel();
                if (secPanel) {
                    QString targetPath = secPanel->currentPath();
                    ViewMode targetMode = secPanel->currentViewMode();
                    bool targetRecursive = secPanel->isRecursive();
                    int targetZoom = secPanel->zoomLevel();

                    setViewMode(targetMode);
                    setIsRecursive(targetRecursive);
                    setZoomLevel(targetZoom);
                    loadDirectory(targetPath, targetRecursive);

                    if (m_splitManager) {
                        m_splitManager->closePane(secPanel);
                    }
                } else {
                    closeSecondaryPane();
                }
            }
            return;
        }

        QStringList history = NavigationHistoryService::instance().getHistory();
        QString lastPath;
        for (const QString& hPath : history) {
            if (!hPath.isEmpty() && QDir::cleanPath(hPath) != QDir::cleanPath(m_currentPath)) {
                lastPath = hPath;
                break;
            }
        }
        if (lastPath.isEmpty()) {
            lastPath = m_currentPath;
        }
        splitPane(Qt::Horizontal, lastPath);
    });

    connect(m_headerWidget, &ContentHeaderWidget::filterStateChanged, this, [this](const FilterState& state) {
        m_currentFilter = state;
        AppConfig::instance().setValue("ContentPanel/ShowHidden", state.showHidden);
        applyFilters();
    });

    connect(m_headerWidget, &ContentHeaderWidget::recursiveToggled, this, [this](bool recursive) {
        if (m_currentPath.isEmpty() || m_currentPath == "computer://") {
            if (m_headerWidget) m_headerWidget->setRecursive(false);
            return;
        }
        m_isRecursive = recursive;
        NavigationService::instance().setCurrentRecursive(recursive);
        if (m_currentViewMode == ColumnView) {
            if (m_columnView && m_columnView->rightmostPane()) {
                m_columnView->rightmostPane()->loadDirectory();
            }
        } else {
            loadDirectory(m_currentPath, recursive);
        }
    });

    m_mainLayout->addWidget(m_headerWidget);

    connect(&ClipboardService::instance(), &ClipboardService::cutItemsChanged, this, [this]() {
        if (m_treeView && m_treeView->viewport()) m_treeView->viewport()->update();
        if (m_gridView && m_gridView->viewport()) m_gridView->viewport()->update();
    });

    connect(&ClipboardService::instance(), &ClipboardService::pasteCompletedWithTargets, this,
            [this](const QString& targetDir, const QStringList& targetPaths) {
        if (m_isRecursive || QDir::cleanPath(targetDir).compare(QDir::cleanPath(m_currentPath), Qt::CaseInsensitive) == 0) {
            setPendingSelectPaths(targetPaths);
            refreshAll();
        }
    });

    // 🚀【纯净纵向流】：m_viewStack 直接顶格满额接入 m_mainLayout，坚决不嵌套任何导致高度挤压的额外中间层
    m_viewStack = new QStackedWidget(this);
    m_viewStack->setFrameShape(QFrame::NoFrame);
    m_viewStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    initEmptyFilterOverlay();

    m_columnView = new ColumnViewWidget(this, this);
    connect(m_columnView, &ColumnViewWidget::selectionChanged, this, &ContentPanel::onSelectionChanged);
    connect(m_columnView, &ColumnViewWidget::activeColumnRecordsChanged, this, [this](const std::vector<QuarkMeta::ItemRecord>& records) {
        if (m_statsWorker && !records.empty()) {
            m_statsWorker->processAsync(std::move(records), m_currentFilter.showHidden);
        }
    });

    m_proxyModel = new FilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterKeyColumn(0);
    m_proxyModel->setDynamicSortFilter(true);
    m_proxyModel->currentFilter = m_currentFilter;

    m_sectionModel = new SectionProxyModel(this);
    m_sectionModel->setSourceModel(m_proxyModel);

    initGridView();
    initListView();

    if (m_gridView) {
        m_viewStack->addWidget(m_gridView);
    }
    if (m_treeView) {
        m_viewStack->addWidget(m_treeView);
    }
    m_viewStack->addWidget(m_columnView);
    if (m_gridView) {
        m_viewStack->setCurrentWidget(m_gridView);
    }

    m_mainLayout->addWidget(m_viewStack, 1);

    initAlphabetIndexBar();
}

void ContentPanel::initAlphabetIndexBar() {
    if (!m_alphabetIndexController) {
        m_alphabetIndexController = new AlphabetIndexBar(this);
    }
    m_alphabetIndexController->setRightMargin(AlphabetIndexBar::kSpacingPx);
    m_alphabetIndexController->initialize();
}

void ContentPanel::updateAlphabetIndexBarGeometry() {
    if (m_alphabetIndexController) {
        m_alphabetIndexController->updateGeometry();
    }
}

void ContentPanel::updateAlphabetIndexBar() {
    if (m_alphabetIndexController) {
        m_alphabetIndexController->update();
    }
}

void ContentPanel::initGridView() {
    auto* jv = new DropJustifiedView(this);
    m_gridView = jv;
    jv->setSectionModel(m_sectionModel);
    jv->setFrameShape(QFrame::NoFrame);
    jv->setSelectionMode(QAbstractItemView::ExtendedSelection);
    jv->setContextMenuPolicy(Qt::CustomContextMenu);
    jv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    jv->setModel(m_sectionModel);

    jv->setAspectRatioRole(AspectRatioRole);
    auto* delegate = new ThumbnailDelegate(this);
    delegate->setHasThumbnailRole(HasThumbnailRole);
    delegate->setRatingRole(RatingRole);
    delegate->setPathRole(PathRole);
    delegate->setPinnedRole(PinnedRole);
    delegate->setTypeRole(TypeRole);
    delegate->setIsEmptyRole(IsEmptyRole);
    delegate->setColorRole(ColorRole);
    m_gridView->setItemDelegate(delegate);

    jv->installEventFilter(this);
    jv->viewport()->installEventFilter(this);

    connect(jv, &QAbstractItemView::doubleClicked, this, &ContentPanel::activateIndex);
    connect(jv->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ContentPanel::onSelectionChanged);
    connect(jv, &QAbstractItemView::customContextMenuRequested, this, [this](const QPoint& pos) {
        onCustomContextMenuRequested(m_gridView, pos);
    });
    connect(jv, &JustifiedView::pathsDropped, this, [this](const QStringList& paths, const QModelIndex& targetIndex, Qt::DropAction action) {
        onPathsDropped(paths, targetIndex, currentPath(), action);
    });

    if (jv->verticalScrollBar()) {
        connect(jv->verticalScrollBar(), &QScrollBar::valueChanged, this, &ContentPanel::startVisibleTimer);
    }
    connect(jv, &JustifiedView::layoutFinished, this, [this, jv]() {
        updateAlphabetIndexBar();
        startVisibleTimer();
        QModelIndex curIdx = jv->currentIndex();
        if (curIdx.isValid() && jv->selectionModel() && jv->selectionModel()->isSelected(curIdx)) {
            jv->scrollTo(curIdx, QAbstractItemView::EnsureVisible);
        }
    });

    if (m_sectionModel) {
        connect(m_sectionModel, &QAbstractItemModel::modelReset, this, [this]() {
            updateAlphabetIndexBar();
            startVisibleTimer();
        });
        connect(m_sectionModel, &QAbstractItemModel::layoutChanged, this, [this]() {
            updateAlphabetIndexBar();
            startVisibleTimer();
        });
        connect(m_sectionModel, &QAbstractItemModel::rowsInserted, this, [this]() {
            updateAlphabetIndexBar();
            startVisibleTimer();
        });
        connect(m_sectionModel, &QAbstractItemModel::rowsRemoved, this, [this]() {
            updateAlphabetIndexBar();
            startVisibleTimer();
        });
    }
}

void ContentPanel::initListView() {
    auto* tree = new DropTreeView(this);
    m_treeView = tree;
    tree->setFrameShape(QFrame::NoFrame);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setModel(m_sectionModel);
    tree->setAlternatingRowColors(true);
    tree->setRootIsDecorated(false);
    tree->setSortingEnabled(false);
    tree->header()->setSectionsClickable(true);
    tree->header()->setFixedHeight(32);
    tree->header()->setMinimumSectionSize(0);
    tree->applyColumnPolicies();
    tree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto* delegate = new TreeItemDelegate(this, true, true);
    tree->setItemDelegate(delegate);

    tree->installEventFilter(this);
    tree->viewport()->installEventFilter(this);

    connect(tree, &QAbstractItemView::doubleClicked, this, &ContentPanel::activateIndex);
    connect(tree->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ContentPanel::onSelectionChanged);
    connect(tree, &QAbstractItemView::customContextMenuRequested, this, [this](const QPoint& pos) {
        onCustomContextMenuRequested(m_treeView, pos);
    });
    connect(tree, &DropTreeView::pathsDropped, this, [this](const QStringList& paths, const QModelIndex& targetIndex, Qt::DropAction action) {
        onPathsDropped(paths, targetIndex, currentPath(), action);
    });

    if (tree->verticalScrollBar()) {
        connect(tree->verticalScrollBar(), &QScrollBar::valueChanged, this, &ContentPanel::startVisibleTimer);
    }
    connect(tree->header(), &QHeaderView::sectionClicked, this, [this](int logicalIndex) {
        if (logicalIndex == static_cast<int>(FileListColumn::Status)) return;
        FileListColumn col = static_cast<FileListColumn>(logicalIndex);
        SortType newType = ContentSortController::sortTypeForColumn(col);
        if (static_cast<int>(newType) < 0) return;

        if (currentSortType() == newType) {
            setSortOrder(currentSortOrder() == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder);
        } else {
            setSortCriteria(newType, Qt::AscendingOrder);
        }
    });

    if (m_sortController && tree->header()) {
        FileListColumn initCol = ContentSortController::columnForSortType(m_sortController->sortType());
        tree->header()->setSortIndicator(static_cast<int>(initCol), m_sortController->sortOrder());
    }
}

bool ContentPanel::eventFilter(QObject* obj, QEvent* event) {
    if (!event) return QFrame::eventFilter(obj, event);

    QWidget* watchedWidget = qobject_cast<QWidget*>(obj);
    if (event->type() == QEvent::ContextMenu &&
        m_emptyFilterOverlay &&
        m_emptyFilterOverlay->isVisible() &&
        watchedWidget &&
        (watchedWidget == m_emptyFilterOverlay || m_emptyFilterOverlay->isAncestorOf(watchedWidget))) {
        auto* contextEvent = static_cast<QContextMenuEvent*>(event);
        QAbstractItemView* view = activeItemView();
        if (view && view->viewport()) {
            setActivePane(true);
            onCustomContextMenuRequested(
                view,
                view->viewport()->mapFromGlobal(contextEvent->globalPos()));
        }
        contextEvent->accept();
        return true;
    }

    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn) {
        setActivePane(true);
    }

    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent && mouseEvent->button() == Qt::LeftButton) {
            QAbstractItemView* view = nullptr;
            if (m_gridView && (obj == m_gridView || obj == m_gridView->viewport())) {
                view = m_gridView;
            } else if (m_treeView && (obj == m_treeView || obj == m_treeView->viewport())) {
                view = m_treeView;
            }
            if (view) {
                QModelIndex idx = view->indexAt(mouseEvent->pos());
                if (!idx.isValid()) {
                    NavigationService::instance().goUp();
                    return true;
                }
                if (idx.data(SectionHeaderRole).toBool()) {
                    return true;
                }
            }
        }
    }

    if (m_keyHandler && m_keyHandler->handleEvent(obj, event)) return true;
    return QFrame::eventFilter(obj, event);
}

void ContentPanel::ensureSourceModelIsDiskModel() {
    if (m_model != m_diskModel) {
        m_model = m_diskModel;
        m_model->setCurrentPath(m_currentPath);
        if (m_proxyModel) m_proxyModel->setSourceModel(m_model);
    }
}

void ContentPanel::applySort() {
    if (m_sortController) {
        QElapsedTimer timer;
        timer.start();
        if (m_proxyModel) m_sortController->applySortToModel(m_proxyModel);
        if (m_columnView) {
            m_columnView->applySort(static_cast<int>(m_sortController->sortType()), m_sortController->sortOrder());
        }
        updateAlphabetIndexBar();
        qint64 ms = timer.elapsed();
        if (ms > 5) {
            Logger::log(QString("[Perf-ContentPanel] applySort 耗时=%1ms, viewMode=%2").arg(ms).arg(m_currentViewMode));
        }
    }
}

void ContentPanel::setLoading(bool loading) {
    bool wasLoading = m_isLoading.exchange(loading);
    if (wasLoading && !loading) {
        updateAlphabetIndexBar();
        if (m_changeWatcher) {
            m_changeWatcher->onLoadFinished();
        }
    }
}

void ContentPanel::notifyRecordsChanged() {
    updateAlphabetIndexBar();
    startVisibleTimer();
    recalculateAndEmitStats();
    updateStatusBarStats();
}

void ContentPanel::setCurrentCategoryType(const QString& type) {
    m_currentCategoryType = type;
    if (m_changeWatcher) {
        m_changeWatcher->updateTarget();
    }
}

void ContentPanel::setIsRecursive(bool recursive) {
    m_isRecursive = recursive;
    NavigationService::instance().setCurrentRecursive(recursive);
    if (m_headerWidget) {
        m_headerWidget->setRecursive(recursive);
    }
    if (m_changeWatcher) {
        m_changeWatcher->updateTarget();
    }
}

void ContentPanel::incrementModelGeneration() {
    if (m_diskModel) m_diskModel->incrementGeneration();
}

void ContentPanel::reloadThumbnailForPath(const QString& path) {
    if (m_diskModel) m_diskModel->reloadThumbnailForPath(path);
}

bool ContentPanel::isTreeView(QObject* view) const {
    return (view == m_treeView);
}

void ContentPanel::startVisibleTimer() {
    if (m_visibleTimer) {
        m_visibleTimer->start();
    } else {
        refreshVisibleThumbnails();
    }
}

void ContentPanel::onCustomContextMenuRequested(const QPoint& pos) {
    QAbstractItemView* view = qobject_cast<QAbstractItemView*>(sender());
    if (!view) view = activeItemView();
    if (!view) return;
    ContentContextMenu menuHandler(this);
    menuHandler.showMenu(view, pos);
}

void ContentPanel::onCustomContextMenuRequested(QAbstractItemView* view, const QPoint& pos) {
    if (!view) view = activeItemView();
    if (!view) return;
    ContentContextMenu menuHandler(this);
    menuHandler.showMenu(view, pos);
}

bool ContentPanel::isSplitMode() const {
    ContentPanel* root = rootPane();
    if (root && root != this) return root->isSplitMode();
    return m_splitManager ? m_splitManager->isSplitMode() : false;
}

Qt::Orientation ContentPanel::splitOrientation() const {
    ContentPanel* root = rootPane();
    if (root && root != this) return root->splitOrientation();
    return m_splitManager ? m_splitManager->splitOrientation() : Qt::Horizontal;
}

bool ContentPanel::isSecondaryPane() const {
    return m_splitManager ? m_splitManager->isSecondaryPane() : false;
}

void ContentPanel::setIsSecondaryPane(bool secondary) {
    if (m_splitManager) m_splitManager->setIsSecondaryPane(secondary);
}

ContentPanel* ContentPanel::secondaryContentPanel() const {
    return m_splitManager ? m_splitManager->secondaryContentPanel() : nullptr;
}

QList<ContentPanel*> ContentPanel::panes() const {
    ContentPanel* root = rootPane();
    if (root && root != this) return root->panes();
    return m_splitManager ? m_splitManager->panes() : QList<ContentPanel*>{const_cast<ContentPanel*>(this)};
}

int ContentPanel::paneCount() const {
    ContentPanel* root = rootPane();
    if (root && root != this) return root->paneCount();
    return m_splitManager ? m_splitManager->paneCount() : 1;
}

ContentPanel* ContentPanel::rootPane() const {
    return m_splitManager ? m_splitManager->rootPane() : const_cast<ContentPanel*>(this);
}

void ContentPanel::splitPane(Qt::Orientation orientation, const QString& secondaryPath, bool insertBefore) {
    if (m_splitManager) m_splitManager->splitPane(orientation, secondaryPath, insertBefore);
}

void ContentPanel::closePane(ContentPanel* pane) {
    if (m_splitManager) m_splitManager->closePane(pane);
}

void ContentPanel::closeSecondaryPane() {
    if (m_splitManager) m_splitManager->closeSecondaryPane();
}

void ContentPanel::requestClosePane() {
    emit closePaneRequested();
}

void ContentPanel::setActivePane(bool active) {
    if (m_splitManager) m_splitManager->setActivePane(active);
    if (active) {
        QAbstractItemView* curView = activeItemView();
        if (curView && !curView->hasFocus()) {
            curView->setFocus(Qt::MouseFocusReason);
        }
        emit panelActivated(this);
    }
}

QString ContentPanel::activePath() const {
    if (m_currentViewMode == ColumnView && m_columnView) {
        ColumnViewPane* pane = m_columnView->activePane();
        if (pane && !pane->currentPath().isEmpty()) {
            return pane->currentPath();
        }
    }
    return m_currentPath;
}

QString ContentPanel::pathForView(QAbstractItemView* view) const {
    if (m_currentViewMode == ColumnView && m_columnView && view) {
        ColumnViewPane* pane = m_columnView->findPaneForView(view);
        if (pane && !pane->currentPath().isEmpty()) {
            return pane->currentPath();
        }
    }
    return activePath();
}

void ContentPanel::redistributePaneSizes() {
    if (m_splitManager) m_splitManager->redistributePaneSizes();
}

void ContentPanel::updateDragOverlay(const QPoint& pos) {
    if (m_splitManager) m_splitManager->updateDragOverlayGlobal(mapToGlobal(pos));
}

void ContentPanel::hideDragOverlay() {
    if (m_splitManager) m_splitManager->hideDragOverlay();
}

void ContentPanel::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()) {
        if (event->mimeData()->hasFormat("application/x-quarkmeta-panemove")) {
            event->acceptProposedAction();
            return;
        }
        if (event->mimeData()->hasFormat("application/x-quarkmeta-taburl")) {
            if (paneCount() < kMaxPanes) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    QFrame::dragEnterEvent(event);
}

void ContentPanel::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()) {
        if (event->mimeData()->hasFormat("application/x-quarkmeta-panemove")) {
            quintptr ptrVal = event->mimeData()->data("application/x-quarkmeta-panemove").toULongLong();
            ContentPanel* srcPanel = reinterpret_cast<ContentPanel*>(ptrVal);
            QPoint globalPos = mapToGlobal(event->position().toPoint());
            ContentPanel* root = rootPane();
            if (root && root->splitManager()) {
                ContentPaneSplitManager::SplitEvaluationResult eval = root->splitManager()->evaluateDropAtPosition(globalPos, srcPanel);
                if (eval.isValid) {
                    root->splitManager()->updateDragOverlayGlobal(globalPos, srcPanel);
                    event->acceptProposedAction();
                } else {
                    root->splitManager()->hideDragOverlay();
                    event->ignore();
                }
            }
            return;
        }
        if (event->mimeData()->hasFormat("application/x-quarkmeta-taburl")) {
            if (paneCount() < kMaxPanes) {
                QPoint globalPos = mapToGlobal(event->position().toPoint());
                ContentPanel* root = rootPane();
                if (root && root->splitManager()) {
                    ContentPaneSplitManager::SplitEvaluationResult eval = root->splitManager()->evaluateDropAtPosition(globalPos, nullptr);
                    if (eval.isValid) {
                        root->splitManager()->updateDragOverlayGlobal(globalPos, nullptr);
                        event->acceptProposedAction();
                    } else {
                        root->splitManager()->hideDragOverlay();
                        event->ignore();
                    }
                }
                return;
            }
        }
    }
    QFrame::dragMoveEvent(event);
}

void ContentPanel::dragLeaveEvent(QDragLeaveEvent* event) {
    ContentPanel* root = rootPane();
    if (root && root->splitManager()) {
        root->splitManager()->hideDragOverlay();
    }
    QFrame::dragLeaveEvent(event);
}

void ContentPanel::dropEvent(QDropEvent* event) {
    ContentPanel* root = rootPane();
    if (event->mimeData()) {
        if (event->mimeData()->hasFormat("application/x-quarkmeta-panemove")) {
            if (root && root->splitManager()) {
                root->splitManager()->hideDragOverlay();
            }
            quintptr ptrVal = event->mimeData()->data("application/x-quarkmeta-panemove").toULongLong();
            ContentPanel* srcPanel = reinterpret_cast<ContentPanel*>(ptrVal);
            QPoint globalPos = mapToGlobal(event->position().toPoint());
            if (root && root->splitManager()) {
                ContentPaneSplitManager::SplitEvaluationResult eval = root->splitManager()->evaluateDropAtPosition(globalPos, srcPanel);
                if (eval.isValid && eval.targetPanel) {
                    root->splitManager()->movePane(srcPanel, eval.targetPanel, eval.orientation, eval.insertBefore);
                    event->acceptProposedAction();
                    return;
                }
            }
            return;
        }
        if (event->mimeData()->hasFormat("application/x-quarkmeta-taburl")) {
            if (root && root->splitManager()) {
                root->splitManager()->hideDragOverlay();
            }
            if (paneCount() < kMaxPanes) {
                QString tabUrl = QString::fromUtf8(event->mimeData()->data("application/x-quarkmeta-taburl"));
                if (tabUrl.isEmpty()) {
                    tabUrl = event->mimeData()->text();
                }

                if (!tabUrl.isEmpty()) {
                    QPoint globalPos = mapToGlobal(event->position().toPoint());
                    if (root && root->splitManager()) {
                        ContentPaneSplitManager::SplitEvaluationResult eval = root->splitManager()->evaluateDropAtPosition(globalPos, nullptr);
                        if (eval.isValid && eval.targetPanel) {
                            root->splitManager()->splitPaneAtTarget(eval.targetPanel, eval.orientation, tabUrl, eval.insertBefore);
                            event->acceptProposedAction();
                            return;
                        }
                    }
                }
            }
        }
    }
    if (root && root->splitManager()) {
        root->splitManager()->hideDragOverlay();
    }
    QFrame::dropEvent(event);
}

void ContentPanel::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    if (m_emptyFilterOverlay && m_viewStack) {
        m_emptyFilterOverlay->setGeometry(m_viewStack->rect());
    }
    updateAlphabetIndexBarGeometry();
}

void ContentPanel::setCurrentCategoryTitle(const QString& title) {
    m_categoryTitle = title;
    updateHeaderTitle();
}

QString ContentPanel::displayPath() const {
    if (!m_categoryTitle.isEmpty() && (m_currentCategoryType == "library" || m_currentCategoryType == "smart_folder")) {
        return m_categoryTitle;
    }
    return m_currentPath;
}

void ContentPanel::updateHeaderTitle() {
    if (!m_headerWidget) return;

    if (!m_categoryTitle.isEmpty() && (m_currentCategoryType == "library" || m_currentCategoryType == "smart_folder")) {
        m_headerWidget->setTitle(m_categoryTitle);
        m_headerWidget->setHeaderIcon("all_data");
        return;
    }

    m_headerWidget->setHeaderIcon("folder_filled");

    if (!isSplitMode()) {
        m_headerWidget->setTitle("");
        return;
    }

    if (m_currentPath.isEmpty() || m_currentPath == "computer://") {
        m_headerWidget->setTitle("此电脑");
        return;
    }
    if (m_currentPath == "trash://") {
        m_headerWidget->setTitle("回收站");
        return;
    }

    QString cleanP = QDir::cleanPath(m_currentPath);
    QFileInfo fi(cleanP);
    if (fi.isRoot() || cleanP.endsWith(":\\") || cleanP.endsWith(":/") || (cleanP.length() == 2 && cleanP.endsWith(':'))) {
        m_headerWidget->setTitle(cleanP);
        return;
    }

    QString folderName = fi.fileName();
    m_headerWidget->setTitle(folderName.isEmpty() ? cleanP : folderName);
}

void ContentPanel::applyViewTarget(const QString& path, const QString& categoryType, const QString& categoryTitle, int categoryId, bool recursive) {
    m_currentPath = path;
    m_currentCategoryType = categoryType;
    m_categoryTitle = categoryTitle;
    m_categoryId = categoryId;
    m_isRecursive = recursive;

    if (categoryType != "smart_folder") {
        m_activeSmartFolder = SmartFolderRecord();
    }

    if (m_model) {
        m_model->setCurrentPath(path);
    }

    if (m_headerWidget) {
        m_headerWidget->setRecursive(recursive);
    }

    updateHeaderTitle();

    ContentPanel* root = rootPane();
    if (root && root->splitManager()) {
        emit root->splitManager()->layoutChanged();
    }

    if (m_changeWatcher) {
        m_changeWatcher->updateTarget();
    }
}

void ContentPanel::setCurrentPath(const QString& path) {
    applyViewTarget(path, "", "", 0, m_isRecursive);
}

void ContentPanel::loadDirectory(const QString& path, bool recursive) {
    updateHeaderTitle();
    if (m_currentViewMode == ColumnView) {
        if (path.contains("://") && path != "computer://") {
            return;
        }
        setCurrentPath(path);
        m_isRecursive = recursive;
        if (m_columnView) {
            if (m_columnView->containsPath(path)) {
                m_columnView->refreshAllColumns();
            } else {
                m_columnView->setRootPath(path);
                restoreSelections();
            }
        }
        updateAlphabetIndexBar();
        updateStatusBarStats();
        return;
    }
    if (m_dataLoader) m_dataLoader->loadDirectory(path, recursive);
}

void ContentPanel::loadCategory(const QString& categoryType) {
    if (m_dataLoader) m_dataLoader->loadCategory(categoryType);
}

void ContentPanel::loadPaths(const QStringList& paths, int reqId) {
    if (m_dataLoader) m_dataLoader->loadPaths(paths, reqId);
}

void ContentPanel::loadLibraryPaths(const QStringList& paths, int reqId) {
    if (m_dataLoader) m_dataLoader->loadLibraryPaths(paths, reqId);
}

void ContentPanel::appendPaths(const QStringList& paths, int reqId) {
    if (m_dataLoader) m_dataLoader->appendPaths(paths, reqId);
}

bool ContentPanel::isWritablePhysicalDirectoryView() const {
    if (!m_currentCategoryType.isEmpty()) return false;
    QString p = activePath();
    if (p.isEmpty() || p.contains("://")) return false;
    return QDir(p).exists();
}

bool ContentPanel::canPaste(const QString& targetOverride) const {
    if (targetOverride.isEmpty()) {
        if (!isWritablePhysicalDirectoryView()) return false;
        return ClipboardService::instance().canPaste(activePath());
    }
    if (targetOverride.contains("://") || !QDir(targetOverride).exists()) return false;
    return ClipboardService::instance().canPaste(targetOverride);
}

void ContentPanel::performCopy(bool cutMode) {
    if (cutMode) ClipboardService::instance().cutItems(getSelectedPaths());
    else ClipboardService::instance().copyItems(getSelectedPaths());
}

void ContentPanel::performPaste() {
    if (m_fileOpsHandler) {
        m_fileOpsHandler->executePasteOperation();
    }
}

bool ContentPanel::resolvePasteDestination() {
    return m_fileOpsHandler ? m_fileOpsHandler->resolvePasteDestination() : false;
}

void ContentPanel::createNewItem(const QString& type) {
    if (m_fileOpsHandler) m_fileOpsHandler->createNewItem(type);
}

void ContentPanel::performBatchRename() {
    if (m_fileOpsHandler) m_fileOpsHandler->performBatchRename();
}

void ContentPanel::onPathsDropped(const QStringList& paths, const QModelIndex& targetIndex, const QString& targetDirOverride, Qt::DropAction action) {
    if (m_fileOpsHandler) m_fileOpsHandler->onPathsDropped(paths, targetIndex, targetDirOverride, action);
}

void ContentPanel::activateIndex(const QModelIndex& index) {
    if (!index.isValid()) return;
    if (index.data(SectionHeaderRole).toBool()) return;
    QString path = index.data(PathRole).toString();
    if (path.isEmpty()) return;

    bool isTrash = (m_currentCategoryType == "trash" || m_currentPath == "trash://" || index.data(IsDiskTrashRole).toBool());
    if (isTrash) {
        ToolTipOverlay::instance()->showToast("请先还原后再访问", 2000, QColor("#f39c12"));
        return;
    }

    if (QFileInfo(path).isDir()) {
        if (m_currentViewMode == ColumnView && m_columnView) {
            ColumnViewPane* pane = m_columnView->activePane();
            int paneIdx = pane ? pane->property("paneIndex").toInt() : 0;
            m_columnView->expandFolder(path, paneIdx);
        } else {
            emit directorySelected(path);
        }
    } else {
        emit fileActivated(path);
    }
}

void ContentPanel::toggleFolderSectionCollapse() {
    if (m_currentViewMode == ColumnView && m_columnView) {
        m_columnView->toggleFolderSectionCollapse();
    } else if (m_sectionModel) {
        m_sectionModel->setFolderCollapsed(!m_sectionModel->isFolderCollapsed());
        updateAlphabetIndexBar();
    }
}

bool ContentPanel::isFolderSectionCollapsed() const {
    if (m_currentViewMode == ColumnView) {
        return false;
    }
    return m_sectionModel && m_sectionModel->isFolderCollapsed();
}

void ContentPanel::expandFolderSection() {
    if (m_sectionModel && m_sectionModel->isFolderCollapsed()) {
        m_sectionModel->setFolderCollapsed(false);
        updateAlphabetIndexBar();
    }
}

void ContentPanel::setViewMode(ViewMode mode) {
    if (m_currentViewMode == mode) {
        return;
    }
    m_selectionState.currentFolder = m_currentPath;
    QStringList selList = getSelectedPaths();
    m_selectionState.selectedPaths = QSet<QString>(selList.begin(), selList.end());
    if (!m_selectionState.selectedPaths.isEmpty()) {
        QString focusedCandidate;
        QAbstractItemView* view = activeItemView();
        if (view) {
            QModelIndex curIdx = view->currentIndex();
            if (curIdx.isValid() && !curIdx.data(SectionHeaderRole).toBool()) {
                focusedCandidate = curIdx.data(PathRole).toString();
            }
        }
        if (!focusedCandidate.isEmpty()) {
            m_selectionState.focusedPath = focusedCandidate;
        } else {
            m_selectionState.focusedPath = *m_selectionState.selectedPaths.begin();
        }
    } else {
        m_selectionState.focusedPath.clear();
    }

    ViewMode oldMode = m_currentViewMode;
    m_currentViewMode = mode;

    if (mode == ListView) {
        int savedListZoom = AppConfig::instance().getValue("UI/ListZoomLevel", 30).toInt();
        m_zoomLevel = qBound(30, savedListZoom, 230);
    } else {
        int savedGridZoom = AppConfig::instance().getValue("UI/GridZoomLevel", 96).toInt();
        m_zoomLevel = qBound(93, savedGridZoom, 230);
    }

    if (mode == ListView) {
        m_viewStack->setCurrentWidget(m_treeView);
    } else if (mode == ColumnView) {
        if (m_columnView) {
            QString targetPath = !m_selectionState.focusedPath.isEmpty() ? m_selectionState.focusedPath : m_currentPath;
            if (targetPath.contains("://") && targetPath != "computer://") {
                targetPath = "computer://";
            }
            m_columnView->setRootPath(targetPath);
            m_viewStack->setCurrentWidget(m_columnView);
        }
    } else {
        auto* jv = qobject_cast<JustifiedView*>(m_gridView);
        if (jv) jv->setLayoutMode(mode == GridView ? JustifiedView::GridMode : JustifiedView::JustifiedMode);
        m_viewStack->setCurrentWidget(m_gridView);
    }

    if (oldMode == ColumnView && mode != ColumnView) {
        if (!m_currentPath.isEmpty()) {
            if (!m_currentCategoryType.isEmpty()) {
                refreshAll();
            } else {
                loadDirectory(m_currentPath, m_isRecursive);
            }
        }
    }

    setMinimumWidth(kMinPaneWidth);

    restoreSelections();

    AppConfig::instance().setValue("ContentPanel/ViewMode", static_cast<int>(mode));
    updateGridSize();
    updateAlphabetIndexBar();
    emit viewModeChanged(mode);
    emit zoomLevelChanged(m_zoomLevel);

    refreshVisibleThumbnails();

    if (m_changeWatcher) {
        m_changeWatcher->updateTarget();
    }
}

void ContentPanel::setZoomLevel(int level) {
    int minZoom = (m_currentViewMode == ListView) ? 30 : 93;
    int bounded = qBound(minZoom, level, 230);
    if (m_zoomLevel == bounded) return;
    m_zoomLevel = bounded;
    updateGridSize();
    emit zoomLevelChanged(m_zoomLevel);
    startVisibleTimer();
}

void ContentPanel::updateGridSize() {
    if (m_viewCoordinator) {
        m_viewCoordinator->updateGridSize(m_zoomLevel);
    }
    if (m_currentViewMode == ListView) {
        AppConfig::instance().setValue("UI/ListZoomLevel", m_zoomLevel);
    } else {
        AppConfig::instance().setValue("UI/GridZoomLevel", m_zoomLevel);
    }
}

void ContentPanel::applyFilters(const FilterState& state) {
    QString currentKw = m_currentFilter.keyword;
    bool sh = m_currentFilter.showHidden;
    m_currentFilter = state;
    m_currentFilter.keyword = currentKw;
    m_currentFilter.showHidden = sh;
    applyFilters();
}

void ContentPanel::applyFilters() {
    if (m_proxyModel) {
        m_proxyModel->currentFilter = m_currentFilter;
        m_proxyModel->updateFilter();
    }
    if (m_columnView) {
        m_columnView->applyFilterState(m_currentFilter);
    }
    updateAlphabetIndexBar();
    updateStatusBarStats();
}

void ContentPanel::search(const QString& query) {
    m_currentFilter.keyword = query;
    applyFilters();

    if (!query.trimmed().isEmpty() && m_model) {
        QStringList matchedPaths;
        if (m_proxyModel) {
            const int rows = m_proxyModel->rowCount();
            matchedPaths.reserve(rows);
            for (int r = 0; r < rows; ++r) {
                QModelIndex idx = m_proxyModel->index(r, 0);
                if (!idx.isValid() || idx.data(SectionHeaderRole).toBool()) continue;
                QString p = idx.data(PathRole).toString();
                if (!p.isEmpty()) matchedPaths.append(p);
            }
        }
        if (!matchedPaths.isEmpty()) {
            m_model->updateRecordMetadataBatch(matchedPaths);
        }
    }
}

void ContentPanel::refreshAll() {
    if (m_isLoading) return;

    if (m_currentViewMode == ColumnView) {
        if (m_columnView) m_columnView->refreshAllColumns();
        updateAlphabetIndexBar();
        return;
    }

    if (m_currentCategoryType == "trash" || m_currentPath == "trash://") {
        loadCategory("trash");
        return;
    }

    if (m_currentCategoryType == "library" || m_currentPath.startsWith("library://")) {
        if (m_categoryId != 0) {
            m_lastLoadedLibraryPaths = LibraryService::instance().getCategoryPaths(m_categoryId);
        }
        loadLibraryPaths(m_lastLoadedLibraryPaths);
        return;
    }

    if (m_currentCategoryType == "smart_folder" || m_currentPath.startsWith("smartfolder://")) {
        if (m_activeSmartFolder.id > 0 || !m_activeSmartFolder.rules.isEmpty()) {
            loadSmartFolder(m_activeSmartFolder, m_categoryTitle);
            return;
        }
        loadLibraryPaths(m_lastLoadedLibraryPaths);
        return;
    }

    if (!m_currentPath.isEmpty() && m_currentPath != "computer://" && !m_currentPath.contains("://")) {
        loadDirectory(m_currentPath, m_isRecursive);
    } else if (m_currentPath == "computer://") {
        loadDirectory("computer://");
    }
}

void ContentPanel::loadSmartFolder(const SmartFolderRecord& record, const QString& displayName) {
    if (m_dataLoader) {
        m_dataLoader->loadSmartFolder(record, displayName);
    }
}

void ContentPanel::refreshSmartFolderIfShowing() {
    if (m_currentCategoryType != "smart_folder" || (m_activeSmartFolder.id <= 0 && m_activeSmartFolder.rules.isEmpty())) {
        return;
    }

    uint64_t reqId = ++m_smartFolderReqId;
    SmartFolderRecord recordSnapshot = m_activeSmartFolder;
    QPointer<ContentPanel> weakThis(this);

    (void)QtConcurrent::run([weakThis, recordSnapshot, reqId]() {
        QStringList newPaths = SmartFolderDao::getMatchingPaths(recordSnapshot);

        QMetaObject::invokeMethod(qApp, [weakThis, recordSnapshot, reqId, newPaths]() {
            if (!weakThis || reqId != weakThis->m_smartFolderReqId) return;
            if (weakThis->m_currentCategoryType != "smart_folder" || weakThis->m_activeSmartFolder.id != recordSnapshot.id) return;

            auto normalizeSet = [](const QStringList& list) {
                QSet<QString> s;
                for (const QString& p : list) {
                    s.insert(QDir::toNativeSeparators(p).toLower());
                }
                return s;
            };

            if (normalizeSet(newPaths) == normalizeSet(weakThis->m_lastLoadedLibraryPaths)) {
                return;
            }

            QStringList selList = weakThis->getSelectedPaths();
            weakThis->m_selectionState.currentFolder = weakThis->m_currentPath;
            weakThis->m_selectionState.selectedPaths = QSet<QString>(selList.begin(), selList.end());
            if (!weakThis->m_selectionState.selectedPaths.isEmpty()) {
                weakThis->m_selectionState.focusedPath = *weakThis->m_selectionState.selectedPaths.begin();
            } else {
                weakThis->m_selectionState.focusedPath.clear();
            }

            weakThis->setLastLoadedLibraryPaths(newPaths);
            weakThis->loadLibraryPaths(newPaths);
        });
    });
}

void ContentPanel::updateItemMetadata(QString path) {
    if (path.isEmpty()) return;
    if (m_model) m_model->updateRecordMetadata(path);
    if (m_gridView && m_gridView->viewport()) m_gridView->viewport()->update();
    if (m_treeView && m_treeView->viewport()) m_treeView->viewport()->update();
    if (m_currentViewMode == ColumnView && m_columnView) m_columnView->updateMetadataForPath(path);

    if (m_currentCategoryType == "smart_folder") {
        LibrarySyncService::instance().syncPaths(QStringList{path});
        refreshSmartFolderIfShowing();
    }
}

void ContentPanel::updateItemsMetadata(const QStringList& paths) {
    if (paths.isEmpty()) return;
    if (m_model) m_model->updateRecordMetadataBatch(paths);
    if (m_gridView && m_gridView->viewport()) m_gridView->viewport()->update();
    if (m_treeView && m_treeView->viewport()) m_treeView->viewport()->update();
    if (m_currentViewMode == ColumnView && m_columnView) {
        for (const QString& p : paths) m_columnView->updateMetadataForPath(p);
    }

    if (m_currentCategoryType == "smart_folder") {
        LibrarySyncService::instance().syncPaths(paths);
        refreshSmartFolderIfShowing();
    }
}
void ContentPanel::migrateModelCache(const QString& oldPath, const QString& newPath) { if (m_model) m_model->migrateCache(oldPath, newPath); }
void ContentPanel::clearFolderCache(const QString& folderPath) { if (m_model) m_model->clearCacheForFolder(folderPath); }

void ContentPanel::onSelectionChanged() {
    QStringList currentSel = getSelectedPaths();
    m_selectionState.selectedPaths = QSet<QString>(currentSel.begin(), currentSel.end());
    emitSelectionChangedSignal();
}

void ContentPanel::emitSelectionChangedSignal() {
    QStringList paths = getSelectedPaths();
    emit selectionChanged(paths);
    updateStatusBarStats();
}

void ContentPanel::updateStatusBarStats(int cachedSelectedCount) {
    int folderCount = 0;
    int fileCount = 0;
    if (m_currentViewMode != ColumnView && m_sectionModel) {
        folderCount = m_sectionModel->folderCount();
        fileCount = m_sectionModel->fileCount();
    } else if (m_currentViewMode == ColumnView && m_columnView && m_columnView->rightmostPane()) {
        folderCount = m_columnView->rightmostPane()->sectionProxyModel() ? m_columnView->rightmostPane()->sectionProxyModel()->folderCount() : 0;
        fileCount = m_columnView->rightmostPane()->sectionProxyModel() ? m_columnView->rightmostPane()->sectionProxyModel()->fileCount() : 0;
    }
    int visibleCount = folderCount + fileCount;
    int fullCount = m_model ? m_model->rowCount() : visibleCount;
    int hiddenCount = fullCount - visibleCount;
    int selectedCount = (cachedSelectedCount >= 0) ? cachedSelectedCount : getSelectedIndexes().size();

    updateEmptyStateOverlay(visibleCount, fullCount, hiddenCount);

    QString numStyle = "color:#ff551c; font-weight:bold;";
    QString statusText = (hiddenCount > 0)
        ? QString("<html><body style=\"margin:0px; padding:0px; font-size:11px;\"><span style=\"%1\">%2</span> 个项目，<span style=\"%1\">%3</span> 个已隐藏，选中了 <span style=\"%1\">%4</span> 个</body></html>")
              .arg(numStyle).arg(visibleCount).arg(hiddenCount).arg(selectedCount)
        : QString("<html><body style=\"margin:0px; padding:0px; font-size:11px;\"><span style=\"%1\">%2</span> 个项目，选中了 <span style=\"%1\">%3</span> 个</body></html>")
              .arg(numStyle).arg(visibleCount).arg(selectedCount);

    emit statusBarMessageReady(statusText);
}

void ContentPanel::initEmptyFilterOverlay() {
    if (m_emptyFilterOverlay || !m_viewStack) return;

    m_emptyFilterOverlay = new EmptyStateOverlay(m_viewStack, [this]() {
        search("");
        emit filterResetRequested();
    });
    m_emptyFilterOverlay->installEventFilter(this);
    const auto overlayChildren = m_emptyFilterOverlay->findChildren<QWidget*>();
    for (QWidget* child : overlayChildren) {
        child->installEventFilter(this);
    }
}

void ContentPanel::updateEmptyStateOverlay(int visibleCount, int fullCount, int hiddenCount) {
    if (!m_emptyFilterOverlay) return;

    m_emptyFilterOverlay->updateState(visibleCount, fullCount, hiddenCount);

    JustifiedView* jv = qobject_cast<JustifiedView*>(m_gridView);
    if (jv) {
        jv->setShowEmptyHint(visibleCount != 0);
    }
}

void ContentPanel::runWhenStatsSettled(std::function<void()> fn) {
    if (!fn) return;
    bool isTimerActive = (m_statsDebounceTimer && m_statsDebounceTimer->isActive());
    bool isWorkerSettled = (m_statsWorker && m_statsWorker->isSettled());

    if (!isTimerActive && isWorkerSettled) {
        fn();
        return;
    }

    if (!m_statsWorker) {
        fn();
        return;
    }

    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = connect(m_statsWorker, &ContentStatsWorker::settled, this, [fn, conn]() {
        if (*conn) {
            QObject::disconnect(*conn);
        }
        fn();
    });
}

void ContentPanel::beginWatchSuppress() {
    if (m_changeWatcher) {
        m_changeWatcher->beginSuppress();
    }
}

void ContentPanel::endWatchSuppress() {
    if (m_changeWatcher) {
        m_changeWatcher->endSuppress();
    }
}

void ContentPanel::recalculateAndEmitStats() {
    QElapsedTimer timer;
    timer.start();
    size_t count = 0;

    if (m_currentViewMode == ColumnView && m_columnView) {
        ColumnViewPane* pane = m_columnView->rightmostPane();
        if (!pane) pane = m_columnView->activePane();
        if (pane && pane->model() && m_statsWorker) {
            const auto& allRecs = pane->model()->allRecords();
            if (!allRecs.empty()) {
                count = allRecs.size();
                std::vector<ItemRecord> snapshot = allRecs;
                m_statsWorker->processAsync(std::move(snapshot), m_currentFilter.showHidden);
            }
        }
    } else if (m_model && m_statsWorker) {
        const auto& allRecs = m_model->allRecords();
        if (!allRecs.empty()) {
            count = allRecs.size();
            std::vector<ItemRecord> snapshot = allRecs;
            m_statsWorker->processAsync(std::move(snapshot), m_currentFilter.showHidden);
        }
    }

    qint64 ms = timer.elapsed();
    if (ms > 5 || count > 500) {
        Logger::log(QString("[Perf-ContentPanel] recalculateAndEmitStats 调度派发: 记录数=%1, 主线程派发耗时=%2ms").arg(count).arg(ms));
    }
}

void ContentPanel::refreshVisibleThumbnails() {
    if (!m_model || CoreController::isShuttingDown()) return;

    if (m_currentViewMode == ColumnView) {
        if (m_columnView && m_columnView->activePane()) {
            m_columnView->activePane()->refreshVisibleThumbnails();
        }
    } else {
        if (m_viewCoordinator) m_viewCoordinator->refreshVisibleThumbnails();
    }
}

void ContentPanel::selectAndEditPath(const QString& path) {
    if (path.isEmpty()) return;

    QAbstractItemView* view = activeItemView();
    QAbstractItemModel* model = view ? view->model() : nullptr;

    if (m_currentViewMode == ColumnView) {
        if (m_columnView && m_columnView->activePane()) {
            view = m_columnView->activePane()->listView();
            if (view) model = view->model();
        }
    }

    if (!model || !view) return;

    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex idx = model->index(i, 0);
        if (idx.data(SectionHeaderRole).toBool()) continue;
        QString idxPath = idx.data(PathRole).toString();
        if (QString::compare(QDir::cleanPath(idxPath), QDir::cleanPath(path), Qt::CaseInsensitive) == 0) {
            view->setFocus();
            view->scrollTo(idx);
            view->setCurrentIndex(idx);
            if (view->selectionModel()) {
                view->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            }
            view->edit(idx);
            break;
        }
    }
}

void ContentPanel::selectAndScrollToPath(const QString& path) { selectAndScrollToItem(path); }
void ContentPanel::selectAndScrollToItem(const QString& path) {
    if (m_currentViewMode == ColumnView) {
        if (m_columnView) {
            QFileInfo info(path);
            if (info.exists() && !info.isDir()) {
                QString dirPath = info.absolutePath();
                if (!m_columnView->containsPath(dirPath)) {
                    m_columnView->setRootPath(path);
                } else if (m_columnView->rightmostPane()) {
                    m_columnView->rightmostPane()->selectItemByPath(path);
                }
            } else {
                if (!m_columnView->containsPath(path)) {
                    m_columnView->setRootPath(path);
                }
            }
        }
        return;
    }
    QAbstractItemView* view = activeItemView();
    if (!view || !view->model() || path.isEmpty()) return;
    QAbstractItemModel* model = view->model();
    int total = model->rowCount();
    for (int i = 0; i < total; ++i) {
        QModelIndex idx = model->index(i, 0);
        if (idx.data(SectionHeaderRole).toBool()) continue;
        if (idx.data(PathRole).toString() == path) {
            if (view->selectionModel()) {
                view->scrollTo(idx);
                view->setCurrentIndex(idx);
                view->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            }
            break;
        }
    }
}

QString ContentPanel::getAdjacentFilePath(const QString& currentPath, int delta) {
    QAbstractItemView* view = activeItemView();
    if (!view || !view->model()) return QString();
    QAbstractItemModel* model = view->model();
    int rowCount = model->rowCount();
    if (rowCount == 0) return QString();

    int curIdx = -1;
    for (int i = 0; i < rowCount; ++i) {
        QModelIndex idx = model->index(i, 0);
        if (idx.data(SectionHeaderRole).toBool()) continue;
        if (idx.data(PathRole).toString() == currentPath) {
            curIdx = i;
            break;
        }
    }

    if (curIdx == -1) return QString();

    int step = (delta > 0) ? 1 : -1;
    int remaining = std::abs(delta);
    int targetRow = curIdx;

    while (remaining > 0) {
        targetRow += step;
        if (targetRow < 0 || targetRow >= rowCount) return QString();
        QModelIndex idx = model->index(targetRow, 0);
        if (idx.data(SectionHeaderRole).toBool()) continue;
        remaining--;
    }

    return model->index(targetRow, 0).data(PathRole).toString();
}

QStringList ContentPanel::getSelectedPaths() const {
    QStringList paths;
    for (const auto& idx : getSelectedIndexes()) {
        if (idx.column() == static_cast<int>(FileListColumn::Name)) {
            QString p = idx.data(PathRole).toString();
            if (!p.isEmpty()) paths << p;
        }
    }
    return paths;
}

QList<int> ContentPanel::getSelectedTrashIds() const {
    QList<int> ids;
    for (const auto& idx : getSelectedIndexes()) {
        if (idx.column() == 0 && idx.data(IsDiskTrashRole).toBool()) {
            int id = idx.data(DiskTrashIdRole).toInt();
            if (id > 0) ids << id;
        }
    }
    return ids;
}

QAbstractItemView* ContentPanel::activeItemView() const {
    if (m_currentViewMode == ColumnView) {
        if (m_columnView && m_columnView->activePane()) {
            return m_columnView->activePane()->listView();
        }
        return nullptr;
    }

    if (m_currentViewMode == ListView) {
        return m_treeView;
    }

    return m_gridView;
}

QModelIndexList ContentPanel::getSelectedIndexes() const {
    if (m_viewCoordinator) {
        return m_viewCoordinator->getSelectedIndexes();
    }
    QAbstractItemView* view = activeItemView();
    if (view && view->selectionModel()) {
        return view->selectionModel()->selectedIndexes();
    }
    return {};
}

void ContentPanel::restoreActiveView() {
    if (m_currentViewMode == ColumnView) {
        m_viewStack->setCurrentWidget(m_columnView);
    } else {
        m_viewStack->setCurrentWidget(m_currentViewMode == ListView ? static_cast<QWidget*>(m_treeView) : static_cast<QWidget*>(m_gridView));
    }
}

void ContentPanel::restoreSelections() {
    if (!m_viewCoordinator || m_isRestoringSelections) return;

    m_isRestoringSelections = true;

    if (!m_pendingSelectPath.isEmpty()) {
        QString targetPath = m_pendingSelectPath;
        bool isEdit = m_pendingIsEdit;
        m_pendingSelectPath.clear();
        m_pendingIsEdit = false;

        QSet<QString> targetSet;
        targetSet.insert(targetPath);
        m_viewCoordinator->restoreSelections(targetSet, isEdit);
    } else if (!m_selectionState.selectedPaths.isEmpty()) {
        QSet<QString> pathsToRestore = m_selectionState.selectedPaths;
        m_selectionState.selectedPaths.clear();
        m_viewCoordinator->restoreSelections(pathsToRestore, false);
    }

    m_isRestoringSelections = false;
    setActivePane(true);
}

void ContentPanel::setPendingSelectPath(const QString& fullPath, bool edit) {
    if (fullPath.isEmpty()) return;
    m_pendingSelectPath = QDir::toNativeSeparators(QDir::cleanPath(fullPath));
    m_pendingIsEdit = edit;
}

void ContentPanel::setPendingSelectPaths(const QStringList& fullPaths) {
    if (fullPaths.isEmpty()) return;
    m_selectionState.selectedPaths.clear();
    for (const QString& p : fullPaths) {
        m_selectionState.selectedPaths.insert(QDir::toNativeSeparators(QDir::cleanPath(p)));
    }
}

void ContentPanel::setPendingSelectName(const QString& name, bool edit) {
    if (name.isEmpty()) return;
    setPendingSelectPath(QDir(m_currentPath).filePath(name), edit);
}

void ContentPanel::updateLayersButtonState() {
    if (!m_headerWidget) return;
    bool isComp = m_currentPath.isEmpty() || m_currentPath == "computer://";
    m_headerWidget->setLayersEnabled(!isComp, isComp ? "“此电脑”不支持递归显示" : "显示子文件夹中的项目（Ctrl+R）");
}

ContentPanel::DataSourceType ContentPanel::dataSourceType() const {
    return (m_currentCategoryType == "path_list" || m_currentCategoryType == "search") ? DataSourceType::PathList : DataSourceType::DiskNav;
}

void ContentPanel::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        setZoomLevel(m_zoomLevel + (event->angleDelta().y() > 0 ? 8 : -8));
        event->accept();
        return;
    }
    QFrame::wheelEvent(event);
}

} // namespace QuarkMeta