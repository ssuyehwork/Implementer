#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>

namespace RapidNotes {

/**
 * @brief 快捷设置与扩展管理工具栏组件（原 DriveBarWidget 更名与重构）
 * 位于 TitleBarWidget 与 NavBarWidget 之间，提供标签管理、扩展名设置、存储位置设置等快捷入口
 */
class SettingsBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit SettingsBarWidget(QWidget* parent = nullptr);
    ~SettingsBarWidget() override = default;

    QPushButton* tagManagerButton() const { return m_btnTagManager; }
    QPushButton* textExtensionButton() const { return m_btnTextExtension; }
    QPushButton* cacheLocationButton() const { return m_btnCacheLocation; }
    QPushButton* extensionManagerButton() const { return m_btnExtensionManager; }
    QHBoxLayout* settingsBarLayout() const { return m_settingsBarLayout; }

    void refreshPinnedButtons();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void initUi();
    void setupExtensionMenu();
    QPushButton* createIconButton(const QString& iconKey, const QColor& color, const QString& tooltipText);

    QHBoxLayout* m_settingsBarLayout = nullptr;
    QPushButton* m_btnExtensionManager = nullptr;
    QPushButton* m_btnTagManager = nullptr;
    QPushButton* m_btnTextExtension = nullptr;
    QPushButton* m_btnCacheLocation = nullptr;
};

} // namespace RapidNotes
