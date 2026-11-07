#include "StatisticsService.h"
#include "DatabaseManager.h"
#include "MetadataManager.h"
#include "../core/VolumeOnlineManager.h"
#include <QThreadPool>
#include <QRunnable>
#include <QCoreApplication>

namespace QuarkMeta {

class RecountTask : public QRunnable {
public:
    RecountTask(std::function<void(const StatisticsSnapshot&)> callback)
        : m_callback(callback) {}

    void run() override {
        StatisticsSnapshot snapshot = StatisticsService::instance().computeSnapshotFromDb();
        QMetaObject::invokeMethod(&StatisticsService::instance(), [snapshot, cb = m_callback]() {
            if (cb) cb(snapshot);
            emit StatisticsService::instance().statisticsUpdated(snapshot);
        });
    }

private:
    std::function<void(const StatisticsSnapshot&)> m_callback;
};

StatisticsService& StatisticsService::instance() {
    static StatisticsService inst;
    return inst;
}

StatisticsService::StatisticsService(QObject* parent)
    : QObject(parent) {
    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    connect(m_debounceTimer, &QTimer::timeout, this, [this]() {
        requestFullRecountAsync();
    });

    connect(&VolumeOnlineManager::instance(), &VolumeOnlineManager::volumeStateChanged,
            this, [this](const QString& driveLetter, bool isOnline) {
        Q_UNUSED(driveLetter);
        Q_UNUSED(isOnline);
        if (m_debounceTimer) {
            m_debounceTimer->start(300); // 300ms 防抖出账
        }
    });
}

StatisticsSnapshot StatisticsService::getCachedSnapshot() const {
    std::lock_guard<std::mutex> lock(m_snapshotMutex);
    return m_cachedSnapshot;
}

void StatisticsService::requestFullRecountAsync(std::function<void(const StatisticsSnapshot&)> callback) {
    QThreadPool::globalInstance()->start(new RecountTask(callback));
}

void StatisticsService::notifyAssetAdded(bool hasTags) {
    m_totalCount.fetch_add(1);
    if (!hasTags) {
        m_untaggedCount.fetch_add(1);
    }

    std::lock_guard<std::mutex> lock(m_snapshotMutex);
    m_cachedSnapshot.systemCounts["all"] = m_totalCount.load();
    m_cachedSnapshot.systemCounts["untagged"] = m_untaggedCount.load();

    emit statisticsUpdated(m_cachedSnapshot);
}

void StatisticsService::notifyAssetRemoved(bool hadTags, bool wasTrash) {
    purgeAsset(hadTags, wasTrash);
}

void StatisticsService::purgeAsset(bool hasTags, bool isTrash) {
    if (isTrash) {
        if (m_trashCount.load() > 0) m_trashCount.fetch_sub(1);
    } else {
        if (m_totalCount.load() > 0) m_totalCount.fetch_sub(1);
        if (!hasTags && m_untaggedCount.load() > 0) {
            m_untaggedCount.fetch_sub(1);
        }
    }

    std::lock_guard<std::mutex> lock(m_snapshotMutex);
    m_cachedSnapshot.systemCounts["all"] = m_totalCount.load();
    m_cachedSnapshot.systemCounts["untagged"] = m_untaggedCount.load();
    m_cachedSnapshot.systemCounts["trash"] = m_trashCount.load();

    emit statisticsUpdated(m_cachedSnapshot);
}

void StatisticsService::notifyAssetTrashChanged(bool toTrash, bool hasTags) {
    if (toTrash) {
        m_totalCount.fetch_sub(1);
        m_trashCount.fetch_add(1);
        if (!hasTags) m_untaggedCount.fetch_sub(1);
    } else {
        m_totalCount.fetch_add(1);
        m_trashCount.fetch_sub(1);
        if (!hasTags) m_untaggedCount.fetch_add(1);
    }

    std::lock_guard<std::mutex> lock(m_snapshotMutex);
    m_cachedSnapshot.systemCounts["all"] = m_totalCount.load();
    m_cachedSnapshot.systemCounts["trash"] = m_trashCount.load();
    m_cachedSnapshot.systemCounts["untagged"] = m_untaggedCount.load();

    emit statisticsUpdated(m_cachedSnapshot);
}

void StatisticsService::notifyDiskTrashCountChanged(int delta) {
    m_trashCount.fetch_add(delta);

    std::lock_guard<std::mutex> lock(m_snapshotMutex);
    m_cachedSnapshot.systemCounts["trash"] = m_trashCount.load();

    emit statisticsUpdated(m_cachedSnapshot);
}

StatisticsSnapshot StatisticsService::computeSnapshotFromDb() {
    StatisticsSnapshot snapshot;

    // 0. 获取当前物理在线托管盘符集合
    QSet<QString> onlineDrives = VolumeOnlineManager::instance().getOnlineDrives();

    int allCount = 0;
    int untaggedCount = 0;

    // 1. 纯内存 0ms 秒级核算（绝对真相源）
    MetadataManager::instance().forEachCachedItem([&](const std::wstring& path, const RuntimeMeta& meta) {
        if (meta.isFolder) return;

        // 🛡️ 物理在线断言 (谓词：drive_letter IN onlineDrives)
        if (path.length() >= 2 && path[1] == L':') {
            QChar dChar = QChar(path[0]).toUpper();
            if (dChar.isLetter()) {
                QString driveStr(dChar);
                if (!onlineDrives.contains(driveStr)) {
                    return; // 🚨 盘符离线直接排除该资产，不参与全库任何计数！
                }
            }
        }

        // 全部有效数据
        allCount++;

        // 未标签
        if (meta.tags.isEmpty()) {
            untaggedCount++;
        }
    });

    // 2. 汇总物理磁盘回收站 (离线盘过滤)
    int diskTrashCount = 0;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (db) {
        sqlite3_stmt* stmtDisk = nullptr;
        if (sqlite3_prepare_v2(db, "SELECT original_path FROM disk_trash", -1, &stmtDisk, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmtDisk) == SQLITE_ROW) {
                const unsigned char* rawPath = sqlite3_column_text(stmtDisk, 0);
                if (rawPath) {
                    QString origP = QString::fromUtf8(reinterpret_cast<const char*>(rawPath));
                    QString letter = VolumeOnlineManager::extractDriveLetter(origP);
                    if (letter.isEmpty() || onlineDrives.contains(letter.toUpper())) {
                        diskTrashCount++;
                    }
                }
            }
            sqlite3_finalize(stmtDisk);
        }
    }

    snapshot.totalCount = allCount;
    snapshot.untaggedCount = untaggedCount;
    snapshot.trashCount = diskTrashCount;

    // 3. 同步原子内存缓存
    m_totalCount.store(allCount);
    m_untaggedCount.store(untaggedCount);
    m_trashCount.store(snapshot.trashCount);

    {
        std::lock_guard<std::mutex> lock(m_snapshotMutex);
        m_cachedSnapshot = snapshot;
    }

    return snapshot;
}

} // namespace QuarkMeta
