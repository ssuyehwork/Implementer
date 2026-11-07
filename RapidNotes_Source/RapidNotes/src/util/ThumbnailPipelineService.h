#pragma once

#include <QObject>
#include <QPixmap>
#include <QImage>
#include <QString>
#include <QStringList>
#include <QCache>
#include <QMutex>
#include <QThreadPool>
#include <atomic>
#include <functional>
#include "CoreEngine.h"

namespace QuarkMeta {

class ThumbnailPipelineService : public QObject {
    Q_OBJECT

public:
    static ThumbnailPipelineService& instance();

    /**
     * @brief 一级内存 LRU 缓存直取 (0ms 耗时，UI 主线程安全)
     */
    QPixmap getFromMemoryCache(const QString& filePath, int targetSize) const;

    /**
     * @brief 异步按批次并发提取缩略图 (自动走三级缓存降级流水线)
     * @param filePaths 待提图的物理路径列表
     * @param targetSize 目标正方形边长像素 (如 96, 128, 230)
     * @param onSingleLoaded 单个缩略图就绪回调 (在 UI 主线程安全触发)
     */
    void loadBatchAsync(const QStringList& filePaths,
                        int targetSize,
                        std::function<void(const QString& path, const QPixmap& pixmap)> onSingleLoaded);

    /**
     * @brief 递增代际号并瞬间熔断所有正在排队的旧任务
     */
    void incrementGeneration();
    void cancelAll();

    std::shared_ptr<CancellationToken> currentToken() const;

    /**
     * @brief 内存缓存清理
     */
    void clearMemoryCache();

    /**
     * @brief 前台缩略图提取专用线程池（收藏面板等也共用，不占全局线程池）
     */
    QThreadPool* decodePool() { return &m_decodePool; }

private:
    explicit ThumbnailPipelineService(QObject* parent = nullptr);
    ~ThumbnailPipelineService() override = default;
    ThumbnailPipelineService(const ThumbnailPipelineService&) = delete;
    ThumbnailPipelineService& operator=(const ThumbnailPipelineService&) = delete;

    mutable QMutex m_cacheMutex;
    mutable QCache<QString, QPixmap> m_memoryCache;

    QThreadPool m_decodePool;
    std::atomic<uint64_t> m_currentGeneration{1};
    std::shared_ptr<CancellationToken> m_currentToken;
    static constexpr int kMaxMemoryCacheCount = 800; // 内存最多缓存 800 张缩略图 (约 50~80MB)
};

} // namespace QuarkMeta
