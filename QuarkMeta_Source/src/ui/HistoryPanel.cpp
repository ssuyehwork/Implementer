#include "HistoryPanel.h"
#include "UiHelper.h"
#include "ToolTipOverlay.h"
#include "controllers/ContextMenuFactory.h"
#include "../core/NavigationService.h"
#include "../core/ClipboardService.h"
#include "../meta/FavoriteService.h"
#include "../meta/HistoryService.h"
#include "../meta/LibraryService.h"
#include "../util/ThumbnailPipelineService.h"
#include "../util/ColorPaletteEngine.h"
#include "../util/ShellHelper.h"
#include "dialogs/FramelessConfirmDialog.h"
#include <QDesktopServices>
#include <QFileInfo>
#include <QDir>
#include <QFileIconProvider>
#include <QDrag>
#include <QMimeData>
#include <QUrl>
#include <QMouseEvent>
#include <QHeaderView>

namespace QuarkMeta {

HistoryPanel::HistoryPanel(QWidget* parent) : QWidget(parent) {
    initUi();

    connect(&HistoryService::instance(), &HistoryService::historyItemAdded, this, &HistoryPanel::onHistoryItemAdded);
    connect(&HistoryService::instance(), &HistoryService::historyItemRemoved, this, &HistoryPanel::onHistoryItemRemoved);
    connect(&HistoryService::instance(), &HistoryService::historyCleared, this, &HistoryPanel::onHistoryCleared);
}

void HistoryPanel::initUi() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setObjectName("HistoryTreeWidget");
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setIndentation(0);
    m_treeWidget->setIconSize(QSize(16, 16));
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeWidget->setDragEnabled(true);
    m_treeWidget->setMouseTracking(true);

    m_mainLayout->addWidget(m_treeWidget);

    connect(m_treeWidget, &QTreeWidget::itemClicked, this, &HistoryPanel::onItemClicked);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &HistoryPanel::onContextMenuRequested);

    if (m_treeWidget->viewport()) {
        m_treeWidget->viewport()->installEventFilter(this);
    }
}

void HistoryPanel::loadHistory() {
    if (m_isLoaded) return;
    m_isLoaded = true;

    m_treeWidget->clear();
    auto list = HistoryService::instance().getAllHistory();
    for (const auto& rec : list) {
        if (QFile::exists(rec.filePath)) {
            QTreeWidgetItem* item = createHistoryItem(rec.filePath);
            m_treeWidget->addTopLevelItem(item);
        }
    }
}

QTreeWidgetItem* HistoryPanel::createHistoryItem(const QString& path) {
    QFileInfo fi(path);
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, fi.fileName());
    item->setData(0, Qt::UserRole, path);

    // 普通文件优先展示系统/通用图标
    QFileIconProvider provider;
    QIcon sysIcon = provider.icon(fi);
    if (!sysIcon.isNull()) {
        item->setIcon(0, sysIcon);
    } else {
        item->setIcon(0, UiHelper::getIcon("file", "#888888", 16));
    }

    // 图形图片类文件异步提取缩略图回填
    if (ColorPaletteEngine::isGraphicsFile(path)) {
        updateItemThumbnail(item, path);
    }

    return item;
}

void HistoryPanel::updateItemThumbnail(QTreeWidgetItem* item, const QString& path) {
    ThumbnailPipelineService::instance().loadBatchAsync({path}, 32, [this, item](const QString&, const QPixmap& pix) {
        if (!pix.isNull() && item && item->treeWidget()) {
            item->setIcon(0, QIcon(pix));
        }
    });
}

void HistoryPanel::onItemClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(item);
    Q_UNUSED(column);
    // 单击仅由 QTreeWidget 处理选中高亮，不发射定位信号以彻底杜绝额外目录载入与缩略图解析开销
}

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

        // 3. 在 QuarkMeta 中显示 (单选可用，多选时置灰禁用，SSOT 调用 NavigationService::locateItem)
        QAction* actShowInApp = menu.addAction(UiHelper::getIcon("folder_filled", QColor("#EEEEEE"), 18), "在 QuarkMeta 中显示");
        if (selectedPaths.size() == 1) {
            QString singlePath = selectedPaths.first();
            connect(actShowInApp, &QAction::triggered, this, [singlePath]() {
                NavigationService::instance().locateItem(singlePath);
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
        connect(removeAct, &QAction::triggered, this, [selectedPaths]() {
            // 只需调用 Service 移除数据，Service 发射的 historyItemRemoved 信号会自动清理 UI 节点
            // 彻底消除对 QTreeWidgetItem 指针的野指针二次释放 (Double Free)
            for (const QString& path : selectedPaths) {
                HistoryService::instance().removeHistory(path);
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

void HistoryPanel::onHistoryItemAdded(const QString& path) {
    if (!m_isLoaded) return;

    // 检查是否存在现有 Item
    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = m_treeWidget->topLevelItem(i);
        if (item->data(0, Qt::UserRole).toString() == path) {
            // 顶置现有 Item
            m_treeWidget->takeTopLevelItem(i);
            m_treeWidget->insertTopLevelItem(0, item);
            return;
        }
    }

    // 新建 Item 插入第 0 行
    QTreeWidgetItem* newItem = createHistoryItem(path);
    m_treeWidget->insertTopLevelItem(0, newItem);

    // 保持最多 100 项
    if (m_treeWidget->topLevelItemCount() > 100) {
        delete m_treeWidget->takeTopLevelItem(m_treeWidget->topLevelItemCount() - 1);
    }
}

void HistoryPanel::onHistoryItemRemoved(const QString& path) {
    if (!m_isLoaded) return;
    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = m_treeWidget->topLevelItem(i);
        if (item->data(0, Qt::UserRole).toString() == path) {
            delete m_treeWidget->takeTopLevelItem(i);
            break;
        }
    }
}

void HistoryPanel::onHistoryCleared() {
    m_treeWidget->clear();
    m_isLoaded = false;
    loadHistory();
}

void HistoryPanel::filterTreeNodes(const QString& keyword) {
    QString kw = keyword.trimmed().toLower();
    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = m_treeWidget->topLevelItem(i);
        if (kw.isEmpty()) {
            item->setHidden(false);
        } else {
            bool match = item->text(0).toLower().contains(kw) || item->data(0, Qt::UserRole).toString().toLower().contains(kw);
            item->setHidden(!match);
        }
    }
}

bool HistoryPanel::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_treeWidget->viewport()) {
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
                QTreeWidgetItem* item = m_treeWidget->itemAt(mouseEvent->pos());
                if (item) {
                    QString path = item->data(0, Qt::UserRole).toString();
                    if (!path.isEmpty()) {
                        QString nativePath = QDir::toNativeSeparators(path);
                        if (m_lastHoveredPath != nativePath) {
                            m_lastHoveredPath = nativePath;
                            ToolTipOverlay::instance()->showText(QCursor::pos(), nativePath, 0);
                        }
                    } else {
                        m_lastHoveredPath.clear();
                        ToolTipOverlay::hideTip();
                    }
                } else {
                    m_lastHoveredPath.clear();
                    ToolTipOverlay::hideTip();
                }
            }
        } else if (event->type() == QEvent::Leave) {
            m_lastHoveredPath.clear();
            ToolTipOverlay::hideTip();
        }
    }
    return QWidget::eventFilter(watched, event);
}

} // namespace QuarkMeta
