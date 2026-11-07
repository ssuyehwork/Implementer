#pragma once

#include <QObject>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QWidget>
#include <QEvent>
#include <QPointer>
#include "ContentPanel.h"

namespace QuarkMeta {

class SearchHistoryPanel;

enum class SearchScope {
    CurrentFolder,
    Library
};

class SearchController : public QObject {
    Q_OBJECT
public:
    explicit SearchController(QWidget* parent = nullptr);
    ~SearchController() override = default;

    QWidget* toolbarWidget() const { return m_searchContainer; }
    QLineEdit* searchEdit() const { return m_searchEdit; }
    QPushButton* searchButton() const { return m_btnSearch; }

    // 搜索框 SSOT 统一控制接口
    void clearSearch(bool silent = false);
    void focusSearch();
    SearchHistoryPanel* historyPanel() const { return m_searchHistoryPanel; }
    SearchScope searchScope() const { return m_searchScope; }

    void bindContentPanel(ContentPanel* contentPanel);
    // 仅切换当前搜索目标窗格，不重复接线 UI 信号；供多窗格激活切换时调用
    void setActiveContentPanel(ContentPanel* panel);

signals:
    void searchExecuted();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void doSearch(const QString& keyword);
    void showSearchMenu();
    void updatePlaceholderText();

    QWidget* m_searchContainer = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QPushButton* m_btnSearch = nullptr;
    QAction* m_clearAction = nullptr;
    bool m_isInternalClearing = false;
    QTimer* m_searchTimer = nullptr;
    SearchHistoryPanel* m_searchHistoryPanel = nullptr;
    QPointer<ContentPanel> m_contentPanel;
    SearchScope m_searchScope = SearchScope::CurrentFolder;
};

} // namespace QuarkMeta
