#include "LibrarySyncService.h"
#include "LibraryDao.h"
#include "LibraryItemDao.h"
#include "QuarkMetaJsonStore.h"
#include "ItemMetaMapper.h"
#include "MetadataManager.h"
#include <QtConcurrent/QtConcurrent>
#include <QDir>
#include <QTimer>
#include <QDebug>

namespace QuarkMeta {

LibrarySyncService& LibrarySyncService::instance() {
    static LibrarySyncService s_instance;
    return s_instance;
}

LibrarySyncService::LibrarySyncService(QObject* parent) : QObject(parent) {
    m_workPool.setMaxThreadCount(1);
    loadLibraryPathSet();

    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(200); // 200ms 防抖
    connect(m_debounceTimer, &QTimer::timeout, this, &LibrarySyncService::flushDebouncedQueue);

    QuarkMetaJsonStore::instance().addChangeListener([this](const QuarkMetaJsonStore::ChangeEvent& ev) {
        handleJsonChangeEvent(ev);
    });

    QTimer::singleShot(1500, this, [this]() {
        triggerStartupReconciliation();
    });
}

void LibrarySyncService::handleJsonChangeEvent(const QuarkMetaJsonStore::ChangeEvent& event) {
    if (event.type == QuarkMetaJsonStore::ChangeType::ItemChanged) {
        if (!isLibraryPath(event.path)) return; // 拦截阻断非库成员！
        scheduleDebouncedSync(event.path);
    } else if (event.type == QuarkMetaJsonStore::ChangeType::ItemRenamed) {
        if (!isLibraryPath(event.path)) return;
        (void)QtConcurrent::run(&m_workPool, [this, oldP = event.path, newP = event.newPath]() {
            LibraryDao::updatePathPrefix(oldP, newP);
            LibraryItemDao::updatePath(oldP, newP);
            {
                std::unique_lock<std::shared_mutex> lock(m_pathMutex);
                m_libraryPathsSet.erase(normalizePath(oldP).toStdWString());
                m_libraryPathsSet.insert(normalizePath(newP).toStdWString());
            }
            QMetaObject::invokeMethod(this, &LibrarySyncService::libraryItemsSynced, Qt::QueuedConnection);
        });
    } else if (event.type == QuarkMetaJsonStore::ChangeType::FolderMigrated) {
        (void)QtConcurrent::run(&m_workPool, [this, oldF = event.path, newF = event.newPath]() {
            LibraryDao::updatePathPrefix(oldF, newF);
            LibraryItemDao::updatePathPrefix(oldF, newF);
            loadLibraryPathSet(); // 彻底重新从数据库刷新内存成员集合
            QMetaObject::invokeMethod(this, &LibrarySyncService::libraryItemsSynced, Qt::QueuedConnection);
        });
    } else if (event.type == QuarkMetaJsonStore::ChangeType::ItemRemoved) {
        if (!isLibraryPath(event.path)) return;
        (void)QtConcurrent::run(&m_workPool, [this, eventP = event.path]() {
            LibraryItemDao::markSyncStateByPath(eventP, 2); // 标记 sync_state = 2 (缺失)
            QMetaObject::invokeMethod(this, &LibrarySyncService::libraryItemsSynced, Qt::QueuedConnection);
        });
    }
}

void LibrarySyncService::scheduleDebouncedSync(const QString& path) {
    {
        std::lock_guard<std::mutex> lock(m_debounceMutex);
        m_pendingSyncPaths.insert(normalizePath(path).toStdWString());
    }
    QMetaObject::invokeMethod(this, [this]() {
        if (m_debounceTimer && !m_debounceTimer->isActive()) {
            m_debounceTimer->start();
        }
    }, Qt::QueuedConnection);
}

void LibrarySyncService::flushDebouncedQueue() {
    QStringList pathsToSync;
    {
        std::lock_guard<std::mutex> lock(m_debounceMutex);
        for (const auto& w : m_pendingSyncPaths) {
            pathsToSync.append(QString::fromStdWString(w));
        }
        m_pendingSyncPaths.clear();
    }
    if (!pathsToSync.isEmpty()) {
        syncPaths(pathsToSync);
    }
}

LibrarySyncService::~LibrarySyncService() {
    m_workPool.waitForDone();
}

QString LibrarySyncService::normalizePath(const QString& path) {
    return QDir::toNativeSeparators(path).toLower();
}

void LibrarySyncService::loadLibraryPathSet() {
    QStringList paths = LibraryDao::getCategoryPaths(-1);
    std::unique_lock<std::shared_mutex> lock(m_pathMutex);
    m_libraryPathsSet.clear();
    for (const QString& p : paths) {
        m_libraryPathsSet.insert(normalizePath(p).toStdWString());
    }
}

bool LibrarySyncService::isLibraryPath(const QString& path) const {
    std::shared_lock<std::shared_mutex> lock(m_pathMutex);
    return m_libraryPathsSet.count(normalizePath(path).toStdWString()) > 0;
}

void LibrarySyncService::syncPaths(const QStringList& paths) {
    if (paths.isEmpty()) return;

    {
        std::unique_lock<std::shared_mutex> lock(m_pathMutex);
        for (const QString& p : paths) {
            m_libraryPathsSet.insert(normalizePath(p).toStdWString());
        }
    }

    (void)QtConcurrent::run(&m_workPool, [this, paths]() {
        const int batchSize = 100;
        bool anyBatchSuccess = false;
        for (int i = 0; i < paths.size(); i += batchSize) {
            QStringList batch = paths.mid(i, batchSize);
            std::vector<ItemRecord> records;
            records.reserve(batch.size());
            for (const QString& path : batch) {
                ItemRecord rec = ItemRecord::create(path);
                ItemMeta meta;
                if (QuarkMetaJsonStore::instance().readItemMeta(path.toStdWString(), meta)) {
                    ItemMetaMapper::apply(meta, rec);
                }
                QFileInfo info(path);
                std::wstring folderPath = info.isDir() ? path.toStdWString() : info.absolutePath().toStdWString();
                rec.metaFileId = QuarkMetaJsonStore::instance().getMetaFileId(folderPath);
                records.push_back(rec);
            }
            if (LibraryItemDao::upsertItems(records)) {
                anyBatchSuccess = true;
            } else {
                qWarning() << "[LibrarySyncService] Failed to upsert batch, first path:"
                           << (batch.isEmpty() ? QString() : batch.first())
                           << "batch size:" << batch.size();
            }
        }
        if (anyBatchSuccess) {
            QMetaObject::invokeMethod(this, &LibrarySyncService::libraryItemsSynced, Qt::QueuedConnection);
        }
    });
}

void LibrarySyncService::removePaths(const QStringList& paths) {
    if (paths.isEmpty()) return;

    (void)QtConcurrent::run(&m_workPool, [this, paths]() {
        QStringList pathsToRemove;
        {
            std::unique_lock<std::shared_mutex> lock(m_pathMutex);
            for (const QString& p : paths) {
                QString norm = normalizePath(p);
                // 确认该路径已不被任何分类引用
                QStringList cats = LibraryDao::getCategoryPaths(-1);
                if (!cats.contains(p, Qt::CaseInsensitive)) {
                    m_libraryPathsSet.erase(norm.toStdWString());
                    pathsToRemove.append(p);
                }
            }
        }
        if (!pathsToRemove.isEmpty()) {
            LibraryItemDao::removeItemsByPaths(pathsToRemove);
            QMetaObject::invokeMethod(this, &LibrarySyncService::libraryItemsSynced, Qt::QueuedConnection);
        }
    });
}

void LibrarySyncService::processSyncBatch(const QStringList& batch) {
    std::vector<ItemRecord> records;
    records.reserve(batch.size());
    for (const QString& path : batch) {
        ItemRecord rec = ItemRecord::create(path);
        ItemMeta meta;
        if (QuarkMetaJsonStore::instance().readItemMeta(path.toStdWString(), meta)) {
            ItemMetaMapper::apply(meta, rec);
        }
        QFileInfo info(path);
        std::wstring folderPath = info.isDir() ? path.toStdWString() : info.absolutePath().toStdWString();
        rec.metaFileId = QuarkMetaJsonStore::instance().getMetaFileId(folderPath);
        records.push_back(rec);
    }
    LibraryItemDao::upsertItems(records);
}

void LibrarySyncService::triggerStartupReconciliation() {
    (void)QtConcurrent::run(&m_workPool, [this]() {
        // 对账 1: 分类中有而库表中缺少的，增量补齐
        QStringList categoryPaths = LibraryDao::getCategoryPaths(-1);
        QStringList existingPaths = LibraryItemDao::getAllLibraryPaths();
        std::unordered_set<std::wstring> existingSet;
        for (const auto& p : existingPaths) {
            existingSet.insert(normalizePath(p).toStdWString());
        }

        QStringList missingPaths;
        for (const QString& cp : categoryPaths) {
            if (existingSet.count(normalizePath(cp).toStdWString()) == 0) {
                missingPaths.append(cp);
            }
        }
        if (!missingPaths.isEmpty()) {
            syncPaths(missingPaths);
        }

        // 对账 2: 库表中有但已无任何分类引用的孤儿条目，自动清理
        std::unordered_set<std::wstring> catSet;
        for (const QString& cp : categoryPaths) {
            catSet.insert(normalizePath(cp).toStdWString());
        }

        QStringList orphanPaths;
        for (const auto& p : existingPaths) {
            if (catSet.count(normalizePath(p).toStdWString()) == 0) {
                orphanPaths.append(p);
            }
        }
        if (!orphanPaths.isEmpty()) {
            LibraryItemDao::removeItemsByPaths(orphanPaths);
        }
    });
}

} // namespace QuarkMeta
