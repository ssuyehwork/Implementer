#include "QuickShortcutHandler.h"
#include "../QuickWindow.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QApplication>

namespace RapidNotes {

QuickShortcutHandler::QuickShortcutHandler(QuickWindow* window, QObject* parent)
    : QObject(parent), m_window(window) {}

bool QuickShortcutHandler::isEditingFocus() const {
    QWidget* focusWidget = QApplication::focusWidget();
    if (!focusWidget) return false;
    return qobject_cast<QLineEdit*>(focusWidget) != nullptr ||
           qobject_cast<QTextEdit*>(focusWidget) != nullptr;
}

bool QuickShortcutHandler::handleKeyPress(QKeyEvent* event) {
    if (!event || !m_window) return false;

    // 1. 打字态焦点严格保护
    if (isEditingFocus()) {
        if (event->key() == Qt::Key_Escape) {
            m_window->hide();
            return true;
        }
        return false; // 原生放行输入
    }

    // 2. 常规快捷键调度
    if (event->key() == Qt::Key_Delete) {
        bool shiftPressed = (event->modifiers() & Qt::ShiftModifier);
        m_window->doDeleteSelected(shiftPressed);
        return true;
    }

    if (event->key() == Qt::Key_Escape) {
        m_window->hide();
        return true;
    }

    return false;
}

} // namespace RapidNotes
