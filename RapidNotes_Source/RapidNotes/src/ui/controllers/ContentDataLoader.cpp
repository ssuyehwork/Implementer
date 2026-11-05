#include "ContentDataLoader.h"
#include "../ContentPanel.h"

namespace RapidNotes {

ContentDataLoader::ContentDataLoader(ContentPanel* panel)
    : QObject(panel), m_panel(panel) {
}

void ContentDataLoader::loadDirectory(const QString& path, bool recursive) {
    Q_UNUSED(path);
    Q_UNUSED(recursive);
}

void ContentDataLoader::loadCategory(const QString& categoryType) {
    Q_UNUSED(categoryType);
}

void ContentDataLoader::loadPaths(const QStringList& paths, int reqId) {
    Q_UNUSED(paths);
    Q_UNUSED(reqId);
}

void ContentDataLoader::loadLibraryPaths(const QStringList& paths, int reqId) {
    Q_UNUSED(paths);
    Q_UNUSED(reqId);
}

void ContentDataLoader::appendPaths(const QStringList& paths, int reqId) {
    Q_UNUSED(paths);
    Q_UNUSED(reqId);
}

void ContentDataLoader::loadSmartFolder(const SmartFolderRecord& record, const QString& displayName) {
    Q_UNUSED(record);
    Q_UNUSED(displayName);
}

} // namespace RapidNotes
