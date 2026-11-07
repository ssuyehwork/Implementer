#pragma once

#include <QStringList>
#include <QImage>
#include <vector>
#include <QSet>
#include "../core/ItemRecord.h"

namespace QuarkMeta {

struct DuplicateItemInfo {
    QString path;
    QString filename;
    int width = 0;
    int height = 0;
    qint64 size = 0;
    QString tagHint;
    QImage thumbnail;
};

struct DuplicateConflictGroup {
    DuplicateItemInfo existingItem;
    DuplicateItemInfo newItem;
};

class DuplicateDetectorService {
public:
    static QString computeFastHash(const QString& filePath, qint64 fileSize = -1);
    static QString computeFullSha256(const QString& filePath, const std::atomic<bool>* cancelToken = nullptr);

    // 严谨三阶内容哈希验重（必须在后台线程调用）
    static QSet<QString> findDuplicatePaths(const std::vector<ItemRecord>& records);

    // 后台比对新导入项与数据库已有项，返回重复冲突组列表
    static std::vector<DuplicateConflictGroup> detectDuplicates(const QStringList& newImportedPaths);
};

} // namespace QuarkMeta
