#pragma once

#include <QFrame>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QTreeView>
#include <QScrollArea>
#include <QSet>
#include <QModelIndexList>
#include <QSplitter>
#include <atomic>

#include "ScanStats.h"
#include "FilterPanel.h"
#include "models/ItemModelBase.h"
#include "models/DiskItemModel.h"
#include "models/FilterProxyModel.h"
#include "controllers/ContentSortController.h"
#include "../core/ModelContract.h"
#include "../meta/SmartFolderDao.h"

namespace QuarkMeta {

class DropTreeView;
class JustifiedView;
class ContentKeyHandler;
class ContentDataLoader;
class ContentFileOpsHandler;
class ContentStatsWorker;
class SectionProxyModel;
class ContentPaneSplitManager;
class ContentChangeWatcher;

/**
 * @brief 内容面板（面板四）：核心业务展示工作台（纯视图承载与高级意图分发）
 */
class ContentPanel : public QFrame {
    Q_OBJECT

public:
    enum class DataSourceType {
        DiskNav,
        PathList
    };

    using SortType = QuarkMeta::SortType;
    static constexpr SortType SortByName = SortType::SortByName;
    static constexpr SortType SortByCreateDate = SortType::SortByCreateDate;
    static constexpr SortType SortByModifyDate = SortType::SortByModifyDate;
    static constexpr SortType SortByExtension = SortType::SortByExtension;
    static constexpr SortType SortBySize = SortType::SortBySize;
    static constexpr SortType SortByDimension = SortType::SortByDimension;
    static constexpr SortType SortByRating = SortType::SortByRating;
    static constexpr SortType SortByAddedDate = SortType::SortByAddedDate;

    enum ViewMode {
        ListView,
        GridView,
        JustifiedViewMode,
        ColumnView
    };

    enum ContextAction {
        ActionNone, ActionOpen, ActionOpenInNewTab, ActionOpenDefault, ActionShowInExplorer, ActionShowInQuarkMeta, ActionNewFolder, ActionNewMd, ActionNewTxt, ActionCreateFolderWithSelectedItems,
        ActionPin, ActionUnpin, ActionColorTag, ActionEncrypt, ActionDecrypt, ActionChangePwd,
        ActionBatchRename, ActionRename, ActionCopy, ActionCut, ActionDuplicate, ActionPaste, ActionCopyTags, ActionPasteTags, ActionRepeatLastOp, ActionDelete,
        ActionPermanentDelete, ActionSecureDelete, ActionRestore, ActionRestoreAll, ActionEmptyTrash,
        ActionCopyName, ActionCopyPath, ActionAddToFavorites, ActionRefresh, ActionReextractThumbnail, ActionBatchCreate, ActionFileCompare,
        ActionOpenDirectoryInExplorer, ActionExtractArchive, ActionCompressToZip
    };

    explicit ContentPanel(QWidget* parent = nullptr);
    ~ContentPanel() override = default;

    static constexpr int kMinPaneWidth = 230;
    static constexpr int kMinPaneHeight = 230;
    static constexpr int kMaxPanes = 4;

    // Dual-pane state inspection & split controls
    /**
     * @brief 检查当前整个窗格组（主窗格 + 所有副窗格）是否处于分屏模式
     * @note 无论在主窗格还是副窗格上调用，均返回整个窗格组的统一分屏状态。禁止调用方对此类方法执行递归调用！
     */
    bool isSplitMode() const;
    Qt::Orientation splitOrientation() const;
    bool isSecondaryPane() const;
    void setIsSecondaryPane(bool secondary);
    ContentPanel* secondaryContentPanel() const;

    /**
     * @brief 获取当前整个分屏窗格组的全部副窗格列表（扁平一维列表，不含主窗格）
     * @note 无论在主窗格还是副窗格上调用，均返回整个窗格组的扁平副窗格列表。禁止调用方对此列表进行递归遍历！
     */
    QList<ContentPanel*> panes() const;

    /**
     * @brief 获取当前整个分屏窗格组的总窗格数（主窗格 1 + 全部副窗格数）
     */
    int paneCount() const;
    ContentPanel* rootPane() const;
    void splitPane(Qt::Orientation orientation, const QString& secondaryPath = QString(), bool insertBefore = false);
    void closePane(ContentPanel* pane);
    void closeSecondaryPane();
    void requestClosePane();
    void setActivePane(bool active);

    QSize minimumSizeHint() const override { return QSize(kMinPaneWidth, 100); }
    void deferredInit() {}

    // 1. 状态与配置查询
    QString currentPath() const { return m_currentPath; }
    QString activePath() const;
    QString pathForView(QAbstractItemView* view) const;
    bool isRecursive() const { return m_isRecursive; }
    int zoomLevel() const { return m_zoomLevel; }
    ViewMode currentViewMode() const { return m_currentViewMode; }
    bool canPaste(const QString& targetOverride = QString()) const;
    bool isWritablePhysicalDirectoryView() const;
    DataSourceType dataSourceType() const;
    bool isContextMenuActive() const { return m_isContextMenuActive; }
    QString getCurrentCategoryType() const { return m_currentCategoryType; }
    QString currentCategoryTitle() const { return m_categoryTitle; }
    int currentCategoryId() const { return m_categoryId; }
    void setCurrentCategoryId(int id) { m_categoryId = id; }
    QString displayPath() const;
    void setCurrentCategoryTitle(const QString& title);
    int currentLoadRequestId() const { return m_loadRequestId.load(); }
    int loadRequestId() const { return m_loadRequestId.load(); }
    int incrementLoadRequestId() { return ++m_loadRequestId; }
    const FilterState& currentFilter() const { return m_currentFilter; }
    bool isLoading() const { return m_isLoading.load(); }
    SmartFolderRecord activeSmartFolder() const { return m_activeSmartFolder; }
    void setActiveSmartFolder(const SmartFolderRecord& record) { m_activeSmartFolder = record; }
    uint64_t smartFolderRequestId() const { return m_smartFolderReqId; }
    uint64_t incrementSmartFolderRequestId() { return ++m_smartFolderReqId; }

    // 2. 状态与配置高阶方法
    void updateHeaderTitle();
    void applyViewTarget(const QString& path, const QString& categoryType, const QString& categoryTitle = QString(), int categoryId = 0, bool recursive = false);
    void setCurrentPath(const QString& path);
    void setIsRecursive(bool recursive);
    void setLoading(bool loading);
    void notifyRecordsChanged();
    void ensureSourceModelIsDiskModel();
    void applySort();
    void startVisibleTimer();
    void updateLayersButtonState();
    void restoreActiveView();
    void restoreSelections();
    void incrementModelGeneration();
    void reloadThumbnailForPath(const QString& path);
    bool isTreeView(QObject* view) const;

    // 3. 排序策略控制器
    ContentSortController* sortController() const { return m_sortController; }
    SortType currentSortType() const { return m_sortController ? m_sortController->sortType() : SortType::SortByName; }
    Qt::SortOrder currentSortOrder() const { return m_sortController ? m_sortController->sortOrder() : Qt::AscendingOrder; }
    void setSortType(SortType type) { if (m_sortController) m_sortController->setSortType(type); }
    void setSortOrder(Qt::SortOrder order) { if (m_sortController) m_sortController->setSortOrder(order); }
    void setSortCriteria(SortType type, Qt::SortOrder order) { if (m_sortController) m_sortController->setSortCriteria(type, order); }
    void setContextMenuActive(bool active) { m_isContextMenuActive = active; }
    void setCurrentCategoryType(const QString& type);

    // 4. 视图与控制器引用
    QAbstractItemView* activeItemView() const;
    QVBoxLayout* mainLayout() const { return m_mainLayout; }
    class ContentHeaderWidget* headerWidget() const { return m_headerWidget; }
    QStackedWidget* viewStack() const { return m_viewStack; }
    QAbstractItemView* gridView() const { return m_gridView; }
    QTreeView* treeView() const;
    class ColumnViewWidget* columnView() const { return m_columnView; }
    DropTreeView* dropTreeView() const { return m_treeView; }
    DiskItemModel* diskModel() const { return m_diskModel; }
    ContentKeyHandler* keyHandler() const { return m_keyHandler; }
    ContentDataLoader* dataLoader() const { return m_dataLoader; }
    ContentFileOpsHandler* fileOpsHandler() const { return m_fileOpsHandler; }
    ContentStatsWorker* statsWorker() const { return m_statsWorker; }
    class ContentViewCoordinator* viewCoordinator() const { return m_viewCoordinator; }
    ContentPaneSplitManager* splitManager() const { return m_splitManager; }

    // 5. 业务操作分发
    void performCopy(bool cutMode);
    void performPaste();
    void performBatchRename();
    bool resolvePasteDestination();
    void setViewMode(ViewMode mode);
    void toggleFolderSectionCollapse();
    bool isFolderSectionCollapsed() const;
    void expandFolderSection();
    void selectAndScrollToPath(const QString& path);
    void selectAndScrollToItem(const QString& path);
    void selectAndEditPath(const QString& path);
    QString getAdjacentFilePath(const QString& currentPath, int delta);

    bool eventFilter(QObject* obj, QEvent* event) override;

    // 6. 模型与选中数据访问
    ItemModelBase* model() const { return m_model; }
    QStringList lastLoadedLibraryPaths() const { return m_lastLoadedLibraryPaths; }
    void setLastLoadedLibraryPaths(const QStringList& paths) { m_lastLoadedLibraryPaths = paths; }
    QStringList getSelectedPaths() const;
    QList<int> getSelectedTrashIds() const;
    QModelIndexList getSelectedIndexes() const;

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

signals:
    void filterResetRequested();
    void panelActivated(ContentPanel* panel);
    void secondaryPaneCreated(ContentPanel* pane);
    void secondaryPaneClosed();
    void closePaneRequested();
    void zoomLevelChanged(int level);
    void viewModeChanged(ViewMode mode);
    void fileActivated(const QString& path);
    void selectionChanged(const QStringList& paths);
    void directorySelected(const QString& path);
    void requestAddFavorite(const QStringList& paths);
    void requestRemoveFavorite(const QStringList& paths);
    void dataSourceChanged(const QString& source);
    void directoryStatsReady(const QuarkMeta::ScanStats& stats);
    void statusBarStatsUpdated(int fileCount, int folderCount, int totalCount);
    void statusBarMessageReady(const QString& message);
    void dualPanePathsChanged(const QString& path1, const QString& path2);

public slots:
    void setZoomLevel(int level);
    void onSelectionChanged();
    void onCustomContextMenuRequested(const QPoint& pos);
    void onCustomContextMenuRequested(QAbstractItemView* view, const QPoint& pos);
    void activateIndex(const QModelIndex& index);
    void onPathsDropped(const QStringList& paths, const QModelIndex& targetIndex, const QString& targetDirOverride = QString(), Qt::DropAction action = Qt::CopyAction);
    void loadDirectory(const QString& path, bool recursive = false);
    void setPendingSelectPath(const QString& fullPath, bool edit = false);
    void setPendingSelectName(const QString& name, bool edit = false);
    void refreshAll();
    void loadSmartFolder(const SmartFolderRecord& record, const QString& displayName = QString());
    void refreshSmartFolderIfShowing();
    void updateItemMetadata(QString path);
    void updateItemsMetadata(const QStringList& paths);
    void migrateModelCache(const QString& oldPath, const QString& newPath);
    void clearFolderCache(const QString& folderPath);
    void search(const QString& query);
    void applyFilters(const FilterState& state);
    void applyFilters();
    void createNewItem(const QString& type);
    void loadPaths(const QStringList& paths, int reqId = 0);
    void loadLibraryPaths(const QStringList& paths, int reqId = 0);
    void appendPaths(const QStringList& paths, int reqId = 0);
    void loadCategory(const QString& categoryType);
    void refreshVisibleThumbnails();
    void recalculateAndEmitStats();
    void runWhenStatsSettled(std::function<void()> fn);

protected:
    void wheelEvent(QWheelEvent* event) override;

private:
    void initUi();
    void initGridView();
    void initListView();
    void updateGridSize();
    void updateStatusBarStats(int cachedSelectedCount = -1);
    void initEmptyFilterOverlay();
    void updateEmptyStateOverlay(int visibleCount, int fullCount, int hiddenCount);
    void emitSelectionChangedSignal();

    // 单一事实来源配置与状态 (FilterState)
    QStringList m_lastLoadedLibraryPaths;
    FilterState m_currentFilter;
    int m_zoomLevel = 96;
    QString m_currentPath;
    struct SelectionState {
        QString currentFolder;
        QString focusedPath;
        QSet<QString> selectedPaths;
    };
    SelectionState m_selectionState;
    bool m_isRestoringSelections = false;
    bool m_isPendingEdit = false;
    QString m_pendingSelectPath;
    bool m_pendingIsEdit = false;
    QString m_currentCategoryType;
    QString m_categoryTitle;
    int m_categoryId = 0;
    bool m_isRecursive = false;
    SmartFolderRecord m_activeSmartFolder;
    uint64_t m_smartFolderReqId = 0;
    ViewMode m_currentViewMode = static_cast<ViewMode>(-1);
    std::atomic<bool> m_isLoading{false};
    bool m_isContextMenuActive = false;
    std::atomic<int> m_loadRequestId{0};

    // UI 组件指针
    QVBoxLayout* m_mainLayout = nullptr;
    class ContentHeaderWidget* m_headerWidget = nullptr;

    ContentPaneSplitManager* m_splitManager = nullptr;

    void redistributePaneSizes();

    void updateDragOverlay(const QPoint& pos);
    void hideDragOverlay();

    DropTreeView* m_treeView = nullptr;
    QAbstractItemView* m_gridView = nullptr;
    FilterProxyModel* m_proxyModel = nullptr;
    SectionProxyModel* m_sectionModel = nullptr;

    QStackedWidget* m_viewStack = nullptr;
    QWidget* m_emptyFilterOverlay = nullptr;
    QLabel* m_emptyFilterTitleLabel = nullptr;
    QLabel* m_emptyFilterSubLabel = nullptr;
    QPushButton* m_emptyFilterResetBtn = nullptr;
    class ColumnViewWidget* m_columnView = nullptr;
    DiskItemModel* m_diskModel = nullptr;
    ItemModelBase* m_model = nullptr;
    
    QTimer* m_visibleTimer = nullptr;
    QTimer* m_statsDebounceTimer = nullptr;
    ContentSortController* m_sortController = nullptr;
    ContentKeyHandler* m_keyHandler = nullptr;
    ContentDataLoader* m_dataLoader = nullptr;
    ContentFileOpsHandler* m_fileOpsHandler = nullptr;
    ContentStatsWorker* m_statsWorker = nullptr;
    class ContentViewCoordinator* m_viewCoordinator = nullptr;
    ContentChangeWatcher* m_changeWatcher = nullptr;

public:
    void beginWatchSuppress();
    void endWatchSuppress();
};

} // namespace QuarkMeta