#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <functional>
#include "FilterStateModel.h"
#include "ScanStatsEngine.h"

namespace QuarkMeta {

class LinkStatusGroup {
public:
    using AddFilterRowFunc = std::function<QCheckBox*(QVBoxLayout* layout, const QString& label, int count, const QString& rowKey)>;

    static void populate(QWidget* parentWidget,
                         QVBoxLayout* contentLayout,
                         FilterStateModel* filterModel,
                         const QuarkMeta::ScanStats& currentStats,
                         const FilterState& currentState,
                         AddFilterRowFunc addFilterRow);
};

} // namespace QuarkMeta
