#pragma once

#include <QObject>
#include <QPointer>
#include <QString>
#include "NavPanel.h"
#include "FavoritePanel.h"
#include "LibraryPanel.h"
#include "MarkedPanel.h"
#include "AddressBar.h"
#include "TabBarWidget.h"
#include "SearchController.h"
#include "ContentPanel.h"

namespace QuarkMeta {

class ActivePaneCoordinator;

/**
 * @brief 导航与全局路径协同协调器
 * 负责全应用位置显示（单一实现）、路径导航跳转、搜索联动与多面板路径联动。
 */
class NavigationCoordinator : public QObject {
    Q_OBJECT
public:
    NavigationCoordinator(ActivePaneCoordinator* activeCoordinator,
                          NavPanel* navPanel,
                          FavoritePanel* favoritePanel,
                          LibraryPanel* libraryPanel,
                          MarkedPanel* markedPanel,
                          AddressBar* addressBar,
                          TabBarWidget* tabBar,
                          SearchController* searchController,
                          QObject* parent = nullptr);
    ~NavigationCoordinator() override = default;

    void showLocation(const QString& addressText, const QString& tabTitle, const QString& tabUrl);
    void locateItem(const QString& fullPath);
    void refreshCurrent();

    void bindPane(ContentPanel* pane);
    void wire();

signals:
    void statusMessageRequested(const QString& message);

private:
    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<NavPanel> m_navPanel;
    QPointer<FavoritePanel> m_favoritePanel;
    QPointer<LibraryPanel> m_libraryPanel;
    QPointer<MarkedPanel> m_markedPanel;
    QPointer<AddressBar> m_addressBar;
    QPointer<TabBarWidget> m_tabBar;
    QPointer<SearchController> m_searchController;
};

} // namespace QuarkMeta
