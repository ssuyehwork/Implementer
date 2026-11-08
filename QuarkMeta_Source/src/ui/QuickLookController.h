#pragma once

#include <QObject>
#include <QString>
#include <QPointer>
#include "ContentPanel.h"
#include "MetaPanel.h"

namespace QuarkMeta {

class ActivePaneCoordinator;

/**
 * @brief QuickLook 预览联动控制器
 * 负责文件双击/激活响应、前后切换相邻可预览文件（单一实现）与预览中删除流程。
 */
class QuickLookController : public QObject {
    Q_OBJECT
public:
    explicit QuickLookController(ActivePaneCoordinator* activeCoordinator,
                                MetaPanel* metaPanel = nullptr,
                                QObject* parent = nullptr);
    ~QuickLookController() override = default;

    void bindPane(ContentPanel* pane);
    void wire();

private:
    QString findAdjacentPreviewable(ContentPanel* target, const QString& currentPath, int direction);

    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<MetaPanel> m_metaPanel;
};

} // namespace QuarkMeta
