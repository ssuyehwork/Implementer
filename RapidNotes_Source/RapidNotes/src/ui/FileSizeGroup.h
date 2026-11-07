#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include "FilterStateModel.h"

namespace QuarkMeta {

class FileSizeGroup {
public:
    static void populate(QWidget* parentWidget,
                         QVBoxLayout* contentLayout,
                         FilterStateModel* filterModel);
};

} // namespace QuarkMeta
