#pragma once

#include <QObject>
#include <QKeySequence>
#include <QMap>
#include <functional>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

struct ShortcutKeyCombo {
    int vkCode = 0;
    bool ctrl = false;
    bool shift = false;
    bool alt = false;

    bool operator<(const ShortcutKeyCombo& other) const {
        if (vkCode != other.vkCode) return vkCode < other.vkCode;
        if (ctrl != other.ctrl) return ctrl < other.ctrl;
        if (shift != other.shift) return shift < other.shift;
        return alt < other.alt;
    }
};

/**
 * @brief 局内快捷键防抢占保护器 (Local Shortcut Guard)
 * 基于 Windows 低级键盘钩子 WH_KEYBOARD_LL 实现，优先级高于 RegisterHotKey 系统热键。
 * 仅在应用处于前台激活状态时挂载钩子，失去焦点时立即自动卸载。
 */
class LocalShortcutInterceptor : public QObject {
    Q_OBJECT

public:
    static LocalShortcutInterceptor& instance();

    void registerProtectedShortcut(const QKeySequence& sequence, std::function<void()> callback);
    void registerProtectedShortcut(int key, Qt::KeyboardModifiers modifiers, std::function<void()> callback);
    void unregisterAllShortcuts();

    void updateFocusState(bool isActive);
    bool isActive() const { return m_isActive; }

    void setEditFocusPredicate(std::function<bool()> predicate);

    static ShortcutKeyCombo parseKeySequence(const QKeySequence& sequence);
    static ShortcutKeyCombo parseQtKey(int key, Qt::KeyboardModifiers modifiers);

private:
    explicit LocalShortcutInterceptor(QObject* parent = nullptr);
    ~LocalShortcutInterceptor() override;

    void installHook();
    void uninstallHook();

#ifdef Q_OS_WIN
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
#endif

    bool handleLowLevelKey(int vkCode, bool ctrl, bool shift, bool alt);

    bool m_isActive = false;
    bool m_hookInstalled = false;

#ifdef Q_OS_WIN
    HHOOK m_hook = nullptr;
#endif

    QMap<ShortcutKeyCombo, std::function<void()>> m_callbacks;
    std::function<bool()> m_editFocusPredicate;
};

} // namespace QuarkMeta
