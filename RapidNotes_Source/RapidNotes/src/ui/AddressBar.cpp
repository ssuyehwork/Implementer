#include "AddressBar.h"
#include "UiHelper.h"
#include "ToolTipOverlay.h"
#include "StyleLibrary.h"
#include "../core/NavigationHistoryService.h"
#include "../core/NavigationService.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>
#include <QApplication>

namespace RapidNotes {

AddressBar::AddressBar(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_addressContainer = new QWidget(this);
    m_addressContainer->setObjectName("AddressContainer");
    m_addressContainer->setFixedHeight(32);

    QHBoxLayout* containerLayout = new QHBoxLayout(m_addressContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    m_breadcrumbBar = new BreadcrumbBar(m_addressContainer);
    m_breadcrumbBar->setFixedHeight(30);
    m_breadcrumbBar->setObjectName("AddressBreadcrumbBar");

    m_btnRefresh = new QPushButton(m_addressContainer);
    m_btnRefresh->setFixedSize(30, 30);
    m_btnRefresh->setIcon(UiHelper::getIcon("refresh", QColor("#CCCCCC"), 16));
    m_btnRefresh->setProperty("tooltipText", "刷新 (F5)");
    m_btnRefresh->setCursor(Qt::ArrowCursor);
    m_btnRefresh->setObjectName("BtnRefreshAddress");
    m_btnRefresh->setAttribute(Qt::WA_Hover);
    m_btnRefresh->installEventFilter(this);

    containerLayout->addWidget(m_breadcrumbBar, 1);
    containerLayout->addWidget(m_btnRefresh);

    layout->addWidget(m_addressContainer);

    connect(m_btnRefresh, &QPushButton::clicked, this, &AddressBar::refreshRequested);
    connect(m_breadcrumbBar, &BreadcrumbBar::blankAreaClicked, this, &AddressBar::onBreadcrumbBlankClicked);
    connect(m_breadcrumbBar, &BreadcrumbBar::pathClicked, this, &AddressBar::onBreadcrumbClicked);

    m_breadcrumbBar->setAttribute(Qt::WA_Hover);
    m_breadcrumbBar->installEventFilter(this);
    m_addressContainer->installEventFilter(this);

    m_historyPanel = new AddressHistoryPanel(this);
    connect(m_historyPanel, &AddressHistoryPanel::historyItemClicked, this, [this](const QString& path) {
        emit pathChanged(path);
        m_historyPanel->hide();
    });
}

void AddressBar::setPath(const QString& path) {
    m_currentPath = path;
    m_breadcrumbBar->setPath(path);
    NavigationHistoryService::instance().appendPath(path);
}

void AddressBar::onBreadcrumbBlankClicked() {
    QStringList history = NavigationHistoryService::instance().getHistory();
    if (!history.isEmpty()) {
        m_historyPanel->setHistory(history);
        m_historyPanel->showBelow(m_addressContainer);
    }
}

void AddressBar::onBreadcrumbClicked(const QString& path) {
    emit pathChanged(path);
}

bool AddressBar::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_btnRefresh) {
        if (event->type() == QEvent::HoverEnter || event->type() == QEvent::Enter) {
            m_btnRefresh->setIcon(UiHelper::getIcon("refresh", Qt::white, 16));
            QString text = m_btnRefresh->property("tooltipText").toString();
            if (!text.isEmpty()) {
                ToolTipOverlay::instance()->showText(QCursor::pos(), text, 0);
            }
        } else if (event->type() == QEvent::HoverLeave || event->type() == QEvent::Leave) {
            m_btnRefresh->setIcon(UiHelper::getIcon("refresh", QColor("#CCCCCC"), 16));
            ToolTipOverlay::hideTip();
        }
    }

    if ((obj == m_addressContainer || obj == m_breadcrumbBar) &&
        event->type() == QEvent::MouseButtonDblClick) {
        
        QStringList history = NavigationHistoryService::instance().getHistory();
        if (!history.isEmpty()) {
            m_historyPanel->setHistory(history);
            m_historyPanel->showBelow(m_addressContainer);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

} // namespace RapidNotes
