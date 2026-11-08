#pragma once

#include <QRect>
#include <QFont>
#include <QFontMetrics>
#include <QModelIndex>
#include <QString>
#include "LibraryNodeKind.h"

namespace QuarkMeta {

/**
 * @brief 库树视图布局常量与几何计算唯一真理源 (SSOT)
 * 所有关于行高、缩进、留白、边距的数值只在此处定义一次，供绘制、命中检测与行内编辑器定位共用。
 */
class LibraryTreeLayout {
public:
    // 布局常量
    static constexpr int kRowHeight = 28;                 // 普通节点行高
    static constexpr int kSectionRowHeight = 22;          // 分组标题行高
    static constexpr int kSectionLeftOffset = 6;          // 分组标题左偏移
    static constexpr int kSectionPaddingLeft = 6;         // 分组标题内左留白
    static constexpr int kSectionPaddingRight = 6;        // 分组标题内右留白
    static constexpr int kArrowSize = 14;                 // 折叠指示箭头宽/高
    static constexpr int kArrowSpacing = 4;               // 箭头与文本/图标间距
    static constexpr int kAddBtnSize = 20;                // "+"按钮尺寸
    static constexpr int kAddBtnRightPadding = 10;        // "+"按钮右侧留白
    static constexpr int kAddBtnIconSize = 16;            // "+"按钮图标绘制尺寸
    static constexpr int kBaseLeftMargin = 10;            // 普通节点基础左边距
    static constexpr int kIndentPerLevel = 14;            // 普通节点每层缩进
    static constexpr int kIconSize = 18;                  // 普通节点图标尺寸
    static constexpr int kIconTextSpacing = 8;            // 图标与文字间距
    static constexpr int kRightMargin = 17;               // 右边距 (7px 滚动条 + 10px 留白)
    static constexpr int kCountTextExtraWidth = 12;       // 计数文字宽度附加留白

    /**
     * @brief 计算节点的自定义树层级深度 (customDepth)
     * 仅向上统计属于 Category 的父节点层数
     */
    static int customDepth(const QModelIndex& index) {
        int depth = 0;
        QModelIndex parentIdx = index.parent();
        while (parentIdx.isValid()) {
            int kindInt = parentIdx.data(NodeKindRole).toInt();
            if (kindInt == static_cast<int>(LibraryNodeKind::Category)) {
                depth++;
            }
            parentIdx = parentIdx.parent();
        }
        return depth;
    }

    /**
     * @brief 格式化节点计数文字
     * -999 显示"…"，大于等于 0 显示数字，其余为空
     */
    static QString countText(int count) {
        if (count == -999) {
            return QStringLiteral("…");
        } else if (count >= 0) {
            return QString::number(count);
        }
        return QString();
    }

    /**
     * @brief 计算三大分组标头 ③ 区域（文字+箭头折叠块）的矩形
     */
    static QRect sectionHeaderRect(const QRect& optionRect, const QString& text, const QFont& font) {
        QFontMetrics fm(font);
        int textWidth = fm.horizontalAdvance(text);
        int totalWidth = kSectionPaddingLeft + textWidth + kArrowSpacing + kArrowSize + kSectionPaddingRight;
        int top = optionRect.top() + (optionRect.height() - kSectionRowHeight) / 2;
        int left = optionRect.left() + kSectionLeftOffset;
        return QRect(left, top, totalWidth, kSectionRowHeight);
    }

    /**
     * @brief 计算三大分组标头 ① 区域（右侧"+"加号按钮）的矩形
     */
    static QRect addBtnRect(const QRect& optionRect) {
        int top = optionRect.top() + (optionRect.height() - kAddBtnSize) / 2;
        int left = optionRect.right() - kAddBtnRightPadding - kAddBtnSize;
        return QRect(left, top, kAddBtnSize, kAddBtnSize);
    }

    /**
     * @brief 计算普通分类节点的左侧折叠展开箭头矩形
     */
    static QRect arrowRect(const QRect& optionRect, int depth) {
        int leftMargin = kBaseLeftMargin + depth * kIndentPerLevel;
        return QRect(optionRect.left() + leftMargin, optionRect.top() + (optionRect.height() - kArrowSize) / 2, kArrowSize, kArrowSize);
    }

    /**
     * @brief 计算普通节点的图标矩形
     */
    static QRect iconRect(const QRect& optionRect, int depth, bool isCategory) {
        int leftMargin = kBaseLeftMargin + depth * kIndentPerLevel;
        int iconLeft = optionRect.left() + leftMargin + (isCategory ? (kArrowSize + kArrowSpacing) : 0);
        return QRect(iconLeft, optionRect.top() + (optionRect.height() - kIconSize) / 2, kIconSize, kIconSize);
    }

    /**
     * @brief 计算普通节点的计数矩形
     */
    static QRect countRect(const QRect& optionRect, int countWidth) {
        if (countWidth <= 0) return QRect();
        return QRect(optionRect.right() - countWidth - kRightMargin, optionRect.top(), countWidth, optionRect.height());
    }

    /**
     * @brief 计算普通节点的文字显示矩形
     */
    static QRect textRect(const QRect& optionRect, const QRect& iconR, int countWidth) {
        int textLeft = iconR.right() + kIconTextSpacing;
        int textWidth = optionRect.right() - textLeft - countWidth - kRightMargin;
        if (textWidth < 0) textWidth = 0;
        return QRect(textLeft, optionRect.top(), textWidth, optionRect.height());
    }

    /**
     * @brief 计算普通节点的行内编辑器矩形
     */
    static QRect editorRect(const QRect& optionRect, int depth, bool isCategory, int countWidth) {
        int leftMargin = kBaseLeftMargin + depth * kIndentPerLevel;
        int iconLeft = optionRect.left() + leftMargin + (isCategory ? (kArrowSize + kArrowSpacing) : 0);
        int textLeft = iconLeft + kIconSize + kIconTextSpacing;

        QRect rect = optionRect;
        rect.setLeft(textLeft - 2);
        rect.setRight(optionRect.right() - kRightMargin - countWidth);
        rect.setTop(optionRect.top() + 2);
        rect.setHeight(optionRect.height() - 4);
        return rect;
    }
};

} // namespace QuarkMeta
