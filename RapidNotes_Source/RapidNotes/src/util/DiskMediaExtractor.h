#ifndef DISKMEDIAEXTRACTOR_H
#define DISKMEDIAEXTRACTOR_H

#include <QImage>
#include <QString>
#include <QList>
#include <QPair>
#include <QSize>
#include <QMutex>
#include <QHash>
#include <QSet>
#include <mutex>
#include <cstdint>
#include <memory>
#include "../core/CoreController.h"
#include "../core/CoreEngine.h"

namespace QuarkMeta {

class DiskMediaExtractor {
public:
    // 磁盘缩略图缓存的唯一尺寸（文件夹里显示用）。
    // 自动提取与"重新生成缩略图"都只产出/缓存这一档。
    static constexpr int kThumbSize = 230;

    static std::mutex s_qtGuiMutex;
    static std::mutex s_jsonSaveMutex;

    struct ExtractResult {
        QImage thumbnail512;   // 历史字段名：内容为 ≤230 的缩略图，不要改名
        QSize originalSize;
        bool isValid = false;
    };

    // ---- 缓存路径 ----
    static QString getDiskThumbCachePathByFileId(uint32_t volSerial, uint64_t fileId);
    static QString getDiskThumbCachePath(const QString& filePath);

    // ---- 缓存读写 ----
    static QImage getCapsuleThumbnailReadOnly(const QString& filePath);
    static bool saveDiskThumbnail(const QString& filePath, const QImage& img);
    static void roamThumbnailCache(const QString& oldFilePath, const QString& newFilePath, bool isMove);
    static void roamThumbnailCacheBatch(const QList<QPair<QString, QString>>& pairs, bool isMove);

    // ---- 档位①：自动提取 ----
    static ExtractResult getCapsuleExtractResult(const QString& filePath, int size = kThumbSize, std::shared_ptr<CancellationToken> token = {});
    static QImage getCapsuleThumbnail(const QString& filePath, int size = kThumbSize, std::shared_ptr<CancellationToken> token = {});

    // ---- 档位②：重新生成缩略图（超时放长，成功后把失败标记改回 0）----
    static QImage forceExtractDeepThumbnail(const QString& filePath, int size = kThumbSize, std::shared_ptr<CancellationToken> token = {});

    static QSize fastExtractImageSize(const QString& filePath);

    // ---- 失败标记与元数据延迟落盘 ----
    static bool isMarkedFailed(const QString& folderPath, const QString& fileName);
    static void scheduleFailureMark(const QString& folderPath, const QString& fileName);
    static void flushPendingUpdates();

private:
    static void scheduleSizeUpdate(const QString& folderPath, const QString& fileName, const QSize& size);

    // 待落盘队列（失败标记 + 尺寸），由 flushPendingUpdates 合并写入 .QuarkMeta.json
    static QMutex s_pendingMutex;
    static QHash<QString, QSet<QString>> s_pendingFailures;
    static QHash<QString, QHash<QString, QSize>> s_pendingSizes;

    // 失败名单的内存副本（每个文件夹只从 JSON 读一次）
    static QMutex s_failureMutex;
    static QHash<QString, QSet<QString>> s_failedNames;
    static QSet<QString> s_failureLoadedFolders;
};

} // namespace QuarkMeta

#endif // DISKMEDIAEXTRACTOR_H
