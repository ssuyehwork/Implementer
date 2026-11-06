#pragma once

#include <QObject>
#include <QStringList>

namespace RapidNotes {

class LibrarySyncService : public QObject {
    Q_OBJECT
public:
    static LibrarySyncService& instance() {
        static LibrarySyncService inst;
        return inst;
    }

    void syncPaths(const QStringList& paths) { Q_UNUSED(paths); }
    void removePaths(const QStringList& paths) { Q_UNUSED(paths); }
    bool isLibraryPath(const QString& path) const { Q_UNUSED(path); return false; }
    void triggerStartupReconciliation() {}

signals:
    void libraryItemsSynced();

private:
    explicit LibrarySyncService(QObject* parent = nullptr) : QObject(parent) {}
    ~LibrarySyncService() override = default;
};

} // namespace RapidNotes
