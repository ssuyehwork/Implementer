#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <functional>
#include "FilterStateModel.h"

namespace QuarkMeta {

class ModifyDateGroup {
public:
    using RebuildDateCheckboxesFunc = std::function<void(bool isCreateDate, bool descending)>;
    using SaveFilterHistoryFunc = std::function<void(const QString& key, const QString& text)>;

    static QLineEdit* populate(QObject* owner,
                               QWidget* groupWidget,
                               QHBoxLayout* headerLayout,
                               QVBoxLayout* contentLayout,
                               FilterStateModel* filterModel,
                               const FilterState& currentState,
                               bool& descending,
                               RebuildDateCheckboxesFunc rebuildCheckboxes,
                               SaveFilterHistoryFunc saveHistory);
};

} // namespace QuarkMeta
