#include "CategoryPaneSplitManager.h"
#include <QVBoxLayout>

namespace RapidNotes {

CategoryPaneSplitManager::CategoryPaneSplitManager(QWidget* containerWidget, QObject* parent)
    : QObject(parent), m_containerWidget(containerWidget) {
    if (m_containerWidget) {
        auto* layout = new QVBoxLayout(m_containerWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        m_splitter = new QSplitter(m_orientation, m_containerWidget);
        m_splitter->setHandleWidth(1);
        m_splitter->setStyleSheet("QSplitter::handle { background-color: #2D2D2D; }");
        m_splitter->setChildrenCollapsible(false);
        layout->addWidget(m_splitter);
    }
}

void CategoryPaneSplitManager::setActivePane(QWidget* pane) {
    if (m_activePane != pane && m_panes.contains(pane)) {
        m_activePane = pane;
        emit activePaneChanged(pane);
    }
}

void CategoryPaneSplitManager::split(Qt::Orientation orientation, int newCategoryId, const QString& title) {
    Q_UNUSED(newCategoryId);
    Q_UNUSED(title);
    if (!m_splitter) return;

    m_orientation = orientation;
    m_splitter->setOrientation(orientation);

    // 默认最多支持两栏并排
    if (m_panes.size() >= 2) return;

    auto* newPane = new QWidget(m_splitter);
    newPane->setObjectName(QString("CategoryPane_%1").arg(m_panes.size() + 1));
    m_splitter->addWidget(newPane);
    m_panes.append(newPane);

    setActivePane(newPane);
    emit paneCountChanged(m_panes.size());
}

void CategoryPaneSplitManager::closeSecondaryPanes() {
    if (m_panes.size() <= 1) return;

    while (m_panes.size() > 1) {
        QWidget* secondary = m_panes.takeLast();
        if (secondary) secondary->deleteLater();
    }

    if (!m_panes.isEmpty()) {
        setActivePane(m_panes.first());
    }
    emit paneCountChanged(m_panes.size());
}

CategoryTabSplitState CategoryPaneSplitManager::captureSplitState() const {
    CategoryTabSplitState state;
    state.isSplit = isSplitMode();
    state.orientation = m_orientation;
    for (int i = 0; i < m_panes.size(); ++i) {
        if (m_panes[i] == m_activePane) {
            state.activePaneIndex = i;
        }
    }
    return state;
}

void CategoryPaneSplitManager::restoreSplitState(const CategoryTabSplitState& state) {
    if (!state.isSplit) {
        closeSecondaryPanes();
        return;
    }

    m_orientation = state.orientation;
    if (m_splitter) m_splitter->setOrientation(m_orientation);

    if (m_panes.size() < 2) {
        split(m_orientation, -1, "");
    }

    if (state.activePaneIndex >= 0 && state.activePaneIndex < m_panes.size()) {
        setActivePane(m_panes[state.activePaneIndex]);
    }
}

} // namespace RapidNotes
