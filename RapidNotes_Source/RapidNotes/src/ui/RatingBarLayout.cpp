#include "RatingBarLayout.h"

namespace QuarkMeta {

RatingBarMetrics RatingBarLayout::calculate(const QRect& targetRect,
                                            RatingBarMode mode,
                                            int /*zoomLevel*/,
                                            int ratingH) {
    RatingBarMetrics m;
    // 🚀 星星固定 14px，其余参数 100% 保持不变
    m.starSize = 14;
    m.starSpacing = 0;
    m.banW = 14;
    m.banGap = 4;

    m.totalWidth = m.banW + m.banGap + (5 * m.starSize);
    int startX = targetRect.left() + (targetRect.width() - m.totalWidth) / 2;

    if (mode == RatingBarMode::GridCard) {
        const int gap = 4;
        int ratingY = targetRect.bottom() + gap;
        m.centerY = ratingY + ratingH / 2;
    } else { // TreeRow (列表模式)
        m.centerY = targetRect.top() + targetRect.height() / 2;
    }

    m.banRect = QRect(startX, m.centerY - m.banW / 2, m.banW, m.banW);
    m.starsStartX = startX + m.banW + m.banGap;

    return m;
}

} // namespace QuarkMeta