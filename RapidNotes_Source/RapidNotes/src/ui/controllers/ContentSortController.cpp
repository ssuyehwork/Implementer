#include "ContentSortController.h"
#include "../models/FilterProxyModel.h"
#include "../../core/AppConfig.h"
#include "../../meta/MetadataManager.h"

namespace QuarkMeta {

ContentSortController::ContentSortController(QObject* parent)
    : QObject(parent) {
    loadFromConfig();
}

void ContentSortController::loadFromConfig() {
    int savedType = AppConfig::instance().getValue("ContentPanel/RightClickSortType", static_cast<int>(SortType::SortByName)).toInt();
    int savedOrder = AppConfig::instance().getValue("ContentPanel/RightClickSortOrder", static_cast<int>(Qt::AscendingOrder)).toInt();

    m_sortType = static_cast<SortType>(savedType);
    m_sortOrder = static_cast<Qt::SortOrder>(savedOrder);
}

FileListColumn ContentSortController::columnForSortType(SortType type) {
    switch (type) {
        case SortType::SortByName: return FileListColumn::Name;
        case SortType::SortByRating: return FileListColumn::Rating;
        case SortType::SortByDimension: return FileListColumn::Dimension;
        case SortType::SortByExtension: return FileListColumn::Type;
        case SortType::SortBySize: return FileListColumn::Size;
        case SortType::SortByModifyDate: return FileListColumn::ModifiedDate;
        case SortType::SortByCreateDate: return FileListColumn::CreatedDate;
        case SortType::SortByAddedDate: default: break;
    }
    return static_cast<FileListColumn>(-1);
}

SortType ContentSortController::sortTypeForColumn(FileListColumn col) {
    switch (col) {
        case FileListColumn::Name: return SortType::SortByName;
        case FileListColumn::Rating: return SortType::SortByRating;
        case FileListColumn::Dimension: return SortType::SortByDimension;
        case FileListColumn::Type: return SortType::SortByExtension;
        case FileListColumn::Size: return SortType::SortBySize;
        case FileListColumn::ModifiedDate: return SortType::SortByModifyDate;
        case FileListColumn::CreatedDate: return SortType::SortByCreateDate;
        case FileListColumn::Status: case FileListColumn::Count: default: break;
    }
    return static_cast<SortType>(-1);
}

void ContentSortController::saveToConfig() {
    AppConfig::instance().setValue("ContentPanel/RightClickSortType", static_cast<int>(m_sortType));
    AppConfig::instance().setValue("ContentPanel/RightClickSortOrder", static_cast<int>(m_sortOrder));
    AppConfig::instance().sync();

    if (!m_currentFolderPath.isEmpty()) {
        FolderMeta meta = MetadataManager::instance().getFolderMeta(m_currentFolderPath);

        switch (m_sortType) {
            case SortType::SortByName: meta.sortBy = L"name"; break;
            case SortType::SortByCreateDate: meta.sortBy = L"cdate"; break;
            case SortType::SortByModifyDate: meta.sortBy = L"mtime"; break;
            case SortType::SortByExtension: meta.sortBy = L"type"; break;
            case SortType::SortBySize: meta.sortBy = L"size"; break;
            case SortType::SortByDimension: meta.sortBy = L"dim"; break;
            case SortType::SortByRating: meta.sortBy = L"rating"; break;
            default: meta.sortBy = L"name"; break;
        }
        meta.sortOrder = (m_sortOrder == Qt::AscendingOrder) ? L"asc" : L"desc";

        MetadataManager::instance().setFolderMeta(m_currentFolderPath, meta);
    }
}

void ContentSortController::setCurrentFolderPath(const QString& folderPath) {
    m_currentFolderPath = folderPath;
    if (folderPath.isEmpty()) {
        loadFromConfig();
        return;
    }

    FolderMeta meta = MetadataManager::instance().getFolderMeta(folderPath);
    if (!meta.sortBy.empty()) {
        if (meta.sortBy == L"name") m_sortType = SortType::SortByName;
        else if (meta.sortBy == L"cdate") m_sortType = SortType::SortByCreateDate;
        else if (meta.sortBy == L"mtime") m_sortType = SortType::SortByModifyDate;
        else if (meta.sortBy == L"type") m_sortType = SortType::SortByExtension;
        else if (meta.sortBy == L"size") m_sortType = SortType::SortBySize;
        else if (meta.sortBy == L"dim") m_sortType = SortType::SortByDimension;
        else if (meta.sortBy == L"rating") m_sortType = SortType::SortByRating;

        m_sortOrder = (meta.sortOrder == L"desc") ? Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        loadFromConfig();
    }
}

void ContentSortController::setSortType(SortType type) {
    if (m_sortType != type) {
        m_sortType = type;
        saveToConfig();
        emit sortCriteriaChanged(m_sortType, m_sortOrder);
    }
}

void ContentSortController::setSortOrder(Qt::SortOrder order) {
    if (m_sortOrder != order) {
        m_sortOrder = order;
        saveToConfig();
        emit sortCriteriaChanged(m_sortType, m_sortOrder);
    }
}

void ContentSortController::setSortCriteria(SortType type, Qt::SortOrder order) {
    bool changed = (m_sortType != type || m_sortOrder != order);
    m_sortType = type;
    m_sortOrder = order;
    if (changed) {
        saveToConfig();
        emit sortCriteriaChanged(m_sortType, m_sortOrder);
    }
}

// 🚀【打通模型通信】：应用排序时必须同步将 m_sortType 灌入 FilterProxyModel！
void ContentSortController::applySortToModel(QSortFilterProxyModel* proxyModel) {
    if (!proxyModel) return;
    if (auto* proxy = qobject_cast<FilterProxyModel*>(proxyModel)) {
        proxy->setSortCriteria(static_cast<int>(m_sortType), m_sortOrder);
    } else {
        proxyModel->sort(0, m_sortOrder);
    }
}

} // namespace QuarkMeta