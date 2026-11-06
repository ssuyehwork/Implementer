#include "PermanentDeleteService.h"
#include "DatabaseManager.h"
#include <QList>

namespace RapidNotes {

PermanentDeleteService& PermanentDeleteService::instance() {
    static PermanentDeleteService s_instance;
    return s_instance;
}

PermanentDeleteService::PermanentDeleteService(QObject* parent) : QObject(parent) {}

bool PermanentDeleteService::execute(const QStringList& paths, QWidget* parentWidget, bool isSecureShred) {
    Q_UNUSED(parentWidget);
    Q_UNUSED(isSecureShred);
    if (paths.isEmpty()) return false;

    for (const QString& p : paths) {
        bool ok = false;
        int id = p.startsWith("note://") ? p.mid(7).toInt(&ok) : p.toInt(&ok);
        if (ok) {
            DatabaseManager::instance().deleteNotePermanently(id);
        }
    }
    emit permanentDeleteCompleted();
    return true;
}

bool PermanentDeleteService::executeEmptyTrash(QWidget* parentWidget, bool isSecureShred) {
    Q_UNUSED(parentWidget);
    Q_UNUSED(isSecureShred);
    bool ok = DatabaseManager::instance().emptyTrash();
    if (ok) {
        emit permanentDeleteCompleted();
    }
    return ok;
}

} // namespace RapidNotes
