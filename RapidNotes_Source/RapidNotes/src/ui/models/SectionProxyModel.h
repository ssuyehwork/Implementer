#pragma once

#include <QAbstractProxyModel>
#include <QVector>
#include <QString>

namespace QuarkMeta {

class SectionProxyModel : public QAbstractProxyModel {
    Q_OBJECT

public:
    enum class RowType {
        FolderHeader,
        FolderItem,
        FileHeader,
        FileItem
    };

    struct MappingEntry {
        RowType type;
        int sourceRow; // -1 for headers, >= 0 for items
    };

    explicit SectionProxyModel(QObject* parent = nullptr);
    ~SectionProxyModel() override = default;

    void setSourceModel(QAbstractItemModel* sourceModel) override;

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    // Folder collapse control
    void setFolderCollapsed(bool collapsed);
    bool isFolderCollapsed() const { return m_folderCollapsed; }

    // Statistics
    int folderCount() const { return m_folderCount; }
    int fileCount() const { return m_fileCount; }

private slots:
    void onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    void onSourceRowsInserted(const QModelIndex& parent, int start, int end);
    void onSourceRowsRemoved(const QModelIndex& parent, int start, int end);
    void onSourceModelReset();
    void onSourceLayoutAboutToBeChanged();
    void onSourceLayoutChanged();

private:
    void rebuildMapping();
    void rebuildReverseIndex();
    void syncHeaders();

    QVector<MappingEntry> m_mapping;
    QVector<int> m_sourceToProxyMap;
    QVector<int> m_folderSourceRows;
    QVector<int> m_fileSourceRows;
    bool m_folderCollapsed = false;
    int m_folderCount = 0;
    int m_fileCount = 0;
    int m_fileSectionStartProxyRow = -1;
    QModelIndexList m_layoutChangeProxyIndexes;
    QList<QPersistentModelIndex> m_layoutChangeSourceIndexes;
};

} // namespace QuarkMeta
