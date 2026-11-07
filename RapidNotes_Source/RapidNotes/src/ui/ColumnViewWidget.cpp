#include "ColumnViewWidget.h"
#include "ColumnBlankCanvasWidget.h"
#include "ColumnViewPane.h"
#include "ContentPanel.h"
#include "models/SectionProxyModel.h"
#include "../core/NavigationService.h"
#include <QDir>
#include <QFileInfo>
#include <QResizeEvent>
#include <QScrollBar>

namespace QuarkMeta {

ColumnViewWidget::ColumnViewWidget(ContentPanel* contentPanel, QWidget* parent)
    : QScrollArea(parent), m_contentPanel(contentPanel)
{
    setObjectName("ColumnViewScrollArea");
    setWidgetResizable(true);

    m_container = new QWidget(this);
    m_layout = new QHBoxLayout(m_container);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setAlignment(Qt::AlignLeft);

    m_blankCanvasWidget = new ColumnBlankCanvasWidget(this, m_contentPanel, m_container);
    m_blankCanvasWidget->show();

    setWidget(m_container);

    // 监听背景留白区域的双击事件，用于触发右侧空白区域双击回退
    viewport()->installEventFilter(this);
    m_container->installEventFilter(this);
    if (m_contentPanel) {
        installEventFilter(m_contentPanel);
        viewport()->installEventFilter(m_contentPanel);
        m_container->installEventFilter(m_contentPanel);
    }

    if (horizontalScrollBar()) {
        connect(horizontalScrollBar(), &QScrollBar::rangeChanged, this, [this](int min, int max) {
            Q_UNUSED(min);
            if (m_autoScrollToRight) {
                horizontalScrollBar()->setValue(max);
                m_autoScrollToRight = false;
            }
        });
    }
}

void ColumnViewWidget::scrollToRightmostPane() {
    m_autoScrollToRight = true;
    QMetaObject::invokeMethod(this, [this]() {
        if (horizontalScrollBar()) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
        }
        if (!m_panes.isEmpty() && m_panes.last()) {
            ensureWidgetVisible(m_panes.last(), 0, 0);
        }
    }, Qt::QueuedConnection);
}

ColumnViewPane* ColumnViewWidget::activePane() const {
    if (m_activePaneIndex >= 0 && m_activePaneIndex < m_panes.size()) {
        return m_panes[m_activePaneIndex];
    }
    return m_panes.isEmpty() ? nullptr : m_panes.last();
}

ColumnViewPane* ColumnViewWidget::rightmostPane() const {
    return m_panes.isEmpty() ? nullptr : m_panes.last();
}

ColumnViewPane* ColumnViewWidget::findPaneForView(QAbstractItemView* view) const {
    if (!view) return nullptr;
    for (ColumnViewPane* pane : m_panes) {
        if (pane && (pane->listView() == view || pane->folderListView() == view)) {
            return pane;
        }
    }
    return nullptr;
}

ColumnViewPane* ColumnViewWidget::findPaneForPath(const QString& targetPath) const {
    if (m_panes.isEmpty() || targetPath.isEmpty()) return nullptr;

    QString cleanTarget = QDir::toNativeSeparators(QDir::cleanPath(targetPath));
    QString targetParent = QDir::toNativeSeparators(QDir::cleanPath(QFileInfo(cleanTarget).absolutePath()));

    // 优先匹配包含目标项所在父目录的 Pane
    for (int i = m_panes.size() - 1; i >= 0; --i) {
        ColumnViewPane* pane = m_panes[i];
        if (!pane) continue;
        QString panePath = QDir::toNativeSeparators(QDir::cleanPath(pane->currentPath()));
        if (QString::compare(panePath, targetParent, Qt::CaseInsensitive) == 0 ||
            QString::compare(panePath, cleanTarget, Qt::CaseInsensitive) == 0) {
            return pane;
        }
    }
    return rightmostPane();
}

void ColumnViewWidget::focusPane(int paneIndex) {
    if (paneIndex >= 0 && paneIndex < m_panes.size()) {
        setActivePaneIndex(paneIndex);
        ColumnViewPane* pane = m_panes[paneIndex];
        if (pane) {
            DropListView* view = pane->listView();
            if (view) view->setFocus();
        }
    }
}

bool ColumnViewWidget::containsPath(const QString& path) const {
    if (path.isEmpty()) return false;
    QString cleanTarget = QDir::toNativeSeparators(QDir::cleanPath(path));
    for (auto* pane : m_panes) {
        if (pane) {
            QString panePath = QDir::toNativeSeparators(QDir::cleanPath(pane->currentPath()));
            if (QString::compare(panePath, cleanTarget, Qt::CaseInsensitive) == 0) {
                return true;
            }
        }
    }
    return false;
}

QStringList ColumnViewWidget::getSelectedPaths() const {
    ColumnViewPane* pane = activePane();
    if (!pane) return {};
    QStringList paths;
    if (pane->listView() && pane->listView()->selectionModel()) {
        for (const auto& idx : pane->listView()->selectionModel()->selectedIndexes()) {
            if (idx.column() == static_cast<int>(FileListColumn::Name) && !idx.data(SectionHeaderRole).toBool()) {
                QString p = idx.data(PathRole).toString();
                if (!p.isEmpty()) paths << p;
            }
        }
    }
    return paths;
}

QModelIndexList ColumnViewWidget::getSelectedIndexes() const {
    ColumnViewPane* pane = activePane();
    if (!pane) return {};
    if (pane->listView() && pane->listView()->selectionModel()) {
        return pane->listView()->selectionModel()->selectedIndexes();
    }
    return {};
}

void ColumnViewWidget::applyFilterState(const FilterState& state) {
    m_currentFilter = state;
    for (int i = 0; i < m_panes.size(); ++i) {
        if (i == m_panes.size() - 1) {
            m_panes[i]->setFilterState(m_currentFilter);
        } else {
            FilterState parentFilter;
            parentFilter.showHidden = m_currentFilter.showHidden;
            m_panes[i]->setFilterState(parentFilter);
        }
    }
}

void ColumnViewWidget::applySort(int sortType, Qt::SortOrder sortOrder) {
    for (auto* pane : m_panes) {
        if (pane) {
            pane->applySort(sortType, sortOrder);
        }
    }
}

void ColumnViewWidget::setRootPath(const QString& path) {
    clearAllColumns();
    if (path.isEmpty()) return;

    if (path == "computer://") {
        appendColumn("computer://");
        setActivePaneIndex(0);
        updatePaneWidths();
        return;
    }

    QString targetFilePath;
    QString dirPath = path;
    QFileInfo info(path);
    if (info.exists() && !info.isDir()) {
        targetFilePath = path;
        dirPath = info.absolutePath();
    }

    // 1. 拆分完整的祖先路径栈
    QList<QString> pathStack;
    QDir dir(dirPath);
    QString curr = dir.absolutePath();

    while (!curr.isEmpty()) {
        pathStack.prepend(curr);
        QDir parentDir(curr);
        if (!parentDir.cdUp() || parentDir.absolutePath() == curr) {
            break;
        }
        curr = parentDir.absolutePath();
    }

    // 2. 逐层展开列，并在父列中高亮选中对应的子项
    for (int i = 0; i < pathStack.size(); ++i) {
        const QString& p = pathStack[i];
        appendColumn(p);
        if (i > 0 && i - 1 < m_panes.size() - 1) {
            m_panes[i - 1]->selectItemByPath(p);
        }
    }

    if (!targetFilePath.isEmpty() && !m_panes.isEmpty()) {
        m_panes.last()->selectItemByPath(targetFilePath);
    }

    setActivePaneIndex(m_panes.size() - 1);

    updatePaneWidths();
    updateParentHighlights();
}

void ColumnViewWidget::clearAllColumns() {
    dismissSubColumns(-1);
}

void ColumnViewWidget::dismissSubColumns(int fromIndex) {
    while (m_panes.size() > fromIndex + 1) {
        ColumnViewPane* pane = m_panes.takeLast();
        m_layout->removeWidget(pane);
        pane->deleteLater();
    }
    updatePaneWidths();
    updateParentHighlights();
    for (int i = 0; i < m_panes.size(); ++i) {
        if (i == m_panes.size() - 1) {
            m_panes[i]->setFilterState(m_currentFilter);
        } else {
            FilterState parentFilter;
            parentFilter.showHidden = m_currentFilter.showHidden;
            m_panes[i]->setFilterState(parentFilter);
        }
    }
    if (rightmostPane() && rightmostPane()->model()) {
        emit activeColumnRecordsChanged(rightmostPane()->model()->allRecords());
    }
}

ColumnViewPane* ColumnViewWidget::appendColumn(const QString& path) {
    int newIdx = m_panes.size();
    ColumnViewPane* pane = new ColumnViewPane(path, m_contentPanel, m_container);
    pane->setProperty("paneIndex", newIdx);

    connect(pane, &ColumnViewPane::blankSpaceDoubleClicked, this, [this](int paneIdx) {
        goUpColumnFromIndex(paneIdx);
    });
    if (m_contentPanel) {
        pane->applySort(static_cast<int>(m_contentPanel->currentSortType()), m_contentPanel->currentSortOrder());
    }
    pane->loadDirectory();

    connect(pane, &ColumnViewPane::recordsLoaded, this, [this, pane](const std::vector<ItemRecord>& records) {
        if (pane == rightmostPane()) {
            emit activeColumnRecordsChanged(records);
            if (m_contentPanel) {
                m_contentPanel->recalculateAndEmitStats();
            }
        }
    });

    connect(pane, &ColumnViewPane::selectionChanged, this, [this, pane]() {
        setActivePaneIndex(pane->property("paneIndex").toInt());
        emit selectionChanged();
        if (rightmostPane() && rightmostPane()->model()) {
            emit activeColumnRecordsChanged(rightmostPane()->model()->allRecords());
        }
    });

    connect(pane, &ColumnViewPane::folderClicked, this, [this](const QString&, int paneIdx) {
        setActivePaneIndex(paneIdx);
        emit selectionChanged();
    });

    connect(pane, &ColumnViewPane::fileClicked, this, [this](const QString&, int paneIdx) {
        setActivePaneIndex(paneIdx);
        emit selectionChanged();
    });

    auto handleFolderExpand = [this](const QString& folderPath, int paneIdx) {
        if (paneIdx + 1 < m_panes.size() &&
            QDir::cleanPath(m_panes[paneIdx + 1]->currentPath()) == QDir::cleanPath(folderPath)) {
            setActivePaneIndex(paneIdx + 1);
            focusPane(paneIdx + 1);
            emit selectionChanged();
            return;
        }

        dismissSubColumns(paneIdx);
        clearOtherSelections(paneIdx);
        appendColumn(folderPath);
        emit pathNavigated(folderPath);
        if (m_contentPanel) {
            m_contentPanel->recalculateAndEmitStats();
        }
    };

    connect(pane, &ColumnViewPane::folderExpandRequested, this, handleFolderExpand);
    connect(pane, &ColumnViewPane::folderSelected, this, handleFolderExpand);

    connect(pane, &ColumnViewPane::fileSelected, this, [this](const QString&, int paneIdx) {
        setActivePaneIndex(paneIdx);
        emit selectionChanged();
    });

    m_panes.append(pane);
    m_layout->addWidget(pane);
    setActivePaneIndex(newIdx);
    updatePaneWidths();
    updateParentHighlights();
    for (int i = 0; i < m_panes.size(); ++i) {
        if (i == m_panes.size() - 1) {
            m_panes[i]->setFilterState(m_currentFilter);
        } else {
            FilterState parentFilter;
            parentFilter.showHidden = m_currentFilter.showHidden;
            m_panes[i]->setFilterState(parentFilter);
        }
    }
    scrollToRightmostPane();
    return pane;
}

void ColumnViewWidget::clearAllSelections() {
    for (auto* pane : m_panes) {
        if (pane) {
            pane->clearSelection();
        }
    }
    emit selectionChanged();
}

void ColumnViewWidget::toggleFolderSectionCollapse() {
    ColumnViewPane* pane = activePane();
    if (!pane) pane = rightmostPane();
    if (pane && pane->sectionProxyModel()) {
        pane->sectionProxyModel()->setFolderCollapsed(!pane->sectionProxyModel()->isFolderCollapsed());
    }
}

void ColumnViewWidget::clearOtherSelections(int activePaneIdx) {
    for (int i = activePaneIdx + 1; i < m_panes.size(); ++i) {
        m_panes[i]->clearSelection();
    }
}

void ColumnViewWidget::updateParentHighlights() {
    for (int i = 0; i < m_panes.size(); ++i) {
        ColumnViewPane* parentPane = m_panes[i];
        if (!parentPane) continue;

        ColumnViewPane* childPane = (i + 1 < m_panes.size()) ? m_panes[i + 1] : nullptr;
        QString childPath = childPane ? QDir::toNativeSeparators(QDir::cleanPath(childPane->currentPath())) : "";

        parentPane->setExpandedChildPath(childPath);
    }
}

void ColumnViewWidget::updatePaneWidths() {
    if (m_panes.isEmpty()) return;

    // 【架构与设计理念刚性红线】列视图各列列宽严格、永恒固定为 230 像素，严禁任何形式的等比例均分拉伸！
    constexpr int kColumnPaneWidth = 230;
    const int count = m_panes.size();
    const int viewportW = viewport()->width();

    int totalPanesWidth = 0;
    for (int i = 0; i < count; ++i) {
        m_panes[i]->setFixedWidth(kColumnPaneWidth);
        totalPanesWidth += kColumnPaneWidth;
    }

    // 【架构与设计理念刚性红线】最右侧刻意留白画布（ColumnBlankCanvasWidget）：
    // 1. 当列总宽未占满视口时：留白宽度拉伸自适应填补视口剩余所有空间（viewportW - totalPanesWidth）；
    // 2. 物理刚性下限：最右侧留白画布宽度绝对不低于 230px（kColumnPaneWidth），保证双分栏拉窄时留白列不受打折挤压。
    int blankWidth = qMax(viewportW - totalPanesWidth, kColumnPaneWidth);

    int containerHeight = m_container ? m_container->height() : viewport()->height();

    if (m_blankCanvasWidget) {
        m_blankCanvasWidget->setGeometry(totalPanesWidth, 0, blankWidth, qMax(containerHeight, viewport()->height()));
    }
    if (m_container) {
        m_container->setMinimumWidth(totalPanesWidth + blankWidth);
    }
}

void ColumnViewWidget::resizeEvent(QResizeEvent* event) {
    QScrollArea::resizeEvent(event);
    updatePaneWidths();
}

bool ColumnViewWidget::eventFilter(QObject* obj, QEvent* event) {
    if ((obj == viewport() || obj == m_container) && event && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent && mouseEvent->button() == Qt::LeftButton) {
            goUpColumnFromIndex(m_panes.size() - 1);
            return true;
        }
    }
    return QScrollArea::eventFilter(obj, event);
}

void ColumnViewWidget::refreshActiveColumn() {
    ColumnViewPane* pane = activePane();
    if (pane) {
        pane->loadDirectory();
    }
}

void ColumnViewWidget::refreshAllColumns() {
    for (auto* pane : m_panes) {
        if (pane) {
            pane->loadDirectory();
        }
    }
}

void ColumnViewWidget::updateMetadataForPath(const QString& path) {
    for (auto* pane : m_panes) {
        if (pane && pane->model()) {
            pane->model()->updateRecordMetadata(path);
            if (pane->listView() && pane->listView()->viewport()) {
                pane->listView()->viewport()->update();
            }
        }
    }
}

void ColumnViewWidget::setActivePaneIndex(int newIndex) {
    if (newIndex < 0 || newIndex >= m_panes.size()) return;
    m_activePaneIndex = newIndex;
    for (int i = 0; i < m_panes.size(); ++i) {
        if (m_panes[i]) {
            m_panes[i]->setActive(i == newIndex);
        }
    }
}

void ColumnViewWidget::activatePaneFromBlankClick(int paneIndex) {
    if (paneIndex >= 0 && paneIndex < m_panes.size()) {
        setActivePaneIndex(paneIndex);
        ColumnViewPane* pane = m_panes[paneIndex];
        if (pane) {
            pane->clearSelection();
            focusPane(paneIndex);
            emit selectionChanged();
        }
    }
}

void ColumnViewWidget::goUpColumn() {
    goUpColumnFromIndex(m_panes.size() - 1);
}

void ColumnViewWidget::goUpColumnFromIndex(int paneIndex) {
    if (m_panes.isEmpty()) {
        NavigationService::instance().goUp();
        return;
    }

    if (paneIndex >= m_panes.size() - 1) {
        // 双击发生在最右侧列或背景留白处：逐级关闭最右侧列
        if (m_panes.size() > 1) {
            dismissSubColumns(m_panes.size() - 2);
            ColumnViewPane* newActive = rightmostPane();
            if (newActive) {
                setActivePaneIndex(m_panes.size() - 1);
                emit pathNavigated(newActive->currentPath());
                emit selectionChanged();
            }
        } else {
            // 仅剩最后一列（如盘符根目录 G:/）时，降级退回“此电脑”(computer://)
            NavigationService::instance().goUp();
        }
    } else if (paneIndex >= 0) {
        // 双击发生在中间父列的空白处：裁撤该列右侧的所有子列并保持当前父列高亮
        dismissSubColumns(paneIndex);
        ColumnViewPane* newActive = rightmostPane();
        if (newActive) {
            setActivePaneIndex(m_panes.size() - 1);
            emit pathNavigated(newActive->currentPath());
            emit selectionChanged();
        }
    }
}

void ColumnViewWidget::expandFolder(const QString& folderPath, int paneIdx) {
    if (paneIdx + 1 < m_panes.size() &&
        QDir::cleanPath(m_panes[paneIdx + 1]->currentPath()) == QDir::cleanPath(folderPath)) {
        setActivePaneIndex(paneIdx + 1);
        focusPane(paneIdx + 1);
        emit selectionChanged();
        return;
    }

    dismissSubColumns(paneIdx);
    clearOtherSelections(paneIdx);
    appendColumn(folderPath);
    emit pathNavigated(folderPath);
    if (m_contentPanel) {
        m_contentPanel->recalculateAndEmitStats();
    }
}

} // namespace QuarkMeta
