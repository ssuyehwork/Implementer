#include "FileFilterService.h"
#include <QFileInfo>

namespace QuarkMeta {
bool FileFilterService::isAuxiliaryFile(const QString& path) {
    if (path.isEmpty()) return true;

    QFileInfo info(path);
    QString fileName = info.fileName();

    // 1. 过滤内部配置文件与缩略图
    if (fileName.endsWith(".QuarkMeta.json", Qt::CaseInsensitive) ||
        fileName.endsWith(".QuarkMeta.json.tmp", Qt::CaseInsensitive) ||
        fileName.endsWith("_thumbnail.png", Qt::CaseInsensitive)) {
        return true;
    }

    // 2. 过滤缓存目录
    if (fileName.compare(".QuarkMeta", Qt::CaseInsensitive) == 0) {
        return true;
    }

    return false;
}
}
