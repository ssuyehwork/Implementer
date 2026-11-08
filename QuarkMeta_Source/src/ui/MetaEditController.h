#pragma once

#include <QObject>
#include <QPointer>
#include "MetaPanel.h"

namespace QuarkMeta {

class ActivePaneCoordinator;

/**
 * @brief 元数据面板编辑操作控制器
 * 负责响应 MetaPanel 的评分、颜色、预设标签、备注、链接与重命名信号并翻译为 CoreEngine 命令。
 */
class MetaEditController : public QObject {
    Q_OBJECT
public:
    explicit MetaEditController(ActivePaneCoordinator* activeCoordinator, MetaPanel* metaPanel, QObject* parent = nullptr);
    ~MetaEditController() override = default;

    void wire();

private:
    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<MetaPanel> m_metaPanel;
};

} // namespace QuarkMeta
