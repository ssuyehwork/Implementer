#include "CoreController.h"
#include "AppConfig.h"
#include "DeviceWatcher.h"
#include "../meta/MetadataManager.h"
#include "../meta/DatabaseManager.h"
#include "../meta/MediaExtractorPipeline.h"
#include "../meta/LibrarySyncService.h"
#include "../util/DiskMediaExtractor.h"
#include "../ui/Logger.h"
#include <QThreadPool>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QtConcurrent>
#include <unordered_set>
#include "PhysicalDiskSearchExtractor.h"

namespace QuarkMeta {

std::atomic<bool> CoreController::s_isShuttingDown{false};
std::atomic<uint64_t> CoreController::s_navigationGeneration{1};

CoreController& CoreController::instance() {
    static CoreController inst;
    return inst;
}

void CoreController::initializeCoreComponents() {
    // 0. 全局设备监听器启动
    DeviceWatcher::instance().startListening();

    // 1. 底层持久化 SQLite 连接启动与定时机制保障
    QuarkMeta::DatabaseManager::instance();

    // 2. 元数据内存索引结构预热
    QuarkMeta::MetadataManager::instance();

    // 3. 后台提取特征管道、定时器及事件队列预热
    QuarkMeta::MediaExtractorPipeline::instance();

    // 4. 库同步对账服务预热（触发启动 1.5 秒后自动对账补齐）
    QuarkMeta::LibrarySyncService::instance();

    // 4. 定时合并刷新缩略图提取失败标记落盘
    QTimer* failureFlushTimer = new QTimer(QCoreApplication::instance());
    failureFlushTimer->setInterval(1000);
    QObject::connect(failureFlushTimer, &QTimer::timeout, []() {
        (void)QtConcurrent::run(DiskMediaExtractor::flushPendingUpdates);
    });
    failureFlushTimer->start();
}

void CoreController::requestShutdown() { s_isShuttingDown.store(true); }
bool CoreController::isShuttingDown() { return s_isShuttingDown.load(); }
uint64_t CoreController::incrementNavigationGeneration() { return ++s_navigationGeneration; }
uint64_t CoreController::currentNavigationGeneration() { return s_navigationGeneration.load(); }

CoreController::CoreController(QObject* parent) : QObject(parent) {
}

CoreController::~CoreController() {}

/**
 * @brief 启动系统初始化链条
 * 运行核心控制逻辑
 */
void CoreController::startSystem() {
    QThreadPool::globalInstance()->start([this]() {
        try {

            QMetaObject::invokeMethod(this, [this]() {
                setStatus("正在载入元数据缓存...", true);
            }, Qt::QueuedConnection);

            // 纯磁盘直连模式注销全盘 metadata 数据表预训练扫描


            // 在系统顶层统一提取一次“上次是否正常关闭”状态，提取后立刻置脏
            bool wasCleanShutdown = AppConfig::instance().getValue("System/LastCleanShutdown", false).toBool();
            Q_UNUSED(wasCleanShutdown);
            AppConfig::instance().setValue("System/LastCleanShutdown", false);
            AppConfig::instance().sync();


            QMetaObject::invokeMethod(this, [this]() {
                setStatus("系统就绪", false);
                emit initializationFinished();
            }, Qt::QueuedConnection);

        } catch (...) {
            QMetaObject::invokeMethod(this, [this]() {
                setStatus("初始化失败", false);
                emit initializationFinished();
            }, Qt::QueuedConnection);
        }
    });
}

void CoreController::performSearch(const QString& keyword, const QString& scopeSource, int categoryId, const QString& parentPath) {
    // 1. 物理中止旧任务：无论新词是否为空，只要发起 performSearch 就必须清理前序任务
    abortSearch();

    if (keyword.isEmpty()) {
        return;
    }

    m_isSearchAborted = false;
    m_isSearching = true;
    int searchId = ++m_currentSearchId;

    emit searchStarted();

    // 2. 异步启动双轨搜索任务
    (void)QtConcurrent::run([this, keyword, scopeSource, categoryId, parentPath, searchId]() {
        QStringList cacheResults;
        std::unordered_set<std::wstring> seenPaths;
        int totalFound = 0;

        // --- 第一阶段：内存缓存检索 (极速响应) ---
        cacheResults = MetadataManager::instance().searchInCache(keyword, scopeSource, categoryId, parentPath);
        for (const auto& p : cacheResults) {
            seenPaths.insert(p.toStdWString());
        }
        totalFound = static_cast<int>(cacheResults.size());

        // 发射第一批缓存结果
        if (!m_isSearchAborted && m_currentSearchId == searchId) {
            emit searchResultsAvailable(cacheResults, false);
        }

        // --- 第二阶段：如果是物理导航模式，执行 I/O 扫描补全 (Plan-57) ---
        if (scopeSource == "nav" && !parentPath.isEmpty() && !m_isSearchAborted && m_currentSearchId == searchId) {
            int foundInDisk = PhysicalDiskSearchExtractor::performDiskSearch(
                parentPath, keyword, m_isSearchAborted, m_currentSearchId, searchId, seenPaths,
                [this](const QStringList& batch) {
                    emit searchResultsAvailable(batch, true);
                }
            );
            totalFound += foundInDisk;
        }

        m_isSearching = false;
        if (!m_isSearchAborted && m_currentSearchId == searchId) {
            emit searchFinished(totalFound);
        }
    });
}

void CoreController::abortSearch() {
    m_isSearchAborted = true;
    // 等待现有搜索任务退出的轻量化处理（实际生产环境可能需要更复杂的等待机制）
}


void CoreController::setStatus(const QString& text, bool indexing) {
    if (m_statusText != text) {
        m_statusText = text;
        emit statusTextChanged(m_statusText);
    }
    if (m_isIndexing != indexing) {
        m_isIndexing = indexing;
        emit isIndexingChanged(m_isIndexing);
    }
}

} // namespace QuarkMeta
