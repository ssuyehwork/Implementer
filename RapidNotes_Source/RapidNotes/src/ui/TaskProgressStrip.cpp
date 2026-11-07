#include "TaskProgressStrip.h"
#include <QPoint>
#include <QRect>

namespace QuarkMeta {

constexpr int kStripHeight = 5;

TaskProgressStrip::TaskProgressStrip(QWidget* parent)
    : QProgressBar(parent) {
    setObjectName("TaskProgressStrip");
    setFixedHeight(kStripHeight);
    setRange(0, 100);
    setTextVisible(false);
    hide();

    connect(&TaskProgressService::instance(), &TaskProgressService::progressUpdated,
            this, [this](int percent, const QString& title, const QString& detail, int activeCount) {
        Q_UNUSED(title);
        Q_UNUSED(detail);
        if (activeCount > 0) {
            setValue(percent);
            if (!isVisible()) {
                show();
                updateGeometryPos();
            }
        } else {
            hide();
        }
    });

    connect(&TaskProgressService::instance(), &TaskProgressService::allJobsFinished,
            this, [this]() {
        hide();
    });
}

void TaskProgressStrip::setAnchors(QWidget* bodyWrapper, QWidget* statusBar) {
    m_bodyWrapper = bodyWrapper;
    m_statusBar = statusBar;

    QWidget* topParent = parentWidget();
    if (topParent) {
        topParent->installEventFilter(this);
    }
    if (m_bodyWrapper) {
        m_bodyWrapper->installEventFilter(this);
    }
    if (m_statusBar) {
        m_statusBar->installEventFilter(this);
    }

    updateGeometryPos();
}

bool TaskProgressStrip::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::Resize || event->type() == QEvent::Move || event->type() == QEvent::Show) {
        updateGeometryPos();
    }
    return QProgressBar::eventFilter(watched, event);
}

void TaskProgressStrip::updateGeometryPos() {
    QWidget* topParent = parentWidget();
    if (!topParent || !m_bodyWrapper || !m_statusBar) return;

    QPoint bodyLeftTop = m_bodyWrapper->mapTo(topParent, QPoint(0, 0));
    QPoint statusLeftTop = m_statusBar->mapTo(topParent, QPoint(0, 0));

    int x = bodyLeftTop.x();
    int w = m_bodyWrapper->width();
    int y = statusLeftTop.y() - kStripHeight;

    setGeometry(x, y, w, kStripHeight);
    raise();
}

} // namespace QuarkMeta
