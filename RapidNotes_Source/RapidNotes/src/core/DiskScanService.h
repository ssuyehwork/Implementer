#pragma once

#include <QString>
#include <vector>
#include <functional>
#include "ItemRecord.h"
#include "CoreEngine.h"

namespace QuarkMeta {

class DiskScanService {
public:
    /**
     * @brief 扫描指定物理路径，返回原始条目列表（不做任何 .arc 语义翻译）
     * @param path 起始物理路径
     * @param recursive 是否递归扫描子目录
     * @param shouldContinue 取消检查回调；每处理一个条目前调用一次，返回 false 时立即中止扫描
     */
    static std::vector<ItemRecord> scanDirectory(const QString& path,
                                                  bool recursive,
                                                  const std::function<bool()>& shouldContinue = std::function<bool()>(),
                                                  int maxDepth = 16);

    static std::vector<ItemRecord> scanDirectory(const QString& path,
                                                  bool recursive,
                                                  std::shared_ptr<CancellationToken> token);
};

} // namespace QuarkMeta
