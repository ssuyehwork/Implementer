#ifndef NOMINMAX
#define NOMINMAX
#endif
#pragma once

#include <QObject>
#include <QWidget>
#include <QPointer>
#include <QEvent>

namespace RapidNotes {

/**
 * @brief 工业级无边框窗口助手类
 * 完全基于 Windows 原生 WM_NCHITTEST 进行硬件级缩放与拖拽，不使用暴力 grabMouse
 */
class FramelessWindowHelper : public QObject {
    Q_OBJECT

public:
    // 默认缩放热区宽度（px）；apply() 可按窗口可见边框内缩量覆盖，确保热区恰好覆盖可见边缘
    static constexpr int kBaseResizeMargin = 8;

    static FramelessWindowHelper* apply(QWidget* window, QWidget* titleBar = nullptr,
                                        int resizeMargin = kBaseResizeMargin);
    static void setAlwaysOnTop(QWidget* window, bool onTop);
    static bool isAlwaysOnTop(QWidget* window);

    bool handleNativeEvent(void* message, qintptr* result);
    static bool isInteractiveWidget(QWidget* child, QWidget* titleBar, QWidget* window);

private:
    explicit FramelessWindowHelper(QWidget* window, QWidget* titleBar = nullptr,
                                   int resizeMargin = kBaseResizeMargin);
    ~FramelessWindowHelper() override = default;

    // 注入原生窗口样式（WS_THICKFRAME 等），可在句柄重建后重复调用（幂等）
    void applyNativeStyles();
    // 生命周期钩子：WinIdChange（句柄重建）时自动补注 WS_THICKFRAME，取代一次性 static 开关
    bool eventFilter(QObject* watched, QEvent* event) override;

    QPointer<QWidget> m_window;
    QPointer<QWidget> m_titleBar;
    int m_resizeMargin = kBaseResizeMargin;
};

} // namespace RapidNotes
