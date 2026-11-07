#pragma once

#include <QImage>
#include <QString>
#include <QByteArray>
#include <memory>
#include "../core/CoreEngine.h"

namespace QuarkMeta {

class FormatDecoders {
public:
    // TIFF 物理内存解码（含安全防御）
    static QImage decodeTiffMemorySafely(const QByteArray& tiffData, int maxMemoryMB = 64);

    // PSD 嵌套缩略图提取
    static QImage extractPsdHeaderThumbnail(const QString& filePath);

    // AI 嵌套预览图与 XMP 提取
    static QImage extractAiPreview(const QString& filePath, int targetSize = 512, int customTimeoutMs = 0, std::shared_ptr<CancellationToken> token = nullptr);
    static QImage extractAiQuickLook(const QString& filePath, int targetSize = 2048, int customTimeoutMs = 0, std::shared_ptr<CancellationToken> token = nullptr);

    // 通用兼容接口（默认路由至缩略图策略）
    static QImage extractEpsPreview(const QString& filePath, int targetSize = 512, int customTimeoutMs = 0, std::shared_ptr<CancellationToken> token = nullptr);

    // 策略 1：自动提取缩略图（GS 100 DPI 优先 → 内嵌 TIFF → %%BeginPreview）
    static QImage extractEpsThumbnail(const QString& filePath, int targetSize = 512, int customTimeoutMs = 0, std::shared_ptr<CancellationToken> token = nullptr);

    // 策略 2：手动双击提取（GS 144 DPI 优先 → 内嵌 TIFF → %%BeginPreview）
    static QImage extractEpsQuickLook(const QString& filePath, int targetSize = 2048, int customTimeoutMs = 0, std::shared_ptr<CancellationToken> token = nullptr, bool* fromGhostscript = nullptr);

    // External Process: Ghostscript 降采样渲染 (customTimeoutMs > 0 时使用自定义长效超时，默认 72 DPI)
    static QImage renderGhostscriptSafely(const QString& filePath, int targetSize = 512, int customTimeoutMs = 0, std::shared_ptr<CancellationToken> token = nullptr, int dpi = 72);

private:
    static QString findGhostscriptExecutable();
    static QImage renderPdfAiFirstPage(const QString& filePath, int targetSize = 512);

    // AI/PDF 内嵌后备提取链（%AI7-10 缩略图 → XMP 图 → Windows 系统缩略图 → 内嵌 PNG → 内嵌 JPG → Shell 兜底）
    static QImage extractAiEmbeddedFallback(const QString& filePath, int targetSize = 512);

    // EPS 内嵌预览提取：DOS 二进制头 TIFF → %%BeginPreview 文本预览（不含 Ghostscript）
    static QImage extractEpsEmbeddedPreview(const QString& filePath);
};

} // namespace QuarkMeta
