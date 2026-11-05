#include "PanelLayoutManager.h"
#include <QSettings>

namespace RapidNotes {

PanelLayoutManager::PanelLayoutManager(QMainWindow* mainWindow,
                                       QSplitter* mainSplitter,
                                       QWidget* sidebarPanel,
                                       QWidget* contentPanel,
                                       QWidget* detailPanel,
                                       QObject* parent)
    : QObject(parent),
      m_mainWindow(mainWindow),
      m_mainSplitter(mainSplitter),
      m_sidebarPanel(sidebarPanel),
      m_contentPanel(contentPanel),
      m_detailPanel(detailPanel) {
}

void PanelLayoutManager::initLayout() {
    if (!m_mainSplitter) return;

    m_mainSplitter->setHandleWidth(1);
    m_mainSplitter->setStyleSheet("QSplitter::handle { background-color: #2D2D2D; }");
    m_mainSplitter->setChildrenCollapsible(false);

    resetSplitterLayout();
}

void PanelLayoutManager::resetSplitterLayout() {
    if (!m_mainSplitter) return;

    QList<int> defaultSizes;
    defaultSizes << 240 << 760;
    if (m_detailPanel) defaultSizes << 320;
    m_mainSplitter->setSizes(defaultSizes);
}

void PanelLayoutManager::setPanelVisible(const QString& panelId, bool visible) {
    QWidget* target = nullptr;
    if (panelId == "sidebar") target = m_sidebarPanel.data();
    else if (panelId == "content") target = m_contentPanel.data();
    else if (panelId == "detail") target = m_detailPanel.data();

    if (target) {
        target->setVisible(visible);
        emit panelVisibilityChanged(panelId, visible);
    }
}

bool PanelLayoutManager::isPanelVisible(const QString& panelId) const {
    if (panelId == "sidebar" && m_sidebarPanel) return m_sidebarPanel->isVisible();
    if (panelId == "content" && m_contentPanel) return m_contentPanel->isVisible();
    if (panelId == "detail" && m_detailPanel) return m_detailPanel->isVisible();
    return false;
}

void PanelLayoutManager::saveLayoutState() {
    if (!m_mainSplitter) return;
    QSettings settings("RapidNotes", "WindowStates");
    settings.setValue("MainSplitterState", m_mainSplitter->saveState());
}

void PanelLayoutManager::restoreLayoutState() {
    if (!m_mainSplitter) return;
    QSettings settings("RapidNotes", "WindowStates");
    QByteArray state = settings.value("MainSplitterState").toByteArray();
    if (!state.isEmpty()) {
        m_mainSplitter->restoreState(state);
    } else {
        resetSplitterLayout();
    }
}

} // namespace RapidNotes
