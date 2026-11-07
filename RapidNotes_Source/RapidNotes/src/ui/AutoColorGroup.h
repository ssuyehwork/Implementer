#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <functional>
#include <QStringList>
#include "FilterStateModel.h"

namespace QuarkMeta {

class AutoColorGroup {
public:
    static void populate(QWidget* parentWidget,
                         QVBoxLayout* contentLayout,
                         FilterStateModel* filterModel,
                         std::function<void()> requestRebuild);

    static void pushRecent(const QString& hex);
};

} // namespace QuarkMeta
