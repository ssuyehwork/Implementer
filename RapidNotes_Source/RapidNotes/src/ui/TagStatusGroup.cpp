#include "TagStatusGroup.h"
#include <QButtonGroup>

namespace QuarkMeta {

void TagStatusGroup::populate(QWidget* parentWidget,
                              QVBoxLayout* contentLayout,
                              FilterStateModel* filterModel,
                              const QuarkMeta::ScanStats& currentStats,
                              const FilterState& currentState,
                              AddFilterRowFunc addFilterRow) {
    if (!contentLayout || !filterModel || !addFilterRow) return;

    QButtonGroup* tagGroup = new QButtonGroup(parentWidget ? parentWidget : contentLayout->parentWidget());
    tagGroup->setExclusive(false);

    QCheckBox* cbYes = addFilterRow(contentLayout, "已标签", currentStats.hasTagCount, "tag:yes");
    if (currentState.tagPresence == FilterState::Yes) cbYes->setChecked(true);
    QObject::connect(cbYes, &QCheckBox::toggled, cbYes, [filterModel, tagGroup, cbYes](bool on) {
        FilterState st = filterModel->state();
        if (on) {
            for (QAbstractButton* b : tagGroup->buttons()) if (b != cbYes && b->isChecked()) b->setChecked(false);
            st.tagPresence = FilterState::Yes;
        } else st.tagPresence = FilterState::All;
        filterModel->setState(st);
    });
    tagGroup->addButton(cbYes);

    QCheckBox* cbNo = addFilterRow(contentLayout, "未标签", currentStats.noTagCount, "tag:no");
    if (currentState.tagPresence == FilterState::No) cbNo->setChecked(true);
    QObject::connect(cbNo, &QCheckBox::toggled, cbNo, [filterModel, tagGroup, cbNo](bool on) {
        FilterState st = filterModel->state();
        if (on) {
            for (QAbstractButton* b : tagGroup->buttons()) if (b != cbNo && b->isChecked()) b->setChecked(false);
            st.tagPresence = FilterState::No;
        } else st.tagPresence = FilterState::All;
        filterModel->setState(st);
    });
    tagGroup->addButton(cbNo);
}

} // namespace QuarkMeta
