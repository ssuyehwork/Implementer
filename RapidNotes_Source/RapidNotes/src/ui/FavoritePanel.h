#pragma once

#include <QFrame>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include "DropTreeView.h"

namespace QuarkMeta {

class FavoriteItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit FavoriteItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void setFilterKeyword(const QString& kw) { m_filterKeyword = kw; }
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    void paintHighlightedText(QPainter* painter, const QRect& rect, const QString& text, const QString& keyword, const QFont& font, const QColor& normalColor, const QColor& matchColor) const;
    QString m_filterKeyword;
};

/**
 * @brief 独立收藏夹面板（主界面第二栏）
 * 垂直贯通独占，专门展示常用快捷文件/文件夹
 */
class FavoritePanel : public QFrame {
    Q_OBJECT

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    explicit FavoritePanel(QWidget* parent = nullptr);
    ~FavoritePanel() override = default;

    // 🚀【物理契约】：刚性锁定 230px 下限，杜绝被 QSplitter 挤压偷扣像素
    QSize minimumSizeHint() const override { return QSize(230, 100); }

    void setFocusHighlight(bool visible);
    bool containsPath(const QString& path) const;
    void removeFavoriteItem(const QString& path);
    void addFavoriteItem(const QString& path, int parentId = 0);
    void addVirtualCategory(const QString& name, int parentId = 0);
    void loadFavorites();
    void saveFavorites();
    void filterTreeNodes(const QString& keyword);

signals:
    void directorySelected(const QString& path, bool recursive = false);
    void requestLocateFile(const QString& path);
    void favoriteStateChanged(const QString& path, bool isFavorite);

private slots:
    void onFavoriteClicked(const QModelIndex& index);
    void onFavoriteContextMenu(const QPoint& pos);
    void onPathsDroppedToFavorite(const QStringList& paths, const QModelIndex& target);
    void onInternalMoveRequested(const QList<QPersistentModelIndex>& sources, const QModelIndex& target, InternalDropZone zone);
    void markOrderDirty();
    void flushOrder();

private:
    void initUi();
    void refreshItemIcons();
    void updateItemThumbnail(const QString& path, const QPixmap& pix);
    void createAndEditCategory(int parentId = 0);
    void sortItemsByName(bool ascending);

    QVBoxLayout* m_mainLayout = nullptr;
    DropTreeView* m_favoriteView = nullptr;
    QStandardItemModel* m_favoriteModel = nullptr;
    FavoriteItemDelegate* m_delegate = nullptr;
    bool m_isLoading = false;
    bool m_orderDirty = false;
    int m_pendingEditNodeId = 0;
};

} // namespace QuarkMeta
