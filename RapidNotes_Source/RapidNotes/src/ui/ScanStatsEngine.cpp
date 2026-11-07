#include "ScanStatsEngine.h"

namespace QuarkMeta {

ScanStatsEngine::ScanStatsEngine(QObject* parent) : QObject(parent) {}

void ScanStatsEngine::updateStats(const QuarkMeta::ScanStats& stats) {
    m_stats = stats;
    emit statsUpdated(m_stats);
}

} // namespace QuarkMeta
