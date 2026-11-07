#include "DuplicateStatusGroup.h"
#include <QButtonGroup>
#include <QList>
#include <tuple>

namespace QuarkMeta {

void DuplicateStatusGroup::populate(QVBoxLayout* contentLayout,
                                    FilterStateModel* filterModel,
                                    const QuarkMeta::ScanStats& currentStats,
                                    const FilterState& currentState,
                                    AddFilterRowFunc addFilterRow) {
    if (!contentLayout || !filterModel || !addFilterRow) return;

    QButtonGroup* dupGroup = new QButtonGroup(contentLayout->parentWidget());
    dupGroup->setExclusive(false);

    const QList<std::tuple<FilterState::DuplicatePresence, QString, int, QString>> dupItems = {
        {FilterState::DuplicateOnly, "重复项", currentStats.duplicateCount, "dup:only"},
        {FilterState::UniqueOnly, "未重复", currentStats.uniqueCount, "dup:unique"}
    };
    for (const auto& [presence, label, count, rowKey] : dupItems) {
        QCheckBox* cb = addFilterRow(contentLayout, label, count, rowKey);
        if (currentState.duplicatePresence == presence) cb->setChecked(true);
        QObject::connect(cb, &QCheckBox::toggled, cb, [filterModel, presence, dupGroup, cb](bool on) {
            FilterState st = filterModel->state();
            if (on) {
                for (QAbstractButton* b : dupGroup->buttons()) if (b != cb && b->isChecked()) b->setChecked(false);
                st.duplicatePresence = presence;
            } else st.duplicatePresence = FilterState::DupAll;
            filterModel->setState(st);
        });
        dupGroup->addButton(cb);
    }
}

} // namespace QuarkMeta
