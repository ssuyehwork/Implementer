#pragma once

#include <QString>
#include <QStringList>
#include <QList>
#include "MetadataDefs.h"

namespace QuarkMeta {

#ifndef QUARKMETA_NODEORDERENTRY_DEFINED
#define QUARKMETA_NODEORDERENTRY_DEFINED
struct NodeOrderEntry {
    int id = 0;
    int parentId = 0;
    int sortOrder = 0;
};
#endif

struct LibraryCategoryRecord {
    int id = 0;
    int parentId = 0;
    QString name;
    QString iconKey = "Data-Classification.svg";
    QString colorHex = "#888888";
    int sortOrder = 0;
    bool isPinned = false;
    QStringList presetTags;
    QStringList associatedPaths;
};

class LibraryDao {
public:
    static bool initTable();
    static QList<LibraryCategoryRecord> getAllCategories();
    static int addCategory(const QString& name, int parentId = 0, const QString& iconKey = "Data-Classification.svg", const QString& colorHex = "#888888");
    static bool updateCategoryNode(int id, const QString& name, const QString& iconKey, const QString& colorHex);
    static bool setCategoryPinned(int id, bool pinned);
    static bool updateNodeParentAndOrder(int id, int newParentId, int sortOrder);
    static bool updateNodesParentAndOrder(const QList<NodeOrderEntry>& entries);
    static bool removeCategoryById(int id);
    static bool updateCategoryPaths(int id, const QStringList& paths);
    static bool addPathsToCategory(int id, const QStringList& paths);
    static bool removePathsFromCategory(int id, const QStringList& paths);
    static bool removePathsFromLibrary(const QStringList& paths);
    static QStringList getCategoryPaths(int id, bool verifyExistence = true);
    static LibraryCategoryRecord getCategoryById(int id);
    static QStringList getPresetTagsWithAncestors(int id);
    static bool updatePresetTags(int id, const QStringList& tags);
    static bool updatePathPrefix(const QString& oldPrefix, const QString& newPrefix);

};

} // namespace QuarkMeta
