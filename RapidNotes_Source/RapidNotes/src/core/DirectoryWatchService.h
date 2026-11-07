#ifndef DIRECTORYWATCHSERVICE_H
#define DIRECTORYWATCHSERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QSet>
#include <QMutex>
#include <QThread>
#include <QTimer>
#include <atomic>
#include <memory>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

struct WatchKey {
    QString normPath;
    bool includeSubtree = false;

    bool operator==(const WatchKey& other) const {
        return normPath == other.normPath && includeSubtree == other.includeSubtree;
    }
};

inline size_t qHash(const WatchKey& key, size_t seed = 0) {
    return ::qHash(key.normPath, seed) ^ (key.includeSubtree ? 0x9e3779b9 : 0);
}

class DirectoryWatchService : public QObject {
    Q_OBJECT

public:
    static DirectoryWatchService& instance();

    void watch(const QString& dirPath, bool includeSubtree);
    void unwatch(const QString& dirPath, bool includeSubtree);
    void discardPending(const QString& dirPath, bool includeSubtree);
    void stopAll();

    static QString normalizePathKey(const QString& rawPath);

signals:
    void directoryChanged(const QString& dirPath, const QStringList& dirtyNames, bool overflow);
    void watchedDirectoryLost(const QString& dirPath);

private:
    explicit DirectoryWatchService(QObject* parent = nullptr);
    ~DirectoryWatchService() override;

    DirectoryWatchService(const DirectoryWatchService&) = delete;
    DirectoryWatchService& operator=(const DirectoryWatchService&) = delete;

    struct DebounceState {
        qint64 firstEventTime = 0;
        qint64 lastEventTime = 0;
        QSet<QString> accumulatedDirtyNames;
        bool accumulatedOverflow = false;
        QString displayPath;
    };

    struct WatchItem {
#ifdef Q_OS_WIN
        OVERLAPPED overlapped{};
        HANDLE hDir = INVALID_HANDLE_VALUE;
        alignas(DWORD) BYTE buffer[32768]{};
#endif
        WatchKey key;
        QString originalDirPath;
        int refCount = 0;
        bool isStopping = false;
    };

    class WorkerThread : public QThread {
    public:
        explicit WorkerThread(DirectoryWatchService* service);
        ~WorkerThread() override;

        void stopWorker();
        void wakeUpWorker();

    protected:
        void run() override;

    private:
        DirectoryWatchService* m_service = nullptr;
        std::atomic<bool> m_stopRequested{false};
#ifdef Q_OS_WIN
        HANDLE m_wakeUpEvent = NULL;
#endif
    };

    void processWorkerQueue();
    void onRawEventsBatch(const WatchKey& key, const QStringList& dirtyNames, bool overflow);
    void onDirectoryLostInternal(const WatchKey& key, const QString& dirPath);
    void checkDebounceTimers();
    void removeRetiredItem(const WatchKey& key);

#ifdef Q_OS_WIN
    static VOID WINAPI notificationCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
#endif

    QMutex m_mutex;
    QHash<WatchKey, std::shared_ptr<WatchItem>> m_watches;
    QHash<WatchKey, std::shared_ptr<WatchItem>> m_retiredItems;
    QList<std::shared_ptr<WatchItem>> m_pendingStarts;
    QList<std::shared_ptr<WatchItem>> m_pendingStops;

    QHash<WatchKey, DebounceState> m_debounceMap;
    QHash<WatchKey, uint64_t> m_discardGenerations;
    QTimer* m_debounceTimer = nullptr;

    WorkerThread* m_workerThread = nullptr;
    std::atomic<bool> m_isStoppingAll{false};
};

} // namespace QuarkMeta

#endif // DIRECTORYWATCHSERVICE_H
