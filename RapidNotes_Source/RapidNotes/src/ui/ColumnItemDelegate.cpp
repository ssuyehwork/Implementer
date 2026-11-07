#include "ColumnItemDelegate.h"
#include "ColumnViewPane.h"
#include "../core/ModelContract.h"
#include "UiHelper.h"
#include "StyleLibrary.h"
#include "CardPainterHelper.h"
#include "ViewDragDropHelper.h"

#include <QPainter>
#include <QPainterPath>

namespace QuarkMeta {

ColumnItemDelegate::ColumnItemDelegate(QObject* parent)
    : RenameCapableDelegate(parent) {
}

QSize ColumnItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSize sz = RenameCapableDelegate::sizeHint(option, index);
    sz.setHeight(28);
    return sz;
}

void ColumnItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (!index.isValid()) return;

    if (index.data(SectionHeaderRole).toBool()) {
        painter->save();
        QRect rect = option.rect;
        painter->fillRect(rect, QColor("#1E1E1E"));

        QFont font = painter->font();
        font.setBold(true);
        font.setPixelSize(12);
        painter->setFont(font);
        painter->setPen(QColor("#3498db"));

        QString text = index.data(SectionHeaderTextRole).toString();

        QFontMetrics fm(font);
        int leftX = rect.left() + 10;
        int textW = fm.horizontalAdvance(text);

        QRect textRect(leftX, rect.top(), textW + 10, rect.height());
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);

        if (index.data(SectionKindRole).toInt() == 1) {
            bool collapsed = index.data(SectionCollapsedRole).toBool();
            int arrowX = leftX + textW + 6;
            int iconSize = 12;
            QRect iconRect(arrowX, rect.top() + (rect.height() - iconSize) / 2, iconSize, iconSize);
            QIcon arrowIcon = UiHelper::getIcon(collapsed ? "chevron_right" : "chevron_down", QColor("#3498db"), iconSize);
            arrowIcon.paint(painter, iconRect);
        }

        painter->restore();
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    bool selected = (option.state & QStyle::State_Selected);
    bool hover = (option.state & QStyle::State_MouseOver);
    bool isParentExpanded = false;
    ColumnViewPane* pane = qobject_cast<ColumnViewPane*>(const_cast<QWidget*>(option.widget ? option.widget->parentWidget() : nullptr));
    if (pane && !pane->expandedChildPath().isEmpty()) {
        QString itemPath = QDir::toNativeSeparators(QDir::cleanPath(index.data(PathRole).toString()));
        QString childPath = QDir::toNativeSeparators(QDir::cleanPath(pane->expandedChildPath()));
        isParentExpanded = (QString::compare(itemPath, childPath, Qt::CaseInsensitive) == 0);
    }

    bool isDropTarget = index.data(IsDropTargetRole).toBool() ||
                       ViewDragDropHelper::isDropTarget(qobject_cast<const QAbstractItemView*>(option.widget), index);

    // 1. 背景绘制
    QColor bg;
    if (isDropTarget) {
        bg = QColor("#3498db");
        bg.setAlphaF(0.35f);
    } else if (selected) {
        bg = QColor("#378ADD");
        bg.setAlphaF(0.18f);
    } else if (isParentExpanded) {
        bg = QColor("#334455");
    } else if (hover) {
        bg = QColor("#2A2D2E");
    } else {
        bg = QColor("#1E1E1E");
    }

    painter->setBrush(bg);
    painter->setPen(Qt::NoPen);
    painter->drawRect(option.rect);

    QRect rect = option.rect.adjusted(8, 0, -8, 0);

    // 2. 左侧图标 (文件 / 文件夹)
    bool isFolder = (index.data(TypeRole).toString() == "folder");
    bool isEmpty = index.data(IsEmptyRole).toBool();
    bool hasThumbnail = index.data(HasThumbnailRole).toBool();
    QVariant deco = index.data(Qt::DecorationRole);

    int iconSize = 18;
    QRect iconRect(rect.left(), rect.top() + (rect.height() - iconSize) / 2, iconSize, iconSize);

    if (hasThumbnail) {
        QPixmap pix;
        if (deco.canConvert<QIcon>() && !deco.value<QIcon>().isNull()) {
            pix = deco.value<QIcon>().pixmap(iconRect.size());
        } else if (deco.canConvert<QPixmap>() && !deco.value<QPixmap>().isNull()) {
            pix = deco.value<QPixmap>();
        }
        if (!pix.isNull()) {
            painter->save();
            QPainterPath clipPath;
            clipPath.addRoundedRect(iconRect, 3, 3);
            painter->setClipPath(clipPath);

            QSize scaledSize = pix.size().scaled(iconRect.size(), Qt::KeepAspectRatio);
            QRect targetRect(
                iconRect.left() + (iconRect.width() - scaledSize.width()) / 2,
                iconRect.top() + (iconRect.height() - scaledSize.height()) / 2,
                scaledSize.width(),
                scaledSize.height()
            );
            painter->drawPixmap(targetRect, pix);
            painter->restore();
        } else {
            QIcon fallbackIcon = UiHelper::getIcon("file", QColor("#888888"), 18);
            fallbackIcon.paint(painter, iconRect, Qt::AlignCenter);
        }
    } else {
        if (deco.canConvert<QIcon>() && !deco.value<QIcon>().isNull()) {
            deco.value<QIcon>().paint(painter, iconRect, Qt::AlignCenter);
        } else if (deco.canConvert<QPixmap>() && !deco.value<QPixmap>().isNull()) {
            QPixmap pix = deco.value<QPixmap>();
            painter->drawPixmap(iconRect, pix.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            QIcon fallbackIcon = UiHelper::getIcon(isFolder ? "folder" : "file", QColor("#888888"), 18);
            fallbackIcon.paint(painter, iconRect, Qt::AlignCenter);
        }
    }

    // 空文件夹虚线提示
    if (isFolder && isEmpty) {
        painter->save();
        painter->setPen(QPen(QColor("#41F2F2"), 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(iconRect.adjusted(-1, -1, 1, 1), 3, 3);
        painter->restore();
    }

    // 3. 读取元数据 (色标与星级)
    bool isDir = isFolder || index.data(Qt::UserRole + 2).toBool();
    int rating = index.data(RatingRole).toInt();
    QString colorName = index.data(ColorRole).toString();

    // 绘制 5px 宽度的左侧垂直色条
    if (!colorName.isEmpty()) {
        QString hexColor = Style::getColorHexByName(colorName);
        if (hexColor.startsWith("#")) {
            painter->setBrush(QColor(hexColor));
            painter->setPen(Qt::NoPen);
            QRect colorBarRect(option.rect.left(), option.rect.top(), 5, option.rect.height());
            painter->drawRect(colorBarRect);
        }
    }

    // 4. 从右向左依次计算右侧组件布局：文件夹箭头 -> 星级 -> 置顶图标
    int rightX = option.rect.right() - (isDir ? 22 : 6);

    // 计算星级组件位置 (若 rating > 0)
    int starSize = 12;
    int numWidth = 12;
    int starRightX = rightX;
    QRect starIconRect;
    QRect numRect;

    if (rating > 0) {
        starIconRect = QRect(starRightX - numWidth - starSize, option.rect.top() + (option.rect.height() - starSize) / 2, starSize, starSize);
        numRect = QRect(starRightX - numWidth, option.rect.top() + (option.rect.height() - starSize) / 2, numWidth, starSize);
        rightX = starIconRect.left();
    }

    // 置顶图标位置 (仅当置顶时，在星级/箭头左侧间隔 2px 处绘制)
    bool isPinned = index.data(PinnedRole).toBool();
    int pinRightX = rightX;
    if (isPinned) {
        pinRightX -= CardPainterHelper::kPinToRatingGap;
    }
    int pinW = CardPainterHelper::drawInlinePinIcon(painter, pinRightX, option.rect.top(), option.rect.height(), isPinned);

    if (pinW > 0) {
        rightX = pinRightX - pinW;
    }

    // 确定文本可用有效区域
    rightX -= CardPainterHelper::kPinToRatingGap;
    int availableWidth = rightX - (option.rect.left() + 32);
    if (availableWidth < 10) availableWidth = 10;

    QRect actualTextRect(option.rect.left() + 32, option.rect.top(), availableWidth, option.rect.height());

    QString name = index.data(Qt::DisplayRole).toString();
    QColor textColor = selected ? QColor("#FFFFFF") : QColor("#EEEEEE");
    painter->setPen(textColor);
    painter->setFont(option.font);
    QString elidedText = option.fontMetrics.elidedText(name, Qt::ElideRight, actualTextRect.width());
    painter->drawText(actualTextRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);

    // 5. 绘制星级标示 (若 rating > 0)
    if (rating > 0) {
        QColor orangeColor = QColor("#FF551C");
        QIcon starIcon = UiHelper::getIcon("star_filled", orangeColor, starSize);
        starIcon.paint(painter, starIconRect, Qt::AlignCenter);

        painter->setPen(orangeColor);
        painter->setFont(QFont("Segoe UI", 9, QFont::Bold));
        painter->drawText(numRect, Qt::AlignRight | Qt::AlignVCenter, QString::number(rating));
    }

    // 6. 如果是文件夹，最右侧绘制向右箭头 chevron_right
    if (isDir) {
        QRect arrowRect(option.rect.right() - 20, option.rect.top() + (option.rect.height() - 14) / 2, 14, 14);
        QColor arrowColor = selected ? QColor("#FFFFFF") : (isEmpty ? QColor("#41F2F2") : QColor("#888888"));
        UiHelper::getIcon("chevron_right", arrowColor, 14).paint(painter, arrowRect, Qt::AlignCenter);
    }

    painter->restore();
}

} // namespace QuarkMeta
