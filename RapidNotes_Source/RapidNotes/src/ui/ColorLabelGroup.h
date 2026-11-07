#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QMap>
#include <QString>
#include <QColor>
#include <functional>
#include "FilterStateModel.h"

namespace QuarkMeta {

class ColorLabelGroup {
public:
    using AddFilterRowFunc = std::function<QCheckBox*(QVBoxLayout* layout, const QString& label, int count, const QColor& color, const QString& rowKey)>;

    static void populate(QWidget* parentWidget,
                         QVBoxLayout* contentLayout,
                         FilterStateModel* filterModel,
                         const QMap<QString, int>& colorCounts,
                         const FilterState& currentState,
                         AddFilterRowFunc addFilterRow);
};

} // namespace QuarkMeta
