#include "RenameCapableDelegate.h"
#include "../core/ModelContract.h"

namespace QuarkMeta {

QWidget* RenameCapableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const {
    FileNameLineEdit* editor = new FileNameLineEdit(parent);
    bool isFolder = (index.data(TypeRole).toString() == "folder");
    editor->setIsFolder(isFolder);
    return editor;
}

void RenameCapableDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QString value = index.model()->data(index, Qt::EditRole).toString();
    FileNameLineEdit* lineEdit = qobject_cast<FileNameLineEdit*>(editor);
    if (lineEdit) {
        lineEdit->setText(value);
    }
}

void RenameCapableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;
    QString newName = lineEdit->text().trimmed();
    if (!newName.isEmpty()) {
        model->setData(index, newName, Qt::EditRole);
    }
}

} // namespace QuarkMeta
