#include "ContentFileOpsHandler.h"
#include "../ContentPanel.h"

namespace RapidNotes {

ContentFileOpsHandler::ContentFileOpsHandler(ContentPanel* panel)
    : QObject(panel), m_panel(panel) {
}

void ContentFileOpsHandler::createNewItem(const QString& type) {
    Q_UNUSED(type);
}

void ContentFileOpsHandler::performBatchRename() {
}

bool ContentFileOpsHandler::resolvePasteDestination() {
    return false;
}

void ContentFileOpsHandler::onPathsDropped(const QStringList& paths, const QModelIndex& targetIndex, const QString& targetDirOverride, Qt::DropAction action) {
    Q_UNUSED(paths);
    Q_UNUSED(targetIndex);
    Q_UNUSED(targetDirOverride);
    Q_UNUSED(action);
}

} // namespace RapidNotes
