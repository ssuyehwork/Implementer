#include "CardPainterHelper.h"
#include "UiHelper.h"
#include <QPainterPath>
#include <QFont>
#include <QtMath>
#include <QCache>
#include <algorithm>

namespace QuarkMeta {

QSize CardPainterHelper::defaultIconLogicalSize(const QSize& areaSize) {
    if (areaSize.width() < 70 && areaSize.height() < 70) {
        int w = qRound(areaSize.width() * 0.65);
        int h = qRound(areaSize.height() * 0.65);
        return QSize(w, h);
    }
    return QSize(35, 45);
}

// 禁止修改 renderDefaultIcon 函数
QPixmap CardPainterHelper::renderDefaultIcon(const QIcon& icon, const QSize& areaSize, qreal devicePixelRatio) {
    if (icon.isNull()) return QPixmap();

    QSize target = defaultIconLogicalSize(areaSize);
    QSize physicalTarget = target * devicePixelRatio;

    // 🔥【绝对精准的防碰撞 Key】：以真实图标 cacheKey + 宽高 + 设备像素比作为唯一字符串 Key
    // 物理上杜绝任何哈希碰撞，文件夹是文件夹，cpp 是 cpp，绝不会串号！
    QString cacheKey = QStringLiteral("%1_%2x%3_%4")
                       .arg(icon.cacheKey())
                       .arg(physicalTarget.width())
                       .arg(physicalTarget.height())
                       .arg(devicePixelRatio);

    static QCache<QString, QPixmap> s_renderedIconCache(200);
    QPixmap* cachedPx = s_renderedIconCache.object(cacheKey);
    if (cachedPx) {
        return *cachedPx; // 命中自身专属缓存，直接返回
    }

    QPixmap px = icon.pixmap(physicalTarget);
    if (px.isNull()) {
        px = icon.pixmap(35, 45);
    }

    if (!px.isNull() && (px.width() > physicalTarget.width() || px.height() > physicalTarget.height())) {
        px = px.scaled(physicalTarget, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    if (!px.isNull()) {
        px.setDevicePixelRatio(devicePixelRatio);
        s_renderedIconCache.insert(cacheKey, new QPixmap(px)); // 写入专属缓存
    }

    return px;
}

void CardPainterHelper::drawCardCover(QPainter* painter, const QRect& cardRect, bool isSelected, 
                                     bool hasThumb, const QPixmap& thumb, const QIcon& defaultIcon, 
                                     bool isGridMode, bool isWaitingThumb) {
    Q_UNUSED(isSelected);
    Q_UNUSED(isGridMode);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    // ① 设置卡片圆角裁切
    QPainterPath clipPath;
    clipPath.addRoundedRect(cardRect, 6, 6);
    painter->setClipPath(clipPath);

    // ② 严格保持透明：卡片内部 100% 完全透明
    painter->setPen(Qt::NoPen);
    if (isWaitingThumb) {
        painter->setBrush(QColor("#2A2A2A")); // 仅在等待缩略图时显示轻量占位灰底
        painter->drawRect(cardRect);
    } else {
        painter->setBrush(Qt::transparent);
        painter->drawRect(cardRect);
    }

    if (hasThumb && !thumb.isNull()) {
        QSize imgSize = thumb.size();
        QSize targetSize = imgSize.scaled(cardRect.size(), Qt::KeepAspectRatio);
        QRect targetRect(cardRect.center().x() - targetSize.width() / 2,
                         cardRect.center().y() - targetSize.height() / 2,
                         targetSize.width(), targetSize.height());
        painter->drawPixmap(targetRect, thumb);
    } else if (!defaultIcon.isNull()) {
        qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : 1.0;
        QPixmap iconPixmap = renderDefaultIcon(defaultIcon, cardRect.size(), dpr);
        if (!iconPixmap.isNull()) {
            QSizeF logicalSize = iconPixmap.deviceIndependentSize();
            int w = qRound(logicalSize.width());
            int h = qRound(logicalSize.height());
            QRect iconRect(cardRect.center().x() - w / 2,
                           cardRect.center().y() - h / 2,
                           w, h);

            painter->drawPixmap(iconRect, iconPixmap);
        }
    }
    painter->restore();
}

void CardPainterHelper::drawCardBorder(QPainter* painter, const QRect& cardRect, bool isSelected) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    if (isSelected) {
        painter->setPen(QPen(QColor("#3498db"), 2));
    } else {
        painter->setPen(QPen(QColor("#4a4a4a"), 2));
    }

    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(cardRect, 6, 6);
    painter->restore();
}

void CardPainterHelper::drawStatusIndicators(QPainter* painter, const QRect& cardRect, bool isPinned) {
    if (isPinned) {
        QRect statusRect(cardRect.right() - 22, cardRect.top() + 8, 16, 16);
        UiHelper::getIcon("pin_vertical", QColor("#FF551C"), 16).paint(painter, statusRect);
    }
}

void CardPainterHelper::drawExtensionBadge(QPainter* painter, const QRect& cardRect, 
                                           const QString& ext, bool hasThumb) {
    auto colors = ColorPaletteEngine::getExtensionBadgeColors(ext);
    QColor badgeColor = colors.first;
    QColor textColor = colors.second;

    if (!hasThumb) {
        badgeColor.setAlpha(160);
        textColor.setAlpha(200);
    }

    QRect extRect(cardRect.left() + 8, cardRect.top() + 8, 36, 18);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(badgeColor);
    painter->drawRoundedRect(extRect, 2, 2);
    painter->setPen(textColor);
    QFont extFont = painter->font(); extFont.setPointSize(8); extFont.setBold(true);
    painter->setFont(extFont);
    painter->drawText(extRect, Qt::AlignCenter, ext);
    painter->restore();
}

bool CardPainterHelper::canModifyRatingOnPress(bool isCurrentlySelected, Qt::KeyboardModifiers modifiers) {
    if (!isCurrentlySelected) return false;
    if (modifiers & (Qt::ControlModifier | Qt::ShiftModifier)) return false;
    return true;
}

void CardPainterHelper::drawRatingStars(QPainter* painter, const QRect& banRect, 
                                        const QRect& cardRect, int starSize, int starSpacing, int ratingY, int ratingH, int starsStartX,
                                        int rating, const QString& colorStr, bool isSelected) {
    Q_UNUSED(cardRect);

    // 🚀【彻底根治】：使用真实传入的 starSpacing，彻底废除 -4px 负间距硬编码！
    int actualSpacing = starSpacing;

    if (!colorStr.isEmpty()) {
        QColor bgColor = UiHelper::parseColorName(colorStr);
        if (bgColor.isValid()) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setBrush(bgColor);
            painter->setPen(Qt::NoPen);
            
            QRect lastStarRect(starsStartX + 4 * (starSize + actualSpacing), 
                               ratingY + (ratingH - starSize) / 2, 
                               starSize, starSize);
            QRect totalRect = banRect.united(lastStarRect);
            painter->drawRoundedRect(totalRect.adjusted(-4, -1, 4, 1), 4, 4);
            painter->restore();
        }
    }

    bool drawStars = (rating > 0) || isSelected;
    if (drawStars) {
        QColor bgColor = colorStr.isEmpty() ? QColor(0,0,0,0) : UiHelper::parseColorName(colorStr);
        
        double luminance = 0.0;
        if (bgColor.isValid() && bgColor.alpha() > 0) {
            luminance = (0.299 * bgColor.red() + 0.587 * bgColor.green() + 0.114 * bgColor.blue()) / 255.0;
        }

        QColor starColor, emptyStarColor;
        if (colorStr.isEmpty()) {
            starColor      = QColor("#CCCCCC");
            emptyStarColor = QColor("#888888");
        } else if (luminance < 0.5) {
            starColor      = QColor("#FFFFFF");
            emptyStarColor = QColor(255, 255, 255, 160);
        } else {
            starColor      = QColor("#1A1A1A");
            emptyStarColor = QColor(0, 0, 0, 140);
        }

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        UiHelper::getIcon("no_color", starColor, banRect.width()).paint(painter, banRect);
        
        QPixmap filledStar = UiHelper::getIcon("star_filled", starColor, starSize).pixmap(starSize, starSize);
        QPixmap emptyStar  = UiHelper::getIcon("star", emptyStarColor, starSize).pixmap(starSize, starSize);
        
        for (int i = 0; i < 5; ++i) {
            QRect starRect(starsStartX + i * (starSize + actualSpacing), 
                           ratingY + (ratingH - starSize) / 2, 
                           starSize, starSize);
            painter->drawPixmap(starRect, (i < rating) ? filledStar : emptyStar);
        }
        painter->restore();
    }
}

void CardPainterHelper::drawEmptyFolderBorder(QPainter* painter, const QRect& cardRect) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor("#41F2F2"), 1, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(cardRect, 6, 6);
    painter->restore();
}

void CardPainterHelper::drawCategoryBackground(QPainter* painter, const QRect& contentRect, bool isSelected, bool isHover, const QString& colorHex) {
    if (!isSelected && !isHover) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QColor baseColor = colorHex.isEmpty() ? QColor("#3498db") : QColor(colorHex);
    QColor bg = isSelected ? baseColor : QColor("#2a2d2e");
    if (isSelected) {
        bg.setAlphaF(0.2f); 
    }

    painter->setBrush(bg);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(contentRect, 4, 4);
    painter->restore();
}

int CardPainterHelper::drawInlinePinIcon(QPainter* painter, int rightX, int topY, int height, bool isPinned) {
    if (!isPinned) return 0;

    int iconX = rightX - kPinIconSize;
    int iconY = topY + (height - kPinIconSize) / 2;
    QRect iconRect(iconX, iconY, kPinIconSize, kPinIconSize);

    UiHelper::getIcon("pin_vertical", QColor("#FF551C"), kPinIconSize).paint(painter, iconRect);

    return kPinIconSize;
}

} // namespace QuarkMeta