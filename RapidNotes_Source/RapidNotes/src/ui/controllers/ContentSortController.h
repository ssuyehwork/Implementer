#pragma once

#include <QObject>
#include <QSortFilterProxyModel>
#include "../../core/ModelContract.h"

namespace QuarkMeta {

enum class SortType {
    SortByName,
    SortByCreateDate,
    SortByModifyDate,
    SortByExtension,
    SortBySize,
    SortByDimension,
    SortByRating,
    SortByAddedDate
};

class ContentSortController : public QObject {
    Q_OBJECT

public:
    explicit ContentSortController(QObject* parent = nullptr);
    ~ContentSortController() override = default;

    SortType sortType() const { return m_sortType; }
    Qt::SortOrder sortOrder() const { return m_sortOrder; }

    void setSortType(SortType type);
    void setSortOrder(Qt::SortOrder order);
    void setSortCriteria(SortType type, Qt::SortOrder order);
    void setCurrentFolderPath(const QString& folderPath);

    static FileListColumn columnForSortType(SortType type);
    static SortType sortTypeForColumn(FileListColumn col);

    void applySortToModel(QSortFilterProxyModel* proxyModel);
    void loadFromConfig();
    void saveToConfig();

signals:
    void sortCriteriaChanged(SortType type, Qt::SortOrder order);

private:
    SortType m_sortType = SortType::SortByName;
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;
    QString m_currentFolderPath;
};

} // namespace QuarkMeta
