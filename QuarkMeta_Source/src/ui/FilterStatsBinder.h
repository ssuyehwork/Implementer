#pragma once

#include <QObject>
#include <QPointer>
#include "FilterPanel.h"
#include "MetaPanel.h"
#include "ContentPanel.h"

namespace QuarkMeta {

class ActivePaneCoordinator;

/**
 * @brief 筛选与统计状态绑定器
 * 负责在窗格切换时断开旧连接、连接新窗格的统计信号、广播 FilterStateChanged 与同步筛选条件。
 */
class FilterStatsBinder : public QObject {
    Q_OBJECT
public:
    explicit FilterStatsBinder(ActivePaneCoordinator* activeCoordinator,
                               FilterPanel* filterPanel,
                               MetaPanel* metaPanel,
                               QObject* parent = nullptr);
    ~FilterStatsBinder() override = default;

    void bindFilterToActivePanel(ContentPanel* activePanel);
    void wire();

private:
    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<FilterPanel> m_filterPanel;
    QPointer<MetaPanel> m_metaPanel;
    QMetaObject::Connection m_statsConnection;
};

} // namespace QuarkMeta
