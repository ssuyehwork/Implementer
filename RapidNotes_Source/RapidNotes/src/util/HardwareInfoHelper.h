#pragma once

#include <QString>
#include <QStringList>

namespace QuarkMeta {

/**
 * @brief 硬件特征提取与 5 选 1 授权校验辅助类
 */
class HardwareInfoHelper {
public:
    /**
     * @brief 获取 Windows 系统主分区（C 盘）所在的物理磁盘序列号
     */
    static QString getCDiskPhysicalSerialNumber();

    /**
     * @brief 获取程序当前运行目录所在的物理磁盘序列号（支持移动硬盘随插随用识别）
     */
    static QString getAppDrivePhysicalSerialNumber();

    /**
     * @brief 获取指定驱动器路径（如 C: 或 D:）对应的物理磁盘序列号
     */
    static QString getDiskPhysicalSerialNumberByDrive(const QString& drivePath);

    /**
     * @brief 获取主板序列号 / UUID
     */
    static QString getBoardSerialNumber();

    /**
     * @brief 使用 CPUID 指令提取处理器特征序列号
     */
    static QString getCpuId();

    /**
     * @brief 获取 BIOS 序列号
     */
    static QString getBiosSerialNumber();

    /**
     * @brief 获取所有硬盘物理序列号列表
     */
    static QStringList getAllDiskSerialNumbers();

    /**
     * @brief 获取所有网络适配器的 MAC 地址列表
     */
    static QStringList getAllMacAddresses();

    /**
     * @brief 硬件通行证验证：只要满足任意一项即判定授权合法
     * @return true 允许启动，false 触发安全拦截
     */
    static bool isHardwareAuthorized();
};

} // namespace QuarkMeta
