#pragma once
#include <QString>
#include <QStringList>
#include <QObject>

namespace RapidNotes {

class DiskTrashService : public QObject {
    Q_OBJECT
public:
    static bool moveToDiskTrash(const QStringList& paths) { Q_UNUSED(paths); return true; }
    static bool restoreFromDiskTrash(int id, const QString& trashPath) { Q_UNUSED(id); Q_UNUSED(trashPath); return true; }
};

} // namespace RapidNotes
