#pragma once

#include <QString>
#include <QStringList>
#include <QDateTime>

namespace QuarkMeta {

struct HistoryRecord {
    int id = 0;
    QString filePath;
    QDateTime accessedAt;
};

class HistoryDao {
public:
    static bool initTable();
    static bool addOrUpdateHistory(const QString& path);
    static QList<HistoryRecord> getAllHistory();
    static bool removeHistory(const QString& path);
    static bool clearAllHistory();
    static bool updateHistoryPaths(const QStringList& oldPaths, const QStringList& newPaths);
};

} // namespace QuarkMeta
