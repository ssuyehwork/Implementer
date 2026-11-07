#include "IconCacheManager.h"
#include "SvgIconRenderer.h"
#include <QFileIconProvider>
#include <QFileInfo>

namespace QuarkMeta {

IconCacheManager& IconCacheManager::instance() {
    static IconCacheManager inst;
    return inst;
}

IconCacheManager::IconCacheManager(QObject* parent)
    : QObject(parent) {
}

QIcon IconCacheManager::getCachedIcon(const QString& ext, bool isDir) {
    QString key = isDir ? "folder" : ext.toLower();
    {
        QReadLocker lock(&m_cacheLock);
        auto it = m_iconCache.find(key);
        if (it != m_iconCache.end()) return *it;
    }

    QFileIconProvider provider;
    QIcon icon;
    try {
        if (isDir) {
            icon = SvgIconRenderer::getIcon("folder_filled", QColor("#888888"), 128);
        } else {
            if (key.length() > 12) key = "unknown";
            icon = provider.icon(QFileInfo("dummy." + key));
            if (icon.isNull()) icon = provider.icon(QFileIconProvider::File);
        }
    } catch (...) {
        icon = SvgIconRenderer::getIcon(isDir ? "folder_filled" : "file", QColor("#888888"), 128);
    }

    {
        QWriteLocker lock(&m_cacheLock);
        m_iconCache[key] = icon;
    }
    return icon;
}

} // namespace QuarkMeta
