#pragma once

#include "MetadataDefs.h"
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <array>
#include <string>
#include <vector>
#include <QStringList>

namespace QuarkMeta {

struct RuntimeMeta {
    int rating;
    std::wstring manualColor;
    std::wstring autoColor;
    QStringList tags;
    std::wstring note;
    std::wstring url;
    bool pinned;
    bool encrypted;
    bool isFolder;
    int width;
    int height;
    int thumbStatus;
    std::wstring originalPath;
    std::wstring baseName;
    std::wstring ext;
    std::string sha256;

    long long ctime;
    long long mtime;
    long long atime;
    long long fileSize;
    long long added_at;

    std::vector<PaletteEntry> palettes;

    RuntimeMeta() : rating(0), pinned(false), encrypted(false), isFolder(false), width(0), height(0), thumbStatus(0), ctime(0), mtime(0), atime(0), fileSize(0), added_at(0) {}

    bool hasUserOperations() const {
        return rating > 0 || !manualColor.empty() || !autoColor.empty() || !tags.isEmpty() || !note.empty() || !url.empty() || pinned || encrypted;
    }
};

class MetaMemoryCache {
public:
    static constexpr size_t NUM_SHARDS = 256;

    struct MetaShard {
        mutable std::shared_mutex mutex;
        std::unordered_map<std::wstring, RuntimeMeta> items;
    };

    static MetaMemoryCache& instance();

    size_t getShardIndex(const std::wstring& path) const;
    RuntimeMeta getMeta(const std::wstring& normalizedPath);
    bool contains(const std::wstring& normalizedPath);
    void put(const std::wstring& normalizedPath, const RuntimeMeta& meta);
    void update(const std::wstring& normalizedPath, std::function<void(RuntimeMeta&)> modifier);
    void remove(const std::wstring& normalizedPath);

    template<typename Func>
    void forEachItem(Func&& fn) const {
        for (size_t i = 0; i < NUM_SHARDS; ++i) {
            std::shared_lock<std::shared_mutex> lock(m_shards[i].mutex);
            for (const auto& pair : m_shards[i].items) {
                fn(pair.first, pair.second);
            }
        }
    }

    template<typename Func>
    void forEachItemMut(Func&& fn) {
        for (size_t i = 0; i < NUM_SHARDS; ++i) {
            std::unique_lock<std::shared_mutex> lock(m_shards[i].mutex);
            for (auto& pair : m_shards[i].items) {
                fn(pair.first, pair.second);
            }
        }
    }

    std::array<MetaShard, NUM_SHARDS>& shards() { return m_shards; }

private:
    MetaMemoryCache() = default;
    ~MetaMemoryCache() = default;

    std::array<MetaShard, NUM_SHARDS> m_shards;
};

} // namespace QuarkMeta
