#ifndef RapidNotes_CORE_ENGINE_H
#define RapidNotes_CORE_ENGINE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QThreadPool>
#include <atomic>
#include <memory>
#include <functional>
#include "CentralEventHub.h"

namespace RapidNotes {

/**
 * @brief 强类型应用程序命令类型枚举
 */
enum class AppCommandType {
    SetRating,         // 设置星级
    SetColor,          // 设置颜色标记
    SetTags,           // 设置/添加/移除标签
    AddTag,            // 批量添加标签
    RemoveTag,         // 批量移除标签
    RenameTag,         // 重命名全局标签
    RemoveGlobalTag,   // 擦除全局标签
    SetNote,           // 设置备注
    SetURL,            // 设置链接
    SetPinned,         // 置顶/取消置顶
    ToggleFavorite,    // 添加/取消收藏
    MoveToTrash,       // 移入回收站
    MoveToCategory,    // 批量移入指定分类
    RestoreFromTrash,  // 从回收站还原
    RenameItems,       // 重命名文件/文件夹
    DeletePermanently, // 物理删除文件
    RecordAccess       // 记录访问历史
};

/**
 * @brief 异步取消令牌 (CancellationToken)
 */
class CancellationToken {
public:
    CancellationToken() : m_canceled(false) {}
    void cancel() { m_canceled.store(true, std::memory_order_relaxed); }
    bool isCanceled() const { return m_canceled.load(std::memory_order_relaxed); }
    void reset() { m_canceled.store(false, std::memory_order_relaxed); }

private:
    std::atomic<bool> m_canceled;
};

/**
 * @brief 命令统一数据包结构
 */
struct AppCommand {
    AppCommandType type;
    QStringList targetPaths;
    QVariantMap params;
};

/**
 * @brief 中央大脑 (CoreEngine) - 业务决策与调度指挥中心
 * 
 * 铁律职责：
 * 1. 统一接收 UI 或外部提交的 AppCommand 命令；
 * 2. 负责业务合法性校验与调度后端 Service 执行真实写数据库/磁盘操作；
 * 3. 操作成功后驱动 CentralEventHub 进行增量 AppEvent 事件广播。
 */
class CoreEngine : public QObject {
    Q_OBJECT

public:
    static CoreEngine& instance();

    // 命令统一执行入口
    bool executeCommand(const AppCommand& cmd);

    // 异步批量给多个路径添加预设/通用标签
    void addTagsToPathsAsync(const QStringList& paths, const QStringList& tags, std::function<void(int pathCount)> onFinished = nullptr, int existingJobId = 0);

    // 取消令牌助手入口
    std::shared_ptr<CancellationToken> createCancellationToken();

private:
    explicit CoreEngine(QObject* parent = nullptr);
    ~CoreEngine() override = default;
    CoreEngine(const CoreEngine&) = delete;
    CoreEngine& operator=(const CoreEngine&) = delete;

    // 从 note://<id> 格式或纯数字字符串批量解析笔记 ID
    static QList<int> extractNoteIds(const QStringList& paths);

    void handleSetRating(const QStringList& paths, int rating);
    void handleSetColor(const QStringList& paths, const QString& color);
    void handleSetTags(const QStringList& paths, const QStringList& tags);
    void handleSetNote(const QStringList& paths, const QString& note);
    void handleSetURL(const QStringList& paths, const QString& url);
    void handleRecordAccess(const QStringList& paths);
    void handleToggleFavorite(const QStringList& paths);
    void handleRenameItem(const QString& path, const QString& newTitle);

    QThreadPool m_asyncTagPool;
};

} // namespace RapidNotes

#endif // RapidNotes_CORE_ENGINE_H
