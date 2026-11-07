#pragma once

#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QColor>
#include <QString>

namespace QuarkMeta {

/**
 * @brief 全局唯一主题与样式管理器 (Single Source of Styling Truth)
 */
class ThemeManager : public QObject {
    Q_OBJECT

public:
    static ThemeManager& instance();

    /**
     * @brief 应用程序启动时一键注入全局暗黑主题
     */
    void initialize(QApplication* app);

    /**
     * @brief 为任意弹出菜单 (QMenu) 一键赋予标准的精致暗黑样式 (包括托盘菜单)
     */
    void applyMenuStyle(QWidget* menu) const;

    /**
     * @brief 获取权威的全局 QSS 样式表字符串
     */
    QString getGlobalStyleSheet() const;

private:
    explicit ThemeManager(QObject* parent = nullptr);
    ~ThemeManager() override = default;
};

} // namespace QuarkMeta
