#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QMap>
#include <QStringList>
#include <functional>
#include "FilterStateModel.h"

namespace QuarkMeta {

class FileTypeGroup {
public:
    using AddFilterRowFunc = std::function<QCheckBox*(QVBoxLayout* layout, const QString& label, int count)>;
    using SaveFilterHistoryFunc = std::function<void(const QString& key, const QString& text)>;

    static QLineEdit* populate(QObject* owner,
                               QWidget* groupWidget,
                               QVBoxLayout* contentLayout,
                               FilterStateModel* filterModel,
                               const FilterState& currentState,
                               int emptyFolderCount,
                               const QMap<QString, int>& typeCounts,
                               AddFilterRowFunc addFilterRow,
                               SaveFilterHistoryFunc saveHistory);
};

} // namespace QuarkMeta
