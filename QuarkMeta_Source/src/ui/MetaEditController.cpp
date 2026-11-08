#include "MetaEditController.h"
#include "ActivePaneCoordinator.h"
#include "../core/CoreEngine.h"
#include "../util/ShellHelper.h"
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QColor>

namespace QuarkMeta {

MetaEditController::MetaEditController(ActivePaneCoordinator* activeCoordinator, MetaPanel* metaPanel, QObject* parent)
    : QObject(parent), m_activeCoordinator(activeCoordinator), m_metaPanel(metaPanel) {
}

void MetaEditController::wire() {
    if (!m_metaPanel || !m_activeCoordinator) return;

    connect(m_metaPanel, &MetaPanel::ratingChanged, this, [](const QStringList& paths, int rating) {
        if (paths.isEmpty()) return;
        AppCommand cmd;
        cmd.type = AppCommandType::SetRating;
        cmd.targetPaths = paths;
        cmd.params["rating"] = rating;
        CoreEngine::instance().executeCommand(cmd);
    });

    connect(m_metaPanel, &MetaPanel::colorChanged, this, [](const QStringList& paths, const QString& hex) {
        if (paths.isEmpty()) return;
        AppCommand cmd;
        cmd.type = AppCommandType::SetColor;
        cmd.targetPaths = paths;
        cmd.params["color"] = hex;
        CoreEngine::instance().executeCommand(cmd);
    });

    connect(m_metaPanel, &MetaPanel::primaryColorChanged, this, [](const QString& path, const QColor& color) {
        if (path.isEmpty()) return;
        AppCommand cmd;
        cmd.type = AppCommandType::SetColor;
        cmd.targetPaths = {path};
        cmd.params["color"] = color.name(QColor::HexRgb);
        CoreEngine::instance().executeCommand(cmd);
    });

    connect(m_metaPanel, &MetaPanel::tagsAddRequested, this, [](const QStringList& paths, const QStringList& tags) {
        if (paths.isEmpty() || tags.isEmpty()) return;
        if (paths.size() > 50) {
            CoreEngine::instance().addTagsToPathsAsync(paths, tags);
        } else {
            for (const QString& tag : tags) {
                AppCommand cmd;
                cmd.type = AppCommandType::AddTag;
                cmd.targetPaths = paths;
                cmd.params["tag"] = tag;
                CoreEngine::instance().executeCommand(cmd);
            }
        }
    });

    connect(m_metaPanel, &MetaPanel::tagsRemoveRequested, this, [](const QStringList& paths, const QStringList& tags) {
        if (paths.isEmpty() || tags.isEmpty()) return;
        for (const QString& tag : tags) {
            AppCommand cmd;
            cmd.type = AppCommandType::RemoveTag;
            cmd.targetPaths = paths;
            cmd.params["tag"] = tag;
            CoreEngine::instance().executeCommand(cmd);
        }
    });

    connect(m_metaPanel, &MetaPanel::renameRequested, this, [this](const QString& oldPath, const QString& newName) {
        ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
        bool ok = ShellHelper::renameItem(oldPath, newName);
        if (ok) {
            if (target) {
                QString parentDir = QFileInfo(oldPath).absolutePath();
                QString newPath = QDir(parentDir).filePath(newName);
                target->migrateModelCache(oldPath, newPath);
                target->refreshAll();
            }
        } else {
            if (target) {
                target->updateItemMetadata(oldPath);
            }
        }
    });

    connect(m_metaPanel, &MetaPanel::noteEdited, this, [](const QStringList& paths, const QString& newNote) {
        if (!paths.isEmpty()) {
            AppCommand cmd;
            cmd.type = AppCommandType::SetNote;
            cmd.targetPaths = paths;
            cmd.params["note"] = newNote;
            CoreEngine::instance().executeCommand(cmd);
        }
    });

    connect(m_metaPanel, &MetaPanel::linkEdited, this, [](const QStringList& paths, const QString& newLink) {
        if (!paths.isEmpty()) {
            AppCommand cmd;
            cmd.type = AppCommandType::SetURL;
            cmd.targetPaths = paths;
            cmd.params["url"] = newLink;
            CoreEngine::instance().executeCommand(cmd);
        }
    });
}

} // namespace QuarkMeta
