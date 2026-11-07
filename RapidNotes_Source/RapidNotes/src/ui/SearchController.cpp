#include "SearchController.h"
#include "SearchHistoryPanel.h"
#include "ContentPanel.h"
#include "../core/SearchHistoryService.h"
#include "../meta/LibraryDao.h"
#include "UiHelper.h"
#include "StyleLibrary.h"
#include <QHBoxLayout>
#include <QStyle>
#include <QMenu>
#include <QActionGroup>

using namespace QuarkMeta::Style;

namespace QuarkMeta {

SearchController::SearchController(QWidget* parent)
    : QObject(parent) {
    m_searchContainer = new QWidget(parent);
    m_searchContainer->setObjectName("SearchContainer");
    m_searchContainer->setFixedHeight(32);

    QHBoxLayout* searchLayout = new QHBoxLayout(m_searchContainer);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(0);

    m_searchEdit = new QLineEdit(m_searchContainer);
    m_searchEdit->setObjectName("SearchEdit");
    updatePlaceholderText();
    m_searchEdit->setFixedHeight(30);
    m_clearAction = UiHelper::setupLineEditClearButton(m_searchEdit);
    m_searchEdit->installEventFilter(this);

    UiHelper::setupLineEditContextMenu(m_searchEdit);

    m_btnSearch = new QPushButton(m_searchContainer);
    m_btnSearch->setObjectName("BtnSearchAddress");
    m_btnSearch->setFixedSize(30, 30);
    m_btnSearch->setIcon(UiHelper::getIcon("seach-7", QColor("#CCCCCC"), 16));
    m_btnSearch->setIconSize(QSize(16, 16));
    m_btnSearch->setCursor(Qt::ArrowCursor);
    m_btnSearch->setProperty("tooltipText", "搜索范围与选项");
    m_btnSearch->setAttribute(Qt::WA_Hover);
    m_btnSearch->installEventFilter(this);

    connect(m_btnSearch, &QPushButton::clicked, this, &SearchController::showSearchMenu);

    searchLayout->addWidget(m_btnSearch, 0);
    searchLayout->addWidget(m_searchEdit, 1);

    m_searchHistoryPanel = new SearchHistoryPanel(parent);
    m_searchHistoryPanel->setCategory("global");
    m_searchHistoryPanel->setHistory(SearchHistoryService::instance().getHistory("global"));

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(300);
}

void SearchController::setActiveContentPanel(ContentPanel* panel) {
    m_contentPanel = panel;
}

void SearchController::bindContentPanel(ContentPanel* contentPanel) {
    setActiveContentPanel(contentPanel);
    if (!m_contentPanel) return;

    connect(m_searchEdit, &QLineEdit::returnPressed, this, [this]() {
        doSearch(m_searchEdit->text().trimmed());
    });

    connect(m_searchEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        if (m_isInternalClearing) return;
        if (text.isEmpty()) {
            m_searchTimer->stop();
            doSearch("");
        } else {
            m_searchTimer->start();
        }
    });

    connect(m_searchTimer, &QTimer::timeout, this, [this]() {
        doSearch(m_searchEdit->text().trimmed());
    });

    m_searchEdit->installEventFilter(this);

    connect(m_searchHistoryPanel, &SearchHistoryPanel::historyItemClicked, this, [this](QString keyword) {
        if (m_searchHistoryPanel) {
            m_searchHistoryPanel->hide();
        }
        m_searchEdit->setText(keyword);
        QMetaObject::invokeMethod(this, [this, keyword]() {
            doSearch(keyword);
        }, Qt::QueuedConnection);
    });
}

void SearchController::updatePlaceholderText() {
    if (!m_searchEdit) return;
    if (m_searchScope == SearchScope::Library) {
        m_searchEdit->setPlaceholderText("搜索：库");
    } else {
        m_searchEdit->setPlaceholderText("搜索：当前文件夹");
    }
}

void SearchController::showSearchMenu() {
    if (!m_btnSearch) return;

    QMenu menu(m_btnSearch);
    UiHelper::applyMenuStyle(&menu);

    QActionGroup* group = new QActionGroup(&menu);
    group->setExclusive(true);

    QAction* actFolder = menu.addAction("搜索：当前文件夹");
    actFolder->setCheckable(true);
    actFolder->setChecked(m_searchScope == SearchScope::CurrentFolder);
    group->addAction(actFolder);

    QAction* actLibrary = menu.addAction("搜索：库");
    actLibrary->setCheckable(true);
    actLibrary->setChecked(m_searchScope == SearchScope::Library);
    group->addAction(actLibrary);

    connect(actFolder, &QAction::triggered, this, [this]() {
        m_searchScope = SearchScope::CurrentFolder;
        updatePlaceholderText();
    });

    connect(actLibrary, &QAction::triggered, this, [this]() {
        m_searchScope = SearchScope::Library;
        updatePlaceholderText();
    });

    QPoint globalPos = m_btnSearch->mapToGlobal(QPoint(0, m_btnSearch->height()));
    menu.exec(globalPos);
}

void SearchController::clearSearch(bool silent) {
    if (!m_searchEdit) return;
    m_isInternalClearing = silent;
    m_searchEdit->clear();
    if (m_clearAction) {
        m_clearAction->setVisible(false);
    }
    m_isInternalClearing = false;
}

void SearchController::focusSearch() {
    if (!m_searchEdit) return;
    m_searchEdit->setFocus(Qt::ShortcutFocusReason);
    m_searchEdit->selectAll();
}

void SearchController::doSearch(const QString& keyword) {
    if (!m_contentPanel) return;

    // 1. 优先同步与重置 ContentPanel 内部的搜索关键词，消除残存过滤词
    m_contentPanel->search(keyword);

    if (m_searchScope == SearchScope::Library) {
        if (keyword.isEmpty()) {
            m_contentPanel->refreshAll();
        } else {
            LibraryDao::initTable();
            QStringList allLibraryPaths = LibraryDao::getCategoryPaths(-1);
            allLibraryPaths.removeDuplicates();

            m_contentPanel->loadPaths(allLibraryPaths);
            m_contentPanel->search(keyword);
        }
    }

    if (!keyword.isEmpty()) {
        SearchHistoryService::instance().appendSearch("global", keyword);
        if (m_searchHistoryPanel) {
            m_searchHistoryPanel->setHistory(SearchHistoryService::instance().getHistory("global"));
        }
    }
    if (m_searchHistoryPanel) {
        m_searchHistoryPanel->hide();
    }
    emit searchExecuted();
}

bool SearchController::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonDblClick && watched == m_searchEdit) {
        auto history = SearchHistoryService::instance().getHistory("global");
        if (!history.isEmpty()) {
            m_searchHistoryPanel->setHistory(history);
            m_searchHistoryPanel->showBelow(m_searchEdit);
        }
        return true;
    }

    if (watched == m_searchEdit) {
        if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
            m_searchContainer->setProperty("focused", event->type() == QEvent::FocusIn);
            m_searchContainer->style()->unpolish(m_searchContainer);
            m_searchContainer->style()->polish(m_searchContainer);
        }
    }

    if (watched == m_btnSearch) {
        if (event->type() == QEvent::Enter) {
            m_btnSearch->setIcon(UiHelper::getIcon("seach-7", Qt::white, 16));
        } else if (event->type() == QEvent::Leave) {
            m_btnSearch->setIcon(UiHelper::getIcon("seach-7", QColor("#CCCCCC"), 16));
        }
    }

    return QObject::eventFilter(watched, event);
}

} // namespace QuarkMeta