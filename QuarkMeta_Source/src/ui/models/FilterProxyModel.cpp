#include "FilterProxyModel.h"
#include "ItemModelBase.h"
#include "../UiHelper.h"
#include <QDateTime>
#include <QRegularExpression>
#include <cmath>

namespace QuarkMeta {

FilterProxyModel::FilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {}

void FilterProxyModel::setSortCriteria(int type, Qt::SortOrder order) {
    bool typeChanged = (m_sortType != type);
    m_sortType = type;

    if (sortColumn() != 0 || sortOrder() != order) {
        sort(0, order);
    } else if (typeChanged) {
        invalidate();
    }
}

void FilterProxyModel::updateFilter() {
    invalidate();
}

void FilterProxyModel::setCachedDuplicatePaths(const QSet<QString>& paths) {
    if (m_cachedDuplicatePaths == paths) return;
    m_cachedDuplicatePaths = paths;
    updateFilter();
}

bool FilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    Q_UNUSED(sourceParent);
    const auto* sourceModelPtr = qobject_cast<const ItemModelBase*>(sourceModel());
    if (!sourceModelPtr) return true;

    const auto& records = sourceModelPtr->allRecords();
    if (sourceRow < 0 || sourceRow >= static_cast<int>(records.size())) return false;
    const auto& record = records[sourceRow];

    // 0. 隐藏属性过滤
    if (record.isHidden && !currentFilter.showHidden) {
        return false;
    }

    // 2. 评级过滤
    if (!currentFilter.ratings.isEmpty()) {
        if (!currentFilter.ratings.contains(record.rating)) return false;
    }

    // 3. 🚨 物理隔离：手动标注颜色过滤 (只针对 record.manualColor)
    if (!currentFilter.manualColors.isEmpty()) {
        bool matchManual = false;
        static const QMap<QString, QString> s_manualHexMap = {
            {"红色", "#E24B4A"}, {"橙色", "#EF9F27"}, {"黄色", "#FECF0E"},
            {"绿色", "#639922"}, {"青色", "#1D9E75"}, {"蓝色", "#378ADD"},
            {"紫色", "#7F77DD"}, {"灰色", "#5F5E5A"}
        };

        for (const QString& mc : currentFilter.manualColors) {
            if (mc == "无色标" || mc.isEmpty()) {
                if (record.manualColor.isEmpty()) {
                    matchManual = true;
                    break;
                }
            } else {
                QString targetHex = s_manualHexMap.value(mc, mc);
                if (record.manualColor.compare(targetHex, Qt::CaseInsensitive) == 0 ||
                    record.manualColor.contains(mc, Qt::CaseInsensitive)) {
                    matchManual = true;
                    break;
                }
            }
        }
        if (!matchManual) return false;
    }

    // 3.5 🚨 物理隔离：自动提取色彩过滤 (色相 + 精准度纯度要求 + 灰黑白明度匹配)
    if (!currentFilter.colors.isEmpty()) {
        bool matchAuto = false;

        for (const QString& fc : currentFilter.colors) {
            QColor targetCol = UiHelper::parseColorName(fc);
            float area = ColorPaletteEngine::matchedAreaRatio(targetCol, record.palettes, record.autoColor, currentFilter.colorPrecision);
            if (area > 0.0f && (area * 100.0f >= static_cast<float>(currentFilter.minColorArea))) {
                matchAuto = true;
                break;
            }
        }
        if (!matchAuto) return false;
    }

    // 4. 类型过滤
    if (!currentFilter.types.isEmpty() || !currentFilter.typeFilterText.isEmpty()) {
        QString type = record.isDir ? "folder" : "file";
        QString ext = record.suffix.toUpper();
        bool matchType = false;

        if (!currentFilter.typeFilterText.isEmpty()) {
            QString searchText = currentFilter.typeFilterText.trimmed();
            if (searchText == "文件夹" || searchText.toLower() == "folder") {
                if (type == "folder") matchType = true;
            } else if (searchText == "空文件夹") {
                if (type == "folder" && record.isEmpty) matchType = true;
            } else {
                if (ext.contains(searchText.toUpper())) matchType = true;
            }
            if (!matchType) return false;
        }

        if (!currentFilter.types.isEmpty()) {
            matchType = false;
            for (const QString& fType : currentFilter.types) {
                if (fType == "folder") {
                    if (type == "folder") { matchType = true; break; }
                } else if (fType == "file") {
                    if (type != "folder") { matchType = true; break; }
                } else if (fType == "空文件夹") {
                    if (type == "folder" && record.isEmpty) { matchType = true; break; }
                } else {
                    if (ext == fType.toUpper()) { matchType = true; break; }
                }
            }
            if (!matchType) return false;
        }
    }

    // 5. 日期过滤
    if (!currentFilter.createDates.isEmpty() || !currentFilter.createDateFilterText.isEmpty()) {
        QString dStr = QDateTime::fromMSecsSinceEpoch(record.ctime).date().toString("dd-MM-yyyy");
        if (!currentFilter.createDateFilterText.isEmpty() && !dStr.contains(currentFilter.createDateFilterText.trimmed())) {
            return false;
        }
        if (!currentFilter.createDates.isEmpty() && !currentFilter.createDates.contains(dStr)) {
            return false;
        }
    }

    if (!currentFilter.modifyDates.isEmpty() || !currentFilter.modifyDateFilterText.isEmpty()) {
        QString dStr = QDateTime::fromMSecsSinceEpoch(record.mtime).date().toString("dd-MM-yyyy");
        if (!currentFilter.modifyDateFilterText.isEmpty() && !dStr.contains(currentFilter.modifyDateFilterText.trimmed())) {
            return false;
        }
        if (!currentFilter.modifyDates.isEmpty() && !currentFilter.modifyDates.contains(dStr)) {
            return false;
        }
    }

    // 6. 附加属性过滤 (链接、备注、标签、尺寸、判重)
    if (currentFilter.linkPresence != FilterState::All) {
        bool hasLink = !record.url.isEmpty();
        if (currentFilter.linkPresence == FilterState::Yes && !hasLink) return false;
        if (currentFilter.linkPresence == FilterState::No && hasLink) return false;
    }

    if (currentFilter.notePresence != FilterState::All) {
        bool hasNote = !record.note.isEmpty();
        if (currentFilter.notePresence == FilterState::Yes && !hasNote) return false;
        if (currentFilter.notePresence == FilterState::No && hasNote) return false;
    }

    if (currentFilter.tagPresence != FilterState::All) {
        bool hasTags = !record.tags.isEmpty();
        if (currentFilter.tagPresence == FilterState::Yes && !hasTags) return false;
        if (currentFilter.tagPresence == FilterState::No && hasTags) return false;
    }

    if (currentFilter.minSize != -1 && record.size < currentFilter.minSize) return false;
    if (currentFilter.maxSize != -1 && record.size > currentFilter.maxSize) return false;

    if (currentFilter.ratio != FilterState::AspectAny) {
        if (record.width > 0 && record.height > 0) {
            double r = static_cast<double>(record.width) / record.height;
            if (currentFilter.ratio == FilterState::Horizontal && record.width <= record.height) return false;
            if (currentFilter.ratio == FilterState::Vertical && record.height <= record.width) return false;
            if (currentFilter.ratio == FilterState::Square && std::abs(r - 1.0) > 0.05) return false;
            if (currentFilter.ratio == FilterState::Ratio169 && std::abs(r - 1.77) > 0.05) return false;
        } else {
            return false;
        }
    }

    if (currentFilter.duplicatePresence != FilterState::DupAll) {
        if (record.isDir) return false;
        bool isDuplicate = m_cachedDuplicatePaths.contains(record.path);
        if (currentFilter.duplicatePresence == FilterState::DuplicateOnly && !isDuplicate) return false;
        if (currentFilter.duplicatePresence == FilterState::UniqueOnly && isDuplicate) return false;
    }

    // 6.5 缩略图状态过滤 (统一使用 ItemRecord::thumbnailState)
    if (currentFilter.thumbnailPresence != FilterState::ThumbAll) {
        if (record.isDir || !ColorPaletteEngine::isGraphicsFile(record.suffix.toLower())) {
            return false;
        }

        bool isReady = (record.thumbnailState == ItemRecord::ThumbnailState::Ready);
        if (currentFilter.thumbnailPresence == FilterState::HasThumbnail && !isReady) return false;
        if (currentFilter.thumbnailPresence == FilterState::NoThumbnail && isReady) return false;
    }

    // 7. 搜索关键词匹配 (支持分行、顿号、逗号切割的多项目 OR 匹配)
    if (!currentFilter.keyword.isEmpty()) {
        static const QRegularExpression sepRegex("[\r\n、,，]+");
        QStringList keywords = currentFilter.keyword.split(sepRegex, Qt::SkipEmptyParts);

        if (!keywords.isEmpty()) {
            bool anyMatch = false;
            for (const QString& rawKw : keywords) {
                QString kw = rawKw.trimmed();
                if (kw.isEmpty()) continue;

                bool match = record.filename.contains(kw, Qt::CaseInsensitive);

                if (!match) {
                    for (const QString& tag : record.tags) {
                        if (tag.contains(kw, Qt::CaseInsensitive)) {
                            match = true;
                            break;
                        }
                    }
                }

                if (!match && !record.note.isEmpty()) {
                    if (record.note.contains(kw, Qt::CaseInsensitive)) {
                        match = true;
                    }
                }

                if (match) {
                    anyMatch = true;
                    break;
                }
            }

            if (!anyMatch) return false;
        }
    }

    return true;
}

bool FilterProxyModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const {
    const auto* sourceModelPtr = qobject_cast<const ItemModelBase*>(sourceModel());
    if (!sourceModelPtr) return QSortFilterProxyModel::lessThan(source_left, source_right);

    const auto& records = sourceModelPtr->allRecords();
    int leftRow = source_left.row();
    int rightRow = source_right.row();
    if (leftRow < 0 || leftRow >= static_cast<int>(records.size()) || 
        rightRow < 0 || rightRow >= static_cast<int>(records.size())) {
        return QSortFilterProxyModel::lessThan(source_left, source_right);
    }

    const auto& leftRec = records[leftRow];
    const auto& rightRec = records[rightRow];

    // 🚀【强制优先级 1：文件夹优先】：文件夹在分区最顶部，无视升降序反转（不跨区）
    if (leftRec.isDir != rightRec.isDir) {
        return (sortOrder() == Qt::AscendingOrder) == leftRec.isDir;
    }

    // 🚀【强制优先级 2：置顶优先】：在各自分区内（文件夹区/文件区），置顶项永远在最顶部，无视升降序反转
    if (leftRec.pinned != rightRec.pinned) {
        return (sortOrder() == Qt::AscendingOrder) == leftRec.pinned;
    }

    auto compareNames = [](const ItemRecord& l, const ItemRecord& r) {
        return l.filename.localeAwareCompare(r.filename) < 0;
    };

    // 0: SortByName, 1: SortByCreateDate, 2: SortByModifyDate, 3: SortByExtension, 4: SortBySize, 5: SortByDimension, 6: SortByRating, 7: SortByAddedDate
    switch (m_sortType) {
        case 0: return compareNames(leftRec, rightRec);
        case 1:
            if (leftRec.ctime != rightRec.ctime) return leftRec.ctime < rightRec.ctime;
            return compareNames(leftRec, rightRec);
        case 2:
            if (leftRec.mtime != rightRec.mtime) return leftRec.mtime < rightRec.mtime;
            return compareNames(leftRec, rightRec);
        case 3: {
            int comp = leftRec.suffix.localeAwareCompare(rightRec.suffix);
            if (comp != 0) return comp < 0;
            return compareNames(leftRec, rightRec);
        }
        case 4: {
            long long lSize = leftRec.isDir ? -1 : leftRec.size;
            long long rSize = rightRec.isDir ? -1 : rightRec.size;
            if (lSize != rSize) return lSize < rSize;
            return compareNames(leftRec, rightRec);
        }
        case 5: {
            long long lDim = static_cast<long long>(leftRec.width) * leftRec.height;
            long long rDim = static_cast<long long>(rightRec.width) * rightRec.height;
            if (lDim != rDim) return lDim < rDim;
            return compareNames(leftRec, rightRec);
        }
        case 6:
            if (leftRec.rating != rightRec.rating) return leftRec.rating < rightRec.rating;
            return compareNames(leftRec, rightRec);
        case 7: {
            long long leftAdded = leftRec.added_at == 0 ? leftRec.ctime : leftRec.added_at;
            long long rightAdded = rightRec.added_at == 0 ? rightRec.ctime : rightRec.added_at;
            if (leftAdded != rightAdded) return leftAdded < rightAdded;
            return compareNames(leftRec, rightRec);
        }
        default:
            return compareNames(leftRec, rightRec);
    }
}

} // namespace QuarkMeta