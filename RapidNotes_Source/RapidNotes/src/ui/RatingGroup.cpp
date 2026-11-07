#include "RatingGroup.h"
#include "components/ClickableRow.h"
#include "UiHelper.h"
#include <QLabel>
#include <QPainter>
#include <QPixmap>

namespace QuarkMeta {

static QString ratingDisplayName(int r) {
    return r == 0 ? "无评级" : QString("★").repeated(r);
}

void RatingGroup::populate(QWidget* parentWidget,
                           QVBoxLayout* contentLayout,
                           FilterStateModel* filterModel,
                           const QMap<int, int>& ratingCounts,
                           const FilterState& currentState,
                           AddFilterRowFunc addFilterRow) {
    if (!contentLayout || !filterModel || !addFilterRow) return;

    QWidget* contextObj = parentWidget ? parentWidget : contentLayout->parentWidget();

    for (int r : {0, 1, 2, 3, 4, 5}) {
        int cnt = ratingCounts.value(r, 0);
        bool isChecked = currentState.ratings.contains(r);
        if (cnt <= 0 && !isChecked) continue;

        QCheckBox* cb = addFilterRow(contentLayout, ratingDisplayName(r), cnt);
        cb->blockSignals(true);
        cb->setChecked(isChecked);
        cb->blockSignals(false);

        ClickableRow* row = qobject_cast<ClickableRow*>(cb->parentWidget());
        if (row) {
            row->setProperty("ratingValue", r);
            if (r > 0) {
                QLabel* lbl = row->findChild<QLabel*>("FilterItemLabel");
                if (lbl) {
                    int starSize = 12;
                    int spacing = 2;
                    int totalW = r * starSize + (r - 1) * spacing;
                    QPixmap pix(totalW, starSize);
                    pix.fill(Qt::transparent);
                    QPainter painter(&pix);
                    QPixmap starPix = UiHelper::getIcon("star_filled", QColor("#CCCCCC"), starSize).pixmap(starSize, starSize);
                    for (int i = 0; i < r; ++i) {
                        painter.drawPixmap(i * (starSize + spacing), 0, starPix);
                    }
                    painter.end();
                    lbl->setPixmap(pix);
                }
            }
        }

        QObject::connect(cb, &QCheckBox::toggled, contextObj, [filterModel, r](bool on) {
            FilterState st = filterModel->state();
            if (on) { if (!st.ratings.contains(r)) st.ratings.append(r); }
            else st.ratings.removeAll(r);
            filterModel->setState(st);
        });
    }
}

} // namespace QuarkMeta
