#include "NavigationService.h"
#include "NavigationHistoryService.h"
#include "DatabaseManager.h"
#include <QSqlQuery>

namespace RapidNotes {

NavigationService& NavigationService::instance() {
    static NavigationService s_instance;
    return s_instance;
}

NavigationService::NavigationService(QObject* parent) : QObject(parent), m_currentUrl("library://-1") {
}

QString NavigationService::normalizeUrl(const QString& rawUrl) const {
    if (rawUrl.isEmpty()) return "library://-1";

    QString url = rawUrl.trimmed();
    if (url == "computer://" || url == "库" || url == "全部数据" || url == "library://") {
        return "library://-1";
    }

    if (url.startsWith("library://") || url == "trash://") {
        return url;
    }

    if (url.startsWith("category://")) {
        return QString("library://%1").arg(url.mid(11));
    }

    // 若传入的是纯数字字符串，转化为 library://<id>
    bool isInt = false;
    int idVal = url.toInt(&isInt);
    if (isInt) {
        return QString("library://%1").arg(idVal);
    }

    return url;
}

QString NavigationService::currentDisplayPath() const {
    if (m_currentUrl == "trash://") return "回收站";

    if (m_currentUrl.startsWith("library://")) {
        int catId = m_currentUrl.mid(10).toInt();
        if (catId == -1) return "库 → 全部数据";
        if (catId == -2) return "库 → 未分类";

        QString catName = DatabaseManager::instance().getCategoryNameById(catId);
        if (!catName.isEmpty()) {
            return QString("库 → %1").arg(catName);
        }
        return "库";
    }

    return m_currentUrl;
}

bool NavigationService::isVirtualProtocol() const {
    return true;
}

bool NavigationService::canGoUp() const {
    if (m_currentUrl.isEmpty() || m_currentUrl == "library://-1" || m_currentUrl == "library://-2" || m_currentUrl == "trash://") {
        return false;
    }
    if (m_currentUrl.startsWith("library://")) {
        int catId = m_currentUrl.mid(10).toInt();
        return catId > 0;
    }
    return false;
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
    NavigationHistoryService::instance().appendPath(normalized);

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

    if (m_currentUrl.startsWith("library://")) {
        int catId = m_currentUrl.mid(10).toInt();
        if (catId > 0) {
            QSqlQuery query(DatabaseManager::instance().database());
            query.prepare("SELECT parent_id FROM categories WHERE id = :id");
            query.bindValue(":id", catId);
            if (query.exec() && query.next()) {
                int parentId = query.value(0).isNull() ? -1 : query.value(0).toInt();
                if (parentId > 0) {
                    navigateTo(QString("library://%1").arg(parentId));
                    return;
                }
            }
        }
    }

    // 默认回到全部数据
    navigateTo("library://-1");
}

void NavigationService::refresh() {
    if (!m_currentUrl.isEmpty()) {
        emit currentUrlChanged(m_currentUrl, currentDisplayPath(), false);
    }
}

} // namespace RapidNotes
