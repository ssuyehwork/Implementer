#include "ItemMetaMapper.h"

namespace QuarkMeta {

void ItemMetaMapper::apply(const ItemMeta& meta, ItemRecord& record) {
    if (!meta.fileId.empty()) {
        record.fileId = QString::fromStdString(meta.fileId);
    }
    record.rating = meta.rating;
    record.manualColor = QString::fromStdWString(meta.color);
    record.pinned = meta.pinned;
    record.note = QString::fromStdWString(meta.note);
    record.url = QString::fromStdWString(meta.url);
    record.encrypted = meta.encrypted;

    record.tags.clear();
    for (const auto& t : meta.tags) {
        record.tags.append(QString::fromStdWString(t));
    }

    record.width = meta.width;
    record.height = meta.height;
    record.autoColor = QString::fromStdWString(meta.autoColor);
    record.added_at = meta.addedAt;

    record.palettes.clear();
    for (const auto& pe : meta.palettes) {
        record.palettes.push_back({pe.color, pe.ratio});
    }
}

} // namespace QuarkMeta
