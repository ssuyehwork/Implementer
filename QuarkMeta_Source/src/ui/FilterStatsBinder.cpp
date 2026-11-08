#include "FilterStatsBinder.h"
#include "ActivePaneCoordinator.h"
#include "../core/CentralEventHub.h"

namespace QuarkMeta {

FilterStatsBinder::FilterStatsBinder(ActivePaneCoordinator* activeCoordinator,
                                     FilterPanel* filterPanel,
                                     MetaPanel* metaPanel,
                                     QObject* parent)
    : QObject(parent),
      m_activeCoordinator(activeCoordinator),
      m_filterPanel(filterPanel),
      m_metaPanel(metaPanel) {
}

void FilterStatsBinder::bindFilterToActivePanel(ContentPanel* activePanel) {
    if (!m_filterPanel || !activePanel) return;

    // 1. 严格解绑上一个窗格的统计信号，杜绝后台非焦点窗格冲刷侧边栏
    if (m_statsConnection) {
        QObject::disconnect(m_statsConnection);
    }

    // 2. 绑定新焦点窗格的统计就绪信号
    m_statsConnection = connect(activePanel, &ContentPanel::directoryStatsReady, m_filterPanel, [this](const ScanStats& stats) {
        if (m_filterPanel) {
            m_filterPanel->populateStats(stats);
            AppEvent ev;
            ev.type = AppEventType::FilterStateChanged;
            CentralEventHub::instance().publishEvent(ev);
        }
    });

    // 3. 切换焦点的瞬间：反向刷出新焦点窗格已有的 FilterState 与统计数据给 FilterPanel
    m_filterPanel->blockSignals(true);
    m_filterPanel->syncUIFromFilterState();
    m_filterPanel->blockSignals(false);

    // 触发当前焦点窗格重新计算并广播统计
    activePanel->recalculateAndEmitStats();
}

void FilterStatsBinder::wire() {
    if (m_filterPanel) {
        ContentPanel* root = m_activeCoordinator ? m_activeCoordinator->rootPane() : nullptr;
        if (root) {
            bindFilterToActivePanel(root);
        }

        // 筛选条件变动时精准应用至当前激活分栏
        connect(m_filterPanel, &FilterPanel::filterChanged, this, [this](const FilterState& state) {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                target->applyFilters(state);
            }
        });
    }

    if (m_metaPanel && m_filterPanel) {
        connect(m_metaPanel, &MetaPanel::searchByColor, m_filterPanel, [this](const QColor& color) {
            if (m_filterPanel) {
                m_filterPanel->selectColor(color);
            }
        });
    }
}

} // namespace QuarkMeta
