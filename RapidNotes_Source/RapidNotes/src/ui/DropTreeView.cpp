#include "DropTreeView.h"
#include "ViewDragDropHelper.h"
#include "models/SectionProxyModel.h"
#include "../core/ModelContract.h"
#include "ContentPanel.h"
#include <QPainter>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QAbstractProxyModel>
#include <QScrollBar>
#include <QStringList>
#include <QFileInfo>
#include <QDrag>
#include <QMimeData>
#include <QUrl>
#include <QApplication>
#include <algorithm>
#include "Logger.h"

namespace QuarkMeta {

// 声明式规则表：单一真理来源 (Single Source of Truth)
static const std::vector<ColumnPolicy> kFileListColumnPolicies = {
    { FileListColumn::Name,         0,   QHeaderView::Fixed, false },
    { FileListColumn::Status,       40,  QHeaderView::Fixed, true  },
    { FileListColumn::Rating,       100, QHeaderView::Fixed, false },
    { FileListColumn::Dimension,    100, QHeaderView::Fixed, false },
    { FileListColumn::Type,         60,  QHeaderView::Fixed, false },
    { FileListColumn::Size,         80,  QHeaderView::Fixed, false },
    { FileListColumn::CreatedDate,  130, QHeaderView::Fixed, false },
    { FileListColumn::ModifiedDate, 130, QHeaderView::Fixed, false },
};

DropTreeView::DropTreeView(QWidget* parent) : QTreeView(parent) {
    setHeader(new ContentHeaderView(Qt::Horizontal, this));
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    DragDropEventFilter::install(this);

    // 🚀【强力锁定 QPalette】：强制设定暗色 Base 与 AlternateBase，防止原生 Windows 调色板在交替行露白
    QPalette pal = palette();
    pal.setColor(QPalette::Base, QColor("#1E1E1E"));
    pal.setColor(QPalette::AlternateBase, QColor("#252526"));
    pal.setColor(QPalette::Text, QColor("#EEEEEE"));
    pal.setColor(QPalette::WindowText, QColor("#EEEEEE"));
    setPalette(pal);
    if (viewport()) {
        viewport()->setPalette(pal);
    }

    m_edgeScrollTimer = new QTimer(this);
    connect(m_edgeScrollTimer, &QTimer::timeout, this, [this]() {
        if (m_edgeScrollDelta != 0 && verticalScrollBar()) {
            verticalScrollBar()->setValue(verticalScrollBar()->value() + m_edgeScrollDelta);
        }
    });
}

void DropTreeView::setInternalReorderEnabled(bool enabled) {
    m_internalReorderEnabled = enabled;
}

void DropTreeView::setInternalDragValidator(std::function<bool(const QModelIndex&)> canDragNode) {
    m_internalDragValidator = canDragNode;
}

void DropTreeView::setInternalDropValidator(std::function<bool(const QList<QPersistentModelIndex>&, const QModelIndex&, InternalDropZone)> canDrop) {
    m_internalDropValidator = canDrop;
}

void DropTreeView::setModel(QAbstractItemModel* model) {
    if (this->model()) {
        disconnect(this->model(), &QAbstractItemModel::modelReset, this, &DropTreeView::updateGroupHeaderSpanning);
        disconnect(this->model(), &QAbstractItemModel::layoutChanged, this, &DropTreeView::updateGroupHeaderSpanning);
        disconnect(this->model(), &QAbstractItemModel::rowsInserted, this, &DropTreeView::updateGroupHeaderSpanning);
    }
    QTreeView::setModel(model);
    if (model) {
        connect(model, &QAbstractItemModel::modelReset, this, &DropTreeView::updateGroupHeaderSpanning);
        connect(model, &QAbstractItemModel::layoutChanged, this, &DropTreeView::updateGroupHeaderSpanning);
        connect(model, &QAbstractItemModel::rowsInserted, this, &DropTreeView::updateGroupHeaderSpanning);
        updateGroupHeaderSpanning();
    }
}

void DropTreeView::updateGroupHeaderSpanning() {
    if (!model()) return;
    int total = model()->rowCount();
    for (int r = 0; r < total; ++r) {
        QModelIndex idx = model()->index(r, 0);
        if (idx.data(SectionHeaderRole).toBool()) {
            setFirstColumnSpanned(r, QModelIndex(), true);
        }
    }
}

void DropTreeView::mousePressEvent(QMouseEvent* event) {
    QModelIndex idx = indexAt(event->pos());
    if (idx.isValid() && idx.data(SectionHeaderRole).toBool()) {
        if (event->button() == Qt::LeftButton) {
            bool inInteractiveBounds = true;
            if (idx.data(SectionKindRole).toInt() == 1) {
                QRect rect = visualRect(idx);
                QFont font("Microsoft YaHei", 10, QFont::Bold);
                QFontMetrics fm(font);
                int leftX = rect.left() + 10;
                int textW = fm.horizontalAdvance(idx.data(SectionHeaderTextRole).toString());
                int interactiveMaxX = leftX + textW + 30; // 文本宽度 + 箭头图标 + 容差 Padding
                if (event->pos().x() < leftX || event->pos().x() > interactiveMaxX) {
                    inInteractiveBounds = false;
                }
            }

            if (inInteractiveBounds) {
                if (idx.data(SectionKindRole).toInt() == 1) {
                    SectionProxyModel* secModel = nullptr;
                    QAbstractItemModel* cur = model();
                    while (cur) {
                        secModel = qobject_cast<SectionProxyModel*>(cur);
                        if (secModel) break;
                        auto* proxy = qobject_cast<QAbstractProxyModel*>(cur);
                        if (proxy) cur = proxy->sourceModel();
                        else break;
                    }
                    if (secModel) {
                        secModel->setFolderCollapsed(!secModel->isFolderCollapsed());
                    }
                }
                event->accept();
                return;
            }
        }
    }
    QTreeView::mousePressEvent(event);
}

void DropTreeView::startDrag(Qt::DropActions supportedActions) {
    if (!m_internalReorderEnabled) {
        ViewDragDropHelper::executeStartDrag(this, supportedActions);
        return;
    }

    if (!selectionModel()) return;

    QModelIndexList selected = selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    // 只取第 0 列，去重
    QList<QModelIndex> col0Indexes;
    for (const QModelIndex& idx : selected) {
        if (idx.column() == 0 && !col0Indexes.contains(idx)) {
            col0Indexes.append(idx);
        }
    }
    if (col0Indexes.isEmpty()) return;

    // 若某条目的祖先也在选中里，则只保留祖先
    QList<QModelIndex> filteredIndexes;
    for (const QModelIndex& idx : col0Indexes) {
        bool ancestorSelected = false;
        QModelIndex p = idx.parent();
        while (p.isValid()) {
            if (col0Indexes.contains(p)) {
                ancestorSelected = true;
                break;
            }
            p = p.parent();
        }
        if (!ancestorSelected) {
            filteredIndexes.append(idx);
        }
    }

    // 逐个用 canDragNode 校验，任何一个不通过则不开始拖拽
    if (m_internalDragValidator) {
        for (const QModelIndex& idx : filteredIndexes) {
            if (!m_internalDragValidator(idx)) {
                return;
            }
        }
    }

    m_internalDragSources.clear();
    QList<QUrl> urls;
    for (const QModelIndex& idx : filteredIndexes) {
        m_internalDragSources.append(QPersistentModelIndex(idx));

        QString path = idx.data(PathRole).toString();
        if (path.isEmpty()) {
            path = idx.data(Qt::UserRole + 1).toString();
        }
        if (!path.isEmpty() && QFileInfo::exists(path)) {
            urls.append(QUrl::fromLocalFile(path));
        }
    }

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
    clearInternalDropIndicator();
    stopEdgeScroll();
}

InternalDropZone DropTreeView::calculateDropZone(const QPoint& pos, QModelIndex& outTargetIndex) const {
    outTargetIndex = indexAt(pos);
    if (!outTargetIndex.isValid()) {
        return InternalDropZone::Blank;
    }

    QRect rect = visualRect(outTargetIndex);
    int top25 = rect.top() + rect.height() * 0.25;
    int bottom25 = rect.bottom() - rect.height() * 0.25;

    if (pos.y() < top25) {
        return InternalDropZone::Above;
    } else if (pos.y() > bottom25) {
        return InternalDropZone::Below;
    } else {
        return InternalDropZone::OnItem;
    }
}

void DropTreeView::clearInternalDropIndicator() {
    m_showIndicator = false;
    m_indicatorIndex = QModelIndex();
    m_indicatorZone = InternalDropZone::Blank;
    ViewDragDropHelper::clearHover(this);
    if (viewport()) viewport()->update();
}

void DropTreeView::updateEdgeScroll(const QPoint& pos) {
    if (!viewport()) return;
    int vpHeight = viewport()->height();
    int margin = 24;

    if (pos.y() < margin) {
        m_edgeScrollDelta = -10;
        if (!m_edgeScrollTimer->isActive()) m_edgeScrollTimer->start(30);
    } else if (pos.y() > vpHeight - margin) {
        m_edgeScrollDelta = 10;
        if (!m_edgeScrollTimer->isActive()) m_edgeScrollTimer->start(30);
    } else {
        stopEdgeScroll();
    }
}

void DropTreeView::stopEdgeScroll() {
    m_edgeScrollDelta = 0;
    if (m_edgeScrollTimer && m_edgeScrollTimer->isActive()) {
        m_edgeScrollTimer->stop();
    }
}

void DropTreeView::dragEnterEvent(QDragEnterEvent* event) {
    if (m_internalReorderEnabled && event->mimeData() && event->mimeData()->hasFormat("application/x-quarkmeta-tree-nodes")) {
        event->acceptProposedAction();
        return;
    }
    QTreeView::dragEnterEvent(event);
}

void DropTreeView::dragMoveEvent(QDragMoveEvent* event) {
    if (m_internalReorderEnabled && event->mimeData() && event->mimeData()->hasFormat("application/x-quarkmeta-tree-nodes")) {
        // 校验 sources 是否含有失效索引
        for (const QPersistentModelIndex& pidx : m_internalDragSources) {
            if (!pidx.isValid()) {
                clearInternalDropIndicator();
                stopEdgeScroll();
                event->ignore();
                return;
            }
        }

        updateEdgeScroll(event->position().toPoint());

        QModelIndex targetIdx;
        InternalDropZone zone = calculateDropZone(event->position().toPoint(), targetIdx);

        bool valid = true;
        if (m_internalDropValidator) {
            valid = m_internalDropValidator(m_internalDragSources, targetIdx, zone);
        }

        if (valid) {
            m_showIndicator = true;
            m_indicatorIndex = targetIdx;
            m_indicatorZone = zone;

            if (zone == InternalDropZone::OnItem && targetIdx.isValid()) {
                ViewDragDropHelper::setHoverTarget(this, targetIdx);
            } else {
                ViewDragDropHelper::clearHover(this);
            }

            if (viewport()) viewport()->update();
            event->acceptProposedAction();
        } else {
            clearInternalDropIndicator();
            event->ignore();
        }
        return;
    }

    QTreeView::dragMoveEvent(event);
}

void DropTreeView::dragLeaveEvent(QDragLeaveEvent* event) {
    if (m_internalReorderEnabled) {
        clearInternalDropIndicator();
        stopEdgeScroll();
    }
    QTreeView::dragLeaveEvent(event);
}

void DropTreeView::dropEvent(QDropEvent* event) {
    if (m_internalReorderEnabled && event->mimeData() && event->mimeData()->hasFormat("application/x-quarkmeta-tree-nodes")) {
        stopEdgeScroll();

        QModelIndex targetIdx;
        InternalDropZone zone = calculateDropZone(event->position().toPoint(), targetIdx);

        bool valid = true;
        if (m_internalDropValidator) {
            valid = m_internalDropValidator(m_internalDragSources, targetIdx, zone);
        }

        if (valid) {
            emit internalMoveRequested(m_internalDragSources, targetIdx, zone);
            event->acceptProposedAction();
        } else {
            event->ignore();
        }

        clearInternalDropIndicator();
        return;
    }

    QTreeView::dropEvent(event);
}

void DropTreeView::applyColumnPolicies() {
    if (m_isApplyingPolicies) return;
    m_isApplyingPolicies = true;

    QHeaderView* hdr = header();
    if (!hdr) {
        m_isApplyingPolicies = false;
        return;
    }

    hdr->setMinimumSectionSize(0);

    int totalVisibleFixedWidth = 0;
    for (const auto& policy : kFileListColumnPolicies) {
        if (policy.alwaysHidden) continue;
        if (policy.column != FileListColumn::Name) {
            totalVisibleFixedWidth += policy.fixedWidth;
        }
    }

    int availWidth = viewport() ? viewport()->width() : width();
    int minPaneW = ContentPanel::kMinPaneWidth;
    int calculatedNameWidth = std::max(minPaneW, availWidth - totalVisibleFixedWidth);

    for (const auto& policy : kFileListColumnPolicies) {
        int colIdx = static_cast<int>(policy.column);
        if (policy.alwaysHidden) {
            setColumnHidden(colIdx, true);
            continue;
        }

        setColumnHidden(colIdx, false);
        hdr->setSectionResizeMode(colIdx, QHeaderView::Fixed);

        if (policy.column == FileListColumn::Name) {
            hdr->resizeSection(colIdx, calculatedNameWidth);
        } else {
            hdr->resizeSection(colIdx, policy.fixedWidth);
        }
    }

    m_isApplyingPolicies = false;
}

void DropTreeView::resizeEvent(QResizeEvent* event) {
    QTreeView::resizeEvent(event);
    applyColumnPolicies();
}

void DropTreeView::keyboardSearch(const QString& search) {
    Q_UNUSED(search);
}

void DropTreeView::paintEvent(QPaintEvent* event) {
    QTreeView::paintEvent(event);

    if (m_internalReorderEnabled && m_showIndicator && m_indicatorZone != InternalDropZone::OnItem) {
        QPainter painter(viewport());
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, false);

        QPen pen(QColor("#3498db"), 2);
        painter.setPen(pen);

        int vpWidth = viewport()->width();

        if (m_indicatorZone == InternalDropZone::Above && m_indicatorIndex.isValid()) {
            QRect rect = visualRect(m_indicatorIndex);
            painter.drawLine(0, rect.top(), vpWidth, rect.top());
        } else if (m_indicatorZone == InternalDropZone::Below && m_indicatorIndex.isValid()) {
            QRect rect = visualRect(m_indicatorIndex);
            painter.drawLine(0, rect.bottom(), vpWidth, rect.bottom());
        } else if (m_indicatorZone == InternalDropZone::Blank) {
            int lastY = 0;
            if (model() && model()->rowCount() > 0) {
                QModelIndex lastIdx = model()->index(model()->rowCount() - 1, 0);
                if (lastIdx.isValid()) {
                    lastY = visualRect(lastIdx).bottom();
                }
            }
            if (lastY <= 0) lastY = 2;
            painter.drawLine(0, lastY, vpWidth, lastY);
        }

        painter.restore();
    }

    if (!m_emptyHint.isEmpty() && model() && model()->rowCount() == 0) {
        QPainter painter(viewport());
        painter.save();
        painter.setPen(QColor("#888888"));
        painter.setFont(QFont("Microsoft YaHei", 12));
        painter.drawText(viewport()->rect(), Qt::AlignCenter, m_emptyHint);
        painter.restore();
    }
}

} // namespace QuarkMeta
