#pragma once

#include <QObject>
#include <QList>
#include <QStringList>

namespace QuarkMeta {

struct FilterState {
    static constexpr int kHueTolerance = 20;

    QList<int>   ratings;
    QStringList  colors;          // 自动提取色彩过滤色 (Hex 字符串，如 #E24B4A)
    QStringList  manualColors;    // 手动标注颜色过滤 (红、橙、黄、绿、青、蓝、紫、灰、无色标)
    int          colorTolerance = 10; // 已废弃，由 colorPrecision 取代
    int          colorPrecision = 50; // 精准度 (0~100，默认 50)
    int          colorHue = 0;        // 色相 (0~359，默认 0)
    int          minColorArea = 0;    // 占比 (0~100，默认 0%)
    QString      keyword;
    QStringList  types;
    QStringList  createDates;
    QStringList  modifyDates;

    enum Presence { All, Yes, No };
    Presence linkPresence = All;
    Presence notePresence = All;
    Presence tagPresence = All;

    enum AspectRatio { AspectAny, Horizontal, Vertical, Square, Ratio169 };
    AspectRatio ratio = AspectAny;

    long long minSize = -1;
    long long maxSize = -1;

    QString typeFilterText;
    QString createDateFilterText;
    QString modifyDateFilterText;

    bool showHidden = false;

    enum DuplicatePresence { DupAll, DuplicateOnly, UniqueOnly };
    DuplicatePresence duplicatePresence = DupAll;

    enum ThumbnailPresence { ThumbAll, HasThumbnail, NoThumbnail };
    ThumbnailPresence thumbnailPresence = ThumbAll;

    bool isEmpty() const {
        return ratings.isEmpty() && colors.isEmpty() && manualColors.isEmpty() && keyword.isEmpty() && types.isEmpty() &&
               createDates.isEmpty() && modifyDates.isEmpty() &&
               linkPresence == All && notePresence == All && tagPresence == All && ratio == AspectAny &&
               minSize == -1 && maxSize == -1 &&
               typeFilterText.trimmed().isEmpty() && createDateFilterText.trimmed().isEmpty() &&
               modifyDateFilterText.trimmed().isEmpty() && duplicatePresence == DupAll &&
               thumbnailPresence == ThumbAll;
    }
};

class FilterStateModel : public QObject {
    Q_OBJECT
public:
    explicit FilterStateModel(QObject* parent = nullptr);
    ~FilterStateModel() override = default;

    const FilterState& state() const { return m_state; }
    void setState(const FilterState& state);
    void reset(bool force = false);

signals:
    void stateChanged(const FilterState& state);

private:
    FilterState m_state;
};

} // namespace QuarkMeta
