#include "ColorLabelGroup.h"
#include "StyleLibrary.h"

namespace QuarkMeta {

void ColorLabelGroup::populate(QWidget* parentWidget,
                               QVBoxLayout* contentLayout,
                               FilterStateModel* filterModel,
                               const QMap<QString, int>& colorCounts,
                               const FilterState& currentState,
                               AddFilterRowFunc addFilterRow) {
    if (!contentLayout || !filterModel || !addFilterRow) return;

    QWidget* contextObj = parentWidget ? parentWidget : contentLayout->parentWidget();
    const auto& colorsList = Style::getColorPalette();

    for (const auto& item : colorsList) {
        QString hexUpper = item.hex.toUpper();
        QString rowKey = QString("color:%1").arg(hexUpper);

        int cnt = colorCounts.value(item.hex, colorCounts.value(item.name, 0));
        bool isChecked = (currentState.manualColors.contains(item.name) ||
                          (!hexUpper.isEmpty() && currentState.manualColors.contains(hexUpper)) ||
                          (!item.hex.isEmpty() && currentState.manualColors.contains(item.hex)));

        if (cnt == 0 && !isChecked) {
            continue;
        }

        QCheckBox* cb = addFilterRow(contentLayout, item.name, cnt, item.color, rowKey);
        cb->setChecked(isChecked);
        QObject::connect(cb, &QCheckBox::checkStateChanged, contextObj, [filterModel, name = item.name, hexUpper](Qt::CheckState state) {
            FilterState st = filterModel->state();
            if (state == Qt::Checked) {
                if (!st.manualColors.contains(name)) st.manualColors.append(name);
            } else {
                st.manualColors.removeAll(name);
                if (!hexUpper.isEmpty()) {
                    st.manualColors.removeAll(hexUpper);
                }
            }
            filterModel->setState(st);
        });
    }
}

} // namespace QuarkMeta
