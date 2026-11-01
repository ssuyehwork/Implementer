#include "NavigationService.h"
#include "NavigationHistoryService.h"
#include "DeviceWatcher.h"
#include "../meta/LibraryDao.h"
#include "../meta/SmartFolderDao.h"
#include <QDir>
#include <QFileInfo>

namespace QuarkMeta {

NavigationService& NavigationService::instance() {
    static NavigationService s_instance;
    return s_instance;
}

NavigationService::NavigationService(QObject* parent) : QObject(parent) {
    connect(&DeviceWatcher::instance(), &DeviceWatcher::driveUnmounted, this, [this](const QString& driveLetter) {
        if (m_currentUrl.contains(driveLetter, Qt::CaseInsensitive)) {
            navigateTo("computer://");
        }
    });
}

QString NavigationService::normalizeUrl(const QString& rawUrl) const {
    if (rawUrl.isEmpty()) return "computer://";

    QString url = rawUrl.trimmed();
    if (url.startsWith("file://", Qt::CaseInsensitive)) {
        url = url.mid(7);
    }

    if (url == "computer://" || url == "trash://" || url.contains("://")) {
        return url;
    }

    QString clean = QDir::fromNativeSeparators(QDir::cleanPath(url));
    if (clean.endsWith(':')) {
        clean += "/";
    }
    return clean;
}

QString NavigationService::currentDisplayPath() const {
    if (m_currentUrl == "computer://") return "此电脑";
    if (m_currentUrl == "trash://") return "回收站";

    if (m_currentUrl.startsWith("library://")) {
        int catId = m_currentUrl.mid(10).toInt();
        if (catId == -1) return "库 → 全部数据";
        if (catId == -2) return "库 → 未分类";
        LibraryCategoryRecord rec = LibraryDao::getCategoryById(catId);
        return rec.name.isEmpty() ? "库" : ("库 → " + rec.name);
    }

    if (m_currentUrl.startsWith("smartfolder://")) {
        int sfId = m_currentUrl.mid(14).toInt();
        SmartFolderRecord rec = SmartFolderDao::getSmartFolderById(sfId);
        return rec.name.isEmpty() ? "库" : ("库 → " + rec.name);
    }

    if (m_currentUrl.contains("://")) return m_currentUrl;
    return QDir::toNativeSeparators(m_currentUrl);
}

bool NavigationService::isVirtualProtocol() const {
    return m_currentUrl.contains("://");
}

bool NavigationService::canGoUp() const {
    if (m_currentUrl.isEmpty() || m_currentUrl == "computer://" || m_currentUrl == "trash://") {
        return false;
    }
    return true;
}

void NavigationService::emitNavState() {
    emit navStateChanged(canGoBack(), canGoForward(), canGoUp());
}

void NavigationService::openInNewTab(const QString& rawPath) {
    if (rawPath.isEmpty()) return;
    QString normUrl = normalizeUrl(rawPath);
    emit requestOpenInNewTab(normUrl);
}

void NavigationService::navigateTo(const QString& rawUrl, bool recordHistory, bool recursive) {
    if (rawUrl.isEmpty()) return;

    QString normalized = normalizeUrl(rawUrl);

    if (recordHistory) {
        if (m_currentIndex < m_history.size() - 1) {
            m_history = m_history.mid(0, m_currentIndex + 1);
        }

        if (m_history.isEmpty() || m_history.last() != normalized) {
            m_history.append(normalized);
            if (m_history.size() > kMaxHistoryDepth) {
                m_history.removeFirst();
            }
            m_currentIndex = m_history.size() - 1;
        }
    }

    m_currentUrl = normalized;

    if (!isVirtualProtocol()) {
        NavigationHistoryService::instance().appendPath(normalized);
        NavigationHistoryService::recordRecentVisitedFolder(QDir::toNativeSeparators(m_currentUrl).toStdWString());
    }

    emit currentUrlChanged(m_currentUrl, currentDisplayPath(), recursive);
    emitNavState();
}

void NavigationService::goBack() {
    if (canGoBack()) {
        m_currentIndex--;
        m_currentUrl = m_history[m_currentIndex];
        emit currentUrlChanged(m_currentUrl, currentDisplayPath(), false);
        emitNavState();
    }
}

void NavigationService::goForward() {
    if (canGoForward()) {
        m_currentIndex++;
        m_currentUrl = m_history[m_currentIndex];
        emit currentUrlChanged(m_currentUrl, currentDisplayPath(), false);
        emitNavState();
    }
}

QString NavigationService::takePendingGoUpChildPath() {
    QString path = m_pendingGoUpChildPath;
    m_pendingGoUpChildPath.clear();
    return path;
}

void NavigationService::goUp() {
    if (!canGoUp()) return;

    QString departingPath = m_currentUrl;
    QDir dir(m_currentUrl);
    if (dir.isRoot()) {
        m_pendingGoUpChildPath.clear();
        navigateTo("computer://");
        return;
    }

    if (dir.cdUp()) {
        m_pendingGoUpChildPath = departingPath;
        navigateTo(dir.absolutePath());
    } else {
        m_pendingGoUpChildPath.clear();
        navigateTo("computer://");
    }
}

void NavigationService::refresh() {
    if (!m_currentUrl.isEmpty()) {
        emit currentUrlChanged(m_currentUrl, currentDisplayPath(), false);
    }
}

} // namespace QuarkMeta
