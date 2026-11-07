#ifndef QUARKMETA_CENTRAL_EVENT_HUB_H
#define QUARKMETA_CENTRAL_EVENT_HUB_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

namespace QuarkMeta {

/**
 * @brief 强类型应用程序事件枚举
 */
enum class AppEventType {
    VolumeStateChanged,     // 驱动器挂载/卸载
    PathNavigated,          // 目录导航变更
    SelectionChanged,       // UI选中项变更
    MetadataUpdated,        // 元数据变动(星级/颜色/标签/备注/置顶等)
    FavoritesUpdated,       // 收藏夹状态变更
    ItemsDeleted,           // 文件物理擦除/删除
    ItemsRenamed,           // 文件批量或单项重命名
    FilterStateChanged,     // 条件筛选状态变更
    UndoRedoPerformed       // 撤销/重做事务完成
};

/**
 * @brief 全局事件统一载体结构
 */
struct AppEvent {
    AppEventType type;
    QString targetPath;
    QStringList paths;
    QVariantMap payload;
};

/**
 * @brief 传声筒 (CentralEventHub) - 纯消息事件总线
 *
 * 铁律职责：
 * 1. 负责全系统强类型事件的解耦分发与广播；
 * 2. 绝对不包含任何业务数据逻辑、数据库读写或磁盘/内存缓存逻辑；
 * 3. 保证线程安全。
 */
class CentralEventHub : public QObject {
    Q_OBJECT

public:
    static CentralEventHub& instance();

    // 广播事件通用入口
    void publishEvent(const AppEvent& event);

signals:
    // 全局事件统一广播信号
    void eventOccurred(const QuarkMeta::AppEvent& event);

private:
    explicit CentralEventHub(QObject* parent = nullptr);
    ~CentralEventHub() override = default;
    CentralEventHub(const CentralEventHub&) = delete;
    CentralEventHub& operator=(const CentralEventHub&) = delete;
};

} // namespace QuarkMeta

#endif // QUARKMETA_CENTRAL_EVENT_HUB_H
