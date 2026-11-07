#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

namespace QuarkMeta {

class AddressBar;
class SearchController;
class HoverEventFilter;

/**
 * @brief 顶部导航栏与搜索控制独立组件
 * 封装后退/前进/向上按钮、地址栏、搜索框以及窄屏(宽度<650px)时的响应式折行逻辑
 */
class NavBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit NavBarWidget(QWidget* parent = nullptr, HoverEventFilter* hoverFilter = nullptr);
    ~NavBarWidget() override = default;

    AddressBar* addressBar() const { return m_addressBar; }
    SearchController* searchController() const { return m_searchController; }

    QPushButton* backButton() const { return m_btnBack; }
    QPushButton* forwardButton() const { return m_btnForward; }
    QPushButton* upButton() const { return m_btnUp; }

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void initUi(HoverEventFilter* hoverFilter);
    void updateResponsiveLayout();

    QVBoxLayout* m_mainLayout = nullptr;
    QWidget* m_row1Widget = nullptr;
    QHBoxLayout* m_row1Layout = nullptr;

    QPushButton* m_btnBack = nullptr;
    QPushButton* m_btnForward = nullptr;
    QPushButton* m_btnUp = nullptr;

    AddressBar* m_addressBar = nullptr;
    SearchController* m_searchController = nullptr;

    bool m_isTwoRowMode = false;
};

} // namespace QuarkMeta
