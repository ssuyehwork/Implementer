#pragma once

#include <QObject>
#include <QStringList>
#include <functional>

namespace QuarkMeta {

/**
 * @brief 专职负责超大/复杂矢量文件的深度重试提取服务
 * 特性：高超时容忍 (30~60s)、强制覆盖旧缓存、独立并发队列
 */
class DeepThumbnailExtractor : public QObject {
    Q_OBJECT
public:
    static DeepThumbnailExtractor& instance();

    /**
     * @brief 异步深度重新提取一组文件的缩略图
     * @param filePaths 目标文件物理路径列表
     * @param onItemCompleted 单个文件完成回调 (path, success)
     * @param onAllFinished 全量任务结束回调 (successCount, totalCount)
     */
    void extractBatchAsync(
        const QStringList& filePaths,
        std::function<void(const QString& path, bool success)> onItemCompleted = nullptr,
        std::function<void(int successCount, int totalCount)> onAllFinished = nullptr
    );

private:
    explicit DeepThumbnailExtractor(QObject* parent = nullptr) : QObject(parent) {}
    ~DeepThumbnailExtractor() override = default;
};

} // namespace QuarkMeta
