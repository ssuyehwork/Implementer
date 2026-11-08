#pragma once

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QSet>
#include <QMap>
#include "ContentPanel.h"
#include "MetaPanel.h"
#include "../core/CentralEventHub.h"

namespace QuarkMeta {

class ActivePaneCoordinator;

/**
 * @brief 元数据面板展示与同步协调器
 * 负责 updateMetaPanelFromPanel、30ms防抖、多选两阶段性能优化与全局元数据变更事件响应。
 */
class MetaPanelPresenter : public QObject {
    Q_OBJECT
public:
    explicit MetaPanelPresenter(ActivePaneCoordinator* activeCoordinator, MetaPanel* metaPanel, ContentPanel* rootPanel, QObject* parent = nullptr);
    ~MetaPanelPresenter() override = default;

    void updateMetaPanel(ContentPanel* panel);
    void bindPane(ContentPanel* pane);
    void wire();

public slots:
    void handleEvent(const QuarkMeta::AppEvent& event);

private:
    void renderEmptySelection();
    void renderSingleSelection(ContentPanel* panel, const QString& path);
    void renderMultiSelection(ContentPanel* panel, const QStringList& paths);

    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<MetaPanel> m_metaPanel;
    QPointer<ContentPanel> m_rootPanel;

    struct DebounceState {
        QTimer* timer = nullptr;
        QSet<QString> pendingCleanPaths;
    };
    QMap<ContentPanel*, DebounceState> m_debounceMap;
};

} // namespace QuarkMeta
