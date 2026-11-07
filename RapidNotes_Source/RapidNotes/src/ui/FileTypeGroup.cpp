#include "FileTypeGroup.h"
#include <QHBoxLayout>

namespace QuarkMeta {

QLineEdit* FileTypeGroup::populate(QObject* owner,
                                   QWidget* groupWidget,
                                   QVBoxLayout* contentLayout,
                                   FilterStateModel* filterModel,
                                   const FilterState& currentState,
                                   int emptyFolderCount,
                                   const QMap<QString, int>& typeCounts,
                                   AddFilterRowFunc addFilterRow,
                                   SaveFilterHistoryFunc saveHistory) {
    if (!contentLayout || !filterModel || !addFilterRow) return nullptr;

    QWidget* wType = new QWidget(groupWidget);
    QHBoxLayout* lType = new QHBoxLayout(wType);
    lType->setContentsMargins(5, 6, 5, 4);
    lType->setSpacing(0);

    QLineEdit* editType = new QLineEdit(wType);
    editType->setClearButtonEnabled(true);
    editType->setPlaceholderText("例： png / 文件夹...");
    editType->setText(currentState.typeFilterText);
    editType->setObjectName("FilterSearchEdit");
    editType->setFixedHeight(22);
    if (owner) editType->installEventFilter(owner);

    QObject::connect(editType, &QLineEdit::returnPressed, owner, [filterModel, editType, saveHistory]() {
        FilterState st = filterModel->state();
        st.typeFilterText = editType->text();
        if (saveHistory) saveHistory("Type", st.typeFilterText);
        filterModel->setState(st);
    });

    QObject::connect(editType, &QLineEdit::textChanged, owner, [filterModel](const QString& text) {
        FilterState st = filterModel->state();
        if (text.isEmpty() && !st.typeFilterText.isEmpty()) {
            st.typeFilterText = "";
            filterModel->setState(st);
        }
    });

    lType->addWidget(editType);
    contentLayout->addWidget(wType);

    if (emptyFolderCount > 0) {
        QCheckBox* cb = addFilterRow(contentLayout, "空文件夹", emptyFolderCount);
        cb->blockSignals(true);
        cb->setChecked(currentState.types.contains("空文件夹"));
        cb->blockSignals(false);
        QObject::connect(cb, &QCheckBox::toggled, owner, [filterModel](bool on) {
            FilterState st = filterModel->state();
            if (on) { if (!st.types.contains("空文件夹")) st.types.append("空文件夹"); }
            else    st.types.removeAll("空文件夹");
            filterModel->setState(st);
        });
    }

    if (typeCounts.contains("folder") && typeCounts["folder"] > 0) {
        QCheckBox* cb = addFilterRow(contentLayout, "文件夹", typeCounts["folder"]);
        cb->blockSignals(true);
        cb->setChecked(currentState.types.contains("folder"));
        cb->blockSignals(false);
        QObject::connect(cb, &QCheckBox::toggled, owner, [filterModel](bool on) {
            FilterState st = filterModel->state();
            if (on) { if (!st.types.contains("folder")) st.types.append("folder"); }
            else    st.types.removeAll("folder");
            filterModel->setState(st);
        });
    }

    if (typeCounts.contains("file") && typeCounts["file"] > 0) {
        QCheckBox* cb = addFilterRow(contentLayout, "文件", typeCounts["file"]);
        cb->blockSignals(true);
        cb->setChecked(currentState.types.contains("file"));
        cb->blockSignals(false);
        QObject::connect(cb, &QCheckBox::toggled, owner, [filterModel](bool on) {
            FilterState st = filterModel->state();
            if (on) { if (!st.types.contains("file")) st.types.append("file"); }
            else    st.types.removeAll("file");
            filterModel->setState(st);
        });
    }

    QStringList exts = typeCounts.keys(); exts.sort();
    for (const QString& ext : exts) {
        if (ext == "folder" || ext == "file" || ext == "空文件夹" || typeCounts[ext] <= 0) continue;
        QString label = ext.isEmpty() ? "无扩展名" : ext;
        QCheckBox* cb = addFilterRow(contentLayout, label, typeCounts[ext]);
        cb->blockSignals(true);
        cb->setChecked(currentState.types.contains(ext));
        cb->blockSignals(false);
        QObject::connect(cb, &QCheckBox::toggled, owner, [filterModel, ext](bool on) {
            FilterState st = filterModel->state();
            if (on) { if (!st.types.contains(ext)) st.types.append(ext); }
            else st.types.removeAll(ext);
            filterModel->setState(st);
        });
    }

    return editType;
}

} // namespace QuarkMeta
