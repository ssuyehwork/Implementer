#pragma once
#include <QRect>

namespace QuarkMeta {

struct RowLayout {
    int rowHeight;          // 统一行高 (30px ~ 230px)
    QRect cardRect;         // 左侧正方形微卡片物理区域 (side x side)
    QRect textRect;         // 右侧文件名文本物理区域
    QRect editorRect;       // 行内重命名编辑框物理区域
    int headerTextStartX;   // 表头第 0 列名称文字绝对对齐的物理起点 X
};

class RowLayoutEngine {
public:
    static constexpr int kLeftMargin = 6;      // 左外边距 6px
    static constexpr int kVerticalPadding = 3; // 上下边距 3px
    static constexpr int kSpacing = 8;         // 卡片与文字间距 8px
    static constexpr int kMinRowHeight = 30;   // 最小行高 30px
    static constexpr int kMaxRowHeight = 230;  // 最大行高 230px

    /**
     * @brief 纯函数：根据单元格矩形和当前缩放级别计算唯一的标准几何布局
     */
    static RowLayout calculate(const QRect& totalRect, int zoomLevel = 30);
    
    /**
     * @brief 计算当前缩放级别对应的标准行高
     */
    static int calculateRowHeight(int zoomLevel);

    /**
     * @brief 计算表头第 0 列名称文字的对齐起点 X
     */
    static int calculateHeaderTextStartX(int zoomLevel);
};

} // namespace QuarkMeta
