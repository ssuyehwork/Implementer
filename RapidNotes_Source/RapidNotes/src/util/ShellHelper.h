#pragma once
#include <QString>
#include <QStringList>
#include <QSet>
#include <QMap>
#include <QIcon>
#include <QWidget>

namespace QuarkMeta {

/**
 * @brief 系统服务层工具类 (ShellHelper)
 * 封装 Windows 原生 Shell 调用与通用格式化逻辑。
 */
class ShellHelper {
public:
    /**
     * @brief 移入回收站
     */
    static bool moveToTrash(const QStringList& paths);

    /**
     * @brief 执行复制或移动
     */
    static bool copyOrMoveItems(const QStringList& sourcePaths, const QString& destDir, bool isMove,
                                bool overwriteAll = false, bool autoRenameAll = false,
                                const QSet<QString>& overwriteFiles = {}, const QSet<QString>& autoRenameFiles = {},
                                QMap<QString, QString>* outDestMap = nullptr);

    /**
     * @brief 显示文件属性对话框
     */
    static void showProperties(const QString& path);

    /**
     * @brief 在资源管理器中定位
     */
    static void openInExplorer(const QString& path);

    /**
     * @brief 在资源管理器中打开当前目录
     */
    static void openDirectoryInExplorer(const QString& dirPath);

    struct AssocAppInfo {
        QString name;
        QString execPath;
        QIcon icon;
    };

    /**
     * @brief 获取指定文件扩展名的系统关联应用程序列表
     */
    static QList<AssocAppInfo> getAssociatedApps(const QString& filePath);

    /**
     * @brief 唤起 Windows 原生“打开方式”对话框
     */
    static void showOpenWithDialog(const QString& filePath, QWidget* parent = nullptr);

    /**
     * @brief 重命名条目
     */
    static bool renameItem(const QString& oldPath, const QString& newPath);

    /**
     * @brief 格式化字节大小
     */
    static QString formatSize(qint64 bytes);

    /**
     * @brief 物理设置文件/文件夹隐藏属性 (解耦自 DatabaseManager)
     */
    static void ensureHidden(const std::wstring& path);

    /**
     * @brief 解压压缩包到指定目标目录
     * @param archivePath 压缩包物理路径（支持 .zip / .7z / .rar / .tar.* 等格式）
     * @param destDir     解压目标目录（调用方负责确保该目录已创建）
     * @note  .zip 格式使用 PowerShell Expand-Archive 原生后台解压；
     *        其余格式通过 ShellExecuteExW 委托系统关联程序（7-Zip / WinRAR）处理。
     */
    static void extractArchive(const QString& archivePath, const QString& destDir);

};

} // namespace QuarkMeta
