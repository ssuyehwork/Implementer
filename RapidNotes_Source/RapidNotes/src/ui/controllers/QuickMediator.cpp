#include "QuickMediator.h"
#include "../QuickWindow.h"
#include "../../core/CoreEngine.h"
#include "../../core/CentralEventHub.h"

namespace RapidNotes {

QuickMediator::QuickMediator(QuickWindow* window, QObject* parent)
    : QObject(parent), m_window(window) {
    auto& hub = CentralEventHub::instance();
    connect(&hub, &CentralEventHub::eventPublished, this, [this](const AppEvent& event) {
        if (event.type == AppEventType::MetadataUpdated) {
            onMetadataUpdated(event.paths);
        } else if (event.type == AppEventType::ItemsDeleted) {
            onItemsDeleted(event.paths);
        }
    });
}

void QuickMediator::requestMoveToCategory(const QList<int>& ids, int categoryId) {
    if (ids.isEmpty()) return;
    AppCommand cmd;
    cmd.type = AppCommandType::MoveToCategory;
    for (int id : ids) cmd.targetPaths.append(QString("note://%1").arg(id));
    cmd.params["categoryId"] = categoryId;
    CoreEngine::instance().executeCommand(cmd);
}

void QuickMediator::requestToggleFavorite(const QList<int>& ids) {
    if (ids.isEmpty()) return;
    AppCommand cmd;
    cmd.type = AppCommandType::ToggleFavorite;
    for (int id : ids) cmd.targetPaths.append(QString("note://%1").arg(id));
    CoreEngine::instance().executeCommand(cmd);
}

void QuickMediator::requestMoveToTrash(const QList<int>& ids) {
    if (ids.isEmpty()) return;
    AppCommand cmd;
    cmd.type = AppCommandType::MoveToTrash;
    for (int id : ids) cmd.targetPaths.append(QString("note://%1").arg(id));
    CoreEngine::instance().executeCommand(cmd);
}

void QuickMediator::requestPermanentDelete(const QList<int>& ids) {
    if (ids.isEmpty()) return;
    AppCommand cmd;
    cmd.type = AppCommandType::DeletePermanently;
    for (int id : ids) cmd.targetPaths.append(QString("note://%1").arg(id));
    CoreEngine::instance().executeCommand(cmd);
}

void QuickMediator::requestSetRating(const QList<int>& ids, int rating) {
    if (ids.isEmpty()) return;
    AppCommand cmd;
    cmd.type = AppCommandType::SetRating;
    for (int id : ids) cmd.targetPaths.append(QString("note://%1").arg(id));
    cmd.params["rating"] = rating;
    CoreEngine::instance().executeCommand(cmd);
}

void QuickMediator::requestSetColor(const QList<int>& ids, const QString& color) {
    if (ids.isEmpty()) return;
    AppCommand cmd;
    cmd.type = AppCommandType::SetColor;
    for (int id : ids) cmd.targetPaths.append(QString("note://%1").arg(id));
    cmd.params["color"] = color;
    CoreEngine::instance().executeCommand(cmd);
}

void QuickMediator::requestSetTags(const QList<int>& ids, const QStringList& tags) {
    if (ids.isEmpty()) return;
    AppCommand cmd;
    cmd.type = AppCommandType::SetTags;
    for (int id : ids) cmd.targetPaths.append(QString("note://%1").arg(id));
    cmd.params["tags"] = tags;
    CoreEngine::instance().executeCommand(cmd);
}

void QuickMediator::onMetadataUpdated(const QStringList& paths) {
    Q_UNUSED(paths);
    if (m_window) m_window->scheduleRefresh();
}

void QuickMediator::onItemsDeleted(const QStringList& paths) {
    Q_UNUSED(paths);
    if (m_window) m_window->scheduleRefresh();
}

} // namespace RapidNotes
