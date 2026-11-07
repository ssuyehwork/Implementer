#include "NoteStatusGroup.h"
#include <QButtonGroup>

namespace QuarkMeta {

void NoteStatusGroup::populate(QWidget* parentWidget,
                               QVBoxLayout* contentLayout,
                               FilterStateModel* filterModel,
                               const QuarkMeta::ScanStats& currentStats,
                               const FilterState& currentState,
                               AddFilterRowFunc addFilterRow) {
    if (!contentLayout || !filterModel || !addFilterRow) return;

    QButtonGroup* noteGroup = new QButtonGroup(parentWidget ? parentWidget : contentLayout->parentWidget());
    noteGroup->setExclusive(false);

    QCheckBox* cbYes = addFilterRow(contentLayout, "有备注", currentStats.hasNoteCount, "note:yes");
    if (currentState.notePresence == FilterState::Yes) cbYes->setChecked(true);
    QObject::connect(cbYes, &QCheckBox::toggled, cbYes, [filterModel, noteGroup, cbYes](bool on) {
        FilterState st = filterModel->state();
        if (on) {
            for (QAbstractButton* b : noteGroup->buttons()) if (b != cbYes && b->isChecked()) b->setChecked(false);
            st.notePresence = FilterState::Yes;
        } else st.notePresence = FilterState::All;
        filterModel->setState(st);
    });
    noteGroup->addButton(cbYes);

    QCheckBox* cbNo = addFilterRow(contentLayout, "无备注", currentStats.noNoteCount, "note:no");
    if (currentState.notePresence == FilterState::No) cbNo->setChecked(true);
    QObject::connect(cbNo, &QCheckBox::toggled, cbNo, [filterModel, noteGroup, cbNo](bool on) {
        FilterState st = filterModel->state();
        if (on) {
            for (QAbstractButton* b : noteGroup->buttons()) if (b != cbNo && b->isChecked()) b->setChecked(false);
            st.notePresence = FilterState::No;
        } else st.notePresence = FilterState::All;
        filterModel->setState(st);
    });
    noteGroup->addButton(cbNo);
}

} // namespace QuarkMeta
