#include "ThumbnailPipelineService.h"
#include "ColorPaletteEngine.h"
#include "DiskMediaExtractor.h"
#include <QImageReader>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QtConcurrent>
#include <QMutexLocker>
#include <QThread>

namespace QuarkMeta {

ThumbnailPipelineService& ThumbnailPipelineService::instance() {
    static ThumbnailPipelineService s_instance;
    return s_instance;
}

ThumbnailPipelineService::ThumbnailPipelineService(QObject* parent)
    : QObject(parent) {
    m_memoryCache.setMaxCost(kMaxMemoryCacheCount);
    m_decodePool.setMaxThreadCount(qMax(2, QThread::idealThreadCount() / 2));
}

QPixmap ThumbnailPipelineService::getFromMemoryCache(const QString& filePath, int targetSize) const {
    QString key = QString("%1@%2").arg(QDir::toNativeSeparators(filePath).toLower()).arg(targetSize);
    QMutexLocker locker(&m_cacheMutex);
    QPixmap* cached = m_memoryCache.object(key);
    if (cached && !cached->isNull()) {
        return *cached;
    }
    return QPixmap();
}

void ThumbnailPipelineService::clearMemoryCache() {
    QMutexLocker locker(&m_cacheMutex);
    m_memoryCache.clear();
}

void ThumbnailPipelineService::incrementGeneration() {
    m_currentGeneration.fetch_add(1, std::memory_order_relaxed);
    QMutexLocker locker(&m_cacheMutex);
    if (m_currentToken) {
        m_currentToken->cancel();
        m_currentToken.reset();
    }
}

std::shared_ptr<CancellationToken> ThumbnailPipelineService::currentToken() const {
    QMutexLocker locker(&m_cacheMutex);
    return m_currentToken;
}

void ThumbnailPipelineService::cancelAll() {
    incrementGeneration();
}

void ThumbnailPipelineService::loadBatchAsync(const QStringList& filePaths,
                                              int targetSize,
                                              std::function<void(const QString& path, const QPixmap& pixmap)> onSingleLoaded) {
    if (filePaths.isEmpty()) return;

    std::shared_ptr<CancellationToken> token;
    {
        QMutexLocker locker(&m_cacheMutex);
        if (!m_currentToken || m_currentToken->isCanceled()) {
            m_currentToken = std::make_shared<CancellationToken>();
        }
        token = m_currentToken;
    }

    uint64_t taskGen = m_currentGeneration.load(std::memory_order_relaxed);

    QStringList pathsToFetch;
    for (const QString& path : filePaths) {
        QPixmap memPix = getFromMemoryCache(path, targetSize);
        if (!memPix.isNull()) {
            if (onSingleLoaded) onSingleLoaded(path, memPix);
        } else {
            pathsToFetch << path;
        }
    }

    if (pathsToFetch.isEmpty()) return;

    (void)QtConcurrent::run(&m_decodePool, [this, pathsToFetch, targetSize, taskGen, token, onSingleLoaded]() {
        for (const QString& path : pathsToFetch) {
            if (m_currentGeneration.load(std::memory_order_relaxed) != taskGen) {
                return;
            }

            if (token && token->isCanceled()) return;

            // 唯一入口：读缓存 / 失败拦截 / 解码 / 写缓存 / 尺寸与失败标记 全部在 DiskMediaExtractor 内完成
            QImage finalImg = DiskMediaExtractor::getCapsuleExtractResult(path, DiskMediaExtractor::kThumbSize, token).thumbnail512;

            if (m_currentGeneration.load(std::memory_order_relaxed) != taskGen) {
                return;
            }

            QMetaObject::invokeMethod(qApp, [this, path, targetSize, finalImg, taskGen, onSingleLoaded]() {
                if (!onSingleLoaded) return;

                if (m_currentGeneration.load(std::memory_order_relaxed) == taskGen && !finalImg.isNull()) {
                    QPixmap pix = QPixmap::fromImage(finalImg);
                    if (!pix.isNull()) {
                        QString key = QString("%1@%2").arg(QDir::toNativeSeparators(path).toLower()).arg(targetSize);
                        {
                            QMutexLocker locker(&m_cacheMutex);
                            m_memoryCache.insert(key, new QPixmap(pix), 1);
                        }
                        onSingleLoaded(path, pix);
                    } else {
                        onSingleLoaded(path, QPixmap());
                    }
                } else {
                    onSingleLoaded(path, QPixmap());
                }
            }, Qt::QueuedConnection);
        }
    });
}

} // namespace QuarkMeta
