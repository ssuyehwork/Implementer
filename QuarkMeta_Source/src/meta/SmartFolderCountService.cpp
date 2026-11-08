#include "SmartFolderCountService.h"
#include "DatabaseManager.h"
#include <QtConcurrent/QtConcurrent>
#include <QPointer>
#include <QCoreApplication>

namespace QuarkMeta {

SmartFolderCountService::SmartFolderCountService(QObject* parent) : QObject(parent) {
    m_countDebounceTimer = new QTimer(this);
    m_countDebounceTimer->setSingleShot(true);
    connect(m_countDebounceTimer, &QTimer::timeout, this, [this]() {
        uint64_t reqId = ++m_currentCountReqId;
        executeCalculationAsync(reqId, QList<SmartFolderRecord>());
    });
}

void SmartFolderCountService::requestCountsImmediately(const QList<SmartFolderRecord>& smartFolders) {
    if (smartFolders.isEmpty()) return;
    uint64_t reqId = ++m_currentCountReqId;
    executeCalculationAsync(reqId, smartFolders);
}

void SmartFolderCountService::requestCountsDebounced() {
    if (m_countDebounceTimer) {
        m_countDebounceTimer->start(150);
    }
}

void SmartFolderCountService::invalidate(int smartFolderId) {
    if (smartFolderId == 0) {
        m_smartFolderCountCache.clear();
    } else {
        m_smartFolderCountCache.remove(smartFolderId);
    }
}

int SmartFolderCountService::cachedCount(int smartFolderId) const {
    if (m_smartFolderCountCache.contains(smartFolderId)) {
        return m_smartFolderCountCache.value(smartFolderId);
    }
    return -999;
}

void SmartFolderCountService::executeCalculationAsync(uint64_t reqId, const QList<SmartFolderRecord>& passedSmartFolders) {
    QString dbPath = DatabaseManager::getGlobalDbPath();
    QPointer<SmartFolderCountService> weakThis(this);

    (void)QtConcurrent::run([weakThis, reqId, passedSmartFolders, dbPath]() {
        QList<SmartFolderRecord> smartFolders = passedSmartFolders;
        if (smartFolders.isEmpty()) {
            smartFolders = SmartFolderDao::getAllSmartFolders();
        }

        QMap<int, int> results = SmartFolderDao::countMatchingItemsBatchReadOnly(dbPath, smartFolders);

        QMetaObject::invokeMethod(qApp, [weakThis, reqId, results]() {
            if (!weakThis || reqId != weakThis->m_currentCountReqId) {
                return; // 请求已过期或对象已析构，安全丢弃
            }

            for (auto it = results.constBegin(); it != results.constEnd(); ++it) {
                weakThis->m_smartFolderCountCache.insert(it.key(), it.value());
            }

            emit weakThis->countsReady(results);
        });
    });
}

} // namespace QuarkMeta
