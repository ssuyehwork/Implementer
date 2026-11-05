#include "ContentChangeWatcher.h"
#include "../ContentPanel.h"

namespace RapidNotes {

ContentChangeWatcher::ContentChangeWatcher(ContentPanel* panel)
    : QObject(panel), m_panel(panel) {
}

void ContentChangeWatcher::updateTarget() {}
void ContentChangeWatcher::onLoadFinished() {}
void ContentChangeWatcher::beginSuppress() { ++m_suppressCount; }
void ContentChangeWatcher::endSuppress() { if (m_suppressCount > 0) --m_suppressCount; }
void ContentChangeWatcher::onDirectoryChanged(const QString&, const QStringList&, bool) {}
void ContentChangeWatcher::onWatchedDirectoryLost(const QString&) {}
void ContentChangeWatcher::processAccumulatedChanges() {}
void ContentChangeWatcher::unwatchAll() {}
bool ContentChangeWatcher::shouldWatchPath(const QString&) const { return false; }
bool ContentChangeWatcher::isDelayed() const { return false; }

} // namespace RapidNotes
