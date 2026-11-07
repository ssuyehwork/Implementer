#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>

namespace QuarkMeta {

/**
 * @brief 扩展与组件管理工具栏组件
 */
class DriveBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit DriveBarWidget(QWidget* parent = nullptr);
    ~DriveBarWidget() override = default;

    QPushButton* tagManagerButton() const { return m_btnTagManager; }
    QPushButton* textExtensionButton() const { return m_btnTextExtension; }
    QPushButton* saveThumbnailButton() const { return m_btnSaveThumbnail; }
    QPushButton* extensionManagerButton() const { return m_btnExtensionManager; }
    QHBoxLayout* driveBarLayout() const { return m_driveBarLayout; }

    void refreshPinnedButtons();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void initUi();
    void setupExtensionMenu();
    QPushButton* createIconButton(const QString& iconKey, const QColor& color, const QString& tooltipText);

    QHBoxLayout* m_driveBarLayout = nullptr;
    QPushButton* m_btnExtensionManager = nullptr;
    QPushButton* m_btnTagManager = nullptr;
    QPushButton* m_btnTextExtension = nullptr;
    QPushButton* m_btnSaveThumbnail = nullptr;
};

} // namespace QuarkMeta
