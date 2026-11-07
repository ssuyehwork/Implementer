#pragma once

#include <QRect>
#include <QPoint>

namespace QuarkMeta {

/**
 * @brief 归一化卡片几何结构体
 */
struct CardLayout {
    QRect totalRect;       // 单元格总矩形
    QRect coverRect;       // 缩略图主体区域
    QRect badgeRect;       // 扩展名徽章 (DIR/FILE)
    QRect pinRect;         // 置顶图标
    QRect capsuleRect;     // 底部色标星级胶囊条
    QRect banRect;         // ⊘ 禁选按钮
    QRect starRects[5];    // 5 颗星各自精确矩形
    QRect textRect;        // 文件名区域 (最多2行)

    // 🚀【精确设为 14px】
    int starSize = 14;
    int starSpacing = 0;

    int hitStar(const QPoint& pt) const {
        if (banRect.contains(pt)) return 0;
        for (int i = 0; i < 5; ++i) {
            if (starRects[i].contains(pt)) return i + 1;
        }
        return -1;
    }

    bool isTextHit(const QPoint& pt) const { return textRect.contains(pt); }
    bool isCoverHit(const QPoint& pt) const { return coverRect.contains(pt); }
};

class CardLayoutEngine {
public:
    static constexpr int kCardPadding = 3;
    static constexpr int kTextHeight = 36;
    static constexpr int kRatingHeight = 24;
    static constexpr int kGap = 4;

    static constexpr int totalPaddingHorizontal() { return kCardPadding * 2; }
    static constexpr int extraHeight() { return kCardPadding * 2 + kTextHeight + kRatingHeight + kGap; }

    static CardLayout calculate(const QRect& totalRect, int zoomLevel = 96);
};

} // namespace QuarkMeta