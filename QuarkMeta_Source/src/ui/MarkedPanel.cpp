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
    m_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeWidget->setAcceptDrops(true);
    if (m_treeWidget->viewport()) {
        m_treeWidget->viewport()->setAcceptDrops(true);
        m_treeWidget->viewport()->installEventFilter(this);
    }
    m_treeWidget->installEventFilter(this);

    m_mainLayout->addWidget(m_treeWidget);

    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &MarkedPanel::onItemDoubleClicked);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &MarkedPanel::onContextMenuRequested);
}

void MarkedPanel::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        showDragOverlay();
        return;
    }
    QWidget::dragEnterEvent(event);
}

void MarkedPanel::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        QPoint pos = event->position().toPoint();
        QPoint viewportPos = (m_treeWidget && m_treeWidget->viewport()) ? m_treeWidget->viewport()->mapFrom(this, pos) : pos;
        QTreeWidgetItem* item = m_treeWidget ? m_treeWidget->itemAt(viewportPos) : nullptr;
        QString hoverPath = item ? item->data(0, Qt::UserRole).toString() : QString();
        if (hoverPath != m_hoverTargetPath) {
            m_hoverTargetPath = hoverPath;
            updateDragOverlayText();
        }
        return;
    }
    QWidget::dragMoveEvent(event);
}

void MarkedPanel::dragLeaveEvent(QDragLeaveEvent* event) {
    hideDragOverlay();
    QWidget::dragLeaveEvent(event);
}

void MarkedPanel::dropEvent(QDropEvent* event) {
    hideDragOverlay();
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        handleDropPaths(event->mimeData(), event->position().toPoint(), false);
        return;
    }
    QWidget::dropEvent(event);
}

void MarkedPanel::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (m_dragOverlay && m_dragOverlay->isVisible()) {
        m_dragOverlay->setGeometry(rect());
        m_dragOverlay->raise();
    }
}

bool MarkedPanel::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_treeWidget || (m_treeWidget && watched == m_treeWidget->viewport())) {
        if (event->type() == QEvent::DragEnter) {
            auto* de = static_cast<QDragEnterEvent*>(event);
            if (de->mimeData() && de->mimeData()->hasUrls()) {
                de->acceptProposedAction();
                showDragOverlay();
                return true;
            }
        } else if (event->type() == QEvent::DragMove) {
            auto* dm = static_cast<QDragMoveEvent*>(event);
            if (dm->mimeData() && dm->mimeData()->hasUrls()) {
                dm->acceptProposedAction();
                bool isViewportPos = (m_treeWidget && watched == m_treeWidget->viewport());
                QPoint pos = dm->position().toPoint();
                QPoint viewportPos = isViewportPos ? pos : (m_treeWidget->viewport() ? m_treeWidget->viewport()->mapFrom(m_treeWidget, pos) : pos);
                QTreeWidgetItem* item = m_treeWidget ? m_treeWidget->itemAt(viewportPos) : nullptr;
                QString hoverPath = item ? item->data(0, Qt::UserRole).toString() : QString();
                if (hoverPath != m_hoverTargetPath) {
                    m_hoverTargetPath = hoverPath;
                    updateDragOverlayText();
                }
                return true;
            }
        } else if (event->type() == QEvent::DragLeave) {
            hideDragOverlay();
            return false;
        } else if (event->type() == QEvent::Drop) {
            hideDragOverlay();
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

void MarkedPanel::showDragOverlay(const QString& hoverTargetPath) {
    if (!m_dragOverlay) {
        m_dragOverlay = new QWidget(this);
        m_dragOverlay->setObjectName("MarkedDragOverlay");
        m_dragOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_dragOverlay->setAttribute(Qt::WA_StyledBackground);
        m_dragOverlay->setStyleSheet(
            "QWidget#MarkedDragOverlay { background-color: rgba(0, 122, 255, 0.90); border: 2px solid #007AFF; }"
        );

        QVBoxLayout* layout = new QVBoxLayout(m_dragOverlay);
        layout->setContentsMargins(10, 10, 10, 10);
        m_dragOverlayLabel = new QLabel(m_dragOverlay);
        m_dragOverlayLabel->setAlignment(Qt::AlignCenter);
        m_dragOverlayLabel->setStyleSheet(
            "color: #FFFFFF; font-size: 13px; font-weight: bold;"
        );
        layout->addWidget(m_dragOverlayLabel);
    }

    // 性能要求：getActiveMarked() 是数据库查询，只允许在 DragEnter 时调用一次并缓存到 m_activeTargetNames
    m_activeTargetNames.clear();
    const QStringList activePaths = MarkedFolderService::instance().getActiveMarked();
    for (const QString& path : activePaths) {
        QString name = QFileInfo(path).fileName();
        if (name.isEmpty()) name = path;
        m_activeTargetNames.append(name);
    }

    m_hoverTargetPath = hoverTargetPath;
    updateDragOverlayText();

    m_dragOverlay->setGeometry(rect());
    m_dragOverlay->show();
    m_dragOverlay->raise();
}

void MarkedPanel::hideDragOverlay() {
    m_hoverTargetPath.clear();
    m_activeTargetNames.clear();
    if (m_dragOverlay) {
        m_dragOverlay->hide();
    }
}

void MarkedPanel::updateDragOverlayText() {
    if (!m_dragOverlayLabel) return;

    if (!m_hoverTargetPath.isEmpty()) {
        QString name = QFileInfo(m_hoverTargetPath).fileName();
        if (name.isEmpty()) name = m_hoverTargetPath;
        m_dragOverlayLabel->setText(QString("项目即将分发至 1 个目标文件夹\n%1").arg(name));
    } else {
        if (!m_activeTargetNames.isEmpty()) {
            QStringList lines;
            lines << QString("项目即将分发至 %1 个目标文件夹").arg(m_activeTargetNames.size());
            lines.append(m_activeTargetNames);
            m_dragOverlayLabel->setText(lines.join("\n"));
        } else {
            m_dragOverlayLabel->setText("尚未标记目的地");
        }
    }
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
    if (!m_currentFilterKeyword.isEmpty()) {
        filterTreeNodes(m_currentFilterKeyword);
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

void MarkedPanel::onItemDoubleClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if (!item) return;
    QString path = item->data(0, Qt::UserRole).toString();
    if (!path.isEmpty()) {
        emit requestLocateFolder(path);
    }
}

void MarkedPanel::onContextMenuRequested(const QPoint& pos) {
    QList<QTreeWidgetItem*> selectedItems = m_treeWidget ? m_treeWidget->selectedItems() : QList<QTreeWidgetItem*>();
    QTreeWidgetItem* clickedItem = m_treeWidget ? m_treeWidget->itemAt(pos) : nullptr;
    if (clickedItem && !selectedItems.contains(clickedItem)) {
        m_treeWidget->clearSelection();
        clickedItem->setSelected(true);
        selectedItems = {clickedItem};
    }

    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    if (!selectedItems.isEmpty()) {
        QStringList selectedPaths;
        bool allActive = true;
        for (QTreeWidgetItem* item : selectedItems) {
            QString path = item->data(0, Qt::UserRole).toString();
            if (!path.isEmpty()) {
                selectedPaths.append(path);
                bool active = item->data(0, Qt::UserRole + 1).toBool();
                if (!active) {
                    allActive = false;
                }
            }
        }

        // 1. 打开与定位组 (仅单选有效，多选置灰)
        QAction* openAct = menu.addAction(UiHelper::getIcon("folder", QColor("#EEEEEE"), 16), "打开文件夹");
        QAction* newTabAct = menu.addAction(UiHelper::getIcon("add", QColor("#EEEEEE"), 16), "在新标签页中打开");
        if (selectedPaths.size() == 1) {
            QString singlePath = selectedPaths.first();
            connect(openAct, &QAction::triggered, this, [this, singlePath]() {
                emit requestLocateFolder(singlePath);
            });
            connect(newTabAct, &QAction::triggered, this, [singlePath]() {
                NavigationService::instance().openInNewTab(singlePath);
            });
            ContextMenuFactory::buildShowInExplorerAction(&menu, singlePath, this);
        } else {
            openAct->setEnabled(false);
            newTabAct->setEnabled(false);
            QAction* expAct = menu.addAction(UiHelper::getIcon("folder_search", QColor("#EEEEEE"), 16), ContextMenuFactory::kTextShowInExplorer);
            connect(expAct, &QAction::triggered, this, [selectedPaths]() {
                for (const QString& p : selectedPaths) {
                    ShellHelper::openInExplorer(p);
                }
            });
        }

        menu.addSeparator();

        // 2. 标记状态控制组 (支持批量)
        if (allActive) {
            QAction* unmarkAct = menu.addAction(UiHelper::getIcon("Destination.svg", QColor("#EEEEEE"), 16), "取消标记");
            connect(unmarkAct, &QAction::triggered, this, [selectedPaths]() {
                for (const QString& p : selectedPaths) {
                    MarkedFolderService::instance().removeMarked(p);
                }
            });
        } else {
            QAction* markAct = menu.addAction(UiHelper::getIcon("Destination.svg", QColor("#EEEEEE"), 16), "标记为目标文件夹");
            connect(markAct, &QAction::triggered, this, [selectedPaths]() {
                for (const QString& p : selectedPaths) {
                    MarkedFolderService::instance().addMarked(p);
                }
            });
        }

        QAction* keepOnlyAct = menu.addAction(UiHelper::getIcon("Destination.svg", QColor("#EEEEEE"), 16), "取消其他标记");
        if (selectedPaths.size() == 1) {
            QString singlePath = selectedPaths.first();
            connect(keepOnlyAct, &QAction::triggered, this, [singlePath]() {
                MarkedFolderService::instance().keepOnlyMarked(singlePath);
            });
        } else {
            keepOnlyAct->setEnabled(false);
        }

        QAction* removeAct = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE"), 16), "从标记历史中移除");
        connect(removeAct, &QAction::triggered, this, [selectedPaths]() {
            for (const QString& p : selectedPaths) {
                MarkedFolderService::instance().removeFolderRecord(p);
            }
        });

        menu.addSeparator();

        // 3. 辅助工具组 (支持批量复制完整路径)
        ContextMenuFactory::buildCopyPathAction(&menu, selectedPaths, this);

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
    m_currentFilterKeyword = keyword;
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
