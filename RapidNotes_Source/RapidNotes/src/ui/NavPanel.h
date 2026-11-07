#pragma once

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QDir>
#include <QFrame>
#include <QTabBar>
#include <QStackedWidget>
#include "HistoryPanel.h"

namespace QuarkMeta {

/**
 * @brief 导航面板（面板一）
 * 使用 QTreeView + QFileSystemModel 实现文件夹树导航
 */
class NavPanel : public QFrame {
    Q_OBJECT

public:
    explicit NavPanel(QWidget* parent = nullptr);
    ~NavPanel() override = default;

    bool eventFilter(QObject* watched, QEvent* event) override;

    // 🚀【物理契约】：刚性锁定 230px 下限，杜绝被 QSplitter 挤压偷扣像素
    QSize minimumSizeHint() const override { return QSize(230, 100); }

    void deferredInit();
    void setFocusHighlight(bool visible);
    void setRootPath(const QString& path);
    void selectPath(const QString& path);

signals:
    void directorySelected(const QString& path);
    void requestLocateFile(const QString& path);
    void requestOpenTrash();
    void requestAddFavorite(const QString& path);
    void requestRemoveFavorite(const QString& path);

private slots:
    void onItemExpanded(const QModelIndex& index);
    void onTreeClicked(const QModelIndex& index);
    void onTreeContextMenu(const QPoint& pos);
    void updateTreeHeight();
    void updateRecentVisitedList();

private:
    void initUi();
    void fetchChildDirs(QStandardItem* parent);
    QStandardItem* findItemByData(QStandardItem* parent, const QString& targetData);

    QTreeView* m_treeView = nullptr;
    QStandardItemModel* m_model = nullptr;
    QVBoxLayout* m_mainLayout = nullptr;
    QStandardItem* m_recentRootItem = nullptr;
    QString m_currentPath;

    QTabBar* m_tabBar = nullptr;
    QStackedWidget* m_stackedWidget = nullptr;
    HistoryPanel* m_historyPanel = nullptr;
};

} // namespace QuarkMeta