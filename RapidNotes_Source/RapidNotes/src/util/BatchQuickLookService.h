#pragma once

#include <QObject>
#include <QStringList>
#include <memory>
#include <mutex>
#include "../core/CoreEngine.h"

namespace QuarkMeta {

/**
 * @brief 专职负责后台批量预生成 2048px 高清 QuickLook 预览图的高性能服务
 * 特性：
 * 1. 原生批量处理管线 (extractBatchAsync)；
 * 2. 预检过滤 (Pre-Scan)，只对真正缺失/过期的文件做增量生成，拒绝算力与 IO 浪费；
 * 3. 驱动 BatchProgressDialog 进度对话框展示；
 * 4. 0ms 顺畅抢占：空格预览触发时 10~50ms 即时强行熔断并让出 GS 锁。
 */
class BatchQuickLookService : public QObject {
    Q_OBJECT
public:
    static BatchQuickLookService& instance();

    /**
     * @brief 原生批量异步接口：预生成大图预览
     * @param rootPaths 目标路径列表 (支持文件夹、文件或库路径)
     */
    void extractBatchAsync(const QStringList& rootPaths);

    /**
     * @brief 触发即时强行熔断 (取消当前正在运行的批量生成任务，杀掉 GS 进程并释放锁)
     */
    void cancelCurrentBatch();

    bool isRunning() const;

private:
    explicit BatchQuickLookService(QObject* parent = nullptr);
    ~BatchQuickLookService() override = default;

    std::shared_ptr<CancellationToken> m_currentToken;
    mutable std::mutex m_mutex;
};

} // namespace QuarkMeta
