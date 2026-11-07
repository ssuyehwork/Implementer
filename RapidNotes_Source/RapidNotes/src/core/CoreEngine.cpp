#include "CoreEngine.h"
#include "LastOperationManager.h"
#include "../meta/MetadataManager.h"
#include "../meta/QuarkMetaJsonStore.h"
#include "../meta/FavoriteDao.h"
#include "../meta/FavoriteService.h"
#include "../ui/PanelMediator.h"
#include "../ui/ContentPanel.h"
#include "../ui/ToolTipOverlay.h"
#include "TagLexiconService.h"
#include "TaskProgressService.h"
#include <QCoreApplication>

namespace QuarkMeta {

CoreEngine& CoreEngine::instance() {
    static CoreEngine s_instance;
    return s_instance;
}

CoreEngine::CoreEngine(QObject* parent)
    : QObject(parent) {
    m_asyncTagPool.setMaxThreadCount(1);
    qRegisterMetaType<QuarkMeta::AppCommand>("QuarkMeta::AppCommand");
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
        tagsToApply.removeDuplicates();
        if (tagsToApply.isEmpty()) break;

        TagLexiconService::instance().addTags(tagsToApply);
        MetadataManager::instance().addTagsBatch(cmd.targetPaths, tagsToApply);

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
        tagsToRemove.removeDuplicates();
        if (tagsToRemove.isEmpty()) break;

        MetadataManager::instance().removeTagsBatch(cmd.targetPaths, tagsToRemove);

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

        // 🚀 仅作用于 inspiration.db 标签词库，解耦全盘扫描
        TagLexiconService::instance().renameTag(oldTag, newTag);

        AppEvent ev;
        ev.type = AppEventType::MetadataUpdated;
        CentralEventHub::instance().publishEvent(ev);
        break;
    }
    case AppCommandType::RemoveGlobalTag: {
        QString tag = cmd.params.value("tag").toString().trimmed();
        if (tag.isEmpty()) break;

        // 🚀 仅作用于 inspiration.db 标签词库，解耦全盘扫描
        TagLexiconService::instance().deleteTag(tag);

        AppEvent ev;
        ev.type = AppEventType::MetadataUpdated;
        CentralEventHub::instance().publishEvent(ev);
        break;
    }
    case AppCommandType::SetPinned: {
        bool pinned = cmd.params.value("pinned", false).toBool();
        for (const QString& path : cmd.targetPaths) {
            MetadataManager::instance().setPinned(path.toStdWString(), pinned, true);
        }
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
    case AppCommandType::DeletePermanently: {
        for (const QString& path : cmd.targetPaths) {
            MetadataManager::instance().removeMetadataSync(path.toStdWString());
        }
        MetadataManager::instance().notifyUI(MetadataManager::RefreshLevel::FullRebuild);
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

void CoreEngine::handleSetRating(const QStringList& paths, int rating) {
    LastOperationManager::instance().recordSetRating(rating);
    MetadataManager::instance().setRatingBatch(paths, rating);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = paths;
    ev.payload["field"] = "rating";
    ev.payload["value"] = rating;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::handleSetColor(const QStringList& paths, const QString& color) {
    LastOperationManager::instance().recordSetColor(color);
    MetadataManager::instance().setColorBatch(paths, color);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = paths;
    ev.payload["field"] = "color";
    ev.payload["value"] = color;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::handleSetTags(const QStringList& paths, const QStringList& tags) {
    LastOperationManager::instance().recordPasteTags(tags);
    TagLexiconService::instance().addTags(tags);
    MetadataManager::instance().addTagsBatch(paths, tags);

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
        int jobId = existingJobId > 0 ? existingJobId : TaskProgressService::instance().createJob("正在应用预设标签...", totalPaths);

        TagLexiconService::instance().addTags(tags);

        constexpr int kBatchSize = 100;
        int processed = 0;
        for (int i = 0; i < totalPaths; i += kBatchSize) {
            QStringList batch = paths.mid(i, kBatchSize);
            MetadataManager::instance().addTagsBatch(batch, tags);
            processed += batch.size();
            TaskProgressService::instance().updateProgress(jobId, processed, totalPaths, QString("已应用 %1 / %2 项").arg(processed).arg(totalPaths));
        }

        QMetaObject::invokeMethod(qApp, [paths, jobId, onFinished]() {
            TaskProgressService::instance().updateProgress(jobId, paths.size(), paths.size(), "正在刷新界面...");

            AppEvent ev;
            ev.type = AppEventType::MetadataUpdated;
            ev.paths = paths;
            CentralEventHub::instance().publishEvent(ev);

            ContentPanel* activePanel = nullptr;
            if (PanelMediator* mediator = PanelMediator::instance()) {
                activePanel = mediator->activeContentPanel();
            }

            auto proceedToSave = [jobId, paths, onFinished]() {
                TaskProgressService::instance().updateProgress(jobId, paths.size(), paths.size(), "正在保存...");
                QuarkMetaJsonStore::instance().flushAllDirtyBuffersAsync([jobId, paths, onFinished](bool allOk) {
                    TaskProgressService::instance().finishJob(jobId);
                    if (!allOk) {
                        ToolTipOverlay::instance()->showToast("部分元数据保存失败", 2000, QColor("#e81123"));
                    }
                    if (onFinished) {
                        onFinished(paths.size());
                    }
                });
            };

            if (activePanel) {
                activePanel->runWhenStatsSettled(proceedToSave);
            } else {
                proceedToSave();
            }
        }, Qt::QueuedConnection);
    });
}

void CoreEngine::handleSetNote(const QStringList& paths, const QString& note) {
    MetadataManager::instance().setNoteBatch(paths, note);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = paths;
    ev.payload["field"] = "note";
    ev.payload["value"] = note;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::handleSetURL(const QStringList& paths, const QString& url) {
    MetadataManager::instance().setURLBatch(paths, url);

    AppEvent ev;
    ev.type = AppEventType::MetadataUpdated;
    ev.paths = paths;
    ev.payload["field"] = "url";
    ev.payload["value"] = url;
    CentralEventHub::instance().publishEvent(ev);
}

void CoreEngine::handleRecordAccess(const QStringList& paths) {
    for (const QString& path : paths) {
        MetadataManager::instance().recordAccess(path.toStdWString());
    }
}

void CoreEngine::handleToggleFavorite(const QStringList& paths) {
    if (paths.isEmpty()) return;

    bool allFav = true;
    for (const QString& p : paths) {
        if (!FavoriteService::instance().isFavorite(p)) {
            allFav = false;
            break;
        }
    }

    for (const QString& p : paths) {
        FavoriteService::instance().toggleFavorite(p);
    }

    AppEvent ev;
    ev.type = AppEventType::FavoritesUpdated;
    ev.paths = paths;
    ev.payload["isFavorite"] = !allFav;
    CentralEventHub::instance().publishEvent(ev);
}

} // namespace QuarkMeta
