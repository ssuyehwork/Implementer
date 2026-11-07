#include "UiHelper.h"
#include "ThemeManager.h"
#include "SvgIcons.h"
#include "../core/ItemRecord.h"
#include "../util/DiskMediaExtractor.h"

#include <QApplication>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QMenu>
#include <QClipboard>
#include <QKeyEvent>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {


void UiHelper::showTextEditContextMenu(QWidget* textWidget, const QPoint& pos) {
    if (!textWidget) return;
    QMenu menu(textWidget);
    applyMenuStyle(&menu);

    QTextEdit* textEdit = qobject_cast<QTextEdit*>(textWidget);
    QPlainTextEdit* plainEdit = qobject_cast<QPlainTextEdit*>(textWidget);
    if (!textEdit && !plainEdit) return;

    bool canUndo = textEdit ? textEdit->document()->isUndoAvailable() : plainEdit->document()->isUndoAvailable();
    bool canRedo = textEdit ? textEdit->document()->isRedoAvailable() : plainEdit->document()->isRedoAvailable();
    bool isReadOnly = textEdit ? textEdit->isReadOnly() : plainEdit->isReadOnly();
    bool hasSelection = textEdit ? textEdit->textCursor().hasSelection() : plainEdit->textCursor().hasSelection();

    QAction* actUndo = menu.addAction(getIcon("undo", QColor("#EEEEEE")), "撤销");
    actUndo->setShortcut(QKeySequence::Undo);
    actUndo->setEnabled(canUndo);
    if (textEdit) QObject::connect(actUndo, &QAction::triggered, textEdit, &QTextEdit::undo);
    else QObject::connect(actUndo, &QAction::triggered, plainEdit, &QPlainTextEdit::undo);

    QAction* actRedo = menu.addAction(getIcon("redo", QColor("#EEEEEE")), "重做");
    actRedo->setShortcut(QKeySequence::Redo);
    actRedo->setEnabled(canRedo);
    if (textEdit) QObject::connect(actRedo, &QAction::triggered, textEdit, &QTextEdit::redo);
    else QObject::connect(actRedo, &QAction::triggered, plainEdit, &QPlainTextEdit::redo);

    menu.addSeparator();

    QAction* actCut = menu.addAction(getIcon("cut", QColor("#EEEEEE")), "剪切");
    actCut->setShortcut(QKeySequence::Cut);
    actCut->setEnabled(!isReadOnly && hasSelection);
    if (textEdit) QObject::connect(actCut, &QAction::triggered, textEdit, &QTextEdit::cut);
    else QObject::connect(actCut, &QAction::triggered, plainEdit, &QPlainTextEdit::cut);

    QAction* actCopy = menu.addAction(getIcon("copy", QColor("#EEEEEE")), "复制");
    actCopy->setShortcut(QKeySequence::Copy);
    actCopy->setEnabled(hasSelection);
    if (textEdit) QObject::connect(actCopy, &QAction::triggered, textEdit, &QTextEdit::copy);
    else QObject::connect(actCopy, &QAction::triggered, plainEdit, &QPlainTextEdit::copy);

    QAction* actPaste = menu.addAction(getIcon("paste", QColor("#EEEEEE")), "粘贴");
    actPaste->setShortcut(QKeySequence::Paste);
    actPaste->setEnabled(!isReadOnly && !QApplication::clipboard()->text().isEmpty());
    if (textEdit) QObject::connect(actPaste, &QAction::triggered, textEdit, &QTextEdit::paste);
    else QObject::connect(actPaste, &QAction::triggered, plainEdit, &QPlainTextEdit::paste);

    menu.addSeparator();

    QAction* actSelectAll = menu.addAction(getIcon("select", QColor("#EEEEEE")), "全选");
    actSelectAll->setShortcut(QKeySequence::SelectAll);
    if (textEdit) QObject::connect(actSelectAll, &QAction::triggered, textEdit, &QTextEdit::selectAll);
    else QObject::connect(actSelectAll, &QAction::triggered, plainEdit, &QPlainTextEdit::selectAll);

    menu.exec(textWidget->mapToGlobal(pos));
}

void UiHelper::showLineEditContextMenu(QLineEdit* edit, const QPoint& pos) {
    if (!edit) return;
    QMenu menu(edit);
    applyMenuStyle(&menu);

    QAction* actUndo = menu.addAction(getIcon("undo", QColor("#EEEEEE")), "撤销");
    actUndo->setShortcut(QKeySequence::Undo);
    actUndo->setEnabled(edit->isUndoAvailable());
    QObject::connect(actUndo, &QAction::triggered, edit, &QLineEdit::undo);

    QAction* actRedo = menu.addAction(getIcon("redo", QColor("#EEEEEE")), "重做");
    actRedo->setShortcut(QKeySequence::Redo);
    actRedo->setEnabled(edit->isRedoAvailable());
    QObject::connect(actRedo, &QAction::triggered, edit, &QLineEdit::redo);

    menu.addSeparator();

    QAction* actCut = menu.addAction(getIcon("cut", QColor("#EEEEEE")), "剪切");
    actCut->setShortcut(QKeySequence::Cut);
    actCut->setEnabled(!edit->isReadOnly() && edit->hasSelectedText());
    QObject::connect(actCut, &QAction::triggered, edit, &QLineEdit::cut);

    QAction* actCopy = menu.addAction(getIcon("copy", QColor("#EEEEEE")), "复制");
    actCopy->setShortcut(QKeySequence::Copy);
    actCopy->setEnabled(edit->hasSelectedText());
    QObject::connect(actCopy, &QAction::triggered, edit, &QLineEdit::copy);

    QAction* actPaste = menu.addAction(getIcon("paste", QColor("#EEEEEE")), "粘贴");
    actPaste->setShortcut(QKeySequence::Paste);
    actPaste->setEnabled(!edit->isReadOnly() && !QApplication::clipboard()->text().isEmpty());
    QObject::connect(actPaste, &QAction::triggered, edit, &QLineEdit::paste);

    QAction* actDelete = menu.addAction(getIcon("delete_forever", QColor("#EEEEEE")), "删除");
    actDelete->setEnabled(!edit->isReadOnly() && edit->hasSelectedText());
    QObject::connect(actDelete, &QAction::triggered, edit, [edit]() {
        edit->insert("");
    });

    menu.addSeparator();

    QAction* actSelectAll = menu.addAction(getIcon("select", QColor("#EEEEEE")), "全选");
    actSelectAll->setShortcut(QKeySequence::SelectAll);
    actSelectAll->setEnabled(!edit->text().isEmpty());
    QObject::connect(actSelectAll, &QAction::triggered, edit, &QLineEdit::selectAll);

    menu.exec(edit->mapToGlobal(pos));
}

QMenu* UiHelper::createMenu(QWidget* parent) {
    QMenu* menu = new QMenu(parent);
    applyMenuStyle(menu);
    return menu;
}

QAction* UiHelper::setupLineEditClearButton(QLineEdit* edit) {
    if (!edit) return nullptr;
    QAction* clearAction = edit->addAction(getIcon("clear-2", QColor("#888888"), 15), QLineEdit::TrailingPosition);
    clearAction->setVisible(!edit->text().isEmpty());
    QObject::connect(clearAction, &QAction::triggered, edit, &QLineEdit::clear);
    QObject::connect(edit, &QLineEdit::textChanged, edit, [clearAction](const QString& text) {
        clearAction->setVisible(!text.isEmpty());
    });
    return clearAction;
}

void UiHelper::setupLineEditContextMenu(QLineEdit* edit) {
    if (!edit) return;
    edit->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(edit, &QLineEdit::customContextMenuRequested, edit, [edit](const QPoint& pos) {
        showLineEditContextMenu(edit, pos);
    });
}


} // namespace QuarkMeta
