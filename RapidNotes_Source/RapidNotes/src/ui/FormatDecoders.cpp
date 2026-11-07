#include "FormatDecoders.h"
#include "WindowsShellThumbnailProvider.h"
#include "../core/CoreController.h"
#include <QUuid>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QMutex>
#include <QSemaphore>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QImageReader>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

extern "C" {
#include "tiffio.h"
}

struct TiffMemoryStream {
    const char* data;
    tmsize_t size;
    tmsize_t offset;
};

static tmsize_t tiffReadProc(thandle_t clientData, void* buf, tmsize_t size) {
    auto stream = reinterpret_cast<TiffMemoryStream*>(clientData);
    if (stream->offset + size > stream->size) {
        size = stream->size - stream->offset;
    }
    if (size > 0) {
        memcpy(buf, stream->data + stream->offset, size);
        stream->offset += size;
    }
    return size;
}

static tmsize_t tiffWriteProc(thandle_t, void*, tmsize_t) {
    return 0;
}

static toff_t tiffSeekProc(thandle_t clientData, toff_t off, int whence) {
    auto stream = reinterpret_cast<TiffMemoryStream*>(clientData);
    switch (whence) {
        case SEEK_SET: stream->offset = off; break;
        case SEEK_CUR: stream->offset += off; break;
        case SEEK_END: stream->offset = stream->size + off; break;
    }
    return stream->offset;
}

static int tiffCloseProc(thandle_t) {
    return 0;
}

static toff_t tiffSizeProc(thandle_t clientData) {
    return reinterpret_cast<TiffMemoryStream*>(clientData)->size;
}

static int tiffMapProc(thandle_t clientData, void** pbase, toff_t* psize) {
    auto stream = reinterpret_cast<TiffMemoryStream*>(clientData);
    *pbase = const_cast<char*>(stream->data);
    *psize = stream->size;
    return 1;
}

static void tiffUnmapProc(thandle_t, void*, toff_t) {
}

namespace QuarkMeta {

QImage FormatDecoders::decodeTiffMemorySafely(const QByteArray& tiffData, int maxMemoryMB) {
    TiffMemoryStream stream;
    stream.data = tiffData.constData();
    stream.size = tiffData.size();
    stream.offset = 0;

    TIFF* tif = TIFFClientOpen("MemoryTIFF", "r",
                               reinterpret_cast<thandle_t>(&stream),
                               tiffReadProc, tiffWriteProc,
                               tiffSeekProc, tiffCloseProc,
                               tiffSizeProc, tiffMapProc, tiffUnmapProc);
    if (!tif) {
        return QImage();
    }

    uint32_t width = 0, height = 0;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

    uint64_t requiredBytes = static_cast<uint64_t>(width) * height * 4;
    if (requiredBytes > static_cast<uint64_t>(maxMemoryMB) * 1024 * 1024 || width == 0 || height == 0) {
        TIFFClose(tif);
        return QImage();
    }

    QImage img(width, height, QImage::Format_RGBA8888);
    if (img.isNull()) {
        TIFFClose(tif);
        return QImage();
    }

    if (!TIFFReadRGBAImageOriented(tif, width, height, reinterpret_cast<uint32_t*>(img.bits()), ORIENTATION_TOPLEFT, 0)) {
        TIFFClose(tif);
        return QImage();
    }
    TIFFClose(tif);

    return img;
}

QImage FormatDecoders::extractPsdHeaderThumbnail(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return QImage();

    QByteArray header = file.read(26);
    if (header.size() < 26 || !header.startsWith("8BPS")) return QImage();

    quint32 colorModeLen = 0;
    {
        QByteArray lenBytes = file.read(4);
        if (lenBytes.size() < 4) return QImage();
        colorModeLen = (quint8(lenBytes[0]) << 24) | (quint8(lenBytes[1]) << 16) |
                       (quint8(lenBytes[2]) << 8) | quint8(lenBytes[3]);
    }
    file.seek(file.pos() + colorModeLen);

    QByteArray resLenBytes = file.read(4);
    if (resLenBytes.size() < 4) return QImage();
    quint32 resSectionLen = (quint8(resLenBytes[0]) << 24) | (quint8(resLenBytes[1]) << 16) |
                             (quint8(resLenBytes[2]) << 8) | quint8(resLenBytes[3]);

    qint64 resSectionEnd = file.pos() + resSectionLen;
    while (file.pos() < resSectionEnd) {
        QByteArray sig = file.read(4);
        if (sig != "8BIM") break;

        QByteArray idBytes = file.read(2);
        if (idBytes.size() < 2) break;
        quint16 resId = (quint8(idBytes[0]) << 8) | quint8(idBytes[1]);

        quint8 nameLen = 0;
        file.getChar(reinterpret_cast<char*>(&nameLen));
        file.seek(file.pos() + nameLen + ((nameLen % 2 == 0) ? 1 : 0));

        QByteArray dataLenBytes = file.read(4);
        if (dataLenBytes.size() < 4) break;
        quint32 dataLen = (quint8(dataLenBytes[0]) << 24) | (quint8(dataLenBytes[1]) << 16) |
                           (quint8(dataLenBytes[2]) << 8) | quint8(dataLenBytes[3]);

        if (resId == 0x040C) {
            if (dataLen < 28) break;
            file.seek(file.pos() + 28);
            QByteArray jpegData = file.read(dataLen - 28);
            QImage img;
            if (img.loadFromData(jpegData, "JPEG")) {
                return img;
            }
            break;
        }

        file.seek(file.pos() + dataLen + (dataLen % 2));
    }
    return QImage();
}

QImage FormatDecoders::extractAiEmbeddedFallback(const QString& filePath, int targetSize) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return QImage();

    QByteArray rawData = file.read(2 * 1024 * 1024);
    file.close();

    if (rawData.isEmpty() || CoreController::isShuttingDown()) return QImage();

    // 2. 内嵌缩略图块（%AI7/8/9/10_Thumbnail）
    int thumbHeaderIdx = rawData.indexOf("%AI7_Thumbnail:");
    if (thumbHeaderIdx == -1) thumbHeaderIdx = rawData.indexOf("%AI8_Thumbnail:");
    if (thumbHeaderIdx == -1) thumbHeaderIdx = rawData.indexOf("%AI9_Thumbnail:");
    if (thumbHeaderIdx == -1) thumbHeaderIdx = rawData.indexOf("%AI10_Thumbnail:");

    if (thumbHeaderIdx != -1) {
        int lineEnd = rawData.indexOf('\n', thumbHeaderIdx);
        if (lineEnd != -1) {
            QByteArray headerLine = rawData.mid(thumbHeaderIdx, lineEnd - thumbHeaderIdx);
            QString headerStr = QString::fromLatin1(headerLine);
            QStringList parts = headerStr.section(':', 1).trimmed().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                int width = parts[0].toInt();
                int height = parts[1].toInt();

                int blockEnd = rawData.indexOf("%AI7_ThumbnailEnd", lineEnd);
                if (blockEnd == -1) blockEnd = rawData.indexOf("%AI9_ThumbnailEnd", lineEnd);
                if (blockEnd == -1) blockEnd = rawData.indexOf("%%EndData", lineEnd);

                if (blockEnd != -1 && width > 0 && height > 0) {
                    QByteArray hexData;
                    hexData.reserve(blockEnd - lineEnd);

                    int cur = lineEnd;
                    while (cur < blockEnd) {
                        int nextLine = rawData.indexOf('\n', cur);
                        if (nextLine == -1) break;
                        QByteArray line = rawData.mid(cur, nextLine - cur).trimmed();
                        if (line.startsWith("%")) {
                            hexData.append(line.mid(1).trimmed());
                        }
                        cur = nextLine + 1;
                    }

                    QByteArray binaryData = QByteArray::fromHex(hexData);
                    const int paletteSize = 256 * 3;

                    if (binaryData.size() >= paletteSize + width * height) {
                        QList<QRgb> colorTable;
                        colorTable.reserve(256);
                        const uchar* palPtr = reinterpret_cast<const uchar*>(binaryData.constData());

                        for (int i = 0; i < 256; ++i) {
                            colorTable.append(qRgb(palPtr[i * 3 + 2], palPtr[i * 3 + 1], palPtr[i * 3]));
                        }

                        QImage img(width, height, QImage::Format_Indexed8);
                        img.setColorTable(colorTable);
                        const uchar* pixelPtr = palPtr + paletteSize;
                        for (int y = 0; y < height; ++y) {
                            memcpy(img.scanLine(y), pixelPtr + y * width, width);
                        }
                        if (!img.isNull()) {
                            return img.convertToFormat(QImage::Format_ARGB32);
                        }
                    }
                }
            }
        }
    }

    // 3. 内嵌 XMP 图（xmpGImg:image）
    int xmpStart = rawData.indexOf("<xmpGImg:image>");
    if (xmpStart != -1) {
        xmpStart += 15;
        int xmpEnd = rawData.indexOf("</xmpGImg:image>", xmpStart);
        if (xmpEnd != -1) {
            QByteArray base64Data = rawData.mid(xmpStart, xmpEnd - xmpStart).trimmed();
            base64Data.replace("\n", "").replace("\r", "").replace(" ", "");
            QByteArray jpgBytes = QByteArray::fromBase64(base64Data);
            QImage img;
            if (img.loadFromData(jpgBytes)) {
                return img;
            }
        }
    }

    // 4. Windows 系统缩略图（renderPdfAiFirstPage）
    QImage pdfRenderImg = renderPdfAiFirstPage(filePath, targetSize);
    if (!pdfRenderImg.isNull()) {
        return pdfRenderImg;
    }

    // 5. 内嵌 PNG
    int pngStart = rawData.indexOf("\x89PNG\r\n\x1a\n");
    if (pngStart != -1) {
        int pngEnd = rawData.indexOf("IEND", pngStart);
        if (pngEnd != -1) {
            QByteArray pngData = rawData.mid(pngStart, (pngEnd + 8) - pngStart);
            QImage img;
            if (img.loadFromData(pngData, "PNG") && img.width() >= 32) {
                return img;
            }
        }
    }

    // 6. 内嵌 JPG
    int jpgStart = rawData.indexOf("\xFF\xD8\xFF");
    if (jpgStart != -1) {
        int jpgEnd = rawData.indexOf("\xFF\xD9", jpgStart);
        if (jpgEnd != -1) {
            QByteArray jpgData = rawData.mid(jpgStart, (jpgEnd + 2) - jpgStart);
            QImage img;
            if (img.loadFromData(jpgData, "JPEG") && img.width() >= 32) {
                return img;
            }
        }
    }

    // 7. Windows 系统缩略图（WindowsShellThumbnailProvider，原有兜底）
    return WindowsShellThumbnailProvider::getShellThumbnail(filePath, targetSize);
}

QImage FormatDecoders::extractAiPreview(const QString& filePath, int targetSize, int customTimeoutMs, std::shared_ptr<CancellationToken> token) {
    // 1. Ghostscript (100 DPI) 优先
    QImage gsImg = renderGhostscriptSafely(filePath, targetSize, customTimeoutMs, token, 100);
    if (!gsImg.isNull()) {
        return gsImg;
    }
    // 2~7. 内嵌后备链
    return extractAiEmbeddedFallback(filePath, targetSize);
}

QImage FormatDecoders::extractAiQuickLook(const QString& filePath, int targetSize, int customTimeoutMs, std::shared_ptr<CancellationToken> token) {
    // 1. Ghostscript (144 DPI) 优先
    QImage gsImg = renderGhostscriptSafely(filePath, targetSize, customTimeoutMs, token, 144);
    if (!gsImg.isNull()) {
        return gsImg;
    }
    // 2~7. 内嵌后备链（避免重复触发 Ghostscript）
    return extractAiEmbeddedFallback(filePath, targetSize);
}

QImage FormatDecoders::extractEpsEmbeddedPreview(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QImage();
    }

    QByteArray header = file.read(30);
    if (header.size() < 30) {
        return QImage();
    }

    if (quint8(header[0]) == 0xC5 && quint8(header[1]) == 0xD0 &&
        quint8(header[2]) == 0xD3 && quint8(header[3]) == 0xC6) {

        quint32 tiffOffset = (quint8(header[20])) | (quint8(header[21]) << 8) |
                             (quint8(header[22]) << 16) | (quint8(header[23]) << 24);
        quint32 tiffLength = (quint8(header[24])) | (quint8(header[25]) << 8) |
                             (quint8(header[26]) << 16) | (quint8(header[27]) << 24);
        if (tiffOffset > 0 && tiffLength > 0) {
            file.seek(tiffOffset);
            QByteArray tiffData = file.read(tiffLength);
            QImage img = decodeTiffMemorySafely(tiffData);
            if (!img.isNull()) {
                return img;
            }
        }
    }

    file.seek(0);
    QTextStream in(&file);
    bool inPreview = false;
    QString hexData;
    int width = 0, height = 0;

    QRegularExpression rxSpaces("\\s+");

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("%%BeginPreview:")) {
            QStringList parts = line.split(rxSpaces, Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                width = parts[1].toInt();
                height = parts[2].toInt();
                inPreview = true;
            }
            continue;
        }
        if (line.startsWith("%%EndPreview")) {
            break;
        }
        if (inPreview) {
            if (line.startsWith("%")) {
                hexData.append(line.mid(1).trimmed());
            }
        }
    }

    if (!hexData.isEmpty() && width > 0 && height > 0) {
        QByteArray binaryData = QByteArray::fromHex(hexData.toLatin1());
        QImage img;
        if (img.loadFromData(binaryData)) {
            return img;
        }
    }

    return QImage();
}

QImage FormatDecoders::extractEpsThumbnail(const QString& filePath, int targetSize, int customTimeoutMs, std::shared_ptr<CancellationToken> token) {
    // 1. Ghostscript (100 DPI) 优先
    QImage gsImg = renderGhostscriptSafely(filePath, targetSize, customTimeoutMs, token, 100);
    if (!gsImg.isNull()) {
        return gsImg;
    }
    // 2. 内嵌预览（TIFF / %%BeginPreview）
    return extractEpsEmbeddedPreview(filePath);
}

QImage FormatDecoders::extractEpsQuickLook(const QString& filePath, int targetSize, int customTimeoutMs, std::shared_ptr<CancellationToken> token, bool* fromGhostscript) {
    if (fromGhostscript) *fromGhostscript = false;

    QImage gsImg = renderGhostscriptSafely(filePath, targetSize, customTimeoutMs, token, 144);
    if (!gsImg.isNull()) {
        if (fromGhostscript) *fromGhostscript = true;
        return gsImg;
    }
    return extractEpsEmbeddedPreview(filePath);
}

QImage FormatDecoders::extractEpsPreview(const QString& filePath, int targetSize, int customTimeoutMs, std::shared_ptr<CancellationToken> token) {
    return extractEpsThumbnail(filePath, targetSize, customTimeoutMs, token);
}

QString FormatDecoders::findGhostscriptExecutable() {
    static QMutex mutex;
    static QString cachedPath;
    static bool searched = false;

    QMutexLocker locker(&mutex);
    if (searched) return cachedPath;
    searched = true;

#ifdef Q_OS_WIN
    QString gs = QStandardPaths::findExecutable("gswin64c.exe");
    if (gs.isEmpty()) gs = QStandardPaths::findExecutable("gswin64.exe");
    if (gs.isEmpty()) gs = QStandardPaths::findExecutable("gs.exe");

    if (gs.isEmpty()) {
        QDir gsBase("C:/Program Files/gs");
        if (gsBase.exists()) {
            QStringList subDirs = gsBase.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::Reversed);
            for (const QString& sub : subDirs) {
                QString candidate = QString("C:/Program Files/gs/%1/bin/gswin64c.exe").arg(sub);
                if (QFile::exists(candidate)) { gs = candidate; break; }
                candidate = QString("C:/Program Files/gs/%1/bin/gswin64.exe").arg(sub);
                if (QFile::exists(candidate)) { gs = candidate; break; }
            }
        }
    }
    cachedPath = gs;
#else
    cachedPath = QStandardPaths::findExecutable("gs");
#endif
    return cachedPath;
}

static QSemaphore g_gsConcurrencyLimit(1);

QImage FormatDecoders::renderGhostscriptSafely(const QString& filePath, int targetSize, int customTimeoutMs, std::shared_ptr<CancellationToken> token, int dpi) {
    if ((token && token->isCanceled()) || CoreController::isShuttingDown()) return QImage();

    QString gsExec = findGhostscriptExecutable();
    if (gsExec.isEmpty()) {
        return QImage();
    }

    int acqWaitMs = (customTimeoutMs > 0) ? 5000 : 100;
    if (!g_gsConcurrencyLimit.tryAcquire(1, acqWaitMs)) {
        return QImage();
    }
    struct ReleaseGuard {
        QSemaphore& s;
        ~ReleaseGuard() { s.release(); }
    } guard{g_gsConcurrencyLimit};

    if (CoreController::isShuttingDown()) return QImage();

    int timeoutMs = 2000;
    if (customTimeoutMs > 0) {
        timeoutMs = customTimeoutMs;
    } else {
        QFileInfo fi(filePath);
        qint64 fileSizeMB = fi.size() / (1024 * 1024);
        if (fileSizeMB > 20) timeoutMs = 10000;
        else if (fileSizeMB > 5) timeoutMs = 8000;
    }

    QString tempPng = QDir::tempPath() + QString("/gs_thumb_%1_%2.png").arg(QUuid::createUuid().toString(QUuid::WithoutBraces)).arg(QString::number(qHash(filePath), 16));

    QString ext = QFileInfo(filePath).suffix().toLower();
    QString cropArg;
    if (ext == "ai" || ext == "pdf") {
        cropArg = "-dUseCropBox";
    } else if (ext == "eps") {
        cropArg = "-dEPSCrop";
    }

    QStringList args;
    args << "-dNOPAUSE"
         << "-dBATCH"
         << "-dSAFER"
         << "-sDEVICE=pngalpha"
         << QString("-r%1").arg(dpi)
         << "-dFirstPage=1"
         << "-dLastPage=1";
    if (!cropArg.isEmpty()) {
        args << cropArg;
    }
    args << QString("-sOutputFile=%1").arg(tempPng)
         << QDir::toNativeSeparators(filePath);

    QProcess process;
#ifdef Q_OS_WIN
    process.setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments* args) {
        args->flags |= CREATE_NO_WINDOW;
    });
#endif
    process.start(gsExec, args);

    qint64 elapsed = 0;
    bool finished = false;
    while (elapsed < timeoutMs) {
        if ((token && token->isCanceled()) || CoreController::isShuttingDown()) {
            if (process.state() == QProcess::Running) {
                process.kill();
                process.waitForFinished(200);
            }
            if (QFile::exists(tempPng)) QFile::remove(tempPng);
            return QImage();
        }
        if (process.state() == QProcess::NotRunning || process.waitForFinished(50)) {
            finished = true;
            break;
        }
        elapsed += 50;
    }

    if (finished && QFile::exists(tempPng)) {
        QImageReader reader(tempPng);
        reader.setAllocationLimit(512);
        QSize origSize = reader.size();
        if (origSize.isValid() && (origSize.width() > targetSize || origSize.height() > targetSize)) {
            reader.setScaledSize(origSize.scaled(targetSize, targetSize, Qt::KeepAspectRatio));
        }
        QImage img = reader.read();
        QFile::remove(tempPng);

        if (!img.isNull()) {
            return img;
        }
    } else if (!finished) {
        if (process.state() == QProcess::Running) {
            process.kill();
            process.waitForFinished(200);
        }
    }

    if (process.state() == QProcess::Running) {
        process.kill();
        process.waitForFinished(200);
    }

    if (QFile::exists(tempPng)) QFile::remove(tempPng);
    return QImage();
}

QImage FormatDecoders::renderPdfAiFirstPage(const QString& filePath, int targetSize) {
#ifdef Q_OS_WIN
    QImage img = WindowsShellThumbnailProvider::getShellThumbnail(filePath, targetSize);
    if (!img.isNull()) {
        return img;
    }
#else
    Q_UNUSED(filePath);
    Q_UNUSED(targetSize);
#endif
    return QImage();
}

} // namespace QuarkMeta
