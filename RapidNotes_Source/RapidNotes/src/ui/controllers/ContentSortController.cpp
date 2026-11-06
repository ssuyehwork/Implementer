#include "ContentSortController.h"
#include "../models/FilterProxyModel.h"
#include "../../core/AppConfig.h"

namespace RapidNotes {

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

void ContentSortController::saveToConfig() {
    AppConfig::instance().setValue("ContentPanel/RightClickSortType", static_cast<int>(m_sortType));
    AppConfig::instance().setValue("ContentPanel/RightClickSortOrder", static_cast<int>(m_sortOrder));
    AppConfig::instance().sync();
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

void ContentSortController::setCurrentFolderPath(const QString& folderPath) {
    m_currentFolderPath = folderPath;
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

void ContentSortController::applySortToModel(QSortFilterProxyModel* proxyModel) {
    if (!proxyModel) return;
    if (auto* proxy = qobject_cast<FilterProxyModel*>(proxyModel)) {
        proxy->setSortCriteria(static_cast<int>(m_sortType), m_sortOrder);
    } else {
        proxyModel->sort(0, m_sortOrder);
    }
}

} // namespace RapidNotes
