#include "MarkedPanel.h"
#include "UiHelper.h"
#include "../meta/MarkedFolderService.h"
#include "ToolTipOverlay.h"
#include <QFileInfo>
#include <QDir>
#include <QMenu>
#include <QMouseEvent>
#include <QMimeData>
#include <QUrl>
#include <QClipboard>
#include <QGuiApplication>
#include "../util/ShellHelper.h"
#include "../core/NavigationService.h"
#include "controllers/ContextMenuFactory.h"

namespace QuarkMeta {

MarkedPanel::MarkedPanel(QWidget* parent) : QWidget(parent) {
    initUi();
    connect(&MarkedFolderService::instance(), &MarkedFolderService::markedFoldersChanged, this, &MarkedPanel::onMarkedFoldersChanged);
    loadMarkedFolders();
}

void MarkedPanel::initUi() {
    setAcceptDrops(true);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setObjectName("MarkedTreeWidget");
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setIndentation(0);
    m_treeWidget->setIconSize(QSize(16, 16));
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeWidget->setAcceptDrops(true);
    if (m_treeWidget->viewport()) {
        m_treeWidget->viewport()->setAcceptDrops(true);
        m_treeWidget->viewport()->installEventFilter(this);
    }
    m_treeWidget->installEventFilter(this);

    m_mainLayout->addWidget(m_treeWidget);

    connect(m_treeWidget, &QTreeWidget::itemClicked, this, &MarkedPanel::onItemClicked);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &MarkedPanel::onContextMenuRequested);
}

void MarkedPanel::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        return;
    }
    QWidget::dragEnterEvent(event);
}

void MarkedPanel::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        return;
    }
    QWidget::dragMoveEvent(event);
}

void MarkedPanel::dropEvent(QDropEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        handleDropPaths(event->mimeData(), event->position().toPoint(), false);
        return;
    }
    QWidget::dropEvent(event);
}

bool MarkedPanel::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_treeWidget || (m_treeWidget && watched == m_treeWidget->viewport())) {
        if (event->type() == QEvent::DragEnter) {
            auto* de = static_cast<QDragEnterEvent*>(event);
            if (de->mimeData() && de->mimeData()->hasUrls()) {
                de->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::DragMove) {
            auto* dm = static_cast<QDragMoveEvent*>(event);
            if (dm->mimeData() && dm->mimeData()->hasUrls()) {
                dm->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            auto* de = static_cast<QDropEvent*>(event);
            if (de->mimeData() && de->mimeData()->hasUrls()) {
                de->acceptProposedAction();
                bool isViewport = (m_treeWidget && watched == m_treeWidget->viewport());
                handleDropPaths(de->mimeData(), de->position().toPoint(), isViewport);
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MarkedPanel::handleDropPaths(const QMimeData* mimeData, const QPoint& pos, bool isViewportPos) {
    if (!mimeData) return;
    QStringList sourcePaths;
    const QList<QUrl> urls = mimeData->urls();
    for (const QUrl& url : urls) {
        QString localPath = url.toLocalFile();
        if (!localPath.isEmpty()) {
            sourcePaths.append(localPath);
        }
    }

    if (!sourcePaths.isEmpty()) {
        QTreeWidgetItem* item = nullptr;
        if (m_treeWidget) {
            QPoint viewportPos = isViewportPos ? pos : (m_treeWidget->viewport() ? m_treeWidget->viewport()->mapFrom(m_treeWidget, pos) : pos);
            item = m_treeWidget->itemAt(viewportPos);
        }

        if (item) {
            QString targetPath = item->data(0, Qt::UserRole).toString();
            if (!targetPath.isEmpty()) {
                MarkedFolderService::instance().executeTransferToMarked(sourcePaths, QStringList{targetPath});
                return;
            }
        }

        MarkedFolderService::instance().executeTransferToMarked(sourcePaths);
    }
}

void MarkedPanel::loadMarkedFolders() {
    m_treeWidget->clear();
    QStringList allList = MarkedFolderService::instance().getAllMarked();
    for (const QString& path : allList) {
        if (QFile::exists(path)) {
            bool active = MarkedFolderService::instance().isMarked(path);
            QTreeWidgetItem* item = createMarkedItem(path, active);
            m_treeWidget->addTopLevelItem(item);
        }
    }
}

QTreeWidgetItem* MarkedPanel::createMarkedItem(const QString& path, bool active) {
    QFileInfo fi(path);
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, fi.fileName().isEmpty() ? path : fi.fileName());
    item->setData(0, Qt::UserRole, path);
    item->setData(0, Qt::UserRole + 1, active);
    item->setIcon(0, UiHelper::getIcon("Destination.svg", active ? QColor("#ff551c") : QColor("#555555"), 16));
    return item;
}

void MarkedPanel::onItemClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if (!item) return;
    QString path = item->data(0, Qt::UserRole).toString();
    if (!path.isEmpty()) {
        emit requestLocateFolder(path);
    }
}

void MarkedPanel::onContextMenuRequested(const QPoint& pos) {
    QTreeWidgetItem* item = m_treeWidget ? m_treeWidget->itemAt(pos) : nullptr;
    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    if (item) {
        QString path = item->data(0, Qt::UserRole).toString();
        bool active = item->data(0, Qt::UserRole + 1).toBool();

        // 1. 打开与定位组
        QAction* openAct = menu.addAction(UiHelper::getIcon("folder", QColor("#EEEEEE"), 16), "打开文件夹");
        connect(openAct, &QAction::triggered, this, [this, path]() {
            emit requestLocateFolder(path);
        });

        QAction* newTabAct = menu.addAction(UiHelper::getIcon("add", QColor("#EEEEEE"), 16), "在新标签页中打开");
        connect(newTabAct, &QAction::triggered, this, [path]() {
            NavigationService::instance().openInNewTab(path);
        });

        ContextMenuFactory::buildShowInExplorerAction(&menu, path, this);

        menu.addSeparator();

        // 2. 标记状态控制组
        if (active) {
            QAction* unmarkAct = menu.addAction(UiHelper::getIcon("Destination.svg", QColor("#EEEEEE"), 16), "取消标记");
            connect(unmarkAct, &QAction::triggered, this, [path]() {
                MarkedFolderService::instance().removeMarked(path);
            });
        } else {
            QAction* markAct = menu.addAction(UiHelper::getIcon("Destination.svg", QColor("#EEEEEE"), 16), "标记为目标文件夹");
            connect(markAct, &QAction::triggered, this, [path]() {
                MarkedFolderService::instance().addMarked(path);
            });
        }

        QAction* keepOnlyAct = menu.addAction(UiHelper::getIcon("Destination.svg", QColor("#EEEEEE"), 16), "取消其他标记");
        connect(keepOnlyAct, &QAction::triggered, this, [path]() {
            MarkedFolderService::instance().keepOnlyMarked(path);
        });

        QAction* removeAct = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE"), 16), "从标记历史中移除");
        connect(removeAct, &QAction::triggered, this, [path]() {
            MarkedFolderService::instance().removeFolderRecord(path);
        });

        menu.addSeparator();

        // 3. 辅助工具组
        ContextMenuFactory::buildCopyPathAction(&menu, QStringList{path}, this);

        QAction* refreshAct = menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 16), "刷新");
        connect(refreshAct, &QAction::triggered, this, [this]() {
            loadMarkedFolders();
        });

        menu.addSeparator();
    }

    // 4. 全局清理组
    QAction* unmarkAllAct = menu.addAction(UiHelper::getIcon("Destination.svg", QColor("#EEEEEE"), 16), "取消所有标记");
    connect(unmarkAllAct, &QAction::triggered, this, []() {
        MarkedFolderService::instance().clearAll();
    });

    QAction* clearHistoryAct = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE"), 16), "清空标记历史");
    connect(clearHistoryAct, &QAction::triggered, this, []() {
        MarkedFolderService::instance().clearAllFolderRecords();
    });

    menu.exec(m_treeWidget->mapToGlobal(pos));
}

void MarkedPanel::onMarkedFoldersChanged() {
    loadMarkedFolders();
}

void MarkedPanel::filterTreeNodes(const QString& keyword) {
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

} // namespace QuarkMeta
