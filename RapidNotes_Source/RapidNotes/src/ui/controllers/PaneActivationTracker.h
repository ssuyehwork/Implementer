#pragma once

#include <QObject>

namespace QuarkMeta {

class ContentPanel;

/**
 * @brief 全应用窗格激活事件追踪器
 * 在 qApp 上安装事件过滤器，处理 MouseButtonPress 与 FocusIn 事件，
 * 精准捕获用户点击或聚焦的 ContentPanel 并触发激活。
 */
class PaneActivationTracker : public QObject {
    Q_OBJECT
public:
    explicit PaneActivationTracker(QObject* parent = nullptr);
    ~PaneActivationTracker() override = default;

signals:
    void paneInteracted(ContentPanel* panel);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

} // namespace QuarkMeta
