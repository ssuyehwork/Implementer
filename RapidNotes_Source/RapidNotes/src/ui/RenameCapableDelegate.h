#pragma once

#include <QStyledItemDelegate>
#include "FileNameLineEdit.h"

namespace QuarkMeta {

/**
 * @brief Base Delegate enforcing unified inline rename logic across all view delegates.
 */
class RenameCapableDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override final;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override final;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override final;
};

} // namespace QuarkMeta
