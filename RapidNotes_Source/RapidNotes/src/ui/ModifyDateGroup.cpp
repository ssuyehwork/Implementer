#include "ModifyDateGroup.h"
#include "UiHelper.h"

namespace QuarkMeta {

QLineEdit* ModifyDateGroup::populate(QObject* owner,
                                     QWidget* groupWidget,
                                     QHBoxLayout* headerLayout,
                                     QVBoxLayout* contentLayout,
                                     FilterStateModel* filterModel,
                                     const FilterState& currentState,
                                     bool& descending,
                                     RebuildDateCheckboxesFunc rebuildCheckboxes,
                                     SaveFilterHistoryFunc saveHistory) {
    if (!contentLayout || !filterModel || !headerLayout) return nullptr;

    QPushButton* btnSort = new QPushButton(groupWidget);
    btnSort->setFixedSize(16, 16);
    btnSort->setIconSize(QSize(12, 12));
    btnSort->setIcon(UiHelper::getIcon(descending ? "scroll-010.svg" : "scroll-007.svg", QColor("#B0B0B0")));
    btnSort->setFlat(true);
    btnSort->setCursor(Qt::PointingHandCursor);
    btnSort->setObjectName("FilterBtnSort");
    headerLayout->addWidget(btnSort);

    QObject::connect(btnSort, &QPushButton::clicked, owner, [owner, btnSort, &descending, rebuildCheckboxes]() {
        descending = !descending;
        btnSort->setIcon(UiHelper::getIcon(descending ? "scroll-010.svg" : "scroll-007.svg", QColor("#B0B0B0")));
        if (rebuildCheckboxes) rebuildCheckboxes(false, descending);
    });

    QWidget* wModifyDate = new QWidget(groupWidget);
    QHBoxLayout* lModifyDate = new QHBoxLayout(wModifyDate);
    lModifyDate->setContentsMargins(5, 6, 5, 4);
    lModifyDate->setSpacing(0);

    QLineEdit* editModifyDate = new QLineEdit(wModifyDate);
    editModifyDate->setClearButtonEnabled(true);
    editModifyDate->setPlaceholderText("例： 2025 / 03-2025...");
    editModifyDate->setText(currentState.modifyDateFilterText);
    editModifyDate->setObjectName("FilterSearchEdit");
    editModifyDate->setFixedHeight(22);
    if (owner) editModifyDate->installEventFilter(owner);

    QObject::connect(editModifyDate, &QLineEdit::returnPressed, owner, [filterModel, editModifyDate, saveHistory]() {
        FilterState st = filterModel->state();
        st.modifyDateFilterText = editModifyDate->text();
        if (saveHistory) saveHistory("ModifyDate", st.modifyDateFilterText);
        filterModel->setState(st);
    });

    QObject::connect(editModifyDate, &QLineEdit::textChanged, owner, [filterModel](const QString& text) {
        FilterState st = filterModel->state();
        if (text.isEmpty() && !st.modifyDateFilterText.isEmpty()) {
            st.modifyDateFilterText = "";
            filterModel->setState(st);
        }
    });

    lModifyDate->addWidget(editModifyDate);
    contentLayout->addWidget(wModifyDate);

    if (rebuildCheckboxes) rebuildCheckboxes(false, descending);

    return editModifyDate;
}

} // namespace QuarkMeta
