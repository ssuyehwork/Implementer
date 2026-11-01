#include "LibraryPanel.h"
#include "controllers/ContentKeyHandler.h"
#include <QKeyEvent>
#include "UiHelper.h"
#include "ToolTipOverlay.h"
#include "PresetTagsDialog.h"
#include "ColorPicker.h"
#include "ShellIconManager.h"
#include "ViewDragDropHelper.h"
#include "../meta/LibraryDao.h"
#include "../meta/SmartFolderDao.h"
#include "dialogs/SmartFolderDialog.h"
#include "../meta/LibraryService.h"
#include "../meta/MetaIndexService.h"
#include "../meta/DatabaseManager.h"
#include <QtConcurrent/QtConcurrent>
#include "../meta/QuarkMetaJsonStore.h"
#include "../core/CoreEngine.h"
#include "../core/PresetTagService.h"
#include "../core/ModelContract.h"
#include "../core/AppConfig.h"
#include "controllers/ContextMenuFactory.h"
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QWidgetAction>
#include <QGridLayout>
#include <QHeaderView>
#include <QFileInfo>
#include <QDir>
#include <QCursor>
#include <QPainter>
#include <QLineEdit>
#include <QTimer>
#include <QMouseEvent>
#include <functional>

namespace QuarkMeta {

QRect LibraryItemDelegate::addBtnRect(const QRect& optionRect) {
    int size = 16;
    int top = optionRect.top() + (optionRect.height() - size) / 2;
    int rightPadding = 12;
    int left = optionRect.right() - rightPadding - size;
    return QRect(left, top, size, size);
}

QSize LibraryItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    sz.setHeight(28);
    return sz;
}

bool LibraryItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {
    if (!index.isValid()) {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    int nodeId = index.data(Qt::UserRole + 1).toInt();
    if (nodeId == -10 || nodeId == -20 || nodeId == 0) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            return true;
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouseEv = static_cast<QMouseEvent*>(event);
            if (mouseEv && mouseEv->button() == Qt::LeftButton) {
                if (nodeId == -20 || nodeId == 0) {
                    QRect btnRect = addBtnRect(option.rect);
                    if (btnRect.contains(mouseEv->pos())) {
                        emit requestAddCategory(nodeId);
                        return true;
                    }
                }
                emit requestToggleSection(nodeId);
                return true;
            }
        }
    } else if (nodeId > 0 && model && model->hasChildren(index)) {
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouseEv = static_cast<QMouseEvent*>(event);
            if (mouseEv && mouseEv->button() == Qt::LeftButton) {
                // 计算该节点图标左侧折叠箭头的区域
                int customDepth = 0;
                QModelIndex parentIdx = index.parent();
                while (parentIdx.isValid()) {
                    if (parentIdx.data(Qt::UserRole + 1).toInt() > 0) {
                        customDepth++;
                    }
                    parentIdx = parentIdx.parent();
                }
                int arrowSize = 14;
                int arrowLeft = option.rect.left() + 10 + customDepth * 14;
                QRect arrowClickRect(arrowLeft - 2, option.rect.top() + (option.rect.height() - arrowSize) / 2 - 2, arrowSize + 4, arrowSize + 4);

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

    int nodeId = index.data(Qt::UserRole + 1).toInt();

    // 1. 渲染三大分组 Header (快速访问 -10, 智能文件夹 -20, 文件夹 0)，不得绘制选中/拖放背景
    if (nodeId == -10 || nodeId == -20 || nodeId == 0) {
        painter->fillRect(opt.rect, Qt::transparent);

        const QTreeView* treeView = qobject_cast<const QTreeView*>(option.widget);
        bool isExpanded = treeView ? treeView->isExpanded(index) : true;
        int countVal = index.data(Qt::UserRole + 9).toInt();

        QString sectionText;
        if (nodeId == -10) sectionText = QString("快速访问(%1)").arg(countVal);
        else if (nodeId == -20) sectionText = QString("智能文件夹(%1)").arg(countVal);
        else sectionText = QString("文件夹(%1)").arg(countVal);

        QFont hdrFont = opt.font;
        hdrFont.setPointSize(10);
        hdrFont.setBold(true);

        QFontMetrics fm(hdrFont);
        int textWidth = fm.horizontalAdvance(sectionText);
        int textLeft = opt.rect.left() + 10;
        QRect textRect(textLeft, opt.rect.top(), textWidth, opt.rect.height());

        painter->setFont(hdrFont);
        painter->setPen((opt.state & QStyle::State_MouseOver) ? QColor("#FFFFFF") : QColor("#888888"));
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, sectionText);

        // 绘制紧跟文字右侧的展开/折叠箭头 (chevron_down / chevron_right)
        QString arrowKey = isExpanded ? "chevron_down" : "chevron_right";
        QIcon arrowIcon = UiHelper::getIcon(arrowKey, QColor("#888888"), 14);
        int arrowLeft = textRect.right() + 4;
        QRect arrowRect(arrowLeft, opt.rect.top() + (opt.rect.height() - 14) / 2, 14, 14);
        arrowIcon.paint(painter, arrowRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);

        // 智能文件夹与文件夹分组右侧绘制加号按钮 (+) 图标 ("add")
        if (nodeId == -20 || nodeId == 0) {
            QRect addRect = addBtnRect(opt.rect);
            QIcon addIcon = UiHelper::getIcon("add", (opt.state & QStyle::State_MouseOver) ? QColor("#FFFFFF") : QColor("#888888"), 16);
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

    // 2. 渲染系统分类与普通自定义分类节点 (nodeId != 0)
    int customDepth = 0;
    QModelIndex parentIdx = index.parent();
    while (parentIdx.isValid()) {
        if (parentIdx.data(Qt::UserRole + 1).toInt() > 0) {
            customDepth++;
        }
        parentIdx = parentIdx.parent();
    }

    int arrowSize = 14;
    int arrowSpacing = 4;
    int leftMargin = 10 + customDepth * 14;
    int iconSize = 18;
    int spacing = 8;
    int rightMargin = 10;

    // 若当前为包含子分类的父分类节点，在其图标左侧绘制折叠箭头 (chevron_down / chevron_right)
    const QTreeView* treeView = qobject_cast<const QTreeView*>(option.widget);
    bool hasChildren = index.model() ? index.model()->hasChildren(index) : false;
    bool isExpanded = treeView ? treeView->isExpanded(index) : true;

    if (nodeId > 0 && hasChildren) {
        QString arrowKey = isExpanded ? "chevron_down" : "chevron_right";
        QIcon arrowIcon = UiHelper::getIcon(arrowKey, QColor("#888888"), arrowSize);
        QRect arrowRect(opt.rect.left() + leftMargin, opt.rect.top() + (opt.rect.height() - arrowSize) / 2, arrowSize, arrowSize);
        arrowIcon.paint(painter, arrowRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
    }

    // 父节点的图标向右平移 (arrowSize + arrowSpacing)，保持与子节点/无子节点图标对齐
    int iconLeft = opt.rect.left() + leftMargin + (nodeId > 0 ? (arrowSize + arrowSpacing) : 0);

    int count = index.data(Qt::UserRole + 9).toInt();
    QString countStr;
    if (count == -999) {
        countStr = "…";
    } else if (count >= 0) {
        countStr = QString::number(count);
    }
    int countWidth = countStr.isEmpty() ? 0 : (opt.fontMetrics.horizontalAdvance(countStr) + 12);

    QRect iconRect(iconLeft, opt.rect.top() + (opt.rect.height() - iconSize) / 2, iconSize, iconSize);
    QRect textRect(iconRect.right() + spacing, opt.rect.top(), opt.rect.width() - (iconLeft - opt.rect.left()) - iconSize - spacing - countWidth - rightMargin, opt.rect.height());

    QVariant decoData = index.data(Qt::DecorationRole);
    if (decoData.canConvert<QIcon>()) {
        QIcon icon = decoData.value<QIcon>();
        if (!icon.isNull()) {
            icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }
    }

    QString text = index.data(Qt::DisplayRole).toString();
    QColor normalTextColor = (opt.state & QStyle::State_Selected) ? QColor("#FFFFFF") : QColor("#EEEEEE");
    QColor matchTextColor("#3498db");

    if (nodeId > 0) {
        // 自定义分类节点绘制关键字匹配高亮
        paintHighlightedText(painter, textRect, text, m_filterKeyword, opt.font, normalTextColor, matchTextColor);
    } else {
        // 系统分类节点不高亮，正常绘制
        painter->setFont(opt.font);
        painter->setPen(normalTextColor);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, opt.fontMetrics.elidedText(text, Qt::ElideRight, textRect.width() - 4));
    }

    // 绘制右对齐数字计数
    if (!countStr.isEmpty()) {
        QRect countRect(opt.rect.right() - countWidth - rightMargin, opt.rect.top(), countWidth, opt.rect.height());
        QFont countFont = opt.font;
        countFont.setBold(true);
        painter->setFont(countFont);
        painter->setPen((opt.state & QStyle::State_Selected) ? QColor("#FFFFFF") : QColor("#999999"));
        painter->drawText(countRect, Qt::AlignRight | Qt::AlignVCenter, countStr);
    }

    painter->restore();
}

void LibraryItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (!editor || !index.isValid()) return;

    int nodeId = index.data(Qt::UserRole + 1).toInt();
    if (nodeId <= 0) return;

    int customDepth = 0;
    QModelIndex parentIdx = index.parent();
    while (parentIdx.isValid()) {
        if (parentIdx.data(Qt::UserRole + 1).toInt() > 0) {
            customDepth++;
        }
        parentIdx = parentIdx.parent();
    }

    int arrowSize = 14;
    int arrowSpacing = 4;
    int leftMargin = 10 + customDepth * 14;
    int iconSize = 18;
    int spacing = 8;
    int rightMargin = 10;

    int iconLeft = option.rect.left() + leftMargin + (nodeId > 0 ? (arrowSize + arrowSpacing) : 0);
    int textLeft = iconLeft + iconSize + spacing;

    int count = index.data(Qt::UserRole + 9).toInt();
    QString countStr;
    if (count == -999) {
        countStr = "…";
    } else if (count >= 0) {
        countStr = QString::number(count);
    }
    int countWidth = countStr.isEmpty() ? 0 : (option.fontMetrics.horizontalAdvance(countStr) + 12);

    QRect editorRect = option.rect;
    editorRect.setLeft(textLeft - 2);
    editorRect.setRight(option.rect.right() - rightMargin - countWidth);
    editorRect.setTop(option.rect.top() + 2);
    editorRect.setHeight(option.rect.height() - 4);

    editor->setGeometry(editorRect);
}

LibraryPanel::LibraryPanel(QWidget* parent) : QFrame(parent) {
    setObjectName("LibraryContainer");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(230);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    initUi();
    loadLibrary();
}

void LibraryPanel::initUi() {
    m_treeView = new LibraryTreeView(this);
    m_treeView->setObjectName("LibraryTreeView");
    m_treeView->setHeaderHidden(true);
    if (m_treeView->header()) {
        m_treeView->header()->setStretchLastSection(true);
        m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    }
    m_treeView->setIndentation(0);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setDragEnabled(true);
    m_treeView->setAcceptDrops(true);
    m_treeView->setDropIndicatorShown(true);
    m_treeView->setDefaultDropAction(Qt::MoveAction);
    m_treeView->setDragDropMode(QAbstractItemView::DragDrop);
    m_treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_treeView->installEventFilter(this);
    if (m_treeView->viewport()) {
        m_treeView->viewport()->installEventFilter(this);
    }

    m_model = new QStandardItemModel(this);
    m_treeView->setModel(m_model);
    m_delegate = new LibraryItemDelegate(this);
    m_treeView->setItemDelegate(m_delegate);

    connect(m_delegate, &LibraryItemDelegate::requestAddCategory, this, [this](int nodeId) {
        if (nodeId == -20) {
            // 触发智能文件夹规则配置对话框
            SmartFolderDialog dlg(this);
            if (dlg.exec() == QDialog::Accepted) {
                SmartFolderRecord rec = dlg.getRecord();
                int id = SmartFolderDao::addSmartFolder(rec);
                if (id > 0) {
                    invalidateSmartFolderCache(id);
                    loadLibrary();
                }
            }
        } else {
            createAndEditCategory(0);
        }
    });
    connect(m_delegate, &LibraryItemDelegate::requestToggleSection, this, &LibraryPanel::toggleSectionByNodeId);

    m_mainLayout->addWidget(m_treeView, 1);

    // 启用树内拖拽重排序底座
    m_treeView->setInternalReorderEnabled(true);

    m_treeView->setInternalDragValidator([](const QModelIndex& index) {
        if (!index.isValid()) return false;
        int nodeId = index.data(Qt::UserRole + 1).toInt();
        return nodeId > 0;
    });

    m_treeView->setInternalDropValidator([this](const QList<QPersistentModelIndex>& sources, const QModelIndex& target, InternalDropZone zone) -> bool {
        if (sources.isEmpty()) return false;

        // 校验 sources 节点：全为 nodeId > 0
        for (const QPersistentModelIndex& psrc : sources) {
            if (!psrc.isValid()) return false;
            int srcId = psrc.data(Qt::UserRole + 1).toInt();
            if (srcId <= 0) return false;
        }

        auto isSourceOrDescendant = [](const QModelIndex& idx, const QList<QPersistentModelIndex>& srcList) -> bool {
            QModelIndex cur = idx;
            while (cur.isValid()) {
                for (const QPersistentModelIndex& psrc : srcList) {
                    if (psrc.isValid() && psrc == cur) return true;
                }
                cur = cur.parent();
            }
            return false;
        };

        QStandardItem* sectionItem = findItemByNodeId(m_model->invisibleRootItem(), 0);
        if (!sectionItem) return false;

        QStandardItem* targetParentItem = nullptr;
        int destRow = 0;

        if (zone == InternalDropZone::OnItem) {
            if (!target.isValid()) return false;
            int targetId = target.data(Qt::UserRole + 1).toInt();
            if (targetId == -10) {
                return true; // 允许拖拽放置到快速访问标头上以置顶
            }
            if (targetId <= 0) return false; // 其他系统行或标题行不能作为 OnItem 目标

            if (isSourceOrDescendant(target, sources)) return false;

            targetParentItem = m_model->itemFromIndex(target);
            if (targetParentItem) destRow = targetParentItem->rowCount();
        } else if (zone == InternalDropZone::Above || zone == InternalDropZone::Below) {
            if (!target.isValid()) return false;
            int targetId = target.data(Qt::UserRole + 1).toInt();
            if (targetId <= 0) return false; // 系统行或标题行不能作为 Above/Below 目标

            for (const QPersistentModelIndex& psrc : sources) {
                if (psrc.isValid() && psrc == target) return false;
            }
            if (target.parent().isValid() && isSourceOrDescendant(target.parent(), sources)) return false;

            targetParentItem = target.parent().isValid() ? m_model->itemFromIndex(target.parent()) : sectionItem;
            destRow = (zone == InternalDropZone::Above) ? target.row() : target.row() + 1;
        } else if (zone == InternalDropZone::Blank) {
            targetParentItem = sectionItem;
            destRow = sectionItem->rowCount();
        }

        if (!targetParentItem) return false;

        bool anyChanged = false;
        for (const QPersistentModelIndex& psrc : sources) {
            if (!psrc.isValid()) continue;
            QStandardItem* srcItem = m_model->itemFromIndex(psrc);
            if (!srcItem) continue;

            QStandardItem* srcParentItem = srcItem->parent() ? srcItem->parent() : sectionItem;
            int srcRow = srcItem->row();

            if (srcParentItem != targetParentItem) {
                anyChanged = true;
                break;
            } else {
                if (zone == InternalDropZone::OnItem || zone == InternalDropZone::Blank) {
                    if (srcRow != targetParentItem->rowCount() - 1) {
                        anyChanged = true;
                        break;
                    }
                } else if (zone == InternalDropZone::Above || zone == InternalDropZone::Below) {
                    if (srcRow != destRow && srcRow != destRow - 1) {
                        anyChanged = true;
                        break;
                    }
                }
            }
        }

        return anyChanged;
    });

    connect(m_treeView, &DropTreeView::internalMoveRequested, this, &LibraryPanel::onInternalMoveRequested);

    connect(m_treeView, &QTreeView::clicked, this, &LibraryPanel::onCategoryClicked);
    connect(m_treeView, &QWidget::customContextMenuRequested, this, &LibraryPanel::onCategoryContextMenu);
    connect(m_treeView, &DropTreeView::pathsDropped, this, &LibraryPanel::onPathsDroppedToCategory);

    connect(m_model, &QStandardItemModel::rowsMoved, this, &LibraryPanel::markOrderDirty, Qt::DirectConnection);
    connect(m_model, &QStandardItemModel::rowsInserted, this, &LibraryPanel::markOrderDirty, Qt::DirectConnection);
    connect(m_model, &QStandardItemModel::rowsRemoved, this, &LibraryPanel::markOrderDirty, Qt::DirectConnection);

    connect(&LibraryService::instance(), &LibraryService::libraryChanged, this, [this]() {
        QTimer::singleShot(0, this, [this]() {
            loadLibrary();
        });
    });

    connect(m_model, &QStandardItemModel::itemChanged, this, [this](QStandardItem* item) {
        if (!item || m_isLoading) return;
        int nodeId = item->data(Qt::UserRole + 1).toInt();
        if (nodeId > 0) {
            QString name = item->text();
            QString iconKey = item->data(Qt::UserRole + 2).toString();
            QString colorHex = item->data(Qt::UserRole + 3).toString();
            LibraryDao::updateCategoryNode(nodeId, name, iconKey, colorHex);
            // 🚀【消除整树暴力重建】：节点重命名直接在本地 Item 生效，严禁粗暴 clear() 重建整棵树
        }
    });
}

void LibraryPanel::markOrderDirty() {
    if (m_isLoading) return;
    m_orderDirty = true;
    QTimer::singleShot(0, this, &LibraryPanel::flushOrder);
}

void LibraryPanel::flushOrder() {
    if (!m_orderDirty) return;
    saveLibraryOrder();
    m_orderDirty = false;
}

void LibraryPanel::onInternalMoveRequested(const QList<QPersistentModelIndex>& sources, const QModelIndex& target, InternalDropZone zone) {
    if (sources.isEmpty() || !m_model) return;

    QStandardItem* sectionItem = findItemByNodeId(m_model->invisibleRootItem(), 0);
    if (!sectionItem) return;

    QStandardItem* parentItem = nullptr;
    int destRow = 0;

    if (zone == InternalDropZone::OnItem) {
        if (target.isValid()) {
            int targetId = target.data(Qt::UserRole + 1).toInt();
            if (targetId == -10) {
                // 拖拽到“快速访问”标头：将选中的节点全量触发置顶
                for (const QPersistentModelIndex& pidx : sources) {
                    if (pidx.isValid()) {
                        int srcId = pidx.data(Qt::UserRole + 1).toInt();
                        if (srcId > 0) {
                            LibraryDao::setCategoryPinned(srcId, true);
                        }
                    }
                }
                loadLibrary();
                return;
            }

            parentItem = m_model->itemFromIndex(target);
            if (parentItem) {
                destRow = parentItem->rowCount();
            }
        }
    } else if (zone == InternalDropZone::Above) {
        if (target.isValid()) {
            parentItem = (target.parent().isValid() && target.parent().data(Qt::UserRole + 1).toInt() != 0)
                         ? m_model->itemFromIndex(target.parent())
                         : sectionItem;
            destRow = target.row();
        }
    } else if (zone == InternalDropZone::Below) {
        if (target.isValid()) {
            parentItem = (target.parent().isValid() && target.parent().data(Qt::UserRole + 1).toInt() != 0)
                         ? m_model->itemFromIndex(target.parent())
                         : sectionItem;
            destRow = target.row() + 1;
        }
    } else if (zone == InternalDropZone::Blank) {
        parentItem = sectionItem;
        destRow = sectionItem->rowCount();
    }

    if (!parentItem) {
        parentItem = sectionItem;
        destRow = sectionItem->rowCount();
    }

    // 1. 记录展开状态
    QMap<int, bool> expandedMap;
    std::function<void(QStandardItem*)> saveExpandedStates = [&](QStandardItem* pItem) {
        int rCount = pItem ? pItem->rowCount() : m_model->rowCount();
        for (int i = 0; i < rCount; ++i) {
            QStandardItem* child = pItem ? pItem->child(i) : m_model->item(i);
            if (!child) continue;
            int nodeId = child->data(Qt::UserRole + 1).toInt();
            if (nodeId > 0 && m_treeView) {
                expandedMap[nodeId] = m_treeView->isExpanded(child->index());
            }
            saveExpandedStates(child);
        }
    };
    saveExpandedStates(sectionItem);

    // 2. 转换 itemsToMove
    QList<QStandardItem*> itemsToMove;
    for (const QPersistentModelIndex& pidx : sources) {
        if (pidx.isValid()) {
            QStandardItem* item = m_model->itemFromIndex(pidx);
            if (item) itemsToMove.append(item);
        }
    }
    if (itemsToMove.isEmpty()) return;

    QList<QStandardItem*> movedItemsList;
    for (QStandardItem* item : itemsToMove) {
        QStandardItem* srcParent = item->parent() ? item->parent() : sectionItem;
        int srcRow = item->row();

        QList<QStandardItem*> rowItems = srcParent->takeRow(srcRow);
        if (rowItems.isEmpty()) continue;

        int insertRow = destRow;
        if (srcParent == parentItem && srcRow < insertRow) {
            insertRow--;
        }

        parentItem->insertRow(insertRow, rowItems);
        destRow = insertRow + 1;
        movedItemsList.append(rowItems.first());
    }

    // 3. 恢复展开状态
    std::function<void(QStandardItem*)> restoreExpandedStates = [&](QStandardItem* pItem) {
        int rCount = pItem ? pItem->rowCount() : m_model->rowCount();
        for (int i = 0; i < rCount; ++i) {
            QStandardItem* child = pItem ? pItem->child(i) : m_model->item(i);
            if (!child) continue;
            int nodeId = child->data(Qt::UserRole + 1).toInt();
            if (nodeId > 0 && expandedMap.contains(nodeId) && m_treeView) {
                m_treeView->setExpanded(child->index(), expandedMap.value(nodeId));
            }
            restoreExpandedStates(child);
        }
    };
    restoreExpandedStates(sectionItem);

    // 4. 若放入某分类内部，展开该分类
    if (zone == InternalDropZone::OnItem && target.isValid() && target.data(Qt::UserRole + 1).toInt() > 0) {
        m_treeView->setExpanded(target, true);
    }

    // 5. 恢复选中状态
    if (m_treeView && m_treeView->selectionModel()) {
        m_treeView->selectionModel()->clearSelection();
        for (QStandardItem* movedItem : movedItemsList) {
            m_treeView->selectionModel()->select(movedItem->index(), QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    }

    // 6. 落库保存
    saveLibraryOrder();
    m_orderDirty = false;
}

void LibraryPanel::saveLibraryOrder() {
    if (!m_model || m_isLoading) return;

    QStandardItem* sectionItem = findItemByNodeId(m_model->invisibleRootItem(), 0);
    if (!sectionItem) return;

    QList<NodeOrderEntry> entries;
    int orderCounter = 1;

    std::function<void(QStandardItem*, int)> traverseAndCollect = [&](QStandardItem* parentItem, int parentId) {
        int rowCount = parentItem->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QStandardItem* child = parentItem->child(i);
            if (!child) continue;
            int nodeId = child->data(Qt::UserRole + 1).toInt();
            if (nodeId > 0) {
                NodeOrderEntry entry;
                entry.id = nodeId;
                entry.parentId = parentId;
                entry.sortOrder = orderCounter++;
                entries.append(entry);

                traverseAndCollect(child, nodeId);
            }
        }
    };

    traverseAndCollect(sectionItem, 0);

    if (!entries.isEmpty()) {
        LibraryDao::updateNodesParentAndOrder(entries);
    }
}

void LibraryPanel::sortCategoriesByName(bool ascending) {
    if (!m_model) return;

    QStandardItem* sectionItem = findItemByNodeId(m_model->invisibleRootItem(), 0);
    if (!sectionItem) return;

    std::function<void(QStandardItem*)> sortChildren = [&](QStandardItem* parentItem) {
        int rowCount = parentItem->rowCount();
        if (rowCount <= 1) return;

        QList<QStandardItem*> childItems;
        for (int i = rowCount - 1; i >= 0; --i) {
            QStandardItem* item = parentItem->takeRow(i).first();
            if (item) childItems.prepend(item);
        }

        std::sort(childItems.begin(), childItems.end(), [ascending](QStandardItem* a, QStandardItem* b) {
            return ascending ? (a->text().localeAwareCompare(b->text()) < 0)
                              : (a->text().localeAwareCompare(b->text()) > 0);
        });

        for (QStandardItem* item : childItems) {
            parentItem->appendRow(item);
            sortChildren(item);
        }
    };

    sortChildren(sectionItem);
    saveLibraryOrder();
    m_orderDirty = false;
}

void LibraryPanel::toggleSectionByNodeId(int nodeId) {
    if (!m_model || !m_treeView) return;
    QStandardItem* sectionItem = findItemByNodeId(m_model->invisibleRootItem(), nodeId);
    if (!sectionItem) return;
    QModelIndex idx = sectionItem->index();
    bool isExpanded = m_treeView->isExpanded(idx);
    m_treeView->setExpanded(idx, !isExpanded);

    QString configKey;
    if (nodeId == -10) configKey = "Library/QuickAccessSectionCollapsed";
    else if (nodeId == -20) configKey = "Library/SmartFolderSectionCollapsed";
    else configKey = "Library/CustomSectionCollapsed";

    AppConfig::instance().setValue(configKey, isExpanded);
}

bool LibraryPanel::eventFilter(QObject* watched, QEvent* event) {
    if ((watched == m_treeView || (m_treeView && watched == m_treeView->viewport()))
        && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEv = static_cast<QKeyEvent*>(event);
        if (keyEv->key() == Qt::Key_F2 && keyEv->modifiers() == Qt::NoModifier) {
            QModelIndex curIdx = m_treeView->currentIndex();
            if (curIdx.isValid()) {
                int nodeId = curIdx.data(Qt::UserRole + 1).toInt();
                // 仅允许用户自定义分类（nodeId > 0）或智能文件夹进入行内编辑，屏蔽系统固定项和分组标头
                if (nodeId > 0) {
                    m_treeView->edit(curIdx);
                    keyEv->accept();
                    return true;
                }
            }
        }
        if (keyEv->modifiers() & Qt::ControlModifier) {
            if (keyEv->key() == Qt::Key_Home) {
                if (ContentKeyHandler::navigateViewFirstItem(m_treeView)) return true;
            } else if (keyEv->key() == Qt::Key_End) {
                if (ContentKeyHandler::navigateViewLastItem(m_treeView)) return true;
            }
        }
    }
    return QFrame::eventFilter(watched, event);
}

void LibraryPanel::toggleSection() {
    toggleSectionByNodeId(0);
}

void LibraryPanel::onCategoryClicked(const QModelIndex& index) {
    if (!index.isValid()) return;
    int nodeId = index.data(Qt::UserRole + 1).toInt();
    if (nodeId == -10 || nodeId == -20 || nodeId == 0) {
        return;
    }

    QString nodeName = index.data(Qt::DisplayRole).toString();
    if (index.data(TypeRole).toString() == "smart_folder") {
        SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(nodeId);
        emit smartFolderSelected(rec);
        return;
    }

    QStringList paths = LibraryService::instance().getCategoryPaths(nodeId);
    emit categoryPathsSelected(paths, nodeName, nodeId);
}

void LibraryPanel::filterTreeNodes(const QString& keyword) {
    if (!m_model || !m_treeView) return;

    std::function<bool(QStandardItem*)> filterRecursive = [&](QStandardItem* item) -> bool {
        if (!item) return false;
        int nodeId = item->data(Qt::UserRole + 1).toInt();

        bool matchesSelf = keyword.isEmpty() || item->text().contains(keyword, Qt::CaseInsensitive);
        bool anyChildMatches = false;

        for (int i = 0; i < item->rowCount(); ++i) {
            QStandardItem* child = item->child(i);
            if (filterRecursive(child)) {
                anyChildMatches = true;
                m_treeView->setRowHidden(i, item->index(), false);
            } else {
                m_treeView->setRowHidden(i, item->index(), true);
            }
        }

        if (nodeId == -10 || nodeId == -20 || nodeId == 0) {
            return true;
        }

        bool visible = matchesSelf || anyChildMatches;
        if (!keyword.isEmpty() && visible && item->hasChildren()) {
            m_treeView->expand(item->index());
        }
        return visible;
    };

    for (int i = 0; i < m_model->rowCount(); ++i) {
        QStandardItem* topItem = m_model->item(i);
        if (!topItem) continue;
        int nodeId = topItem->data(Qt::UserRole + 1).toInt();
        if (nodeId == -1 || nodeId == -2) {
            m_treeView->setRowHidden(i, QModelIndex(), false);
        } else if (nodeId == -10 || nodeId == -20 || nodeId == 0) {
            filterRecursive(topItem);
            m_treeView->setRowHidden(i, QModelIndex(), false);
            if (!keyword.isEmpty()) {
                m_treeView->expand(topItem->index());
            } else {
                QString cfgKey = (nodeId == -10) ? "Library/QuickAccessSectionCollapsed" :
                                 (nodeId == -20) ? "Library/SmartFolderSectionCollapsed" : "Library/CustomSectionCollapsed";
                bool sectionCollapsed = AppConfig::instance().getValue(cfgKey, false).toBool();
                m_treeView->setExpanded(topItem->index(), !sectionCollapsed);
            }
        }
    }
}

void LibraryPanel::onCategoryContextMenu(const QPoint& pos) {
    QModelIndex index = m_treeView->indexAt(pos);

    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    if (!index.isValid()) {
        QAction* newCatAct = menu.addAction(UiHelper::getIcon("Data-Classification.svg", QColor("#EEEEEE")), "新建库分类");
        connect(newCatAct, &QAction::triggered, this, [this]() {
            createAndEditCategory(0);
        });

        auto* sortMenu = menu.addMenu(UiHelper::getIcon("list_ul", QColor("#AAAAAA")), "排列");
        UiHelper::applyMenuStyle(sortMenu);
        QAction* sortAsc = sortMenu->addAction("按名称 (A→Z)");
        connect(sortAsc, &QAction::triggered, this, [this]() { sortCategoriesByName(true); });
        QAction* sortDesc = sortMenu->addAction("按名称 (Z→A)");
        connect(sortDesc, &QAction::triggered, this, [this]() { sortCategoriesByName(false); });
        QAction* sortManual = sortMenu->addAction("手动");
        connect(sortManual, &QAction::triggered, this, [this]() { loadLibrary(); });

        menu.exec(m_treeView->viewport()->mapToGlobal(pos));
        return;
    }

    if (index.data(TypeRole).toString() == "smart_folder") {
        int smartFolderId = index.data(Qt::UserRole + 1).toInt();
        QString curIconKey = index.data(Qt::UserRole + 2).toString();
        QString curColorHex = index.data(Qt::UserRole + 3).toString();

        if (curIconKey.isEmpty() || curIconKey == "folder_filled" || curIconKey == "folder") curIconKey = "smart-folder-1.svg";
        if (curColorHex.isEmpty()) curColorHex = "#888888";

        // 右键具体的智能文件夹节点
        QAction* editRuleAct = menu.addAction(UiHelper::getIcon("edit", QColor("#EEEEEE")), "编辑规则");
        connect(editRuleAct, &QAction::triggered, this, [this, smartFolderId]() {
            SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(smartFolderId);
            SmartFolderDialog dlg(rec, this);
            if (dlg.exec() == QDialog::Accepted) {
                SmartFolderRecord updatedRec = dlg.getRecord();
                if (SmartFolderDao::updateSmartFolder(updatedRec)) {
                    invalidateSmartFolderCache(smartFolderId);
                    loadLibrary();
                }
            }
        });
        menu.addSeparator();

        ContextMenuFactory::buildIconPickerMenu(&menu, curIconKey, curColorHex,
            [this, smartFolderId](const QString& iconKey) {
                SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(smartFolderId);
                if (rec.id > 0) {
                    rec.iconKey = iconKey;
                    if (SmartFolderDao::updateSmartFolder(rec)) {
                        loadLibrary();
                    }
                }
            },
            [this, smartFolderId](const QString& hexColor) {
                SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(smartFolderId);
                if (rec.id > 0) {
                    rec.colorHex = hexColor.isEmpty() ? "#888888" : hexColor.toUpper();
                    if (SmartFolderDao::updateSmartFolder(rec)) {
                        loadLibrary();
                    }
                }
            }
        );

        menu.addSeparator();

        QAction* deleteSmartAct = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE")), "删除智能文件夹");
        connect(deleteSmartAct, &QAction::triggered, this, [this, smartFolderId]() {
            if (SmartFolderDao::removeSmartFolderById(smartFolderId)) {
                invalidateSmartFolderCache(smartFolderId);
                loadLibrary();
            }
        });

        menu.exec(m_treeView->viewport()->mapToGlobal(pos));
        return;
    }

    int nodeId = index.data(Qt::UserRole + 1).toInt();
    if (nodeId == -1 || nodeId == -2) {
        // 固定系统分类禁止弹出修改菜单
        return;
    }

    if (nodeId == -10) {
        // 快速访问分组右键菜单
        QAction* clearAct = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE")), "清空快速访问");
        connect(clearAct, &QAction::triggered, this, [this]() {
            // 清空快速访问快捷方式
        });
        menu.exec(m_treeView->viewport()->mapToGlobal(pos));
        return;
    }

    if (nodeId == -20) {
        // 智能文件夹分组右键菜单
        QAction* newSmartAct = menu.addAction(UiHelper::getIcon("smart-folder-1.svg", QColor("#EEEEEE")), "新建智能文件夹");
        connect(newSmartAct, &QAction::triggered, this, [this]() {
            SmartFolderDialog dlg(this);
            if (dlg.exec() == QDialog::Accepted) {
                SmartFolderRecord rec = dlg.getRecord();
                int id = SmartFolderDao::addSmartFolder(rec);
                if (id > 0) {
                    loadLibrary();
                }
            }
        });
        menu.exec(m_treeView->viewport()->mapToGlobal(pos));
        return;
    }

    if (nodeId == 0) {
        // 右键标题行弹出与空白处相同的“新建库分类”与排列菜单
        QAction* newCatAct = menu.addAction(UiHelper::getIcon("Data-Classification.svg", QColor("#EEEEEE")), "新建库分类");
        connect(newCatAct, &QAction::triggered, this, [this]() {
            createAndEditCategory(0);
        });

        auto* sortMenu = menu.addMenu(UiHelper::getIcon("list_ul", QColor("#AAAAAA")), "排列");
        UiHelper::applyMenuStyle(sortMenu);
        QAction* sortAsc = sortMenu->addAction("按名称 (A→Z)");
        connect(sortAsc, &QAction::triggered, this, [this]() { sortCategoriesByName(true); });
        QAction* sortDesc = sortMenu->addAction("按名称 (Z→A)");
        connect(sortDesc, &QAction::triggered, this, [this]() { sortCategoriesByName(false); });
        QAction* sortManual = sortMenu->addAction("手动");
        connect(sortManual, &QAction::triggered, this, [this]() { loadLibrary(); });

        menu.exec(m_treeView->viewport()->mapToGlobal(pos));
        return;
    }

    QString curIconKey = index.data(Qt::UserRole + 2).toString();
    QString curColorHex = index.data(Qt::UserRole + 3).toString();
    bool isPinned = index.data(Qt::UserRole + 4).toBool();

    if (curIconKey.isEmpty() || curIconKey == "folder_filled" || curIconKey == "folder") curIconKey = "Data-Classification.svg";
    if (curColorHex.isEmpty()) curColorHex = "#888888";

    // 绘制置顶/取消置顶菜单项
    if (isPinned) {
        QAction* unpinAct = menu.addAction(UiHelper::getIcon("pin_vertical", QColor("#EEEEEE")), "从“快速访问”移除");
        connect(unpinAct, &QAction::triggered, this, [this, nodeId]() {
            onPinCategoryTriggered(nodeId, false);
        });
    } else {
        QAction* pinAct = menu.addAction(UiHelper::getIcon("pin_tilted", QColor("#EEEEEE")), "添加至“快速访问”");
        connect(pinAct, &QAction::triggered, this, [this, nodeId]() {
            onPinCategoryTriggered(nodeId, true);
        });
    }

    menu.addSeparator();

    QAction* newSubCatAct = menu.addAction(UiHelper::getIcon("Data-Classification.svg", QColor("#EEEEEE")), "新建子分类");
    connect(newSubCatAct, &QAction::triggered, this, [this, nodeId]() {
        createAndEditCategory(nodeId);
    });

    QAction* presetTagAct = menu.addAction(UiHelper::getIcon("tag_filled", QColor("#EEEEEE")), "设置预设标签");
    connect(presetTagAct, &QAction::triggered, this, [this, nodeId]() {
        PresetTagsDialog dlg(nodeId, this, true);
        dlg.exec();
    });

    menu.addSeparator();

    QPersistentModelIndex persistentIdx(index);
    ContextMenuFactory::buildIconPickerMenu(&menu, curIconKey, curColorHex,
        [this, persistentIdx](const QString& iconKey) {
            if (!persistentIdx.isValid()) return;
            QStandardItem* item = m_model ? m_model->itemFromIndex(persistentIdx) : nullptr;
            if (!item) return;

            QString colorHex = item->data(Qt::UserRole + 3).toString();
            if (colorHex.isEmpty()) colorHex = "#888888";

            QIcon newIcon = UiHelper::getIcon(iconKey, QColor(colorHex), 18);
            item->setIcon(newIcon);
            item->setData(iconKey, Qt::UserRole + 2);

            int nodeId = item->data(Qt::UserRole + 1).toInt();
            LibraryDao::updateCategoryNode(nodeId, item->text(), iconKey, colorHex);
        },
        [this, persistentIdx](const QString& hexColor) {
            if (!persistentIdx.isValid()) return;
            QStandardItem* item = m_model ? m_model->itemFromIndex(persistentIdx) : nullptr;
            if (!item) return;

            QString finalColor = hexColor.isEmpty() ? "#888888" : hexColor.toUpper();
            QString iconKey = item->data(Qt::UserRole + 2).toString();
            if (iconKey.isEmpty()) iconKey = "folder_filled";

            QIcon newIcon = UiHelper::getIcon(iconKey, QColor(finalColor), 18);
            item->setIcon(newIcon);
            item->setData(finalColor, Qt::UserRole + 3);

            int nodeId = item->data(Qt::UserRole + 1).toInt();
            LibraryDao::updateCategoryNode(nodeId, item->text(), iconKey, finalColor);
        }
    );

    QAction* renameAct = menu.addAction(UiHelper::getIcon("rename-1.svg", QColor("#EEEEEE")), "重命名");
    connect(renameAct, &QAction::triggered, this, [this, index]() {
        if (m_treeView) m_treeView->edit(index);
    });

    QAction* removeAct = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE")), "删除分类");
    connect(removeAct, &QAction::triggered, this, [this, nodeId]() {
        LibraryService::instance().removeCategory(nodeId);
    });

    menu.addSeparator();

    auto* sortMenu = menu.addMenu(UiHelper::getIcon("list_ul", QColor("#AAAAAA")), "排列");
    UiHelper::applyMenuStyle(sortMenu);
    QAction* sortAsc = sortMenu->addAction("按名称 (A→Z)");
    connect(sortAsc, &QAction::triggered, this, [this]() { sortCategoriesByName(true); });
    QAction* sortDesc = sortMenu->addAction("按名称 (Z→A)");
    connect(sortDesc, &QAction::triggered, this, [this]() { sortCategoriesByName(false); });
    QAction* sortManual = sortMenu->addAction("手动");
    connect(sortManual, &QAction::triggered, this, [this]() { loadLibrary(); });

    menu.exec(m_treeView->viewport()->mapToGlobal(pos));
}

void LibraryPanel::onPathsDroppedToCategory(const QStringList& paths, const QModelIndex& target) {
    if (paths.isEmpty()) return;

    // 拖拽到空白处时默认为“未分类” (-2)，拖到具体节点则判断其 nodeId
    int nodeId = -2;
    if (target.isValid()) {
        nodeId = target.data(Qt::UserRole + 1).toInt();
    }

    // 拖到系统分类(-1)、快速访问(-10)、智能文件夹(-20) 或 “文件夹(N)”标题行(0) 时：静默返回，不做任何处理
    if (nodeId == -1 || nodeId == -10 || nodeId == -20 || nodeId == 0) {
        return;
    }

    // 只接受 nodeId > 0 或 -2
    if (nodeId > 0 || nodeId == -2) {
        bool ok = LibraryService::instance().addPathsToCategory(nodeId, paths);
        if (!ok) {
            ToolTipOverlay::instance()->showToast("归入失败", 1500, QColor("#e81123"));
            return;
        }

        if (nodeId == -2) {
            // 目标为 -2 时：不执行子项索引，直接显示成功提示
            ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目归入未分类").arg(paths.size()), 1500, QColor("#2ecc71"));
            return;
        }

        int appliedTagCount = PresetTagService::instance().applyPresetTagsForDroppedItems(nodeId, paths);

        if (appliedTagCount > 0) {
            ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目关联至当前分类，并自动应用了 %2 个预设标签").arg(paths.size()).arg(appliedTagCount), 2000, QColor("#2ecc71"));
        } else {
            ToolTipOverlay::instance()->showToast(QString("已关联 %1 个路径到当前分类").arg(paths.size()), 1500, QColor("#2ecc71"));
        }
    }
}

void LibraryPanel::loadLibrary() {
    if (!m_model) return;
    m_isLoading = true;
    m_model->clear();

    // 1. 注入 2 个固定系统分类 (全部数据, 未分类)
    auto addSystemItem = [this](const QString& name, const QString& iconKey, const QString& colorHex, int sysId) {
        int count = LibraryDao::getCategoryPaths(sysId).size();
        QIcon icon = UiHelper::getIcon(iconKey, QColor(colorHex), 18);
        QStandardItem* item = new QStandardItem(icon, name);
        item->setData("system", TypeRole);
        item->setData(sysId, IdRole);
        item->setData(sysId, Qt::UserRole + 1);
        item->setData(iconKey, Qt::UserRole + 2);
        item->setData(colorHex, Qt::UserRole + 3);
        item->setData(count, Qt::UserRole + 9);
        item->setEditable(false);
        m_model->appendRow(item);
    };

    addSystemItem("全部数据", "all_data", "#3498db", -1);
    addSystemItem("未分类", "uncategorized-2", "#95a5a6", -2);

    auto list = LibraryDao::getAllCategories();

    // 计算置顶项数量
    int pinnedCount = 0;
    for (const auto& rec : list) {
        if (rec.isPinned) pinnedCount++;
    }

    // 加载所有智能文件夹
    auto smartFolders = SmartFolderDao::getAllSmartFolders();

    // 2. 构建三大折叠分组 Header 节点 (-10 快速访问, -20 智能文件夹, 0 文件夹)
    QStandardItem* quickAccessHeader = new QStandardItem();
    quickAccessHeader->setData("section", TypeRole);
    quickAccessHeader->setData(-10, IdRole);
    quickAccessHeader->setData(-10, Qt::UserRole + 1);
    quickAccessHeader->setData(pinnedCount, Qt::UserRole + 9);
    quickAccessHeader->setFlags(Qt::ItemIsEnabled);
    m_model->appendRow(quickAccessHeader);

    QStandardItem* smartFolderHeader = new QStandardItem();
    smartFolderHeader->setData("section", TypeRole);
    smartFolderHeader->setData(-20, IdRole);
    smartFolderHeader->setData(-20, Qt::UserRole + 1);
    smartFolderHeader->setData(smartFolders.size(), Qt::UserRole + 9);
    smartFolderHeader->setFlags(Qt::ItemIsEnabled);
    m_model->appendRow(smartFolderHeader);

    for (const auto& sfRec : smartFolders) {
        int countVal = m_smartFolderCountCache.contains(sfRec.id) ? m_smartFolderCountCache.value(sfRec.id) : -999;
        QString sfIconKey = sfRec.iconKey;
        if (sfIconKey.isEmpty() || sfIconKey == "folder_filled" || sfIconKey == "folder") {
            sfIconKey = "smart-folder-1.svg";
        }
        QIcon sfIcon = UiHelper::getIcon(sfIconKey, QColor(sfRec.colorHex), 18);
        QStandardItem* sfItem = new QStandardItem(sfIcon, sfRec.name);
        sfItem->setData("smart_folder", TypeRole);
        sfItem->setData(sfRec.id, IdRole);
        sfItem->setData(sfRec.id, Qt::UserRole + 1);
        sfItem->setData(sfIconKey, Qt::UserRole + 2);
        sfItem->setData(sfRec.colorHex, Qt::UserRole + 3);
        sfItem->setData(countVal, Qt::UserRole + 9);
        sfItem->setEditable(false);
        smartFolderHeader->appendRow(sfItem);
    }

    int customCount = list.size();

    QStandardItem* sectionItem = new QStandardItem();
    sectionItem->setData("section", TypeRole);
    sectionItem->setData(0, IdRole);
    sectionItem->setData(0, Qt::UserRole + 1);
    sectionItem->setData(customCount, Qt::UserRole + 9);
    sectionItem->setFlags(Qt::ItemIsEnabled);
    m_model->appendRow(sectionItem);

    // 3. 加载用户自定义分类并挂载在 sectionItem 及其子节点下
    QMap<int, QStandardItem*> itemMap;
    for (const auto& rec : list) {
        int count = LibraryDao::getCategoryPaths(rec.id).size();
        QString catIconKey = rec.iconKey;
        if (catIconKey.isEmpty() || catIconKey == "folder_filled" || catIconKey == "folder") {
            catIconKey = "Data-Classification.svg";
        }
        QIcon icon = UiHelper::getIcon(catIconKey, QColor(rec.colorHex), 18);
        QStandardItem* item = new QStandardItem(icon, rec.name);
        item->setData("category", TypeRole);
        item->setData(rec.id, IdRole);
        item->setData(rec.id, Qt::UserRole + 1);
        item->setData(catIconKey, Qt::UserRole + 2);
        item->setData(rec.colorHex, Qt::UserRole + 3);
        item->setData(rec.isPinned, Qt::UserRole + 4);
        item->setData(count, Qt::UserRole + 9);

        itemMap.insert(rec.id, item);

        // 若被置顶，在快速访问标头下挂载镜像节点
        if (rec.isPinned) {
            QStandardItem* pinnedMirrorItem = new QStandardItem(icon, rec.name);
            pinnedMirrorItem->setData("category", TypeRole);
            pinnedMirrorItem->setData(rec.id, IdRole);
            pinnedMirrorItem->setData(rec.id, Qt::UserRole + 1);
            pinnedMirrorItem->setData(catIconKey, Qt::UserRole + 2);
            pinnedMirrorItem->setData(rec.colorHex, Qt::UserRole + 3);
            pinnedMirrorItem->setData(true, Qt::UserRole + 4);
            pinnedMirrorItem->setData(count, Qt::UserRole + 9);
            quickAccessHeader->appendRow(pinnedMirrorItem);
        }
    }

    for (const auto& rec : list) {
        if (!itemMap.contains(rec.id)) continue;
        QStandardItem* item = itemMap.value(rec.id);

        if (rec.parentId == -20) {
            smartFolderHeader->appendRow(item);
        } else if (rec.parentId > 0 && itemMap.contains(rec.parentId)) {
            itemMap.value(rec.parentId)->appendRow(item);
        } else {
            sectionItem->appendRow(item);
        }
    }

    // 重建树后先 expandAll()，再按保存的折叠状态设置标题行的展开/折叠
    if (m_treeView) {
        m_treeView->expandAll();
        bool qaCollapsed = AppConfig::instance().getValue("Library/QuickAccessSectionCollapsed", false).toBool();
        m_treeView->setExpanded(quickAccessHeader->index(), !qaCollapsed);

        bool sfCollapsed = AppConfig::instance().getValue("Library/SmartFolderSectionCollapsed", false).toBool();
        m_treeView->setExpanded(smartFolderHeader->index(), !sfCollapsed);

        bool sectionCollapsed = AppConfig::instance().getValue("Library/CustomSectionCollapsed", false).toBool();
        m_treeView->setExpanded(sectionItem->index(), !sectionCollapsed);
    }

    m_isLoading = false;

    // 触发智能文件夹异步后台计算
    if (!smartFolders.isEmpty()) {
        m_currentCountReqId++;
        updateSmartFolderCountsAsync(m_currentCountReqId, smartFolders);
    }

    if (m_pendingEditNodeId > 0 && m_treeView) {
        int targetNodeId = m_pendingEditNodeId;
        QTimer::singleShot(0, this, [this, targetNodeId]() {
            if (m_pendingEditNodeId != targetNodeId) return;
            m_pendingEditNodeId = 0;

            QStandardItem* targetItem = findItemByNodeId(m_model->invisibleRootItem(), targetNodeId);
            if (targetItem && m_treeView) {
                QModelIndex parentIdx = targetItem->parent() ? targetItem->parent()->index() : QModelIndex();
                if (parentIdx.isValid()) {
                    m_treeView->expand(parentIdx);
                }
                m_treeView->setCurrentIndex(targetItem->index());
                m_treeView->edit(targetItem->index());
            }
        });
    }
}

QStandardItem* LibraryPanel::findItemByNodeId(QStandardItem* parent, int nodeId) {
    if (!parent) return nullptr;
    for (int i = 0; i < parent->rowCount(); ++i) {
        QStandardItem* child = parent->child(i);
        if (!child) continue;
        if (child->data(Qt::UserRole + 1).toInt() == nodeId) {
            return child;
        }
        QStandardItem* found = findItemByNodeId(child, nodeId);
        if (found) return found;
    }
    return nullptr;
}

void LibraryPanel::invalidateSmartFolderCache(int smartFolderId) {
    if (smartFolderId == 0) {
        m_smartFolderCountCache.clear();
    } else {
        m_smartFolderCountCache.remove(smartFolderId);
    }
}

void LibraryPanel::updateSmartFolderCountsAsync(uint64_t reqId, const QList<SmartFolderRecord>& smartFolders) {
    QString dbPath = DatabaseManager::getGlobalDbPath();

    (void)QtConcurrent::run([this, reqId, smartFolders, dbPath]() {
        QMap<int, int> results;
        for (const auto& sfRec : smartFolders) {
            int c = SmartFolderDao::countMatchingItemsReadOnly(dbPath, sfRec);
            results.insert(sfRec.id, c);
        }

        QMetaObject::invokeMethod(this, [this, reqId, results]() {
            if (reqId != m_currentCountReqId || !m_model) return;

            QStandardItem* sfHeader = findItemByNodeId(m_model->invisibleRootItem(), -20);
            for (auto it = results.constBegin(); it != results.constEnd(); ++it) {
                int sfId = it.key();
                int newCount = it.value();

                m_smartFolderCountCache.insert(sfId, newCount);

                if (sfHeader) {
                    QStandardItem* sfItem = findItemByNodeId(sfHeader, sfId);
                    if (sfItem) {
                        int oldCount = sfItem->data(Qt::UserRole + 9).toInt();
                        if (oldCount != newCount) {
                            sfItem->setData(newCount, Qt::UserRole + 9);
                        }
                    }
                }
            }
        });
    });
}

void LibraryPanel::onPinCategoryTriggered(int nodeId, bool pin) {
    if (nodeId <= 0) return;
    LibraryDao::setCategoryPinned(nodeId, pin);
    loadLibrary();
}

void LibraryPanel::createAndEditCategory(int parentId) {
    int targetHeaderId = (parentId == -20) ? -20 : 0;
    QString targetConfigKey = (parentId == -20) ? "Library/SmartFolderSectionCollapsed" : "Library/CustomSectionCollapsed";

    if (m_treeView && m_model) {
        QStandardItem* targetHeaderItem = findItemByNodeId(m_model->invisibleRootItem(), targetHeaderId);
        if (targetHeaderItem) {
            m_treeView->setExpanded(targetHeaderItem->index(), true);
            AppConfig::instance().setValue(targetConfigKey, false);
        }
    }

    QString defaultName = (parentId == -20) ? "新建智能文件夹" : "新建分类";
    int newId = LibraryService::instance().createCategory(defaultName, parentId);
    if (newId > 0) {
        m_pendingEditNodeId = newId;
        loadLibrary();
    }
}

} // namespace QuarkMeta
