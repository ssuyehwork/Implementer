#include "PaneActivationTracker.h"
#include "../ContentPanel.h"
#include <QEvent>
#include <QWidget>

namespace QuarkMeta {

PaneActivationTracker::PaneActivationTracker(QObject* parent)
    : QObject(parent) {
}

bool PaneActivationTracker::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn) {
        QWidget* widget = qobject_cast<QWidget*>(watched);
        if (widget) {
            QWidget* curr = widget;
            ContentPanel* foundPanel = nullptr;
            while (curr) {
                ContentPanel* panel = qobject_cast<ContentPanel*>(curr);
                if (panel) {
                    foundPanel = panel; // 持续向上寻找最近的 ContentPanel (副窗格位于根窗格内部，取最近的)
                    break;
                }
                curr = curr->parentWidget();
            }
            if (foundPanel) {
                emit paneInteracted(foundPanel);
            }
        }
    }
    return false; // 始终返回 false，不吞掉任何事件
}

} // namespace QuarkMeta
