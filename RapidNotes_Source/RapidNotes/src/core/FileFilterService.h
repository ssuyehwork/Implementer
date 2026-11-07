#pragma once
#include <QString>

namespace QuarkMeta {
class FileFilterService {
public:
    // 统一过滤无用辅助配置文件、缩略图及系统缓存目录
    static bool isAuxiliaryFile(const QString& path);
};
}
