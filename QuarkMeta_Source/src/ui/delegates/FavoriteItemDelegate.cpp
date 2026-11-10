#include "FavoriteItemDelegate.h"
#include "../UiHelper.h"
#include "../ViewDragDropHelper.h"
#include "../../core/ModelContract.h"
#include <QPainterPath>
#include <QFontMetrics>
#include <QIcon>
#include <QPixmap>

namespace QuarkMeta {

FavoriteItemDelegate::FavoriteItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {
}

QSize FavoriteItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    sz.setHeight(28);
    return sz;
}

void FavoriteItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    
    // 背景高亮：优先级 拖放目标 > 选中 > 悬停 > 透明
    bool isDropTarget = index.data(IsDropTargetRole).toBool() ||
                        ViewDragDropHelper::isDropTarget(option.widget, index);

    if (isDropTarget) {
        QColor dropBg("#3498db");
        dropBg.setAlphaF(0.35f);
        painter->fillRect(opt.rect, dropBg);
    } else if (opt.state & QStyle::State_Selected) {
        QColor selBg("#378ADD");
        selBg.setAlphaF(0.15f);
        painter->fillRect(opt.rect, selBg);
    } else if (opt.state & QStyle::State_MouseOver) {
        painter->fillRect(opt.rect, QColor("#2A2D2E"));
    } else {
        painter->fillRect(opt.rect, Qt::transparent);
    }

    // 几何布局（根据树层级深度计算缩进，保证子节点图标与文本排版层次清晰）
    int depth = 0;
    QModelIndex parentIdx = index.parent();
    while (parentIdx.isValid()) {
        depth++;
        parentIdx = parentIdx.parent();
    }

    int leftMargin = 10 + depth * 14;
    int iconSize = 18;
    int spacing = 8;

    QRect iconRect(opt.rect.left() + leftMargin, opt.rect.top() + (opt.rect.height() - iconSize) / 2, iconSize, iconSize);
    QRect textRect(iconRect.right() + spacing, opt.rect.top(), opt.rect.width() - leftMargin - iconSize - spacing, opt.rect.height());

    // 微卡片圆角绘制（严格对标列表视图 TreeItemDelegate 标准：Qt::KeepAspectRatio + targetRect 真实四角对称圆角裁剪）
    QVariant decoData = index.data(Qt::DecorationRole);
    bool isFolder = index.data(Qt::UserRole + 4).toBool();
    bool hasCustomThumb = index.data(Qt::UserRole + 5).toBool();

    if (!isFolder && hasCustomThumb && decoData.canConvert<QIcon>()) {
        QIcon icon = decoData.value<QIcon>();
        QPixmap pix = icon.pixmap(QSize(64, 64));

        if (!pix.isNull()) {
            painter->save();
            // 1. 等比缩放计算目标几何矩形 targetRect
            QPixmap scaled = pix.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int x = iconRect.center().x() - scaled.width() / 2;
            int y = iconRect.center().y() - scaled.height() / 2;
            QRect targetRect(x, y, scaled.width(), scaled.height());

            // 2. 将圆角路径精准施加在 targetRect 上，彻底杜绝外溢冲刷与三角直角现象
            QPainterPath clipPath;
            clipPath.addRoundedRect(targetRect, 4, 4);
            painter->setClipPath(clipPath);

            painter->drawPixmap(targetRect, scaled);
            painter->restore();
        } else {
            icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }
    } else {
        QIcon icon = decoData.value<QIcon>();
        if (!icon.isNull()) {
            icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }
    }

    // 绘制文本
    QString text = index.data(Qt::DisplayRole).toString();
    QColor normalTextColor = (opt.state & QStyle::State_Selected) ? QColor("#FFFFFF") : QColor("#EEEEEE");
    QColor matchTextColor("#3498db");

    bool isAutoRec = index.data(Qt::UserRole + 9).toBool();
    bool isMarked = index.data(Qt::UserRole + 10).toBool();

    // 遵循 Memories 第 44 章规范：7px 滚动条 + 10px 呼吸间距 = 17px 基础右边距
    int rightReserved = 17;
    if (isAutoRec) rightReserved += 20;
    if (isMarked) rightReserved += 20;

    QRect adjustedTextRect = textRect.adjusted(0, 0, -(rightReserved - 8), 0);

    paintHighlightedText(painter, adjustedTextRect, text, m_filterKeyword, opt.font, normalTextColor, matchTextColor);

    // 绘制开启自动递归右侧 layers 图标与标记 Destination 图标
    int currentOffset = 17 + 16; // 图标宽 16px，最右侧保留 17px
    if (isAutoRec) {
        QRect layersRect(opt.rect.right() - currentOffset, opt.rect.top() + (opt.rect.height() - 16) / 2, 16, 16);
        QIcon layersIcon = UiHelper::getIcon("layers", QColor("#2ecc71"), 16);
        layersIcon.paint(painter, layersRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        currentOffset += 20;
    }

    if (isMarked) {
        QRect markIconRect(opt.rect.right() - currentOffset, opt.rect.top() + (opt.rect.height() - 16) / 2, 16, 16);
        QIcon markIcon = UiHelper::getIcon("Destination.svg", QColor("#ff551c"), 16);
        markIcon.paint(painter, markIconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
    }

    painter->restore();
}

void FavoriteItemDelegate::paintHighlightedText(QPainter* painter, const QRect& rect, const QString& text, const QString& keyword, const QFont& font, const QColor& normalColor, const QColor& matchColor) const {
    if (text.isEmpty()) return;

    QFontMetrics fm(font);
    painter->setFont(font);

    if (keyword.isEmpty()) {
        painter->setPen(normalColor);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, fm.elidedText(text, Qt::ElideRight, rect.width()));
        return;
    }

    int curX = rect.left();
    int endX = rect.right();
    int lastIdx = 0;
    int kwLen = keyword.length();

    while (lastIdx < text.length() && curX < endX) {
        int foundIdx = text.indexOf(keyword, lastIdx, Qt::CaseInsensitive);
        if (foundIdx == -1) {
            QString remStr = text.mid(lastIdx);
            QString elidedRem = fm.elidedText(remStr, Qt::ElideRight, endX - curX);
            painter->setPen(normalColor);
            painter->drawText(QRect(curX, rect.top(), endX - curX, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, elidedRem);
            break;
        }

        if (foundIdx > lastIdx) {
            QString normalSegment = text.mid(lastIdx, foundIdx - lastIdx);
            int segWidth = fm.horizontalAdvance(normalSegment);
            if (curX + segWidth > endX) {
                QString elidedSeg = fm.elidedText(normalSegment, Qt::ElideRight, endX - curX);
                painter->setPen(normalColor);
                painter->drawText(QRect(curX, rect.top(), endX - curX, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, elidedSeg);
                break;
            }
            painter->setPen(normalColor);
            painter->drawText(QRect(curX, rect.top(), segWidth, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, normalSegment);
            curX += segWidth;
        }

        QString matchSegment = text.mid(foundIdx, kwLen);
        int matchWidth = fm.horizontalAdvance(matchSegment);
        if (curX + matchWidth > endX) {
            QString elidedMatch = fm.elidedText(matchSegment, Qt::ElideRight, endX - curX);
            painter->setPen(matchColor);
            painter->drawText(QRect(curX, rect.top(), endX - curX, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, elidedMatch);
            break;
        }
        painter->setPen(matchColor);
        painter->drawText(QRect(curX, rect.top(), matchWidth, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, matchSegment);
        curX += matchWidth;

        lastIdx = foundIdx + kwLen;
    }
}

void FavoriteItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (!editor) return;

    int depth = 0;
    QModelIndex parentIdx = index.parent();
    while (parentIdx.isValid()) {
        depth++;
        parentIdx = parentIdx.parent();
    }

    int leftMargin = 10 + depth * 14;
    int iconSize = 18;
    int spacing = 8;

    int textLeft = option.rect.left() + leftMargin + iconSize + spacing;
    
    bool isAutoRec = index.data(Qt::UserRole + 9).toBool();
    bool isMarked = index.data(Qt::UserRole + 10).toBool();
    int rightMargin = 17;
    if (isAutoRec) rightMargin += 20;
    if (isMarked) rightMargin += 20;

    QRect editorRect = option.rect;
    editorRect.setLeft(textLeft - 2);
    editorRect.setRight(option.rect.right() - rightMargin);
    editorRect.setTop(option.rect.top() + 2);
    editorRect.setHeight(option.rect.height() - 4);

    editor->setGeometry(editorRect);
}

} // namespace QuarkMeta
