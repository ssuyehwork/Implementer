#include "QuarkMetaJsonStore.h"
#include "MetaIndexService.h"
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

namespace QuarkMeta {

namespace {

QStringList mergeAddTagsInternal(const std::vector<std::wstring>& existingTagsStd, const QStringList& newTags) {
    QStringList result;
    QSet<QString> existingLower;
    for (const auto& wtag : existingTagsStd) {
        QString tag = QString::fromStdWString(wtag);
        result.append(tag);
        existingLower.insert(tag.toLower());
    }
    for (const QString& tag : newTags) {
        QString clean = tag.trimmed();
        if (clean.isEmpty()) continue;
        if (!existingLower.contains(clean.toLower())) {
            result.append(clean);
            existingLower.insert(clean.toLower());
        }
    }
    return result;
}

QStringList mergeRemoveTagsInternal(const std::vector<std::wstring>& existingTagsStd, const QStringList& removeTags) {
    QSet<QString> removeSetLower;
    for (const QString& tag : removeTags) {
        removeSetLower.insert(tag.trimmed().toLower());
    }
    QStringList result;
    for (const auto& wtag : existingTagsStd) {
        QString tag = QString::fromStdWString(wtag);
        if (!removeSetLower.contains(tag.toLower())) {
            result.append(tag);
        }
    }
    return result;
}

std::vector<std::wstring> toStdWStringVector(const QStringList& tags) {
    std::vector<std::wstring> res;
    res.reserve(tags.size());
    for (const QString& t : tags) {
        res.push_back(t.toStdWString());
    }
    return res;
}

} // namespace

QuarkMetaJsonStore& QuarkMetaJsonStore::instance() {
    static QuarkMetaJsonStore s_instance;
    return s_instance;
}

QuarkMetaJsonStore::QuarkMetaJsonStore(QObject* parent)
    : QObject(parent) {
    m_flushPool.setMaxThreadCount(1);

    m_flushTimer = new QTimer(this);
    m_flushTimer->setSingleShot(true);
    m_flushTimer->setInterval(kFlushDebounceMs);
    connect(m_flushTimer, &QTimer::timeout, this, &QuarkMetaJsonStore::onFlushTimeout);

    m_maxFlushTimer = new QTimer(this);
    m_maxFlushTimer->setSingleShot(true);
    m_maxFlushTimer->setInterval(kMaxFlushDelayMs);
    connect(m_maxFlushTimer, &QTimer::timeout, this, &QuarkMetaJsonStore::onFlushTimeout);

    if (auto* app = QCoreApplication::instance()) {
        this->moveToThread(app->thread());
        connect(app, &QCoreApplication::aboutToQuit, this, [this]() {
            flushAllDirtyBuffers(true);
            m_flushPool.waitForDone();
        });
    }
}

QuarkMetaJsonStore::~QuarkMetaJsonStore() {
    flushAllDirtyBuffers(true);
    m_flushPool.waitForDone();
}

std::wstring QuarkMetaJsonStore::normalizeFolderPath(const std::wstring& path) {
    if (path.empty()) return L"";
    QString qp = QDir::toNativeSeparators(QDir::cleanPath(QString::fromStdWString(path))).toLower();
    if (qp.length() == 2 && qp.endsWith(':')) qp += '\\';
    return qp.toStdWString();
}

void QuarkMetaJsonStore::prefetchFolderJson(const std::wstring& folderPath) {
    if (folderPath.empty()) return;
    std::wstring normFolder = normalizeFolderPath(folderPath);

    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        if (m_folderCacheMap.find(normFolder) != m_folderCacheMap.end()) {
            return;
        }
    }

    auto jsonPtr = std::make_shared<QuarkMetaJson>(folderPath);
    jsonPtr->load();

    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        m_folderCacheMap.emplace(normFolder, jsonPtr);
    }
}

void QuarkMetaJsonStore::addChangeListener(ChangeListener listener) {
    if (listener) {
        std::lock_guard<std::mutex> lock(m_listenerMutex);
        m_listeners.push_back(listener);
    }
}

void QuarkMetaJsonStore::notifyListeners(const ChangeEvent& event) {
    std::vector<ChangeListener> listenersCopy;
    {
        std::lock_guard<std::mutex> lock(m_listenerMutex);
        listenersCopy = m_listeners;
    }
    for (const auto& listener : listenersCopy) {
        try { listener(event); } catch (...) {}
    }
}

std::shared_ptr<QuarkMetaJson> QuarkMetaJsonStore::getOrLoadFolderLocked(const std::wstring& normFolder, const std::wstring& folderPath) {
    auto it = m_folderCacheMap.find(normFolder);
    if (it != m_folderCacheMap.end() && it->second) {
        return it->second;
    }

    auto jsonPtr = std::make_shared<QuarkMetaJson>(folderPath);
    jsonPtr->load();
    m_folderCacheMap[normFolder] = jsonPtr;
    return jsonPtr;
}

void QuarkMetaJsonStore::updateItemMeta(const std::wstring& filePath, std::function<void(ItemMeta&)> updater) {
    if (!updater) return;

    QFileInfo info(QString::fromStdWString(filePath));
    std::wstring folderPath = info.absolutePath().toStdWString();
    std::wstring fileName = info.fileName().toLower().toStdWString();
    std::wstring normFolder = normalizeFolderPath(folderPath);

    prefetchFolderJson(folderPath);

    if (info.isDir()) {
        prefetchFolderJson(info.absoluteFilePath().toStdWString());
    }

    {
        std::lock_guard<std::mutex> lock(m_storeMutex);

        auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);

        ItemMeta& meta = jsonPtr->items()[fileName];
        meta.type = info.isDir() ? L"folder" : L"file";
        updater(meta);

        markDirtyLocked(normFolder);

        // 如果是文件夹，同步更新自身目录内部的 .QuarkMeta.json 镜像
        if (info.isDir()) {
            std::wstring selfDirPath = info.absoluteFilePath().toStdWString();
            std::wstring normSelfDir = normalizeFolderPath(selfDirPath);

            auto selfJsonPtr = getOrLoadFolderLocked(normSelfDir, selfDirPath);

            FolderMeta& fMeta = selfJsonPtr->folder();
            ItemMeta dummyItem;
            dummyItem.rating = fMeta.rating;
            dummyItem.color = fMeta.color;
            dummyItem.pinned = fMeta.pinned;
            dummyItem.note = fMeta.note;
            dummyItem.url = fMeta.url;
            dummyItem.encrypted = fMeta.encrypted;
            dummyItem.encryptSalt = fMeta.encryptSalt;
            dummyItem.encryptIv = fMeta.encryptIv;
            dummyItem.encryptVerifyHash = fMeta.encryptVerifyHash;
            dummyItem.folderId = fMeta.folderId;
            dummyItem.tags = fMeta.tags;
            dummyItem.palettes = fMeta.palettes;

            updater(dummyItem);

            fMeta.rating = dummyItem.rating;
            fMeta.color = dummyItem.color;
            fMeta.pinned = dummyItem.pinned;
            fMeta.note = dummyItem.note;
            fMeta.url = dummyItem.url;
            fMeta.encrypted = dummyItem.encrypted;
            fMeta.encryptSalt = dummyItem.encryptSalt;
            fMeta.encryptIv = dummyItem.encryptIv;
            fMeta.encryptVerifyHash = dummyItem.encryptVerifyHash;
            fMeta.folderId = dummyItem.folderId;
            fMeta.tags = dummyItem.tags;
            fMeta.palettes = dummyItem.palettes;

            markDirtyLocked(normSelfDir);
        }
    }

    scheduleFlush();
    notifyListeners({ChangeType::ItemChanged, QDir::toNativeSeparators(QString::fromStdWString(filePath)), ""});
}

void QuarkMetaJsonStore::updateFolderItemsBatch(const std::wstring& folderPath,
                                            const std::vector<std::pair<std::wstring, std::function<void(ItemMeta&)>>>& updaters) {
    if (folderPath.empty() || updaters.empty()) return;

    prefetchFolderJson(folderPath);
    std::wstring normFolder = normalizeFolderPath(folderPath);

    std::vector<QString> changedPaths;
    changedPaths.reserve(updaters.size());

    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);

        for (const auto& [fileName, updater] : updaters) {
            if (!updater || fileName.empty()) continue;
            std::wstring key = QuarkMetaJson::normalizeItemKey(fileName);
            ItemMeta& meta = jsonPtr->items()[key];
            updater(meta);

            QString fullPath = QDir::toNativeSeparators(QDir(QString::fromStdWString(folderPath)).filePath(QString::fromStdWString(fileName)));
            changedPaths.push_back(fullPath);
        }

        markDirtyLocked(normFolder);
    }

    scheduleFlush();

    for (const auto& p : changedPaths) {
        notifyListeners({ChangeType::ItemChanged, p, ""});
    }
}

QMap<QString, QStringList> QuarkMetaJsonStore::addTagsBatch(const QStringList& paths, const QStringList& tags) {
    QMap<QString, QStringList> resultMap;
    if (paths.isEmpty()) return resultMap;

    struct PathItem {
        QString nativePath;
        QFileInfo info;
        std::wstring folderPath;
        std::wstring normFolder;
        std::wstring fileNameKey;
    };

    std::unordered_map<std::wstring, std::vector<PathItem>> folderGroups;
    std::unordered_set<std::wstring> uniqueFoldersToPrefetch;

    for (const QString& rawPath : paths) {
        if (rawPath.isEmpty()) continue;
        QString nativePath = QDir::toNativeSeparators(rawPath);
        QFileInfo info(nativePath);
        std::wstring folderPath = info.absolutePath().toStdWString();
        std::wstring normFolder = normalizeFolderPath(folderPath);

        uniqueFoldersToPrefetch.insert(folderPath);
        if (info.isDir()) {
            uniqueFoldersToPrefetch.insert(info.absoluteFilePath().toStdWString());
        }

        PathItem item;
        item.nativePath = nativePath;
        item.info = info;
        item.folderPath = folderPath;
        item.normFolder = normFolder;
        item.fileNameKey = info.fileName().toLower().toStdWString();

        folderGroups[normFolder].push_back(std::move(item));
    }

    for (const auto& fPath : uniqueFoldersToPrefetch) {
        prefetchFolderJson(fPath);
    }

    for (auto& pair : folderGroups) {
        const std::wstring& normFolder = pair.first;
        const auto& itemsInFolder = pair.second;
        if (itemsInFolder.empty()) continue;

        std::lock_guard<std::mutex> lock(m_storeMutex);

        auto jsonPtr = getOrLoadFolderLocked(normFolder, itemsInFolder.front().folderPath);

        for (const auto& item : itemsInFolder) {
            ItemMeta& meta = jsonPtr->items()[item.fileNameKey];
            if (meta.type.empty()) {
                meta.type = item.info.isDir() ? L"folder" : L"file";
            }

            QStringList finalTags = mergeAddTagsInternal(meta.tags, tags);
            meta.tags = toStdWStringVector(finalTags);
            resultMap[item.nativePath] = finalTags;

            if (item.info.isDir()) {
                std::wstring selfDirPath = item.info.absoluteFilePath().toStdWString();
                std::wstring selfNorm = normalizeFolderPath(selfDirPath);
                auto selfJsonPtr = getOrLoadFolderLocked(selfNorm, selfDirPath);
                selfJsonPtr->folder().tags = meta.tags;
                markDirtyLocked(selfNorm);
            }
        }

        markDirtyLocked(normFolder);
    }

    scheduleFlush();

    for (auto it = resultMap.begin(); it != resultMap.end(); ++it) {
        notifyListeners({ChangeType::ItemChanged, it.key(), ""});
    }

    return resultMap;
}

QMap<QString, QStringList> QuarkMetaJsonStore::removeTagsBatch(const QStringList& paths, const QStringList& tags) {
    QMap<QString, QStringList> resultMap;
    if (paths.isEmpty()) return resultMap;

    struct PathItem {
        QString nativePath;
        QFileInfo info;
        std::wstring folderPath;
        std::wstring normFolder;
        std::wstring fileNameKey;
    };

    std::unordered_map<std::wstring, std::vector<PathItem>> folderGroups;
    std::unordered_set<std::wstring> uniqueFoldersToPrefetch;

    for (const QString& rawPath : paths) {
        if (rawPath.isEmpty()) continue;
        QString nativePath = QDir::toNativeSeparators(rawPath);
        QFileInfo info(nativePath);
        std::wstring folderPath = info.absolutePath().toStdWString();
        std::wstring normFolder = normalizeFolderPath(folderPath);

        uniqueFoldersToPrefetch.insert(folderPath);
        if (info.isDir()) {
            uniqueFoldersToPrefetch.insert(info.absoluteFilePath().toStdWString());
        }

        PathItem item;
        item.nativePath = nativePath;
        item.info = info;
        item.folderPath = folderPath;
        item.normFolder = normFolder;
        item.fileNameKey = info.fileName().toLower().toStdWString();

        folderGroups[normFolder].push_back(std::move(item));
    }

    for (const auto& fPath : uniqueFoldersToPrefetch) {
        prefetchFolderJson(fPath);
    }

    {
        std::lock_guard<std::mutex> lock(m_storeMutex);

        for (auto& pair : folderGroups) {
            const std::wstring& normFolder = pair.first;
            const auto& itemsInFolder = pair.second;
            if (itemsInFolder.empty()) continue;

            auto jsonPtr = getOrLoadFolderLocked(normFolder, itemsInFolder.front().folderPath);

            for (const auto& item : itemsInFolder) {
                ItemMeta& meta = jsonPtr->items()[item.fileNameKey];
                if (meta.type.empty()) {
                    meta.type = item.info.isDir() ? L"folder" : L"file";
                }

                QStringList finalTags = mergeRemoveTagsInternal(meta.tags, tags);
                meta.tags = toStdWStringVector(finalTags);
                resultMap[item.nativePath] = finalTags;

                if (item.info.isDir()) {
                    std::wstring selfDirPath = item.info.absoluteFilePath().toStdWString();
                    std::wstring selfNorm = normalizeFolderPath(selfDirPath);
                    auto selfJsonPtr = getOrLoadFolderLocked(selfNorm, selfDirPath);
                    selfJsonPtr->folder().tags = meta.tags;
                    markDirtyLocked(selfNorm);
                }
            }

            markDirtyLocked(normFolder);
        }
    }

    scheduleFlush();

    for (auto it = resultMap.begin(); it != resultMap.end(); ++it) {
        notifyListeners({ChangeType::ItemChanged, it.key(), ""});
    }

    return resultMap;
}

QString QuarkMetaJsonStore::getMetaFileId(const std::wstring& folderPath) {
    if (folderPath.empty()) return QString();
    prefetchFolderJson(folderPath);

    std::wstring normFolder = normalizeFolderPath(folderPath);
    std::lock_guard<std::mutex> lock(m_storeMutex);

    auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
    return QString::fromStdString(jsonPtr->folder().metaFileId);
}

FolderMeta QuarkMetaJsonStore::getFolderMeta(const std::wstring& folderPath) {
    if (folderPath.empty()) return FolderMeta();
    prefetchFolderJson(folderPath);

    std::wstring normFolder = normalizeFolderPath(folderPath);
    std::lock_guard<std::mutex> lock(m_storeMutex);

    auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
    return jsonPtr->folder();
}

void QuarkMetaJsonStore::setFolderMeta(const std::wstring& folderPath, const FolderMeta& meta) {
    if (folderPath.empty()) return;
    prefetchFolderJson(folderPath);

    std::wstring normFolder = normalizeFolderPath(folderPath);
    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
        jsonPtr->folder() = meta;
        markDirtyLocked(normFolder);
    }
    scheduleFlush();
}

QuarkMetaJson::ItemMap QuarkMetaJsonStore::readFolderMeta(const std::wstring& folderPath) {
    if (folderPath.empty()) return {};
    prefetchFolderJson(folderPath);

    std::wstring normFolder = normalizeFolderPath(folderPath);
    std::lock_guard<std::mutex> lock(m_storeMutex);

    auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
    return jsonPtr->items();
}

bool QuarkMetaJsonStore::readItemMeta(const std::wstring& filePath, ItemMeta& outMeta) {
    QFileInfo info(QString::fromStdWString(filePath));
    std::wstring folderPath = info.absolutePath().toStdWString();
    std::wstring fileName = info.fileName().toLower().toStdWString();

    prefetchFolderJson(folderPath);

    std::wstring normFolder = normalizeFolderPath(folderPath);
    std::lock_guard<std::mutex> lock(m_storeMutex);

    auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
    const auto& itemsMap = jsonPtr->items();
    auto it = itemsMap.find(fileName);
    if (it != itemsMap.end()) {
        outMeta = it->second;
        return true;
    }
    return false;
}

void QuarkMetaJsonStore::markDirtyLocked(const std::wstring& normFolder) {
    m_dirtyFolderPaths.insert(normFolder);
    ++m_folderRevision[normFolder];
}

void QuarkMetaJsonStore::scheduleFlush() {
    QMetaObject::invokeMethod(this, [this]() {
        if (m_flushTimer) {
            m_flushTimer->start(kFlushDebounceMs);
        }
        if (m_maxFlushTimer && !m_maxFlushTimer->isActive()) {
            m_maxFlushTimer->start(kMaxFlushDelayMs);
        }
    }, Qt::QueuedConnection);
}

void QuarkMetaJsonStore::setRatingBatch(const QStringList& paths, int rating) {
    if (paths.isEmpty()) return;
    QMap<std::wstring, QStringList> folderGroups;
    for (const QString& path : paths) {
        if (path.isEmpty()) continue;
        QFileInfo fi(path);
        std::wstring normFolder = normalizeFolderPath(fi.absolutePath().toStdWString());
        folderGroups[normFolder].append(fi.fileName());
    }

    for (auto it = folderGroups.begin(); it != folderGroups.end(); ++it) {
        std::wstring normFolder = it.key();
        std::wstring folderPath = normFolder;
        const QStringList& fileNames = it.value();

        std::lock_guard<std::mutex> lock(m_storeMutex);
        auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
        if (!jsonPtr) continue;

        auto& items = jsonPtr->items();
        for (const QString& fileName : fileNames) {
            std::wstring key = QuarkMetaJson::normalizeItemKey(fileName.toStdWString());
            items[key].rating = rating;
        }
        markDirtyLocked(normFolder);
    }

    scheduleFlush();
    for (const QString& path : paths) {
        notifyListeners({ChangeType::ItemChanged, QDir::toNativeSeparators(path), ""});
    }
}

void QuarkMetaJsonStore::setColorBatch(const QStringList& paths, const QString& color) {
    if (paths.isEmpty()) return;
    QMap<std::wstring, QStringList> folderGroups;
    for (const QString& path : paths) {
        if (path.isEmpty()) continue;
        QFileInfo fi(path);
        std::wstring normFolder = normalizeFolderPath(fi.absolutePath().toStdWString());
        folderGroups[normFolder].append(fi.fileName());
    }

    for (auto it = folderGroups.begin(); it != folderGroups.end(); ++it) {
        std::wstring normFolder = it.key();
        std::wstring folderPath = normFolder;
        const QStringList& fileNames = it.value();

        std::lock_guard<std::mutex> lock(m_storeMutex);
        auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
        if (!jsonPtr) continue;

        auto& items = jsonPtr->items();
        for (const QString& fileName : fileNames) {
            std::wstring key = QuarkMetaJson::normalizeItemKey(fileName.toStdWString());
            items[key].color = color.toStdWString();
        }
        markDirtyLocked(normFolder);
    }

    scheduleFlush();
    for (const QString& path : paths) {
        notifyListeners({ChangeType::ItemChanged, QDir::toNativeSeparators(path), ""});
    }
}

void QuarkMetaJsonStore::setNoteBatch(const QStringList& paths, const QString& note) {
    if (paths.isEmpty()) return;
    QMap<std::wstring, QStringList> folderGroups;
    for (const QString& path : paths) {
        if (path.isEmpty()) continue;
        QFileInfo fi(path);
        std::wstring normFolder = normalizeFolderPath(fi.absolutePath().toStdWString());
        folderGroups[normFolder].append(fi.fileName());
    }

    for (auto it = folderGroups.begin(); it != folderGroups.end(); ++it) {
        std::wstring normFolder = it.key();
        std::wstring folderPath = normFolder;
        const QStringList& fileNames = it.value();

        std::lock_guard<std::mutex> lock(m_storeMutex);
        auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
        if (!jsonPtr) continue;

        auto& items = jsonPtr->items();
        for (const QString& fileName : fileNames) {
            std::wstring key = QuarkMetaJson::normalizeItemKey(fileName.toStdWString());
            items[key].note = note.toStdWString();
        }
        markDirtyLocked(normFolder);
    }

    scheduleFlush();
    for (const QString& path : paths) {
        notifyListeners({ChangeType::ItemChanged, QDir::toNativeSeparators(path), ""});
    }
}

void QuarkMetaJsonStore::setURLBatch(const QStringList& paths, const QString& url) {
    if (paths.isEmpty()) return;
    QMap<std::wstring, QStringList> folderGroups;
    for (const QString& path : paths) {
        if (path.isEmpty()) continue;
        QFileInfo fi(path);
        std::wstring normFolder = normalizeFolderPath(fi.absolutePath().toStdWString());
        folderGroups[normFolder].append(fi.fileName());
    }

    for (auto it = folderGroups.begin(); it != folderGroups.end(); ++it) {
        std::wstring normFolder = it.key();
        std::wstring folderPath = normFolder;
        const QStringList& fileNames = it.value();

        std::lock_guard<std::mutex> lock(m_storeMutex);
        auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
        if (!jsonPtr) continue;

        auto& items = jsonPtr->items();
        for (const QString& fileName : fileNames) {
            std::wstring key = QuarkMetaJson::normalizeItemKey(fileName.toStdWString());
            items[key].url = url.toStdWString();
        }
        markDirtyLocked(normFolder);
    }

    scheduleFlush();
    for (const QString& path : paths) {
        notifyListeners({ChangeType::ItemChanged, QDir::toNativeSeparators(path), ""});
    }
}

void QuarkMetaJsonStore::setPinnedBatch(const QStringList& paths, bool pinned) {
    if (paths.isEmpty()) return;
    QMap<std::wstring, QStringList> folderGroups;
    for (const QString& path : paths) {
        if (path.isEmpty()) continue;
        QFileInfo fi(path);
        std::wstring normFolder = normalizeFolderPath(fi.absolutePath().toStdWString());
        folderGroups[normFolder].append(fi.fileName());
    }

    for (auto it = folderGroups.begin(); it != folderGroups.end(); ++it) {
        std::wstring normFolder = it.key();
        std::wstring folderPath = normFolder;
        const QStringList& fileNames = it.value();

        std::lock_guard<std::mutex> lock(m_storeMutex);
        auto jsonPtr = getOrLoadFolderLocked(normFolder, folderPath);
        if (!jsonPtr) continue;

        auto& items = jsonPtr->items();
        for (const QString& fileName : fileNames) {
            std::wstring key = QuarkMetaJson::normalizeItemKey(fileName.toStdWString());
            items[key].pinned = pinned;
        }
        markDirtyLocked(normFolder);
    }

    scheduleFlush();
    for (const QString& path : paths) {
        notifyListeners({ChangeType::ItemChanged, QDir::toNativeSeparators(path), ""});
    }
}

bool QuarkMetaJsonStore::saveTicket(const FlushTicket& ticket) {
    if (ticket.snapshot.isLoadFailed()) {
        return false;
    }
    std::lock_guard<std::mutex> saveLock(m_saveMutex);
    uint64_t& saved = m_savedRevision[ticket.normFolder];
    if (saved >= ticket.revision) return true;   // 更新的快照已落盘，丢弃过期快照视作成功
    bool ok = ticket.snapshot.save();
    if (ok) {
        saved = ticket.revision;
    } else {
        // 写盘失败容错保护：重新将该目录标记为脏缓冲区，防止丢数据
        std::lock_guard<std::mutex> lock(m_storeMutex);
        m_dirtyFolderPaths.insert(ticket.normFolder);
        scheduleFlush();
    }
    return ok;
}

void QuarkMetaJsonStore::onFlushTimeout() {
    if (m_flushTimer) m_flushTimer->stop();
    if (m_maxFlushTimer) m_maxFlushTimer->stop();
    flushAllDirtyBuffers();
}

void QuarkMetaJsonStore::flushAllDirtyBuffers(bool sync) {
    std::vector<FlushTicket> tickets;
    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        if (m_dirtyFolderPaths.empty()) return;
        for (const auto& normFolder : m_dirtyFolderPaths) {
            auto it = m_folderCacheMap.find(normFolder);
            if (it != m_folderCacheMap.end() && it->second) {
                tickets.emplace_back(normFolder, m_folderRevision[normFolder], *(it->second));
            }
        }
        m_dirtyFolderPaths.clear();
    }
    if (tickets.empty()) return;
    if (sync) {
        for (const auto& t : tickets) saveTicket(t);
    } else {
        m_flushPool.start([this, tickets]() {
            for (const auto& t : tickets) saveTicket(t);
        });
    }
}

void QuarkMetaJsonStore::flushAllDirtyBuffersAsync(std::function<void(bool allOk)> onDone) {
    std::vector<FlushTicket> tickets;
    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        if (!m_dirtyFolderPaths.empty()) {
            for (const auto& normFolder : m_dirtyFolderPaths) {
                auto it = m_folderCacheMap.find(normFolder);
                if (it != m_folderCacheMap.end() && it->second) {
                    tickets.emplace_back(normFolder, m_folderRevision[normFolder], *(it->second));
                }
            }
            m_dirtyFolderPaths.clear();
        }
    }

    if (tickets.empty()) {
        if (onDone) {
            QMetaObject::invokeMethod(qApp, [onDone]() {
                onDone(true);
            }, Qt::QueuedConnection);
        }
        return;
    }

    m_flushPool.start([this, tickets, onDone]() {
        bool allOk = true;
        for (const auto& t : tickets) {
            if (!saveTicket(t)) {
                allOk = false;
            }
        }
        if (onDone) {
            QMetaObject::invokeMethod(qApp, [onDone, allOk]() {
                onDone(allOk);
            }, Qt::QueuedConnection);
        }
    });
}

void QuarkMetaJsonStore::invalidateFolderCache(const std::wstring& folderPath) {
    std::wstring normFolder = normalizeFolderPath(folderPath);
    std::lock_guard<std::mutex> lock(m_storeMutex);
    auto it = m_folderCacheMap.find(normFolder);
    if (it != m_folderCacheMap.end() && it->second && m_dirtyFolderPaths.count(normFolder) > 0) {
        FlushTicket ticket(normFolder, m_folderRevision[normFolder], *(it->second));
        saveTicket(ticket);   // 先同步落盘再驱逐，防止丢失未落盘修改
    }
    m_dirtyFolderPaths.erase(normFolder);
    m_folderCacheMap.erase(normFolder);
}

bool QuarkMetaJsonStore::renameItem(const QString& folderPath, const QString& oldName, const QString& newName) {
    if (oldName == newName) return true;
    std::wstring wFolder = folderPath.toStdWString();
    prefetchFolderJson(wFolder);
    std::wstring normFolder = normalizeFolderPath(wFolder);
    std::wstring oldKey = QuarkMetaJson::normalizeItemKey(oldName.toStdWString());
    std::wstring newKey = QuarkMetaJson::normalizeItemKey(newName.toStdWString());
    bool ok = true;
    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        auto json = getOrLoadFolderLocked(normFolder, wFolder);
        if (json->isLoadFailed()) {
            ok = false;
        } else {
            auto& items = json->items();
            auto it = items.find(oldKey);
            if (it != items.end()) {
                ItemMeta meta = it->second;
                items.erase(it);
                items[newKey] = meta;
                markDirtyLocked(normFolder);
            }
        }
    }
    scheduleFlush();
    if (ok) {
        QString oldPath = QDir::toNativeSeparators(QDir(folderPath).filePath(oldName));
        QString newPath = QDir::toNativeSeparators(QDir(folderPath).filePath(newName));
        notifyListeners({ChangeType::ItemRenamed, oldPath, newPath});
    }
    return ok;
}

bool QuarkMetaJsonStore::migrateFolderCache(const QString& oldFolderPath, const QString& newFolderPath) {
    flushAllDirtyBuffers(true);
    bool ok = QuarkMetaJson::migrateFolderCache(oldFolderPath, newFolderPath);

    std::wstring oldNorm = normalizeFolderPath(oldFolderPath.toStdWString());
    std::wstring newNorm = normalizeFolderPath(newFolderPath.toStdWString());

    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        std::vector<std::wstring> keysToMove;
        for (const auto& [key, ptr] : m_folderCacheMap) {
            if (key == oldNorm || (key.length() > oldNorm.length() && key.compare(0, oldNorm.length(), oldNorm) == 0)) {
                keysToMove.push_back(key);
            }
        }
        for (const auto& oldKey : keysToMove) {
            auto ptr = m_folderCacheMap[oldKey];
            m_folderCacheMap.erase(oldKey);

            std::wstring subPath = oldKey.substr(oldNorm.length());
            std::wstring newKey = newNorm + subPath;
            m_folderCacheMap[newKey] = ptr;

            if (m_dirtyFolderPaths.count(oldKey) > 0) {
                m_dirtyFolderPaths.erase(oldKey);
                m_dirtyFolderPaths.insert(newKey);
            }
        }
    }

    invalidateFolderCache(oldFolderPath.toStdWString());
    invalidateFolderCache(newFolderPath.toStdWString());
    if (ok) {
        notifyListeners({ChangeType::FolderMigrated, QDir::toNativeSeparators(oldFolderPath), QDir::toNativeSeparators(newFolderPath)});
    }
    return ok;
}

bool QuarkMetaJsonStore::removeItem(const QString& filePath) {
    if (filePath.isEmpty()) return true;
    QFileInfo info(filePath);
    std::wstring folderPath = QDir::toNativeSeparators(info.absolutePath()).toStdWString();
    std::wstring key = QuarkMetaJson::normalizeItemKey(info.fileName().toStdWString());
    prefetchFolderJson(folderPath);
    std::wstring normFolder = normalizeFolderPath(folderPath);
    bool ok = true;
    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        auto json = getOrLoadFolderLocked(normFolder, folderPath);
        if (json->isLoadFailed()) {
            ok = false;
        } else if (json->items().erase(key) > 0) {
            markDirtyLocked(normFolder);
        }
    }
    scheduleFlush();
    if (ok) {
        notifyListeners({ChangeType::ItemRemoved, QDir::toNativeSeparators(filePath), ""});
    }
    return ok;
}

bool QuarkMetaJsonStore::transferItems(const QList<QPair<QString, QString>>& pairs, bool isMove, bool replaceDestWhenSourceMissing) {
    if (pairs.isEmpty()) return false;

    struct Entry {
        std::wstring srcFolder, destFolder, srcNorm, destNorm, srcKey, destKey;
        ItemMeta meta;
        bool hasMeta = false;
        bool valid = true;
    };
    std::vector<Entry> entries;
    entries.reserve(pairs.size());
    for (const auto& pair : pairs) {
        if (pair.first == pair.second) continue;
        QFileInfo oldInfo(pair.first);
        QFileInfo newInfo(pair.second);
        Entry e;
        e.srcFolder = QDir::toNativeSeparators(oldInfo.absolutePath()).toStdWString();
        e.destFolder = QDir::toNativeSeparators(newInfo.absolutePath()).toStdWString();
        e.srcNorm = normalizeFolderPath(e.srcFolder);
        e.destNorm = normalizeFolderPath(e.destFolder);
        e.srcKey = QuarkMetaJson::normalizeItemKey(oldInfo.fileName().toStdWString());
        e.destKey = QuarkMetaJson::normalizeItemKey(newInfo.fileName().toStdWString());
        entries.push_back(std::move(e));
    }
    if (entries.empty()) return true;

    for (const auto& e : entries) {
        prefetchFolderJson(e.srcFolder);
        prefetchFolderJson(e.destFolder);
    }

    bool allOk = true;
    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        // 阶段一：读取源条目；移动时从源抹除
        for (auto& e : entries) {
            auto srcJson = getOrLoadFolderLocked(e.srcNorm, e.srcFolder);
            auto destJson = getOrLoadFolderLocked(e.destNorm, e.destFolder);
            if (srcJson->isLoadFailed() || destJson->isLoadFailed()) {
                e.valid = false;
                allOk = false;
                continue;
            }
            auto& srcItems = srcJson->items();
            auto it = srcItems.find(e.srcKey);
            if (it != srcItems.end()) {
                e.meta = it->second;
                e.hasMeta = true;
                if (isMove) {
                    srcItems.erase(it);
                    markDirtyLocked(e.srcNorm);
                }
            }
        }
        // 阶段二：写入目标
        for (auto& e : entries) {
            if (!e.valid) continue;
            auto destJson = getOrLoadFolderLocked(e.destNorm, e.destFolder);
            auto& destItems = destJson->items();
            if (e.hasMeta) {
                destItems[e.destKey] = e.meta;
                markDirtyLocked(e.destNorm);
            } else if (replaceDestWhenSourceMissing) {
                if (destItems.erase(e.destKey) > 0) markDirtyLocked(e.destNorm);
            }
        }
    }
    scheduleFlush();

    if (allOk) {
        for (const auto& pair : pairs) {
            if (isMove) {
                notifyListeners({ChangeType::ItemRenamed, QDir::toNativeSeparators(pair.first), QDir::toNativeSeparators(pair.second)});
            } else {
                notifyListeners({ChangeType::ItemChanged, QDir::toNativeSeparators(pair.second), ""});
            }
        }
    }
    return allOk;
}

} // namespace QuarkMeta
