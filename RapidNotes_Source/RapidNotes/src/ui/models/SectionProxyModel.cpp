#include "SectionProxyModel.h"
#include "../../core/ModelContract.h"
#include <algorithm>

namespace QuarkMeta {

SectionProxyModel::SectionProxyModel(QObject* parent)
    : QAbstractProxyModel(parent) {
}

void SectionProxyModel::setSourceModel(QAbstractItemModel* newSourceModel) {
    if (sourceModel() == newSourceModel) return;

    if (sourceModel()) {
        disconnect(sourceModel(), &QAbstractItemModel::dataChanged, this, &SectionProxyModel::onSourceDataChanged);
        disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &SectionProxyModel::onSourceRowsInserted);
        disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &SectionProxyModel::onSourceRowsRemoved);
        disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &SectionProxyModel::onSourceModelReset);
        disconnect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged, this, &SectionProxyModel::onSourceLayoutAboutToBeChanged);
        disconnect(sourceModel(), &QAbstractItemModel::layoutChanged, this, &SectionProxyModel::onSourceLayoutChanged);
    }

    QAbstractProxyModel::setSourceModel(newSourceModel);

    if (sourceModel()) {
        connect(sourceModel(), &QAbstractItemModel::dataChanged, this, &SectionProxyModel::onSourceDataChanged);
        connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &SectionProxyModel::onSourceRowsInserted);
        connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &SectionProxyModel::onSourceRowsRemoved);
        connect(sourceModel(), &QAbstractItemModel::modelReset, this, &SectionProxyModel::onSourceModelReset);
        connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged, this, &SectionProxyModel::onSourceLayoutAboutToBeChanged);
        connect(sourceModel(), &QAbstractItemModel::layoutChanged, this, &SectionProxyModel::onSourceLayoutChanged);
    }

    rebuildMapping();
}

void SectionProxyModel::rebuildReverseIndex() {
    if (!sourceModel()) {
        m_sourceToProxyMap.clear();
        m_fileSectionStartProxyRow = -1;
        return;
    }

    int total = sourceModel()->rowCount();
    m_sourceToProxyMap.fill(-1, total);
    m_fileSectionStartProxyRow = -1;

    for (int pRow = 0; pRow < m_mapping.size(); ++pRow) {
        const auto& entry = m_mapping.at(pRow);
        if (entry.type == RowType::FileItem && m_fileSectionStartProxyRow == -1) {
            m_fileSectionStartProxyRow = pRow;
        }
        if (entry.sourceRow >= 0 && entry.sourceRow < total) {
            m_sourceToProxyMap[entry.sourceRow] = pRow;
        }
    }
}

void SectionProxyModel::rebuildMapping() {
    beginResetModel();
    m_mapping.clear();
    m_folderSourceRows.clear();
    m_fileSourceRows.clear();

    if (sourceModel()) {
        int total = sourceModel()->rowCount();
        for (int i = 0; i < total; ++i) {
            QModelIndex srcIdx = sourceModel()->index(i, 0);
            QString typeStr = srcIdx.data(TypeRole).toString();
            if (typeStr == "folder") {
                m_folderSourceRows.append(i);
            } else {
                m_fileSourceRows.append(i);
            }
        }
    }

    m_folderCount = m_folderSourceRows.size();
    m_fileCount = m_fileSourceRows.size();

    if (!sourceModel() || (m_folderCount == 0 && m_fileCount == 0)) {
        rebuildReverseIndex();
        endResetModel();
        return;
    }

    // 1. Folder Header
    if (m_folderCount > 0) {
        m_mapping.append({RowType::FolderHeader, -1});
        if (!m_folderCollapsed) {
            for (int srcRow : m_folderSourceRows) {
                m_mapping.append({RowType::FolderItem, srcRow});
            }
        }
    }

    // 2. File Header & File Items
    if (m_fileCount > 0) {
        if (m_folderCount > 0) {
            m_mapping.append({RowType::FileHeader, -1});
        }
        for (int srcRow : m_fileSourceRows) {
            m_mapping.append({RowType::FileItem, srcRow});
        }
    }

    rebuildReverseIndex();
    endResetModel();
}

QModelIndex SectionProxyModel::mapToSource(const QModelIndex& proxyIndex) const {
    if (!proxyIndex.isValid() || proxyIndex.row() < 0 || proxyIndex.row() >= m_mapping.size()) {
        return QModelIndex();
    }

    const auto& entry = m_mapping.at(proxyIndex.row());
    if (entry.sourceRow < 0 || !sourceModel()) {
        return QModelIndex();
    }

    return sourceModel()->index(entry.sourceRow, proxyIndex.column());
}

QModelIndex SectionProxyModel::mapFromSource(const QModelIndex& sourceIndex) const {
    if (!sourceIndex.isValid() || !sourceModel()) {
        return QModelIndex();
    }

    int srcRow = sourceIndex.row();
    if (srcRow >= 0 && srcRow < m_sourceToProxyMap.size()) {
        int pRow = m_sourceToProxyMap.at(srcRow);
        if (pRow >= 0 && pRow < m_mapping.size()) {
            return createIndex(pRow, sourceIndex.column());
        }
    }

    return QModelIndex();
}

QModelIndex SectionProxyModel::index(int row, int column, const QModelIndex& parent) const {
    if (parent.isValid() || row < 0 || row >= m_mapping.size() || column < 0 || column >= columnCount()) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex SectionProxyModel::parent(const QModelIndex& child) const {
    Q_UNUSED(child);
    return QModelIndex();
}

int SectionProxyModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_mapping.size();
}

int SectionProxyModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return sourceModel() ? sourceModel()->columnCount() : 1;
}

QVariant SectionProxyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (sourceModel()) {
        return sourceModel()->headerData(section, orientation, role);
    }
    return QAbstractProxyModel::headerData(section, orientation, role);
}

QVariant SectionProxyModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_mapping.size()) {
        return QVariant();
    }

    const auto& entry = m_mapping.at(index.row());

    if (role == SectionKindRole) {
        if (entry.type == RowType::FolderHeader) return 1;
        if (entry.type == RowType::FileHeader) return 2;
        return 0;
    }

    if (role == SectionHeaderRole) {
        return (entry.type == RowType::FolderHeader || entry.type == RowType::FileHeader);
    }

    if (role == SectionHeaderTextRole) {
        if (entry.type == RowType::FolderHeader) {
            return QString("文件夹 (%1)").arg(m_folderCount);
        }
        if (entry.type == RowType::FileHeader) {
            return QString("文件 (%1)").arg(m_fileCount);
        }
        return QVariant();
    }

    if (role == SectionCollapsedRole) {
        if (entry.type == RowType::FolderHeader) {
            return m_folderCollapsed;
        }
        return false;
    }

    if (role == SectionRowRole) {
        if (entry.type == RowType::FolderItem) {
            return index.row() - 1;
        }
        if (entry.type == RowType::FileItem) {
            if (m_fileSectionStartProxyRow >= 0) {
                return index.row() - m_fileSectionStartProxyRow;
            }
        }
        return -1;
    }

    if (entry.sourceRow < 0) {
        return QVariant();
    }

    QModelIndex srcIdx = mapToSource(index);
    return srcIdx.isValid() ? srcIdx.data(role) : QVariant();
}

bool SectionProxyModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_mapping.size()) {
        return false;
    }

    const auto& entry = m_mapping.at(index.row());
    if (entry.sourceRow < 0) {
        return false;
    }

    QModelIndex srcIdx = mapToSource(index);
    return srcIdx.isValid() ? sourceModel()->setData(srcIdx, value, role) : false;
}

Qt::ItemFlags SectionProxyModel::flags(const QModelIndex& index) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_mapping.size()) {
        return Qt::NoItemFlags;
    }

    const auto& entry = m_mapping.at(index.row());
    if (entry.type == RowType::FolderHeader || entry.type == RowType::FileHeader) {
        return Qt::ItemIsEnabled;
    }

    QModelIndex srcIdx = mapToSource(index);
    return srcIdx.isValid() ? sourceModel()->flags(srcIdx) : Qt::NoItemFlags;
}

void SectionProxyModel::sort(int column, Qt::SortOrder order) {
    if (sourceModel()) {
        sourceModel()->sort(column, order);
    }
}

void SectionProxyModel::setFolderCollapsed(bool collapsed) {
    if (m_folderCollapsed == collapsed || m_folderCount == 0) return;

    if (collapsed) {
        beginRemoveRows(QModelIndex(), 1, m_folderCount);
        m_folderCollapsed = true;
        m_mapping.clear();
        m_mapping.append({RowType::FolderHeader, -1});
        if (m_fileCount > 0) {
            if (m_folderCount > 0) m_mapping.append({RowType::FileHeader, -1});
            for (int srcRow : m_fileSourceRows) {
                m_mapping.append({RowType::FileItem, srcRow});
            }
        }
        rebuildReverseIndex();
        endRemoveRows();
    } else {
        beginInsertRows(QModelIndex(), 1, m_folderCount);
        m_folderCollapsed = false;
        m_mapping.clear();
        m_mapping.append({RowType::FolderHeader, -1});
        for (int srcRow : m_folderSourceRows) {
            m_mapping.append({RowType::FolderItem, srcRow});
        }
        if (m_fileCount > 0) {
            if (m_folderCount > 0) m_mapping.append({RowType::FileHeader, -1});
            for (int srcRow : m_fileSourceRows) {
                m_mapping.append({RowType::FileItem, srcRow});
            }
        }
        rebuildReverseIndex();
        endInsertRows();
    }

    QModelIndex headerIdx = index(0, 0);
    emit dataChanged(headerIdx, headerIdx, {SectionCollapsedRole, SectionHeaderTextRole});
}

void SectionProxyModel::onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
    if (!topLeft.isValid() || !bottomRight.isValid()) return;

    int minProxyRow = -1;
    int maxProxyRow = -1;

    for (int r = topLeft.row(); r <= bottomRight.row(); ++r) {
        if (r >= 0 && r < m_sourceToProxyMap.size()) {
            int pRow = m_sourceToProxyMap.at(r);
            if (pRow >= 0) {
                if (minProxyRow == -1 || pRow < minProxyRow) minProxyRow = pRow;
                if (maxProxyRow == -1 || pRow > maxProxyRow) maxProxyRow = pRow;
            }
        }
    }

    if (minProxyRow != -1 && maxProxyRow != -1) {
        QModelIndex pTopLeft = index(minProxyRow, topLeft.column());
        QModelIndex pBottomRight = index(maxProxyRow, bottomRight.column());
        emit dataChanged(pTopLeft, pBottomRight, roles);
    }
}

void SectionProxyModel::syncHeaders() {
    bool wantFolderHeader = (m_folderCount > 0);
    bool wantFileHeader = (m_folderCount > 0 && m_fileCount > 0);

    bool hasFolderHeader = (!m_mapping.isEmpty() && m_mapping.first().type == RowType::FolderHeader);
    int fileHeaderIndex = -1;
    for (int i = 0; i < m_mapping.size(); ++i) {
        if (m_mapping[i].type == RowType::FileHeader) {
            fileHeaderIndex = i;
            break;
        }
    }
    bool hasFileHeader = (fileHeaderIndex != -1);

    // 1. Sync Folder Header
    if (wantFolderHeader && !hasFolderHeader) {
        beginInsertRows(QModelIndex(), 0, 0);
        m_mapping.prepend({RowType::FolderHeader, -1});
        rebuildReverseIndex();
        endInsertRows();
    } else if (!wantFolderHeader && hasFolderHeader) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_mapping.removeAt(0);
        rebuildReverseIndex();
        endRemoveRows();
    }

    // Recalculate fileHeaderIndex after FolderHeader change
    fileHeaderIndex = -1;
    for (int i = 0; i < m_mapping.size(); ++i) {
        if (m_mapping[i].type == RowType::FileHeader) {
            fileHeaderIndex = i;
            break;
        }
    }
    hasFileHeader = (fileHeaderIndex != -1);

    // 2. Sync File Header
    if (wantFileHeader && !hasFileHeader) {
        int insertPos = 0;
        if (!m_folderCollapsed && m_folderCount > 0) {
            insertPos = 1 + m_folderCount;
        } else if (m_folderCollapsed && m_folderCount > 0) {
            insertPos = 1;
        }
        beginInsertRows(QModelIndex(), insertPos, insertPos);
        m_mapping.insert(insertPos, {RowType::FileHeader, -1});
        rebuildReverseIndex();
        endInsertRows();
    } else if (!wantFileHeader && hasFileHeader) {
        beginRemoveRows(QModelIndex(), fileHeaderIndex, fileHeaderIndex);
        m_mapping.removeAt(fileHeaderIndex);
        rebuildReverseIndex();
        endRemoveRows();
    }

    // 3. Notify header text changes if headers exist
    for (int i = 0; i < m_mapping.size(); ++i) {
        if (m_mapping[i].type == RowType::FolderHeader || m_mapping[i].type == RowType::FileHeader) {
            QModelIndex hIdx = index(i, 0);
            emit dataChanged(hIdx, hIdx, {SectionHeaderTextRole});
        }
    }
}

void SectionProxyModel::onSourceRowsInserted(const QModelIndex& parent, int start, int end) {
    Q_UNUSED(parent);
    int count = end - start + 1;
    if (count > 500 || !sourceModel()) {
        rebuildMapping();
        return;
    }

    // 1. Update source row indices >= start for existing entries
    for (int& folderSrc : m_folderSourceRows) {
        if (folderSrc >= start) folderSrc += count;
    }
    for (int& fileSrc : m_fileSourceRows) {
        if (fileSrc >= start) fileSrc += count;
    }
    for (auto& entry : m_mapping) {
        if (entry.sourceRow >= start) {
            entry.sourceRow += count;
        }
    }

    // 2. Insert new entries
    for (int r = start; r <= end; ++r) {
        QModelIndex srcIdx = sourceModel()->index(r, 0);
        QString typeStr = srcIdx.data(TypeRole).toString();
        bool isFolder = (typeStr == "folder");

        if (isFolder) {
            int insertPosInFolders = 0;
            while (insertPosInFolders < m_folderSourceRows.size() && m_folderSourceRows[insertPosInFolders] < r) {
                insertPosInFolders++;
            }
            m_folderSourceRows.insert(insertPosInFolders, r);
            m_folderCount++;

            if (!m_folderCollapsed) {
                int targetProxyRow = (m_mapping.isEmpty() || m_mapping.first().type != RowType::FolderHeader) ? 0 : 1 + insertPosInFolders;
                beginInsertRows(QModelIndex(), targetProxyRow, targetProxyRow);
                m_mapping.insert(targetProxyRow, {RowType::FolderItem, r});
                rebuildReverseIndex();
                endInsertRows();
            }
        } else {
            int insertPosInFiles = 0;
            while (insertPosInFiles < m_fileSourceRows.size() && m_fileSourceRows[insertPosInFiles] < r) {
                insertPosInFiles++;
            }
            m_fileSourceRows.insert(insertPosInFiles, r);
            m_fileCount++;

            int fileHeaderProxyRow = -1;
            for (int p = 0; p < m_mapping.size(); ++p) {
                if (m_mapping[p].type == RowType::FileHeader) {
                    fileHeaderProxyRow = p;
                    break;
                }
            }

            int targetProxyRow = (fileHeaderProxyRow != -1) ? fileHeaderProxyRow + 1 + insertPosInFiles : m_mapping.size();
            beginInsertRows(QModelIndex(), targetProxyRow, targetProxyRow);
            m_mapping.insert(targetProxyRow, {RowType::FileItem, r});
            rebuildReverseIndex();
            endInsertRows();
        }
    }

    rebuildReverseIndex();
    syncHeaders();
}

void SectionProxyModel::onSourceRowsRemoved(const QModelIndex& parent, int start, int end) {
    Q_UNUSED(parent);
    int count = end - start + 1;
    if (count > 500 || !sourceModel()) {
        rebuildMapping();
        return;
    }

    // 1. Identify proxy rows to remove (from bottom to top)
    QVector<int> proxyRowsToRemove;
    for (int pRow = m_mapping.size() - 1; pRow >= 0; --pRow) {
        int sRow = m_mapping[pRow].sourceRow;
        if (sRow >= start && sRow <= end) {
            proxyRowsToRemove.append(pRow);
        }
    }

    // Group contiguous proxy row removals and perform beginRemoveRows
    for (int pRow : proxyRowsToRemove) {
        beginRemoveRows(QModelIndex(), pRow, pRow);
        m_mapping.removeAt(pRow);
        rebuildReverseIndex();
        endRemoveRows();
    }

    // Update folder & file source row lists
    for (int r = start; r <= end; ++r) {
        m_folderSourceRows.removeOne(r);
        m_fileSourceRows.removeOne(r);
    }

    // Adjust remaining source row numbers > end
    for (int& folderSrc : m_folderSourceRows) {
        if (folderSrc > end) folderSrc -= count;
    }
    for (int& fileSrc : m_fileSourceRows) {
        if (fileSrc > end) fileSrc -= count;
    }
    for (auto& entry : m_mapping) {
        if (entry.sourceRow > end) {
            entry.sourceRow -= count;
        }
    }

    m_folderCount = m_folderSourceRows.size();
    m_fileCount = m_fileSourceRows.size();

    rebuildReverseIndex();
    syncHeaders();
}

void SectionProxyModel::onSourceModelReset() {
    rebuildMapping();
}

void SectionProxyModel::onSourceLayoutAboutToBeChanged() {
    emit layoutAboutToBeChanged();
    m_layoutChangeProxyIndexes.clear();
    m_layoutChangeSourceIndexes.clear();

    if (!sourceModel()) return;

    int cols = sourceModel()->columnCount();
    for (int r = 0; r < m_mapping.size(); ++r) {
        if (m_mapping[r].sourceRow >= 0) {
            for (int c = 0; c < cols; ++c) {
                QModelIndex pIdx = createIndex(r, c);
                QModelIndex sIdx = mapToSource(pIdx);
                if (sIdx.isValid()) {
                    m_layoutChangeProxyIndexes.append(pIdx);
                    m_layoutChangeSourceIndexes.append(QPersistentModelIndex(sIdx));
                }
            }
        }
    }
}

void SectionProxyModel::onSourceLayoutChanged() {
    m_mapping.clear();
    m_folderSourceRows.clear();
    m_fileSourceRows.clear();

    if (sourceModel()) {
        int total = sourceModel()->rowCount();
        for (int i = 0; i < total; ++i) {
            QModelIndex srcIdx = sourceModel()->index(i, 0);
            QString typeStr = srcIdx.data(TypeRole).toString();
            if (typeStr == "folder") {
                m_folderSourceRows.append(i);
            } else {
                m_fileSourceRows.append(i);
            }
        }
    }

    m_folderCount = m_folderSourceRows.size();
    m_fileCount = m_fileSourceRows.size();

    if (m_folderCount > 0) {
        m_mapping.append({RowType::FolderHeader, -1});
        if (!m_folderCollapsed) {
            for (int srcRow : m_folderSourceRows) {
                m_mapping.append({RowType::FolderItem, srcRow});
            }
        }
    }

    if (m_fileCount > 0) {
        if (m_folderCount > 0) {
            m_mapping.append({RowType::FileHeader, -1});
        }
        for (int srcRow : m_fileSourceRows) {
            m_mapping.append({RowType::FileItem, srcRow});
        }
    }

    rebuildReverseIndex();

    if (!m_layoutChangeProxyIndexes.isEmpty()) {
        QModelIndexList newProxyIndexes;
        for (const QPersistentModelIndex& sIdx : m_layoutChangeSourceIndexes) {
            if (sIdx.isValid()) {
                newProxyIndexes.append(mapFromSource(sIdx));
            } else {
                newProxyIndexes.append(QModelIndex());
            }
        }
        changePersistentIndexList(m_layoutChangeProxyIndexes, newProxyIndexes);
    }
    m_layoutChangeProxyIndexes.clear();
    m_layoutChangeSourceIndexes.clear();
    emit layoutChanged();
}

} // namespace QuarkMeta
