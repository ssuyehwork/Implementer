#include "NavBarWidget.h"
#include "AddressBar.h"
#include "SearchController.h"
#include "HoverEventFilter.h"
#include "UiHelper.h"
#include "../core/NavigationService.h"

#include <QResizeEvent>

namespace QuarkMeta {

constexpr int kLayoutEdgeMargin = 5;
constexpr int kSearchBoxWidth = 230;

NavBarWidget::NavBarWidget(QWidget* parent, HoverEventFilter* hoverFilter)
    : QWidget(parent) {
    setObjectName("NavBar");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(42);
    initUi(hoverFilter);
}

void NavBarWidget::initUi(HoverEventFilter* hoverFilter) {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(kLayoutEdgeMargin, 2, kLayoutEdgeMargin, 2);
    m_mainLayout->setSpacing(2);

    m_row1Widget = new QWidget(this);
    m_row1Layout = new QHBoxLayout(m_row1Widget);
    m_row1Layout->setContentsMargins(0, 0, 0, 0);
    m_row1Layout->setSpacing(5);
    m_row1Layout->setAlignment(Qt::AlignVCenter);

    auto createBtn = [this, hoverFilter](const QString& iconKey, const QString& tip) -> QPushButton* {
        QPushButton* btn = new QPushButton(this);
        btn->setAttribute(Qt::WA_Hover);
        btn->setFixedSize(32, 28);
        btn->setIcon(UiHelper::getIcon(iconKey, QColor("#EEEEEE")));
        btn->setIconSize(QSize(18, 18));
        btn->setProperty("tooltipText", tip);
        btn->setObjectName("NavControlBtn");
        if (hoverFilter) {
            btn->installEventFilter(hoverFilter);
        }
        return btn;
    };

    m_btnBack = createBtn("nav_prev", "后退");
    m_btnForward = createBtn("nav_next", "前进");
    m_btnUp = createBtn("arrow_up", "上级");

    connect(m_btnBack, &QPushButton::clicked, &NavigationService::instance(), &NavigationService::goBack);
    connect(m_btnForward, &QPushButton::clicked, &NavigationService::instance(), &NavigationService::goForward);
    connect(m_btnUp, &QPushButton::clicked, &NavigationService::instance(), &NavigationService::goUp);

    connect(&NavigationService::instance(), &NavigationService::navStateChanged, this,
            [this](bool canBack, bool canForward, bool canUp) {
        m_btnBack->setEnabled(canBack);
        m_btnForward->setEnabled(canForward);
        m_btnUp->setEnabled(canUp);
    });

    m_addressBar = new AddressBar(this);
    m_addressBar->setMinimumWidth(80);

    m_searchController = new SearchController(this);

    m_row1Layout->addWidget(m_btnBack);
    m_row1Layout->addWidget(m_btnForward);
    m_row1Layout->addWidget(m_btnUp);
    m_row1Layout->addWidget(m_addressBar, 1);

    if (m_searchController && m_searchController->toolbarWidget()) {
        m_searchController->toolbarWidget()->setFixedWidth(kSearchBoxWidth);
        m_row1Layout->addWidget(m_searchController->toolbarWidget());
    }

    m_mainLayout->addWidget(m_row1Widget);
}

void NavBarWidget::updateResponsiveLayout() {
    if (!m_searchController || !m_searchController->toolbarWidget()) return;

    QWidget* searchW = m_searchController->toolbarWidget();

    bool needTwoRow = (width() < 650);

    if (needTwoRow && !m_isTwoRowMode) {
        m_isTwoRowMode = true;
        m_row1Layout->removeWidget(searchW);
        m_mainLayout->addWidget(searchW);
        searchW->setMinimumWidth(0);
        searchW->setMaximumWidth(QWIDGETSIZE_MAX);
        searchW->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setFixedHeight(78);
    } else if (!needTwoRow && m_isTwoRowMode) {
        m_isTwoRowMode = false;
        m_mainLayout->removeWidget(searchW);
        m_row1Layout->addWidget(searchW);
        searchW->setFixedWidth(kSearchBoxWidth);
        searchW->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        setFixedHeight(42);
    }
}

void NavBarWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateResponsiveLayout();
}

} // namespace QuarkMeta