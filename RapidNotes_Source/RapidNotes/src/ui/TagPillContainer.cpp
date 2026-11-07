#include "TagPillContainer.h"
#include "components/TagPill.h"
#include <QMouseEvent>
#include <QLayoutItem>

namespace QuarkMeta {

TagPillContainer::TagPillContainer(QWidget* parent, TagTriggerSource source)
    : QFrame(parent), m_triggerSource(source) {
    setObjectName("PresetTagContainer");
    setFrameShape(QFrame::StyledPanel);
    setCursor(Qt::PointingHandCursor);
    setMinimumHeight(42);

    m_flowLayout = new FlowLayout(this, 8, 8, 8);
    setLayout(m_flowLayout);

    installEventFilter(this);
}

void TagPillContainer::setTags(const QStringList& tags) {
    if (m_tags == tags) return;
    m_tags = tags;
    populateTagPills();
}

QStringList TagPillContainer::tags() const {
    return m_tags;
}

void TagPillContainer::populateTagPills() {
    QLayoutItem* item;
    while ((item = m_flowLayout->takeAt(0)) != nullptr) {
        if (QWidget* w = item->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete item;
    }

    for (const QString& tag : m_tags) {
        TagPill* pill = new TagPill(tag, this);
        connect(pill, &TagPill::deleteRequested, this, &TagPillContainer::onTagDeleted);
        m_flowLayout->addWidget(pill);
    }

    updateGeometry();
    emit containerHeightChanged();
}

void TagPillContainer::onTagDeleted(const QString& tag) {
    m_tags.removeAll(tag);
    populateTagPills();
    emit tagsChanged(m_tags);
}

void TagPillContainer::onTagContainerClicked() {
    if (m_selectorOverlay) {
        m_selectorOverlay->raise();
        m_selectorOverlay->activateWindow();
        return;
    }

    m_selectorOverlay = new TagSelectorOverlay(m_tags, window(), m_triggerSource);

    QPoint globalPos = mapToGlobal(QPoint(0, height() + 4));
    m_selectorOverlay->move(globalPos);
    m_selectorOverlay->show();
    m_selectorOverlay->raise();
    m_selectorOverlay->activateWindow();

    connect(m_selectorOverlay, &TagSelectorOverlay::selectionChanged, this, [this](const QStringList& selected) {
        m_tags = selected;
        populateTagPills();
        emit tagsChanged(m_tags);
    });

    connect(m_selectorOverlay, &TagSelectorOverlay::overlayClosed, this, [this]() {
        m_selectorOverlay = nullptr;
    });
}

bool TagPillContainer::eventFilter(QObject* obj, QEvent* event) {
    if ((obj == this || isAncestorOf(qobject_cast<QWidget*>(obj))) &&
        (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)) {

        QWidget* targetWidget = qobject_cast<QWidget*>(obj);
        while (targetWidget && targetWidget != this) {
            if (qobject_cast<TagPill*>(targetWidget)) {
                return QFrame::eventFilter(obj, event);
            }
            targetWidget = targetWidget->parentWidget();
        }

        QMouseEvent* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            if (event->type() == QEvent::MouseButtonRelease) {
                onTagContainerClicked();
            }
            return true;
        }
    }
    return QFrame::eventFilter(obj, event);
}

} // namespace QuarkMeta
