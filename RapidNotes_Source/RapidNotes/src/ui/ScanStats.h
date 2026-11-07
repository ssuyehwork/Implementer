#pragma once

#include <QMap>
#include <QString>
#include <QSet>

namespace QuarkMeta {

/**
 * @brief 全局统一的检索与统计快照结构体
 */
struct ScanStats {
    QMap<int, int> ratingCounts;
    QMap<QString, int> colorCounts;
    QMap<QString, int> typeCounts;
    QMap<QString, int> createDateCounts;
    QMap<QString, int> modifyDateCounts;
    int emptyFolderCount = 0;

    int hasLinkCount = 0;
    int noLinkCount = 0;
    int hasNoteCount = 0;
    int noNoteCount = 0;
    int hasTagCount = 0;
    int noTagCount = 0;
    int ratioHorizontalCount = 0;
    int ratioVerticalCount = 0;
    int ratioSquareCount = 0;
    int ratio169Count = 0;
    int duplicateCount = 0;
    int uniqueCount = 0;
    int noThumbnailCount = 0;
    int hasThumbnailCount = 0;

    QSet<QString> duplicatePaths;

    bool operator==(const ScanStats& o) const {
        return ratingCounts == o.ratingCounts &&
               colorCounts == o.colorCounts &&
               typeCounts == o.typeCounts &&
               createDateCounts == o.createDateCounts &&
               modifyDateCounts == o.modifyDateCounts &&
               emptyFolderCount == o.emptyFolderCount &&
               hasLinkCount == o.hasLinkCount &&
               noLinkCount == o.noLinkCount &&
               hasNoteCount == o.hasNoteCount &&
               noNoteCount == o.noNoteCount &&
               hasTagCount == o.hasTagCount &&
               noTagCount == o.noTagCount &&
               ratioHorizontalCount == o.ratioHorizontalCount &&
               ratioVerticalCount == o.ratioVerticalCount &&
               ratioSquareCount == o.ratioSquareCount &&
               ratio169Count == o.ratio169Count &&
               duplicateCount == o.duplicateCount &&
               uniqueCount == o.uniqueCount &&
               noThumbnailCount == o.noThumbnailCount &&
               hasThumbnailCount == o.hasThumbnailCount &&
               duplicatePaths == o.duplicatePaths;
    }

    bool operator!=(const ScanStats& o) const { return !(*this == o); }
};

} // namespace QuarkMeta
