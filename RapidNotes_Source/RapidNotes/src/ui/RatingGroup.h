#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QMap>
#include <functional>
#include "FilterStateModel.h"

namespace QuarkMeta {

class RatingGroup {
public:
    using AddFilterRowFunc = std::function<QCheckBox*(QVBoxLayout* layout, const QString& label, int count)>;

    static void populate(QWidget* parentWidget,
                         QVBoxLayout* contentLayout,
                         FilterStateModel* filterModel,
                         const QMap<int, int>& ratingCounts,
                         const FilterState& currentState,
                         AddFilterRowFunc addFilterRow);
};

} // namespace QuarkMeta
