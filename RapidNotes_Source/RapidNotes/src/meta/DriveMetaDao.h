#pragma once

#include <QString>
#include <string>
#include <unordered_map>
#include "../core/ItemRecord.h"

namespace QuarkMeta {

struct DriveMetaRecord {
    std::wstring drivePath; // 格式："C:\"
    int rating = 0;
    std::wstring color;
    bool pinned = false;
    std::wstring note;
    std::wstring url;
};

class DriveMetaDao {
public:
    /**
     * @brief 初始化 inspiration.db 中的 drive_metadata 数据表
     */
    static bool initTable();

    /**
     * @brief 读取所有盘符的元数据记录并返回 Mapping
     */
    static std::unordered_map<std::wstring, DriveMetaRecord> getAllDriveMeta();

    /**
     * @brief 读取单个盘符的元数据记录
     */
    static DriveMetaRecord getDriveMeta(const std::wstring& drivePath);

    /**
     * @brief 写入或更新盘符元数据
     */
    static bool saveDriveMeta(const DriveMetaRecord& record);
};

} // namespace QuarkMeta
