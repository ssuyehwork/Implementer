#pragma once
#include <Qt>

namespace QuarkMeta {

/**
 * @brief 工业级模型契约 (ModelContract)
 * 物理统一全应用 Role 定义，彻底解决跨组件 Role 冲突问题。
 */
enum CommonRole {
    // 基础角色 (UserRole + 0..100)
    TypeRole            = Qt::UserRole + 0,  // 类型 (file/folder/category/system/bookmark)
    IdRole              = Qt::UserRole + 1,  // 数据库 ID (分类 ID 等)
    NameRole            = Qt::UserRole + 2,  // 原始名称
    PathRole            = Qt::UserRole + 3,  // 物理路径
    ColorRole           = Qt::UserRole + 4,  // 颜色标记 (Hex)
    RatingRole          = Qt::UserRole + 5,  // 星级评级 (0-5)
    TagsRole            = Qt::UserRole + 6,  // 标签列表 (QStringList)
    NoteRole            = Qt::UserRole + 7,  // 备注角色 (QString)
    UrlRole             = Qt::UserRole + 8,  // 关联网址 (QString)

    // 状态角色 (UserRole + 101..200)
    PinnedRole          = Qt::UserRole + 101, // 置顶状态 (快速访问镜像)
    IsLockedRole        = Qt::UserRole + 102, // 锁定/置顶状态 (列表显示)
    EncryptedRole       = Qt::UserRole + 103, // 是否加密
    EncryptHintRole     = Qt::UserRole + 104, // 加密提示
    IsEmptyRole         = Qt::UserRole + 106, // 是否为空目录

    // UI/渲染角色 (UserRole + 201..300)
    AspectRatioRole     = Qt::UserRole + 201, // 图像宽高比
    HasThumbnailRole    = Qt::UserRole + 202, // 是否拥有物理缩略图
    PalettesRole        = Qt::UserRole + 203, // 物理色板数据
    CountRole           = Qt::UserRole + 204, // 子项数量

    // 列视图与拖放交互角色 (UserRole + 210..220)
    IsParentExpandedRole = Qt::UserRole + 210, // 列视图父目录展开高亮
    IsDropTargetRole     = Qt::UserRole + 211, // 拖拽目标悬停高亮
    SectionHeaderRole    = Qt::UserRole + 212, // 是否为分区标头行
    SectionHeaderTextRole = Qt::UserRole + 213,// 分区标头显示文本
    SectionCollapsedRole = Qt::UserRole + 214, // 文件夹分区折叠状态
    SectionKindRole      = Qt::UserRole + 215, // 分区角色类型 (0:普通项, 1:文件夹标头, 2:文件标头)
    SectionRowRole       = Qt::UserRole + 216, // 分区内奇偶序号

    // 磁盘回收站专用角色
    IsDiskTrashRole     = Qt::UserRole + 208, // 是否是磁盘回收站项目
    DiskTrashIdRole     = Qt::UserRole + 209  // 磁盘回收站表 ID
};

/**
 * @brief 列表视图标准列契约 (FileListColumn)
 * 彻底消灭数字字面量，提供强类型枚举语义。
 */
enum class FileListColumn : int {
    Name = 0,        // 名称 (微卡片 + 文本)
    Status = 1,      // 状态 (固定 40px，默认常态隐藏)
    Rating = 2,      // 评分 (固定 100px)
    Dimension = 3,   // 尺寸 (固定 100px)
    Type = 4,        // 类型 (固定 60px)
    Size = 5,        // 大小 (固定 80px)
    CreatedDate = 6, // 创建日期 (固定 130px)
    ModifiedDate = 7,// 修改日期 (固定 130px)
    Count = 8
};

} // namespace QuarkMeta
