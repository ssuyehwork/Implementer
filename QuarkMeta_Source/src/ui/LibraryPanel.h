#pragma once

#include <QFrame>
#include <QTreeView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include "DropTreeView.h"
#include "../meta/SmartFolderDao.h"

namespace QuarkMeta {

class LibraryTreeView : public DropTreeView {
    Q_OBJECT
public:
    explicit LibraryTreeView(QWidget* parent = nullptr) : DropTreeView(parent) {}
    ~LibraryTreeView() override = default;

protected:
    void drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const override {
        if (index.isValid()) {
            int nodeId = index.data(Qt::UserRole + 1).toInt();
            if (nodeId == -10 || nodeId == -20 || nodeId == 0) {
                return;
            }
        }
        DropTreeView::drawBranches(painter, rect, index);
    }
};

class LibraryItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit LibraryItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

    void setFilterKeyword(const QString& keyword) { m_filterKeyword = keyword; }

    static QRect addBtnRect(const QRect& optionRect);

signals:
    void requestAddCategory(int nodeId);
    void requestToggleSection(int nodeId);

private:
    void paintHighlightedText(QPainter* painter, const QRect& rect, const QString& text, const QString& keyword, const QFont& font, const QColor& normalColor, const QColor& matchColor) const;

    QString m_filterKeyword;
};

class LibraryPanel : public QFrame {
    Q_OBJECT

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    explicit LibraryPanel(QWidget* parent = nullptr);
    ~LibraryPanel() override = default;

    void loadLibrary();
    void saveLibraryOrder();
    void sortCategoriesByName(bool ascending);
    void filterTreeNodes(const QString& keyword);

    LibraryItemDelegate* delegate() const { return m_delegate; }
    LibraryTreeView* treeView() const { return m_treeView; }

    void invalidateSmartFolderCache(int smartFolderId = 0);

signals:
    void categoryPathsSelected(const QStringList& paths, const QString& name = QString(), int categoryId = 0);
    void smartFolderSelected(const SmartFolderRecord& record);
    void requestLocateFile(const QString& path);
    void smartFolderPreviewRequested(const SmartFolderRecord& record);

private slots:
    void onCategoryClicked(const QModelIndex& index);
    void onCategoryContextMenu(const QPoint& pos);
    void onPinCategoryTriggered(int nodeId, bool pin);
    void onPathsDroppedToCategory(const QStringList& paths, const QModelIndex& target);
    void onInternalMoveRequested(const QList<QPersistentModelIndex>& sources, const QModelIndex& target, InternalDropZone zone);
    void markOrderDirty();
    void flushOrder();
    void toggleSection();

private:
    void initUi();
    void createAndEditCategory(int parentId = 0);
    QStandardItem* findItemByNodeId(QStandardItem* parent, int nodeId);
    void toggleSectionByNodeId(int nodeId);

    void updateSmartFolderCountsAsync(uint64_t reqId, const QList<SmartFolderRecord>& smartFolders);

    QVBoxLayout* m_mainLayout = nullptr;
    LibraryTreeView* m_treeView = nullptr;
    QStandardItemModel* m_model = nullptr;
    LibraryItemDelegate* m_delegate = nullptr;
    bool m_isLoading = false;
    bool m_orderDirty = false;
    int m_pendingEditNodeId = 0;

    // 智能文件夹异步匹配数量内存缓存
    QMap<int, int> m_smartFolderCountCache;
    uint64_t m_currentCountReqId = 0;
};

} // namespace QuarkMeta
