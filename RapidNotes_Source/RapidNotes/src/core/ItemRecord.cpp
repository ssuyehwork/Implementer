#include "ItemRecord.h"
#include "../meta/MetadataManager.h"
#include "../util/ColorPaletteEngine.h"
#include <QFileInfo>
#include <QDir>
#include <mutex>
#include <unordered_map>

namespace QuarkMeta {

void ItemRecord::fromMetadata(ItemRecord& r, const RuntimeMeta& meta) {
    r.rating = meta.rating;
    r.manualColor = QString::fromStdWString(meta.manualColor);
    r.autoColor = QString::fromStdWString(meta.autoColor);
    r.tags = meta.tags;
    r.pinned = meta.pinned;
    r.encrypted = meta.encrypted;
    r.url = QString::fromStdWString(meta.url);
    r.note = QString::fromStdWString(meta.note);
    r.sha256 = QString::fromStdString(meta.sha256);
    r.width = meta.width;
    r.height = meta.height;
    r.added_at = meta.added_at;
    r.thumbStatus = meta.thumbStatus;
    r.isManaged = meta.hasUserOperations();
    r.palettes.clear();
    for (const auto& pe : meta.palettes) {
        r.palettes.push_back({pe.color, pe.ratio});
    }

    if (!r.isDir && ColorPaletteEngine::isGraphicsFile(r.suffix.toLower())) {
        if (r.thumbStatus == 1) {
            r.thumbnailState = ThumbnailState::Failed;
        } else if (r.thumbnailState == ThumbnailState::NotApplicable || r.thumbnailState == ThumbnailState::Failed) {
            r.thumbnailState = ThumbnailState::Pending;
        }
    } else {
        r.thumbnailState = ThumbnailState::NotApplicable;
    }
}

ItemRecord ItemRecord::create(const QString& path, const RuntimeMeta* providedMeta) {
    ItemRecord r;
    QFileInfo info(path);

    QString nPath = QDir::toNativeSeparators(info.absoluteFilePath());
    std::wstring wPath = nPath.toStdWString();

    RuntimeMeta meta;
    if (providedMeta) {
        meta = *providedMeta;
    }

    long long size = 0, ctime = 0, mtime = 0, atime = 0;
    MetadataManager::fetchWinApiMetadataDirect(wPath, &size, nullptr, &ctime, &mtime, &atime);
    r.size = size;
    r.ctime = ctime;
    r.mtime = mtime;
    r.atime = atime;
    r.isDir = info.isDir();
    r.path = nPath;
    r.filename = info.fileName();
    r.isHidden = info.isHidden();

    if (r.isDir) {
        QDir sub(nPath);
        r.isEmpty = sub.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty();
        r.suffix = "";
    } else {
        r.suffix = info.suffix();
    }

    if (r.isDir || !ColorPaletteEngine::isGraphicsFile(r.suffix.toLower())) {
        r.thumbnailState = ThumbnailState::NotApplicable;
    } else if (r.thumbStatus == 1) {
        r.thumbnailState = ThumbnailState::Failed;
    } else {
        r.thumbnailState = ThumbnailState::Pending;
    }

    return r;
}

} // namespace QuarkMeta
