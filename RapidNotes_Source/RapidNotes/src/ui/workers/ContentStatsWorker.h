#pragma once

#include <QObject>
#include <vector>
#include <atomic>
#include <mutex>
#include <QString>
#include <QSet>
#include "../ScanStats.h"
#include "../../core/ItemRecord.h"

namespace QuarkMeta {

class ContentStatsWorker : public QObject {
    Q_OBJECT
public:
    explicit ContentStatsWorker(QObject* parent = nullptr);
    ~ContentStatsWorker() override = default;

    static ScanStats calculateStats(const std::vector<ItemRecord>& records, bool showHidden);
    void processAsync(std::vector<ItemRecord> records, bool showHidden);

    bool isSettled() const { return m_pendingCount.load() == 0; }

signals:
    void statsReady(const QuarkMeta::ScanStats& stats);
    void settled();

private:
    std::atomic<uint64_t> m_currentReqId{0};
    std::atomic<int> m_pendingCount{0};

    // 文件集合签名及重复文件检测结果缓存（用 std::mutex 保护）
    mutable std::mutex m_cacheMutex;
    QString m_lastSignature;
    QSet<QString> m_cachedDuplicatePaths;
};

} // namespace QuarkMeta
