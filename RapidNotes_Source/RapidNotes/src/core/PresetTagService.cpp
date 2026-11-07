#include "PresetTagService.h"
#include "../meta/LibraryDao.h"
#include "CoreEngine.h"
#include "TaskProgressService.h"
#include <QSet>
#include <QtConcurrent>
#include <QApplication>

namespace QuarkMeta {

PresetTagService& PresetTagService::instance() {
    static PresetTagService s_instance;
    return s_instance;
}

bool PresetTagService::saveLibraryPresetTags(int categoryId, const QStringList& newTags, const QStringList& initialTags, std::function<void(int tagCount, int itemCount)> onTraceFinished) {
    // 1. 计算相对 initialTags 新增的标签 (忽略大小写比较)
    QSet<QString> initialSetLower;
    for (const QString& tag : initialTags) {
        initialSetLower.insert(tag.toLower());
    }

    QStringList newlyAddedTags;
    for (const QString& tag : newTags) {
        if (!initialSetLower.contains(tag.toLower())) {
            newlyAddedTags.append(tag);
        }
    }

    // 2. 🚀【0ms 即时唤起进度条】：主线程被调用瞬间立即创建进度条 Job，保证视觉反馈零延迟
    int jobId = TaskProgressService::instance().createJob("正在应用预设标签...", 0);

    // 3. 异步投递到后台工作线程池，主线程立即返回 0 毫秒无卡顿
    (void)QtConcurrent::run([this, jobId, categoryId, newTags, newlyAddedTags, onTraceFinished]() {
        processAsyncPresetTagsUpdate(jobId, categoryId, newTags, newlyAddedTags, onTraceFinished);
    });

    return true;
}

void PresetTagService::processAsyncPresetTagsUpdate(int jobId, int categoryId, const QStringList& newTags, const QStringList& newlyAddedTags, std::function<void(int tagCount, int itemCount)> onTraceFinished) {
    // A. 后台写 SQLite 数据库
    if (!LibraryDao::updatePresetTags(categoryId, newTags)) {
        TaskProgressService::instance().finishJob(jobId);
        return;
    }

    if (newlyAddedTags.isEmpty()) {
        TaskProgressService::instance().finishJob(jobId);
        if (onTraceFinished) {
            QMetaObject::invokeMethod(qApp, [onTraceFinished]() {
                onTraceFinished(0, 0);
            }, Qt::QueuedConnection);
        }
        return;
    }

    // B. 后台线程高效检索分类下关联的所有文件路径
    QStringList paths = LibraryDao::getCategoryPaths(categoryId, false);
    paths.removeDuplicates();
    if (paths.isEmpty()) {
        TaskProgressService::instance().finishJob(jobId);
        if (onTraceFinished) {
            int newlyAddedCount = newlyAddedTags.size();
            QMetaObject::invokeMethod(qApp, [onTraceFinished, newlyAddedCount]() {
                onTraceFinished(newlyAddedCount, 0);
            }, Qt::QueuedConnection);
        }
        return;
    }

    // C. 定向批处理：在后台按物理父文件夹归类，复用现有 jobId 推进 TaskProgressStrip
    int newlyAddedCount = newlyAddedTags.size();
    CoreEngine::instance().addTagsToPathsAsync(paths, newlyAddedTags, [onTraceFinished, newlyAddedCount](int pathCount) {
        if (onTraceFinished) {
            onTraceFinished(newlyAddedCount, pathCount);
        }
    }, jobId);
}

int PresetTagService::applyPresetTagsForDroppedItems(int categoryId, const QStringList& paths) {
    if (paths.isEmpty()) {
        return 0;
    }

    QStringList tags = LibraryDao::getPresetTagsWithAncestors(categoryId);
    if (tags.isEmpty()) {
        return 0;
    }

    CoreEngine::instance().addTagsToPathsAsync(paths, tags);
    return tags.size();
}

} // namespace QuarkMeta
