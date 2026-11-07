#include "ContentStatsWorker.h"
#include "../UiHelper.h"
#include "../../meta/DuplicateDetectorService.h"

#include <QDateTime>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QPointer>
#include <QDebug>
#include <cmath>

namespace QuarkMeta {

ContentStatsWorker::ContentStatsWorker(QObject* parent) : QObject(parent) {}

ScanStats ContentStatsWorker::calculateStats(const std::vector<ItemRecord>& records, bool showHidden) {
    ScanStats stats;
    if (records.empty()) return stats;

    for (const auto& record : records) {
        if (record.isHidden && !showHidden) continue;

        stats.ratingCounts[record.rating]++;
        stats.colorCounts[UiHelper::normalizeColorHex(record.manualColor)]++;

        if (record.ctime > 0) {
            QString cDateStr = QDateTime::fromMSecsSinceEpoch(record.ctime).date().toString("dd-MM-yyyy");
            if (!cDateStr.isEmpty()) stats.createDateCounts[cDateStr]++;
        }
        if (record.mtime > 0) {
            QString mDateStr = QDateTime::fromMSecsSinceEpoch(record.mtime).date().toString("dd-MM-yyyy");
            if (!mDateStr.isEmpty()) stats.modifyDateCounts[mDateStr]++;
        }

        if (!record.url.isEmpty()) stats.hasLinkCount++; else stats.noLinkCount++;
        if (!record.note.isEmpty()) stats.hasNoteCount++; else stats.noNoteCount++;
        if (!record.tags.isEmpty()) stats.hasTagCount++; else stats.noTagCount++;

        if (record.isDir) {
            stats.typeCounts["folder"]++;
            if (record.isEmpty) stats.emptyFolderCount++;
        } else {
            stats.typeCounts["file"]++;
            stats.typeCounts[record.suffix.toUpper()]++;

            if (record.width > 0 && record.height > 0) {
                double r = static_cast<double>(record.width) / record.height;
                if (record.width > record.height) stats.ratioHorizontalCount++;
                if (record.height > record.width) stats.ratioVerticalCount++;
                if (std::abs(r - 1.0) <= 0.05) stats.ratioSquareCount++;
                if (std::abs(r - 1.77) <= 0.05) stats.ratio169Count++;
            }

            if (ColorPaletteEngine::isGraphicsFile(record.suffix.toLower())) {
                if (record.thumbnailState == ItemRecord::ThumbnailState::Ready) {
                    stats.hasThumbnailCount++;
                } else {
                    stats.noThumbnailCount++;
                }
            }
        }
    }

    return stats;
}

void ContentStatsWorker::processAsync(std::vector<ItemRecord> records, bool showHidden) {
    uint64_t reqId = ++m_currentReqId;
    m_pendingCount++;

    if (records.empty()) {
        {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            m_lastSignature.clear();
            m_cachedDuplicatePaths.clear();
        }
        emit statsReady(ScanStats());
        if (--m_pendingCount == 0) {
            emit settled();
        }
        return;
    }

    // 生成当前非文件夹记录的集合签名 (path + size + mtime)
    QString currentSignature;
    currentSignature.reserve(records.size() * 32);
    for (const auto& rec : records) {
        if (!rec.isDir) {
            currentSignature.append(rec.path).append('|')
                            .append(QString::number(rec.size)).append('|')
                            .append(QString::number(rec.mtime)).append(';');
        }
    }

    bool signatureMatches = false;
    QSet<QString> cachedDuplicates;
    {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        if (!currentSignature.isEmpty() && currentSignature == m_lastSignature) {
            signatureMatches = true;
            cachedDuplicates = m_cachedDuplicatePaths;
        }
    }

    QPointer<ContentStatsWorker> weakThis(this);
    (void)QtConcurrent::run([weakThis, records = std::move(records), showHidden, reqId, signatureMatches, cachedDuplicates, currentSignature]() mutable {
        ScanStats stats = ContentStatsWorker::calculateStats(records, showHidden);

        if (signatureMatches) {
            stats.duplicatePaths = cachedDuplicates;
            stats.duplicateCount = static_cast<int>(stats.duplicatePaths.size());
        } else {
            stats.duplicatePaths = DuplicateDetectorService::findDuplicatePaths(records);
            stats.duplicateCount = static_cast<int>(stats.duplicatePaths.size());
        }

        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis, stats = std::move(stats), reqId, currentSignature, signatureMatches]() mutable {
            if (weakThis) {
                if (weakThis->m_currentReqId == reqId) {
                    if (!signatureMatches) {
                        std::lock_guard<std::mutex> lock(weakThis->m_cacheMutex);
                        weakThis->m_lastSignature = currentSignature;
                        weakThis->m_cachedDuplicatePaths = stats.duplicatePaths;
                    }
                    emit weakThis->statsReady(stats);
                }
                if (--weakThis->m_pendingCount == 0) {
                    emit weakThis->settled();
                }
            }
        });
    });
}

} // namespace QuarkMeta
