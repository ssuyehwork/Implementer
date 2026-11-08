#pragma once

#include <QObject>
#include <QPointer>
#include <functional>
#include "TabBarWidget.h"
#include "ContentPanel.h"

namespace QuarkMeta {

class ActivePaneCoordinator;

/**
 * @brief 标签栏协同协调器
 * 负责多标签页生命周期、分屏状态导出保存（单一实现）、标签页切换导航与属性变更。
 */
class TabBarCoordinator : public QObject {
    Q_OBJECT
public:
    explicit TabBarCoordinator(ActivePaneCoordinator* activeCoordinator,
                             TabBarWidget* tabBar,
                             const std::function<void()>& refreshCurrentFunc,
                             QObject* parent = nullptr);
    ~TabBarCoordinator() override = default;

    void wire();

private:
    void exportAndSaveSplitState();

    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<TabBarWidget> m_tabBar;
    std::function<void()> m_refreshCurrentFunc;
};

} // namespace QuarkMeta
