#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSet>
#include <QPointer>
#include <QTimer>
#include "models/DiskItemModel.h"
#include "models/FilterProxyModel.h"
#include "DropListView.h"

namespace QuarkMeta {

class ContentPanel;
class SectionProxyModel;
class FolderSectionHeaderBar;

class ColumnViewPane : public QWidget {
    Q_OBJECT
public:
    explicit ColumnViewPane(const QString& path, ContentPanel* contentPanel = nullptr, QWidget* parent = nullptr);
    ~ColumnViewPane() override = default;

    QString currentPath() const { return m_path; }
    void loadDirectory();

    bool isActive() const { return m_isActive; }
    // 【架构红线】setActive / m_isActive 专属于列视图（Column View）中单列（ColumnViewPane）的焦点激活状态。
    // 顶部蓝色焦点提示线（#3498db, 1px）由 paintEvent 根据此状态绘制。
    // 严禁将此接口或蓝线绘制逻辑挪用至 ContentPanel（内容面板/窗格）或
    // ContentHeaderWidget（内容面板标题栏）。窗格活跃状态由 ContentPaneSplitManager::setActivePane
    // 通过 QSS property "activePane" 独立管理，两者完全正交，绝对不可混用。
    void setActive(bool active);

    void selectItemByPath(const QString& targetPath);
    void setPendingSelectPaths(const QSet<QString>& paths, bool edit = false);
    void clearSelection();
    void setFilterState(const FilterState& state);
    void setExpandedChildPath(const QString& path) { m_expandedChildPath = path; update(); }
    QString expandedChildPath() const { return m_expandedChildPath; }
    void applySort(int sortType, Qt::SortOrder sortOrder);

    DropListView* listView() const { return m_listView; }
    DropListView* folderListView() const { return m_listView; }
    FilterProxyModel* proxyModel() const { return m_proxyModel; }
    FilterProxyModel* folderProxyModel() const { return m_proxyModel; }
    FilterProxyModel* fileProxyModel() const { return m_proxyModel; }
    SectionProxyModel* sectionProxyModel() const { return m_sectionProxyModel; }
    DiskItemModel* model() const { return m_model; }
    FolderSectionHeaderBar* folderHeader() const { return nullptr; }

    void refreshVisibleThumbnails();

signals:
    void folderClicked(const QString& folderPath, int paneIndex);
    void fileClicked(const QString& filePath, int paneIndex);
    void folderExpandRequested(const QString& folderPath, int paneIndex);
    void folderSelected(const QString& folderPath, int paneIndex);
    void fileSelected(const QString& filePath, int paneIndex);
    void selectionChanged();
    void recordsLoaded(const std::vector<ItemRecord>& records);
    void blankSpaceDoubleClicked(int paneIndex);
    void contextMenuRequested(QAbstractItemView* view, const QPoint& pos);
    void pathsDroppedSignal(const QStringList& paths, const QModelIndex& targetIndex, const QString& targetDir, Qt::DropAction action);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void tryPendingSelection();

private:
    QString m_path;
    QString m_pendingSelectPath;
    QSet<QString> m_pendingSelectPaths;
    bool m_isPendingEdit = false;
    ContentPanel* m_contentPanel = nullptr;
    DiskItemModel* m_model = nullptr;
    FilterProxyModel* m_proxyModel = nullptr;
    SectionProxyModel* m_sectionProxyModel = nullptr;
    bool m_isActive = false;
    QScrollArea* m_paneScrollArea = nullptr;
    DropListView* m_folderListView = nullptr;
    DropListView* m_listView = nullptr;
    QTimer* m_visibleTimer = nullptr;
    QString m_expandedChildPath;
};

} // namespace QuarkMeta
