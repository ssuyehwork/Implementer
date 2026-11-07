#include "FilterStateModel.h"
#include "AppConfig.h"

namespace QuarkMeta {

FilterStateModel::FilterStateModel(QObject* parent) : QObject(parent) {
    m_state.colorPrecision = qBound(0, AppConfig::instance().getValue("Filter/ColorPrecision", 50).toInt(), 100);
    m_state.colorHue = qBound(0, AppConfig::instance().getValue("Filter/ColorHue", 0).toInt(), 359);
    m_state.minColorArea = AppConfig::instance().getValue("Filter/MinColorArea", 0).toInt();
}

void FilterStateModel::setState(const FilterState& state) {
    m_state = state;
    emit stateChanged(m_state);
}

void FilterStateModel::reset(bool force) {
    Q_UNUSED(force);
    FilterState defaultState;
    defaultState.colorPrecision = m_state.colorPrecision;
    defaultState.colorHue = m_state.colorHue;
    defaultState.minColorArea = m_state.minColorArea;
    m_state = defaultState;
    emit stateChanged(m_state);
}

} // namespace QuarkMeta
