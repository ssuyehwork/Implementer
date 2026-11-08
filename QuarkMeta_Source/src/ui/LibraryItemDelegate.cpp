#include "LibraryItemDelegate.h"
#include "UiHelper.h"
#include "ViewDragDropHelper.h"
#include <QTreeView>

namespace QuarkMeta {

SectionHoverArea LibraryItemDelegate::hitTestSectionRow(const QRect& rowRect, LibraryNodeKind kind, int count, const QFont& baseFont, const QPoint& pos) {
    if (!rowRect.contains(pos)) {
        return SectionHoverArea::None;
    }

    // 1. 优先判定"+"加号按钮（仅智能文件夹与文件夹存在）
    if (kind == LibraryNodeKind::SectionSmartFolder || kind == LibraryNodeKind::SectionCustom) {
        QRect btnRect = LibraryTreeLayout::addBtnRect(rowRect);
        if (btnRect.contains(pos)) {
            return SectionHoverArea::AddButton;
        }
    }

    // 2. 判定折叠组件块（文字 + 折叠箭头区域）
    QString text = sectionTitleText(kind, count);
    QFont font = sectionFont(baseFont);
    QRect hdrRect = LibraryTreeLayout::sectionHeaderRect(rowRect, text, font);
    if (hdrRect.contains(pos)) {
        return SectionHoverArea::Header;
    }

    // 3. 行内其余空白区域
    return SectionHoverArea::Row;
}

QSize LibraryItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    sz.setHeight(LibraryTreeLayout::kRowHeight);
    return sz;
}

bool LibraryItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {
    if (!index.isValid()) {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    LibraryNodeKind kind = static_cast<LibraryNodeKind>(index.data(NodeKindRole).toInt());

    // 1. 三大分组标头交互
    if (isSectionKind(kind)) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            return true;
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouseEv = static_cast<QMouseEvent*>(event);
            if (mouseEv && mouseEv->button() == Qt::LeftButton) {
                int countVal = index.data(CountRole).toInt();
                SectionHoverArea area = hitTestSectionRow(option.rect, kind, countVal, option.font, mouseEv->pos());
                if (area == SectionHoverArea::AddButton) {
                    emit requestAddCategory(kind);
                    return true;
                } else if (area == SectionHoverArea::Header) {
                    emit requestToggleSection(kind);
                    return true;
                }
                return true; // 行内空白点击被消费掉，防止误触或漂移
            }
        }
    } else if (kind == LibraryNodeKind::Category && model && model->hasChildren(index)) {
        // 2. 包含子分类的分类项：图标左侧折叠箭头点击检测
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouseEv = static_cast<QMouseEvent*>(event);
            if (mouseEv && mouseEv->button() == Qt::LeftButton) {
                int depth = LibraryTreeLayout::customDepth(index);
                QRect arrowR = LibraryTreeLayout::arrowRect(option.rect, depth);
                QRect arrowClickRect(arrowR.left() - 2, arrowR.top() - 2, arrowR.width() + 4, arrowR.height() + 4);

                if (arrowClickRect.contains(mouseEv->pos())) {
                    QTreeView* treeView = qobject_cast<QTreeView*>(const_cast<QWidget*>(option.widget));
                    if (treeView) {
                        treeView->setExpanded(index, !treeView->isExpanded(index));
                        return true;
                    }
                }
            }
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void LibraryItemDelegate::paintHighlightedText(QPainter* painter, const QRect& rect, const QString& text, const QString& keyword, const QFont& font, const QColor& normalColor, const QColor& matchColor) const {
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

void LibraryItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    LibraryNodeKind kind = static_cast<LibraryNodeKind>(index.data(NodeKindRole).toInt());

    // 1. 渲染三大分组标头 (快速访问, 智能文件夹, 文件夹)，绝对禁止绘制选中/拖放背景
    if (isSectionKind(kind)) {
        // 全行底色严格透明，保证②区域（中间空白留白）彻底透明，绝不与两端连接成一体
        painter->fillRect(opt.rect, Qt::transparent);

        // 从 LibraryTreeView 获取悬停状态作为唯一真理源（SSOT）
        const QTreeView* treeView = qobject_cast<const QTreeView*>(option.widget);
        const auto* libTree = option.widget;
        bool isRowHovered = false;
        bool isHdrHovered = false;
        bool isAddBtnHovered = false;

        if (libTree) {
            // 通过属性或直接反射调用读取悬停状态
            QVariant hIdxVar = libTree->property("_libTreeHoveredIndex");
            QVariant hAreaVar = libTree->property("_libTreeHoveredArea");
            if (hIdxVar.isValid() && hIdxVar.canConvert<QModelIndex>()) {
                QModelIndex hoveredIdx = hIdxVar.value<QModelIndex>();
                if (hoveredIdx == index) {
                    SectionHoverArea area = static_cast<SectionHoverArea>(hAreaVar.toInt());
                    isRowHovered = (area != SectionHoverArea::None);
                    isHdrHovered = (area == SectionHoverArea::Header);
                    isAddBtnHovered = (area == SectionHoverArea::AddButton);
                }
            }
        }

        bool isExpanded = treeView ? treeView->isExpanded(index) : true;
        int countVal = index.data(CountRole).toInt();

        QString sectionText = sectionTitleText(kind, countVal);
        QFont hdrFont = sectionFont(opt.font);
        QRect hdrRect = LibraryTreeLayout::sectionHeaderRect(opt.rect, sectionText, hdrFont);

        // 独立绘制 ③ 区域的高亮背景与内容
        if (isHdrHovered) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor("#2A2D2E"));
            painter->drawRoundedRect(hdrRect, 3, 3);
        }

        QFontMetrics fm(hdrFont);
        int textWidth = fm.horizontalAdvance(sectionText);
        int textLeft = hdrRect.left() + LibraryTreeLayout::kSectionPaddingLeft;
        QRect textRect(textLeft, opt.rect.top(), textWidth, opt.rect.height());

        painter->setFont(hdrFont);
        painter->setPen(isHdrHovered ? QColor("#FFFFFF") : (isRowHovered ? QColor("#DDDDDD") : QColor("#888888")));
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, sectionText);

        // 绘制紧跟文字右侧的展开/折叠箭头 (chevron_down / chevron_right)
        QString arrowKey = isExpanded ? QStringLiteral("chevron_down") : QStringLiteral("chevron_right");
        QIcon arrowIcon = UiHelper::getIcon(arrowKey, isHdrHovered ? QColor("#FFFFFF") : (isRowHovered ? QColor("#CCCCCC") : QColor("#888888")), LibraryTreeLayout::kArrowSize);
        int arrowLeft = textRect.right() + LibraryTreeLayout::kArrowSpacing;
        QRect arrowRect(arrowLeft, opt.rect.top() + (opt.rect.height() - LibraryTreeLayout::kArrowSize) / 2, LibraryTreeLayout::kArrowSize, LibraryTreeLayout::kArrowSize);
        arrowIcon.paint(painter, arrowRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);

        // ① 区域：右侧加号按钮 (+) 图标 ("add")，仅当整行处于悬停时才显示，且高亮完全独立
        if ((kind == LibraryNodeKind::SectionSmartFolder || kind == LibraryNodeKind::SectionCustom) && isRowHovered) {
            QRect addRect = LibraryTreeLayout::addBtnRect(opt.rect);

            if (isAddBtnHovered) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor("#3A3D3E"));
                painter->drawRoundedRect(addRect, 3, 3);
            }

            QIcon addIcon = UiHelper::getIcon("add", isAddBtnHovered ? QColor("#FFFFFF") : QColor("#CCCCCC"), LibraryTreeLayout::kAddBtnIconSize);
            addIcon.paint(painter, addRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }

        painter->restore();
        return;
    }

    // 绘制普通节点背景
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

    // 2. 渲染系统分类与普通自定义分类、智能文件夹节点
    int depth = LibraryTreeLayout::customDepth(index);
    bool isCategoryNode = (kind == LibraryNodeKind::Category);

    // 若当前为包含子分类的父分类节点，在其图标左侧绘制折叠箭头 (chevron_down / chevron_right)
    const QTreeView* treeView = qobject_cast<const QTreeView*>(option.widget);
    bool hasChildren = index.model() ? index.model()->hasChildren(index) : false;
    bool isExpanded = treeView ? treeView->isExpanded(index) : true;

    if (isCategoryNode && hasChildren) {
        QString arrowKey = isExpanded ? QStringLiteral("chevron_down") : QStringLiteral("chevron_right");
        QIcon arrowIcon = UiHelper::getIcon(arrowKey, QColor("#888888"), LibraryTreeLayout::kArrowSize);
        QRect arrowR = LibraryTreeLayout::arrowRect(opt.rect, depth);
        arrowIcon.paint(painter, arrowR, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
    }

    int count = index.data(CountRole).toInt();
    QString countStr = LibraryTreeLayout::countText(count);
    int countWidth = countStr.isEmpty() ? 0 : (opt.fontMetrics.horizontalAdvance(countStr) + LibraryTreeLayout::kCountTextExtraWidth);

    QRect iconR = LibraryTreeLayout::iconRect(opt.rect, depth, isCategoryNode);
    QRect textR = LibraryTreeLayout::textRect(opt.rect, iconR, countWidth);

    QVariant decoData = index.data(Qt::DecorationRole);
    if (decoData.canConvert<QIcon>()) {
        QIcon icon = decoData.value<QIcon>();
        if (!icon.isNull()) {
            icon.paint(painter, iconR, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }
    }

    QString text = index.data(Qt::DisplayRole).toString();
    QColor normalTextColor = (opt.state & QStyle::State_Selected) ? QColor("#FFFFFF") : QColor("#EEEEEE");
    QColor matchTextColor("#3498db");

    if (isUserNodeKind(kind)) {
        // 自定义分类、智能文件夹与镜像节点绘制关键字匹配高亮
        paintHighlightedText(painter, textR, text, m_filterKeyword, opt.font, normalTextColor, matchTextColor);
    } else {
        // 系统分类节点不高亮，正常绘制
        painter->setFont(opt.font);
        painter->setPen(normalTextColor);
        painter->drawText(textR, Qt::AlignLeft | Qt::AlignVCenter, opt.fontMetrics.elidedText(text, Qt::ElideRight, textR.width() - 4));
    }

    // 绘制右对齐数字计数
    if (!countStr.isEmpty()) {
        QRect countR = LibraryTreeLayout::countRect(opt.rect, countWidth);
        QFont countFont = opt.font;
        countFont.setBold(true);
        painter->setFont(countFont);
        painter->setPen((opt.state & QStyle::State_Selected) ? QColor("#FFFFFF") : QColor("#999999"));
        painter->drawText(countR, Qt::AlignRight | Qt::AlignVCenter, countStr);
    }

    painter->restore();
}

void LibraryItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (!editor || !index.isValid()) return;

    LibraryNodeKind kind = static_cast<LibraryNodeKind>(index.data(NodeKindRole).toInt());
    if (!isUserNodeKind(kind)) return;

    int depth = LibraryTreeLayout::customDepth(index);
    bool isCategoryNode = (kind == LibraryNodeKind::Category);

    int count = index.data(CountRole).toInt();
    QString countStr = LibraryTreeLayout::countText(count);
    int countWidth = countStr.isEmpty() ? 0 : (option.fontMetrics.horizontalAdvance(countStr) + LibraryTreeLayout::kCountTextExtraWidth);

    QRect edRect = LibraryTreeLayout::editorRect(option.rect, depth, isCategoryNode, countWidth);
    editor->setGeometry(edRect);
}

} // namespace QuarkMeta
