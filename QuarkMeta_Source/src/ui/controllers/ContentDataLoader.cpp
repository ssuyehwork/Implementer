#include "ContentDataLoader.h"
#include "../ContentPanel.h"
#include "../../core/DiskScanService.h"
#include "../../meta/DiskTrashRepo.h"
#include "../../meta/MetaCacheDecorator.h"
#include "../../meta/MediaExtractorPipeline.h"
#include "../../meta/MetadataManager.h"
#include "../../meta/LibraryItemDao.h"
#include "../../meta/LibrarySyncService.h"
#include "../../meta/LibraryValidationService.h"
#include "../../meta/LibraryDao.h"
#include "../../meta/LibraryService.h"
#include "../../meta/SmartFolderDao.h"
#include "../../util/ThumbnailPipelineService.h"

#include <QDir>
#include <QFileInfo>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QPointer>
#include <QDebug>
#include <QTimer>
#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>

namespace QuarkMeta {

ContentDataLoader::ContentDataLoader(ContentPanel* panel)
    : QObject(panel), m_panel(panel) {}

static std::vector<ItemRecord> loadTrashItemsDirect() {
    std::vector<ItemRecord> records;
    auto rawDiskTrash = DiskTrashRepo::getAllTrashItems();
    for (const auto& raw : rawDiskTrash) {
        ItemRecord rec;
        rec.isDiskTrash = true;
        rec.diskTrashId = raw.id;
        rec.fileId = QString::fromStdWString(raw.fileId);
        rec.path = QString::fromStdWString(raw.trashPath);
        rec.originalPath = QString::fromStdWString(raw.originalPath);
        rec.filename = QString::fromStdWString(raw.fileName);
        rec.suffix = QFileInfo(rec.filename).suffix();
        rec.isDir = raw.isFolder;
        rec.size = raw.fileSize;
        rec.ctime = raw.createdAt > 0 ? raw.createdAt : raw.deletedAt;
        rec.mtime = raw.deletedAt;
        rec.isHidden = false;
        records.push_back(rec);
    }
    MetaCacheDecorator::decorate(records);
    return records;
}

void ContentDataLoader::loadDirectory(const QString& path, bool recursive) {
    if (!m_panel) return;
    m_panel->restoreActiveView();
    if (m_panel->sortController()) {
        m_panel->sortController()->setCurrentFolderPath(path);
    }
    MediaExtractorPipeline::instance().cancelAll();
    m_panel->incrementModelGeneration();
    m_panel->ensureSourceModelIsDiskModel();
    ThumbnailPipelineService::instance().cancelAll();

    if (path == "trash://") {
        loadCategory("trash");
        return;
    }

    if (path.startsWith("library://")) {
        int catId = path.mid(10).toInt();
        QString categoryName;
        if (catId == -1) categoryName = "全部数据";
        else if (catId == -2) categoryName = "未分类";
        else {
            LibraryCategoryRecord rec = LibraryDao::getCategoryById(catId);
            categoryName = rec.name;
        }
        QStringList pList = LibraryService::instance().getCategoryPaths(catId);
        m_panel->applyViewTarget(path, "library", categoryName, catId, false);
        m_panel->loadLibraryPaths(pList);
        return;
    }

    if (path.startsWith("smartfolder://")) {
        int sfId = path.mid(14).toInt();
        SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(sfId);
        loadSmartFolder(rec, rec.name);
        return;
    }

    m_panel->setLoading(true);
    int reqId = m_panel->incrementLoadRequestId();
    m_loadRequestId = reqId;
    emit m_panel->dataSourceChanged("nav");

    if (path.isEmpty() || path == "computer://") {
        m_panel->applyViewTarget("computer://", "", "", 0, false);
        m_panel->updateLayersButtonState();
        std::vector<ItemRecord> driveRecords;
        for (const QFileInfo& drive : QDir::drives()) {
            driveRecords.push_back(ItemRecord::create(drive.absolutePath()));
        }
        MetaCacheDecorator::decorate(driveRecords);
        if (m_panel->model()) {
            m_panel->model()->setRecords(driveRecords);
        }
        m_panel->applySort();
        m_panel->setLoading(false);
        m_panel->recalculateAndEmitStats();
        return;
    }

    m_panel->applyViewTarget(path, "", "", 0, recursive);
    m_panel->updateLayersButtonState();

    QPointer<ContentPanel> panelPtr(m_panel);
    (void)QtConcurrent::run([panelPtr, path, recursive, reqId]() {
        if (!panelPtr) return;
        std::vector<ItemRecord> allItems = DiskScanService::scanDirectory(path, recursive, [panelPtr, reqId]() {
            return panelPtr && panelPtr->loadRequestId() == reqId;
        });
        if (!panelPtr) return;

        MetaCacheDecorator::decorate(allItems);

        QMetaObject::invokeMethod(QCoreApplication::instance(), [panelPtr, allItems, reqId]() {
            if (panelPtr && panelPtr->loadRequestId() == reqId) {
                if (panelPtr->model()) {
                    panelPtr->model()->setRecords(allItems);
                }
                panelPtr->applySort();
                panelPtr->setLoading(false);
                panelPtr->recalculateAndEmitStats();
                panelPtr->applyFilters();
                panelPtr->restoreSelections();
                panelPtr->startVisibleTimer();

                // Enqueue scanned files for background media feature & auto-color extraction
                QStringList filePaths;
                for (const auto& item : allItems) {
                    if (!item.isDir && !item.path.isEmpty()) {
                        filePaths.append(item.path);
                    }
                }
                if (!filePaths.isEmpty()) {
                    MetadataManager::instance().registerItemsAsync(filePaths);
                }
            }
        }, Qt::QueuedConnection);
    });
}

void ContentDataLoader::loadCategory(const QString& categoryType) {
    if (!m_panel) return;
    if (categoryType == "trash") {
        m_panel->applyViewTarget("trash://", "trash", "", 0, false);
        loadPaths({});
    } else {
        m_panel->applyViewTarget(categoryType + "://", categoryType, "", 0, false);
        loadPaths({});
    }
}

void ContentDataLoader::loadPaths(const QStringList& paths, int reqId) {
    if (!m_panel) return;

    // 死穴 1 解法：如果当前处于分栏视图，自动自愈切换为网格视图以保证路径列表正常呈现
    if (m_panel->currentViewMode() == ContentPanel::ColumnView) {
        m_panel->setViewMode(ContentPanel::GridView);
    }

    m_panel->restoreActiveView();
    m_panel->ensureSourceModelIsDiskModel();

    if (paths.isEmpty() && m_panel->getCurrentCategoryType() != "trash") {
        if (m_panel->model()) {
            m_panel->model()->clear();
        }
        m_panel->setLoading(false);
        m_panel->recalculateAndEmitStats();
        return;
    }

    m_panel->setLoading(true);
    if (reqId == 0) {
        reqId = m_panel->incrementLoadRequestId();
        m_loadRequestId = reqId;
    }
    if (m_panel->getCurrentCategoryType().isEmpty()) {
        m_panel->setCurrentCategoryType("path_list");
    } else if (m_panel->getCurrentCategoryType() == "library") {
        m_panel->setLastLoadedLibraryPaths(paths);
    }
    m_panel->updateLayersButtonState();

    QPointer<ContentPanel> weakPanel(m_panel);
    (void)QtConcurrent::run([weakPanel, paths, reqId]() {
        if (!weakPanel) return;
        std::vector<ItemRecord> records;

        if (weakPanel->getCurrentCategoryType() == "trash") {
            records = loadTrashItemsDirect();
        } else {
            for (const QString& p : paths) {
                records.push_back(ItemRecord::create(p));
            }
            MetaCacheDecorator::decorate(records);
        }
        if (!weakPanel) return;

        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakPanel, records, reqId]() {
            if (weakPanel && weakPanel->loadRequestId() == reqId) {
                if (weakPanel->model()) {
                    weakPanel->model()->setRecords(records);
                }
                weakPanel->applySort();
                weakPanel->setLoading(false);
                weakPanel->recalculateAndEmitStats();
                weakPanel->applyFilters();
                weakPanel->restoreSelections();
            }
        });
    });
}

void ContentDataLoader::loadSmartFolder(const SmartFolderRecord& record, const QString& displayName) {
    if (!m_panel) return;

    QString targetName = displayName.isEmpty() ? record.name : displayName;
    m_panel->applyViewTarget(QString("smartfolder://%1").arg(record.id), "smart_folder", targetName, -20, false);
    m_panel->setActiveSmartFolder(record);

    uint64_t sfReqId = m_panel->incrementSmartFolderRequestId();
    int reqId = m_panel->incrementLoadRequestId();
    m_loadRequestId = reqId;

    QPointer<ContentPanel> weakPanel(m_panel);
    (void)QtConcurrent::run([weakPanel, record, sfReqId, reqId]() {
        QStringList matchingPaths = SmartFolderDao::getMatchingPaths(record);

        QMetaObject::invokeMethod(qApp, [weakPanel, record, sfReqId, reqId, matchingPaths]() {
            if (!weakPanel) return;
            if (weakPanel->loadRequestId() != reqId || weakPanel->smartFolderRequestId() != sfReqId) return;
            if (weakPanel->getCurrentCategoryType() != "smart_folder" || weakPanel->activeSmartFolder().id != record.id) return;

            weakPanel->setLastLoadedLibraryPaths(matchingPaths);
            weakPanel->loadLibraryPaths(matchingPaths, reqId);
        });
    });
}

void ContentDataLoader::loadLibraryPaths(const QStringList& paths, int reqId) {
    if (!m_panel) return;

    if (m_panel->currentViewMode() == ContentPanel::ColumnView) {
        m_panel->setViewMode(ContentPanel::GridView);
    }

    m_panel->restoreActiveView();
    m_panel->ensureSourceModelIsDiskModel();

    if (paths.isEmpty()) {
        if (m_panel->model()) {
            m_panel->model()->clear();
        }
        m_panel->setLoading(false);
        m_panel->recalculateAndEmitStats();
        return;
    }

    m_panel->setLoading(true);
    if (reqId == 0) {
        reqId = m_panel->incrementLoadRequestId();
        m_loadRequestId = reqId;
    }
    m_panel->setLastLoadedLibraryPaths(paths);
    m_panel->updateLayersButtonState();

    QPointer<ContentPanel> weakPanel(m_panel);
    (void)QtConcurrent::run([weakPanel, paths, reqId]() {
        if (!weakPanel) return;

        // 纯数据库装配 ItemRecord，彻底不读任何 .QuarkMeta.json
        std::vector<ItemRecord> dbItems = LibraryItemDao::getItemsByPaths(paths);

        // 收集数据库中已被同步的路径
        std::unordered_set<std::wstring> fetchedPaths;
        std::vector<ItemRecord> validRecords;
        validRecords.reserve(dbItems.size());

        for (auto& item : dbItems) {
            fetchedPaths.insert(QDir::toNativeSeparators(item.path).toLower().toStdWString());
            validRecords.push_back(std::move(item));
        }

        // 检查是否有缺失项，用 ItemRecord::create 仅取物理属性，并通知 LibrarySyncService 异步补齐
        QStringList missingPaths;
        for (const QString& p : paths) {
            std::wstring normP = QDir::toNativeSeparators(p).toLower().toStdWString();
            if (fetchedPaths.count(normP) == 0) {
                ItemRecord tempRec = ItemRecord::create(p);
                validRecords.push_back(tempRec);
                missingPaths.append(p);
            }
        }

        if (!missingPaths.isEmpty()) {
            LibrarySyncService::instance().syncPaths(missingPaths);
        }

        if (!weakPanel) return;

        // 流式分批调度常数定义
        constexpr size_t kFirstChunkSize = 100;
        constexpr size_t kSubsequentChunkSize = 500;

        const size_t totalRecords = validRecords.size();
        const size_t initialBatchSize = std::min(totalRecords, kFirstChunkSize);

        std::vector<ItemRecord> firstChunk(
            std::make_move_iterator(validRecords.begin()),
            std::make_move_iterator(validRecords.begin() + initialBatchSize)
        );

        auto remainingRecords = std::make_shared<std::vector<ItemRecord>>(
            std::make_move_iterator(validRecords.begin() + initialBatchSize),
            std::make_move_iterator(validRecords.end())
        );

        // 统一在主线程执行首批装配与流式切片调度
        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakPanel, firstChunk = std::move(firstChunk), remainingRecords, reqId]() mutable {
            if (!weakPanel || weakPanel->loadRequestId() != reqId) return;

            if (weakPanel->model()) {
                weakPanel->model()->setRecords(firstChunk, RecordSource::Prepared);
            }
            weakPanel->applySort();
            weakPanel->setLoading(false);
            weakPanel->recalculateAndEmitStats();
            weakPanel->applyFilters();

            if (remainingRecords->empty()) {
                weakPanel->restoreSelections();
                if (weakPanel->model()) {
                    const auto& all = weakPanel->model()->allRecords();
                    MetadataManager::instance().registerLibraryItemsAsync(all);
                    LibraryValidationService::instance().validateRecordsAsync(all, weakPanel.data(), reqId);
                }
                return;
            }

            // 定义流式递归注入调度函数 (Sequential Async Waterfall using QTimer::singleShot)
            struct StreamContext {
                size_t offset = 0;
            };
            auto ctx = std::make_shared<StreamContext>();

            auto scheduleNextChunk = std::make_shared<std::function<void()>>();
            *scheduleNextChunk = [weakPanel, remainingRecords, reqId, ctx, scheduleNextChunk]() {
                if (!weakPanel || weakPanel->loadRequestId() != reqId) {
                    // 若面板已销毁或已发起新请求，主动打断自持有强引用，释放内存
                    *scheduleNextChunk = nullptr;
                    return;
                }

                if (ctx->offset < remainingRecords->size()) {
                    size_t chunkSize = std::min(kSubsequentChunkSize, remainingRecords->size() - ctx->offset);
                    std::vector<ItemRecord> chunk(
                        remainingRecords->begin() + ctx->offset,
                        remainingRecords->begin() + ctx->offset + chunkSize
                    );
                    ctx->offset += chunkSize;

                    if (weakPanel->model()) {
                        weakPanel->model()->appendRecords(chunk);
                    }

                    if (ctx->offset < remainingRecords->size()) {
                        // 让出主线程事件循环，处理 Win32 / Qt 消息队列后注入下一批；通过捕获 scheduleNextChunk 维持接力生命周期
                        QTimer::singleShot(0, weakPanel, [scheduleNextChunk]() {
                            if (*scheduleNextChunk) {
                                (*scheduleNextChunk)();
                            }
                        });
                        return;
                    }
                }

                // 所有批次全部注入完成：执行统一收尾结算
                weakPanel->applySort();
                weakPanel->recalculateAndEmitStats();
                weakPanel->applyFilters();
                weakPanel->restoreSelections();

                if (weakPanel->model()) {
                    const auto& all = weakPanel->model()->allRecords();
                    MetadataManager::instance().registerLibraryItemsAsync(all);
                    LibraryValidationService::instance().validateRecordsAsync(all, weakPanel.data(), reqId);
                }

                // 主动解开自身强引用闭包，确保 std::function 与捕获的 remainingRecords 顺利析构
                *scheduleNextChunk = nullptr;
            };

            // 派发第一批后续追加任务
            QTimer::singleShot(0, weakPanel, [scheduleNextChunk]() {
                if (*scheduleNextChunk) {
                    (*scheduleNextChunk)();
                }
            });
        });
    });
}

void ContentDataLoader::appendPaths(const QStringList& paths, int reqId) {
    if (!m_panel || paths.isEmpty() || (reqId != 0 && m_panel->loadRequestId() != reqId)) return;
    QPointer<ContentPanel> weakPanel(m_panel);
    (void)QtConcurrent::run([weakPanel, paths, reqId]() {
        if (!weakPanel) return;
        std::vector<ItemRecord> newRecs;
        newRecs.reserve(static_cast<size_t>(paths.size()));
        for (const QString& p : paths) {
            newRecs.push_back(ItemRecord::create(p));
        }
        MetaCacheDecorator::decorate(newRecs);
        if (!weakPanel) return;

        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakPanel, newRecs = std::move(newRecs), reqId]() {
            if (weakPanel && (reqId == 0 || weakPanel->loadRequestId() == reqId)) {
                if (weakPanel->model()) {
                    weakPanel->model()->appendRecords(newRecs);
                }
                weakPanel->recalculateAndEmitStats();
                weakPanel->applyFilters();
            }
        });
    });
}

} // namespace QuarkMeta
