#pragma once

#include <QString>
#include <QStringList>
#include <functional>

namespace QuarkMeta {

class PresetTagService {
public:
    static PresetTagService& instance();

    bool saveLibraryPresetTags(int categoryId, const QStringList& newTags, const QStringList& initialTags, std::function<void(int tagCount, int itemCount)> onTraceFinished = nullptr);

    int applyPresetTagsForDroppedItems(int categoryId, const QStringList& paths);

private:
    void processAsyncPresetTagsUpdate(int jobId, int categoryId, const QStringList& newTags, const QStringList& newlyAddedTags, std::function<void(int tagCount, int itemCount)> onTraceFinished);

private:
    PresetTagService() = default;
    ~PresetTagService() = default;
};

} // namespace QuarkMeta
