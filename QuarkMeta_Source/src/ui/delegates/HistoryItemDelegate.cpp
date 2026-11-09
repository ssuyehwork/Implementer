#include "HistoryItemDelegate.h"
#include <QPainterPath>
#include <QFontMetrics>
#include <QIcon>
#include <QPixmap>

namespace QuarkMeta {

HistoryItemDelegate::HistoryItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {
}

QSize HistoryItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    sz.setHeight(28); // 统一 28px 标准微卡片行高
    return sz;
}

void HistoryItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    // 1. 背景高亮
    if (opt.state & QStyle::State_Selected) {
        QColor selBg("#378ADD");
        selBg.setAlphaF(0.15f);
        painter->fillRect(opt.rect, selBg);
    } else if (opt.state & QStyle::State_MouseOver) {
        painter->fillRect(opt.rect, QColor("#2A2D2E"));
    } else {
        painter->fillRect(opt.rect, Qt::transparent);
    }

    // 2. 几何布局（严格对标列表视图与收藏夹规范）
    int leftMargin = 10;
    int iconSize = 18;
    int spacing = 8;

    QRect iconRect(opt.rect.left() + leftMargin, opt.rect.top() + (opt.rect.height() - iconSize) / 2, iconSize, iconSize);
    QRect textRect(iconRect.right() + spacing, opt.rect.top(), opt.rect.width() - leftMargin - iconSize - spacing, opt.rect.height());

    // 3. 微卡片圆角绘制（对标列表视图 TreeItemDelegate 标准：Qt::KeepAspectRatio + targetRect 真实四角对称圆角裁剪）
    QVariant decoData = index.data(Qt::DecorationRole);
    bool hasCustomThumb = index.data(Qt::UserRole + 5).toBool();

    if (hasCustomThumb && decoData.canConvert<QIcon>()) {
        QIcon icon = decoData.value<QIcon>();
        QPixmap pix = icon.pixmap(QSize(64, 64));

        if (!pix.isNull()) {
            painter->save();
            QPixmap scaled = pix.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int x = iconRect.center().x() - scaled.width() / 2;
            int y = iconRect.center().y() - scaled.height() / 2;
            QRect targetRect(x, y, scaled.width(), scaled.height());

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

    // 4. 文本绘制
    QString text = index.data(Qt::DisplayRole).toString();
    QColor normalTextColor = (opt.state & QStyle::State_Selected) ? QColor("#FFFFFF") : QColor("#EEEEEE");
    QColor matchTextColor("#3498db");

    paintHighlightedText(painter, textRect, text, m_filterKeyword, opt.font, normalTextColor, matchTextColor);

    painter->restore();
}

void HistoryItemDelegate::paintHighlightedText(QPainter* painter, const QRect& rect, const QString& text, const QString& keyword, const QFont& font, const QColor& normalColor, const QColor& matchColor) const {
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
            QString remaining = text.mid(lastIdx);
            QString elidedRemaining = fm.elidedText(remaining, Qt::ElideRight, endX - curX);
            painter->setPen(normalColor);
            painter->drawText(QRect(curX, rect.top(), endX - curX, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, elidedRemaining);
            break;
        }

        if (foundIdx > lastIdx) {
            QString beforeMatch = text.mid(lastIdx, foundIdx - lastIdx);
            int beforeWidth = fm.horizontalAdvance(beforeMatch);
            if (curX + beforeWidth > endX) {
                QString elidedBefore = fm.elidedText(beforeMatch, Qt::ElideRight, endX - curX);
                painter->setPen(normalColor);
                painter->drawText(QRect(curX, rect.top(), endX - curX, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, elidedBefore);
                break;
            }
            painter->setPen(normalColor);
            painter->drawText(QRect(curX, rect.top(), beforeWidth, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, beforeMatch);
            curX += beforeWidth;
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

} // namespace QuarkMeta
