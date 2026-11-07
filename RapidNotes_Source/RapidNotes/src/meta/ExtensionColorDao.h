#pragma once
#include <QString>
#include <QColor>
#include <QPair>
#include <QMap>

namespace QuarkMeta {

class ExtensionColorDao {
public:
    static bool initTable();
    static bool getColorForExtension(const QString& ext, QColor& outBg, QColor& outText);
    static bool saveExtensionColor(const QString& ext, const QColor& bg, const QColor& text, bool isCustom = false);
    static QMap<QString, QPair<QColor, QColor>> loadAllColors();
};

} // namespace QuarkMeta
