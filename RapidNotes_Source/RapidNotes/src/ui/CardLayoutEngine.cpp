#include "CardLayoutEngine.h"

namespace QuarkMeta {

CardLayout CardLayoutEngine::calculate(const QRect& totalRect, int /*zoomLevel*/) {
    CardLayout l;
    l.totalRect = totalRect;

    const int padding = kCardPadding;
    const int textHeight = kTextHeight;
    const int ratingH = kRatingHeight;
    const int gap = kGap;

    // 1. Cover 区域
    int coverBottom = totalRect.bottom() - (textHeight + ratingH + gap + padding);
    l.coverRect = QRect(totalRect.left() + padding,
                        totalRect.top() + padding,
                        totalRect.width() - padding * 2,
                        coverBottom - (totalRect.top() + padding));

    // 2. 徽章与置顶
    l.badgeRect = QRect(l.coverRect.left() + 4, l.coverRect.top() + 4, 30, 16);
    l.pinRect = QRect(l.coverRect.right() - 20, l.coverRect.top() + 4, 16, 16);

    // 3. Footer 胶囊条
    int ratingY = l.coverRect.bottom() + gap;
    l.capsuleRect = QRect(totalRect.left() + padding, ratingY, totalRect.width() - padding * 2, ratingH);

    // 4. 🚀 星星固定 14px，其余保持原样
    l.starSize = 14;
    l.starSpacing = 0;
    const int banW = 14;
    const int banGap = 4;

    int totalBarContentW = banW + banGap + (5 * l.starSize);
    int startX = l.capsuleRect.left() + (l.capsuleRect.width() - totalBarContentW) / 2;
    int centerY = l.capsuleRect.top() + l.capsuleRect.height() / 2;

    l.banRect = QRect(startX, centerY - banW / 2, banW, banW);

    int starsStartX = startX + banW + banGap;
    for (int i = 0; i < 5; ++i) {
        l.starRects[i] = QRect(starsStartX + i * l.starSize,
                               centerY - l.starSize / 2,
                               l.starSize, l.starSize);
    }

    // 5. 文本区域
    l.textRect = QRect(totalRect.left() + padding,
                       l.capsuleRect.bottom() + 2,
                       totalRect.width() - padding * 2,
                       textHeight);

    return l;
}

} // namespace QuarkMeta