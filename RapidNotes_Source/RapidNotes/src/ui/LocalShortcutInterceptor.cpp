#include "LocalShortcutInterceptor.h"
#include <QApplication>
#include <QMetaObject>
#include <QKeyCombination>
#include <QDebug>

namespace QuarkMeta {

LocalShortcutInterceptor& LocalShortcutInterceptor::instance() {
    static LocalShortcutInterceptor s_instance;
    return s_instance;
}

LocalShortcutInterceptor::LocalShortcutInterceptor(QObject* parent) : QObject(parent) {
    connect(qApp, &QApplication::aboutToQuit, this, [this]() {
        uninstallHook();
    });
}

LocalShortcutInterceptor::~LocalShortcutInterceptor() {
    uninstallHook();
}

ShortcutKeyCombo LocalShortcutInterceptor::parseQtKey(int key, Qt::KeyboardModifiers modifiers) {
    ShortcutKeyCombo combo;
    combo.ctrl = (modifiers & Qt::ControlModifier) != 0;
    combo.shift = (modifiers & Qt::ShiftModifier) != 0;
    combo.alt = (modifiers & Qt::AltModifier) != 0;

    int cleanKey = key & ~Qt::KeyboardModifierMask;

    if (cleanKey >= Qt::Key_A && cleanKey <= Qt::Key_Z) {
        combo.vkCode = 'A' + (cleanKey - Qt::Key_A);
    } else if (cleanKey >= Qt::Key_0 && cleanKey <= Qt::Key_9) {
        combo.vkCode = '0' + (cleanKey - Qt::Key_0);
    } else if (cleanKey >= Qt::Key_F1 && cleanKey <= Qt::Key_F24) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_F1 + (cleanKey - Qt::Key_F1);
#endif
    } else if (cleanKey == Qt::Key_Tab || cleanKey == Qt::Key_Backtab) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_TAB;
#endif
    } else if (cleanKey == Qt::Key_Space) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_SPACE;
#endif
    } else if (cleanKey == Qt::Key_Escape) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_ESCAPE;
#endif
    } else if (cleanKey == Qt::Key_Return || cleanKey == Qt::Key_Enter) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_RETURN;
#endif
    } else if (cleanKey == Qt::Key_Backspace) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_BACK;
#endif
    } else if (cleanKey == Qt::Key_Delete) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_DELETE;
#endif
    } else if (cleanKey == Qt::Key_Left) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_LEFT;
#endif
    } else if (cleanKey == Qt::Key_Right) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_RIGHT;
#endif
    } else if (cleanKey == Qt::Key_Up) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_UP;
#endif
    } else if (cleanKey == Qt::Key_Down) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_DOWN;
#endif
    } else if (cleanKey == Qt::Key_Slash) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_2;
#endif
    } else if (cleanKey == Qt::Key_Minus) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_MINUS;
#endif
    } else if (cleanKey == Qt::Key_Equal || cleanKey == Qt::Key_Plus) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_PLUS;
#endif
    } else if (cleanKey == Qt::Key_Comma) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_COMMA;
#endif
    } else if (cleanKey == Qt::Key_Period) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_PERIOD;
#endif
    } else if (cleanKey == Qt::Key_Semicolon) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_1;
#endif
    } else if (cleanKey == Qt::Key_Apostrophe) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_7;
#endif
    } else if (cleanKey == Qt::Key_BracketLeft) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_4;
#endif
    } else if (cleanKey == Qt::Key_BracketRight) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_6;
#endif
    } else if (cleanKey == Qt::Key_Backslash) {
#ifdef Q_OS_WIN
        combo.vkCode = VK_OEM_5;
#endif
    }

    return combo;
}

ShortcutKeyCombo LocalShortcutInterceptor::parseKeySequence(const QKeySequence& sequence) {
    if (sequence.isEmpty()) return {};
    QKeyCombination combo = sequence[0];
    return parseQtKey(combo.key(), combo.keyboardModifiers());
}

void LocalShortcutInterceptor::registerProtectedShortcut(const QKeySequence& sequence, std::function<void()> callback) {
    ShortcutKeyCombo combo = parseKeySequence(sequence);
    if (combo.vkCode != 0 && callback) {
        m_callbacks.insert(combo, callback);
    }
}

void LocalShortcutInterceptor::registerProtectedShortcut(int key, Qt::KeyboardModifiers modifiers, std::function<void()> callback) {
    ShortcutKeyCombo combo = parseQtKey(key, modifiers);
    if (combo.vkCode != 0 && callback) {
        m_callbacks.insert(combo, callback);
    }
}

void LocalShortcutInterceptor::unregisterAllShortcuts() {
    m_callbacks.clear();
}

void LocalShortcutInterceptor::setEditFocusPredicate(std::function<bool()> predicate) {
    m_editFocusPredicate = predicate;
}

void LocalShortcutInterceptor::updateFocusState(bool isActive) {
    if (m_isActive == isActive) return;
    m_isActive = isActive;

    if (m_isActive) {
        installHook();
    } else {
        uninstallHook();
    }
}

void LocalShortcutInterceptor::installHook() {
#ifdef Q_OS_WIN
    if (m_hookInstalled || !m_isActive) return;
    m_hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    if (m_hook) {
        m_hookInstalled = true;
    }
#endif
}

void LocalShortcutInterceptor::uninstallHook() {
#ifdef Q_OS_WIN
    if (m_hookInstalled && m_hook) {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
        m_hookInstalled = false;
    }
#endif
}

#ifdef Q_OS_WIN
LRESULT CALLBACK LocalShortcutInterceptor::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        PKBDLLHOOKSTRUCT p = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam);

        bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;

        if (LocalShortcutInterceptor::instance().handleLowLevelKey(static_cast<int>(p->vkCode), ctrl, shift, alt)) {
            return 1;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
#endif

bool LocalShortcutInterceptor::handleLowLevelKey(int vkCode, bool ctrl, bool shift, bool alt) {
    if (!m_isActive) return false;

    // 焦点在文本输入框/可编辑单元格时，让渡所有常见编辑按键的处理
    if (m_editFocusPredicate && m_editFocusPredicate()) {
        if (vkCode == 'C' || vkCode == 'V' || vkCode == 'X' || vkCode == 'A' || vkCode == 'Z' || vkCode == 'Y' || vkCode == VK_TAB || vkCode == VK_RETURN || vkCode == VK_BACK || vkCode == VK_DELETE) {
            return false;
        }
    }

    ShortcutKeyCombo combo;
    combo.vkCode = vkCode;
    combo.ctrl = ctrl;
    combo.shift = shift;
    combo.alt = alt;

    if (m_callbacks.contains(combo)) {
        auto callback = m_callbacks.value(combo);
        if (callback) {
            QMetaObject::invokeMethod(this, [callback]() {
                callback();
            }, Qt::QueuedConnection);
            return true;
        }
    }
    return false;
}

} // namespace QuarkMeta
