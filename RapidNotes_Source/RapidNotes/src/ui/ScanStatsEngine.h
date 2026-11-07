#pragma once

#include <QObject>
#include "ScanStats.h"

namespace QuarkMeta {

class ScanStatsEngine : public QObject {
    Q_OBJECT
public:
    explicit ScanStatsEngine(QObject* parent = nullptr);
    ~ScanStatsEngine() override = default;

    void updateStats(const QuarkMeta::ScanStats& stats);
    const QuarkMeta::ScanStats& currentStats() const { return m_stats; }

signals:
    void statsUpdated(const QuarkMeta::ScanStats& stats);

private:
    QuarkMeta::ScanStats m_stats;
};

} // namespace QuarkMeta
