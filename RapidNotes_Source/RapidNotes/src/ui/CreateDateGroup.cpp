#include "CreateDateGroup.h"
#include "UiHelper.h"

namespace QuarkMeta {

QLineEdit* CreateDateGroup::populate(QObject* owner,
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
        if (rebuildCheckboxes) rebuildCheckboxes(true, descending);
    });

    QWidget* wCreateDate = new QWidget(groupWidget);
    QHBoxLayout* lCreateDate = new QHBoxLayout(wCreateDate);
    lCreateDate->setContentsMargins(5, 6, 5, 4);
    lCreateDate->setSpacing(0);

    QLineEdit* editCreateDate = new QLineEdit(wCreateDate);
    editCreateDate->setClearButtonEnabled(true);
    editCreateDate->setPlaceholderText("例： 2025 / 03-2025...");
    editCreateDate->setText(currentState.createDateFilterText);
    editCreateDate->setObjectName("FilterSearchEdit");
    editCreateDate->setFixedHeight(22);
    if (owner) editCreateDate->installEventFilter(owner);

    QObject::connect(editCreateDate, &QLineEdit::returnPressed, owner, [filterModel, editCreateDate, saveHistory]() {
        FilterState st = filterModel->state();
        st.createDateFilterText = editCreateDate->text();
        if (saveHistory) saveHistory("CreateDate", st.createDateFilterText);
        filterModel->setState(st);
    });

    QObject::connect(editCreateDate, &QLineEdit::textChanged, owner, [filterModel](const QString& text) {
        FilterState st = filterModel->state();
        if (text.isEmpty() && !st.createDateFilterText.isEmpty()) {
            st.createDateFilterText = "";
            filterModel->setState(st);
        }
    });

    lCreateDate->addWidget(editCreateDate);
    contentLayout->addWidget(wCreateDate);

    if (rebuildCheckboxes) rebuildCheckboxes(true, descending);

    return editCreateDate;
}

} // namespace QuarkMeta
