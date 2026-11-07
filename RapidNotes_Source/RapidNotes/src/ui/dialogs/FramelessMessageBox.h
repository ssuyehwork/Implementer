#pragma once
#include <QString>
#include <QWidget>

namespace QuarkMeta {

class FramelessMessageBox {
public:
    static void information(QWidget* parent, const QString& title, const QString& text);
    static void warning(QWidget* parent, const QString& title, const QString& text);
    static bool question(QWidget* parent, const QString& title, const QString& text);
    static void critical(QWidget* parent, const QString& title, const QString& text);
};

} // namespace QuarkMeta
