#include "RowLayoutEngine.h"
#include <algorithm>

namespace QuarkMeta {

int RowLayoutEngine::calculateRowHeight(int zoomLevel) {
    return std::clamp(zoomLevel, kMinRowHeight, kMaxRowHeight);
}

int RowLayoutEngine::calculateHeaderTextStartX(int zoomLevel) {
    int h = calculateRowHeight(zoomLevel);
    int side = std::max(16, h - (kVerticalPadding * 2));
    return kLeftMargin + side + kSpacing;
}

RowLayout RowLayoutEngine::calculate(const QRect& totalRect, int zoomLevel) {
    RowLayout l;
    l.rowHeight = totalRect.height() > 0 ? totalRect.height() : calculateRowHeight(zoomLevel);

    int side = std::max(16, l.rowHeight - (kVerticalPadding * 2));

    // 1. 左侧正方形微卡片物理矩形
    l.cardRect = QRect(totalRect.left() + kLeftMargin, 
                       totalRect.top() + kVerticalPadding, 
                       side, side);

    // 2. 表头与文本起始点 X 坐标 (与卡片严格隔离，绝不重叠)
    int textStartX = l.cardRect.right() + kSpacing;
    l.headerTextStartX = textStartX - totalRect.left();

    // 3. 右侧文本区域 (从卡片右侧向右铺满)
    l.textRect = totalRect;
    l.textRect.setLeft(textStartX);

    // 4. 行内重命名编辑框区域 (物理最大高度限幅 28px，垂直居中)
    l.editorRect = l.textRect;
    const int maxEditorH = 28;
    if (l.editorRect.height() > maxEditorH) {
        int diff = l.editorRect.height() - maxEditorH;
        int topAdj = diff / 2;
        int botAdj = diff - topAdj;
        l.editorRect.adjust(0, topAdj, 0, -botAdj);
    } else {
        l.editorRect.adjust(0, 2, 0, -2);
    }

    return l;
}

} // namespace QuarkMeta
