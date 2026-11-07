#include "DiskMediaExtractor.h"
#include "../ui/ImageDecoderFacade.h"
#include "../meta/QuarkMetaJsonStore.h"
#include "../meta/MetadataDefs.h"
#include "../core/AppConfig.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QUuid>
#include <chrono>
#include <condition_variable>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

std::mutex DiskMediaExtractor::s_qtGuiMutex;
std::mutex DiskMediaExtractor::s_jsonSaveMutex;
QMutex DiskMediaExtractor::s_pendingMutex;
QHash<QString, QSet<QString>> DiskMediaExtractor::s_pendingFailures;
QHash<QString, QHash<QString, QSize>> DiskMediaExtractor::s_pendingSizes;
QMutex DiskMediaExtractor::s_failureMutex;
QHash<QString, QSet<QString>> DiskMediaExtractor::s_failedNames;
QSet<QString> DiskMediaExtractor::s_failureLoadedFolders;

namespace {

bool isTaskCanceled(const std::shared_ptr<CancellationToken>& token) {
    return (token && token->isCanceled()) || CoreController::isShuttingDown();
}

// 同一路径同一时刻只允许一个线程解码；其他线程等待（可被取消），等到后直接命中缓存
std::mutex g_inflightMutex;
std::condition_variable g_inflightCv;
QSet<QString> g_inflightKeys;

class InflightGuard {
public:
    InflightGuard(const QString& path, const std::shared_ptr<CancellationToken>& token)
        : m_key(QDir::toNativeSeparators(path).toLower()) {
        std::unique_lock<std::mutex> lock(g_inflightMutex);
        while (g_inflightKeys.contains(m_key)) {
            if (isTaskCanceled(token)) return;
            g_inflightCv.wait_for(lock, std::chrono::milliseconds(50));
        }
        g_inflightKeys.insert(m_key);
        m_acquired = true;
    }
    ~InflightGuard() {
        if (!m_acquired) return;
        {
            std::lock_guard<std::mutex> lock(g_inflightMutex);
            g_inflightKeys.remove(m_key);
        }
        g_inflightCv.notify_all();
    }
    InflightGuard(const InflightGuard&) = delete;
    InflightGuard& operator=(const InflightGuard&) = delete;

    bool acquired() const { return m_acquired; }

private:
    QString m_key;
    bool m_acquired = false;
};

bool fetchPhysicalFileId(const QString& filePath, uint32_t& outVol, uint64_t& outFrn) {
#ifdef Q_OS_WIN
    std::wstring wPath = QDir::toNativeSeparators(filePath).toStdWString();
    HANDLE hFile = CreateFileW(wPath.c_str(), FILE_READ_ATTRIBUTES,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    BY_HANDLE_FILE_INFORMATION info;
    if (GetFileInformationByHandle(hFile, &info)) {
        outVol = info.dwVolumeSerialNumber;
        outFrn = (static_cast<uint64_t>(info.nFileIndexHigh) << 32) | info.nFileIndexLow;
        CloseHandle(hFile);
        return true;
    }
    CloseHandle(hFile);
    return false;
#else
    Q_UNUSED(filePath);
    Q_UNUSED(outVol);
    Q_UNUSED(outFrn);
    return false;
#endif
}

} // namespace

// ============================================================================
// 失败标记与元数据延迟落盘
// ============================================================================

bool DiskMediaExtractor::isMarkedFailed(const QString& folderPath, const QString& fileName) {
    {
        QMutexLocker locker(&s_failureMutex);
        if (s_failureLoadedFolders.contains(folderPath)) {
            return s_failedNames.value(folderPath).contains(fileName);
        }
    }

    // 该文件夹首次查询：统一通过 Store 内存一致性接口读取
    auto itemsMap = QuarkMetaJsonStore::instance().readFolderMeta(folderPath.toStdWString());
    QSet<QString> persisted;
    for (const auto& kv : itemsMap) {
        if (kv.second.thumbStatus == 1) {
            persisted.insert(QString::fromStdWString(kv.first));
        }
    }

    QMutexLocker locker(&s_failureMutex);
    s_failedNames[folderPath].unite(persisted);
    s_failureLoadedFolders.insert(folderPath);
    return s_failedNames.value(folderPath).contains(fileName);
}

void DiskMediaExtractor::scheduleFailureMark(const QString& folderPath, const QString& fileName) {
    {
        QMutexLocker locker(&s_pendingMutex);
        s_pendingFailures[folderPath].insert(fileName);
    }
    QMutexLocker locker(&s_failureMutex);
    s_failedNames[folderPath].insert(fileName);
}

void DiskMediaExtractor::scheduleSizeUpdate(const QString& folderPath, const QString& fileName, const QSize& size) {
    QMutexLocker locker(&s_pendingMutex);
    s_pendingSizes[folderPath].insert(fileName, size);
}

void DiskMediaExtractor::flushPendingUpdates() {
    QHash<QString, QSet<QString>> failures;
    QHash<QString, QHash<QString, QSize>> sizes;
    {
        QMutexLocker locker(&s_pendingMutex);
        if (s_pendingFailures.isEmpty() && s_pendingSizes.isEmpty()) return;
        failures = s_pendingFailures;
        sizes = s_pendingSizes;
        s_pendingFailures.clear();
        s_pendingSizes.clear();
    }

    QSet<QString> folders;
    for (auto it = failures.constBegin(); it != failures.constEnd(); ++it) folders.insert(it.key());
    for (auto it = sizes.constBegin(); it != sizes.constEnd(); ++it) folders.insert(it.key());

    for (const QString& folder : folders) {
        std::vector<std::pair<std::wstring, std::function<void(ItemMeta&)>>> batchUpdaters;

        for (const QString& fileName : failures.value(folder)) {
            batchUpdaters.emplace_back(fileName.toStdWString(), [](ItemMeta& meta) {
                meta.thumbStatus = 1;
            });
        }

        const QHash<QString, QSize> folderSizes = sizes.value(folder);
        for (auto it = folderSizes.constBegin(); it != folderSizes.constEnd(); ++it) {
            QSize sz = it.value();
            batchUpdaters.emplace_back(it.key().toStdWString(), [sz](ItemMeta& meta) {
                meta.width = sz.width();
                meta.height = sz.height();
            });
        }

        // 🚀【复用 Step 1 批量接口】：按目录单次加锁单次标脏，消除逐文件锁竞争
        if (!batchUpdaters.empty()) {
            QuarkMetaJsonStore::instance().updateFolderItemsBatch(folder.toStdWString(), batchUpdaters);
        }
    }
}

// ============================================================================
// 缓存路径（按文件 ID，文件改名/移动后仍命中；取不到文件 ID 时退回路径哈希）
// 路径函数是纯计算，不创建目录；目录在写缓存时创建。
// ============================================================================

QString DiskMediaExtractor::getDiskThumbCachePathByFileId(uint32_t volSerial, uint64_t fileId) {
    QString volStr = QString("%1").arg(volSerial, 8, 16, QChar('0')).toUpper();
    QString bucket = QString("%1").arg((fileId >> 8) & 0xFF, 2, 16, QChar('0')).toUpper();
    QString fileKey = QString("%1.png").arg(fileId, 16, 16, QChar('0')).toUpper();

    QString cacheDir = AppConfig::instance().getGlobalCacheDir() + "/disk_thumbs/" + volStr + "/" + bucket;
    return cacheDir + "/" + fileKey;
}

QString DiskMediaExtractor::getDiskThumbCachePath(const QString& filePath) {
    uint32_t vol = 0;
    uint64_t frn = 0;
    if (fetchPhysicalFileId(filePath, vol, frn)) {
        return getDiskThumbCachePathByFileId(vol, frn);
    }
    quint64 h = qHash(QDir::toNativeSeparators(filePath).toLower(), 0);
    QString bucket = QString("%1").arg((h >> 32) & 0xFF, 2, 16, QChar('0'));
    QString fileKey = QString("%1.png").arg(h, 16, 16, QChar('0'));
    return AppConfig::instance().getGlobalCacheDir() + "/disk_thumbs/fallback/" + bucket + "/" + fileKey;
}

// ============================================================================
// 缓存读写
// ============================================================================

bool DiskMediaExtractor::saveDiskThumbnail(const QString& filePath, const QImage& img) {
    if (img.isNull()) return false;

    // 缓存只存 kThumbSize 一档：超过则缩小，不放大
    QImage out = img;
    if (out.width() > kThumbSize || out.height() > kThumbSize) {
        out = out.scaled(kThumbSize, kThumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    const QString finalPath = getDiskThumbCachePath(filePath);
    QDir().mkpath(QFileInfo(finalPath).absolutePath());

    // 原子写：先写临时文件，再整体替换，读取方永远读不到写了一半的文件
    const QString tmpPath = finalPath + QStringLiteral(".") + QUuid::createUuid().toString(QUuid::WithoutBraces) + QStringLiteral(".tmp");
    if (!out.save(tmpPath, "PNG")) {
        QFile::remove(tmpPath);
        return false;
    }

#ifdef Q_OS_WIN
    const bool ok = MoveFileExW(QDir::toNativeSeparators(tmpPath).toStdWString().c_str(),
                                QDir::toNativeSeparators(finalPath).toStdWString().c_str(),
                                MOVEFILE_REPLACE_EXISTING) != 0;
#else
    QFile::remove(finalPath);
    const bool ok = QFile::rename(tmpPath, finalPath);
#endif
    if (!ok) {
        QFile::remove(tmpPath);
    }
    return ok;
}

QImage DiskMediaExtractor::getCapsuleThumbnailReadOnly(const QString& filePath) {
    QImage img;
    if (!img.load(getDiskThumbCachePath(filePath))) {
        return QImage();
    }
    // 旧版本遗留的大尺寸缓存读出时统一缩到 kThumbSize（不回写）
    if (img.width() > kThumbSize || img.height() > kThumbSize) {
        img = img.scaled(kThumbSize, kThumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    return img;
}

void DiskMediaExtractor::roamThumbnailCacheBatch(const QList<QPair<QString, QString>>& pairs, bool isMove) {
    if (pairs.isEmpty()) return;

    for (const auto& pair : pairs) {
        roamThumbnailCache(pair.first, pair.second, isMove);
    }
}

void DiskMediaExtractor::roamThumbnailCache(const QString& oldFilePath, const QString& newFilePath, bool isMove) {
    QString oldThumbPath = getDiskThumbCachePath(oldFilePath);
    QString newThumbPath = getDiskThumbCachePath(newFilePath);

    // 按文件 ID 命名时，同卷移动/改向前路径相同，无需处理（否则会把缓存自己删掉）
    if (oldThumbPath == newThumbPath) return;
    if (!QFile::exists(oldThumbPath)) return;

    // 确保目标缓存目录存在
    QDir().mkpath(QFileInfo(newThumbPath).absolutePath());

    if (isMove) {
        // 移动：原子转移
        if (QFile::exists(newThumbPath)) QFile::remove(newThumbPath);
        QFile::rename(oldThumbPath, newThumbPath);
    } else {
        // 复制：克隆缓存文件，确保新目录瞬间秒开缩略图
        if (QFile::exists(newThumbPath)) QFile::remove(newThumbPath);
        QFile::copy(oldThumbPath, newThumbPath);
    }
}

QSize DiskMediaExtractor::fastExtractImageSize(const QString& filePath) {
    return ImageDecoderFacade::readImageDimensions(filePath);
}

// ============================================================================
// 档位①：自动提取
// 流程：读缓存 → 失败标记拦截 → 同路径单飞 → 解码 → 写缓存 → 尺寸延迟落盘 / 失败写 1
// ============================================================================

DiskMediaExtractor::ExtractResult DiskMediaExtractor::getCapsuleExtractResult(const QString& filePath, int size, std::shared_ptr<CancellationToken> token) {
    ExtractResult res;
    if (isTaskCanceled(token)) return res;

    // 1. 缓存命中：免解码直接返回
    res.thumbnail512 = getCapsuleThumbnailReadOnly(filePath);
    if (!res.thumbnail512.isNull()) {
        res.isValid = true;
        return res;
    }

    QFileInfo fi(filePath);
    const QString parentDir = QDir::toNativeSeparators(fi.absolutePath());
    const QString fileName = fi.fileName();

    // 2. 失败标记拦截：thumb_status == 1 是终态，自动提取不再重试，只能靠"重新生成缩略图"恢复
    if (isMarkedFailed(parentDir, fileName)) return res;

    // 3. 同路径单飞：别的线程正在解码同一个文件时等待它
    InflightGuard guard(filePath, token);
    if (!guard.acquired()) return res;

    // 等待期间别的线程可能已经生成好
    res.thumbnail512 = getCapsuleThumbnailReadOnly(filePath);
    if (!res.thumbnail512.isNull()) {
        res.isValid = true;
        return res;
    }

    if (isTaskCanceled(token)) return res;

    // 4. 解码（自动档）
    DecodedMediaResult dec = ImageDecoderFacade::decodeSinglePass(filePath, size, ImageDecoderFacade::ExtractMode::Auto, token);
    if (dec.isValid && !dec.thumbnail512.isNull()) {
        res.originalSize = dec.originalSize;
        res.thumbnail512 = dec.thumbnail512;
        res.isValid = true;

        saveDiskThumbnail(filePath, dec.thumbnail512);

        if (res.originalSize.isValid() && res.originalSize.width() > 0) {
            scheduleSizeUpdate(parentDir, fileName, res.originalSize);
        }
        return res;
    }

    // 5. 解码失败：非取消情况下入队合并落盘 thumb_status = 1
    if (!isTaskCanceled(token)) {
        scheduleFailureMark(parentDir, fileName);
    }
    return res;
}

QImage DiskMediaExtractor::getCapsuleThumbnail(const QString& filePath, int size, std::shared_ptr<CancellationToken> token) {
    ExtractResult res = getCapsuleExtractResult(filePath, size, token);
    return res.thumbnail512;
}

// ============================================================================
// 档位②：重新生成缩略图
// 输出与自动提取一致（同一缓存、同一尺寸），超时放长；成功后把 thumb_status 改回 0
// ============================================================================

QImage DiskMediaExtractor::forceExtractDeepThumbnail(const QString& filePath, int size, std::shared_ptr<CancellationToken> token) {
    InflightGuard guard(filePath, token);
    if (!guard.acquired()) return QImage();

    DecodedMediaResult dec = ImageDecoderFacade::decodeSinglePass(filePath, size, ImageDecoderFacade::ExtractMode::Regenerate, token);
    if (!dec.isValid || dec.thumbnail512.isNull()) {
        return QImage();
    }

    saveDiskThumbnail(filePath, dec.thumbnail512);

    // 重置失败标记 thumb_status，并同步分辨率尺寸到 .QuarkMeta.json
    QFileInfo fi(filePath);
    const QString parentDir = QDir::toNativeSeparators(fi.absolutePath());
    const QString fileName = fi.fileName();
    const std::wstring wFileName = fileName.toStdWString();

    QuarkMetaJsonStore::instance().updateItemMeta(filePath.toStdWString(), [dec](ItemMeta& fileMeta) {
        fileMeta.thumbStatus = 0;
        if (dec.originalSize.isValid() && dec.originalSize.width() > 0) {
            fileMeta.width = dec.originalSize.width();
            fileMeta.height = dec.originalSize.height();
        }
    });

    // 内存失败名单与待落盘队列同步清除，避免旧的 1 再次拦截或被延迟写回
    {
        QMutexLocker locker(&s_failureMutex);
        auto it = s_failedNames.find(parentDir);
        if (it != s_failedNames.end()) it->remove(fileName);
    }
    {
        QMutexLocker locker(&s_pendingMutex);
        auto it = s_pendingFailures.find(parentDir);
        if (it != s_pendingFailures.end()) it->remove(fileName);
    }

    return dec.thumbnail512;
}

} // namespace QuarkMeta
