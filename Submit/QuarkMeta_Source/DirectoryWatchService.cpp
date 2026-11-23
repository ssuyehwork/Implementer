#include "DirectoryWatchService.h"
#include "CoreController.h"
#include "DwsTrace.h"
#include "../ui/Logger.h"
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QMetaObject>
#include <QCoreApplication>
#include <QMutex>

namespace QuarkMeta {

namespace {
    struct CallbackThrottleInfo {
        bool hasLoggedFirst = false;
        qint64 lastLogTime = 0;
    };
    static QMutex s_cbThrottleMutex;
    static QHash<QString, CallbackThrottleInfo> s_cbThrottleMap;
}

DirectoryWatchService& DirectoryWatchService::instance() {
    static bool s_firstInstance = true;
    if (s_firstInstance) {
        s_firstInstance = false;
        bool isMain = (QThread::currentThread() == (qApp ? qApp->thread() : nullptr));
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "instance",
            QString("首次创建 instance, isMainThread=%1, qAppThread=%2")
                .arg(isMain ? "true" : "false")
                .arg(qApp ? reinterpret_cast<quintptr>(qApp->thread()) : 0));
    }
    static DirectoryWatchService service;
    return service;
}

QString DirectoryWatchService::normalizePathKey(const QString& rawPath) {
    if (rawPath.isEmpty()) return QString();
    QString cleaned = QDir::cleanPath(rawPath);
    cleaned = cleaned.toLower();

    if (cleaned.endsWith('/')) {
        cleaned.chop(1);
    }

    if (cleaned.endsWith(':')) {
        cleaned += '/';
    }

    return cleaned;
}

DirectoryWatchService::DirectoryWatchService(QObject* parent) : QObject(parent) {
    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setInterval(50);
    connect(m_debounceTimer, &QTimer::timeout, this, &DirectoryWatchService::checkDebounceTimers);

    m_workerThread = new WorkerThread(this);
    m_workerThread->start();
}

DirectoryWatchService::~DirectoryWatchService() {
    stopAll();
}

void DirectoryWatchService::watch(const QString& dirPath, bool includeSubtree) {
    if (dirPath.isEmpty()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "watch",
            QString("调用入参 dirPath 为空，直接 return; rawPath=%1, includeSubtree=%2").arg(dirPath).arg(includeSubtree ? "true" : "false"));
        return;
    }
    if (m_isStoppingAll.load()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "watch",
            QString("m_isStoppingAll 为 true，拒绝执行 watch; rawPath=%1, includeSubtree=%2").arg(dirPath).arg(includeSubtree ? "true" : "false"));
        return;
    }

    QString normKey = normalizePathKey(dirPath);
    if (normKey.isEmpty()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "watch",
            QString("normalizePathKey 后为空，直接 return; rawPath=%1, includeSubtree=%2").arg(dirPath).arg(includeSubtree ? "true" : "false"));
        return;
    }

    WatchKey key{normKey, includeSubtree};

    QMutexLocker locker(&m_mutex);
    auto it = m_watches.find(key);
    if (it != m_watches.end()) {
        it.value()->refCount++;
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "watch",
            QString("目标已存在于监听列表中，refCount 增加到 %1; rawPath=%2, normKey=%3, includeSubtree=%4")
                .arg(it.value()->refCount).arg(dirPath).arg(normKey).arg(includeSubtree ? "true" : "false"));
        return;
    }

    auto item = std::make_shared<WatchItem>();
    item->key = key;
    item->originalDirPath = dirPath;
    item->refCount = 1;

    m_watches.insert(key, item);
    m_pendingStarts.append(item);

    QUARKMETA_DWS_TRACE("DirectoryWatchService", "watch",
        QString("新建监听条目，refCount=1，加入 pendingStarts; rawPath=%1, normKey=%2, includeSubtree=%3")
            .arg(dirPath).arg(normKey).arg(includeSubtree ? "true" : "false"));

    if (m_workerThread) {
        m_workerThread->wakeUpWorker();
    }
}

void DirectoryWatchService::unwatch(const QString& dirPath, bool includeSubtree) {
    if (dirPath.isEmpty()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "unwatch",
            QString("调用入参 dirPath 为空，直接 return; rawPath=%1, includeSubtree=%2").arg(dirPath).arg(includeSubtree ? "true" : "false"));
        return;
    }
    if (m_isStoppingAll.load()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "unwatch",
            QString("m_isStoppingAll 为 true，拒绝执行 unwatch; rawPath=%1, includeSubtree=%2").arg(dirPath).arg(includeSubtree ? "true" : "false"));
        return;
    }

    QString normKey = normalizePathKey(dirPath);
    if (normKey.isEmpty()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "unwatch",
            QString("normalizePathKey 后为空，直接 return; rawPath=%1, includeSubtree=%2").arg(dirPath).arg(includeSubtree ? "true" : "false"));
        return;
    }

    WatchKey key{normKey, includeSubtree};

    QMutexLocker locker(&m_mutex);
    auto it = m_watches.find(key);
    if (it == m_watches.end()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "unwatch",
            QString("未在 m_watches 找到该 key，无操作 return; rawPath=%1, normKey=%2, includeSubtree=%3")
                .arg(dirPath).arg(normKey).arg(includeSubtree ? "true" : "false"));
        return;
    }

    it.value()->refCount--;
    if (it.value()->refCount <= 0) {
        auto item = it.value();
        item->isStopping = true;
        m_watches.erase(it);
        m_retiredItems.insert(key, item);
        m_pendingStops.append(item);

        QUARKMETA_DWS_TRACE("DirectoryWatchService", "unwatch",
            QString("refCount 减至 %1，移入 retiredItems 并加入 pendingStops; rawPath=%2, normKey=%3, includeSubtree=%4")
                .arg(item->refCount).arg(dirPath).arg(normKey).arg(includeSubtree ? "true" : "false"));

        if (m_workerThread) {
            m_workerThread->wakeUpWorker();
        }
    } else {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "unwatch",
            QString("refCount 减至 %1，仍保留监听; rawPath=%2, normKey=%3, includeSubtree=%4")
                .arg(it.value()->refCount).arg(dirPath).arg(normKey).arg(includeSubtree ? "true" : "false"));
    }
}

void DirectoryWatchService::discardPending(const QString& dirPath, bool includeSubtree) {
    if (dirPath.isEmpty()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "discardPending",
            QString("调用入参 dirPath 为空，直接 return; rawPath=%1, includeSubtree=%2").arg(dirPath).arg(includeSubtree ? "true" : "false"));
        return;
    }
    if (m_isStoppingAll.load()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "discardPending",
            QString("m_isStoppingAll 为 true，拒绝执行 discardPending; rawPath=%1, includeSubtree=%2").arg(dirPath).arg(includeSubtree ? "true" : "false"));
        return;
    }

    QString normKey = normalizePathKey(dirPath);
    if (normKey.isEmpty()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "discardPending",
            QString("normalizePathKey 后为空，直接 return; rawPath=%1, includeSubtree=%2").arg(dirPath).arg(includeSubtree ? "true" : "false"));
        return;
    }

    WatchKey key{normKey, includeSubtree};

    QMutexLocker locker(&m_mutex);
    int prevDirtyCount = 0;
    bool prevOverflow = false;
    auto debIt = m_debounceMap.find(key);
    if (debIt != m_debounceMap.end()) {
        prevDirtyCount = debIt.value().accumulatedDirtyNames.size();
        prevOverflow = debIt.value().accumulatedOverflow;
    }

    QUARKMETA_DWS_TRACE("DirectoryWatchService", "discardPending",
        QString("清空防抖状态; rawPath=%1, normKey=%2, includeSubtree=%3, 清空前accumulatedDirtyNames数量=%4, 清空前accumulatedOverflow=%5")
            .arg(dirPath).arg(normKey).arg(includeSubtree ? "true" : "false").arg(prevDirtyCount).arg(prevOverflow ? "true" : "false"));

    m_debounceMap.remove(key);
    m_discardGenerations[key]++;

    if (m_debounceMap.isEmpty() && m_debounceTimer) {
        m_debounceTimer->stop();
    }
}

void DirectoryWatchService::stopAll() {
    QUARKMETA_DWS_TRACE("DirectoryWatchService", "stopAll",
        QString("stopAll 被调用，isStoppingAll 当前值=%1").arg(m_isStoppingAll.load() ? "true" : "false"));

    if (m_isStoppingAll.exchange(true)) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "stopAll", "已处于 stopAll 状态，直接 return");
        return;
    }

    {
        QMutexLocker locker(&m_mutex);
        m_debounceTimer->stop();
        m_debounceMap.clear();

        for (auto it = m_watches.begin(); it != m_watches.end(); ++it) {
            it.value()->isStopping = true;
            m_retiredItems.insert(it.key(), it.value());
            m_pendingStops.append(it.value());
        }
        m_watches.clear();
        m_pendingStarts.clear();
    }

    if (m_workerThread) {
        m_workerThread->stopWorker();
        if (!m_workerThread->wait(1000)) {
            Logger::log("[DirectoryWatchService] Worker thread wait timeout (1s), forcing cleanup.");
        }
        delete m_workerThread;
        m_workerThread = nullptr;
    }
}

void DirectoryWatchService::onRawEventsBatch(const WatchKey& key, const QStringList& dirtyNames, bool overflow) {
    if (m_isStoppingAll.load()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "onRawEventsBatch",
            QString("因 m_isStoppingAll 为 true 提前 return; normPath=%1, dirtyNamesSize=%2, overflow=%3")
                .arg(key.normPath).arg(dirtyNames.size()).arg(overflow ? "true" : "false"));
        return;
    }

    QMutexLocker locker(&m_mutex);
    auto watchIt = m_watches.find(key);
    if (watchIt == m_watches.end()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "onRawEventsBatch",
            QString("key 在 m_watches 中未找到(已被 unwatch 或移除); normPath=%1, includeSubtree=%2, dirtyNamesCount=%3, fallbackDisplayPath=%4")
                .arg(key.normPath).arg(key.includeSubtree ? "true" : "false").arg(dirtyNames.size()).arg(key.normPath));
    }
    QString displayPath = (watchIt != m_watches.end()) ? watchIt.value()->originalDirPath : key.normPath;

    auto debIt = m_debounceMap.find(key);
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (debIt == m_debounceMap.end()) {
        DebounceState state;
        state.firstEventTime = now;
        state.lastEventTime = now;
        state.displayPath = displayPath;
        if (overflow) {
            state.accumulatedOverflow = true;
        } else {
            for (const QString& name : dirtyNames) {
                state.accumulatedDirtyNames.insert(name);
            }
        }
        m_debounceMap.insert(key, state);
    } else {
        debIt.value().lastEventTime = now;
        if (overflow) {
            debIt.value().accumulatedOverflow = true;
        } else {
            for (const QString& name : dirtyNames) {
                debIt.value().accumulatedDirtyNames.insert(name);
            }
        }
    }

    if (!m_debounceTimer->isActive()) {
        m_debounceTimer->start();
    }
}

void DirectoryWatchService::onDirectoryLostInternal(const WatchKey& key, const QString& dirPath) {
    if (m_isStoppingAll.load()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "onDirectoryLostInternal",
            QString("因 m_isStoppingAll 为 true 提前 return; key=%1, path=%2").arg(key.normPath).arg(dirPath));
        return;
    }

    bool foundInWatches = false;
    {
        QMutexLocker locker(&m_mutex);
        auto it = m_watches.find(key);
        if (it != m_watches.end()) {
            foundInWatches = true;
            auto item = it.value();
            item->isStopping = true;
            m_watches.erase(it);
            m_retiredItems.insert(key, item);
            m_pendingStops.append(item);
        }
        m_debounceMap.remove(key);
    }

    QUARKMETA_DWS_TRACE("DirectoryWatchService", "onDirectoryLostInternal",
        QString("触发目录丢失 internal 处理; key=%1, dirPath=%2, 在m_watches中找到=%3")
            .arg(key.normPath).arg(dirPath).arg(foundInWatches ? "true" : "false"));

    emit watchedDirectoryLost(dirPath);
}

void DirectoryWatchService::removeRetiredItem(const WatchKey& key) {
    QMutexLocker locker(&m_mutex);
    m_retiredItems.remove(key);
}

void DirectoryWatchService::checkDebounceTimers() {
    if (m_isStoppingAll.load()) return;

    QMutexLocker locker(&m_mutex);
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    QList<WatchKey> keysToEmit;
    for (auto it = m_debounceMap.begin(); it != m_debounceMap.end(); ++it) {
        const DebounceState& st = it.value();
        qint64 quietTime = now - st.lastEventTime;
        qint64 totalTime = now - st.firstEventTime;

        if (st.accumulatedOverflow || quietTime >= 300 || totalTime >= 1000) {
            keysToEmit.append(it.key());
        }
    }

    for (const WatchKey& key : keysToEmit) {
        DebounceState st = m_debounceMap.take(key);
        QString displayPath = st.displayPath;
        QStringList names = st.accumulatedDirtyNames.values();
        bool overflow = st.accumulatedOverflow;
        uint64_t gen = m_discardGenerations.value(key, 0);

        QUARKMETA_DWS_TRACE("DirectoryWatchService", "checkDebounceTimers",
            QString("准备发出 directoryChanged 投递; displayPath=%1, namesCount=%2, overflow=%3, gen=%4")
                .arg(displayPath).arg(names.size()).arg(overflow ? "true" : "false").arg(gen));

        QMetaObject::invokeMethod(this, [this, key, gen, displayPath, names, overflow]() {
            if (m_isStoppingAll.load()) {
                QUARKMETA_DWS_TRACE("DirectoryWatchService", "checkDebounceTimers",
                    QString("队列回调中因 m_isStoppingAll 为 true 丢弃事件; key=%1, displayPath=%2").arg(key.normPath).arg(displayPath));
                return;
            }
            {
                QMutexLocker locker(&m_mutex);
                if (m_discardGenerations.value(key, 0) != gen) {
                    QUARKMETA_DWS_TRACE("DirectoryWatchService", "checkDebounceTimers",
                        QString("队列回调中因 gen 不一致丢弃事件(发生过 discardPending); key=%1, displayPath=%2, eventGen=%3, currentGen=%4")
                            .arg(key.normPath).arg(displayPath).arg(gen).arg(m_discardGenerations.value(key, 0)));
                    return;
                }
            }
            QUARKMETA_DWS_TRACE("DirectoryWatchService", "checkDebounceTimers",
                QString("真正 emit directoryChanged; displayPath=%1, namesCount=%2, overflow=%3")
                    .arg(displayPath).arg(names.size()).arg(overflow ? "true" : "false"));
            emit directoryChanged(displayPath, names, overflow);
        }, Qt::QueuedConnection);
    }

    if (m_debounceMap.isEmpty()) {
        m_debounceTimer->stop();
    }
}

#ifdef Q_OS_WIN
VOID WINAPI DirectoryWatchService::notificationCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    if (!lpOverlapped) return;

    WatchItem* item = reinterpret_cast<WatchItem*>(lpOverlapped);
    if (!item) return;

    if (dwErrorCode != ERROR_SUCCESS || item->isStopping || CoreController::isShuttingDown()) {
        bool isStoppingVal = item->isStopping;
        bool isShutdownVal = CoreController::isShuttingDown();
        QString branchDesc;

        WatchKey capturedKey = item->key;
        QString capturedPath = item->originalDirPath;
        if (dwErrorCode == ERROR_ACCESS_DENIED || dwErrorCode == ERROR_INVALID_HANDLE ||
            dwErrorCode == ERROR_DEVICE_NOT_CONNECTED || dwErrorCode == ERROR_HANDLE_EOF) {
            branchDesc = "已知目录失效错误码(ACCESS_DENIED/INVALID_HANDLE/DEVICE_NOT_CONNECTED/HANDLE_EOF)，分发 onDirectoryLostInternal 与 removeRetiredItem";
            QMetaObject::invokeMethod(&DirectoryWatchService::instance(), [capturedKey, capturedPath]() {
                DirectoryWatchService::instance().onDirectoryLostInternal(capturedKey, capturedPath);
            }, Qt::QueuedConnection);
            QMetaObject::invokeMethod(&DirectoryWatchService::instance(), [capturedKey]() {
                DirectoryWatchService::instance().removeRetiredItem(capturedKey);
            }, Qt::QueuedConnection);
        } else if (dwErrorCode != ERROR_SUCCESS) {
            branchDesc = QString("该错误码(%1)未被特殊处理，仅分发 removeRetiredItem，监听将停止重新发起").arg(dwErrorCode);
            QMetaObject::invokeMethod(&DirectoryWatchService::instance(), [capturedKey]() {
                DirectoryWatchService::instance().removeRetiredItem(capturedKey);
            }, Qt::QueuedConnection);
        } else {
            branchDesc = QString("正常错误码但因 item->isStopping=%1 或 CoreController::isShuttingDown=%2 触发退出，分发 removeRetiredItem")
                            .arg(isStoppingVal ? "true" : "false").arg(isShutdownVal ? "true" : "false");
            QMetaObject::invokeMethod(&DirectoryWatchService::instance(), [capturedKey]() {
                DirectoryWatchService::instance().removeRetiredItem(capturedKey);
            }, Qt::QueuedConnection);
        }

        QUARKMETA_DWS_TRACE("DirectoryWatchService", "notificationCompletionRoutine",
            QString("异常/停止分支触发: dwErrorCode=%1, isStopping=%2, isShuttingDown=%3, path=%4, 分支走向=%5")
                .arg(dwErrorCode).arg(isStoppingVal ? "true" : "false").arg(isShutdownVal ? "true" : "false")
                .arg(item->originalDirPath).arg(branchDesc));
        return;
    }

    if (dwNumberOfBytesTransfered == 0) {
        // 限频打印 0 字节溢出事件
        bool shouldLog = false;
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        {
            QMutexLocker throttleLocker(&s_cbThrottleMutex);
            auto& tInfo = s_cbThrottleMap[item->originalDirPath];
            if (!tInfo.hasLoggedFirst) {
                tInfo.hasLoggedFirst = true;
                tInfo.lastLogTime = now;
                shouldLog = true;
            } else if (now - tInfo.lastLogTime >= 1000) {
                tInfo.lastLogTime = now;
                shouldLog = true;
            }
        }
        if (shouldLog) {
            QUARKMETA_DWS_TRACE("DirectoryWatchService", "notificationCompletionRoutine",
                QString("正常事件回调(0字节溢出): key=%1, dirtyNames数量=0, dwNumberOfBytesTransfered=0, overflow=true")
                    .arg(item->key.normPath));
        }

        WatchKey capturedKey = item->key;
        QMetaObject::invokeMethod(&DirectoryWatchService::instance(), [capturedKey]() {
            DirectoryWatchService::instance().onRawEventsBatch(capturedKey, QStringList(), true);
        }, Qt::QueuedConnection);
    } else {
        PFILE_NOTIFY_INFORMATION pNotify = (PFILE_NOTIFY_INFORMATION)item->buffer;
        QStringList dirtyNames;

        while (pNotify) {
            int nameLen = pNotify->FileNameLength / sizeof(WCHAR);
            if (nameLen > 0) {
                QString relPath = QString::fromWCharArray(pNotify->FileName, nameLen);
                relPath = QDir::cleanPath(relPath);
                dirtyNames.append(relPath);
            }

            if (pNotify->NextEntryOffset == 0) break;
            pNotify = (PFILE_NOTIFY_INFORMATION)((LPBYTE)pNotify + pNotify->NextEntryOffset);
        }

        // 限频打印正常事件回调：首次必打，之后同一监听每秒最多1条，最多前3个文件名
        bool shouldLog = false;
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        {
            QMutexLocker throttleLocker(&s_cbThrottleMutex);
            auto& tInfo = s_cbThrottleMap[item->originalDirPath];
            if (!tInfo.hasLoggedFirst) {
                tInfo.hasLoggedFirst = true;
                tInfo.lastLogTime = now;
                shouldLog = true;
            } else if (now - tInfo.lastLogTime >= 1000) {
                tInfo.lastLogTime = now;
                shouldLog = true;
            }
        }
        if (shouldLog) {
            QStringList sampleNames;
            for (int i = 0; i < qMin(dirtyNames.size(), 3); ++i) {
                sampleNames.append(dirtyNames[i]);
            }
            QUARKMETA_DWS_TRACE("DirectoryWatchService", "notificationCompletionRoutine",
                QString("正常事件回调: key=%1, dirtyNames数量=%2, 前3个文件名=[%3], dwNumberOfBytesTransfered=%4, overflow=false")
                    .arg(item->key.normPath).arg(dirtyNames.size()).arg(sampleNames.join(", ")).arg(dwNumberOfBytesTransfered));
        }

        if (!dirtyNames.isEmpty()) {
            WatchKey capturedKey = item->key;
            QMetaObject::invokeMethod(&DirectoryWatchService::instance(), [capturedKey, dirtyNames]() {
                DirectoryWatchService::instance().onRawEventsBatch(capturedKey, dirtyNames, false);
            }, Qt::QueuedConnection);
        }
    }

    // Re-issue ReadDirectoryChangesW for continuous monitoring
    ZeroMemory(&item->overlapped, sizeof(OVERLAPPED));
    BOOL ok = ReadDirectoryChangesW(
        item->hDir,
        item->buffer,
        sizeof(item->buffer),
        item->key.includeSubtree ? TRUE : FALSE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_ATTRIBUTES,
        NULL,
        &item->overlapped,
        DirectoryWatchService::notificationCompletionRoutine
    );

    if (!ok) {
        DWORD err = GetLastError();
        Logger::log(QString("[DirectoryWatchService] Re-issue ReadDirectoryChangesW failed for %1, error: %2")
                    .arg(item->originalDirPath).arg(err));
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "notificationCompletionRoutine",
            QString("重新发起 ReadDirectoryChangesW 失败! path=%1, GetLastError=%2").arg(item->originalDirPath).arg(err));
        WatchKey capturedKey = item->key;
        QString capturedPath = item->originalDirPath;
        QMetaObject::invokeMethod(&DirectoryWatchService::instance(), [capturedKey, capturedPath]() {
            DirectoryWatchService::instance().onDirectoryLostInternal(capturedKey, capturedPath);
        }, Qt::QueuedConnection);
    }
}
#endif

DirectoryWatchService::WorkerThread::WorkerThread(DirectoryWatchService* service)
    : m_service(service) {
#ifdef Q_OS_WIN
    m_wakeUpEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
#endif
}

DirectoryWatchService::WorkerThread::~WorkerThread() {
#ifdef Q_OS_WIN
    if (m_wakeUpEvent) {
        CloseHandle(m_wakeUpEvent);
        m_wakeUpEvent = NULL;
    }
#endif
}

void DirectoryWatchService::WorkerThread::stopWorker() {
    m_stopRequested.store(true);
    wakeUpWorker();
}

void DirectoryWatchService::WorkerThread::wakeUpWorker() {
#ifdef Q_OS_WIN
    if (m_wakeUpEvent) {
        SetEvent(m_wakeUpEvent);
    }
#endif
}

void DirectoryWatchService::WorkerThread::run() {
    QUARKMETA_DWS_TRACE("DirectoryWatchService::WorkerThread", "run", "WorkerThread::run 开始执行");

    while (!m_stopRequested.load() && !CoreController::isShuttingDown()) {
        m_service->processWorkerQueue();

#ifdef Q_OS_WIN
        DWORD dwWait = SleepEx(100, TRUE);
        if (dwWait == WAIT_IO_COMPLETION) {
            // Asynchronous I/O completion routine was executed
            continue;
        }
#else
        QThread::msleep(100);
#endif
    }

    QUARKMETA_DWS_TRACE("DirectoryWatchService::WorkerThread", "run",
        QString("WorkerThread::run 退出循环, m_stopRequested=%1, CoreController::isShuttingDown=%2")
            .arg(m_stopRequested.load() ? "true" : "false")
            .arg(CoreController::isShuttingDown() ? "true" : "false"));

    // Cleanup all remaining handles on worker thread shutdown
    QList<std::shared_ptr<WatchItem>> itemsToCleanup;
    {
        QMutexLocker locker(&m_service->m_mutex);
        for (auto item : m_service->m_pendingStops) {
            itemsToCleanup.append(item);
        }
        m_service->m_pendingStops.clear();

        for (auto item : m_service->m_pendingStarts) {
            itemsToCleanup.append(item);
        }
        m_service->m_pendingStarts.clear();

        for (auto it = m_service->m_watches.begin(); it != m_service->m_watches.end(); ++it) {
            itemsToCleanup.append(it.value());
        }
        m_service->m_watches.clear();

        for (auto it = m_service->m_retiredItems.begin(); it != m_service->m_retiredItems.end(); ++it) {
            itemsToCleanup.append(it.value());
        }
        m_service->m_retiredItems.clear();
    }

#ifdef Q_OS_WIN
    for (auto item : itemsToCleanup) {
        if (item->hDir != INVALID_HANDLE_VALUE) {
            CancelIoEx(item->hDir, &item->overlapped);
            CloseHandle(item->hDir);
            item->hDir = INVALID_HANDLE_VALUE;
        }
    }
#endif
}

void DirectoryWatchService::processWorkerQueue() {
    QList<std::shared_ptr<WatchItem>> starts;
    QList<std::shared_ptr<WatchItem>> stops;

    {
        QMutexLocker locker(&m_mutex);
        starts = m_pendingStarts;
        m_pendingStarts.clear();

        stops = m_pendingStops;
        m_pendingStops.clear();
    }

    if (!starts.isEmpty() || !stops.isEmpty()) {
        QUARKMETA_DWS_TRACE("DirectoryWatchService", "processWorkerQueue",
            QString("处理队列: starts数量=%1, stops数量=%2").arg(starts.size()).arg(stops.size()));
    }

#ifdef Q_OS_WIN
    // 1. Process pending stops
    for (auto item : stops) {
        if (item->hDir != INVALID_HANDLE_VALUE) {
            CancelIoEx(item->hDir, &item->overlapped);
            CloseHandle(item->hDir);
            item->hDir = INVALID_HANDLE_VALUE;
        }
    }

    // 2. Process pending starts
    for (auto item : starts) {
        if (m_isStoppingAll.load()) {
            QUARKMETA_DWS_TRACE("DirectoryWatchService", "processWorkerQueue",
                QString("跳过启动条目(m_isStoppingAll为true): path=%1").arg(item->originalDirPath));
            continue;
        }
        if (item->isStopping) {
            QUARKMETA_DWS_TRACE("DirectoryWatchService", "processWorkerQueue",
                QString("跳过启动条目(item->isStopping为true): path=%1").arg(item->originalDirPath));
            continue;
        }

        QString pathStr = item->originalDirPath;
        if (!pathStr.startsWith("\\\\?\\")) {
            QString native = QDir::toNativeSeparators(pathStr);
            if (native.length() >= 240) {
                if (native.startsWith("\\\\")) {
                    pathStr = "\\\\?\\UNC\\" + native.mid(2);
                } else {
                    pathStr = "\\\\?\\" + native;
                }
            } else {
                pathStr = native;
            }
        } else {
            pathStr = QDir::toNativeSeparators(pathStr);
        }

        std::wstring wPath = pathStr.toStdWString();
        HANDLE hDir = CreateFileW(
            wPath.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            NULL
        );

        if (hDir == INVALID_HANDLE_VALUE) {
            DWORD err = GetLastError();
            Logger::log(QString("[DirectoryWatchService] Failed to open handle for %1, error: %2")
                        .arg(item->originalDirPath).arg(err));
            QUARKMETA_DWS_TRACE("DirectoryWatchService", "processWorkerQueue",
                QString("CreateFileW 失败! originalPath=%1, convertedPath=%2, success=false, GetLastError=%3")
                    .arg(item->originalDirPath).arg(QString::fromStdWString(wPath)).arg(err));
            continue;
        }

        QUARKMETA_DWS_TRACE("DirectoryWatchService", "processWorkerQueue",
            QString("CreateFileW 成功! originalPath=%1, convertedPath=%2, success=true, handle=%3")
                .arg(item->originalDirPath).arg(QString::fromStdWString(wPath)).arg(reinterpret_cast<quintptr>(hDir)));

        item->hDir = hDir;
        ZeroMemory(&item->overlapped, sizeof(OVERLAPPED));

        BOOL ok = ReadDirectoryChangesW(
            item->hDir,
            item->buffer,
            sizeof(item->buffer),
            item->key.includeSubtree ? TRUE : FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_ATTRIBUTES,
            NULL,
            &item->overlapped,
            DirectoryWatchService::notificationCompletionRoutine
        );

        if (!ok) {
            DWORD err = GetLastError();
            Logger::log(QString("[DirectoryWatchService] Initial ReadDirectoryChangesW failed for %1, error: %2")
                        .arg(item->originalDirPath).arg(err));
            QUARKMETA_DWS_TRACE("DirectoryWatchService", "processWorkerQueue",
                QString("首次 ReadDirectoryChangesW 失败! path=%1, success=false, GetLastError=%2")
                    .arg(item->originalDirPath).arg(err));
            CloseHandle(item->hDir);
            item->hDir = INVALID_HANDLE_VALUE;
        } else {
            QUARKMETA_DWS_TRACE("DirectoryWatchService", "processWorkerQueue",
                QString("首次 ReadDirectoryChangesW 成功! path=%1, success=true, includeSubtree=%2")
                    .arg(item->originalDirPath).arg(item->key.includeSubtree ? "true" : "false"));
        }
    }
#else
    Q_UNUSED(starts);
    Q_UNUSED(stops);
#endif
}

} // namespace QuarkMeta
