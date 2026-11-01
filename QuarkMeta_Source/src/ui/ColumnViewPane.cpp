#include "ColumnViewPane.h"
#include "ContentPanel.h"
#include "controllers/ContentViewCoordinator.h"
#include "models/SectionProxyModel.h"
#include "ColumnViewWidget.h"
#include "ColumnItemDelegate.h"
#include "controllers/ContentKeyHandler.h"
#include "../core/DiskScanService.h"
#include "../meta/MetaCacheDecorator.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>

namespace QuarkMeta {

ColumnViewPane::ColumnViewPane(const QString& path, ContentPanel* contentPanel, QWidget* parent)
    : QWidget(parent), m_path(path), m_contentPanel(contentPanel) 
{
    setObjectName("ColumnViewPane");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(220);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 1, 1, 0);
    layout->setSpacing(0);

    m_model = new DiskItemModel(this);
    m_model->setCurrentPath(path);

    m_proxyModel = new FilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);

    m_sectionProxyModel = new SectionProxyModel(this);
    m_sectionProxyModel->setSourceModel(m_proxyModel);

    m_listView = new DropListView(this);
    m_listView->setObjectName("ColumnViewPaneListView");
    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setFocusPolicy(Qt::StrongFocus);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_listView->setDragEnabled(true);
    m_listView->setAcceptDrops(true);
    m_listView->setDropIndicatorShown(true);
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listView->setModel(m_sectionProxyModel);
    m_listView->setItemDelegate(new ColumnItemDelegate(this));

    m_folderListView = m_listView;

    layout->addWidget(m_listView);

    m_visibleTimer = new QTimer(this);
    m_visibleTimer->setSingleShot(true);
    m_visibleTimer->setInterval(60);
    connect(m_visibleTimer, &QTimer::timeout, this, &ColumnViewPane::refreshVisibleThumbnails);

    if (m_listView->verticalScrollBar()) {
        connect(m_listView->verticalScrollBar(), &QScrollBar::valueChanged, this, [this]() {
            if (m_visibleTimer) m_visibleTimer->start();
        });
    }

    connect(m_proxyModel, &QAbstractItemModel::modelReset, this, [this]() {
        tryPendingSelection();
        if (m_visibleTimer) m_visibleTimer->start();
    });
    connect(m_proxyModel, &QAbstractItemModel::layoutChanged, this, [this]() {
        tryPendingSelection();
        if (m_visibleTimer) m_visibleTimer->start();
    });

    connect(m_listView, &DropListView::blankSpaceClicked, this, [this]() {
        int paneIdx = property("paneIndex").toInt();
        if (m_contentPanel && m_contentPanel->columnView()) {
            m_contentPanel->columnView()->activatePaneFromBlankClick(paneIdx);
        }
    });

    connect(m_listView, &DropListView::blankSpaceDoubleClicked, this, [this]() {
        int paneIdx = property("paneIndex").toInt();
        emit blankSpaceDoubleClicked(paneIdx);
    });

    m_listView->installEventFilter(this);
    if (m_listView->viewport()) m_listView->viewport()->installEventFilter(this);

    if (m_contentPanel) {
        m_listView->installEventFilter(m_contentPanel);
        connect(m_listView, &QListView::customContextMenuRequested, this, [this](const QPoint& pos) {
            emit contextMenuRequested(m_listView, pos);
            if (m_contentPanel) {
                m_contentPanel->onCustomContextMenuRequested(m_listView, pos);
            }
        });
        connect(m_listView, &DropListView::pathsDropped, this, [this](const QStringList& paths, const QModelIndex& targetIndex, Qt::DropAction action) {
            emit pathsDroppedSignal(paths, targetIndex, m_path, action);
            if (m_contentPanel) {
                m_contentPanel->onPathsDropped(paths, targetIndex, m_path, action);
            }
        });
    }

    connect(m_listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this]() {
        emit selectionChanged();
    });

    connect(m_listView, &DropListView::sectionHeaderClicked, this, [this](const QModelIndex& index) {
        if (index.data(SectionKindRole).toInt() == 1 && m_sectionProxyModel) {
            m_sectionProxyModel->setFolderCollapsed(!m_sectionProxyModel->isFolderCollapsed());
        }
    });

    connect(m_listView, &QListView::clicked, this, [this](const QModelIndex& index) {
        if (!index.isValid() || index.data(SectionHeaderRole).toBool()) return;
        QString itemPath = index.data(PathRole).toString();
        bool isDir = (index.data(TypeRole).toString() == "folder") || index.data(Qt::UserRole + 2).toBool() || QFileInfo(itemPath).isDir();
        int paneIdx = property("paneIndex").toInt();
        if (isDir) {
            emit folderClicked(itemPath, paneIdx);
        } else {
            emit fileClicked(itemPath, paneIdx);
        }
    });

    connect(m_listView, &QListView::doubleClicked, this, [this](const QModelIndex& index) {
        if (m_contentPanel) {
            m_contentPanel->activateIndex(index);
        }
    });
}

void ColumnViewPane::setActive(bool active) {
    if (m_isActive != active) {
        m_isActive = active;
        update();
    }
}

void ColumnViewPane::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    // 【架构红线 - 蓝色顶部焦点提示线设计约束】
    // 此处绘制的顶部蓝线（颜色 #3498db，宽度 1px）是"列视图（Column View）中活跃列"的焦点视觉标识。
    // ─────────────────────────────────────────────────────────────────────────────────
    // ✅ 唯一合法应用范围：ColumnViewPane（列视图中的单列窗格），当 m_isActive == true 时绘制。
    // ❌ 严禁应用范围：
    //    - ContentPanel（内容面板/窗格）     → 其活跃状态由 ContentPaneSplitManager::setActivePane
    //                                          通过 QSS property "activePane" 独立管理（border样式）。
    //    - ContentHeaderWidget（面板标题栏） → 其活跃状态由 ContentHeaderWidget::setActive
    //                                          通过 QSS property "activePane" 独立管理。
    //    - 任何其他非 ColumnViewPane 类型的 Widget。
    // ─────────────────────────────────────────────────────────────────────────────────
    // 两条"活跃"状态机（列焦点 vs 窗格焦点）完全正交，绝对不可混用或合并。
    if (m_isActive) {
        painter.setPen(QPen(QColor("#3498db"), 1));
        painter.drawLine(0, 0, width(), 0);
    }
}

bool ColumnViewPane::eventFilter(QObject* obj, QEvent* event) {
    if (event && event->type() == QEvent::Wheel) {
        auto* wEvent = static_cast<QWheelEvent*>(event);
        if (!(wEvent->modifiers() & Qt::ControlModifier)) {
            if (m_paneScrollArea && m_paneScrollArea->verticalScrollBar() && m_paneScrollArea->verticalScrollBar()->isVisible()) {
                int delta = wEvent->angleDelta().y();
                if (delta == 0) delta = wEvent->pixelDelta().y();
                if (delta != 0) {
                    QScrollBar* sb = m_paneScrollArea->verticalScrollBar();
                    int step = sb->singleStep();
                    if (step <= 0) step = 20;
                    int scrollAmount = (delta / 120.0) * step * 3;
                    if (scrollAmount == 0) scrollAmount = (delta > 0 ? -step : step);
                    else scrollAmount = -scrollAmount;
                    sb->setValue(sb->value() + scrollAmount);
                    return true;
                }
            }
        }
    }

    if (event && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && (obj == m_paneScrollArea || obj == this)) {
            int paneIdx = property("paneIndex").toInt();
            if (m_contentPanel && m_contentPanel->columnView()) {
                m_contentPanel->columnView()->activatePaneFromBlankClick(paneIdx);
            }
        }
    }

    if (event && event->type() == QEvent::KeyPress && (obj == m_folderListView || obj == m_listView)) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        int paneIdx = property("paneIndex").toInt();
        if (keyEvent->modifiers() & Qt::ControlModifier) {
            if (keyEvent->key() == Qt::Key_Home) {
                DropListView* view = qobject_cast<DropListView*>(obj);
                if (view && ContentKeyHandler::navigateViewFirstItem(view)) return true;
            } else if (keyEvent->key() == Qt::Key_End) {
                DropListView* view = qobject_cast<DropListView*>(obj);
                if (view && ContentKeyHandler::navigateViewLastItem(view)) return true;
            }
        }
        if (keyEvent->key() == Qt::Key_Right || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            DropListView* view = qobject_cast<DropListView*>(obj);
            if (view && view->currentIndex().isValid()) {
                QModelIndex idx = view->currentIndex();
                QString itemPath = idx.data(PathRole).toString();
                bool isDir = (idx.data(TypeRole).toString() == "folder") || idx.data(Qt::UserRole + 2).toBool() || QFileInfo(itemPath).isDir();
                if (isDir && !itemPath.isEmpty()) {
                    emit folderExpandRequested(itemPath, paneIdx);
                    return true;
                }
            }
        } else if (keyEvent->key() == Qt::Key_Left) {
            if (paneIdx > 0 && m_contentPanel && m_contentPanel->columnView()) {
                m_contentPanel->columnView()->focusPane(paneIdx - 1);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void ColumnViewPane::refreshVisibleThumbnails() {
    if (!m_listView || !m_model) return;
    QSet<int> visibleRows = ContentViewCoordinator::calculateVisibleSourceRows(m_listView, m_model);
    if (!visibleRows.isEmpty()) {
        m_model->loadThumbnailsForRows(visibleRows.values());
    }
    if (m_listView->viewport()) {
        m_listView->viewport()->update();
    }
}

void ColumnViewPane::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (m_visibleTimer) m_visibleTimer->start();
    update();
}

void ColumnViewPane::setFilterState(const FilterState& state) {
    if (m_proxyModel) {
        m_proxyModel->currentFilter = state;
        m_proxyModel->updateFilter();
    }
}

void ColumnViewPane::selectItemByPath(const QString& targetPath) {
    m_pendingSelectPath = targetPath;
    tryPendingSelection();
}

void ColumnViewPane::setPendingSelectPaths(const QSet<QString>& paths, bool edit) {
    m_pendingSelectPaths = paths;
    m_pendingSelectPath.clear();
    m_isPendingEdit = edit;
    tryPendingSelection();
}

void ColumnViewPane::applySort(int sortType, Qt::SortOrder sortOrder) {
    if (m_proxyModel) {
        m_proxyModel->setSortCriteria(sortType, sortOrder);
    }
}

void ColumnViewPane::tryPendingSelection() {
    if (!m_sectionProxyModel || !m_listView) return;

    // 🚀【单次有效保证】：保存待匹配数据，并确保退出该函数时单次清空，绝不留存至未来的 modelReset
    QSet<QString> pendingPaths = m_pendingSelectPaths;
    QString pendingPath = m_pendingSelectPath;
    m_pendingSelectPaths.clear();
    m_pendingSelectPath.clear();

    if (!pendingPaths.isEmpty()) {
        QSet<QString> normalizedPending;
        normalizedPending.reserve(pendingPaths.size());
        for (const QString& p : pendingPaths) {
            normalizedPending.insert(QDir::toNativeSeparators(QDir::cleanPath(p)).toLower());
        }

        QItemSelection sel;
        QModelIndex lastIdx;
        int total = m_sectionProxyModel->rowCount();
        for (int r = 0; r < total; ++r) {
            QModelIndex idx = m_sectionProxyModel->index(r, 0);
            if (idx.data(SectionHeaderRole).toBool()) continue;
            QString itemPath = QDir::toNativeSeparators(QDir::cleanPath(idx.data(PathRole).toString())).toLower();
            if (normalizedPending.contains(itemPath)) {
                sel.select(idx, idx);
                lastIdx = idx;
            }
        }

        if (!sel.isEmpty() && m_listView->selectionModel()) {
            m_listView->selectionModel()->select(sel, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            if (lastIdx.isValid()) {
                m_listView->selectionModel()->setCurrentIndex(lastIdx, QItemSelectionModel::NoUpdate);
                // 🚀 仅不在可视区域时进行最小 EnsureVisible 滚动，杜绝 PositionAtCenter 剧烈跳动
                QRect visualRect = m_listView->visualRect(lastIdx);
                if (!m_listView->viewport()->rect().contains(visualRect)) {
                    m_listView->scrollTo(lastIdx, QAbstractItemView::EnsureVisible);
                }
            }
            emit selectionChanged();
            return;
        }
    }

    if (!pendingPath.isEmpty()) {
        QString cleanTarget = QDir::toNativeSeparators(QDir::cleanPath(pendingPath));

        int total = m_sectionProxyModel->rowCount();
        for (int r = 0; r < total; ++r) {
            QModelIndex idx = m_sectionProxyModel->index(r, 0);
            if (idx.data(SectionHeaderRole).toBool()) continue;
            QString itemPath = QDir::toNativeSeparators(QDir::cleanPath(idx.data(PathRole).toString()));

            // 🚀 彻底删除文件名兜底分支，仅按规范化后的完整绝对路径精确匹配
            if (QString::compare(itemPath, cleanTarget, Qt::CaseInsensitive) == 0) {
                if (m_listView->selectionModel()) {
                    m_listView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                    m_listView->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::NoUpdate);
                }
                QRect visualRect = m_listView->visualRect(idx);
                if (!m_listView->viewport()->rect().contains(visualRect)) {
                    m_listView->scrollTo(idx, QAbstractItemView::EnsureVisible);
                }
                emit selectionChanged();
                return;
            }
        }
    }
}

void ColumnViewPane::clearSelection() {
    if (m_listView && m_listView->selectionModel()) {
        QSignalBlocker blocker(m_listView->selectionModel());
        m_listView->clearSelection();
    } else if (m_listView) {
        m_listView->clearSelection();
    }
}

void ColumnViewPane::loadDirectory() {
    QString path = m_path;
    bool recursive = false;
    if (m_contentPanel && m_contentPanel->isRecursive()) {
        if (m_contentPanel->columnView() && 
            m_contentPanel->columnView()->activePane() == this) {
            recursive = true;
        }
    }
    QPointer<ColumnViewPane> weakSelf(this);
    (void)QtConcurrent::run([weakSelf, path, recursive]() {
        if (!weakSelf) return;
        std::vector<ItemRecord> items;
        if (path.isEmpty() || path == "computer://") {
            for (const QFileInfo& drive : QDir::drives()) {
                items.push_back(ItemRecord::create(drive.absolutePath()));
            }
        } else {
            items = DiskScanService::scanDirectory(path, recursive, [weakSelf]() {
                return weakSelf != nullptr;
            });
        }
        MetaCacheDecorator::decorate(items);
        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakSelf, items]() {
            if (weakSelf && weakSelf->m_model) {
                weakSelf->m_model->setRecords(items);
                if (weakSelf->m_contentPanel) {
                    weakSelf->applySort(static_cast<int>(weakSelf->m_contentPanel->currentSortType()), 
                                        weakSelf->m_contentPanel->currentSortOrder());
                }
                if (!weakSelf->m_pendingSelectPaths.isEmpty()) {
                    weakSelf->tryPendingSelection();
                } else if (!weakSelf->m_pendingSelectPath.isEmpty()) {
                    weakSelf->selectItemByPath(weakSelf->m_pendingSelectPath);
                }
                int count = weakSelf->m_model->rowCount();
                if (count > 0) {
                    weakSelf->refreshVisibleThumbnails();
                }
                emit weakSelf->recordsLoaded(weakSelf->m_model->allRecords());
            }
        });
    });
}

} // namespace QuarkMeta
