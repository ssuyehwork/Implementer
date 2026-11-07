#pragma once

#include <QString>
#include <QStringList>
#include <QObject>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <memory>
#include "sqlite3.h"

namespace QuarkMeta {

/**
 * @brief 专职后台增量派生索引服务 (index.db)
 * 1. 唯一职责：单向消费 JSON 落盘事件，派生全局跨文件夹搜索索引。
 * 2. 独立连接、WAL 模式、绝对不依赖 inspiration.db 和任何全局互斥锁。
 * 3. 索引损坏或版本不一致时，自动无损删库重建。
 */
class MetaIndexService : public QObject {
    Q_OBJECT

public:
    static MetaIndexService& instance();

    /**
     * @brief 初始化索引引擎与后台线程 (建议在主程序启动时调用)
     */
    void init();

    /**
     * @brief 安全退出后台线程并关闭数据库连接
     */
    void shutdown();

    /**
     * @brief 单向同步钩子：投递已落盘成功的文件夹路径（0ms 内存入队，绝不阻塞落盘或 UI）
     */
    void enqueueFolder(const std::wstring& folderPath);

    /**
     * @brief 异步分类索引钩子：将分类及其包含的路径投递至后台队列，由后台线程处理索引（0ms UI 阻断）
     */
    void enqueueCategoryIndexing(int categoryId, const QStringList& paths);

    /**
     * @brief 跨文件夹全局搜索接口（走独立只读连接，WAL 模式下并发无锁）
     * @param keyword 搜索关键词 (匹配文件名、备注、标签)
     * @param scopeFolderPath 限定的父文件夹（为空表示全局跨盘跨文件夹搜索）
     * @param limit 最大返回条数
     */
    QStringList searchGlobal(const QString& keyword, const QString& scopeFolderPath = "", int limit = 1000);

private:
    MetaIndexService(QObject* parent = nullptr);
    ~MetaIndexService() override;
    MetaIndexService(const MetaIndexService&) = delete;
    MetaIndexService& operator=(const MetaIndexService&) = delete;

    void workerThreadLoop();
    bool openDatabase(const std::wstring& dbPath, sqlite3** outDb, bool readOnly);
    bool initSchema(sqlite3* db);
    void processFolder(sqlite3* db, const std::wstring& normFolder, const std::wstring& rawFolder);
    void checkFreshnessAndIndex(sqlite3* db, const std::wstring& folderPath);

    std::wstring m_dbPath;
    sqlite3* m_writeDb = nullptr;
    sqlite3* m_readDb = nullptr;
    std::mutex m_readMutex; // 仅保护只读连接自身的并发查询安全

    // 任务队列与防抖合并
    std::mutex m_queueMutex;
    std::condition_variable m_cv;
    struct CategoryTask {
        int categoryId;
        QStringList paths;
    };

    std::unordered_map<std::wstring, std::wstring> m_pendingFolders; // normFolder -> rawFolder
    std::vector<CategoryTask> m_pendingCategoryTasks;

    std::thread m_workerThread;
    std::atomic<bool> m_stopping{false};
    std::atomic<bool> m_initialized{false};

    static constexpr int kSchemaVersion = 1;
};

} // namespace QuarkMeta
