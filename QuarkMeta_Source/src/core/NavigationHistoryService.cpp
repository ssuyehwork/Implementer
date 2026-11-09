#include "NavigationHistoryService.h"
#include "AppConfig.h"
#include "../meta/MetadataManager.h"
#include <QDir>

namespace QuarkMeta {

NavigationHistoryService& NavigationHistoryService::instance() {
    static NavigationHistoryService inst;
    return inst;
}

NavigationHistoryService::NavigationHistoryService(QObject* parent) : QObject(parent) {}

QStringList NavigationHistoryService::getHistory() const {
    return AppConfig::instance().getValue("AddressBar/History").toStringList();
}

void NavigationHistoryService::appendPath(const QString& rawPath) {
    if (rawPath.isEmpty() || rawPath == "computer://" || rawPath.startsWith("分类: ")) return;
    
    // 【归一化修复】将正反斜杠统一为 QDir::cleanPath 的标准格式
    QString cleanP = QDir::cleanPath(rawPath);
    if (cleanP.endsWith('/') || cleanP.endsWith('\\')) {
        if (cleanP.length() > 3) { // 保留盘符如 "C:/"
            cleanP.chop(1);
        }
    }

    QStringList history = getHistory();
    
    // 大小写不敏感去重
    for (int i = history.size() - 1; i >= 0; --i) {
        if (QDir::cleanPath(history[i]).compare(cleanP, Qt::CaseInsensitive) == 0) {
            history.removeAt(i);
        }
    }
    
    history.prepend(cleanP);
    while (history.size() > m_maxLimit) {
        history.removeLast();
    }
    AppConfig::instance().setValue("AddressBar/History", history);
    AppConfig::instance().sync();
    emit historyChanged(history);
}

void NavigationHistoryService::removePath(const QString& path) {
    QStringList history = getHistory();
    history.removeAll(path);
    AppConfig::instance().setValue("AddressBar/History", history);
    AppConfig::instance().sync();
    emit historyChanged(history);
}

void NavigationHistoryService::clearAll() {
    AppConfig::instance().setValue("AddressBar/History", QStringList());
    AppConfig::instance().sync();
    emit historyChanged(QStringList());
}

void NavigationHistoryService::recordRecentVisitedFolder(const std::wstring& path) {
    if (path.empty()) return;

    std::wstring volSerial = MetadataManager::getVolumeSerialNumber(path);
    if (volSerial.empty()) return;

    QString key = QString("RecentVisited/Volume_%1").arg(QString::fromStdWString(volSerial));
    QStringList list = AppConfig::instance().getValue(key, QStringList()).toStringList();

    QString qPath = QString::fromStdWString(MetadataManager::normalizePath(path));
    list.removeAll(qPath);
    list.prepend(qPath);
    while (list.size() > 14) list.removeLast();

    AppConfig::instance().setValue(key, list);
}

QStringList NavigationHistoryService::getRecentVisitedFolders(const std::wstring& volSerial) {
    if (volSerial.empty()) return QStringList();
    QString key = QString("RecentVisited/Volume_%1").arg(QString::fromStdWString(volSerial));
    return AppConfig::instance().getValue(key, QStringList()).toStringList();
}

} // namespace QuarkMeta
