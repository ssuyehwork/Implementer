#pragma once
#include <QStringList>
#include <functional>

namespace RapidNotes {
class DeepThumbnailExtractor {
public:
    static DeepThumbnailExtractor& instance() {
        static DeepThumbnailExtractor s_inst;
        return s_inst;
    }
    void extractBatchAsync(const QStringList& paths,
                           std::function<void(const QString&, bool)> onSingleFinished = nullptr,
                           std::function<void(int, int)> onAllFinished = nullptr) {
        if (onAllFinished) onAllFinished(paths.size(), paths.size());
    }
};
}
