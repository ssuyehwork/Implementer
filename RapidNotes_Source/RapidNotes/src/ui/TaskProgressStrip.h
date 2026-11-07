#pragma once

#include <QProgressBar>
#include <QEvent>
#include <QWidget>
#include "../core/TaskProgressService.h"

namespace QuarkMeta {

/**
 * @brief 顶部悬浮线条进度条
 * 动态锚定在主界面中央视图与底部状态栏交界处，响应该后台任务进度
 */
class TaskProgressStrip : public QProgressBar {
    Q_OBJECT

public:
    explicit TaskProgressStrip(QWidget* parent = nullptr);
    ~TaskProgressStrip() override = default;

    void setAnchors(QWidget* bodyWrapper, QWidget* statusBar);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void updateGeometryPos();

    QWidget* m_bodyWrapper = nullptr;
    QWidget* m_statusBar = nullptr;
};

} // namespace QuarkMeta
