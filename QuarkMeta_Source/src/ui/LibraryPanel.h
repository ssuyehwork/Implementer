#pragma once

#include <QFrame>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include "DropTreeView.h"
#include "LibraryNodeKind.h"
#include "LibraryItemDelegate.h"
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
            LibraryNodeKind kind = static_cast<LibraryNodeKind>(index.data(NodeKindRole).toInt());
            if (isSectionKind(kind)) {
                return;
            }
        }
        DropTreeView::drawBranches(painter, rect, index);
    }
};

class LibraryDragController;
class SmartFolderCountService;

/**
 * @brief 库面板 (LibraryPanel)
 * 作为组装与信号协调的纯瘦容器视图，外观、交互与对外公共契约 100% 保持不变。
 */
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
    void refreshSmartFolderCounts();

    // 统一的新建智能文件夹方法（右键菜单与"+"按钮共用）
    void createNewSmartFolder();

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
    void toggleSection();
    void toggleSectionByNodeKind(LibraryNodeKind kind);

private:
    void initUi();
    void createAndEditCategory(int parentId = 0);

    QVBoxLayout* m_mainLayout = nullptr;
    LibraryTreeView* m_treeView = nullptr;
    QStandardItemModel* m_model = nullptr;
    LibraryItemDelegate* m_delegate = nullptr;
    LibraryDragController* m_dragController = nullptr;
    SmartFolderCountService* m_countService = nullptr;

    bool m_isLoading = false;
    bool m_isUpdatingCounts = false;
    int m_pendingEditNodeId = 0;
};

} // namespace QuarkMeta
