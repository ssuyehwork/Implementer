#pragma once

#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QIcon>
#include <QString>
#include <QColor>
#include <QFontMetrics>

namespace QuarkMeta {

class CardPainterHelper {
public:
    // 0. 默认图标渲染与逻辑尺寸计算
    static QSize defaultIconLogicalSize(const QSize& areaSize);
    static QPixmap renderDefaultIcon(const QIcon& icon, const QSize& areaSize, qreal devicePixelRatio);

    // 1. 绘制主体卡片底色及缩略图 Cover
    static void drawCardCover(QPainter* painter, const QRect& cardRect, bool isSelected,
                             bool hasThumb, const QPixmap& thumb, const QIcon& defaultIcon,
                             bool isGridMode, bool isWaitingThumb);

    // 2. 绘制卡片圆角边框 (选中 3px 蓝色，未选中 1px #4a4a4a)
    static void drawCardBorder(QPainter* painter, const QRect& cardRect, bool isSelected);

    // 3. 绘制置顶状态标记
    static void drawStatusIndicators(QPainter* painter, const QRect& cardRect, bool isPinned);

    // 4. 绘制自适应扩展名徽章
    static void drawExtensionBadge(QPainter* painter, const QRect& cardRect,
                                   const QString& ext, bool hasThumb);

    // 5. 绘制评级星级与彩色胶囊底色
    static void drawRatingStars(QPainter* painter, const QRect& banRect,
                                const QRect& cardRect, int starSize, int starSpacing, int ratingY, int ratingH, int starsStartX,
                                int rating, const QString& colorStr, bool isSelected);

    // 🚀【SSOT 评分点击资格统一校验】：仅当项目已选中且未按住 Ctrl/Shift 键时方可点击修改评分
    static bool canModifyRatingOnPress(bool isCurrentlySelected, Qt::KeyboardModifiers modifiers);

    // 6. 绘制空文件夹特异虚线边框
    static void drawEmptyFolderBorder(QPainter* painter, const QRect& cardRect);

    // 7. 绘制分类侧边栏节点的高亮/彩色背景底色
    static void drawCategoryBackground(QPainter* painter, const QRect& contentRect, bool isSelected, bool isHover, const QString& colorHex);

    static constexpr int kPinIconSize = 16;
    static constexpr int kPinToRatingGap = 2;   // 置顶图标与右侧星级/元素之间的间距 2px
    static constexpr int kPinRightPadding = 6;  // 列表视图中置顶图标距单元格右边缘内边距 6px

    // 8. 在指定右边缘 X 坐标处绘制内联置顶图标（返回图标占用物理宽度，未置顶返回 0）
    static int drawInlinePinIcon(QPainter* painter, int rightX, int topY, int height, bool isPinned);
};

} // namespace QuarkMeta
