#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include "HistoryDao.h"

namespace QuarkMeta {

class HistoryService : public QObject {
    Q_OBJECT

public:
    static HistoryService& instance();

    bool recordFileAccess(const QString& path);
    QList<HistoryRecord> getAllHistory();
    bool removeHistory(const QString& path);
    bool clearAllHistory();

signals:
    void historyItemAdded(const QString& path);
    void historyItemRemoved(const QString& path);
    void historyCleared();

private:
    explicit HistoryService(QObject* parent = nullptr);
    ~HistoryService() override = default;
};

} // namespace QuarkMeta
