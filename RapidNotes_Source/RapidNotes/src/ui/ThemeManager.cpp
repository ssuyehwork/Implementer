#include "ThemeManager.h"
#include "UiHelper.h"
#include <QFile>
#include <QDebug>

namespace QuarkMeta {

ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager(QObject* parent) : QObject(parent) {}

void ThemeManager::initialize(QApplication* app) {
    if (!app) return;
    app->setStyleSheet(getGlobalStyleSheet());
}

QString ThemeManager::getGlobalStyleSheet() const {
    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly)) {
        QString content = QLatin1String(file.readAll());

        // 从 SvgIcons 统一动态提取图标，彻底解除对物理 Icon/*.svg 文件的依赖
        QString comboArrowPath = UiHelper::getSvgTempFilePath("chevron_down", QColor("#AAAAAA"));
        QString checkPath = UiHelper::getSvgTempFilePath("check", QColor("#378ADD"));
        QString treeArrowRight = UiHelper::getSvgTempFilePath("chevron_right", QColor("#378ADD"));
        QString treeArrowDown = UiHelper::getSvgTempFilePath("chevron_down", QColor("#378ADD"));

        content += QString(
            "\nQComboBox::down-arrow { image: url(\"%1\"); }\n"
            "QCheckBox::indicator:checked { image: url(\"%2\"); }\n"
            "QTreeView::branch:has-children:closed { image: url(\"%3\"); }\n"
            "QTreeView::branch:has-children:open { image: url(\"%4\"); }\n"
        ).arg(comboArrowPath, checkPath, treeArrowRight, treeArrowDown);

        return content;
    }

    return QString();
}

void ThemeManager::applyMenuStyle(QWidget* menu) const {
    if (!menu) return;
    menu->setAttribute(Qt::WA_TranslucentBackground, true);
    menu->setWindowFlags(menu->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    menu->setStyleSheet(
        "QMenu {"
        "   background-color: #252526;"
        "   color: #EEEEEE;"
        "   border: 1px solid #333333;"
        "   border-radius: 6px;"
        "   padding: 4px;"
        "}"
        "QMenu::icon {"
        "   margin-right: 10px;"
        "}"
        "QMenu::item {"
        "   background-color: transparent;"
        "   color: #EEEEEE;"
        "   padding: 6px 24px 6px 12px;"
        "   border-radius: 4px;"
        "   font-size: 12px;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #3E3E42;"
        "   color: #FFFFFF;"
        "}"
        "QMenu::item:disabled {"
        "   color: #666666;"
        "   background-color: transparent;"
        "}"
        "QMenu::separator {"
        "   height: 1px;"
        "   background-color: #333333;"
        "   margin: 4px 6px;"
        "}"
    );
}

} // namespace QuarkMeta