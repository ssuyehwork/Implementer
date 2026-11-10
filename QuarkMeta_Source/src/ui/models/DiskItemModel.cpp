#include "DiskItemModel.h"
#include "../../core/ModelContract.h"
#include "UiHelper.h"
#include "ShellIconManager.h"
#include "MetaCacheDecorator.h"
#include "ThumbnailPipelineService.h"
#include "ModelContract.h"
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <limits>
#include <QThreadPool>
#include "../../meta/QuarkMetaJson.h"
#include "../../meta/QuarkMetaJsonStore.h"
#include "../../meta/MetadataDefs.h"
#include "CoreController.h"
#include "DiskMediaExtractor.h"
#include "FileOperationHelper.h"
#include "MetadataManager.h"
#include "DriveMetaDao.h"
#include "../../core/LastOperationManager.h"
#include "../Logger.h"
#include <QElapsedTimer>

namespace QuarkMeta {

QThreadPool* DiskItemModel::thumbnailPool() {
    static QThreadPool pool;
    static std::once_flag flag;
    std::call_once(flag, []() {
        pool.setMaxThreadCount(qMax(2, QThread::idealThreadCount() / 2));
    });
    return &pool;
}

void DiskItemModel::incrementGeneration() {
    m_currentGen.fetch_add(1, std::memory_order_relaxed);
    m_requestedPaths.clear();
}

DiskItemModel::DiskItemModel(QObject* parent) : ItemModelBase(parent) {
    m_iconCache.setMaxCost(500);
    m_thumbBatchTimer = new QTimer(this);
    m_thumbBatchTimer->setSingleShot(true);
    m_thumbBatchTimer->setInterval(35);
    connect(m_thumbBatchTimer, &QTimer::timeout, this, &DiskItemModel::flushPendingThumbDataChanged);
}

void DiskItemModel::flushPendingThumbDataChanged() {
    if (m_pendingThumbRows.isEmpty()) return;

    QList<int> sortedRows = m_pendingThumbRows.values();
    m_pendingThumbRows.clear();
    std::sort(sortedRows.begin(), sortedRows.end());

    int startRow = -1;
    int prevRow = -1;

    for (int r : sortedRows) {
        if (r < 0 || r >= static_cast<int>(m_allRecords.size())) continue;
        if (startRow == -1) {
            startRow = r;
            prevRow = r;
        } else if (r == prevRow + 1) {
            prevRow = r;
        } else {
            emit dataChanged(index(startRow, 0), index(prevRow, columnCount() - 1),
                             {Qt::DecorationRole, HasThumbnailRole});
            startRow = r;
            prevRow = r;
        }
    }
    if (startRow != -1) {
        emit dataChanged(index(startRow, 0), index(prevRow, columnCount() - 1),
                         {Qt::DecorationRole, HasThumbnailRole});
    }
}

DiskItemModel::~DiskItemModel() {}

int DiskItemModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_allRecords.size());
}

int DiskItemModel::columnCount(const QModelIndex&) const {
    return static_cast<int>(FileListColumn::Count);
}

QVariant DiskItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (static_cast<FileListColumn>(section)) {
            case FileListColumn::Name: return QString("名称");
            case FileListColumn::Status: return QString("状态");
            case FileListColumn::Rating: return QString("评分");
            case FileListColumn::Dimension: return QString("尺寸");
            case FileListColumn::Type: return QString("类型");
            case FileListColumn::Size: return QString("大小");
            case FileListColumn::CreatedDate: return QString("创建日期");
            case FileListColumn::ModifiedDate: return QString("修改日期");
            default: break;
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void DiskItemModel::addItemRecord(const ItemRecord& record) {
    appendRecord(record);
}

void DiskItemModel::appendRecord(const ItemRecord& record) {
    appendRecords({record});
}

void DiskItemModel::appendRecords(const std::vector<ItemRecord>& records) {
    if (records.empty()) return;

    QElapsedTimer timer;
    timer.start();

    std::vector<ItemRecord> newRecords = records;
    QStringList addedPaths;
    addedPaths.reserve(static_cast<qsizetype>(newRecords.size()));

    for (auto& rec : newRecords) {
        if (rec.isDir || !ColorPaletteEngine::isGraphicsFile(rec.suffix.toLower())) {
            rec.thumbnailState = ItemRecord::ThumbnailState::NotApplicable;
        } else if (rec.thumbStatus == 1) {
            rec.thumbnailState = ItemRecord::ThumbnailState::Failed;
        } else {
            rec.thumbnailState = ItemRecord::ThumbnailState::Pending;
        }
        addedPaths.append(rec.path);
    }

    MetadataManager::instance().seedMemoryCacheBatch(newRecords);

    int firstNewRow = static_cast<int>(m_allRecords.size());
    int lastNewRow = firstNewRow + static_cast<int>(newRecords.size()) - 1;

    beginInsertRows(QModelIndex(), firstNewRow, lastNewRow);
    m_allRecords.reserve(m_allRecords.size() + newRecords.size());
    for (size_t i = 0; i < newRecords.size(); ++i) {
        int rowIdx = firstNewRow + static_cast<int>(i);
        m_allRecords.push_back(std::move(newRecords[i]));
        m_pathToIndex[m_allRecords.back().path] = rowIdx;
    }
    m_iconCache.setMaxCost(qMax(m_iconCache.maxCost(), static_cast<int>(m_allRecords.size()) + 50));
    endInsertRows();

    preloadDimensionsAsync(addedPaths);

    qint64 ms = timer.elapsed();
    Logger::log(QString("[Perf-DiskItemModel] appendRecords 完成: 追加行数=%1, 总行数=%2, 耗时=%3ms")
        .arg(records.size()).arg(m_allRecords.size()).arg(ms));
}

void DiskItemModel::setRecords(const std::vector<ItemRecord>& records, RecordSource source) {
    if (m_thumbBatchTimer) m_thumbBatchTimer->stop();
    m_pendingThumbRows.clear();
    incrementGeneration();
    beginResetModel();
    m_allRecords = records;

    if (source == RecordSource::Disk) {
        MetaCacheDecorator::decorate(m_allRecords);
    }

    m_pathToIndex.clear();
    m_requestedPaths.clear();
    int populatedMetaCount = 0;

    std::vector<std::pair<int, QString>> pendingTargets;

    for (int i = 0; i < static_cast<int>(m_allRecords.size()); ++i) {
        auto& rec = m_allRecords[i];
        m_pathToIndex[rec.path] = i;

        if (rec.isDir || !ColorPaletteEngine::isGraphicsFile(rec.suffix.toLower())) {
            rec.thumbnailState = ItemRecord::ThumbnailState::NotApplicable;
        } else if (rec.thumbStatus == 1) {
            rec.thumbnailState = ItemRecord::ThumbnailState::Failed;
        } else {
            rec.thumbnailState = ItemRecord::ThumbnailState::Pending;
            pendingTargets.push_back({i, rec.path});
        }

        if (rec.rating > 0 || !rec.manualColor.isEmpty() || !rec.tags.isEmpty() || !rec.note.isEmpty()) {
            populatedMetaCount++;
        }
    }

    // Section F: 一次性播种内存缓存，不触发侧车 .QuarkMeta.json 写盘
    MetadataManager::instance().seedMemoryCacheBatch(m_allRecords);
    m_iconCache.setMaxCost(qMax(500, static_cast<int>(m_allRecords.size()) + 50));
    endResetModel();

    if (!pendingTargets.empty()) {
        uint64_t thisGen = m_currentGen.load(std::memory_order_relaxed);
        QPointer<DiskItemModel> weakThis(this);

        thumbnailPool()->start([weakThis, targets = std::move(pendingTargets), thisGen]() {
            if (!weakThis || weakThis->currentGeneration() != thisGen || CoreController::isShuttingDown()) return;

            std::vector<QString> readyPaths;
            readyPaths.reserve(targets.size());

            for (const auto& target : targets) {
                if (!weakThis || weakThis->currentGeneration() != thisGen || CoreController::isShuttingDown()) return;
                QString thumbPath = DiskMediaExtractor::getDiskThumbCachePath(target.second);
                if (QFile::exists(thumbPath)) {
                    readyPaths.push_back(target.second);
                }
            }

            if (readyPaths.empty() || !weakThis || weakThis->currentGeneration() != thisGen) return;

            QMetaObject::invokeMethod(weakThis.data(), [weakThis, readyPaths = std::move(readyPaths), thisGen]() {
                if (!weakThis || weakThis->currentGeneration() != thisGen) return;

                int minRow = std::numeric_limits<int>::max();
                int maxRow = std::numeric_limits<int>::min();

                for (const QString& path : readyPaths) {
                    auto it = weakThis->m_pathToIndex.find(path);
                    if (it != weakThis->m_pathToIndex.end()) {
                        int r = it->second;
                        if (r >= 0 && r < static_cast<int>(weakThis->m_allRecords.size())) {
                            weakThis->m_allRecords[r].thumbnailState = ItemRecord::ThumbnailState::Ready;
                            if (r < minRow) minRow = r;
                            if (r > maxRow) maxRow = r;
                        }
                    }
                }

                std::vector<int> sortedRows;
                sortedRows.reserve(readyPaths.size());
                for (const QString& path : readyPaths) {
                    auto it = weakThis->m_pathToIndex.find(path);
                    if (it != weakThis->m_pathToIndex.end() && it->second >= 0 && it->second < static_cast<int>(weakThis->m_allRecords.size())) {
                        sortedRows.push_back(it->second);
                    }
                }
                std::sort(sortedRows.begin(), sortedRows.end());
                sortedRows.erase(std::unique(sortedRows.begin(), sortedRows.end()), sortedRows.end());

                int startRow = -1;
                int prevRow = -1;
                for (int r : sortedRows) {
                    if (startRow == -1) {
                        startRow = r;
                        prevRow = r;
                    } else if (r == prevRow + 1) {
                        prevRow = r;
                    } else {
                        emit weakThis->dataChanged(
                            weakThis->index(startRow, 0),
                            weakThis->index(prevRow, weakThis->columnCount() - 1),
                            {HasThumbnailRole}
                        );
                        startRow = r;
                        prevRow = r;
                    }
                }
                if (startRow != -1) {
                    emit weakThis->dataChanged(
                        weakThis->index(startRow, 0),
                        weakThis->index(prevRow, weakThis->columnCount() - 1),
                        {HasThumbnailRole}
                    );
                }
            }, Qt::QueuedConnection);
        });
    }

    preloadDimensionsAsync();
}

struct SizeTarget {
    int index;
    QString path;
    QString suffix;
};

void DiskItemModel::preloadDimensionsAsync(const QStringList& targetPaths) {
    uint64_t thisGen = m_currentGen.load(std::memory_order_relaxed);

    QSet<QString> filterSet;
    if (!targetPaths.isEmpty()) {
        for (const QString& tp : targetPaths) {
            filterSet.insert(QDir::cleanPath(tp).toLower());
        }
    }

    std::vector<SizeTarget> targets;
    targets.reserve(m_allRecords.size());
    for (int i = 0; i < static_cast<int>(m_allRecords.size()); ++i) {
        const auto& rec = m_allRecords[i];
        if (!rec.isDir && rec.width == 0 && ColorPaletteEngine::isGraphicsFile(rec.suffix.toLower())) {
            if (!filterSet.isEmpty() && !filterSet.contains(QDir::cleanPath(rec.path).toLower())) {
                continue;
            }
            targets.push_back({i, rec.path, rec.suffix});
        }
    }
    if (targets.empty()) return;

    QPointer<DiskItemModel> weakThis(this);

    thumbnailPool()->start([weakThis, targets = std::move(targets), thisGen]() {
        if (!weakThis || weakThis->currentGeneration() != thisGen || CoreController::isShuttingDown()) return;

        std::unordered_map<std::wstring, std::pair<int, int>> dimMap;
        std::vector<std::pair<QString, QSize>> resolvedSizes;

        for (const auto& target : targets) {
            if (!weakThis || weakThis->currentGeneration() != thisGen || CoreController::isShuttingDown()) return;

            QSize sz = DiskMediaExtractor::fastExtractImageSize(target.path);
            if (sz.isValid() && sz.width() > 0) {
                QFileInfo fi(target.path);
                dimMap[fi.fileName().toStdWString()] = {sz.width(), sz.height()};
                resolvedSizes.push_back({target.path, sz});
            }
        }

        if (dimMap.empty() || !weakThis || weakThis->currentGeneration() != thisGen) return;

        for (const auto& item : resolvedSizes) {
            QuarkMetaJsonStore::instance().updateItemMeta(item.first.toStdWString(), [sz = item.second](ItemMeta& meta) {
                meta.width = sz.width();
                meta.height = sz.height();
            });
        }

        QMetaObject::invokeMethod(weakThis.data(), [weakThis, resolvedSizes = std::move(resolvedSizes), thisGen]() {
            if (!weakThis || weakThis->currentGeneration() != thisGen) return;

            for (const auto& item : resolvedSizes) {
                const QString& path = item.first;
                const QSize& sz = item.second;

                auto it = weakThis->m_pathToIndex.find(path);
                if (it != weakThis->m_pathToIndex.end()) {
                    int rIdx = it->second;
                    if (rIdx >= 0 && rIdx < static_cast<int>(weakThis->m_allRecords.size())) {
                        auto& rec = weakThis->m_allRecords[rIdx];
                        rec.width = sz.width();
                        rec.height = sz.height();
                        weakThis->m_aspectRatios[QDir::toNativeSeparators(path)] = (double)sz.width() / sz.height();
                    }
                }
            }

            int minRow = std::numeric_limits<int>::max();
            int maxRow = std::numeric_limits<int>::min();

            for (const auto& item : resolvedSizes) {
                const QString& path = item.first;
                auto it = weakThis->m_pathToIndex.find(path);
                if (it != weakThis->m_pathToIndex.end()) {
                    int r = it->second;
                    if (r >= 0 && r < static_cast<int>(weakThis->m_allRecords.size())) {
                        if (r < minRow) minRow = r;
                        if (r > maxRow) maxRow = r;
                    }
                }
            }

            if (minRow <= maxRow && minRow >= 0) {
                emit weakThis->dataChanged(
                    weakThis->index(minRow, 0),
                    weakThis->index(maxRow, weakThis->columnCount() - 1),
                    {Qt::DisplayRole, AspectRatioRole}
                );
            }
        }, Qt::QueuedConnection);
    });
}

void DiskItemModel::clear() {
    if (m_thumbBatchTimer) m_thumbBatchTimer->stop();
    m_pendingThumbRows.clear();
    incrementGeneration();
    beginResetModel();
    m_allRecords.clear();
    m_pathToIndex.clear();
    m_requestedPaths.clear();
    m_aspectRatios.clear();
    endResetModel();
}

void DiskItemModel::updateRecordMetadata(QString path) {
    if (path.isEmpty()) return;
    updateRecordMetadataBatch(QStringList{path});
}

void DiskItemModel::updateRecordMetadataBatch(const QStringList& paths) {
    if (paths.isEmpty()) return;

    QList<int> changedRows;
    for (const QString& path : paths) {
        if (path.isEmpty()) continue;
        QString nPath = QDir::toNativeSeparators(path);
        auto it = m_pathToIndex.find(nPath);
        if (it == m_pathToIndex.end()) {
            QString cleanP = QDir::cleanPath(path);
            for (auto mapIt = m_pathToIndex.begin(); mapIt != m_pathToIndex.end(); ++mapIt) {
                if (QString::compare(mapIt->first, nPath, Qt::CaseInsensitive) == 0 ||
                    QString::compare(QDir::cleanPath(mapIt->first), cleanP, Qt::CaseInsensitive) == 0) {
                    it = mapIt;
                    break;
                }
            }
        }

        if (it != m_pathToIndex.end()) {
            int i = it->second;
            if (i >= 0 && i < static_cast<int>(m_allRecords.size())) {
                auto& record = m_allRecords[i];
                QFileInfo fileInfo(nPath);

                if (fileInfo.isRoot() || nPath.endsWith(":\\") || nPath.endsWith(":/") || (nPath.length() == 2 && nPath.endsWith(':'))) {
                    std::wstring normWPath = MetadataManager::normalizePath(nPath.toStdWString());
                    auto driveRec = DriveMetaDao::getDriveMeta(normWPath);
                    record.rating = driveRec.rating;
                    record.manualColor = QString::fromStdWString(driveRec.color);
                    record.pinned = driveRec.pinned;
                    record.note = QString::fromStdWString(driveRec.note);
                    record.url = QString::fromStdWString(driveRec.url);
                } else {
                    RuntimeMeta meta = MetadataManager::instance().getMeta(nPath.toStdWString());
                    if (meta.rating == 0 && record.path != nPath) {
                        RuntimeMeta fallbackMeta = MetadataManager::instance().getMeta(record.path.toStdWString());
                        if (fallbackMeta.rating > 0) meta = fallbackMeta;
                    }

                    record.rating = meta.rating;
                    record.manualColor = QString::fromStdWString(meta.manualColor);
                    record.pinned = meta.pinned;
                    record.note = QString::fromStdWString(meta.note);
                    record.url = QString::fromStdWString(meta.url);
                    record.tags = meta.tags;
                    record.width = meta.width;
                    record.height = meta.height;
                    record.thumbStatus = meta.thumbStatus;
                    record.autoColor = QString::fromStdWString(meta.autoColor);

                    if (!record.isDir && ColorPaletteEngine::isGraphicsFile(record.suffix.toLower())) {
                        if (record.thumbStatus == 1) {
                            record.thumbnailState = ItemRecord::ThumbnailState::Failed;
                        } else if (record.thumbnailState == ItemRecord::ThumbnailState::Failed) {
                            record.thumbnailState = ItemRecord::ThumbnailState::Pending;
                        }
                    }
                    record.added_at = meta.added_at;

                    record.palettes.clear();
                    for (const auto& pe : meta.palettes) {
                        record.palettes.push_back({pe.color, pe.ratio});
                    }
                }
                changedRows.append(i);
            }
        }
    }

    if (changedRows.isEmpty()) return;

    std::sort(changedRows.begin(), changedRows.end());

    int rangeStart = changedRows.first();
    int rangeEnd = rangeStart;

    for (int i = 1; i < changedRows.size(); ++i) {
        if (changedRows[i] == rangeEnd + 1) {
            rangeEnd = changedRows[i];
        } else {
            emit dataChanged(index(rangeStart, 0), index(rangeEnd, columnCount() - 1), {Qt::DisplayRole, Qt::DecorationRole, RatingRole, ColorRole, HasThumbnailRole});
            rangeStart = changedRows[i];
            rangeEnd = rangeStart;
        }
    }
    emit dataChanged(index(rangeStart, 0), index(rangeEnd, columnCount() - 1), {Qt::DisplayRole, Qt::DecorationRole, RatingRole, ColorRole, HasThumbnailRole});
}

bool DiskItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= static_cast<int>(m_allRecords.size())) return false;

    if (role == Qt::EditRole && index.column() == 0) {
        QString newName = value.toString().trimmed();
        if (newName.isEmpty()) return false;

        auto& record = m_allRecords[index.row()];
        QString oldPath = record.path;
        QFileInfo oldInfo(oldPath);

        if (newName == oldInfo.fileName() || newName == oldInfo.completeBaseName()) return true;

        QFileInfo newInfoCheck(newName);
        QString oldSuffix = oldInfo.suffix();
        if (newInfoCheck.suffix().isEmpty() && !oldSuffix.isEmpty()) {
            newName += "." + oldSuffix;
        }

        bool success = false;
        QString destDir = oldInfo.absolutePath();
        QString newPathStr = QDir(destDir).filePath(newName);

        if (oldPath == newPathStr) {
            success = true;
        } else if (FileOperationHelper::safeRename(oldPath, newPathStr)) {
            success = true;

            QString oldThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(oldPath);
            QString newThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(newPathStr);

            if (QFile::exists(oldThumbHashPath)) {
                FileOperationHelper::safeRename(oldThumbHashPath, newThumbHashPath);
            }

            std::wstring oldW = oldInfo.absoluteFilePath().toStdWString();
            std::wstring newW = QDir(destDir).absoluteFilePath(newPathStr).toStdWString();

            MetadataManager::instance().renameItem(oldW, newW);
        }

        if (success) {
            QString newPath = QDir(oldInfo.absolutePath()).filePath(newName);
            record.path = newPath;
            record.filename = newName;
            
            m_pathToIndex.erase(oldPath);
            m_pathToIndex[newPath] = index.row();
            
            emit dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1), {Qt::EditRole, Qt::DisplayRole});
            return true;
        }
        return false;
    }

    auto& record = m_allRecords[index.row()];
    QString path = record.path;
    QFileInfo fileInfo(path);

    bool isDriveRoot = fileInfo.isRoot() || path.endsWith(":\\") || path.endsWith(":/") || (path.length() == 2 && path.endsWith(':'));
    if (isDriveRoot) {
        std::wstring normWPath = MetadataManager::normalizePath(path.toStdWString());
        DriveMetaRecord driveRec = DriveMetaDao::getDriveMeta(normWPath);
        bool driveUpdated = false;

        if (role == RatingRole) {
            int newRating = value.toInt();
            if (record.rating != newRating) {
                record.rating = newRating;
                driveRec.rating = newRating;
                driveUpdated = true;
                LastOperationManager::instance().recordSetRating(newRating);
            }
        } else if (role == ColorRole) {
            QString newColor = value.toString();
            if (record.manualColor != newColor) {
                record.manualColor = newColor;
                driveRec.color = newColor.toStdWString();
                driveUpdated = true;
                LastOperationManager::instance().recordSetColor(newColor);
            }
        } else if (role == IsLockedRole || role == PinnedRole) {
            bool pinned = value.toBool();
            if (record.pinned != pinned) {
                record.pinned = pinned;
                driveRec.pinned = pinned;
                driveUpdated = true;
            }
        }

        if (driveUpdated) {
            DriveMetaDao::saveDriveMeta(driveRec);
            emit dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1), {role});
            return true;
        }
        return false;
    }

    // 🚀【纯化模型职责】：元数据修改统一通过 CoreEngine 命令分发，严禁在 Model 内越权直写存储层
    AppCommand cmd;
    cmd.targetPaths = {path};

    if (role == RatingRole) {
        int newRating = value.toInt();
        if (record.rating != newRating) {
            record.rating = newRating;
            cmd.type = AppCommandType::SetRating;
            cmd.params["rating"] = newRating;
            CoreEngine::instance().executeCommand(cmd);
            LastOperationManager::instance().recordSetRating(newRating);
            emit dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1), {role});
            return true;
        }
    } else if (role == ColorRole) {
        QString newColor = value.toString();
        if (record.manualColor != newColor) {
            record.manualColor = newColor;
            cmd.type = AppCommandType::SetColor;
            cmd.params["color"] = newColor;
            CoreEngine::instance().executeCommand(cmd);
            LastOperationManager::instance().recordSetColor(newColor);
            emit dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1), {role});
            return true;
        }
    } else if (role == IsLockedRole || role == PinnedRole) {
        bool pinned = value.toBool();
        if (record.pinned != pinned) {
            record.pinned = pinned;
            cmd.type = AppCommandType::SetPinned;
            cmd.params["pinned"] = pinned;
            CoreEngine::instance().executeCommand(cmd);
            emit dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1), {role});
            return true;
        }
    } else if (role == TagsRole) {
        QStringList newTags = value.toStringList();
        record.tags = newTags;
        cmd.type = AppCommandType::SetTags;
        cmd.params["tags"] = newTags;
        CoreEngine::instance().executeCommand(cmd);
        LastOperationManager::instance().recordPasteTags(newTags);
        emit dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1), {role});
        return true;
    }

    if (role == IsDropTargetRole) {
        bool val = value.toBool();
        if (record.isDropTarget != val) {
            record.isDropTarget = val;
            emit dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1), {role});
            return true;
        }
    }

    return false;
}

void DiskItemModel::migrateCache(const QString& oldPath, const QString& newPath) {
    QString nativeOld = QDir::toNativeSeparators(oldPath);
    QString nativeNew = QDir::toNativeSeparators(newPath);
    QIcon* oldIconPtr = m_iconCache.take(oldPath);
    if (oldIconPtr) {
        m_iconCache.insert(nativeNew, oldIconPtr);
    }
    if (m_aspectRatios.contains(nativeOld)) {
        double ratio = m_aspectRatios.take(nativeOld);
        m_aspectRatios[nativeNew] = ratio;
    }
}

void DiskItemModel::clearCacheForFolder(const QString& folderPath) {
    QString nativeFolder = QDir::toNativeSeparators(folderPath);
    QString prefix = nativeFolder;
    if (!prefix.endsWith(QDir::separator())) prefix += QDir::separator();

    for (auto it = m_aspectRatios.begin(); it != m_aspectRatios.end(); ) {
        if (it.key() == nativeFolder || it.key().startsWith(prefix)) {
            it = m_aspectRatios.erase(it);
        } else {
            ++it;
        }
    }
}

void DiskItemModel::loadThumbnailsForRows(const QList<int>& rows) {
    if (rows.isEmpty() || CoreController::isShuttingDown()) return;

    uint64_t thisGen = m_currentGen.load(std::memory_order_relaxed);

    QStringList pathsToLoad;
    for (int r : rows) {
        if (r < 0 || r >= static_cast<int>(m_allRecords.size())) continue;
        const auto& rec = m_allRecords[r];

        QString ext = rec.suffix.toLower();
        bool isGraphic = ColorPaletteEngine::isGraphicsFile(ext);
        if (rec.isDir || !isGraphic) {
            continue;
        }

        // 自动提取失败（thumb_status == 1）是终态：只能靠右键"重新生成缩略图"恢复，这里不再重试
        if (rec.thumbnailState == ItemRecord::ThumbnailState::Failed) {
            continue;
        }

        QString path = rec.path;
        if (m_iconCache.contains(path)) {
            continue;
        }
        if (m_requestedPaths.contains(path)) {
            continue;
        }

        m_requestedPaths.insert(path);
        pathsToLoad << path;
    }

    if (pathsToLoad.isEmpty()) {
        return;
    }

    QPointer<DiskItemModel> weakThis(this);
    ThumbnailPipelineService::instance().loadBatchAsync(pathsToLoad, DiskMediaExtractor::kThumbSize, [weakThis, thisGen](const QString& path, const QPixmap& pixmap) {
        if (!weakThis) return;

        // 🚀【主线程归一化/跨线程锁隔离】：所有的 m_requestedPaths/m_iconCache/m_aspectRatios 修改必须统一置于主线程回调内，
        // 彻底杜绝后台工作线程并发写入 QMap/QCache 导致主线程遍历 QMapNode 时死循环引发的 UI 假死与“应用程序没有响应”！
        QMetaObject::invokeMethod(weakThis.data(), [weakThis, path, pixmap, thisGen]() {
            if (!weakThis || weakThis->currentGeneration() != thisGen) return;

            weakThis->m_requestedPaths.remove(path);

            if (!pixmap.isNull()) {
                QIcon icon(pixmap);
                weakThis->m_iconCache.insert(path, new QIcon(icon));
                double ar = (double)pixmap.width() / pixmap.height();
                weakThis->m_aspectRatios[QDir::toNativeSeparators(path)] = ar;
            }

            auto it = weakThis->m_pathToIndex.find(path);
            if (it != weakThis->m_pathToIndex.end()) {
                int currentIdx = it->second;
                if (currentIdx >= 0 && currentIdx < static_cast<int>(weakThis->m_allRecords.size())) {
                    if (weakThis->m_allRecords[currentIdx].path == path) {
                        if (!pixmap.isNull()) {
                            weakThis->m_allRecords[currentIdx].thumbnailState = ItemRecord::ThumbnailState::Ready;
                            weakThis->m_pendingThumbRows.insert(currentIdx);
                            if (weakThis->m_thumbBatchTimer && !weakThis->m_thumbBatchTimer->isActive()) {
                                weakThis->m_thumbBatchTimer->start();
                            }
                            emit weakThis->thumbnailLoaded(currentIdx);
                        } else {
                            weakThis->m_allRecords[currentIdx].thumbnailState = ItemRecord::ThumbnailState::Failed;
                            emit weakThis->dataChanged(
                                weakThis->index(currentIdx, 0),
                                weakThis->index(currentIdx, weakThis->columnCount() - 1),
                                {HasThumbnailRole}
                            );
                        }
                    }
                }
            }
        }, Qt::QueuedConnection);
    });
}

Qt::ItemFlags DiskItemModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return QAbstractTableModel::flags(index);
    Qt::ItemFlags f = QAbstractTableModel::flags(index) | Qt::ItemIsDragEnabled;
    if (index.column() == static_cast<int>(FileListColumn::Name)) {
        f |= Qt::ItemIsEditable;
    }
    return f;
}

QVariant DiskItemModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_allRecords.size())) return QVariant();

    const auto& record = m_allRecords[index.row()];
    QString path = record.path;

    static auto formatDateTime = [](long long msecs) -> QString {
        if (msecs <= 0) return "-";
        return QDateTime::fromMSecsSinceEpoch(msecs).toString("dd-MM-yyyy HH:mm");
    };

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (static_cast<FileListColumn>(index.column())) {
            case FileListColumn::Name: {
                int lastSlash = std::max(path.lastIndexOf('\\'), path.lastIndexOf('/'));
                if (lastSlash == -1) return path;
                QString name = path.mid(lastSlash + 1);
                if (name.isEmpty() && path.length() >= 2 && path[1] == ':') return path;
                return name;
            }
            case FileListColumn::Dimension: {
                if (record.isDir) return "-";
                if (record.width > 0 && record.height > 0) {
                    return QString("%1 x %2").arg(record.width).arg(record.height);
                }
                return "-";
            }
            case FileListColumn::Type: {
                if (record.isDir) return "文件夹";
                int lastDot = path.lastIndexOf('.');
                return (lastDot != -1) ? path.mid(lastDot + 1).toUpper() : "";
            }
            case FileListColumn::Size: {
                if (record.isDir) return "-";
                if (record.size < 1024) return QString::number(record.size) + " B";
                if (record.size < 1024 * 1024) return QString::number(record.size / 1024.0, 'f', 1) + " KB";
                return QString::number(record.size / (1024.0 * 1024.0), 'f', 1) + " MB";
            }
            case FileListColumn::CreatedDate: {
                return formatDateTime(record.ctime);
            }
            case FileListColumn::ModifiedDate: {
                return formatDateTime(record.mtime);
            }
            default: break;
        }
    } else if (role == PathRole) {
        return path;
    } else if (role == TypeRole) {
        return record.isDir ? "folder" : "file";
    } else if (role == RatingRole) {
        return record.rating;
    } else if (role == ColorRole) {
        return record.manualColor;
    } else if (role == IsLockedRole || role == PinnedRole) {
        return record.pinned;
    } else if (role == EncryptedRole) {
        return record.encrypted;
    } else if (role == TagsRole) {
        return record.tags;
    } else if (role == NoteRole) {
        return record.note;
    } else if (role == UrlRole) {
        return record.url;
    } else if (role == IsEmptyRole) {
        return record.isDir && record.isEmpty;
    } else if (role == AspectRatioRole) {
        if (record.width > 0 && record.height > 0) return (double)record.width / record.height;
        double ratio = m_aspectRatios.value(QDir::toNativeSeparators(path), 1.0);
        return ratio > 0.0 ? ratio : 1.0;
    } else if (role == IsDropTargetRole) {
        return record.isDropTarget;
    } else if (role == IsDiskTrashRole) {
        return record.isDiskTrash;
    } else if (role == DiskTrashIdRole) {
        return record.diskTrashId;
    } else if (role == HasThumbnailRole) {
        return record.thumbnailState == ItemRecord::ThumbnailState::Ready;
    } else if (role == Qt::DecorationRole && index.column() == static_cast<int>(FileListColumn::Name)) {
        QString cleanKey = QDir::cleanPath(path);
        QIcon* cached = m_iconCache.object(cleanKey);
        if (!cached) {
            cached = m_iconCache.object(path);
        }
        if (cached) return *cached;

        QString ext = record.suffix.toLower();
        bool isGraphic = ColorPaletteEngine::isGraphicsFile(ext);
        
        if (isGraphic) return QIcon();
        QIcon icon = ShellIconManager::getFileIconFast(path, record.isDir, ext);
        if (ShellIconManager::isIconCached(path, record.isDir, ext)) {
            m_iconCache.insert(cleanKey, new QIcon(icon));
        }
        return icon;
    }

    return QVariant();
}

QStringList DiskItemModel::applyIncrementalChanges(const std::vector<ItemRecord>& records, const QStringList& removedPaths) {
    // 1. 构建临时规范化键映射，避免 O(n·k)
    std::unordered_map<QString, int, QuarkMeta::QStringHash> normPathMap;
    normPathMap.reserve(m_allRecords.size());
    for (int i = 0; i < static_cast<int>(m_allRecords.size()); ++i) {
        normPathMap[QDir::cleanPath(m_allRecords[i].path).toLower()] = i;
    }

    auto findRowForPath = [this, &normPathMap](const QString& path) -> int {
        auto it = m_pathToIndex.find(path);
        if (it != m_pathToIndex.end()) {
            return it->second;
        }
        QString normKey = QDir::cleanPath(path).toLower();
        auto normIt = normPathMap.find(normKey);
        if (normIt != normPathMap.end()) {
            return normIt->second;
        }
        return -1;
    };

    // 2. 先处理删除
    std::vector<int> rowsToDelete;
    for (const QString& remPath : removedPaths) {
        int r = findRowForPath(remPath);
        if (r >= 0 && r < static_cast<int>(m_allRecords.size())) {
            rowsToDelete.push_back(r);
        }
    }

    if (!rowsToDelete.empty()) {
        std::sort(rowsToDelete.begin(), rowsToDelete.end());
        rowsToDelete.erase(std::unique(rowsToDelete.begin(), rowsToDelete.end()), rowsToDelete.end());

        // 记录哪些原始行被删除，用于重映射 m_pendingThumbRows
        QSet<int> deletedRowsSet(rowsToDelete.begin(), rowsToDelete.end());

        // 按连续区间分组（倒序处理）
        struct Interval { int start; int end; };
        std::vector<Interval> intervals;

        int curStart = rowsToDelete.back();
        int curEnd = rowsToDelete.back();

        for (int i = static_cast<int>(rowsToDelete.size()) - 2; i >= 0; --i) {
            int r = rowsToDelete[i];
            if (r == curStart - 1) {
                curStart = r;
            } else {
                intervals.push_back({curStart, curEnd});
                curStart = r;
                curEnd = r;
            }
        }
        intervals.push_back({curStart, curEnd});

        // 逐个区间执行删除并清理缓存
        for (const auto& iv : intervals) {
            for (int r = iv.start; r <= iv.end; ++r) {
                const QString& p = m_allRecords[r].path;
                m_iconCache.remove(p);
                m_iconCache.remove(QDir::cleanPath(p));
                m_aspectRatios.remove(QDir::toNativeSeparators(p));
                m_requestedPaths.remove(p);
            }

            beginRemoveRows(QModelIndex(), iv.start, iv.end);
            m_allRecords.erase(m_allRecords.begin() + iv.start, m_allRecords.begin() + iv.end + 1);
            endRemoveRows();
        }

        // 全量重建 m_pathToIndex 和 normPathMap
        m_pathToIndex.clear();
        normPathMap.clear();
        normPathMap.reserve(m_allRecords.size());
        for (int i = 0; i < static_cast<int>(m_allRecords.size()); ++i) {
            m_pathToIndex[m_allRecords[i].path] = i;
            normPathMap[QDir::cleanPath(m_allRecords[i].path).toLower()] = i;
        }

        // 重映射 m_pendingThumbRows
        QSet<int> newPendingRows;
        for (int oldRow : m_pendingThumbRows) {
            if (deletedRowsSet.contains(oldRow)) continue;

            int deletedBefore = 0;
            for (int delRow : rowsToDelete) {
                if (delRow < oldRow) {
                    deletedBefore++;
                } else {
                    break;
                }
            }
            int newRow = oldRow - deletedBefore;
            if (newRow >= 0 && newRow < static_cast<int>(m_allRecords.size())) {
                newPendingRows.insert(newRow);
            }
        }
        m_pendingThumbRows = newPendingRows;
    }

    // 3. 处理已存在记录更新
    std::vector<int> updatedRows;
    std::vector<ItemRecord> newRecords;

    for (const auto& inRec : records) {
        int r = findRowForPath(inRec.path);
        if (r >= 0 && r < static_cast<int>(m_allRecords.size())) {
            auto& rec = m_allRecords[r];
            rec.filename = inRec.filename;
            rec.suffix = inRec.suffix;
            rec.size = inRec.size;
            rec.ctime = inRec.ctime;
            rec.mtime = inRec.mtime;
            rec.isDir = inRec.isDir;
            rec.isEmpty = inRec.isEmpty;
            rec.encrypted = inRec.encrypted;
            rec.isDiskTrash = inRec.isDiskTrash;
            rec.diskTrashId = inRec.diskTrashId;
            updatedRows.push_back(r);
        } else {
            newRecords.push_back(inRec);
        }
    }

    if (!updatedRows.empty()) {
        std::sort(updatedRows.begin(), updatedRows.end());
        updatedRows.erase(std::unique(updatedRows.begin(), updatedRows.end()), updatedRows.end());

        int startRow = -1;
        int prevRow = -1;
        for (int r : updatedRows) {
            if (startRow == -1) {
                startRow = r;
                prevRow = r;
            } else if (r == prevRow + 1) {
                prevRow = r;
            } else {
                emit dataChanged(index(startRow, 0), index(prevRow, columnCount() - 1));
                startRow = r;
                prevRow = r;
            }
        }
        if (startRow != -1) {
            emit dataChanged(index(startRow, 0), index(prevRow, columnCount() - 1));
        }
    }

    // 4. 处理新记录插入
    QStringList addedPaths;
    if (!newRecords.empty()) {
        MetaCacheDecorator::decorate(newRecords);

        for (auto& rec : newRecords) {
            if (rec.isDir || !ColorPaletteEngine::isGraphicsFile(rec.suffix.toLower())) {
                rec.thumbnailState = ItemRecord::ThumbnailState::NotApplicable;
            } else if (rec.thumbStatus == 1) {
                rec.thumbnailState = ItemRecord::ThumbnailState::Failed;
            } else {
                rec.thumbnailState = ItemRecord::ThumbnailState::Pending;
            }
            addedPaths.append(rec.path);
        }

        MetadataManager::instance().seedMemoryCacheBatch(newRecords);

        int firstNewRow = static_cast<int>(m_allRecords.size());
        int lastNewRow = firstNewRow + static_cast<int>(newRecords.size()) - 1;

        beginInsertRows(QModelIndex(), firstNewRow, lastNewRow);
        for (size_t i = 0; i < newRecords.size(); ++i) {
            int rowIdx = firstNewRow + static_cast<int>(i);
            m_allRecords.push_back(newRecords[i]);
            m_pathToIndex[newRecords[i].path] = rowIdx;
        }
        m_iconCache.setMaxCost(qMax(m_iconCache.maxCost(), static_cast<int>(m_allRecords.size()) + 50));
        endInsertRows();

        preloadDimensionsAsync(addedPaths);
    }

    return addedPaths;
}

void DiskItemModel::reloadThumbnailForPath(const QString& path) {
    QString nPath = QDir::toNativeSeparators(path);
    m_iconCache.remove(nPath);
    m_iconCache.remove(path);
    m_aspectRatios.remove(nPath);
    m_requestedPaths.remove(nPath);
    m_requestedPaths.remove(path);

    auto it = m_pathToIndex.find(nPath);
    if (it != m_pathToIndex.end()) {
        int rIdx = it->second;
        // 显式重载（如重新生成缩略图成功后）：解除 Failed 终态，允许再次加载 
        if (rIdx >= 0 && rIdx < static_cast<int>(m_allRecords.size()) 
            && m_allRecords[rIdx].thumbnailState == ItemRecord::ThumbnailState::Failed) { 
            m_allRecords[rIdx].thumbnailState = ItemRecord::ThumbnailState::Pending; 
        } 
        loadThumbnailsForRows({rIdx});
        emit dataChanged(
            index(rIdx, 0), 
            index(rIdx, columnCount() - 1), 
            {Qt::DecorationRole, Qt::DisplayRole, AspectRatioRole, HasThumbnailRole}
        );
    }
}

} // namespace QuarkMeta