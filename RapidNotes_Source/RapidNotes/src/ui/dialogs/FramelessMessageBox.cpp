#include "dialogs/FramelessMessageBox.h"
#include "dialogs/FramelessConfirmDialog.h"

namespace QuarkMeta {

void FramelessMessageBox::information(QWidget* parent, const QString& title, const QString& text) {
    FramelessConfirmDialog dlg(title, text, FramelessConfirmDialog::OkOnly, "info", QColor("#3498db"), parent);
    dlg.exec();
}

void FramelessMessageBox::warning(QWidget* parent, const QString& title, const QString& text) {
    FramelessConfirmDialog dlg(title, text, FramelessConfirmDialog::OkOnly, "warning", QColor("#f1c40f"), parent);
    dlg.exec();
}

bool FramelessMessageBox::question(QWidget* parent, const QString& title, const QString& text) {
    FramelessConfirmDialog dlg(title, text, FramelessConfirmDialog::OkCancel, "help", QColor("#3498db"), parent);
    return dlg.exec() == QDialog::Accepted;
}

void FramelessMessageBox::critical(QWidget* parent, const QString& title, const QString& text) {
    FramelessConfirmDialog dlg(title, text, FramelessConfirmDialog::OkOnly, "error", QColor("#e81123"), parent);
    dlg.exec();
}

} // namespace QuarkMeta
