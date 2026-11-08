#pragma once

#include <QStandardItemModel>
#include <QList>
#include <QMap>
#include "../meta/LibraryDao.h"
#include "../meta/SmartFolderDao.h"
#include "LibraryNodeKind.h"

namespace QuarkMeta {

class SmartFolderCountService;

/**
 * @brief 库树模型构建器
 * 负责将数据层（系统项、智能文件夹、自定义分类、置顶镜像）组装构建为标准的 QStandardItemModel。
 */
class LibraryModelBuilder {
public:
    struct BuildResult {
        QStandardItem* quickAccessHeader = nullptr;
        QStandardItem* smartFolderHeader = nullptr;
        QStandardItem* customSectionHeader = nullptr;
        QList<SmartFolderRecord> smartFolders;
    };

    /**
     * @brief 构建完整的树模型
     * @param model 待填充的目标模型（调用前由调用方负责 clear）
     * @param countService 智能文件夹计数缓存提供服务
     * @return 构建出的三大主要分组标头与智能文件夹记录列表
     */
    static BuildResult populateModel(QStandardItemModel* model, SmartFolderCountService* countService);
};

} // namespace QuarkMeta
