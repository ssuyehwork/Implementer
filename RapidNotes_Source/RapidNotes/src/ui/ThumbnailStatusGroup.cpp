#include "ThumbnailStatusGroup.h"
#include <QButtonGroup>

namespace QuarkMeta {

void ThumbnailStatusGroup::populate(QVBoxLayout* contentLayout,
                                   FilterStateModel* filterModel,
                                   const QuarkMeta::ScanStats& currentStats,
                                   const FilterState& currentState,
                                   AddFilterRowFunc addFilterRow) {
    if (!contentLayout || !filterModel || !addFilterRow) return;

    QButtonGroup* thumbGroup = new QButtonGroup(contentLayout->parentWidget());
    thumbGroup->setExclusive(false);

    QCheckBox* cbYes = addFilterRow(contentLayout, "有缩略图", currentStats.hasThumbnailCount, "thumb:has");
    if (currentState.thumbnailPresence == FilterState::HasThumbnail) cbYes->setChecked(true);
    QObject::connect(cbYes, &QCheckBox::toggled, cbYes, [filterModel, thumbGroup, cbYes](bool on) {
        FilterState st = filterModel->state();
        if (on) {
            for (QAbstractButton* b : thumbGroup->buttons()) if (b != cbYes && b->isChecked()) b->setChecked(false);
            st.thumbnailPresence = FilterState::HasThumbnail;
        } else st.thumbnailPresence = FilterState::ThumbAll;
        filterModel->setState(st);
    });
    thumbGroup->addButton(cbYes);

    QCheckBox* cbNo = addFilterRow(contentLayout, "无缩略图 (提取失败)", currentStats.noThumbnailCount, "thumb:none");
    if (currentState.thumbnailPresence == FilterState::NoThumbnail) cbNo->setChecked(true);
    QObject::connect(cbNo, &QCheckBox::toggled, cbNo, [filterModel, thumbGroup, cbNo](bool on) {
        FilterState st = filterModel->state();
        if (on) {
            for (QAbstractButton* b : thumbGroup->buttons()) if (b != cbNo && b->isChecked()) b->setChecked(false);
            st.thumbnailPresence = FilterState::NoThumbnail;
        } else st.thumbnailPresence = FilterState::ThumbAll;
        filterModel->setState(st);
    });
    thumbGroup->addButton(cbNo);
}

} // namespace QuarkMeta
