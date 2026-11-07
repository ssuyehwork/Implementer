#include "CoreEngine.h"
#include "LastOperationManager.h"
#include "DatabaseManager.h"
#include "TagCodec.h"
#include "TagLexiconService.h"
#include "TaskProgressService.h"
#include <QCoreApplication>
#include <QRegularExpression>

namespace RapidNotes {

CoreEngine& CoreEngine::instance() {
    static CoreEngine s_instance;
    return s_instance;
}

CoreEngine::CoreEngine(QObject* parent)
    : QObject(parent) {
    m_asyncTagPool.setMaxThreadCount(1);
    qRegisterMetaType<RapidNotes::AppCommand>("RapidNotes::AppCommand");
}

std::shared_ptr<CancellationToken> CoreEngine::createCancellationToken() {
    return std::make_shared<CancellationToken>();
}

bool CoreEngine::executeCommand(const AppCommand& cmd) {
    if (cmd.targetPaths.isEmpty() && 
        cmd.type != AppCommandType::RecordAccess &&
        cmd.type != AppCommandType::RenameTag &&
        cmd.type != AppCommandType::RemoveGlobalTag) {
        return false;
    }

    switch (cmd.type) {
    case AppCommandType::SetRating: {
        int rating = cmd.params.value("rating", 0).toInt();
        handleSetRating(cmd.targetPaths, rating);
        break;
    }
    case AppCommandType::SetColor: {
        QString color = cmd.params.value("color").toString();
        handleSetColor(cmd.targetPaths, color);
        break;
    }
    case AppCommandType::SetTags: {
        QStringList tags = cmd.params.value("tags").toStringList();
        handleSetTags(cmd.targetPaths, tags);
        break;
    }
    case AppCommandType::AddTag: {
        QStringList tagsToApply;
        if (cmd.params.contains("tags")) {
            tagsToApply = cmd.params.value("tags").toStringList();
        }
        if (cmd.params.contains("tag")) {
            QString singleTag = cmd.params.value("tag").toString().trimmed();
            if (!singleTag.isEmpty()) {
                tagsToApply.append(singleTag);
            }
        }
        tagsToApply = TagCodec::normalize(tagsToApply);
        if (tagsToApply.isEmpty()) break;

        TagLexiconService::instance().addTags(tagsToApply);
        QList<int> noteIds = extractNoteIds(cmd.targetPaths);
        DatabaseManager::instance().addTagsToNotesBatch(noteIds, tagsToApply);

        AppEvent ev;
        ev.type = AppEventType::MetadataUpdated;
        ev.paths = cmd.targetPaths;
        CentralEventHub::instance().publishEvent(ev);
        break;
    }
    case AppCommandType::RemoveTag: {
        QStringList tagsToRemove;
        if (cmd.params.contains("tags")) {
            tagsToRemove = cmd.params.value("tags").toStringList();
        }
        if (cmd.params.contains("tag")) {
            QString singleTag = cmd.params.value("tag").toString().trimmed();
            if (!singleTag.isEmpty()) {
                tagsToRemove.append(singleTag);
            }
        }
        tagsToRemove = TagCodec::normalize(tagsToRemove);
        if (tagsToRemove.isEmpty()) break;

        QList<int> noteIds = extractNoteIds(cmd.targetPaths);
        DatabaseManager::instance().removeTagsFromNotesBatch(noteIds, tagsToRemove);

        AppEvent ev;
        ev.type = AppEventType::MetadataUpdated;
        ev.paths = cmd.targetPaths;
        CentralEventHub::instance().publishEvent(ev);
        break;
    }
    case AppCommandType::RenameTag: {
        QString oldTag = cmd.params.value("oldTag").toString().trimmed();
        QString newTag = cmd.params.value("newTag").toString().trimmed();
        if (oldTag.isEmpty() || newTag.isEmpty() || oldTag == newTag) break;

        TagLexiconService::instance().renameTag(oldTag, newTag);
        DatabaseManager::instance().renameTagGlobally(oldTag, newTag);

        AppEvent ev;
        ev.type = AppEventType::MetadataUpdated;
        CentralEventHub::instance().publishEvent(ev);
        break;
    }
    case AppCommandType::RemoveGlobalTag: {
        QString tag = cmd.params.value("tag").toString().trimmed();
        if (tag.isEmpty()) break;

        TagLexiconService::instance().deleteTag(tag);
        DatabaseManager::instance().deleteTagGlobally(tag);

        AppEvent ev;
        ev.type = AppEventType::MetadataUpdated;
        CentralEventHub::instance().publishEvent(ev);
        break;
    }
    case AppCommandType::SetPinned: {
        bool pinned = cmd.params.value("pinned", false).toBool();
        QList<int> noteIds = extractNoteIds(cmd.targetPaths);
        DatabaseManager::instance().updateNoteStateBatch(noteIds, "is_pinned", pinned ? 1 : 0);

        AppEvent ev;
        ev.type = AppEventType::MetadataUpdated;
        ev.paths = cmd.targetPaths;
        CentralEventHub::instance().publishEvent(ev);
        break;
    }
    case AppCommandType::SetNote: {
        QString note = cmd.params.value("note").toString();
        handleSetNote(cmd.targetPaths, note);
        break;
    }
    case AppCommandType::SetURL: {
        QString url = cmd.params.value("url").toString();
        handleSetURL(cmd.targetPaths, url);
        break;
    }
    case AppCommandType::RenameItems: {
        if (!cmd.targetPaths.isEmpty()) {
            QString newTitle = cmd.params.value("newName").toString().trimmed();
            handleRenameItem(cmd.targetPaths.first(), newTitle);
        }
        break;
    }
    case AppCommandType::MoveToTrash: {
        QList<int> noteIds = extractNoteIds(cmd.targetPaths);
        DatabaseManager::instance().softDeleteNotes(noteIds);

        AppEvent ev;
        ev.type = AppEventType::ItemsDeleted;
        ev.paths = cmd.targetPaths;
        CentralEventHub::instance().publishEvent(ev);
        break;
    }
    case AppCommandType::DeletePermanently: {
        QList<int> noteIds = extractNoteIds(cmd.targetPaths);
        DatabaseManager::instance().deleteNotesBatch(noteIds);

        AppEvent ev;
        ev.type = AppEventType::ItemsDeleted;
        ev.paths = cmd.targetPaths;
        CentralEventHub::instance().publishEvent(ev);
        break;
    }
    case AppCommandType::RecordAccess: {
        handleRecordAccess(cmd.targetPaths);
        break;
    }
    case AppCommandType::ToggleFavorite: {
        handleToggleFavorite(cmd.targetPaths);
        break;
    }
    default:
        return false;
    }

    return true;
}

QList<int> CoreEngine::extractNoteIds(const QStringList& paths) {
    QList<int> ids;
    ids.reserve(paths.size());
    static const QRegularExpression noteUriRegex(QStringLiteral(R"(^note://(\d+)$)"));
    for (const QString& p : paths) {
        QString s = p.trimmed();
        auto match = noteUriRegex.match(s);
        if (match.hasMatch()) {
            ids.append(match.captured(1).toInt());
        } else {
            bool ok = false;
            int directId = s.toInt(&ok);
            if (ok && directId > 0) {
                ids.append(directId);
            }
        }
    }
    return ids;
}

void CoreEngine::handleSetRating(const QStringList& paths, int rating) {
    LastOperationManager::instance().recordSetRating(rating);
    QList<int> noteIds = extractNoteIds(paths);
    DatabaseManager::instance().updateNoteStateBatch(noteIds, "rating", rating);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = paths;
    ev.payload["field"] = "rating";
    ev.payload["value"] = rating;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::handleSetColor(const QStringList& paths, const QString& color) {
    LastOperationManager::instance().recordSetColor(color);
    QList<int> noteIds = extractNoteIds(paths);
    // 写入 notes.color_label 列，彻底与分类背景色分离
    DatabaseManager::instance().updateNoteStateBatch(noteIds, "color_label", color);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = paths;
    ev.payload["field"] = "color";
    ev.payload["value"] = color;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::handleSetTags(const QStringList& paths, const QStringList& tags) {
    LastOperationManager::instance().recordPasteTags(tags);
    QStringList normalizedTags = TagCodec::normalize(tags);
    TagLexiconService::instance().addTags(normalizedTags);
    QString joinedTags = TagCodec::join(normalizedTags);

    QList<int> noteIds = extractNoteIds(paths);
    DatabaseManager::instance().updateNoteStateBatch(noteIds, "tags", joinedTags);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = paths;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::addTagsToPathsAsync(const QStringList& paths, const QStringList& tags, std::function<void(int pathCount)> onFinished, int existingJobId) {
    if (paths.isEmpty() || tags.isEmpty()) {
        if (existingJobId > 0) {
            TaskProgressService::instance().finishJob(existingJobId);
        }
        if (onFinished) {
            QMetaObject::invokeMethod(qApp, [onFinished, pathCount = paths.size()]() {
                onFinished(pathCount);
            }, Qt::QueuedConnection);
        }
        return;
    }

    m_asyncTagPool.start([paths, tags, onFinished, existingJobId]() {
        int totalPaths = paths.size();
        int jobId = existingJobId > 0 ? existingJobId : TaskProgressService::instance().createJob("正在应用标签...", totalPaths);

        QStringList normalizedTags = TagCodec::normalize(tags);
        TagLexiconService::instance().addTags(normalizedTags);

        QList<int> noteIds = extractNoteIds(paths);
        constexpr int kBatchSize = 100;
        int processed = 0;
        for (int i = 0; i < noteIds.size(); i += kBatchSize) {
            QList<int> batch = noteIds.mid(i, kBatchSize);
            DatabaseManager::instance().addTagsToNotesBatch(batch, normalizedTags);
            processed += batch.size();
            TaskProgressService::instance().updateProgress(jobId, processed, totalPaths, QString("已应用 %1 / %2 项").arg(processed).arg(totalPaths));
        }

        QMetaObject::invokeMethod(qApp, [paths, jobId, onFinished]() {
            TaskProgressService::instance().finishJob(jobId);

            AppEvent ev;
            ev.type = AppEventType::MetadataUpdated;
            ev.paths = paths;
            CentralEventHub::instance().publishEvent(ev);

            if (onFinished) {
                onFinished(paths.size());
            }
        }, Qt::QueuedConnection);
    });
}

void CoreEngine::handleSetNote(const QStringList& paths, const QString& note) {
    QList<int> noteIds = extractNoteIds(paths);
    DatabaseManager::instance().updateNoteStateBatch(noteIds, "remark", note);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = paths;
    ev.payload["field"] = "note";
    ev.payload["value"] = note;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::handleSetURL(const QStringList& paths, const QString& url) {
    // 纯文本笔记统一映射到 remark 补充信息或 content 特征中
    QList<int> noteIds = extractNoteIds(paths);
    DatabaseManager::instance().updateNoteStateBatch(noteIds, "remark", url);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = paths;
    ev.payload["field"] = "url";
    ev.payload["value"] = url;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::handleRecordAccess(const QStringList& paths) {
    QList<int> noteIds = extractNoteIds(paths);
    DatabaseManager::instance().recordAccessBatch(noteIds);
}

void CoreEngine::handleToggleFavorite(const QStringList& paths) {
    if (paths.isEmpty()) return;
    QList<int> noteIds = extractNoteIds(paths);
    DatabaseManager::instance().toggleNoteFavoriteBatch(noteIds);

    AppEvent ev;
    ev.type = AppEventType::FavoritesUpdated;
    ev.paths = paths;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::handleRenameItem(const QString& path, const QString& newTitle) {
    if (newTitle.isEmpty()) return;
    QList<int> noteIds = extractNoteIds({path});
    if (noteIds.isEmpty()) return;

    int id = noteIds.first();
    DatabaseManager::instance().updateNoteState(id, "title", newTitle);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = {path};
    ev.payload["field"] = "title";
    ev.payload["value"] = newTitle;
    CentralEventHub::instance().publishEvent(ev);
}

} // namespace RapidNotes
