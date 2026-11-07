#pragma once

#include "../core/BatchRenameService.h"

namespace QuarkMeta {

/**
 * @brief 批量重命名引擎 (轻量级预览计算代理，业务流全权交由 BatchRenameService)
 */
class BatchRenameEngine {
public:
    static BatchRenameEngine& instance() {
        static BatchRenameEngine inst;
        return inst;
    }

    std::vector<std::wstring> preview(const std::vector<std::wstring>& originalPaths,
                                     const std::vector<RenameRule>& rules) {
        return BatchRenameService::instance().computePreview(originalPaths, rules);
    }

private:
    BatchRenameEngine() = default;
    ~BatchRenameEngine() = default;
};

} // namespace QuarkMeta
