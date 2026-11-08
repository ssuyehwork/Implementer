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

QString NavigationService::computeAscendingTarget(const QString& fromPath, const QString& toPath) {
    if (fromPath.isEmpty() || toPath.isEmpty()) return QString();
    if (fromPath.contains("://") || toPath.contains("://")) return QString();

    QString fromNorm = QDir::fromNativeSeparators(QDir::cleanPath(fromPath));
    QString toNorm = QDir::fromNativeSeparators(QDir::cleanPath(toPath));

    if (toNorm.endsWith(':')) toNorm += "/";
    if (fromNorm.endsWith(':')) fromNorm += "/";

    QString fromLower = fromNorm.toLower();
    QString toLower = toNorm.toLower();
    if (toLower.endsWith('/')) toLower.chop(1);
    if (fromLower.endsWith('/')) fromLower.chop(1);

    if (fromLower == toLower) return QString();

    QString prefix = toLower + "/";
    if (!fromLower.startsWith(prefix)) return QString();

    QString rel = fromNorm.mid(prefix.length());
    int slashIdx = rel.indexOf('/');
    QString directChildName = (slashIdx == -1) ? rel : rel.left(slashIdx);
    if (directChildName.isEmpty()) return QString();

    QDir targetDir(toNorm);
    return QDir::toNativeSeparators(targetDir.filePath(directChildName));
}

QString NavigationService::takePendingTargetChildPath(const QString& targetUrl) {
    QString normTarget = normalizeUrl(targetUrl);
    if (QString::compare(normTarget, m_pendingTargetExpectedUrl, Qt::CaseInsensitive) == 0) {
        QString res = m_pendingTargetChildPath;
        m_pendingTargetChildPath.clear();
        m_pendingTargetExpectedUrl.clear();
        return res;
    }
    m_pendingTargetChildPath.clear();
    m_pendingTargetExpectedUrl.clear();
    return QString();
}

void NavigationService::navigateTo(const QString& rawUrl, bool recordHistory, bool recursive) {
    if (rawUrl.isEmpty()) return;

    QString normalized = normalizeUrl(rawUrl);

    // 计算返回上一级时需选中的子项
    QString ascTarget = computeAscendingTarget(m_currentUrl, normalized);
    if (!ascTarget.isEmpty()) {
        m_pendingTargetChildPath = ascTarget;
        m_pendingTargetExpectedUrl = normalized;
    }

    // 路径跳转幂等性守卫 (No-op Protection)：
    // 若目标路径与当前路径一致（大小写不敏感对比）且非递归状态变更，直接返回，阻断无意义的视图重载与筛选面板重置
    if (QString::compare(normalized, m_currentUrl, Qt::CaseInsensitive) == 0 && !recursive) {
        return;
    }

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
        QString prevUrl = m_history[m_currentIndex - 1];
        QString ascTarget = computeAscendingTarget(m_currentUrl, prevUrl);
        if (!ascTarget.isEmpty()) {
            m_pendingTargetChildPath = ascTarget;
            m_pendingTargetExpectedUrl = normalizeUrl(prevUrl);
        }
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

void NavigationService::goUp() {
    if (!canGoUp()) return;

    QDir dir(m_currentUrl);
    if (dir.isRoot()) {
        navigateTo("computer://");
        return;
    }

    if (dir.cdUp()) {
        navigateTo(dir.absolutePath());
    } else {
        navigateTo("computer://");
    }
}

void NavigationService::refresh() {
    if (!m_currentUrl.isEmpty()) {
        emit currentUrlChanged(m_currentUrl, currentDisplayPath(), false);
    }
}

void NavigationService::locateItem(const QString& fullPath) {
    if (fullPath.isEmpty()) return;
    emit requestLocateItem(fullPath);
}

} // namespace QuarkMeta
