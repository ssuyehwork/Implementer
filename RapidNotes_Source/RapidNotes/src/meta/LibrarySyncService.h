#pragma once

#include "QuarkMetaJsonStore.h"
#include <QObject>
#include <QStringList>
#include <QThreadPool>
#include <QTimer>
#include <mutex>
#include <shared_mutex>
#include <unordered_set>

namespace RapidNotes {

class LibrarySyncService : public QObject {
    Q_OBJECT

public:
    static LibrarySyncService& instance();

    void syncPaths(const QStringList& paths);
    void removePaths(const QStringList& paths);
    bool isLibraryPath(const QString& path) const;

    void triggerStartupReconciliation();

signals:
    void libraryItemsSynced();

private:
    explicit LibrarySyncService(QObject* parent = nullptr);
    ~LibrarySyncService() override;

    void loadLibraryPathSet();
    void processSyncBatch(const QStringList& batch);
    static QString normalizePath(const QString& path);

    void handleJsonChangeEvent(const RapidNotesJsonStore::ChangeEvent& event);
    void scheduleDebouncedSync(const QString& path);
    void flushDebouncedQueue();

    QThreadPool m_workPool;
    std::unordered_set<std::wstring> m_libraryPathsSet;
    mutable std::shared_mutex m_pathMutex;

    QTimer* m_debounceTimer = nullptr;
    std::unordered_set<std::wstring> m_pendingSyncPaths;
    std::mutex m_debounceMutex;
};

} // namespace RapidNotes
