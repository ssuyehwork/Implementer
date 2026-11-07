#pragma once

#include <QString>
#include <QStringList>
#include <QDateTime>

namespace QuarkMeta {

struct MarkedFolderRecord {
    int id = 0;
    QString folderPath;
    bool isMarked = true;
    qint64 updatedAt = 0;
};

class MarkedFolderDao {
public:
    static bool initTable();
    static int getMarkedCount();
    static bool addMarkedFolder(const QString& folderPath);
    static bool unmarkFolder(const QString& folderPath);
    static bool unmarkAllFolders();
    static bool removeFolderRecord(const QString& folderPath);
    static bool clearAllFolderRecords();
    static bool isMarked(const QString& folderPath);
    static QStringList getAllMarkedFolders();
    static QStringList getActiveMarkedFolders();
};

} // namespace QuarkMeta
