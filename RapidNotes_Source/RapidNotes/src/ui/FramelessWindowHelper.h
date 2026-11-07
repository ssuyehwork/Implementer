#ifndef NOMINMAX
#define NOMINMAX
#endif
#pragma once

#include <QObject>
#include <QWidget>
#include <QPointer>

namespace QuarkMeta {

/**
 * @brief 工业级无边框窗口助手类
 * 完全基于 Windows 原生 WM_NCHITTEST 进行硬件级缩放与拖拽，不使用暴力 grabMouse
 */
class FramelessWindowHelper : public QObject {
    Q_OBJECT

public:
    static FramelessWindowHelper* apply(QWidget* window, QWidget* titleBar = nullptr);
    static void setAlwaysOnTop(QWidget* window, bool onTop);
    static bool isAlwaysOnTop(QWidget* window);

    bool handleNativeEvent(void* message, qintptr* result);
    static bool isInteractiveWidget(QWidget* child, QWidget* titleBar, QWidget* window);

private:
    explicit FramelessWindowHelper(QWidget* window, QWidget* titleBar = nullptr);
    ~FramelessWindowHelper() override = default;

    QPointer<QWidget> m_window;
    QPointer<QWidget> m_titleBar;

    static constexpr int kBaseResizeMargin = 8;
};

} // namespace QuarkMeta
