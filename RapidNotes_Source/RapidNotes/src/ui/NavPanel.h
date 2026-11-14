#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>
#include <QTabBar>
#include <QStackedWidget>

namespace RapidNotes {

/**
 * @brief 导航面板（面板一）
 * 遵循纯文本笔记架构标准，保留五栏区 Tab 框架与契约占位，作为空壳 (TODO) 运行
 */
class NavPanel : public QFrame {
    Q_OBJECT

public:
    explicit NavPanel(QWidget* parent = nullptr);
    ~NavPanel() override = default;

    bool eventFilter(QObject* watched, QEvent* event) override;

    // 🚀【物理契约】：刚性锁定 230px 下限，杜绝被 QSplitter 挤压偷扣像素
    QSize minimumSizeHint() const override { return QSize(230, 100); }

    void deferredInit();
    void setFocusHighlight(bool visible);
    void setRootPath(const QString& path);
    void selectPath(const QString& path);

signals:
    void directorySelected(const QString& path);
    void requestLocateFile(const QString& path);
    void requestOpenTrash();

private:
    void initUi();
    QWidget* createPlaceholderPage(const QString& iconName, const QString& title, const QString& subtitle);

    QVBoxLayout* m_mainLayout = nullptr;
    QTabBar* m_tabBar = nullptr;
    QStackedWidget* m_stackedWidget = nullptr;
};

} // namespace RapidNotes