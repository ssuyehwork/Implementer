#include "LinkStatusGroup.h"
#include <QButtonGroup>

namespace QuarkMeta {

void LinkStatusGroup::populate(QWidget* parentWidget,
                               QVBoxLayout* contentLayout,
                               FilterStateModel* filterModel,
                               const QuarkMeta::ScanStats& currentStats,
                               const FilterState& currentState,
                               AddFilterRowFunc addFilterRow) {
    if (!contentLayout || !filterModel || !addFilterRow) return;

    QButtonGroup* linkGroup = new QButtonGroup(parentWidget ? parentWidget : contentLayout->parentWidget());
    linkGroup->setExclusive(false);

    QCheckBox* cbYes = addFilterRow(contentLayout, "有链接", currentStats.hasLinkCount, "link:yes");
    if (currentState.linkPresence == FilterState::Yes) cbYes->setChecked(true);
    QObject::connect(cbYes, &QCheckBox::toggled, cbYes, [filterModel, linkGroup, cbYes](bool on) {
        FilterState st = filterModel->state();
        if (on) {
            for (QAbstractButton* b : linkGroup->buttons()) if (b != cbYes && b->isChecked()) b->setChecked(false);
            st.linkPresence = FilterState::Yes;
        } else st.linkPresence = FilterState::All;
        filterModel->setState(st);
    });
    linkGroup->addButton(cbYes);

    QCheckBox* cbNo = addFilterRow(contentLayout, "无链接", currentStats.noLinkCount, "link:no");
    if (currentState.linkPresence == FilterState::No) cbNo->setChecked(true);
    QObject::connect(cbNo, &QCheckBox::toggled, cbNo, [filterModel, linkGroup, cbNo](bool on) {
        FilterState st = filterModel->state();
        if (on) {
            for (QAbstractButton* b : linkGroup->buttons()) if (b != cbNo && b->isChecked()) b->setChecked(false);
            st.linkPresence = FilterState::No;
        } else st.linkPresence = FilterState::All;
        filterModel->setState(st);
    });
    linkGroup->addButton(cbNo);
}

} // namespace QuarkMeta
