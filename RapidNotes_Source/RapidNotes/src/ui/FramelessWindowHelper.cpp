#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "FramelessWindowHelper.h"
#include <QDebug>
#include <QPushButton>
#include <QLineEdit>
#include <QToolButton>
#include <QSlider>
#include <QAbstractButton>
#include <QComboBox>
#include <QSpinBox>
#include <QScrollBar>
#include <QAbstractItemView>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#endif

namespace QuarkMeta {

FramelessWindowHelper* FramelessWindowHelper::apply(QWidget* window, QWidget* titleBar) {
    if (!window) return nullptr;
    return new FramelessWindowHelper(window, titleBar);
}

FramelessWindowHelper::FramelessWindowHelper(QWidget* window, QWidget* titleBar)
    : QObject(window), m_window(window), m_titleBar(titleBar) {

    Qt::WindowFlags requiredFlags = m_window->windowFlags() | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint;
    if (m_window->windowFlags() != requiredFlags) {
        m_window->setWindowFlags(requiredFlags);
    }

#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(m_window->winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    // 只有真正带标题栏（传入了 titleBar）的窗口，才需要完整的系统窗口属性
    // 像 TagSelectorOverlay 这种 Qt::Tool 悬浮面板，不该被强行赋予标题栏/最大化/系统菜单语义
    if (m_titleBar) {
        SetWindowLong(hwnd, GWL_STYLE, style | WS_THICKFRAME | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU);
    } else {
        SetWindowLong(hwnd, GWL_STYLE, style | WS_THICKFRAME);
    }
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
#endif
}

bool FramelessWindowHelper::isInteractiveWidget(QWidget* child, QWidget* titleBar, QWidget* window) {
    QWidget* wWidget = child;
    while (wWidget && wWidget != titleBar && wWidget != window) {
        if (qobject_cast<QAbstractButton*>(wWidget) ||
            qobject_cast<QLineEdit*>(wWidget) ||
            qobject_cast<QSlider*>(wWidget) ||
            qobject_cast<QComboBox*>(wWidget) ||
            qobject_cast<QSpinBox*>(wWidget) ||
            qobject_cast<QScrollBar*>(wWidget) ||
            qobject_cast<QAbstractItemView*>(wWidget)) {
            return true;
        }
        wWidget = wWidget->parentWidget();
    }
    return false;
}

bool FramelessWindowHelper::handleNativeEvent(void* message, qintptr* result) {
#ifdef Q_OS_WIN
    if (!m_window || !result) return false;

    MSG* msg = static_cast<MSG*>(message);
    if (!msg) return false;

    HWND hwnd = msg->hwnd;
    // 关键修正 2：必须以 Win32 原生权威状态为唯一准绳，严禁使用状态滞后的 m_window->isMaximized()
    const bool isMax = ::IsZoomed(hwnd);

    // 0. 尺寸与位置变动原生分发：第一时间校准标题栏最大化/还原图标
    if (msg->message == WM_SIZE || msg->message == WM_WINDOWPOSCHANGED) {
        if (m_titleBar) {
            QMetaObject::invokeMethod(m_titleBar, "setWindowMaximized", Q_ARG(bool, isMax));
        }
    }

    // 1. 客户区计算：彻底修复图二的左偏脱轨Bug
    if (msg->message == WM_NCCALCSIZE) {
        if (msg->wParam == TRUE) {
            NCCALCSIZE_PARAMS* pnc = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
            if (isMax) {
                // 仅在真实最大化时，通过工作区裁切吃掉 Windows 默认的 8px 不可见拉伸边框
                HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                if (monitor) {
                    MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
                    if (GetMonitorInfo(monitor, &monitorInfo)) {
                        pnc->rgrc[0] = monitorInfo.rcWork;
                    }
                }
            }
            // 返回 0：吃掉原生白条与非客户区标题栏，撑满客户区
            *result = 0;
            return true;
        }
        *result = 0;
        return true;
    }

    // 2. 最大化多屏及工作区边缘规范化
    if (msg->message == WM_GETMINMAXINFO) {
        MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(msg->lParam);
        if (mmi) {
            HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            if (monitor) {
                MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
                if (GetMonitorInfo(monitor, &monitorInfo)) {
                    RECT workArea = monitorInfo.rcWork;
                    RECT monitorArea = monitorInfo.rcMonitor;
                    mmi->ptMaxPosition.x = workArea.left - monitorArea.left;
                    mmi->ptMaxPosition.y = workArea.top - monitorArea.top;
                    mmi->ptMaxSize.x = workArea.right - workArea.left;
                    mmi->ptMaxSize.y = workArea.bottom - workArea.top;
                }
            }
            if (m_window) {
                QSize minSz = m_window->minimumSize();
                if (minSz.width() > 0) mmi->ptMinTrackSize.x = minSz.width();
                if (minSz.height() > 0) mmi->ptMinTrackSize.y = minSz.height();
            }
        }
        *result = 0;
        return true;
    }

    // 3. 原生 WM_NCHITTEST 精确命中检测
    if (msg->message == WM_NCHITTEST) {
        POINT screenPt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
        QPoint localPos = m_window->mapFromGlobal(QPoint(screenPt.x, screenPt.y));

        int width = m_window->width();
        int height = m_window->height();

        bool isMaximizedOrFullScreen = isMax || m_window->isFullScreen();

        if (!isMaximizedOrFullScreen) {
            const int m = kBaseResizeMargin;
            bool left = localPos.x() >= 0 && localPos.x() < m;
            bool right = localPos.x() >= width - m && localPos.x() < width;
            bool top = localPos.y() >= 0 && localPos.y() < m;
            bool bottom = localPos.y() >= height - m && localPos.y() < height;

            if (top && left)     { *result = HTTOPLEFT;     return true; }
            if (top && right)    { *result = HTTOPRIGHT;    return true; }
            if (bottom && left)  { *result = HTBOTTOMLEFT;  return true; }
            if (bottom && right) { *result = HTBOTTOMRIGHT; return true; }
            if (left)            { *result = HTLEFT;        return true; }
            if (right)           { *result = HTRIGHT;       return true; }
            if (top)             { *result = HTTOP;         return true; }
            if (bottom)          { *result = HTBOTTOM;      return true; }
        }

        // 标题栏原生拖拽与双击识别（排除交互控件）
        if (m_titleBar && !m_window->isFullScreen()) {
            QRect titleRect = QRect(m_titleBar->mapTo(m_window, QPoint(0, 0)), m_titleBar->size());
            if (titleRect.contains(localPos)) {
                QWidget* childAtPt = m_window->childAt(localPos);
                if (!isInteractiveWidget(childAtPt, m_titleBar, m_window)) {
                    *result = HTCAPTION;
                    return true;
                }
            }
        }

        *result = HTCLIENT;
        return true;
    }

    // 4. 原生光标设置
    if (msg->message == WM_SETCURSOR) {
        WORD hitTest = LOWORD(msg->lParam);
        LPCWSTR cursorId = nullptr;
        switch (hitTest) {
            case HTTOP:
            case HTBOTTOM:
                cursorId = IDC_SIZENS;   break;
            case HTLEFT:
            case HTRIGHT:
                cursorId = IDC_SIZEWE;   break;
            case HTTOPLEFT:
            case HTBOTTOMRIGHT:
                cursorId = IDC_SIZENWSE; break;
            case HTTOPRIGHT:
            case HTBOTTOMLEFT:
                cursorId = IDC_SIZENESW; break;
            default:
                break;
        }
        if (cursorId) {
            SetCursor(LoadCursor(nullptr, cursorId));
            *result = TRUE;
            return true;
        }
        return false;
    }

    // 5. 原生双击标题栏最大化 / 还原（通过 Win32 消息总线响应）
    if (msg->message == WM_NCLBUTTONDBLCLK) {
        if (msg->wParam == HTCAPTION) {
            ::SendMessage(hwnd, WM_SYSCOMMAND, isMax ? SC_RESTORE : SC_MAXIMIZE, 0);
            *result = 0;
            return true;
        }
    }
#else
    Q_UNUSED(message);
    Q_UNUSED(result);
#endif
    return false;
}

void FramelessWindowHelper::setAlwaysOnTop(QWidget* window, bool onTop) {
    if (!window) return;

#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(window->winId());
    SetWindowPos(hwnd, onTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
#else
    Qt::WindowFlags flags = window->windowFlags();
    if (onTop) flags |= Qt::WindowStaysOnTopHint;
    else flags &= ~Qt::WindowStaysOnTopHint;
    window->setWindowFlags(flags);
    window->show();
#endif
}

bool FramelessWindowHelper::isAlwaysOnTop(QWidget* window) {
    if (!window) return false;
    return (window->windowFlags() & Qt::WindowStaysOnTopHint) != 0;
}

} // namespace QuarkMeta