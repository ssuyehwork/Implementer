#include "FlowLayout.h"
#include <QWidget>

namespace QuarkMeta {

FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing) { setContentsMargins(margin, margin, margin, margin); }
FlowLayout::~FlowLayout() { QLayoutItem *item; while ((item = takeAt(0))) delete item; }
void FlowLayout::addItem(QLayoutItem *item) { itemList.append(item); invalidate(); }
int FlowLayout::horizontalSpacing() const { return m_hSpace >= 0 ? m_hSpace : 4; }
int FlowLayout::verticalSpacing() const { return m_vSpace >= 0 ? m_vSpace : 4; }
int FlowLayout::count() const { return itemList.size(); }
QLayoutItem *FlowLayout::itemAt(int index) const { return itemList.value(index); }
QLayoutItem *FlowLayout::takeAt(int index) { return (index >= 0 && index < itemList.size()) ? itemList.takeAt(index) : nullptr; }
Qt::Orientations FlowLayout::expandingDirections() const { return Qt::Orientations(); }
bool FlowLayout::hasHeightForWidth() const { return true; }
int FlowLayout::heightForWidth(int width) const { return doLayout(QRect(0, 0, width, 0), true); }
int FlowLayout::minimumHeightForWidth(int width) const { return heightForWidth(width); }
void FlowLayout::setGeometry(const QRect &rect) { QLayout::setGeometry(rect); doLayout(rect, false); }
QSize FlowLayout::sizeHint() const { return minimumSize(); }
QSize FlowLayout::minimumSize() const {
    QSize size;
    for (const QLayoutItem *item : itemList) {
        size = size.expandedTo(item->minimumSize());
    }
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    size += QSize(left + right, top + bottom);
    return size;
}
int FlowLayout::doLayout(const QRect &rect, bool testOnly) const {
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int y = effectiveRect.y();
    int lineHeight = 0;

    struct LineItem {
        QLayoutItem* item;
        int width;
        int height;
    };
    QList<LineItem> currentLine;

    auto flushLine = [this, &effectiveRect, &y, &lineHeight, testOnly](QList<LineItem>& line, int lineWidth) {
        if (line.isEmpty()) return;
        int xOffset = 0;
        if (m_alignment & Qt::AlignHCenter) {
            xOffset = qMax(0, (effectiveRect.width() - lineWidth) / 2);
        } else if (m_alignment & Qt::AlignRight) {
            xOffset = qMax(0, effectiveRect.width() - lineWidth);
        }

        int currX = effectiveRect.x() + xOffset;
        for (const auto& li : line) {
            if (!testOnly) {
                li.item->setGeometry(QRect(QPoint(currX, y), li.item->sizeHint()));
            }
            currX += li.width + horizontalSpacing();
        }
        y += lineHeight + verticalSpacing();
        line.clear();
        lineHeight = 0;
    };

    for (QLayoutItem *item : itemList) {
        int itemW = item->sizeHint().width();
        int itemH = item->sizeHint().height();

        int currentLineWidth = 0;
        for (const auto& li : currentLine) {
            currentLineWidth += li.width + horizontalSpacing();
        }

        if (!currentLine.isEmpty() && (currentLineWidth + itemW > effectiveRect.width())) {
            flushLine(currentLine, currentLineWidth - horizontalSpacing());
        }

        currentLine.append({item, itemW, itemH});
        lineHeight = qMax(lineHeight, itemH);
    }

    if (!currentLine.isEmpty()) {
        int currentLineWidth = 0;
        for (const auto& li : currentLine) {
            currentLineWidth += li.width + horizontalSpacing();
        }
        flushLine(currentLine, currentLineWidth - horizontalSpacing());
    }

    int totalH = y - rect.y() + bottom;
    if (y > effectiveRect.y()) {
        totalH -= verticalSpacing();
    }
    return totalH;
}

} // namespace QuarkMeta
