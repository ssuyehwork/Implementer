#pragma once

#include "JustifiedView.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

namespace QuarkMeta {

class DropJustifiedView : public JustifiedView {
    Q_OBJECT
public:
    explicit DropJustifiedView(QWidget* parent = nullptr);


protected:
    void startDrag(Qt::DropActions supportedActions) override;
};

} // namespace QuarkMeta
