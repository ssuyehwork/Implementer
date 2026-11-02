#include "NavPanel.h"
#include "controllers/ContentKeyHandler.h"
#include <QKeyEvent>
#include "UiHelper.h"
#include "ShellIconManager.h"
#include "Logger.h"
#include "TreeItemDelegate.h"
#include "DropTreeView.h"
#include "ContentPanel.h"
#include "ToolTipOverlay.h"
#include "../core/AppConfig.h"
#include "../core/NavigationHistoryService.h"
#include "../core/TrashService.h"
#include "../meta/FavoriteDao.h"
#include "../meta/FavoriteService.h"
#include "../meta/MarkedFolderService.h"
#include "../meta/LibraryService.h"
#include "../core/ClipboardService.h"
#include "../core/NavigationService.h"
#include "TabBarWidget.h"
#include "controllers/ContextMenuFactory.h"
#include <QStyleOptionTab>
#include <QPainter>
#include <QHeaderView>
#include <QScrollBar>
#include <QLabel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QStandardPaths>
#include <QTimer>
#include <QPushButton>
#include <QPointer>
#include <QMenu>
#include <QClipboard>
#include <QtConcurrent>
#include <QApplication>

namespace QuarkMeta {

/**
 * @brief 构造函数，设置面板属性
 */
NavPanel::NavPanel(QWidget* parent)
    : QFrame(parent) {
    setObjectName("SidebarContainer");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(230);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    setContextMenuPolicy(Qt::CustomContextMenu);
    initUi();
}

/**
 * @brief 初始化 UI 组件
 */
void NavPanel::deferredInit() {
    if (m_model && m_model->rowCount() > 0) {
        return;
    }

    // 1. 桌面入口 (使用 SVG 语义图标替代原生图标)
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QIcon desktopIcon = UiHelper::getIcon("home", QColor("#3498db"), 18);
    QStandardItem* desktopItem = new QStandardItem(desktopIcon, "桌面");
    desktopItem->setData(desktopPath, Qt::UserRole + 1);
    desktopItem->appendRow(new QStandardItem("Loading..."));
    m_model->appendRow(desktopItem);

    // 2. 此电脑入口
    QIcon computerIcon = UiHelper::getIcon("monitor", QColor("#3498db"), 18);
    QStandardItem* computerItem = new QStandardItem(computerIcon, "此电脑");
    computerItem->setData("computer://", Qt::UserRole + 1);
    m_model->appendRow(computerItem);

    // 3. 磁盘列表 (先填充基础文字路径，0 毫秒卡顿)
    const auto drives = QDir::drives();
    for (const QFileInfo& drive : drives) {
        QString driveName = drive.absolutePath();
        QStandardItem* driveItem = new QStandardItem(driveName);
        driveItem->setData(driveName, Qt::UserRole + 1);
        driveItem->appendRow(new QStandardItem("Loading..."));
        m_model->appendRow(driveItem);
    }

    // 主线程分批次补齐磁盘图标
    QTimer::singleShot(0, [this, drives]() {
        for (int i = 0; i < drives.size(); ++i) {
            if (i + 2 < m_model->rowCount()) {
                QIcon driveIcon = UiHelper::getIcon("hard_drive", QColor("#95a5a6"), 18);
                m_model->item(i + 2)->setIcon(driveIcon);
            }
        }
    });

    // 4. 最近访问 (固定主节点，在所有磁盘正下方)
    QIcon recentIcon = UiHelper::getIcon("clock_history", QColor("#3498db"), 18);
    m_recentRootItem = new QStandardItem(recentIcon, "最近访问");
    m_recentRootItem->setData("recent_root", Qt::UserRole + 1);
    m_model->appendRow(m_recentRootItem);

    // 异步探测填充历史记录，防止慢速物理/网络驱动器阻塞启动
    updateRecentVisitedList();
    if (m_treeView && m_recentRootItem->index().isValid()) {
        m_treeView->expand(m_recentRootItem->index());
    }

    // 5. 回收站 (固定主节点)
    QIcon trashIcon = UiHelper::getIcon("delete_forever", QColor("#e81123"), 18);
    QStandardItem* trashItem = new QStandardItem(trashIcon, "回收站");
    trashItem->setData("trash_root", Qt::UserRole + 1);
    m_model->appendRow(trashItem);
}

void NavPanel::setFocusHighlight(bool visible) {
    Q_UNUSED(visible);
}

void NavPanel::initUi() {
    QWidget* header = new QWidget(this);
    header->setObjectName("ContainerHeader");
    header->setFixedHeight(32);

    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(5, 0, 5, 0);
    headerLayout->setSpacing(0);

    // 侧边栏专用选项卡：直接掌控绘制，彻底杜绝 QSS 下样式代理被拦截失效问题，硬锁 7 像素间距
    class SidebarTabBar : public QTabBar {
    public:
        using QTabBar::QTabBar;
    protected:
        QSize tabSizeHint(int index) const override {
            QSize s = QTabBar::tabSizeHint(index);
            if (!tabIcon(index).isNull() && !tabText(index).isEmpty()) {
                s.setWidth(s.width() - 5);
            }
            return s;
        }

        void paintEvent(QPaintEvent* event) override {
            Q_UNUSED(event);
            QPainter painter(this);
            QStyleOptionTab opt;
            for (int i = 0; i < count(); ++i) {
                initStyleOption(&opt, i);
                // 1. 保留 QSS 样式表对背景色、边框、选态与悬停底色的完全渲染
                style()->drawControl(QStyle::CE_TabBarTabShape, &opt, &painter, this);

                // 2. 自定义精确绘制图标与文字，硬锁间隙为 7px
                QRect tr = opt.rect;
                QSize iconSize = opt.iconSize.isValid() ? opt.iconSize : QSize(16, 16);
                int iconWidth = opt.icon.isNull() ? 0 : iconSize.width();
                int textWidth = opt.fontMetrics.horizontalAdvance(opt.text);
                int spacing = (iconWidth > 0 && !opt.text.isEmpty()) ? 7 : 0;
                int totalContentWidth = iconWidth + spacing + textWidth;
                int startX = tr.left() + (tr.width() - totalContentWidth) / 2;

                if (iconWidth > 0) {
                    int iconY = tr.top() + (tr.height() - iconSize.height()) / 2;
                    QRect iconRect(startX, iconY, iconSize.width(), iconSize.height());
                    opt.icon.paint(&painter, iconRect, Qt::AlignCenter);
                }

                QRect textRect(startX + iconWidth + spacing, tr.top(), textWidth + 2, tr.height());
                painter.save();
                painter.setFont(font());
                QColor textColor = (opt.state & QStyle::State_Selected) ? QColor("#FFFFFF") : ((opt.state & QStyle::State_MouseOver) ? QColor("#CCCCCC") : QColor("#AAAAAA"));
                painter.setPen(textColor);
                painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, opt.text);
                painter.restore();
            }
        }
    };

    m_tabBar = new SidebarTabBar(header);
    m_tabBar->setObjectName("SidebarTabBar");
    m_tabBar->setDrawBase(false);
    m_tabBar->setIconSize(QSize(16, 16));
    m_tabBar->addTab(UiHelper::getIcon("list_ul", "#888888", 16), "目录导航");
    m_tabBar->addTab(UiHelper::getIcon("clock", "#888888", 16), "历史");
    m_tabBar->setCursor(Qt::PointingHandCursor);

    headerLayout->addWidget(m_tabBar);
    headerLayout->addStretch();
    m_mainLayout->addWidget(header);

    m_stackedWidget = new QStackedWidget(this);

    // Page 0: 目录导航
    QWidget* navPage = new QWidget(m_stackedWidget);
    QVBoxLayout* navPageLayout = new QVBoxLayout(navPage);
    navPageLayout->setContentsMargins(0, 0, 0, 0);
    navPageLayout->setSpacing(0);

    m_treeView = new DropTreeView(navPage);
    m_treeView->setObjectName("NavTreeView");
    m_treeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_treeView->setHeaderHidden(true);
    if (m_treeView->header()) {
        m_treeView->header()->setStretchLastSection(true);
        m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    }
    m_treeView->setAnimated(true);
    m_treeView->setIndentation(20);
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setExpandsOnDoubleClick(true);
    m_treeView->setDragEnabled(true);
    m_treeView->setDragDropMode(QAbstractItemView::DragOnly);
    m_treeView->setItemDelegate(new TreeItemDelegate(this, false));
    m_treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_model = new QStandardItemModel(this);
    m_treeView->setModel(m_model);
    navPageLayout->addWidget(m_treeView);

    // Page 1: 历史记录
    m_historyPanel = new HistoryPanel(m_stackedWidget);

    m_stackedWidget->addWidget(navPage);
    m_stackedWidget->addWidget(m_historyPanel);

    m_mainLayout->addWidget(m_stackedWidget, 1);

    connect(m_tabBar, &QTabBar::currentChanged, this, [this](int index) {
        m_stackedWidget->setCurrentIndex(index);
        if (index == 1 && m_historyPanel) {
            m_historyPanel->loadHistory();
        }
    });

    connect(m_historyPanel, &HistoryPanel::requestLocateFile, this, &NavPanel::requestLocateFile);

    QString arrowRight = UiHelper::getSvgTempFilePath("chevron_right", QColor("#378ADD"));
    QString arrowDown = UiHelper::getSvgTempFilePath("chevron_down", QColor("#378ADD"));
    QString treeStyle = QString(
        "QTreeView#NavTreeView::branch:has-children:closed { image: url(\"%1\"); }"
        "QTreeView#NavTreeView::branch:has-children:open { image: url(\"%2\"); }"
    ).arg(arrowRight, arrowDown);
    m_treeView->setStyleSheet(treeStyle);

    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->installEventFilter(this);
    if (m_treeView->viewport()) {
        m_treeView->viewport()->installEventFilter(this);
    }
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &NavPanel::onTreeContextMenu);

    connect(m_treeView, &QTreeView::expanded, this, &NavPanel::onItemExpanded);
    connect(m_treeView, &QTreeView::clicked, this, &NavPanel::onTreeClicked);
    connect(&NavigationHistoryService::instance(), &NavigationHistoryService::historyChanged, this, &NavPanel::updateRecentVisitedList);
}

void NavPanel::updateRecentVisitedList() {
    if (!m_recentRootItem) return;

    QStringList history = NavigationHistoryService::instance().getHistory();
    if (history.isEmpty()) {
        m_recentRootItem->removeRows(0, m_recentRootItem->rowCount());
        return;
    }

    struct RecentItemData {
        QString path;
        QString displayName;
        QIcon icon;
    };

    // 🚀【物理并发加速】：历史路径存在性检测与图标提取异步化，杜绝局域网/失效U盘拖死主线程
    QPointer<NavPanel> weakThis(this);
    (void)QtConcurrent::run([weakThis, history]() {
        QList<RecentItemData> validItems;
        QSet<QString> seenPaths;
        int count = 0;

        for (const QString& path : history) {
            if (count >= 14) break;
            if (path.isEmpty() || path == "computer://" || path.startsWith("trash") || path.startsWith("分类: ")) continue;

            QString normalizedKey = QDir::cleanPath(path).toLower();
            if (seenPaths.contains(normalizedKey)) continue;
            seenPaths.insert(normalizedKey);

            QFileInfo info(path);
            if (!info.exists() || !info.isDir()) continue;

            QString displayName = info.fileName();
            if (displayName.isEmpty()) {
                displayName = QDir::toNativeSeparators(path);
            }

            QIcon icon = ShellIconManager::getFileIcon(path, 18);
            if (icon.isNull()) {
                icon = UiHelper::getIcon("folder_filled", QColor("#3498db"), 18);
            }

            validItems.append({path, displayName, icon});
            count++;
        }

        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis, validItems]() {
            if (!weakThis || !weakThis->m_recentRootItem) return;

            weakThis->m_recentRootItem->removeRows(0, weakThis->m_recentRootItem->rowCount());
            for (const auto& item : validItems) {
                QStandardItem* child = new QStandardItem(item.icon, item.displayName);
                child->setData(item.path, Qt::UserRole + 1);
                child->setData(QDir::toNativeSeparators(item.path), Qt::UserRole + 2);
                weakThis->m_recentRootItem->appendRow(child);
            }

            if (!weakThis->m_currentPath.isEmpty()) {
                weakThis->selectPath(weakThis->m_currentPath);
            }
        }, Qt::QueuedConnection);
    });
}

void NavPanel::setRootPath(const QString& path) {
    Q_UNUSED(path);
}

QStandardItem* NavPanel::findItemByData(QStandardItem* parent, const QString& targetData) {
    if (!parent) return nullptr;
    for (int i = 0; i < parent->rowCount(); ++i) {
        QStandardItem* child = parent->child(i);
        if (!child) continue;
        if (child->data(Qt::UserRole + 1).toString() == targetData) {
            return child;
        }
        QStandardItem* subMatch = findItemByData(child, targetData);
        if (subMatch) {
            return subMatch;
        }
    }
    return nullptr;
}

void NavPanel::selectPath(const QString& path) {
    m_currentPath = path;
    QString targetData = (path == "trash://" || path == "trash") ? "trash_root" : path;
    QStandardItem* foundItem = findItemByData(m_model->invisibleRootItem(), targetData);
    if (foundItem) {
        m_treeView->setCurrentIndex(foundItem->index());
        m_treeView->setFocus();
    }
}

void NavPanel::onTreeContextMenu(const QPoint& pos) {
    QModelIndex index = m_treeView->indexAt(pos);
    if (!index.isValid()) return;

    QString path = index.data(Qt::UserRole + 1).toString();

    if (path == "trash_root") {
        QMenu menu(this);
        UiHelper::applyMenuStyle(&menu);

        QAction* actRestore = menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 18), "还原全部");
        QAction* actEmpty = menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "清空回收站");

        connect(actRestore, &QAction::triggered, this, [this]() {
            TrashService::instance().restoreAll(this);
        });
        connect(actEmpty, &QAction::triggered, this, [this]() {
            TrashService::instance().emptyTrash(this);
        });

        menu.exec(m_treeView->viewport()->mapToGlobal(pos));
        return;
    }

    if (path.isEmpty() || path == "computer://" || path == "recent_root") {
        return;
    }

    QFileInfo info(path);
    if (!info.exists() || !info.isDir()) {
        return;
    }

    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    // 1. 打开文件夹
    QAction* actOpenFolder = menu.addAction(UiHelper::getIcon("folder", QColor("#EEEEEE"), 18), "打开文件夹");
    connect(actOpenFolder, &QAction::triggered, this, [this, path]() {
        emit directorySelected(path);
    });

    // 2. 标签页级联子菜单
    ContextMenuFactory::buildTabCascadeMenu(&menu, path, this);

    // 3. 在“资源管理器”中显示
    ContextMenuFactory::buildShowInExplorerAction(&menu, path, this);

    menu.addSeparator();

    // 4. 置顶 / 收藏夹 / 库
    MarkedFolderService::instance().buildMarkAction(&menu, path, this);
    FavoriteService::instance().buildFavoriteAction(&menu, path, this);
    LibraryService::instance().buildLibraryAction(&menu, QStringList{path}, this);

    menu.addSeparator();

    // 5. 复制名称 / 复制完整路径
    ContextMenuFactory::buildCopyNameAction(&menu, QStringList{path}, this);
    ContextMenuFactory::buildCopyPathAction(&menu, QStringList{path}, this);

    // 6. 粘贴到此文件夹
    QAction* actPaste = menu.addAction(UiHelper::getIcon("paste", QColor("#EEEEEE"), 18), "粘贴到此文件夹");
    actPaste->setEnabled(ClipboardService::instance().canPaste(path));
    connect(actPaste, &QAction::triggered, this, [this, path]() {
        ClipboardService::instance().executePaste(path, this);
    });

    menu.exec(m_treeView->viewport()->mapToGlobal(pos));
}

void NavPanel::onTreeClicked(const QModelIndex& index) {
    QString path = index.data(Qt::UserRole + 1).toString();
    if (path == "trash_root") {
        emit requestOpenTrash();
    } else if (!path.isEmpty() && path != "computer://" && path != "recent_root") {
        emit directorySelected(path);
    } else if (path == "computer://") {
        emit directorySelected("computer://");
    }
}

void NavPanel::onItemExpanded(const QModelIndex& index) {
    QStandardItem* item = m_model->itemFromIndex(index);
    if (!item) return;

    if (item->rowCount() == 1 && item->child(0)->text() == "Loading...") {
        fetchChildDirs(item);
    }
}

void NavPanel::updateTreeHeight() {
}

bool NavPanel::eventFilter(QObject* watched, QEvent* event) {
    if ((watched == m_treeView || (m_treeView && watched == m_treeView->viewport())) 
        && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEv = static_cast<QKeyEvent*>(event);
        if (keyEv->modifiers() & Qt::ControlModifier) {
            if (keyEv->key() == Qt::Key_Home) {
                if (ContentKeyHandler::navigateViewFirstItem(m_treeView)) return true;
            } else if (keyEv->key() == Qt::Key_End) {
                if (ContentKeyHandler::navigateViewLastItem(m_treeView)) return true;
            }
        }
    }
    if (event->type() == QEvent::ToolTip) {
        QString text = watched->property("tooltipText").toString();
        if (!text.isEmpty()) {
            return true;
        }
    } else if (event->type() == QEvent::Leave) {
        ToolTipOverlay::hideTip();
    }
    return QFrame::eventFilter(watched, event);
}

void NavPanel::fetchChildDirs(QStandardItem* parent) {
    QString path = parent->data(Qt::UserRole + 1).toString();
    if (path.isEmpty() || path == "computer://") return;

    parent->removeRows(0, parent->rowCount());
    parent->appendRow(new QStandardItem("正在读取..."));

    QPersistentModelIndex pIdx(parent->index());
    (void)QtConcurrent::run([this, pIdx, path]() {
        QDir dir(path);
        QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

        struct DirInfo { QString name; QString absPath; bool hasSub; };
        QList<DirInfo> results;
        for (const QFileInfo& info : list) {
            QDir subDir(info.absoluteFilePath());
            bool hasSub = !subDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot).isEmpty();
            results << DirInfo{info.fileName(), info.absoluteFilePath(), hasSub};
        }

        QMetaObject::invokeMethod(QCoreApplication::instance(), [this, pIdx, results]() {
            if (!pIdx.isValid()) return;
            QStandardItem* safeParent = m_model->itemFromIndex(pIdx);
            if (!safeParent) return;

            safeParent->removeRows(0, safeParent->rowCount());

            for (const auto& info : results) {
                QIcon folderIcon = ShellIconManager::getFileIcon(info.absPath, 18);
                QStandardItem* child = new QStandardItem(folderIcon, info.name);
                child->setData(info.absPath, Qt::UserRole + 1);

                if (info.hasSub) {
                    child->appendRow(new QStandardItem("Loading..."));
                }
                safeParent->appendRow(child);
            }
        }, Qt::QueuedConnection);
    });
}

} // namespace QuarkMeta