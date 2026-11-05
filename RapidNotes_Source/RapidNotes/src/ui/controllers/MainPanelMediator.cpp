#include "MainPanelMediator.h"
#include "CategoryPaneSplitManager.h"
#include "PanelLayoutManager.h"
#include "../CategoryTabBarWidget.h"

namespace RapidNotes {

MainPanelMediator::MainPanelMediator(CategoryTabBarWidget* tabBar,
                                     CategoryPaneSplitManager* splitManager,
                                     PanelLayoutManager* layoutManager,
                                     QObject* parent)
    : QObject(parent),
      m_tabBar(tabBar),
      m_splitManager(splitManager),
      m_layoutManager(layoutManager) {
}

void MainPanelMediator::setupConnections() {
    if (m_tabBar) {
        connect(m_tabBar, &CategoryTabBarWidget::tabChanged, this, &MainPanelMediator::onTabSwitched);
    }
}

void MainPanelMediator::onCategoryClicked(int categoryId) {
    if (m_tabBar) {
        m_tabBar->updateTabCategory(m_tabBar->currentTabId(), categoryId, "");
    }
    emit categorySelected(categoryId);
}

void MainPanelMediator::onSearchTextChanged(const QString& keyword) {
    emit searchRequested(keyword);
}

void MainPanelMediator::onTabSwitched(int tabId, int categoryId) {
    Q_UNUSED(tabId);
    emit categorySelected(categoryId);
}

} // namespace RapidNotes
