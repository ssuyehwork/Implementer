#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QSvgRenderer>
#include <QColor>
#include <QRectF>
#include <QString>
#include <QMenu>
#include "SvgIcons.h"

namespace IconHelper {

inline QIcon getIcon(const QString& key, const QString& color, int size = 18) {
    QIcon icon;
    if (!SvgIcons::icons.contains(key)) return icon;

    QString svgData = SvgIcons::icons.value(key);
    svgData.replace("currentColor", color);

    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QSvgRenderer renderer(svgData.toUtf8());
    renderer.render(&painter, QRectF(0, 0, size, size));
    painter.end();

    icon.addPixmap(pixmap);
    return icon;
}

inline void setupMenu(QMenu* menu) {
    if (!menu) return;
    menu->setStyleSheet(
        "QMenu { padding: 4px; }"
        "QMenu::item { padding: 6px 24px 6px 28px; }"
        "QMenu::icon { left: 6px; }"
    );
}

} // namespace IconHelper

#endif // ICONHELPER_H
