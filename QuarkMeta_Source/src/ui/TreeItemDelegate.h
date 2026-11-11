#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QMouseEvent>
#include <QLineEdit>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <algorithm>
#include "ContentPanel.h"
#include "RenameCapableDelegate.h"
#include "ViewDragDropHelper.h"
#include "../core/ClipboardService.h"
#include "RatingBarLayout.h"
#include "RowLayoutEngine.h"
#include "../meta/MetadataManager.h"
#include "../core/ModelContract.h"
#include "UiHelper.h"
#include "CardPainterHelper.h"
#include "StyleLibrary.h"
using namespace QuarkMeta::Style;

namespace QuarkMeta {

/**
 * @brief 通用树形视图代理，提供圆角高亮效果
 */
class TreeItemDelegate : public RenameCapableDelegate {
public:
    explicit TreeItemDelegate(QObject* parent = nullptr, bool showStatus = true, bool drawMiniCards = false)
        : RenameCapableDelegate(parent), m_drawMiniCards(drawMiniCards) { Q_UNUSED(showStatus); }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (index.data(SectionHeaderRole).toBool()) {
            return QSize(option.rect.width(), 28);
        }
        const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(option.widget);
        int zoom = view ? view->iconSize().height() + 8 : 30;
        int h = RowLayoutEngine::calculateRowHeight(zoom);
        return QSize(option.rect.width(), h);
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
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

        bool selected = option.state & QStyle::State_Selected;
        bool hover = option.state & QStyle::State_MouseOver;

        // 🚀【行底色彻底统一与防穿透自绘】：直接根据选中/悬停/行号奇偶绘制底色，贯穿整个单元格矩形
        painter->save();
        QColor bg;
        bool useAlternate = false;
        if (const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(option.widget)) {
            useAlternate = view->alternatingRowColors();
        }

        bool isDropTarget = ViewDragDropHelper::isDropTarget(
            qobject_cast<const QAbstractItemView*>(option.widget), index);

        if (isDropTarget) {
            bg = QColor("#3498db");
            bg.setAlphaF(0.35f);
        } else if (selected) {
            bg = QColor("#378ADD");
            bg.setAlphaF(0.15f);
        } else if (hover) {
            bg = QColor("#2A2D2E");
        } else {
            // 根据控件是否开启斑马纹与分区行号奇偶精准赋值底色
            int secRow = index.data(SectionRowRole).toInt();
            if (secRow < 0) secRow = index.row();
            bg = (useAlternate && secRow % 2 == 1) ? QColor("#252526") : QColor("#1E1E1E");
        }
        painter->setBrush(bg);
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
        painter->restore();

        QString itemPath = index.data(PathRole).toString();
        bool isCut = !itemPath.isEmpty() && ClipboardService::instance().isItemCut(itemPath);
        painter->save();
        if (isCut) {
            painter->setOpacity(0.5);
        }

        QStyleOptionViewItem opt = option;
        if (index.column() >= 1) {
            opt.displayAlignment = Qt::AlignCenter;
        }

        opt.state &= ~QStyle::State_Selected;
        opt.state &= ~QStyle::State_MouseOver;
        opt.features &= ~QStyleOptionViewItem::Alternate;
        opt.backgroundBrush = QBrush();
        
        if (selected) {
            opt.palette.setColor(QPalette::Text, Qt::white);
        }

        // 按照 8 列架构重构：状态列、评分列由代理独立绘制；Name 列作为名称列，具有微型圆角卡片预览
        int col = index.column();
        if (col == static_cast<int>(FileListColumn::Name) && m_drawMiniCards) {
            // 自定义绘制名称列与最左侧圆角卡片
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setRenderHint(QPainter::SmoothPixmapTransform);

            RowLayout layout = RowLayoutEngine::calculate(option.rect, option.rect.height());
            QRect squareRect = layout.cardRect;
            QRect textRect   = layout.textRect;

            // 1. 绘制微型卡片背景（严格保持 Version-1 / Version-2 的纯透明背景底板）
            painter->setPen(Qt::NoPen);
            painter->setBrush(Qt::transparent);
            QPainterPath cardPath;
            cardPath.addRoundedRect(squareRect, 4, 4);
            painter->drawPath(cardPath);

            // 2. 图像/图标平滑居中绘制（严格保持 Version-1 / Version-2 的物理等比居中渲染模式）
            QVariant decoData = index.data(Qt::DecorationRole);
            bool hasThumb = index.data(HasThumbnailRole).toBool();

            if (hasThumb) {
                QPixmap thumb;
                if (decoData.canConvert<QPixmap>()) {
                    thumb = decoData.value<QPixmap>();
                } else if (decoData.canConvert<QIcon>()) {
                    QIcon icon = decoData.value<QIcon>();
                    if (!icon.isNull()) thumb = icon.pixmap(squareRect.size());
                }

                if (!thumb.isNull()) {
                    painter->save();
                    QPixmap scaled = thumb.scaled(squareRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    int x = squareRect.center().x() - scaled.width() / 2;
                    int y = squareRect.center().y() - scaled.height() / 2;
                    QRect targetRect(x, y, scaled.width(), scaled.height());

                    QPainterPath clipPath;
                    clipPath.addRoundedRect(targetRect, 4, 4);
                    painter->setClipPath(clipPath);

                    painter->drawPixmap(targetRect, scaled);

                    painter->restore();
                } else {
                    QIcon icon = qvariant_cast<QIcon>(decoData);
                    if (!icon.isNull()) {
                        int iconW = qRound(squareRect.width() * 0.75);
                        int iconH = qRound(squareRect.height() * 0.75);
                        if (squareRect.width() > 70 || squareRect.height() > 69) {
                            iconW = std::min(iconW, 45);
                            iconH = std::min(iconH, 35);
                        }
                        QPixmap iconPixmap = icon.pixmap(QSize(iconW, iconH));
                        if (!iconPixmap.isNull() && (iconPixmap.width() > iconW || iconPixmap.height() > iconH)) {
                            iconPixmap = iconPixmap.scaled(QSize(iconW, iconH), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                        }
                        if (!iconPixmap.isNull()) {
                            QRect iconRect(squareRect.center().x() - iconPixmap.width() / 2,
                                           squareRect.center().y() - iconPixmap.height() / 2,
                                           iconPixmap.width(), iconPixmap.height());
                            painter->drawPixmap(iconRect, iconPixmap);
                        }
                    }
                }
            } else {
                QIcon icon = qvariant_cast<QIcon>(decoData);
                if (!icon.isNull()) {
                    int iconW = qRound(squareRect.width() * 0.75);
                    int iconH = qRound(squareRect.height() * 0.75);
                    if (squareRect.width() > 70 || squareRect.height() > 69) {
                        iconW = std::min(iconW, 45);
                        iconH = std::min(iconH, 35);
                    }
                    QPixmap iconPixmap = icon.pixmap(QSize(iconW, iconH));
                    if (!iconPixmap.isNull() && (iconPixmap.width() > iconW || iconPixmap.height() > iconH)) {
                        iconPixmap = iconPixmap.scaled(QSize(iconW, iconH), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    }
                    if (!iconPixmap.isNull()) {
                        QRect iconRect(squareRect.center().x() - iconPixmap.width() / 2,
                                       squareRect.center().y() - iconPixmap.height() / 2,
                                       iconPixmap.width(), iconPixmap.height());
                        painter->drawPixmap(iconRect, iconPixmap);
                    }
                }
            }

            // 3. 空文件夹绘制青蓝色虚线框 (#41F2F2 Qt::DashLine)
            bool isFolder = (index.data(TypeRole).toString() == "folder");
            bool isEmpty = index.data(IsEmptyRole).toBool();
            if (isFolder && isEmpty) {
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(QPen(QColor("#41F2F2"), 1, Qt::DashLine));
                painter->setBrush(Qt::NoBrush);
                painter->drawRoundedRect(squareRect, 4, 4);
                painter->restore();
            }

            // 4. 文本排版向右偏移：使用统一文本矩形，并绘制内联置顶图标
            QString name = index.data(Qt::DisplayRole).toString();
            QColor textColor = selected ? QColor("#FFFFFF") : QColor("#EEEEEE");

            painter->setPen(textColor);
            painter->setFont(option.font);

            bool isPinned = index.data(PinnedRole).toBool();
            bool isMarked = index.data(Qt::UserRole + 10).toBool();
            int pinRightX = textRect.right() - CardPainterHelper::kPinRightPadding;
            int pinW = CardPainterHelper::drawInlinePinIcon(painter, pinRightX, textRect.top(), textRect.height(), isPinned);

            if (isMarked) {
                QRect markIconRect(pinRightX - (pinW > 0 ? (pinW + 4) : 0) - 16, textRect.top() + (textRect.height() - 16) / 2, 16, 16);
                QIcon markIcon = UiHelper::getIcon("Destination.svg", QColor("#ff551c"), 16);
                markIcon.paint(painter, markIconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
                pinW += 20;
            }

            int reservedW = (pinW > 0) ? (pinW + CardPainterHelper::kPinToRatingGap) : 0;
            QRect actualTextRect = textRect.adjusted(0, 0, -reservedW, 0);
            QString elidedText = option.fontMetrics.elidedText(name, Qt::ElideMiddle, actualTextRect.width() - 10);
            painter->drawText(actualTextRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);

            painter->restore();
        } else if (col == static_cast<int>(FileListColumn::Rating)) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            QModelIndex idx0 = index.model()->index(index.row(), static_cast<int>(FileListColumn::Name));

            int rating = idx0.data(RatingRole).toInt();
            bool isSelected = option.state & QStyle::State_Selected;
            QString colorName = idx0.data(ColorRole).toString();

            if (rating > 0 || isSelected || !colorName.isEmpty()) {
                RatingBarMetrics rm = RatingBarLayout::calculate(option.rect, RatingBarMode::TreeRow);

                CardPainterHelper::drawRatingStars(painter, rm.banRect, option.rect, rm.starSize, rm.starSpacing, 
                                                  option.rect.top(), option.rect.height(), rm.starsStartX,
                                                  rating, colorName, isSelected);
            }
            painter->restore();
        } else {
            QStyledItemDelegate::paint(painter, opt, index);
        }
        painter->restore();
    }

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (index.column() == static_cast<int>(FileListColumn::Name) && m_drawMiniCards) {
            RowLayout layout = RowLayoutEngine::calculate(option.rect, option.rect.height());
            editor->setGeometry(layout.editorRect);
        } else {
            QRect r = option.rect;
            r.adjust(6, 2, -6, -2);
            editor->setGeometry(r);
        }
    }

private:
    bool m_drawMiniCards;
};

} // namespace QuarkMeta
