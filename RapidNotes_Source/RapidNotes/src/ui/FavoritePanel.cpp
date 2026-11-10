#include "FavoritePanel.h"
#include "controllers/ContentKeyHandler.h"
#include <QKeyEvent>
#include "UiHelper.h"
#include "ShellIconManager.h"
#include "WindowsShellThumbnailProvider.h"
#include "../util/DiskMediaExtractor.h"
#include "ColorPicker.h"
#include "ToolTipOverlay.h"
#include "PresetTagsDialog.h"
#include "dialogs/FramelessConfirmDialog.h"
#include "../core/DatabaseManager.h"
#include "../core/CentralEventHub.h"
#include <QApplication>
#include <QClipboard>
#include "../meta/FavoriteDao.h"
#include "../meta/FavoriteService.h"
#include "../meta/MarkedFolderService.h"
#include "../meta/LibraryService.h"
#include "../meta/MetadataManager.h"
#include "../meta/DriveMetaDao.h"
#include "../core/ClipboardService.h"
#include "../core/NavigationService.h"
#include "TabBarWidget.h"
#include "ContentPanel.h"
#include "controllers/ContextMenuFactory.h"
#include "ThumbnailPipelineService.h"
#include <QThreadPool>
#include <QPainter>
#include <QPainterPath>
#include "../core/AppConfig.h"
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QWidgetAction>
#include <QGridLayout>
#include <QFileInfo>
#include <QDir>
#include <QHeaderView>
#include <QCoreApplication>
#include <QtConcurrent>
#include <QPointer>
#include <QTimer>

namespace RapidNotes {

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
    
    // 背景高亮
    if (opt.state & QStyle::State_Selected) {
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

    // 微卡片圆角绘制
    QVariant decoData = index.data(Qt::DecorationRole);
    bool isFolder = index.data(Qt::UserRole + 4).toBool();
    bool hasCustomThumb = index.data(Qt::UserRole + 5).toBool();

    if (!isFolder && hasCustomThumb && decoData.canConvert<QIcon>()) {
        QIcon icon = decoData.value<QIcon>();
        QPixmap pix = icon.pixmap(QSize(64, 64));

        if (!pix.isNull()) {
            QPainterPath clipPath;
            clipPath.addRoundedRect(iconRect, 3, 3);
            painter->save();
            painter->setClipPath(clipPath);

            QPixmap scaled = pix.scaled(iconRect.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            int x = iconRect.center().x() - scaled.width() / 2;
            int y = iconRect.center().y() - scaled.height() / 2;
            painter->drawPixmap(x, y, scaled);
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
    int rightIconWidth = isAutoRec ? 22 : 0;
    QRect adjustedTextRect = textRect.adjusted(0, 0, -rightIconWidth, 0);

    paintHighlightedText(painter, adjustedTextRect, text, m_filterKeyword, opt.font, normalTextColor, matchTextColor);

    // 绘制开启自动递归右侧 layers 图标与标记 Destination 图标
    bool isMarked = index.data(Qt::UserRole + 10).toBool();
    int rightOffset = 22;
    if (isAutoRec) {
        QRect layersRect(opt.rect.right() - rightOffset, opt.rect.top() + (opt.rect.height() - 16) / 2, 16, 16);
        QIcon layersIcon = UiHelper::getIcon("layers", QColor("#2ecc71"), 16);
        layersIcon.paint(painter, layersRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        rightOffset += 20;
    }

    if (isMarked) {
        QRect markIconRect(opt.rect.right() - rightOffset, opt.rect.top() + (opt.rect.height() - 16) / 2, 16, 16);
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
    int rightMargin = isAutoRec ? 26 : 8;

    QRect editorRect = option.rect;
    editorRect.setLeft(textLeft - 2);
    editorRect.setRight(option.rect.right() - rightMargin);
    editorRect.setTop(option.rect.top() + 2);
    editorRect.setHeight(option.rect.height() - 4);

    editor->setGeometry(editorRect);
}

FavoritePanel::FavoritePanel(QWidget* parent)
    : QFrame(parent) {
    setObjectName("FavoriteContainer");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(230);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    initUi();
    loadFavorites();
}

bool FavoritePanel::eventFilter(QObject* watched, QEvent* event) {
    if ((watched == m_favoriteView || (m_favoriteView && watched == m_favoriteView->viewport())) 
        && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEv = static_cast<QKeyEvent*>(event);
        if (keyEv->key() == Qt::Key_F2 && keyEv->modifiers() == Qt::NoModifier) {
            QModelIndex curIdx = m_favoriteView->currentIndex();
            if (curIdx.isValid()) {
                m_favoriteView->setFocus();
                m_favoriteView->edit(curIdx);
                keyEv->accept();
                return true;
            }
        }
        if (keyEv->modifiers() & Qt::ControlModifier) {
            if (keyEv->key() == Qt::Key_Home) {
                if (ContentKeyHandler::navigateViewFirstItem(m_favoriteView)) return true;
            } else if (keyEv->key() == Qt::Key_End) {
                if (ContentKeyHandler::navigateViewLastItem(m_favoriteView)) return true;
            }
        }
    }
    return QFrame::eventFilter(watched, event);
}

void FavoritePanel::setFocusHighlight(bool visible) {
    Q_UNUSED(visible);
}

void FavoritePanel::initUi() {
    m_favoriteView = new DropTreeView(this);
    m_favoriteView->setObjectName("FavoriteTreeView");
    m_favoriteView->setFocusPolicy(Qt::StrongFocus);
    m_favoriteView->setHeaderHidden(true);
    if (m_favoriteView->header()) {
        m_favoriteView->header()->setStretchLastSection(true);
        m_favoriteView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    }
    m_favoriteView->setIndentation(0);
    m_favoriteView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_favoriteView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_favoriteView->setDragEnabled(true);
    m_favoriteView->setAcceptDrops(true);
    m_favoriteView->setDropIndicatorShown(true);
    m_favoriteView->setDefaultDropAction(Qt::MoveAction);
    m_favoriteView->setDragDropMode(QAbstractItemView::DragDrop);
    m_favoriteView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_favoriteView->installEventFilter(this);
    if (m_favoriteView->viewport()) {
        m_favoriteView->viewport()->installEventFilter(this);
    }
    m_favoriteView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_favoriteView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_favoriteModel = new QStandardItemModel(this);
    m_favoriteView->setModel(m_favoriteModel);
    m_delegate = new FavoriteItemDelegate(this);
    m_favoriteView->setItemDelegate(m_delegate);

    m_mainLayout->addWidget(m_favoriteView, 1);

    connect(m_favoriteView, &QTreeView::clicked, this, &FavoritePanel::onFavoriteClicked);
    connect(m_favoriteView, &QWidget::customContextMenuRequested, this, &FavoritePanel::onFavoriteContextMenu);
}

void FavoritePanel::filterTreeNodes(const QString& keyword) {
    if (!m_favoriteModel || !m_favoriteView) return;

    if (m_delegate) {
        m_delegate->setFilterKeyword(keyword.trimmed());
    }

    std::function<bool(QStandardItem*)> filterRecursive = [&](QStandardItem* item) -> bool {
        if (!item) return false;

        bool matchesSelf = keyword.trimmed().isEmpty() || item->text().contains(keyword.trimmed(), Qt::CaseInsensitive);
        bool anyChildMatches = false;

        for (int i = 0; i < item->rowCount(); ++i) {
            QStandardItem* child = item->child(i);
            if (filterRecursive(child)) {
                anyChildMatches = true;
                m_favoriteView->setRowHidden(i, item->index(), false);
            } else {
                m_favoriteView->setRowHidden(i, item->index(), true);
            }
        }

        bool visible = matchesSelf || anyChildMatches;
        if (!keyword.trimmed().isEmpty() && visible && item->hasChildren()) {
            m_favoriteView->expand(item->index());
        }
        return visible;
    };

    for (int i = 0; i < m_favoriteModel->rowCount(); ++i) {
        QStandardItem* topItem = m_favoriteModel->item(i);
        if (!topItem) continue;
        if (filterRecursive(topItem)) {
            m_favoriteView->setRowHidden(i, QModelIndex(), false);
        } else {
            m_favoriteView->setRowHidden(i, QModelIndex(), true);
        }
    }

    if (m_favoriteView->viewport()) {
        m_favoriteView->viewport()->update();
    }
}

void FavoritePanel::onFavoriteClicked(const QModelIndex& index) {
    if (!index.isValid()) return;

    int noteId = index.data(Qt::UserRole + 1).toInt();
    QString title = index.data(Qt::UserRole + 2).toString();
    if (noteId <= 0) return;

    emit noteSelected(noteId, title);
}

void FavoritePanel::onFavoriteContextMenu(const QPoint& pos) {
    QModelIndex index = m_favoriteView->indexAt(pos);

    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    if (!index.isValid()) {
        auto* sortMenu = menu.addMenu(UiHelper::getIcon("list_ul", QColor("#AAAAAA")), "排列");
        UiHelper::applyMenuStyle(sortMenu);
        QAction* sortAsc = sortMenu->addAction("按名称 (A→Z)");
        connect(sortAsc, &QAction::triggered, this, [this]() { sortItemsByName(true); });
        QAction* sortDesc = sortMenu->addAction("按名称 (Z→A)");
        connect(sortDesc, &QAction::triggered, this, [this]() { sortItemsByName(false); });
        QAction* refreshAct = menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE")), "刷新");
        connect(refreshAct, &QAction::triggered, this, [this]() { loadFavorites(); });

        menu.exec(m_favoriteView->viewport()->mapToGlobal(pos));
        return;
    }

    int noteId = index.data(Qt::UserRole + 1).toInt();
    QString title = index.data(Qt::UserRole + 2).toString();
    QString content = index.data(Qt::UserRole + 5).toString();

    // 1. 复制内容
    QAction* copyAct = menu.addAction(UiHelper::getIcon("copy", QColor("#EEEEEE"), 18), "复制内容");
    connect(copyAct, &QAction::triggered, this, [content, title]() {
        QString textToCopy = content.isEmpty() ? title : content;
        QApplication::clipboard()->setText(textToCopy);
        ToolTipOverlay::instance()->showToast("已复制笔记内容", 1500, QColor("#2ecc71"));
    });

    // 2. 取消收藏
    QAction* unFavAct = menu.addAction(UiHelper::getIcon("Favorites.svg", QColor("#EEEEEE"), 18), "取消收藏");
    connect(unFavAct, &QAction::triggered, this, [this, noteId]() {
        DatabaseManager::instance().toggleNoteFavoriteBatch({noteId});
        AppEvent ev;
        ev.type = AppEventType::FavoritesUpdated;
        ev.paths = {QString("note://%1").arg(noteId)};
        CentralEventHub::instance().publishEvent(ev);
        ToolTipOverlay::instance()->showToast("已取消收藏", 1500, QColor("#e74c3c"));
        loadFavorites();
    });

    menu.addSeparator();

    // 3. 排列子菜单
    auto* sortMenu = menu.addMenu(UiHelper::getIcon("list_ul", QColor("#AAAAAA")), "排列");
    UiHelper::applyMenuStyle(sortMenu);
    QAction* sortAsc = sortMenu->addAction("按名称 (A→Z)");
    connect(sortAsc, &QAction::triggered, this, [this]() { sortItemsByName(true); });
    QAction* sortDesc = sortMenu->addAction("按名称 (Z→A)");
    connect(sortDesc, &QAction::triggered, this, [this]() { sortItemsByName(false); });

    menu.exec(m_favoriteView->viewport()->mapToGlobal(pos));
}

void FavoritePanel::loadFavorites() {
    if (!m_favoriteModel) return;
    m_isLoading = true;
    m_favoriteModel->clear();

    // 🚀【纯数据库真理源】：直接从 DatabaseManager 获取所有已收藏的有效笔记 (bookmark 对应 is_favorite = 1 且 is_deleted = 0)
    QList<QVariantMap> favNotes = DatabaseManager::instance().searchNotes("", "bookmark");

    for (const auto& note : favNotes) {
        int noteId = note.value("id").toInt();
        QString title = note.value("title").toString().trimmed();
        QString content = note.value("content").toString().trimmed();
        QString itemType = note.value("item_type").toString();
        QString colorHex = note.value("color").toString();

        if (title.isEmpty()) {
            title = content.left(20).replace("\n", " ");
            if (title.isEmpty()) title = "未命名笔记";
        }

        QColor itemColor = QColor(colorHex);
        if (!itemColor.isValid()) itemColor = QColor("#2ECC71");

        // 根据条目类型选择对应矢量图标
        QString iconKey = "text";
        if (itemType == "image") iconKey = "image";
        else if (itemType == "code") iconKey = "code";
        else if (itemType == "link") iconKey = "link";

        QIcon icon = UiHelper::getIcon(iconKey, itemColor, 18);

        auto* item = new QStandardItem(icon, title);
        item->setEditable(false);
        item->setData(noteId, Qt::UserRole + 1);       // 存储 noteId
        item->setData(title, Qt::UserRole + 2);        // 存储标题
        item->setData(itemType, Qt::UserRole + 3);     // 存储条目类型
        item->setData(colorHex, Qt::UserRole + 4);     // 存储主题色彩
        item->setData(content, Qt::UserRole + 5);      // 存储内容用于快速复制
        item->setToolTip(title + "\n" + content.left(100));

        m_favoriteModel->appendRow(item);
    }

    m_isLoading = false;
}

void FavoritePanel::sortItemsByName(bool ascending) {
    if (!m_favoriteModel) return;

    std::function<void(QStandardItem*)> sortChildren = [&](QStandardItem* parentItem) {
        int rowCount = parentItem ? parentItem->rowCount() : m_favoriteModel->rowCount();
        if (rowCount <= 1) return;

        QList<QStandardItem*> childItems;
        for (int i = rowCount - 1; i >= 0; --i) {
            QStandardItem* item = parentItem ? parentItem->takeRow(i).first() : m_favoriteModel->takeRow(i).first();
            if (item) childItems.prepend(item);
        }

        std::sort(childItems.begin(), childItems.end(), [ascending](QStandardItem* a, QStandardItem* b) {
            return ascending ? (a->text().localeAwareCompare(b->text()) < 0)
                              : (a->text().localeAwareCompare(b->text()) > 0);
        });

        for (QStandardItem* item : childItems) {
            if (parentItem) {
                parentItem->appendRow(item);
            } else {
                m_favoriteModel->appendRow(item);
            }
            sortChildren(item);
        }
    };

    sortChildren(nullptr);
}

} // namespace RapidNotes
