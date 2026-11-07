#include "HistoryService.h"
#include "../core/CentralEventHub.h"
#include <QFileInfo>
#include <QDir>

namespace QuarkMeta {

HistoryService& HistoryService::instance() {
    static HistoryService inst;
    return inst;
}

HistoryService::HistoryService(QObject* parent) : QObject(parent) {
    HistoryDao::initTable();

    connect(&CentralEventHub::instance(), &CentralEventHub::eventOccurred, this, [this](const QuarkMeta::AppEvent& event) {
        if (event.type == AppEventType::ItemsDeleted) {
            for (const QString& path : event.paths) {
                if (HistoryDao::removeHistory(path)) {
                    emit historyItemRemoved(path);
                }
            }
        } else if (event.type == AppEventType::ItemsRenamed) {
            QStringList oldPaths = event.paths;
            QStringList newPaths;
            if (event.payload.contains("newPaths")) {
                newPaths = event.payload["newPaths"].toStringList();
            } else if (!event.targetPath.isEmpty()) {
                newPaths.append(event.targetPath);
            }
            if (!oldPaths.isEmpty() && !newPaths.isEmpty() && oldPaths.size() == newPaths.size()) {
                HistoryDao::updateHistoryPaths(oldPaths, newPaths);
                emit historyCleared();
            }
        }
    });
}

bool HistoryService::recordFileAccess(const QString& rawPath) {
    if (rawPath.isEmpty()) return false;
    QFileInfo fi(rawPath);
    if (fi.isDir() || !fi.exists()) return false;

    QString cleanPath = QDir::toNativeSeparators(fi.absoluteFilePath());
    bool ok = HistoryDao::addOrUpdateHistory(cleanPath);
    if (ok) {
        emit historyItemAdded(cleanPath);
    }
    return ok;
}

QList<HistoryRecord> HistoryService::getAllHistory() {
    return HistoryDao::getAllHistory();
}

bool HistoryService::removeHistory(const QString& rawPath) {
    if (rawPath.isEmpty()) return false;
    QString cleanPath = QDir::toNativeSeparators(QFileInfo(rawPath).absoluteFilePath());
    bool ok = HistoryDao::removeHistory(cleanPath);
    if (ok) {
        emit historyItemRemoved(cleanPath);
    }
    return ok;
}

bool HistoryService::clearAllHistory() {
    bool ok = HistoryDao::clearAllHistory();
    if (ok) {
        emit historyCleared();
    }
    return ok;
}

} // namespace QuarkMeta
