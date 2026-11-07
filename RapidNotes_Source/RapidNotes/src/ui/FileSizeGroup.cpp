#include "FileSizeGroup.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

namespace QuarkMeta {

void FileSizeGroup::populate(QWidget* parentWidget,
                             QVBoxLayout* contentLayout,
                             FilterStateModel* filterModel) {
    if (!contentLayout || !filterModel) return;

    QWidget* contextObj = parentWidget ? parentWidget : contentLayout->parentWidget();

    QHBoxLayout* hs = new QHBoxLayout();
    hs->setContentsMargins(5, 4, 5, 8);
    hs->setSpacing(8);

    QLineEdit* minEdit = new QLineEdit(parentWidget);
    minEdit->setClearButtonEnabled(true);
    QLineEdit* maxEdit = new QLineEdit(parentWidget);
    maxEdit->setClearButtonEnabled(true);
    QComboBox* unitCombo = new QComboBox(parentWidget);
    unitCombo->addItems({"KB", "MB", "GB"});
    unitCombo->setCurrentIndex(1);

    minEdit->setObjectName("FilterSizeEdit");
    maxEdit->setObjectName("FilterSizeEdit");
    unitCombo->setObjectName("FilterUnitCombo");
    minEdit->setPlaceholderText("最小");
    maxEdit->setPlaceholderText("最大");
    minEdit->setFixedHeight(24);
    maxEdit->setFixedHeight(24);

    unitCombo->setFixedHeight(24);
    unitCombo->setFixedWidth(52);

    hs->addWidget(minEdit);
    QLabel* sep = new QLabel("-", parentWidget); sep->setObjectName("FilterSepLabel"); hs->addWidget(sep);
    hs->addWidget(maxEdit);
    hs->addWidget(unitCombo);
    contentLayout->addLayout(hs);

    auto updateSizeFilter = [filterModel, minEdit, maxEdit, unitCombo]() {
        auto toBytes = [](const QString& txt, const QString& unit) -> long long {
            if (txt.isEmpty()) return -1;
            bool ok;
            double val = txt.toDouble(&ok);
            if (!ok) return -1;
            long long factor = 1024;
            if (unit == "MB") factor = 1024 * 1024;
            else if (unit == "GB") factor = 1024 * 1024 * 1024;
            return (long long)(val * factor);
        };
        FilterState st = filterModel->state();
        st.minSize = toBytes(minEdit->text(), unitCombo->currentText());
        st.maxSize = toBytes(maxEdit->text(), unitCombo->currentText());
        filterModel->setState(st);
    };

    QObject::connect(minEdit, &QLineEdit::editingFinished, contextObj, updateSizeFilter);
    QObject::connect(minEdit, &QLineEdit::textChanged, contextObj, [updateSizeFilter](const QString& text) {
        if (text.isEmpty()) updateSizeFilter();
    });
    QObject::connect(maxEdit, &QLineEdit::editingFinished, contextObj, updateSizeFilter);
    QObject::connect(maxEdit, &QLineEdit::textChanged, contextObj, [updateSizeFilter](const QString& text) {
        if (text.isEmpty()) updateSizeFilter();
    });
    QObject::connect(unitCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), contextObj, [updateSizeFilter](int){ updateSizeFilter(); });
}

} // namespace QuarkMeta
