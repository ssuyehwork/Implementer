#pragma once

#include "FramelessDialog.h"
#include "components/FlowLayout.h"
#include "../core/TagLexiconService.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QFrame>
#include <QButtonGroup>

namespace QuarkMeta {

class TagManagerDialog : public FramelessDialog {
    Q_OBJECT
public:
    static void showDialog(QWidget* parent, const QString& currentPath = "", bool isMirrorSource = false);

protected:
    explicit TagManagerDialog(const QString& currentPath, bool isMirrorSource, QWidget* parent = nullptr);
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onSidebarToggled(bool checked);
    void onSidebarItemClicked(int id);
    void onAddNewGroup();

private:
    void initContent();
    void applyTheme();
    void refreshSidebar();
    void refreshTags();
    void createTag(const QString& tagName);
    void showGroupContextMenu(int groupId, const QString& groupName, const QPoint& globalPos);
    void showTagContextMenu(const QString& tagName, const QPoint& globalPos);

    QString m_currentPath;
    int m_activeGroupId = 0; // 0=全部, -1=未分类, -2=常用, >0 为自定义组 ID

    QLineEdit* m_searchEdit = nullptr;
    QPushButton* m_btnToggleSidebar = nullptr;

    QFrame* m_sidebar = nullptr;
    QVBoxLayout* m_sidebarLayout = nullptr;
    QVBoxLayout* m_groupButtonsLayout = nullptr;
    QButtonGroup* m_sidebarGroup = nullptr;

    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_contentWidget = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;

    QWidget* m_addNewTagWidget = nullptr;
    QPushButton* m_btnAddNewTag = nullptr;

    QVBoxLayout* m_tagsScrollLayout = nullptr;

    // 🚀【类型对齐】：采用标准 TagGroup 数据结构
    QList<TagGroup> m_allGroups;
    QStringList m_masterTags;
};

} // namespace QuarkMeta