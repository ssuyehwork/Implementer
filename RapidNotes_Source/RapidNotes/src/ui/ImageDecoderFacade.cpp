#include "ImageDecoderFacade.h"
#include "FormatDecoders.h"
#include <QFileInfo>
#include <QFile>
#include <QImageReader>
#include <QSvgRenderer>
#include <QPainter>
#include <QRegularExpression>

namespace QuarkMeta {

DecodedMediaResult ImageDecoderFacade::decodeSinglePass(const QString& filePath, int targetSize, ExtractMode mode, std::shared_ptr<CancellationToken> token) {
    DecodedMediaResult result;
    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();
    // 自动提取用默认短超时(0)；重新生成与预览用长超时
    const int customTimeoutMs = (mode == ExtractMode::Auto) ? 0 : kLongTimeoutMs;

    // 1. 特殊矢量与设计格式分流
    if (ext == "svg") {
        QSvgRenderer renderer(filePath);
        if (renderer.isValid()) {
            QRectF vb = renderer.viewBoxF();
            result.originalSize = vb.isValid() ? vb.size().toSize() : renderer.defaultSize();
            if (result.originalSize.isEmpty()) result.originalSize = QSize(512, 512);

            QImage img(targetSize, targetSize, QImage::Format_ARGB32_Premultiplied);
            img.fill(Qt::transparent);
            QPainter p(&img);
            renderer.render(&p);
            result.thumbnail512 = img;
            result.isValid = true;
        }
        return result;
    }
    if (ext == "psd" || ext == "psb") {
        result.thumbnail512 = FormatDecoders::extractPsdHeaderThumbnail(filePath);
        result.originalSize = readImageDimensions(filePath);
        result.isValid = !result.thumbnail512.isNull();
        return result;
    }
    if (ext == "ai" || ext == "pdf") {
        result.thumbnail512 = (mode == ExtractMode::Preview)
            ? FormatDecoders::extractAiQuickLook(filePath, targetSize, customTimeoutMs, token)
            : FormatDecoders::extractAiPreview(filePath, targetSize, customTimeoutMs, token);
        result.originalSize = result.thumbnail512.size();
        result.isValid = !result.thumbnail512.isNull();
        return result;
    }
    if (ext == "eps") {
        result.thumbnail512 = (mode == ExtractMode::Preview)
            ? FormatDecoders::extractEpsQuickLook(filePath, targetSize, customTimeoutMs, token)
            : FormatDecoders::extractEpsThumbnail(filePath, targetSize, customTimeoutMs, token);
        result.originalSize = result.thumbnail512.size();
        result.isValid = !result.thumbnail512.isNull();
        return result;
    }

    // 2. 常规栅格图像（JPG / PNG / WEBP / BMP / GIF）硬件分块降采样
    QImageReader reader(filePath);
    reader.setAutoTransform(true); // 自动处理 EXIF 旋转
    if (!reader.canRead()) return result;

    result.originalSize = reader.size(); // 获取真实尺寸

    // 设置底层 DCT 硬件级缩放
    if (result.originalSize.isValid() && (result.originalSize.width() > targetSize || result.originalSize.height() > targetSize)) {
        QSize scaled = result.originalSize.scaled(targetSize, targetSize, Qt::KeepAspectRatio);
        reader.setScaledSize(scaled);
    }

    result.thumbnail512 = reader.read();
    result.isValid = !result.thumbnail512.isNull();
    return result;
}

QImage ImageDecoderFacade::loadScaledImage(const QString& filePath, int targetSize, int maxAllocationMB) {
    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();

    // 1. 路由至专用格式解码器 (PSD / AI / EPS)
    if (ext == "psd" || ext == "psb") {
        return FormatDecoders::extractPsdHeaderThumbnail(filePath);
    }
    if (ext == "ai" || ext == "pdf") {
        return FormatDecoders::extractAiPreview(filePath, targetSize);
    }
    if (ext == "eps") {
        return FormatDecoders::extractEpsPreview(filePath, targetSize);
    }
    if (ext == "tif" || ext == "tiff") {
        QFile f(filePath);
        if (f.open(QIODevice::ReadOnly)) {
            return FormatDecoders::decodeTiffMemorySafely(f.readAll(), 64);
        }
    }

    // 2. 普通图像 (PNG / JPG / WEBP / BMP 等) 走 QImageReader 降采样加载
    QImageReader reader(filePath);
    reader.setAllocationLimit(maxAllocationMB);

    if (!reader.canRead()) return QImage();

    QSize origSize = reader.size();
    if (origSize.isValid() && (origSize.width() > targetSize || origSize.height() > targetSize)) {
        QSize scaledSize = origSize.scaled(targetSize, targetSize, Qt::KeepAspectRatio);
        reader.setScaledSize(scaledSize);
    }

    return reader.read();
}

QSize ImageDecoderFacade::readImageDimensions(const QString& filePath) {
    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();

    // 1. AI / PDF 文件头毫秒级尺寸嗅探 (解析 /CropBox [0 0 W H] / /MediaBox [0 0 W H] / %%BoundingBox)
    if (ext == "ai" || ext == "pdf") {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray head = file.read(64 * 1024);
            file.close();

            // 1. 匹配 /CropBox [ 0 0 595.28 841.89 ]
            static QRegularExpression rxCropBox(R"(/CropBox\s*\[\s*([\d\.\-]+)\s+([\d\.\-]+)\s+([\d\.\-]+)\s+([\d\.\-]+)\s*\])");
            auto matchCrop = rxCropBox.match(head);
            if (matchCrop.hasMatch()) {
                double w = matchCrop.captured(3).toDouble() - matchCrop.captured(1).toDouble();
                double h = matchCrop.captured(4).toDouble() - matchCrop.captured(2).toDouble();
                if (w > 0 && h > 0) return QSize(qRound(w), qRound(h));
            }

            // 2. 匹配 /MediaBox [ 0 0 595.28 841.89 ]
            static QRegularExpression rxMediaBox(R"(/MediaBox\s*\[\s*([\d\.\-]+)\s+([\d\.\-]+)\s+([\d\.\-]+)\s+([\d\.\-]+)\s*\])");
            auto match = rxMediaBox.match(head);
            if (match.hasMatch()) {
                double w = match.captured(3).toDouble() - match.captured(1).toDouble();
                double h = match.captured(4).toDouble() - match.captured(2).toDouble();
                if (w > 0 && h > 0) return QSize(qRound(w), qRound(h));
            }

            // 3. 匹配 PostScript %%BoundingBox: 0 0 595 842
            static QRegularExpression rxBBox(R"(%%BoundingBox:\s*([\d\-]+)\s+([\d\-]+)\s+([\d\-]+)\s+([\d\-]+))");
            auto matchBBox = rxBBox.match(head);
            if (matchBBox.hasMatch()) {
                int w = matchBBox.captured(3).toInt() - matchBBox.captured(1).toInt();
                int h = matchBBox.captured(4).toInt() - matchBBox.captured(2).toInt();
                if (w > 0 && h > 0) return QSize(w, h);
            }
        }
    }

    // 2. EPS 文件头嗅探
    if (ext == "eps") {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray head = file.read(32 * 1024);
            file.close();
            static QRegularExpression rxBBox(R"(%%BoundingBox:\s*([\d\-]+)\s+([\d\-]+)\s+([\d\-]+)\s+([\d\-]+))");
            auto match = rxBBox.match(head);
            if (match.hasMatch()) {
                int w = match.captured(3).toInt() - match.captured(1).toInt();
                int h = match.captured(4).toInt() - match.captured(2).toInt();
                if (w > 0 && h > 0) return QSize(w, h);
            }
        }
    }

    // 3. PSD / PSB 文件头尺寸嗅探
    if (ext == "psd" || ext == "psb") {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray header = file.read(26);
            file.close();
            if (header.size() == 26 && header.startsWith("8BPS")) {
                auto be32 = [](const uchar* p) {
                    return (quint32(p[0]) << 24) | (quint32(p[1]) << 16) | (quint32(p[2]) << 8) | quint32(p[3]);
                };
                const uchar* data = reinterpret_cast<const uchar*>(header.constData());
                quint32 height = be32(data + 14);
                quint32 width  = be32(data + 18);
                if (width > 0 && height > 0) return QSize(width, height);
            }
        }
    }

    // 4. SVG 文件头 4KB 毫秒级轻量正则嗅探
    if (ext == "svg") {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray head = file.read(4096);
            file.close();

            QString headStr = QString::fromUtf8(head);

            // 1. 优先匹配 viewBox="0 0 24 24" 或 viewBox="0 0 1000 1000"
            static QRegularExpression rxViewBox(R"(viewBox\s*=\s*["']\s*([\d\.\-]+)\s+([\d\.\-]+)\s+([\d\.\-]+)\s+([\d\.\-]+)\s*["'])", QRegularExpression::CaseInsensitiveOption);
            auto matchVB = rxViewBox.match(headStr);
            if (matchVB.hasMatch()) {
                double w = matchVB.captured(3).toDouble() - matchVB.captured(1).toDouble();
                double h = matchVB.captured(4).toDouble() - matchVB.captured(2).toDouble();
                if (w > 0 && h > 0) return QSize(qRound(w), qRound(h));
            }

            // 2. 匹配 width="24" height="24" 或 width="24px" height="24px"
            static QRegularExpression rxWidth(R"(width\s*=\s*["']\s*([\d\.]+)(?:px)?\s*["'])", QRegularExpression::CaseInsensitiveOption);
            static QRegularExpression rxHeight(R"(height\s*=\s*["']\s*([\d\.]+)(?:px)?\s*["'])", QRegularExpression::CaseInsensitiveOption);
            auto matchW = rxWidth.match(headStr);
            auto matchH = rxHeight.match(headStr);
            if (matchW.hasMatch() && matchH.hasMatch()) {
                double w = matchW.captured(1).toDouble();
                double h = matchH.captured(1).toDouble();
                if (w > 0 && h > 0) return QSize(qRound(w), qRound(h));
            }
        }

        // 3. 保底后备逻辑：非标准 SVG 走 QSvgRenderer 解析
        QSvgRenderer renderer(filePath);
        if (renderer.isValid()) {
            QRectF vb = renderer.viewBoxF();
            QSize sz = vb.isValid() ? vb.size().toSize() : renderer.defaultSize();
            if (!sz.isEmpty()) return sz;
            return QSize(512, 512);
        }
    }

    // 5. 普通图像走 QImageReader
    QImageReader reader(filePath);
    if (!reader.canRead()) return QSize();
    return reader.size();
}

} // namespace QuarkMeta
