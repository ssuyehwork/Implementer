#pragma once

#include "QuarkMetaJson.h"
#include <QObject>
#include <QString>
#include <QTimer>
#include <QThreadPool>
#include <QList>
#include <QPair>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <cstdint>

namespace QuarkMeta {

class QuarkMetaJsonStore : public QObject {
    Q_OBJECT

public:
    enum class ChangeType {
        ItemChanged,      // 文件属性/元数据变化 (path)
        ItemRenamed,      // 文件重命名/移动 (path=oldPath, newPath=newPath)
        ItemRemoved,      // 文件删除 (path)
        FolderMigrated    // 文件夹迁移/重命名 (path=oldFolder, newPath=newFolder)
    };

    struct ChangeEvent {
        ChangeType type;
        QString path;
        QString newPath;
    };

    using ChangeListener = std::function<void(const ChangeEvent&)>;

    static QuarkMetaJsonStore& instance();

    void addChangeListener(ChangeListener listener);

    /**
     * @brief 原子修改指定路径对应文件的 ItemMeta 记录 (自动缓冲合并落盘)
     */
    void updateItemMeta(const std::wstring& filePath, std::function<void(ItemMeta&)> updater);

    /**
     * @brief 批量原子修改同一个物理目录下的多个条目 ItemMeta (极大降低锁竞争)
     */
    void updateFolderItemsBatch(const std::wstring& folderPath,
                                const std::vector<std::pair<std::wstring, std::function<void(ItemMeta&)>>>& updaters);

    /**
     * @brief 批量向多个路径追加标签（按文件夹分组、合并现有 JSON 标签、预取加载、单批次标脏与刷盘）
     * @return 每个路径对应的最终合并标签列表
     */
    QMap<QString, QStringList> addTagsBatch(const QStringList& paths, const QStringList& tags);

    /**
     * @brief 批量从多个路径移除标签
     * @return 每个路径对应的最终合并标签列表
     */
    QMap<QString, QStringList> removeTagsBatch(const QStringList& paths, const QStringList& tags);
    void setRatingBatch(const QStringList& paths, int rating);
    void setColorBatch(const QStringList& paths, const QString& color);
    void setNoteBatch(const QStringList& paths, const QString& note);
    void setURLBatch(const QStringList& paths, const QString& url);
    void setPinnedBatch(const QStringList& paths, bool pinned);

    /**
     * @brief 获取指定物理目录的 .QuarkMeta.json 侧车文件自身的 meta_file_id
     */
    QString getMetaFileId(const std::wstring& folderPath);

    /**
     * @brief 获取与设置物理文件夹的 FolderMeta（包含 sortBy, sortOrder 等）
     */
    FolderMeta getFolderMeta(const std::wstring& folderPath);
    void setFolderMeta(const std::wstring& folderPath, const FolderMeta& meta);

    /**
     * @brief 一致性读取指定物理文件夹的所有条目 ItemMeta (大小写不敏感匹配)
     */
    QuarkMetaJson::ItemMap readFolderMeta(const std::wstring& folderPath);

    /**
     * @brief 一致性读取指定文件的 ItemMeta，成功找到返回 true
     */
    bool readItemMeta(const std::wstring& filePath, ItemMeta& outMeta);

    /**
     * @brief 物理迁移文件夹缓存文件 (.QuarkMeta.json) 并同步重键内存缓存
     */
    bool migrateFolderCache(const QString& oldFolderPath, const QString& newFolderPath);

    /**
     * @brief 单文件重命名时同步修改条目键名
     */
    bool renameItem(const QString& folderPath, const QString& oldName, const QString& newName);

    bool transferItems(const QList<QPair<QString, QString>>& pairs, bool isMove, bool replaceDestWhenSourceMissing);
    bool removeItem(const QString& filePath);

    /**
     * @brief 强制立即将所有未落盘的脏目录 JSON 刷入物理磁盘 (sync 为 true 时在当前线程同步写入)
     */
    void flushAllDirtyBuffers(bool sync = false);
    void flushAllDirtyBuffersAsync(std::function<void(bool allOk)> onDone);

    /**
     * @brief 驱逐指定文件夹的内存缓存快照，使下次 readFolderMeta() 强制从磁盘重新加载
     * @param folderPath 目标物理文件夹路径（原始大小写，内部自动规范化）
     */
    void invalidateFolderCache(const std::wstring& folderPath);

private slots:
    void onFlushTimeout();

private:
    explicit QuarkMetaJsonStore(QObject* parent = nullptr);
    ~QuarkMetaJsonStore() override;
    QuarkMetaJsonStore(const QuarkMetaJsonStore&) = delete;
    QuarkMetaJsonStore& operator=(const QuarkMetaJsonStore&) = delete;

    static std::wstring normalizeFolderPath(const std::wstring& path);

    void prefetchFolderJson(const std::wstring& folderPath);
    std::shared_ptr<QuarkMetaJson> getOrLoadFolderLocked(const std::wstring& normFolder, const std::wstring& folderPath);

    struct FlushTicket {
        std::wstring normFolder;
        uint64_t revision;
        QuarkMetaJson snapshot;
        FlushTicket(const std::wstring& n, uint64_t r, const QuarkMetaJson& s) : normFolder(n), revision(r), snapshot(s) {}
    };
    void markDirtyLocked(const std::wstring& normFolder);   // 调用方必须已持有 m_storeMutex
    bool saveTicket(const FlushTicket& ticket);              // 内部加 m_saveMutex；锁顺序固定为 m_storeMutex → m_saveMutex，持有 m_saveMutex 时不得获取 m_storeMutex
    void scheduleFlush();

    std::mutex m_storeMutex;
    // 规范化目录路径 -> 内存中缓存的 JSON 对象指针
    std::unordered_map<std::wstring, std::shared_ptr<QuarkMetaJson>> m_folderCacheMap;
    // 标记需要写盘落盘的规范化目录路径集合
    std::unordered_set<std::wstring> m_dirtyFolderPaths;
    std::unordered_map<std::wstring, uint64_t> m_folderRevision;  // 受 m_storeMutex 保护
    std::unordered_map<std::wstring, uint64_t> m_savedRevision;   // 受 m_saveMutex 保护
    std::mutex m_saveMutex;
    QThreadPool m_flushPool;

    std::mutex m_listenerMutex;
    std::vector<ChangeListener> m_listeners;
    void notifyListeners(const ChangeEvent& event);

    QTimer* m_flushTimer = nullptr;
    QTimer* m_maxFlushTimer = nullptr;
    static constexpr int kFlushDebounceMs = 300;  // 300ms 滑动防抖
    static constexpr int kMaxFlushDelayMs = 3000; // 3000ms 最长强制刷盘延迟
};

} // namespace QuarkMeta
