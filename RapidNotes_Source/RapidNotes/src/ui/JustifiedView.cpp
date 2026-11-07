#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "JustifiedView.h"
#include "CardLayoutEngine.h"
#include "UiHelper.h"
#include "models/SectionProxyModel.h"
#include "../core/ModelContract.h"
#include <QPainter>
#include <QScrollBar>
#include <QResizeEvent>
#include <QStyleOptionViewItem>
#include <QAbstractItemDelegate>
#include <QTimer>
#include <algorithm>

namespace QuarkMeta {

JustifiedView::JustifiedView(QWidget* parent) : QAbstractItemView(parent) {
    setFrameShape(QFrame::NoFrame);
    m_layoutTimer = new QTimer(this);
    m_layoutTimer->setSingleShot(true);
    m_layoutTimer->setInterval(50);
    connect(m_layoutTimer, &QTimer::timeout, this, &JustifiedView::onLayoutTimerTimeout);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalScrollBar()->setRange(0, 0);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    verticalScrollBar()->setSingleStep(20);

    setAutoFillBackground(true);
    viewport()->setAutoFillBackground(true);
    viewport()->setAttribute(Qt::WA_OpaquePaintEvent);

    QPalette pal = viewport()->palette();
    pal.setColor(QPalette::Window, QColor("#1E1E1E"));
    viewport()->setPalette(pal);
    setPalette(pal);
}

void JustifiedView::setLayoutMode(LayoutMode mode) {
    if (m_layoutMode != mode) {
        m_layoutMode = mode;
        scheduleLayout();
    }
}

JustifiedView::LayoutMode JustifiedView::layoutMode() const {
    return m_layoutMode;
}

void JustifiedView::setTargetRowHeight(int h) {
    if (m_targetRowHeight != h) {
        m_targetRowHeight = h;
        scheduleLayout();
    }
}

void JustifiedView::setAspectRatioRole(int role) {
    if (m_aspectRatioRole != role) {
        m_aspectRatioRole = role;
        scheduleLayout();
    }
}

void JustifiedView::reset() {
    QAbstractItemView::reset();
    scheduleLayout();
}

void JustifiedView::doItemsLayout() {
    scheduleLayout();
}

void JustifiedView::setModel(QAbstractItemModel* model) {
    if (this->model()) {
        disconnect(this->model(), &QAbstractItemModel::rowsRemoved, this, nullptr);
        disconnect(this->model(), &QAbstractItemModel::modelReset, this, nullptr);
        disconnect(this->model(), &QAbstractItemModel::layoutChanged, this, nullptr);
    }
    QAbstractItemView::setModel(model);
    if (model) {
        connect(model, &QAbstractItemModel::rowsRemoved, this, [this]() {
            doLayout();
        });
        connect(model, &QAbstractItemModel::modelReset, this, [this]() {
            doLayout();
        });
        connect(model, &QAbstractItemModel::layoutChanged, this, [this]() {
            doLayout();
        });
    }
}

void JustifiedView::scheduleLayout() {
    m_layoutDirty = true;
    if (m_layoutTimer && !m_layoutTimer->isActive()) {
        m_layoutTimer->start();
    }
}

void JustifiedView::onLayoutTimerTimeout() {
    if (m_layoutDirty) {
        doLayout();
    }
}

QRect JustifiedView::visualRect(const QModelIndex& index) const {
    if (!index.isValid() || index.row() >= (int)m_geometries.size()) return QRect();
    QRect r = m_geometries[index.row()].rect;
    r.translate(0, -verticalScrollBar()->value());
    return r;
}

void JustifiedView::scrollTo(const QModelIndex& index, ScrollHint hint) {
    QRect rect = visualRect(index);
    if (rect.isEmpty()) return;

    int viewportHeight = viewport()->height();
    int scrollValue = verticalScrollBar()->value();

    if (hint == EnsureVisible) {
        if (rect.top() < 0) verticalScrollBar()->setValue(scrollValue + rect.top());
        else if (rect.bottom() > viewportHeight) verticalScrollBar()->setValue(scrollValue + rect.bottom() - viewportHeight);
    }
}

QModelIndex JustifiedView::indexAt(const QPoint& point) const {
    if (m_geometries.empty()) return QModelIndex();
    int y = point.y() + verticalScrollBar()->value();

    auto it = std::lower_bound(m_geometries.begin(), m_geometries.end(), y,
        [](const ItemGeometry& geo, int targetY) {
            return geo.rect.bottom() < targetY;
        });

    for (; it != m_geometries.end(); ++it) {
        if (it->rect.top() > y) break;
        if (it->rect.contains(point.x(), y)) {
            return model()->index(it->index, 0);
        }
    }
    return QModelIndex();
}

void JustifiedView::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles) {
    if (roles.isEmpty() || roles.contains(m_aspectRatioRole) || roles.contains(Qt::DecorationRole) || roles.contains(ColorRole)) {
        scheduleLayout();
    }
    viewport()->update();
    QAbstractItemView::dataChanged(topLeft, bottomRight, roles);
}

void JustifiedView::rowsInserted(const QModelIndex& parent, int start, int end) {
    scheduleLayout();
    QAbstractItemView::rowsInserted(parent, start, end);
}

void JustifiedView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) {
    doLayout();
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}

QModelIndex JustifiedView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers) {
    QModelIndex current = currentIndex();
    if (!current.isValid()) return model()->index(0, 0);

    int row = current.row();
    int count = (int)m_geometries.size();
    if (row < 0 || row >= count) return current;

    if (cursorAction == MoveLeft) {
        row = std::max(0, row - 1);
    } else if (cursorAction == MoveRight) {
        row = std::min(count - 1, row + 1);
    } else if (cursorAction == MoveUp || cursorAction == MoveDown) {
        QRect currentRect = m_geometries[row].rect;
        int centerX = currentRect.center().x();
        int bestIdx = -1;
        int minDistance = 1000000;

        for (int i = 0; i < count; ++i) {
            if (i == row) continue;
            QRect targetRect = m_geometries[i].rect;

            if (cursorAction == MoveUp && targetRect.bottom() < currentRect.top()) {
                int dy = currentRect.top() - targetRect.bottom();
                int dx = std::abs(targetRect.center().x() - centerX);
                int dist = dy * 100 + dx;
                if (dist < minDistance) {
                    minDistance = dist;
                    bestIdx = i;
                }
            } else if (cursorAction == MoveDown && targetRect.top() > currentRect.bottom()) {
                int dy = targetRect.top() - currentRect.bottom();
                int dx = std::abs(targetRect.center().x() - centerX);
                int dist = dy * 100 + dx;
                if (dist < minDistance) {
                    minDistance = dist;
                    bestIdx = i;
                }
            }
        }
        if (bestIdx != -1) row = bestIdx;
    }

    return model()->index(row, 0);
}

int JustifiedView::horizontalOffset() const { return 0; }
int JustifiedView::verticalOffset() const { return verticalScrollBar()->value(); }
bool JustifiedView::isIndexHidden(const QModelIndex&) const { return false; }

void JustifiedView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command) {
    QRect contentsRect = rect.translated(0, verticalScrollBar()->value());
    QItemSelection selection;
    for (const auto& geo : m_geometries) {
        if (geo.isHeader) continue;
        if (geo.rect.intersects(contentsRect)) {
            QModelIndex idx = model()->index(geo.index, 0);
            selection.select(idx, idx);
        }
    }
    selectionModel()->select(selection, command);
}

QRegion JustifiedView::visualRegionForSelection(const QItemSelection& selection) const {
    QRegion region;
    for (const auto& range : selection) {
        for (int i = range.top(); i <= range.bottom(); ++i) {
            region += visualRect(model()->index(i, 0));
        }
    }
    return region;
}

void JustifiedView::mousePressEvent(QMouseEvent* event) {
    QModelIndex pressIdx = indexAt(event->pos());
    if (pressIdx.isValid() && pressIdx.data(SectionHeaderRole).toBool()) {
        if (event->button() == Qt::LeftButton) {
            bool inInteractiveBounds = true;
            if (pressIdx.data(SectionKindRole).toInt() == 1) {
                int visualIdx = pressIdx.row();
                if (visualIdx >= 0 && visualIdx < static_cast<int>(m_geometries.size())) {
                    const auto& geo = m_geometries[visualIdx];
                    QFont font("Microsoft YaHei", 10, QFont::Bold);
                    QFontMetrics fm(font);
                    int leftX = geo.rect.left() + 10;
                    int textW = fm.horizontalAdvance(geo.headerText);
                    int interactiveMaxX = leftX + textW + 30; // 文本宽度 + 箭头图标 + 容差 Padding
                    if (event->pos().x() < leftX || event->pos().x() > interactiveMaxX) {
                        inInteractiveBounds = false;
                    }
                }
            }

            if (inInteractiveBounds) {
                if (pressIdx.data(SectionKindRole).toInt() == 1) {
                    SectionProxyModel* secModel = m_sectionModel ? m_sectionModel : qobject_cast<SectionProxyModel*>(model());
                    if (secModel) {
                        secModel->setFolderCollapsed(!secModel->isFolderCollapsed());
                    }
                }
                event->accept();
                return;
            } else {
                pressIdx = QModelIndex(); // 超出折叠组件热区，视为点击同行空白区域，放行给后续框选/取消选中逻辑
            }
        }
    }

    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        QModelIndex idx = pressIdx;
        if (!idx.isValid()) {
            m_isDraggingSelection = true;
            m_dragStartPos = event->pos();
            m_selectionRect = QRect();
            selectionModel()->clearSelection();
            event->accept();
            return;
        }
    }

    if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) {
        QModelIndex clicked = indexAt(event->pos());
        if (clicked.isValid() && !clicked.data(SectionHeaderRole).toBool() && m_anchorRow >= 0) {
            int anchorVisual = -1, clickedVisual = -1;
            for (int i = 0; i < (int)m_geometries.size(); ++i) {
                if (m_geometries[i].index == m_anchorRow)      anchorVisual = i;
                if (m_geometries[i].index == clicked.row())    clickedVisual = i;
            }
            if (anchorVisual >= 0 && clickedVisual >= 0) {
                int vFrom = std::min(anchorVisual, clickedVisual);
                int vTo   = std::max(anchorVisual, clickedVisual);
                QItemSelection sel;
                for (int v = vFrom; v <= vTo; ++v) {
                    if (m_geometries[v].isHeader) continue;
                    QModelIndex idx = model()->index(m_geometries[v].index, 0);
                    sel.select(idx, idx);
                }
                selectionModel()->select(sel, QItemSelectionModel::ClearAndSelect);
                selectionModel()->setCurrentIndex(clicked, QItemSelectionModel::NoUpdate);
                event->accept();
                viewport()->update();
                return;
            }
        }
    }

    QAbstractItemView::mousePressEvent(event);
    QModelIndex current = currentIndex();
    if (current.isValid() && !current.data(SectionHeaderRole).toBool()) {
        m_anchorRow = current.row();
    } else {
        m_anchorRow = -1;
    }
    viewport()->update();
}

void JustifiedView::mouseMoveEvent(QMouseEvent* event) {
    if (m_isDraggingSelection) {
        m_selectionRect = QRect(m_dragStartPos, event->pos()).normalized();
        setSelection(m_selectionRect, QItemSelectionModel::ClearAndSelect);
        viewport()->update();
        event->accept();
        return;
    }
    QAbstractItemView::mouseMoveEvent(event);
}

void JustifiedView::mouseReleaseEvent(QMouseEvent* event) {
    if (m_isDraggingSelection) {
        m_isDraggingSelection = false;
        m_selectionRect = QRect();
        viewport()->update();
        event->accept();
        return;
    }
    QAbstractItemView::mouseReleaseEvent(event);
}

void JustifiedView::mouseDoubleClickEvent(QMouseEvent* event) {
    QModelIndex idx = indexAt(event->pos());
    if (!idx.isValid()) {
        QAbstractItemView::mouseDoubleClickEvent(event);
        return;
    }

    if (idx.data(SectionHeaderRole).toBool()) {
        event->accept();
        return;
    }

    // 核心架构意图：双击 = 打开 / 预览（绝无编辑副作用）
    emit doubleClicked(idx);
}

void JustifiedView::paintEvent(QPaintEvent* event) {
    QPainter painter(viewport());
    painter.fillRect(viewport()->rect(), QColor("#1E1E1E"));

    if (m_geometries.empty()) {
        painter.save();
        painter.setPen(QColor("#888888"));
        painter.setFont(QFont("Microsoft YaHei", 12));
        painter.drawText(viewport()->rect(), Qt::AlignCenter, "没有可显示的项目");
        painter.restore();
        return;
    }

    painter.save();
    int scrollY = verticalScrollBar()->value();
    painter.translate(0, -scrollY);

    QRect dirtyRect = event->rect().translated(0, scrollY);
    int dirtyTop = dirtyRect.top();
    int dirtyBottom = dirtyRect.bottom();

    auto startIt = std::lower_bound(m_geometries.begin(), m_geometries.end(), dirtyTop,
        [](const ItemGeometry& geo, int targetY) {
            return geo.rect.bottom() < targetY;
        });

    for (auto it = startIt; it != m_geometries.end(); ++it) {
        const auto& geo = *it;
        if (geo.rect.top() > dirtyBottom) break;

        if (geo.isHeader) {
            painter.save();
            painter.setPen(QColor("#3498db"));
            QFont font("Microsoft YaHei", 10, QFont::Bold);
            painter.setFont(font);

            QFontMetrics fm(font);
            int leftX = geo.rect.left() + 10;
            int textW = fm.horizontalAdvance(geo.headerText);

            QRect textRect(leftX, geo.rect.top(), textW + 10, geo.rect.height());
            painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, geo.headerText);

            if (geo.isFolderHeader) {
                int iconSize = 14;
                int iconX = leftX + textW + 6;
                QRect iconRect(iconX, geo.rect.top() + (geo.rect.height() - iconSize) / 2, iconSize, iconSize);
                QIcon arrowIcon = UiHelper::getIcon(geo.isCollapsed ? "chevron_right" : "chevron_down", QColor("#3498db"), iconSize);
                arrowIcon.paint(&painter, iconRect, Qt::AlignCenter);
            }

            painter.restore();
            continue;
        }

        QModelIndex idx = model()->index(geo.index, 0);
        if (!idx.isValid()) continue;

        QStyleOptionViewItem option;
        initViewItemOption(&option);
        option.rect = geo.rect;

        if (selectionModel()->isSelected(idx))
            option.state |= QStyle::State_Selected;
        if (currentIndex() == idx)
            option.state |= QStyle::State_HasFocus;

        itemDelegateForIndex(idx)->paint(&painter, option, idx);
    }
    painter.restore();

    if (m_isDraggingSelection && !m_selectionRect.isEmpty()) {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, false);
        QColor highlightColor = QColor("#378ADD");
        QColor brushColor = highlightColor;
        brushColor.setAlpha(80);
        painter.setBrush(brushColor);
        painter.setPen(QPen(highlightColor, 1, Qt::SolidLine));
        painter.drawRect(m_selectionRect);
        painter.restore();
    }
}

void JustifiedView::resizeEvent(QResizeEvent* event) {
    doLayout();
    QAbstractItemView::resizeEvent(event);
}

void JustifiedView::updateGeometries() {
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(0, std::max(0, m_totalHeight - viewport()->height()));
    QAbstractItemView::updateGeometries();
}

void JustifiedView::doLayout() {
    m_layoutDirty = false;
    if (!model()) return;
    int count = model()->rowCount();

    if (count == 0) {
        m_geometries.clear();
        m_totalHeight = 0;
        updateGeometries();
        viewport()->update();
        return;
    }

    // 🚀【精确数学对齐】：边距微调为 6px，可用宽度精准扣减 16px 滚动条
    const int margin = 6;
    const int spacing = 5;

    int scrollBarW = (verticalScrollBar() && verticalScrollBar()->isVisible()) ? verticalScrollBar()->width() : 0;
    int containerWidth = width() - scrollBarW - (margin * 2);
    if (containerWidth <= 0) return;

    m_geometries.resize(count);
    int currentY = margin;

    const int cardPadding = CardLayoutEngine::totalPaddingHorizontal();
    const int extraHeight = CardLayoutEngine::extraHeight();

    if (m_layoutMode == GridMode) {
        int itemWidth = m_targetRowHeight + cardPadding;
        int itemHeight = m_targetRowHeight + extraHeight;

        int maxNumInRow = (containerWidth + spacing) / (itemWidth + spacing);
        if (maxNumInRow <= 0) maxNumInRow = 1;

        int standardSpacing = spacing;
        if (maxNumInRow > 1) {
            standardSpacing = (containerWidth - (maxNumInRow * itemWidth)) / (maxNumInRow - 1);
        }

        int i = 0;
        while (i < count) {
            QModelIndex idx = model()->index(i, 0);
            if (idx.data(SectionHeaderRole).toBool()) {
                int headerH = 28;
                bool isFolderHeader = (idx.data(SectionKindRole).toInt() == 1);
                m_geometries[i] = { QRect(margin, currentY, containerWidth, headerH), i, true, idx.data(SectionHeaderTextRole).toString(), idx.data(SectionCollapsedRole).toBool(), isFolderHeader };
                currentY += headerH + spacing;
                i++;
                continue;
            }

            int rowStart = i;
            int numInRow = 0;
            while (i < count && numInRow < maxNumInRow) {
                QModelIndex itemIdx = model()->index(i, 0);
                if (itemIdx.data(SectionHeaderRole).toBool()) {
                    break;
                }
                numInRow++;
                i++;
            }

            int currentX = margin;
            if (maxNumInRow == 1) {
                currentX = margin + std::max(0, (containerWidth - itemWidth) / 2);
            }

            for (int j = 0; j < numInRow; ++j) {
                int itemIdx = rowStart + j;
                m_geometries[itemIdx] = { QRect(currentX, currentY, itemWidth, itemHeight), itemIdx, false, QString(), false };
                currentX += itemWidth + standardSpacing;
            }
            currentY += itemHeight;
            if (i < count) {
                currentY += spacing;
            }
        }
    } else {
        int i = 0;
        while (i < count) {
            QModelIndex idx = model()->index(i, 0);
            if (idx.data(SectionHeaderRole).toBool()) {
                int headerH = 28;
                bool isFolderHeader = (idx.data(SectionKindRole).toInt() == 1);
                m_geometries[i] = { QRect(margin, currentY, containerWidth, headerH), i, true, idx.data(SectionHeaderTextRole).toString(), idx.data(SectionCollapsedRole).toBool(), isFolderHeader };
                currentY += headerH + spacing;
                i++;
                continue;
            }

            int rowStart = i;
            double rowAspectRatioSum = 0;
            std::vector<double> aspectRatios;

            bool forceBreak = false;
            while (i < count) {
                QModelIndex curIdx = model()->index(i, 0);
                if (curIdx.data(SectionHeaderRole).toBool()) {
                    forceBreak = true;
                    break;
                }

                double ar = curIdx.data(m_aspectRatioRole).toDouble();
                if (ar <= 0) ar = 1.0;

                aspectRatios.push_back(ar);
                rowAspectRatioSum += ar;

                int numInRow = (int)aspectRatios.size();
                double estimatedWidth = (rowAspectRatioSum * m_targetRowHeight) + (cardPadding * numInRow) + (spacing * (numInRow - 1));
                if (estimatedWidth > containerWidth) {
                    if (numInRow > 1) {
                        aspectRatios.pop_back();
                        rowAspectRatioSum -= ar;
                    } else {
                        i++;
                    }
                    break;
                }
                i++;
            }

            int rowEnd = i;
            int numInRow = rowEnd - rowStart;
            if (numInRow <= 0) break;

            int actualHeight = m_targetRowHeight;
            bool isLastRow = (i == count);
            bool rowIsJustified = !isLastRow && !forceBreak;

            int availableImageWidth = containerWidth - (spacing * (numInRow - 1)) - (cardPadding * numInRow);

            if (rowIsJustified) {
                actualHeight = qRound(availableImageWidth / rowAspectRatioSum);
                actualHeight = std::max(actualHeight, (int)(m_targetRowHeight * 0.75));
                actualHeight = std::min(actualHeight, (int)(m_targetRowHeight * 1.5));
                rowIsJustified = true;
            }

            int currentX = margin;

            for (int j = 0; j < numInRow; ++j) {
                int itemIdx = rowStart + j;
                int itemWidth;

                if (j == numInRow - 1 && rowIsJustified) {
                    itemWidth = (containerWidth + margin) - currentX;
                } else {
                    itemWidth = qRound(aspectRatios[j] * actualHeight) + cardPadding;
                }

                m_geometries[itemIdx] = { QRect(currentX, currentY, itemWidth, actualHeight + extraHeight), itemIdx, false, QString(), false };
                currentX += itemWidth + spacing;
            }
            currentY += actualHeight + extraHeight + spacing;
        }
    }

    int oldHeight = m_totalHeight;
    m_totalHeight = currentY;
    updateGeometries();
    viewport()->update();

    if (oldHeight != m_totalHeight) {
        emit totalHeightChanged(m_totalHeight);
    }
    emit layoutFinished();
}

bool JustifiedView::isLayoutReady() const {
    if (m_layoutDirty) return false;
    if (!model()) return true;
    return static_cast<int>(m_geometries.size()) >= model()->rowCount();
}

QList<int> JustifiedView::rowsInRange(int top, int bottom) const {
    QList<int> rows;
    if (!isLayoutReady() || m_geometries.empty()) return rows;

    auto startIt = std::lower_bound(m_geometries.begin(), m_geometries.end(), top,
        [](const ItemGeometry& geo, int targetY) {
            return geo.rect.bottom() < targetY;
        });

    for (auto it = startIt; it != m_geometries.end(); ++it) {
        const auto& geo = *it;
        if (geo.rect.top() > bottom) break;
        if (!geo.isHeader) {
            rows.append(geo.index);
        }
    }
    return rows;
}

} // namespace QuarkMeta