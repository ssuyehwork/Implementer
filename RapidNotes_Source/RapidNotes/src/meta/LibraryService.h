#pragma once

#include <QObject>
#include <QStringList>
#include <QMenu>
#include <QAction>
#include "LibraryDao.h"

namespace RapidNotes {

class LibraryService : public QObject {
    Q_OBJECT
public:
    static LibraryService& instance() {
        static LibraryService s_instance;
        return s_instance;
    }

    QStringList getCategoryPaths(int id) const {
        return LibraryDao::getCategoryPaths(id);
    }

    int createCategory(const QString& name, int parentId = 0) {
        return LibraryDao::addCategory(name, parentId);
    }

    bool removeCategory(int id) {
        return LibraryDao::removeCategoryById(id);
    }

    bool addPathsToCategory(int id, const QStringList& paths) {
        Q_UNUSED(id);
        Q_UNUSED(paths);
        return true;
    }

    bool removePathsFromCategory(int id, const QStringList& paths) {
        Q_UNUSED(id);
        Q_UNUSED(paths);
        return true;
    }

    QAction* buildLibraryAction(QMenu* parentMenu, const QStringList& targetPaths, QObject* receiver = nullptr) {
        Q_UNUSED(parentMenu);
        Q_UNUSED(targetPaths);
        Q_UNUSED(receiver);
        return nullptr;
    }

signals:
    void libraryChanged();

private:
    explicit LibraryService(QObject* parent = nullptr) : QObject(parent) {}
    ~LibraryService() override = default;
};

} // namespace RapidNotes
