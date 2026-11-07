#pragma once

#include <QObject>
#include <QStringList>
#include <QWidget>
#include <vector>
#include <utility>

namespace QuarkMeta {

/**
 * @brief 物理安全永久删除与物理抹除服务层 (Single Source of Destruction Truth)
 */
class PermanentDeleteService : public QObject {
    Q_OBJECT

public:
    static PermanentDeleteService& instance();

    /**
     * @brief 永久删除选中的磁盘路径或回收站路径（带弹窗确认、进度条、扇区物理抹除与撤销栈清理）
     */
    bool execute(const QStringList& paths, QWidget* parentWidget = nullptr, bool isSecureShred = true);

    /**
     * @brief 永久删除指定的回收站条目列表
     */
    bool executeTrashItems(const std::vector<std::pair<int, QString>>& trashItems, QWidget* parentWidget = nullptr);

    /**
     * @brief 🚀【统一安全管线】：清空整个回收站（带全量统计、弹窗确认、进度条、扇区物理抹除与撤销栈清理）
     */
    bool executeEmptyTrash(QWidget* parentWidget = nullptr, bool isSecureShred = true);

signals:
    void permanentDeleteCompleted();

private:
    explicit PermanentDeleteService(QObject* parent = nullptr);
    ~PermanentDeleteService() override = default;
    PermanentDeleteService(const PermanentDeleteService&) = delete;
    PermanentDeleteService& operator=(const PermanentDeleteService&) = delete;
};

} // namespace QuarkMeta