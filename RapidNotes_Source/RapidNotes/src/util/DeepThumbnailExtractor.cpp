#include "DeepThumbnailExtractor.h"
#include "DiskMediaExtractor.h"
#include <QtConcurrent>
#include <QThreadPool>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <mutex>

namespace QuarkMeta {

namespace {
// 重新生成缩略图专用单线程池：单项耗时可达 45 秒，
// 不占用前台缩略图线程池，也不占用全局线程池
QThreadPool* deepExtractPool() {
    static QThreadPool pool;
    static std::once_flag flag;
    std::call_once(flag, []() { pool.setMaxThreadCount(1); });
    return &pool;
}
} // namespace

DeepThumbnailExtractor& DeepThumbnailExtractor::instance() {
    static DeepThumbnailExtractor inst;
    return inst;
}

void DeepThumbnailExtractor::extractBatchAsync(
    const QStringList& filePaths,
    std::function<void(const QString& path, bool success)> onItemCompleted,
    std::function<void(int successCount, int totalCount)> onAllFinished)
{
    if (filePaths.isEmpty()) {
        if (onAllFinished) onAllFinished(0, 0);
        return;
    }

    (void)QtConcurrent::run(deepExtractPool(), [filePaths, onItemCompleted, onAllFinished]() {
        int total = filePaths.size();
        int successCount = 0;

        for (const QString& filePath : filePaths) {
            // 1. 物理删除已有的旧缩略图缓存，确保重新生成的绝对纯净
            QString cachePath = DiskMediaExtractor::getDiskThumbCachePath(filePath);
            if (QFile::exists(cachePath)) {
                QFile::remove(cachePath);
            }

            // 2. 触发长效深度解码（重新生成档，放宽超时至 45 秒；输出与自动提取一致：230）
            QImage img = DiskMediaExtractor::forceExtractDeepThumbnail(filePath, DiskMediaExtractor::kThumbSize);
            bool ok = !img.isNull();
            if (ok) successCount++;

            // 3. 单项完成回到主线程通知
            if (onItemCompleted) {
                QMetaObject::invokeMethod(QCoreApplication::instance(), [onItemCompleted, filePath, ok]() {
                    onItemCompleted(filePath, ok);
                });
            }
        }

        // 4. 全量完成通知
        if (onAllFinished) {
            QMetaObject::invokeMethod(QCoreApplication::instance(), [onAllFinished, successCount, total]() {
                onAllFinished(successCount, total);
            });
        }
    });
}

} // namespace QuarkMeta
