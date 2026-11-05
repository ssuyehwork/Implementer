#include "CategoryTabBarWidget.h"
#include "IconHelper.h"
#include <QScrollBar>
#include <QPainter>
#include <QStyle>

namespace RapidNotes {

CategoryTabBarWidget::CategoryTabBarWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("CategoryTabBarWidget");
    setFixedHeight(36);

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(4, 2, 4, 2);
    mainLayout->setSpacing(4);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet("background: transparent; border: none;");

    m_scrollContainer = new QWidget();
    m_scrollContainer->setStyleSheet("background: transparent; border: none;");
    m_tabLayout = new QHBoxLayout(m_scrollContainer);
    m_tabLayout->setContentsMargins(0, 0, 0, 0);
    m_tabLayout->setSpacing(4);

    m_scrollArea->setWidget(m_scrollContainer);
    mainLayout->addWidget(m_scrollArea, 1);

    m_btnNewTab = new QPushButton(this);
    m_btnNewTab->setObjectName("btnNewCategoryTab");
    m_btnNewTab->setFixedSize(28, 28);
    m_btnNewTab->setIcon(IconHelper::getIcon("add", "#888888", 16));
    m_btnNewTab->setToolTip("新建标签页");
    m_btnNewTab->setCursor(Qt::PointingHandCursor);
    m_btnNewTab->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 4px; } QPushButton:hover { background-color: rgba(255, 255, 255, 0.1); }");
    connect(m_btnNewTab, &QPushButton::clicked, this, &CategoryTabBarWidget::newTabRequested);
    mainLayout->addWidget(m_btnNewTab);
}

int CategoryTabBarWidget::currentCategoryId() const {
    for (const auto& tab : m_tabs) {
        if (tab.tabId == m_currentTabId) return tab.categoryId;
    }
    return -1;
}

int CategoryTabBarWidget::addTab(int categoryId, const QString& title, const QString& iconName, const QString& colorHex) {
    CategoryTabInfo tab;
    tab.tabId = m_nextTabId++;
    tab.categoryId = categoryId;
    tab.title = title.isEmpty() ? "全部笔记" : title;
    tab.iconName = iconName;
    tab.colorHex = colorHex.isEmpty() ? "#4a90e2" : colorHex;

    auto* btn = new QWidget(m_scrollContainer);
    btn->setObjectName("CategoryTabItem");
    btn->setFixedHeight(28);
    btn->setCursor(Qt::PointingHandCursor);
    btn->installEventFilter(this);

    auto* l = new QHBoxLayout(btn);
    l->setContentsMargins(8, 2, 6, 2);
    l->setSpacing(6);

    auto* iconLabel = new QLabel(btn);
    iconLabel->setPixmap(IconHelper::getIcon(tab.iconName, tab.colorHex, 14).pixmap(14, 14));
    l->addWidget(iconLabel);

    auto* titleLabel = new QLabel(tab.title, btn);
    titleLabel->setStyleSheet("color: #ccc; font-size: 12px;");
    l->addWidget(titleLabel);

    auto* closeBtn = new QPushButton(btn);
    closeBtn->setFixedSize(16, 16);
    closeBtn->setIcon(IconHelper::getIcon("close", "#777777", 10));
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 2px; } QPushButton:hover { background: #d9534f; color: white; }");
    int tid = tab.tabId;
    connect(closeBtn, &QPushButton::clicked, this, [this, tid]() {
        closeTab(tid);
    });
    l->addWidget(closeBtn);

    tab.tabButton = btn;
    tab.closeButton = closeBtn;
    tab.titleLabel = titleLabel;
    tab.iconLabel = iconLabel;

    m_tabs.append(tab);
    m_tabLayout->addWidget(btn);

    activateTab(tab.tabId);
    return tab.tabId;
}

void CategoryTabBarWidget::closeTab(int tabId) {
    if (m_tabs.size() <= 1) return; // 至少保留一个标签页

    int removeIdx = -1;
    for (int i = 0; i < m_tabs.size(); ++i) {
        if (m_tabs[i].tabId == tabId) {
            removeIdx = i;
            break;
        }
    }
    if (removeIdx == -1) return;

    CategoryTabInfo info = m_tabs.takeAt(removeIdx);
    if (info.tabButton) info.tabButton->deleteLater();

    emit tabClosed(tabId);

    if (m_currentTabId == tabId) {
        int nextIdx = qMin(removeIdx, m_tabs.size() - 1);
        if (nextIdx >= 0) activateTab(m_tabs[nextIdx].tabId);
    } else {
        updateTabStyles();
    }
}

void CategoryTabBarWidget::activateTab(int tabId) {
    if (m_currentTabId == tabId) return;

    m_currentTabId = tabId;
    int catId = -1;
    for (const auto& tab : m_tabs) {
        if (tab.tabId == tabId) {
            catId = tab.categoryId;
            break;
        }
    }

    updateTabStyles();
    ensureActiveTabVisible();
    emit tabChanged(tabId, catId);
}

void CategoryTabBarWidget::updateTabTitle(int tabId, const QString& title) {
    for (auto& tab : m_tabs) {
        if (tab.tabId == tabId) {
            tab.title = title;
            if (tab.titleLabel) tab.titleLabel->setText(title);
            break;
        }
    }
}

void CategoryTabBarWidget::updateTabCategory(int tabId, int categoryId, const QString& title) {
    for (auto& tab : m_tabs) {
        if (tab.tabId == tabId) {
            tab.categoryId = categoryId;
            tab.title = title;
            if (tab.titleLabel) tab.titleLabel->setText(title);
            break;
        }
    }
}

CategoryTabSplitState CategoryTabBarWidget::getTabSplitState(int tabId) const {
    for (const auto& tab : m_tabs) {
        if (tab.tabId == tabId) return tab.splitState;
    }
    return CategoryTabSplitState();
}

void CategoryTabBarWidget::setTabSplitState(int tabId, const CategoryTabSplitState& state) {
    for (auto& tab : m_tabs) {
        if (tab.tabId == tabId) {
            tab.splitState = state;
            break;
        }
    }
}

void CategoryTabBarWidget::updateTabStyles() {
    for (const auto& tab : m_tabs) {
        bool isActive = (tab.tabId == m_currentTabId);
        if (tab.tabButton) {
            if (isActive) {
                tab.tabButton->setStyleSheet("QWidget#CategoryTabItem { background: #2d2d2d; border-radius: 4px; border-bottom: 2px solid #4a90e2; }");
                if (tab.titleLabel) tab.titleLabel->setStyleSheet("color: #ffffff; font-weight: bold; font-size: 12px;");
            } else {
                tab.tabButton->setStyleSheet("QWidget#CategoryTabItem { background: #1e1e1e; border-radius: 4px; border: 1px solid #333333; } QWidget#CategoryTabItem:hover { background: #252526; }");
                if (tab.titleLabel) tab.titleLabel->setStyleSheet("color: #888888; font-size: 12px;");
            }
        }
    }
}

void CategoryTabBarWidget::ensureActiveTabVisible() {
    for (const auto& tab : m_tabs) {
        if (tab.tabId == m_currentTabId && tab.tabButton) {
            m_scrollArea->ensureWidgetVisible(tab.tabButton);
            break;
        }
    }
}

bool CategoryTabBarWidget::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        for (const auto& tab : m_tabs) {
            if (tab.tabButton == watched) {
                activateTab(tab.tabId);
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void CategoryTabBarWidget::wheelEvent(QWheelEvent* event) {
    if (m_scrollArea && m_scrollArea->horizontalScrollBar()) {
        int delta = event->angleDelta().y();
        m_scrollArea->horizontalScrollBar()->setValue(m_scrollArea->horizontalScrollBar()->value() - delta);
        event->accept();
        return;
    }
    QWidget::wheelEvent(event);
}

void CategoryTabBarWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    ensureActiveTabVisible();
}

} // namespace RapidNotes
