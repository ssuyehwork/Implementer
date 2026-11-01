#include "DuplicateDetectorService.h"
#include "MetadataManager.h"
#include "../util/DiskMediaExtractor.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QCryptographicHash>
#include <QImageReader>
#include <QHash>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

namespace QuarkMeta {

QString DuplicateDetectorService::computeFastHash(const QString& filePath, qint64 fileSize) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return QString();

    if (fileSize < 0) {
        fileSize = file.size();
    }

    if (fileSize <= 0) return QString();

    QCryptographicHash hash(QCryptographicHash::Sha256);
    constexpr qint64 chunkSize = 64 * 1024; // 首尾 64KB

    if (fileSize <= chunkSize * 2) {
        if (!hash.addData(&file)) return QString();
    } else {
        QByteArray head = file.read(chunkSize);
        hash.addData(head);

        if (file.seek(fileSize - chunkSize)) {
            QByteArray tail = file.read(chunkSize);
            hash.addData(tail);
        }
    }
    file.close();
    return QString(hash.result().toHex()).toLower();
}

QString DuplicateDetectorService::computeFullSha256(const QString& filePath, const std::atomic<bool>* cancelToken) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return QString();

    QCryptographicHash hash(QCryptographicHash::Sha256);
    constexpr qint64 kBufferSize = 256 * 1024; // 256KB 缓冲区
    QByteArray buffer;
    buffer.resize(kBufferSize);

    while (!file.atEnd()) {
        if (cancelToken && cancelToken->load()) {
            file.close();
            return QString();
        }
        qint64 bytesRead = file.read(buffer.data(), kBufferSize);
        if (bytesRead <= 0) break;
        hash.addData(QByteArrayView(buffer.constData(), bytesRead));
    }
    file.close();

    if (cancelToken && cancelToken->load()) {
        return QString();
    }
    return QString(hash.result().toHex()).toLower();
}

QSet<QString> DuplicateDetectorService::findDuplicatePaths(const std::vector<ItemRecord>& records) {
    QSet<QString> duplicatePaths;
    if (records.size() < 2) return duplicatePaths;

    // 1. 一阶：按文件大小聚合（零 I/O）
    std::unordered_map<long long, std::vector<const ItemRecord*>> sizeBuckets;
    for (const auto& rec : records) {
        if (rec.isDir || rec.size <= 0) continue;
        sizeBuckets[rec.size].push_back(&rec);
    }

    // 2. 二阶与三阶：仅对大小相同的文件计算二进制哈希
    for (const auto& [fileSize, list] : sizeBuckets) {
        if (list.size() < 2) continue; // 大小唯一，绝对不重复

        // 二阶：FastHash 聚合
        QHash<QString, std::vector<const ItemRecord*>> fastHashBuckets;
        for (const auto* rec : list) {
            QString fastHash = computeFastHash(rec->path, rec->size);
            if (!fastHash.isEmpty()) {
                fastHashBuckets[fastHash].push_back(rec);
            }
        }

        // 三阶：全量 SHA-256 严谨决胜
        for (auto it = fastHashBuckets.begin(); it != fastHashBuckets.end(); ++it) {
            const auto& fastList = it.value();
            if (fastList.size() < 2) continue;

            if (fileSize <= 128 * 1024) {
                for (const auto* r : fastList) {
                    duplicatePaths.insert(r->path);
                }
                continue;
            }

            QHash<QString, std::vector<const ItemRecord*>> fullShaBuckets;
            for (const auto* r : fastList) {
                QString fullSha = !r->sha256.isEmpty() ? r->sha256.toLower() : computeFullSha256(r->path);
                if (!fullSha.isEmpty()) {
                    fullShaBuckets[fullSha].push_back(r);
                }
            }

            for (auto shaIt = fullShaBuckets.begin(); shaIt != fullShaBuckets.end(); ++shaIt) {
                const auto& fullList = shaIt.value();
                if (fullList.size() >= 2) {
                    for (const auto* r : fullList) {
                        duplicatePaths.insert(r->path);
                    }
                }
            }
        }
    }

    return duplicatePaths;
}

std::vector<DuplicateConflictGroup> DuplicateDetectorService::detectDuplicates(const QStringList& newImportedPaths) {
    std::vector<DuplicateConflictGroup> conflicts;
    if (newImportedPaths.isEmpty()) return conflicts;

    std::unordered_map<long long, std::vector<std::pair<std::wstring, RuntimeMeta>>> sizeIndexMap;

    MetadataManager::instance().forEachCachedItem([&](const std::wstring& existPathW, const RuntimeMeta& meta) {
        if (meta.isFolder || meta.fileSize <= 0) return;
        sizeIndexMap[meta.fileSize].push_back({existPathW, meta});
    });

    for (const QString& newPath : newImportedPaths) {
        QFileInfo newInfo(newPath);
        if (!newInfo.exists() || newInfo.isDir()) continue;

        qint64 size = newInfo.size();
        if (size <= 0) continue;

        QString fileName = newInfo.fileName();
        QString newFastHash;
        QString newFullSha256;

        auto sizeIt = sizeIndexMap.find(size);
        if (sizeIt != sizeIndexMap.end()) {
            for (const auto& pair : sizeIt->second) {
                const std::wstring& existPathW = pair.first;
                const RuntimeMeta& meta = pair.second;
                QString existPathStr = QString::fromStdWString(existPathW);
                if (existPathStr == newPath) continue;

                if (newFastHash.isEmpty()) {
                    newFastHash = computeFastHash(newPath, size);
                }
                if (newFastHash.isEmpty()) break;

                QString existFastHash = computeFastHash(existPathStr, meta.fileSize);
                if (existFastHash.isEmpty() || existFastHash != newFastHash) {
                    continue;
                }

                if (newFullSha256.isEmpty()) {
                    newFullSha256 = computeFullSha256(newPath);
                }

                QString existSha = !meta.sha256.empty() ? QString::fromStdString(meta.sha256).toLower() : computeFullSha256(existPathStr);

                if (!existSha.isEmpty() && existSha == newFullSha256) {
                    DuplicateConflictGroup group;
                    group.existingItem.path = existPathStr;
                    group.existingItem.filename = QFileInfo(existPathStr).fileName();
                    group.existingItem.width = meta.width;
                    group.existingItem.height = meta.height;
                    group.existingItem.size = meta.fileSize;
                    group.existingItem.tagHint = meta.tags.isEmpty() ? "" : meta.tags.first();
                    group.existingItem.thumbnail = DiskMediaExtractor::getCapsuleThumbnailReadOnly(existPathStr);

                    group.newItem.path = newPath;
                    group.newItem.filename = fileName;
                    group.newItem.width = meta.width; 
                    group.newItem.height = meta.height;
                    group.newItem.size = size;
                    group.newItem.thumbnail = DiskMediaExtractor::getCapsuleThumbnail(newPath, DiskMediaExtractor::kThumbSize);

                    conflicts.push_back(group);
                    break;
                }
            }
        }
    }

    return conflicts;
}

} // namespace QuarkMeta
