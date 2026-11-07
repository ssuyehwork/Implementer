#pragma once

#include <QRect>

namespace QuarkMeta {

enum class RatingBarMode {
    GridCard,   // 网格卡片模式 (紧凑尺寸，随 zoom 动态适配 14px/12px)
    TreeRow     // 列表行模式 (标准尺寸，固定 18px / -4 间距)
};

struct RatingBarMetrics {
    int starSize = 0;
    int starSpacing = 0;
    int banW = 0;
    int banGap = 0;
    int totalWidth = 0;

    QRect banRect;       // 禁选 ⊘ 按钮物理区域
    int starsStartX = 0; // 5 星起始 X 坐标
    int centerY = 0;     // 垂直中心 Y 坐标

    // 计算第 index 颗星 (0~4) 的绝对物理矩形
    QRect starRect(int index) const {
        return QRect(starsStartX + index * (starSize + starSpacing),
                     centerY - starSize / 2,
                     starSize, starSize);
    }
};

class RatingBarLayout {
public:
    /**
     * @brief 统一计算星级条几何排版 (内部自动处理 gap=4 与模式分流)
     * @param targetRect 网格模式下传入 cardRect (内部自动下移 +4px)；列表模式下传入单元格 rect
     * @param mode 视图模式
     * @param zoomLevel 网格模式下的缩放级别 (用于 <100 时缩小)
     * @param ratingH 星级条高度 (默认 24px)
     */
    static RatingBarMetrics calculate(const QRect& targetRect,
                                      RatingBarMode mode,
                                      int zoomLevel = 96,
                                      int ratingH = 24);
};

} // namespace QuarkMeta
