#ifndef QuarkMeta_METADATA_MANAGER_H
#define QuarkMeta_METADATA_MANAGER_H

#include "MetadataDefs.h"
#include "MetaMemoryCache.h"
#include "../core/ItemRecord.h"
#include <QObject>
#include <QString>
#include <QTimer>
#include <QStringList>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <string>
#include <atomic>
#include <deque>
#include <mutex>
#include <memory>
#include <array>

namespace QuarkMeta {

struct LightMeta {
    std::wstring path;
    bool isFolder;
    bool tagsEmpty;
    double atime;
    QStringList tags;
};

/**
 * @brief 元数据管理器（门面模式 Facade Router）
 */
class MetadataManager : public QObject {
    Q_OBJECT
public:
    static MetadataManager& instance();

    bool isLoaded() const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_loaded;
    }

    static std::wstring normalizePath(const std::wstring& path);

    void initFromDatabase();
    RuntimeMeta getMeta(const std::wstring& path);

    /**
     * @brief 在当前引擎下执行多维搜索
     */
    QStringList searchInCache(const QString& keyword, const QString& scopeSource = "", int categoryId = 0, const QString& parentPath = "");

    /**
     * @brief 获取所有标签及其引用计数
     */
    QMap<QString, int> getAllTags() const;

    /**
     * @brief 获取权重最高的前 N 个标签
     */
    QList<QPair<QString, int>> getTopTags(int limit = 20) const;

    /**
     * @brief 物理刷新级别
     */
    enum class RefreshLevel {
        CountsOnly,   // 仅刷新侧边栏计数
        PathUpdate,   // 刷新特定文件
        FullRebuild   // 全量 UI 重建
    };

    void notifyUI(RefreshLevel level, const QString& path = "");
    void notifyFullUIRebuild();

    void registerItem(const std::wstring& path);
    bool shouldExtractMediaFeatures(const ItemRecord& record);
    void registerItemsAsync(const QStringList& paths);
    void registerLibraryItemsAsync(const std::vector<ItemRecord>& records);

    void ensureActivated(const std::wstring& nPath);

    void setRating(const std::wstring& path, int rating, bool notify = true);
    void setSha256(const std::wstring& path, const std::string& sha256, bool notify = false);
    void setAddedAt(const std::wstring& path, long long addedAt, bool notify = true);
    void setColor(const std::wstring& path, const std::wstring& color, bool notify = true);
    void setPinned(const std::wstring& path, bool pinned, bool notify = true);
    void setTags(const std::wstring& path, const QStringList& tags, bool notify = true);
    QMap<QString, QStringList> addTagsBatch(const QStringList& paths, const QStringList& tags);
    QMap<QString, QStringList> removeTagsBatch(const QStringList& paths, const QStringList& tags);
    void setRatingBatch(const QStringList& paths, int rating);
    void setColorBatch(const QStringList& paths, const QString& color);
    void setNoteBatch(const QStringList& paths, const QString& note);
    void setURLBatch(const QStringList& paths, const QString& url);
    void setPinnedBatch(const QStringList& paths, bool pinned);
    void setNote(const std::wstring& path, const std::wstring& note, bool notify = true);
    void setURL(const std::wstring& path, const std::wstring& url, bool notify = true);
    void setEncrypted(const std::wstring& path, bool encrypted, bool notify = true);

    void setPalettes(const std::wstring& path, const QVector<QPair<QColor, float>>& palettes, bool notify = true);

    struct ExtractedFeatureItem {
        std::wstring path;
        int width{0};
        int height{0};
        int64_t mtime{0};
        int64_t fileSize{0};
        std::wstring autoColor;
        QVector<QPair<QColor, float>> palettes;
    };

    void updateExtractedMediaFeatures(
        const std::wstring& path,
        int width,
        int height,
        const std::wstring& autoColor,
        const QVector<QPair<QColor, float>>& palettes
    );

    void updateExtractedMediaFeaturesBatch(const std::vector<ExtractedFeatureItem>& items);
    void seedMemoryCacheBatch(const std::vector<ItemRecord>& records);
    bool shouldExtractMediaFeatures(const std::wstring& path, long long pSize, long long pMtime);

    void renameTag(const QString& oldName, const QString& newName);
    void removeTag(const QString& tagName);

    void setItemVisualMetadata(const std::wstring& path, const std::wstring& color, const QVector<QPair<QColor, float>>& palettes, bool notify = true);
    void setItemDimensions(const std::wstring& path, int width, int height);

    QVector<QColor> getPalettes(const std::wstring& path);

    void renameItem(const std::wstring& oldPath, const std::wstring& newPath);
    void renameBatchAsync(
        const std::vector<std::pair<std::wstring, std::wstring>>& rawPathPairs,
        std::function<void(int successCount)> onCompleted = nullptr
    );

    void removeMetadataSync(const std::wstring& path);
    void syncAfterMove(const std::wstring& oldPath, const std::wstring& newPath);
    static std::wstring getVolumeSerialNumber(const std::wstring& path);

    void beginInternalOperation() {
        m_internalOpsCount.fetch_add(1);
        m_isInternalOperating.store(true);
    }

    void endInternalOperation() {
        int count = m_internalOpsCount.fetch_sub(1) - 1;
        if (count <= 0) {
            m_internalOpsCount.store(0);
            m_isInternalOperating.store(false);
        }
    }

    void setInternalOperating(bool operating) {
        if (operating) {
            beginInternalOperation();
        } else {
            endInternalOperation();
        }
    }
    bool isInternalOperating() const { return m_isInternalOperating.load(); }

    void parsePathComponents(const std::wstring& normalizedPath, bool isFolder, std::wstring& outName, std::wstring& outExt);

    void recordAccess(const std::wstring& path);
    void slideRecentWindow();
    double getCachedAtime(const std::wstring& path);

    template<typename Func>
    void forEachCachedItem(Func&& fn) const {
        MetaMemoryCache::instance().forEachItem(std::forward<Func>(fn));
    }

    std::vector<LightMeta> getLightweightCacheSnapshot() const;
    std::shared_mutex& getMutex() const { return m_mutex; }

    FolderMeta getFolderMeta(const QString& folderPath);
    void setFolderMeta(const QString& folderPath, const FolderMeta& meta);

    static bool fetchWinApiMetadataDirect(const std::wstring& path, long long* outSize = nullptr, std::wstring* outType = nullptr, long long* outCtime = nullptr, long long* outMtime = nullptr, long long* outAtime = nullptr);
    void persistAsync(const std::wstring& path, bool notify = true);

signals:
    void pendingSyncChanged(bool hasPending);

private:
    MetadataManager(QObject* parent = nullptr);
    ~MetadataManager() override = default;

    std::deque<std::wstring> m_recentVisitedQueue;
    std::unordered_set<std::wstring> m_recentVisitedSet;
    std::mutex m_recentMutex;

    mutable std::shared_mutex m_mutex;
    bool m_loaded = false;
    std::atomic<bool> m_isInternalOperating{false};
    std::atomic<int> m_internalOpsCount{0};

    void persistBatchAsync(const std::vector<std::wstring>& paths);
};

} // namespace QuarkMeta

#endif // QuarkMeta_METADATA_MANAGER_H
