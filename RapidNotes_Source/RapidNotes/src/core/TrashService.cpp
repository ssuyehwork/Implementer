#include "TrashService.h"
#include "DatabaseManager.h"

namespace RapidNotes {

TrashService& TrashService::instance() {
    static TrashService s_instance;
    return s_instance;
}

TrashService::TrashService(QObject* parent) : QObject(parent) {}

bool TrashService::moveToTrash(const QStringList& paths, QWidget* parentWidget) {
    Q_UNUSED(parentWidget);
    if (paths.isEmpty()) return false;

    QList<int> noteIds;
    for (const QString& p : paths) {
        if (p.startsWith("note://")) {
            bool ok = false;
            int id = p.mid(7).toInt(&ok);
            if (ok) noteIds.append(id);
        } else {
            bool ok = false;
            int id = p.toInt(&ok);
            if (ok) noteIds.append(id);
        }
    }

    if (!noteIds.isEmpty()) {
        DatabaseManager::instance().updateNoteStateBatch(noteIds, "is_deleted", 1);
        emit trashOperationCompleted();
        return true;
    }
    return false;
}

bool TrashService::restoreItems(const QList<int>& trashIds, QWidget* parentWidget) {
    Q_UNUSED(parentWidget);
    if (trashIds.isEmpty()) return false;

    DatabaseManager::instance().updateNoteStateBatch(trashIds, "is_deleted", 0);
    emit trashOperationCompleted();
    return true;
}

bool TrashService::restoreAll(QWidget* parentWidget) {
    Q_UNUSED(parentWidget);
    bool ok = DatabaseManager::instance().restoreAllFromTrash();
    if (ok) {
        emit trashOperationCompleted();
    }
    return ok;
}

bool TrashService::restoreToDirectory(const QStringList& trashPaths, const QString& targetDir, QWidget* parentWidget) {
    Q_UNUSED(parentWidget);
    Q_UNUSED(targetDir);
    return moveToTrash(trashPaths, parentWidget);
}

bool TrashService::emptyTrash(QWidget* parentWidget) {
    Q_UNUSED(parentWidget);
    bool ok = DatabaseManager::instance().emptyTrash();
    if (ok) {
        emit trashOperationCompleted();
    }
    return ok;
}

} // namespace RapidNotes
