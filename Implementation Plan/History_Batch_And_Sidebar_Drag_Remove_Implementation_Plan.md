# Implementation Plan: 侧边栏历史多选批量操作与侧边栏拖拽移出脱管实施方案

本实施方案旨在严格遵循工业级 C++/Qt 架构规范与 `Memories.md`（第 20、26、30 章），在 `QuarkMeta_Source` 桌面应用中完整实现：
1. **侧边栏“历史”选项卡 (`HistoryPanel`) 全面支持多选与右键批量操作**：将树视图选择模式提升为多选（`ExtendedSelection`），右键上下文菜单的所有功能项全面升级为批量处理（默认程序批量打开、资源管理器分组定位、在QuarkMeta中显示、打开方式级联、批量添加收藏/库、批量复制/剪切、批量复制名称/路径、批量从历史移除）；
2. **侧边栏（收藏夹、库分类、标记）拖拽项目至主内容面板或目录导航面板的“移出/脱管”交互语义**：通过在全局 `ViewDragDropHelper` 中记录拖拽源面板类型与源分类 ID；在接收面板 (`ContentFileOpsHandler` 与 `NavPanel`) 中识别“脱管”意图，从源栏目中解绑/移出/取消标记，绝不触碰底层物理文件。

---

## 一、方案设计与根因分析 (Overview & Root Cause Analysis)

### 1.1 问题一根因与设计目标：历史面板多选与批量操作
- **现状分析**：
  在 `QuarkMeta_Source/src/ui/HistoryPanel.cpp` 中：
  ```cpp
  m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  ```
  视图被写死为单选模式；在 `onContextMenuRequested` 中仅通过 `QTreeWidgetItem* item = m_treeWidget->itemAt(pos)` 获取光标下的单一项，导致右键菜单里的所有动作（打开、资源管理器定位、加入收藏夹、复制路径等）全为单文件操作。在鼠标拖拽事件中，也只提取了单个鼠标所在项的 `QUrl`。
- **设计目标**：
  1. 将树视图的选择模式配置为 `QAbstractItemView::ExtendedSelection`；
  2. 右键菜单被触发时，若右击位置处于未被选中的项上，则先选中该项；若处于已选中的项集合内，则保留多选状态；
  3. 提取当前所有被选中的项目物理路径列表 `selectedPaths`（去重、去空）；
  4. 上下文菜单全量批量化：
     - **用系统默认程序打开**：遍历选中的文件列表，批量记录访问并调用 `QDesktopServices::openUrl`；
     - **在“资源管理器”中显示**：调用资源管理器分组打开逻辑（按所在文件夹分组，若文件夹数 > 5 则弹出 `FramelessConfirmDialog` 确认，否则依次调用 `ShellHelper::openInExplorer`）；
     - **在 QuarkMeta 中显示**：单选时（`selectedPaths.size() == 1`）发射 `requestLocateFile`；多选时置灰禁用，防止并发定位竞争；
     - **打开方式**：直接传递 `selectedPaths` 集合调用 `ContextMenuFactory::buildOpenWithMenu(&menu, selectedPaths, this)`；
     - **添加至收藏夹**：为选中的有效路径集合批量添加收藏，并通过 `ToolTipOverlay` 提示反馈；
     - **添加至“库”**：调用 `LibraryService::instance().buildLibraryAction(&menu, selectedPaths, this)`；
     - **复制 / 剪切**：调用 `ClipboardService::instance().copyItems(selectedPaths)` 与 `cutItems(selectedPaths)`；
     - **复制名称**：调用 `ContextMenuFactory::buildCopyNameAction(&menu, selectedPaths, this)`；
     - **复制完整路径**：调用 `ContextMenuFactory::buildCopyPathAction(&menu, selectedPaths, this)`；
     - **从历史中移除**：批量遍历选中的路径调用 `HistoryService::instance().removeHistory(p)`，并从树控件中移除对应 `QTreeWidgetItem`；
  5. 拖拽启动逻辑：若拖拽发生时存在多选项，提取所有多选项的本地文件路径生成 `QList<QUrl>` 置入 `QMimeData`，实现批量拖拽。

### 1.2 问题二根因与设计目标：侧边栏拖拽移出/脱管交互语义
- **现状分析**：
  目前从侧边栏（收藏夹 `FavoritePanel`、库 `LibraryPanel`、标记 `MarkedPanel`）拖拽项目时，仅注入了系统的 `text/uri-list` 或树内重排的私有数据。主内容面板 (`ContentPanel`) 或目录导航面板 (`NavPanel`) 收到带有 `urls` 的拖放时，若在普通磁盘目录中，会误将其当作物理文件的“复制/移动”，甚至可能造成误操作。
- **设计目标**：
  1. **拖拽源状态跟踪 (SSOT)**：
     - 在 `ViewDragDropHelper` 中新增源类型枚举 `enum class DragSourcePanel { None, Favorite, Library, Marked }`，并在其内部维护 `s_lastDragSourcePanel` 与 `s_lastDragSourceCategoryId`，提供 Setter/Getter；
     - 收藏夹 (`DropTreeView::startDrag`) 拖拽外发时，设置 `ViewDragDropHelper::setLastDragSource(ViewDragDropHelper::DragSourcePanel::Favorite)`；
     - 库面板 (`DropTreeView::startDrag`) 拖拽外发时，设置 `ViewDragDropHelper::setLastDragSource(ViewDragDropHelper::DragSourcePanel::Library, srcCatId)`；
     - 标记面板 (`MarkedPanel::eventFilter`) 拖拽外发时，设置 `ViewDragDropHelper::setLastDragSource(ViewDragDropHelper::DragSourcePanel::Marked)`；
     - 拖拽结束时自动复位 `ViewDragDropHelper::clearLastDragSource()`；
  2. **接收端识别与脱管拦截 (Drop Handling)**：
     - 在主内容面板 (`ContentFileOpsHandler::onPathsDropped`) 与目录导航面板 (`NavPanel`) 接收到拖放时，优先判断 `ViewDragDropHelper::lastDragSourcePanel()`：
       - 若源自收藏夹 (`DragSourcePanel::Favorite`)：拦截常规文件移动，遍历路径调用 `FavoriteService::instance().removeFavorite(path)` 移出收藏，并显示 Toast 提示 `已将 X 个项目从收藏夹中移出`；
       - 若源自库分类 (`DragSourcePanel::Library`)：拦截常规文件移动，获取源分类 ID，调用 `LibraryService::instance().removePathsFromCategory(catId, paths)` 解除归类，并显示 Toast 提示；
       - 若源自标记 (`DragSourcePanel::Marked`)：拦截常规文件移动，遍历路径调用 `MarkedFolderService::instance().removeMarked(path)` 取消标记，并显示 Toast 提示 `已取消 X 个目标文件夹标记`；
     - 严格物理隔离：完全不执行磁盘文件底层的剪切、移动或删除操作，确保安全。

---

## 二、影响文件清单 (Modified Files List)

1. `QuarkMeta_Source/src/ui/ViewDragDropHelper.h`
2. `QuarkMeta_Source/src/ui/ViewDragDropHelper.cpp`
3. `QuarkMeta_Source/src/ui/DropTreeView.cpp`
4. `QuarkMeta_Source/src/ui/HistoryPanel.cpp`
5. `QuarkMeta_Source/src/ui/MarkedPanel.cpp`
6. `QuarkMeta_Source/src/ui/controllers/ContentFileOpsHandler.cpp`
7. `QuarkMeta_Source/src/ui/NavPanel.cpp`

---

## 三、精准替换块 (Detailed Line-by-Line Changes)

### 3.1 `QuarkMeta_Source/src/ui/ViewDragDropHelper.h`

增加拖拽源跟踪定义：
<<<<<<< SEARCH
    static bool handleDragEnter(QAbstractItemView* view, QDragEnterEvent* event);
    static bool handleDragMove(QAbstractItemView* view, QDragMoveEvent* event);
    static bool handleDrop(QAbstractItemView* view, QDropEvent* event, QStringList& outPaths, QModelIndex& outTargetIdx, Qt::DropAction* outAction = nullptr, QPoint* outStartPos = nullptr);
    static void executeStartDrag(QAbstractItemView* view, Qt::DropActions supportedActions);
    static QPoint lastDragStartPos() { return s_lastDragStartPos; }

    static bool isDropTarget(const QWidget* widget, const QModelIndex& index);
    static void setHoverTarget(QAbstractItemView* view, const QModelIndex& index);
    static void clearHover(QAbstractItemView* view = nullptr);

private:
    static QAbstractItemView* s_hoverView;
    static QPersistentModelIndex s_hoverIndex;
    static QPoint s_lastDragStartPos;
=======
    enum class DragSourcePanel {
        None,
        Favorite,
        Library,
        Marked
    };

    static bool handleDragEnter(QAbstractItemView* view, QDragEnterEvent* event);
    static bool handleDragMove(QAbstractItemView* view, QDragMoveEvent* event);
    static bool handleDrop(QAbstractItemView* view, QDropEvent* event, QStringList& outPaths, QModelIndex& outTargetIdx, Qt::DropAction* outAction = nullptr, QPoint* outStartPos = nullptr);
    static void executeStartDrag(QAbstractItemView* view, Qt::DropActions supportedActions);
    static QPoint lastDragStartPos() { return s_lastDragStartPos; }

    static void setLastDragSource(DragSourcePanel source, int categoryId = 0);
    static DragSourcePanel lastDragSourcePanel() { return s_lastDragSourcePanel; }
    static int lastDragSourceCategoryId() { return s_lastDragSourceCategoryId; }
    static void clearLastDragSource();

    static bool isDropTarget(const QWidget* widget, const QModelIndex& index);
    static void setHoverTarget(QAbstractItemView* view, const QModelIndex& index);
    static void clearHover(QAbstractItemView* view = nullptr);

private:
    static DragSourcePanel s_lastDragSourcePanel;
    static int s_lastDragSourceCategoryId;
    static QAbstractItemView* s_hoverView;
    static QPersistentModelIndex s_hoverIndex;
    static QPoint s_lastDragStartPos;
>>>>>>> REPLACE

---

### 3.2 `QuarkMeta_Source/src/ui/ViewDragDropHelper.cpp`

静态成员初始化与管理方法定义：
<<<<<<< SEARCH
QAbstractItemView* ViewDragDropHelper::s_hoverView = nullptr;
QPersistentModelIndex ViewDragDropHelper::s_hoverIndex;
QPoint ViewDragDropHelper::s_lastDragStartPos;
=======
ViewDragDropHelper::DragSourcePanel ViewDragDropHelper::s_lastDragSourcePanel = ViewDragDropHelper::DragSourcePanel::None;
int ViewDragDropHelper::s_lastDragSourceCategoryId = 0;
QAbstractItemView* ViewDragDropHelper::s_hoverView = nullptr;
QPersistentModelIndex ViewDragDropHelper::s_hoverIndex;
QPoint ViewDragDropHelper::s_lastDragStartPos;

void ViewDragDropHelper::setLastDragSource(DragSourcePanel source, int categoryId) {
    s_lastDragSourcePanel = source;
    s_lastDragSourceCategoryId = categoryId;
}

void ViewDragDropHelper::clearLastDragSource() {
    s_lastDragSourcePanel = DragSourcePanel::None;
    s_lastDragSourceCategoryId = 0;
}
>>>>>>> REPLACE

---

### 3.3 `QuarkMeta_Source/src/ui/DropTreeView.cpp`

在 `DropTreeView::startDrag` 中识别收藏夹与库面板，跟踪拖拽源：
<<<<<<< SEARCH
    QMimeData* mimeData = new QMimeData();
    mimeData->setData("application/x-quarkmeta-tree-nodes", QByteArray("1"));
    if (!urls.isEmpty()) {
        mimeData->setUrls(urls);
    }

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);

    QPixmap pixmap(1, 1);
    pixmap.fill(Qt::transparent);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(0, 0));

    drag->exec(supportedActions | Qt::CopyAction | Qt::MoveAction, Qt::MoveAction);

    // 拖拽结束后清理
    m_internalDragSources.clear();
=======
    QMimeData* mimeData = new QMimeData();
    mimeData->setData("application/x-quarkmeta-tree-nodes", QByteArray("1"));

    if (this->objectName() == "FavoriteTreeView") {
        ViewDragDropHelper::setLastDragSource(ViewDragDropHelper::DragSourcePanel::Favorite);
    } else if (this->objectName() == "LibraryTreeView") {
        int srcCatId = -2;
        if (!filteredIndexes.isEmpty()) {
            srcCatId = filteredIndexes.first().data(Qt::UserRole + 1).toInt();
        }
        ViewDragDropHelper::setLastDragSource(ViewDragDropHelper::DragSourcePanel::Library, srcCatId);
    }

    if (!urls.isEmpty()) {
        mimeData->setUrls(urls);
    }

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);

    QPixmap pixmap(1, 1);
    pixmap.fill(Qt::transparent);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(0, 0));

    drag->exec(supportedActions | Qt::CopyAction | Qt::MoveAction, Qt::MoveAction);

    // 拖拽结束后清理
    ViewDragDropHelper::clearLastDragSource();
    m_internalDragSources.clear();
>>>>>>> REPLACE

---

### 3.4 `QuarkMeta_Source/src/ui/HistoryPanel.cpp`

添加必要的头文件包含：
<<<<<<< SEARCH
#include "../util/ThumbnailPipelineService.h"
#include "../util/ColorPaletteEngine.h"
#include <QDesktopServices>
=======
#include "../util/ThumbnailPipelineService.h"
#include "../util/ColorPaletteEngine.h"
#include "../util/ShellHelper.h"
#include "dialogs/FramelessConfirmDialog.h"
#include <QDesktopServices>
>>>>>>> REPLACE

将树视图选择模式修改为 `ExtendedSelection`：
<<<<<<< SEARCH
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeWidget->setDragEnabled(true);
=======
    m_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeWidget->setDragEnabled(true);
>>>>>>> REPLACE

升级右键上下文菜单为全量批量操作：
<<<<<<< SEARCH
void HistoryPanel::onContextMenuRequested(const QPoint& pos) {
    QTreeWidgetItem* item = m_treeWidget->itemAt(pos);
    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    if (item) {
        QString path = item->data(0, Qt::UserRole).toString();
        if (!path.isEmpty()) {
            // 1. 用系统默认程序打开
            QAction* actOpenDefault = menu.addAction(UiHelper::getIcon("launch", QColor("#EEEEEE"), 18), "用系统默认程序打开");
            connect(actOpenDefault, &QAction::triggered, this, [path]() {
                if (QFileInfo::exists(path)) {
                    HistoryService::instance().recordFileAccess(path);
                    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
                }
            });

            // 2. 在“资源管理器”中显示
            ContextMenuFactory::buildShowInExplorerAction(&menu, path, false, this);

            // 3. 在 QuarkMeta 中显示
            QAction* actShowInApp = menu.addAction(UiHelper::getIcon("folder_filled", QColor("#EEEEEE"), 18), "在 QuarkMeta 中显示");
            connect(actShowInApp, &QAction::triggered, this, [this, path]() {
                emit requestLocateFile(path);
            });

            // 4. 打开方式 (级联子菜单)
            ContextMenuFactory::buildOpenWithMenu(&menu, QStringList{path}, this);

            menu.addSeparator();

            // 5. 添加至收藏夹
            FavoriteService::instance().buildFavoriteAction(&menu, path, this);

            // 6. 添加至“库”
            LibraryService::instance().buildLibraryAction(&menu, QStringList{path}, this);

            menu.addSeparator();

            // 7. 复制（Ctrl+C）
            QAction* actCopy = menu.addAction(UiHelper::getIcon("copy", QColor("#EEEEEE"), 18), "复制（Ctrl+C）");
            connect(actCopy, &QAction::triggered, this, [path]() {
                ClipboardService::instance().copyItems({path});
            });

            // 8. 剪切（Ctrl+X）
            QAction* actCut = menu.addAction(UiHelper::getIcon("cut", QColor("#EEEEEE"), 18), "剪切（Ctrl+X）");
            connect(actCut, &QAction::triggered, this, [path]() {
                ClipboardService::instance().cutItems({path});
            });

            // 9. 复制名称
            ContextMenuFactory::buildCopyNameAction(&menu, {path}, this);

            // 10. 复制完整路径
            ContextMenuFactory::buildCopyPathAction(&menu, {path}, this);

            menu.addSeparator();

            // 11. 从历史中移除
            QAction* removeAct = menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "从历史中移除");
            connect(removeAct, &QAction::triggered, this, [path]() {
                HistoryService::instance().removeHistory(path);
            });

            menu.addSeparator();
        }
    }

    // 12. 清空历史 (项菜单末尾或空白处菜单)
    QAction* clearAct = menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "清空历史");
    connect(clearAct, &QAction::triggered, this, []() {
        HistoryService::instance().clearAllHistory();
    });

    menu.exec(m_treeWidget->mapToGlobal(pos));
}
=======
void HistoryPanel::onContextMenuRequested(const QPoint& pos) {
    QTreeWidgetItem* clickedItem = m_treeWidget->itemAt(pos);
    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    QList<QTreeWidgetItem*> selItems = m_treeWidget->selectedItems();
    if (clickedItem && !selItems.contains(clickedItem)) {
        m_treeWidget->setCurrentItem(clickedItem);
        selItems = m_treeWidget->selectedItems();
        if (selItems.isEmpty()) {
            selItems.append(clickedItem);
        }
    }

    QStringList selectedPaths;
    for (QTreeWidgetItem* item : selItems) {
        if (!item) continue;
        QString p = item->data(0, Qt::UserRole).toString();
        if (!p.isEmpty() && !selectedPaths.contains(p)) {
            selectedPaths.append(p);
        }
    }

    if (!selectedPaths.isEmpty()) {
        // 1. 用系统默认程序打开 (批量)
        QAction* actOpenDefault = menu.addAction(UiHelper::getIcon("launch", QColor("#EEEEEE"), 18), "用系统默认程序打开");
        connect(actOpenDefault, &QAction::triggered, this, [selectedPaths]() {
            for (const QString& path : selectedPaths) {
                if (QFileInfo::exists(path)) {
                    HistoryService::instance().recordFileAccess(path);
                    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
                }
            }
        });

        // 2. 在“资源管理器”中显示 (按目录分组批量打开，严格遵守分组确认规则)
        QAction* actShowInExp = menu.addAction(UiHelper::getIcon("folder_search", QColor("#EEEEEE"), 18), ContextMenuFactory::kTextShowInExplorer);
        connect(actShowInExp, &QAction::triggered, this, [this, selectedPaths]() {
            QMap<QString, QStringList> folderGroups;
            for (const QString& itemPath : selectedPaths) {
                if (itemPath.isEmpty()) continue;
                QFileInfo fi(itemPath);
                QString parentDir = fi.isDir() ? fi.dir().absolutePath() : fi.absolutePath();
                folderGroups[parentDir].append(itemPath);
            }

            if (folderGroups.size() > 5) {
                FramelessConfirmDialog dlg(
                    "打开资源管理器确认",
                    QString("本次操作将同时打开 %1 个资源管理器窗口，是否继续？").arg(folderGroups.size()),
                    FramelessConfirmDialog::OkCancel,
                    "warning",
                    QColor("#f1c40f"),
                    this
                );
                if (dlg.exec() != QDialog::Accepted) {
                    return;
                }
            }

            for (auto it = folderGroups.begin(); it != folderGroups.end(); ++it) {
                if (!it.value().isEmpty()) {
                    ShellHelper::openInExplorer(it.value().first());
                }
            }
        });

        // 3. 在 QuarkMeta 中显示 (单选可用，多选时置灰禁用)
        QAction* actShowInApp = menu.addAction(UiHelper::getIcon("folder_filled", QColor("#EEEEEE"), 18), "在 QuarkMeta 中显示");
        if (selectedPaths.size() == 1) {
            QString singlePath = selectedPaths.first();
            connect(actShowInApp, &QAction::triggered, this, [this, singlePath]() {
                emit requestLocateFile(singlePath);
            });
        } else {
            actShowInApp->setEnabled(false);
        }

        // 4. 打开方式 (级联子菜单，支持多选路径集合)
        ContextMenuFactory::buildOpenWithMenu(&menu, selectedPaths, this);

        menu.addSeparator();

        // 5. 添加至收藏夹 (批量添加)
        QAction* actAddFav = menu.addAction(UiHelper::getIcon("Favorites.svg", QColor("#EEEEEE"), 18), "添加至收藏夹");
        connect(actAddFav, &QAction::triggered, this, [this, selectedPaths]() {
            int addedCount = 0;
            for (const QString& path : selectedPaths) {
                if (!FavoriteService::instance().isFavorite(path)) {
                    if (FavoriteService::instance().addFavorite(path)) {
                        addedCount++;
                    }
                }
            }
            if (addedCount > 0) {
                ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目添加至收藏夹").arg(addedCount), 1500, QColor("#2ecc71"));
            } else {
                ToolTipOverlay::instance()->showToast("所选项目已全部在收藏夹中", 1500, QColor("#3498db"));
            }
        });

        // 6. 添加至“库” (批量)
        LibraryService::instance().buildLibraryAction(&menu, selectedPaths, this);

        menu.addSeparator();

        // 7. 复制（Ctrl+C）
        QAction* actCopy = menu.addAction(UiHelper::getIcon("copy", QColor("#EEEEEE"), 18), "复制（Ctrl+C）");
        connect(actCopy, &QAction::triggered, this, [selectedPaths]() {
            ClipboardService::instance().copyItems(selectedPaths);
        });

        // 8. 剪切（Ctrl+X）
        QAction* actCut = menu.addAction(UiHelper::getIcon("cut", QColor("#EEEEEE"), 18), "剪切（Ctrl+X）");
        connect(actCut, &QAction::triggered, this, [selectedPaths]() {
            ClipboardService::instance().cutItems(selectedPaths);
        });

        // 9. 复制名称 (批量)
        ContextMenuFactory::buildCopyNameAction(&menu, selectedPaths, this);

        // 10. 复制完整路径 (批量)
        ContextMenuFactory::buildCopyPathAction(&menu, selectedPaths, this);

        menu.addSeparator();

        // 11. 从历史中移除 (批量)
        QAction* removeAct = menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "从历史中移除");
        connect(removeAct, &QAction::triggered, this, [this, selectedPaths, selItems]() {
            for (const QString& path : selectedPaths) {
                HistoryService::instance().removeHistory(path);
            }
            for (QTreeWidgetItem* itm : selItems) {
                delete itm;
            }
        });

        menu.addSeparator();
    }

    // 12. 清空历史
    QAction* clearAct = menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "清空历史");
    connect(clearAct, &QAction::triggered, this, []() {
        HistoryService::instance().clearAllHistory();
    });

    menu.exec(m_treeWidget->mapToGlobal(pos));
}
>>>>>>> REPLACE

升级拖拽事件为批量多选拖拽：
<<<<<<< SEARCH
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->buttons() & Qt::LeftButton) {
                QTreeWidgetItem* item = m_treeWidget->itemAt(mouseEvent->pos());
                if (item) {
                    QString path = item->data(0, Qt::UserRole).toString();
                    if (!path.isEmpty()) {
                        ToolTipOverlay::hideTip();
                        QDrag* drag = new QDrag(m_treeWidget);
                        QMimeData* mimeData = new QMimeData();
                        mimeData->setUrls({QUrl::fromLocalFile(path)});
                        drag->setMimeData(mimeData);
                        drag->exec(Qt::CopyAction);
                        return true;
                    }
                }
            } else {
=======
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->buttons() & Qt::LeftButton) {
                QTreeWidgetItem* hitItem = m_treeWidget->itemAt(mouseEvent->pos());
                if (hitItem) {
                    QList<QTreeWidgetItem*> selList = m_treeWidget->selectedItems();
                    if (!selList.contains(hitItem)) {
                        selList = {hitItem};
                    }
                    QList<QUrl> urls;
                    for (QTreeWidgetItem* itm : selList) {
                        QString p = itm->data(0, Qt::UserRole).toString();
                        if (!p.isEmpty() && QFileInfo::exists(p)) {
                            urls.append(QUrl::fromLocalFile(p));
                        }
                    }
                    if (!urls.isEmpty()) {
                        ToolTipOverlay::hideTip();
                        QDrag* drag = new QDrag(m_treeWidget);
                        QMimeData* mimeData = new QMimeData();
                        mimeData->setUrls(urls);
                        drag->setMimeData(mimeData);
                        drag->exec(Qt::CopyAction);
                        return true;
                    }
                }
            } else {
>>>>>>> REPLACE

---

### 3.5 `QuarkMeta_Source/src/ui/MarkedPanel.cpp` 标记拖出语义注入

添加必要的头文件包含：
<<<<<<< SEARCH
#include "../util/ShellHelper.h"
#include "../core/NavigationService.h"
#include "controllers/ContextMenuFactory.h"
=======
#include "../util/ShellHelper.h"
#include "../core/NavigationService.h"
#include "controllers/ContextMenuFactory.h"
#include "ViewDragDropHelper.h"
>>>>>>> REPLACE

在 `MarkedPanel::initUi` 中开启拖拽支持：
<<<<<<< SEARCH
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeWidget->setAcceptDrops(true);
=======
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeWidget->setDragEnabled(true);
    m_treeWidget->setAcceptDrops(true);
>>>>>>> REPLACE

在 `MarkedPanel::eventFilter` 中拦截鼠标移动触发 Drag 并设置脱管源：
<<<<<<< SEARCH
bool MarkedPanel::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_treeWidget || (m_treeWidget && watched == m_treeWidget->viewport())) {
        if (event->type() == QEvent::DragEnter) {
=======
bool MarkedPanel::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_treeWidget || (m_treeWidget && watched == m_treeWidget->viewport())) {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->buttons() & Qt::LeftButton) {
                QTreeWidgetItem* hitItem = m_treeWidget->itemAt(mouseEvent->pos());
                if (hitItem) {
                    QString path = hitItem->data(0, Qt::UserRole).toString();
                    if (!path.isEmpty() && QFileInfo::exists(path)) {
                        ToolTipOverlay::hideTip();
                        ViewDragDropHelper::setLastDragSource(ViewDragDropHelper::DragSourcePanel::Marked);
                        QDrag* drag = new QDrag(m_treeWidget);
                        QMimeData* mimeData = new QMimeData();
                        mimeData->setUrls({QUrl::fromLocalFile(path)});
                        drag->setMimeData(mimeData);
                        drag->exec(Qt::CopyAction);
                        ViewDragDropHelper::clearLastDragSource();
                        return true;
                    }
                }
            }
        } else if (event->type() == QEvent::DragEnter) {
>>>>>>> REPLACE

---

### 3.6 `QuarkMeta_Source/src/ui/controllers/ContentFileOpsHandler.cpp` 拖拽移出/脱管响应处理

添加必要的头文件包含：
<<<<<<< SEARCH
#include "../../util/DiskIoService.h"
#include "../../meta/LibraryService.h"

#include <QDir>
=======
#include "../../util/DiskIoService.h"
#include "../../meta/LibraryService.h"
#include "../../meta/FavoriteService.h"
#include "../../meta/MarkedFolderService.h"

#include <QDir>
>>>>>>> REPLACE

在主内容面板的 `ContentFileOpsHandler::onPathsDropped` 顶端拦截脱管意图：
<<<<<<< SEARCH
void ContentFileOpsHandler::onPathsDropped(const QStringList& paths, const QModelIndex& targetIndex, const QString& targetDirOverride, Qt::DropAction action) {
    if (!m_panel || paths.isEmpty()) return;

    // 1. 目标窗格分类视图拖放重定向逻辑
=======
void ContentFileOpsHandler::onPathsDropped(const QStringList& paths, const QModelIndex& targetIndex, const QString& targetDirOverride, Qt::DropAction action) {
    if (!m_panel || paths.isEmpty()) return;

    // 🚀【拖拽移出/脱管交互语义】：若拖拽源来自侧边栏（收藏夹、库分类、标记），执行解绑移出而非磁盘文件操作
    auto dragSource = ViewDragDropHelper::lastDragSourcePanel();
    if (dragSource == ViewDragDropHelper::DragSourcePanel::Favorite) {
        int removedCount = 0;
        for (const QString& p : paths) {
            if (FavoriteService::instance().removeFavorite(p)) {
                removedCount++;
            }
        }
        if (removedCount > 0) {
            ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目从收藏夹中移出").arg(removedCount), 2000, QColor("#3498db"));
        }
        return;
    }

    if (dragSource == ViewDragDropHelper::DragSourcePanel::Library) {
        int catId = ViewDragDropHelper::lastDragSourceCategoryId();
        if (catId > 0) {
            LibraryService::instance().removePathsFromCategory(catId, paths);
            ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目从分类中解除归类").arg(paths.size()), 2000, QColor("#3498db"));
        } else {
            LibraryService::instance().removePathsFromLibrary(paths);
            ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目从“库”中移出").arg(paths.size()), 2000, QColor("#3498db"));
        }
        return;
    }

    if (dragSource == ViewDragDropHelper::DragSourcePanel::Marked) {
        int unmarkedCount = 0;
        for (const QString& p : paths) {
            if (MarkedFolderService::instance().isMarked(p)) {
                MarkedFolderService::instance().removeMarked(p);
                unmarkedCount++;
            }
        }
        if (unmarkedCount > 0) {
            ToolTipOverlay::instance()->showToast(QString("已取消 %1 个目标文件夹标记").arg(unmarkedCount), 2000, QColor("#3498db"));
        }
        return;
    }

    // 1. 目标窗格分类视图拖放重定向逻辑
>>>>>>> REPLACE

---

### 3.7 `QuarkMeta_Source/src/ui/NavPanel.cpp` 目录导航面板支持接收侧边栏脱管拖放

添加必要的头文件包含：
<<<<<<< SEARCH
#include "DropTreeView.h"
#include "ContentPanel.h"
=======
#include "DropTreeView.h"
#include "ViewDragDropHelper.h"
#include "ContentPanel.h"
>>>>>>> REPLACE

在 `NavPanel` 中开启对树视图的拖放接收，并在拖放释放时处理脱管逻辑：
<<<<<<< SEARCH
    m_treeView->setDragDropMode(QAbstractItemView::DragOnly);
=======
    m_treeView->setAcceptDrops(true);
    m_treeView->setDragDropMode(QAbstractItemView::DragDrop);
    connect(m_treeView, &DropTreeView::pathsDropped, this, [this](const QStringList& paths, const QModelIndex& targetIndex, Qt::DropAction action) {
        Q_UNUSED(targetIndex);
        Q_UNUSED(action);
        if (paths.isEmpty()) return;

        auto dragSource = ViewDragDropHelper::lastDragSourcePanel();
        if (dragSource == ViewDragDropHelper::DragSourcePanel::Favorite) {
            int removedCount = 0;
            for (const QString& p : paths) {
                if (FavoriteService::instance().removeFavorite(p)) removedCount++;
            }
            if (removedCount > 0) {
                ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目从收藏夹中移出").arg(removedCount), 2000, QColor("#3498db"));
            }
            return;
        }

        if (dragSource == ViewDragDropHelper::DragSourcePanel::Library) {
            int catId = ViewDragDropHelper::lastDragSourceCategoryId();
            if (catId > 0) {
                LibraryService::instance().removePathsFromCategory(catId, paths);
                ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目从分类中解除归类").arg(paths.size()), 2000, QColor("#3498db"));
            } else {
                LibraryService::instance().removePathsFromLibrary(paths);
                ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目从“库”中移出").arg(paths.size()), 2000, QColor("#3498db"));
            }
            return;
        }

        if (dragSource == ViewDragDropHelper::DragSourcePanel::Marked) {
            int unmarkCount = 0;
            for (const QString& p : paths) {
                if (MarkedFolderService::instance().isMarked(p)) {
                    MarkedFolderService::instance().removeMarked(p);
                    unmarkCount++;
                }
            }
            if (unmarkCount > 0) {
                ToolTipOverlay::instance()->showToast(QString("已取消 %1 个目标文件夹标记").arg(unmarkCount), 2000, QColor("#3498db"));
            }
            return;
        }
    });
>>>>>>> REPLACE

---

## 四、构建与运行时验收标准 (Build & Verification Steps)

### 4.1 编译指令
```bash
cmake --build ArcMeta_Source/ArcMeta_Build/x64-release --config Release
```

### 4.2 运行时验收用例
1. **历史多选与右键验证**：
   - 打开侧边栏“历史”选项卡；
   - 按住 Ctrl 或 Shift 点击多个历史项目，所有选中的项目均显示蓝色高亮；
   - 鼠标右键点击其中一个选中项，弹出上下文菜单；
   - 点击“用系统默认程序打开”：所有选中的文件均被依次正常打开；
   - 点击“在“资源管理器”中显示”：根据文件所属物理目录分组打开资源管理器窗口；
   - 点击“添加至收藏夹”：所有选中的有效路径被批量加入收藏夹，左侧收藏夹即时刷新；
   - 点击“从历史中移除”：选中的所有项目从 SQLite 表和 UI 树中一次性移除；
2. **侧边栏拖拽移出脱管验证**：
   - 选中收藏夹中的某一项，拖拽到主内容面板的空白处或文件列表上方释放：
     - 磁盘上的物理原文件完好无损，没有发生移动或复制；
     - 收藏夹中的该项被成功移除；
     - 弹出 Toast 提示：“已将 1 个项目从收藏夹中移出”。
   - 选中某个已标记的文件夹，拖拽到目录导航树或内容面板释放：
     - 物理文件无变化；
     - 标记状态被取消，右侧徽标与 MarkedPanel 同步更新。

---

## 五、官方 API 复用与防另起炉灶 (Official API Reuse)
1. **资源管理器展示**：严格复用 `ContextMenuFactory::kTextShowInExplorer` 与 `ShellHelper::openInExplorer`，复用目录分组阈值（>5 弹窗确认）。
2. **复制文件名/路径**：严格复用 `ContextMenuFactory::buildCopyNameAction` 与 `ContextMenuFactory::buildCopyPathAction`。
3. **打开方式**：严格复用 `ContextMenuFactory::buildOpenWithMenu` 批量接口。
4. **历史移除**：严格复用 `HistoryService::instance().removeHistory` 与 `HistoryDao`。
5. **收藏夹操作**：严格复用 `FavoriteService::instance().addFavorite` 与 `removeFavorite`。
6. **库操作**：严格复用 `LibraryService::instance().buildLibraryAction` 与 `removePathsFromCategory`。

---

## 六、数据单一事实来源声明表 (Data SSOT Declaration Table)

| 数据项 | 唯一真理源 (SSOT) | 变更通知通道 | 同步/刷新方向 |
| :--- | :--- | :--- | :--- |
| 文件访问历史 | SQLite `file_history` 表 (`HistoryDao`) | `HistoryService::historyChanged` | DB -> `HistoryPanel::m_treeWidget` |
| 收藏夹列表 | SQLite `favorites` 表 (`FavoriteDao`) | `FavoriteService::favoriteChanged` | DB -> `FavoritePanel::m_favoriteModel` |
| 标记目的地 | SQLite `marked_folders` 表 (`MarkedFolderDao`) | `MarkedFolderService::markedFoldersChanged` | DB -> `MarkedPanel::m_treeWidget` |
| 库分类归属 | SQLite `library_items` 表 (`LibraryDao`) | `LibraryService::libraryChanged` | DB -> `LibraryPanel::m_model` |

---

## 七、接口签名与访问级别一致性核查 (Interface Consistency Check)

1. `HistoryService::removeHistory(const QString& path)`：`public`，签名一致；
2. `FavoriteService::addFavorite(const QString& path, int parentId)`：`public`，签名一致；
3. `FavoriteService::removeFavorite(const QString& path)`：`public`，签名一致；
4. `LibraryService::buildLibraryAction(QMenu* parentMenu, const QStringList& targetPaths, QObject* receiver)`：`public`，签名一致；
5. `ContextMenuFactory::buildOpenWithMenu(QMenu* parentMenu, const QStringList& filePaths, QWidget* parentWidget)`：`public`，签名一致；
6. `ContextMenuFactory::buildCopyNameAction(QMenu* menu, const QStringList& paths, QObject* receiver)`：`public`，签名一致；
7. `ContextMenuFactory::buildCopyPathAction(QMenu* menu, const QStringList& paths, QObject* receiver)`：`public`，签名一致；
8. `ViewDragDropHelper::setLastDragSource(DragSourcePanel source, int categoryId)`：`public`，新规统一接口；
9. `ViewDragDropHelper::lastDragSourcePanel()`：`public`，新规统一接口。

---

## 八、头文件包含链与类型完整性检查 (Header Chain Check)

1. `QuarkMeta_Source/src/ui/HistoryPanel.cpp`：
   - 显式 `#include "../util/ShellHelper.h"` 与 `#include "dialogs/FramelessConfirmDialog.h"`，类型全部完整闭合。
2. `QuarkMeta_Source/src/ui/controllers/ContentFileOpsHandler.cpp`：
   - 显式 `#include "../../meta/FavoriteService.h"` 与 `#include "../../meta/MarkedFolderService.h"`，类型全部完整闭合。
3. `QuarkMeta_Source/src/ui/NavPanel.cpp`：
   - 显式 `#include "ViewDragDropHelper.h"`，类型全部完整闭合。
4. `QuarkMeta_Source/src/ui/MarkedPanel.cpp`：
   - 显式 `#include "ViewDragDropHelper.h"`，类型全部完整闭合。

---

## 九、合规与质量自检核对表 (Compliance Self-Check Table)

| 检查项 | 状态 | 规范与位置说明 |
| :--- | :--- | :--- |
| 1. 单一事实来源 (SSOT) 遵守 | 满足 | 历史记录归属于 HistoryService，收藏夹归属于 FavoriteService，拖拽源归口于 ViewDragDropHelper |
| 2. 避免另起炉灶 (Search First) | 满足 | 复用既有 ContextMenuFactory、LibraryService 等所有标准 API |
| 3. 语言规范（全中文） | 满足 | 实施方案全文除标识符外全部采用规范中文 |
| 4. 实施方案物理隔离 | 满足 | 存放于 `Implementation Plan/` 目录下 |
| 5. 命名与映射规范 | 满足 | 命名清晰描述业务功能与主导类 |
| 6. 代码替换块 Git Merge Diff 规范 | 满足 | 采用标准 SEARCH / REPLACE 块，原代码唯一匹配 |
| 7. 接口签名冻结与只读契约 | 满足 | 不修改任何既有公共 API 签名 |
| 8. 访问级别一致性检查 | 满足 | 所有调用成员均为 public 级别 |
| 9. 内存与指针生命周期安全 | 满足 | 无悬空指针，无 Lambda 循环强引用 |
| 10. 交互设计零篡改 | 满足 | 保留既有右键所有选项，仅提升为多选批处理 |

---

## 十、行为零篡改与数值零篡改声明表 (Behavior Preservation Table)

| 既有行为/参数 | 原始设计 | 本次重构设计 | 结论 |
| :--- | :--- | :--- | :--- |
| 单击历史项目交互 | 仅处理高亮选中，不触发定位 | 保持仅处理高亮选中，支持多选 | 100% 行为零篡改 |
| 右键菜单图标与颜色 | `#EEEEEE` 统一中性色图标 | 保持 `#EEEEEE` 统一图标样式 | 100% 数值零篡改 |
| 清空历史逻辑 | 调用 `HistoryService::clearAllHistory()` | 保持完全一致 | 100% 行为零篡改 |
| 磁盘文件物理安全性 | 外部文件拖拽执行复制/移动 | 侧边栏拖拽仅解绑脱管，严禁操作磁盘文件 | 安全加固 |
