#include "MetaMemoryCache.h"
#include <functional>

namespace QuarkMeta {

MetaMemoryCache& MetaMemoryCache::instance() {
    static MetaMemoryCache inst;
    return inst;
}

size_t MetaMemoryCache::getShardIndex(const std::wstring& path) const {
    return std::hash<std::wstring>{}(path) % NUM_SHARDS;
}

RuntimeMeta MetaMemoryCache::getMeta(const std::wstring& normalizedPath) {
    size_t idx = getShardIndex(normalizedPath);
    std::shared_lock<std::shared_mutex> lock(m_shards[idx].mutex);
    auto it = m_shards[idx].items.find(normalizedPath);
    if (it != m_shards[idx].items.end()) {
        return it->second;
    }
    return RuntimeMeta();
}

bool MetaMemoryCache::contains(const std::wstring& normalizedPath) {
    size_t idx = getShardIndex(normalizedPath);
    std::shared_lock<std::shared_mutex> lock(m_shards[idx].mutex);
    return m_shards[idx].items.find(normalizedPath) != m_shards[idx].items.end();
}

void MetaMemoryCache::put(const std::wstring& normalizedPath, const RuntimeMeta& meta) {
    size_t idx = getShardIndex(normalizedPath);
    std::unique_lock<std::shared_mutex> lock(m_shards[idx].mutex);
    m_shards[idx].items[normalizedPath] = meta;
}

void MetaMemoryCache::update(const std::wstring& normalizedPath, std::function<void(RuntimeMeta&)> modifier) {
    size_t idx = getShardIndex(normalizedPath);
    std::unique_lock<std::shared_mutex> lock(m_shards[idx].mutex);
    auto it = m_shards[idx].items.find(normalizedPath);
    if (it != m_shards[idx].items.end()) {
        modifier(it->second);
    }
}

void MetaMemoryCache::remove(const std::wstring& normalizedPath) {
    size_t idx = getShardIndex(normalizedPath);
    std::unique_lock<std::shared_mutex> lock(m_shards[idx].mutex);
    m_shards[idx].items.erase(normalizedPath);
}

} // namespace QuarkMeta
