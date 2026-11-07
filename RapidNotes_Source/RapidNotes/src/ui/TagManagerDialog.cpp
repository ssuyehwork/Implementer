#include "TagManagerDialog.h"
#include "UiHelper.h"
#include "StyleLibrary.h"
#include "FramelessDialog.h"
#include "components/FlowLayout.h"
#include "../core/TagLexiconService.h"
#include <QApplication>
#include <QMenu>
#include <QAction>

namespace QuarkMeta {

void TagManagerDialog::showDialog(QWidget* parent, const QString& currentPath, bool isMirrorSource) {
    TagManagerDialog* dlg = new TagManagerDialog(currentPath, isMirrorSource, parent);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

TagManagerDialog::TagManagerDialog(const QString& currentPath, bool isMirrorSource, QWidget* parent)
    : FramelessDialog("标签管理", parent), m_currentPath(currentPath) {
    Q_UNUSED(isMirrorSource);

    setMinimumSize(800, 600);
    resize(1000, 800);

    initContent();
    applyTheme();
    refreshSidebar();
    refreshTags();
}

void TagManagerDialog::initContent() {
    QVBoxLayout* mainL = new QVBoxLayout(m_contentArea);
    mainL->setContentsMargins(0, 0, 0, 0);
    mainL->setSpacing(0);

    // 1. 顶部操作栏
    QWidget* topBar = new QWidget(this);
    topBar->setFixedHeight(40);
    topBar->setObjectName("TagManagerTopBar");
    QHBoxLayout* topL = new QHBoxLayout(topBar);
    topL->setContentsMargins(15, 0, 15, 0);
    topL->setSpacing(10);

    m_searchEdit = new QLineEdit(topBar);
    m_searchEdit->setPlaceholderText("搜索...");
    m_searchEdit->setFixedHeight(32);
    m_searchEdit->setObjectName("TagManagerSearchEdit");
    UiHelper::setupLineEditContextMenu(m_searchEdit);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &TagManagerDialog::onSearchTextChanged);
    connect(m_searchEdit, &QLineEdit::returnPressed, [this]() {
        QString kw = m_searchEdit->text().trimmed();
        if (!kw.isEmpty()) {
            createTag(kw);
            m_searchEdit->clear();
        }
    });
    topL->addWidget(m_searchEdit, 1);
    mainL->addWidget(topBar);

    // 侧边栏开关
    m_btnToggleSidebar = new QPushButton(this);
    m_btnToggleSidebar->setAutoDefault(false);
    m_btnToggleSidebar->setDefault(false);
    m_btnToggleSidebar->setFixedSize(20, 20);
    m_btnToggleSidebar->setCheckable(true);
    m_btnToggleSidebar->setChecked(true);
    m_btnToggleSidebar->setIcon(UiHelper::getIcon("sidebar", QColor("#CCCCCC"), 16));
    m_btnToggleSidebar->setIconSize(QSize(16, 16));
    m_btnToggleSidebar->setCursor(Qt::PointingHandCursor);
    m_btnToggleSidebar->setProperty("tooltipText", "展开/收起侧边栏");
    m_btnToggleSidebar->setObjectName("TagManagerToggleBtn");
    connect(m_btnToggleSidebar, &QPushButton::toggled, this, &TagManagerDialog::onSidebarToggled);

    if (m_titleLayout && m_pinBtn) {
        int pinIndex = m_titleLayout->indexOf(m_pinBtn);
        m_titleLayout->insertWidget(pinIndex, m_btnToggleSidebar);
    }

    // 2. 主体区
    QWidget* bodyWidget = new QWidget(this);
    QHBoxLayout* bodyL = new QHBoxLayout(bodyWidget);
    bodyL->setContentsMargins(10, 10, 10, 10);
    bodyL->setSpacing(10);

    // A. 侧边栏
    m_sidebar = new QFrame(bodyWidget);
    m_sidebar->setFixedWidth(180);
    m_sidebar->setObjectName("TagManagerSidebar");
    m_sidebarLayout = new QVBoxLayout(m_sidebar);
    m_sidebarLayout->setContentsMargins(10, 15, 10, 10);
    m_sidebarLayout->setSpacing(6);

    QLabel* sideTitle = new QLabel("分类导航", m_sidebar);
    sideTitle->setObjectName("TagManagerSideTitle");
    m_sidebarLayout->addWidget(sideTitle);

    m_groupButtonsLayout = new QVBoxLayout();
    m_groupButtonsLayout->setContentsMargins(0, 0, 0, 0);
    m_groupButtonsLayout->setSpacing(4);
    m_sidebarLayout->addLayout(m_groupButtonsLayout);

    m_sidebarGroup = new QButtonGroup(this);
    m_sidebarGroup->setExclusive(true);
    connect(m_sidebarGroup, &QButtonGroup::idClicked, this, &TagManagerDialog::onSidebarItemClicked);

    m_sidebarLayout->addStretch();

    // 底部“新建分组”按钮
    QPushButton* btnAddGroup = new QPushButton(UiHelper::getIcon("add", QColor("#AAAAAA"), 14), " 新建分组...", m_sidebar);
    btnAddGroup->setAutoDefault(false);
    btnAddGroup->setDefault(false);
    btnAddGroup->setFixedHeight(32);
    btnAddGroup->setCursor(Qt::PointingHandCursor);
    btnAddGroup->setObjectName("TagManagerBtnAddGroup");
    connect(btnAddGroup, &QPushButton::clicked, this, &TagManagerDialog::onAddNewGroup);
    m_sidebarLayout->addWidget(btnAddGroup);

    bodyL->addWidget(m_sidebar);

    // B. 右侧内容区
    m_scrollArea = new QScrollArea(bodyWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setObjectName("TagManagerScrollArea");
    if (m_scrollArea->viewport()) {
// viewport style in style.qss
    }

    m_contentWidget = new QWidget();
    m_contentWidget->setAttribute(Qt::WA_StyledBackground, true);
    m_contentWidget->setObjectName("TagManagerContentWidget");
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(15, 15, 15, 15);
    m_contentLayout->setSpacing(15);

    // 动态新建按钮胶囊
    m_addNewTagWidget = new QWidget(m_contentWidget);
    QHBoxLayout* addL = new QHBoxLayout(m_addNewTagWidget);
    addL->setContentsMargins(0, 0, 0, 0);
    m_btnAddNewTag = new QPushButton(m_addNewTagWidget);
    m_btnAddNewTag->setAutoDefault(false);
    m_btnAddNewTag->setDefault(false);
    m_btnAddNewTag->setCursor(Qt::PointingHandCursor);
    m_btnAddNewTag->setObjectName("TagManagerBtnAddNewTag");
    connect(m_btnAddNewTag, &QPushButton::clicked, [this]() {
        QString kw = m_searchEdit->text().trimmed();
        if (!kw.isEmpty()) {
            createTag(kw);
            m_searchEdit->clear();
        }
    });
    addL->addWidget(m_btnAddNewTag, 0, Qt::AlignLeft);
    m_addNewTagWidget->hide();
    m_contentLayout->addWidget(m_addNewTagWidget);

    QWidget* tagsScrollWidget = new QWidget(m_contentWidget);
    m_tagsScrollLayout = new QVBoxLayout(tagsScrollWidget);
    m_tagsScrollLayout->setContentsMargins(0, 0, 0, 0);
    m_tagsScrollLayout->setSpacing(15);
    m_contentLayout->addWidget(tagsScrollWidget);

    m_contentLayout->addStretch();
    m_scrollArea->setWidget(m_contentWidget);
    bodyL->addWidget(m_scrollArea, 1);

    mainL->addWidget(bodyWidget, 1);
}

void TagManagerDialog::refreshSidebar() {
    while (QLayoutItem* item = m_groupButtonsLayout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    for (auto* btn : m_sidebarGroup->buttons()) {
        m_sidebarGroup->removeButton(btn);
    }

    auto createSideBtn = [this](int id, const QString& icon, const QString& name) {
        QPushButton* btn = new QPushButton(m_sidebar);
        btn->setAutoDefault(false);
        btn->setDefault(false);
        btn->setText(" " + name);
        btn->setIcon(UiHelper::getIcon(icon, QColor("#AAAAAA"), 13));
        btn->setIconSize(QSize(13, 13));
        btn->setCheckable(true);
        btn->setFixedSize(160, 28);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setObjectName("TagManagerSideBtn");
        m_groupButtonsLayout->addWidget(btn);
        m_sidebarGroup->addButton(btn, id);
        return btn;
    };

    createSideBtn(0, "all_data", "全部标签");
    createSideBtn(-1, "uncategorized-2", "未分类标签");
    createSideBtn(-2, "star_filled", "常用标签");

    // 动态加载自定义分组
    m_allGroups = TagLexiconService::instance().getAllTagGroups();
    for (const auto& grp : m_allGroups) {
        QPushButton* btn = createSideBtn(grp.id, "folder_filled", grp.name);
        btn->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(btn, &QWidget::customContextMenuRequested, this, [this, grp](const QPoint& pos) {
            QPushButton* b = qobject_cast<QPushButton*>(sender());
            if (b) showGroupContextMenu(grp.id, grp.name, b->mapToGlobal(pos));
        });
    }

    if (m_sidebarGroup->button(m_activeGroupId)) {
        m_sidebarGroup->button(m_activeGroupId)->setChecked(true);
    } else {
        m_activeGroupId = 0;
        m_sidebarGroup->button(0)->setChecked(true);
    }
}

void TagManagerDialog::onAddNewGroup() {
    FramelessInputDialog dlg("新建分组", "输入分组名称:", "", this);
    if (dlg.exec() == QDialog::Accepted) {
        QString name = dlg.text().trimmed();
        if (!name.isEmpty()) {
            TagLexiconService::instance().createGroup(name);
            refreshSidebar();
        }
    }
}

void TagManagerDialog::showGroupContextMenu(int groupId, const QString& groupName, const QPoint& globalPos) {
    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);
    menu.addAction(UiHelper::getIcon("edit", QColor("#EEEEEE"), 18), "重命名分组")->setData(1);
    menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "删除分组")->setData(2);

    QAction* act = menu.exec(globalPos);
    if (!act) return;

    if (act->data().toInt() == 1) {
        FramelessInputDialog dlg("重命名分组", "新分组名称:", groupName, this);
        if (dlg.exec() == QDialog::Accepted) {
            QString newName = dlg.text().trimmed();
            if (!newName.isEmpty()) {
                TagLexiconService::instance().renameGroup(groupId, newName);
                refreshSidebar();
            }
        }
    } else if (act->data().toInt() == 2) {
        if (FramelessMessageBox::question(this, "确认删除", QString("确定要删除分组 \"%1\" 吗？组内标签将保留并转为未分类。").arg(groupName))) {
            TagLexiconService::instance().deleteGroup(groupId);
            refreshSidebar();
            refreshTags();
        }
    }
}

void TagManagerDialog::onSidebarToggled(bool checked) {
    m_sidebar->setVisible(checked);
    setMinimumSize(800, 600);
}

void TagManagerDialog::onSidebarItemClicked(int id) {
    m_activeGroupId = id;
    refreshTags();
}

void TagManagerDialog::onSearchTextChanged(const QString& text) {
    QString kw = text.trimmed();
    if (kw.isEmpty()) {
        m_addNewTagWidget->hide();
    } else {
        bool exactMatch = m_masterTags.contains(kw, Qt::CaseInsensitive);
        if (!exactMatch) {
            m_btnAddNewTag->setText(QString("新增词条 \"%1\"").arg(kw));
            m_btnAddNewTag->setIcon(UiHelper::getIcon("add", QColor("#FFFFFF"), 12));
            m_btnAddNewTag->setIconSize(QSize(12, 12));
            m_addNewTagWidget->show();
        } else {
            m_addNewTagWidget->hide();
        }
    }
    refreshTags();
}

void TagManagerDialog::createTag(const QString& tagName) {
    QString cleanTag = tagName.trimmed();
    if (cleanTag.isEmpty()) return;

    // 1. 写入 inspiration.db 词库
    TagLexiconService::instance().addTag(cleanTag, m_activeGroupId > 0 ? m_activeGroupId : -1);

    refreshSidebar();
    refreshTags();
}

void TagManagerDialog::showTagContextMenu(const QString& tagName, const QPoint& globalPos) {
    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    // 1. 添加到分组子菜单
    QMenu* groupSubMenu = menu.addMenu(UiHelper::getIcon("folder_filled", QColor("#EEEEEE"), 18), "添加到分组...");
    UiHelper::applyMenuStyle(groupSubMenu);
    for (const auto& grp : m_allGroups) {
        if (grp.id <= 0) continue;
        QAction* actGrp = groupSubMenu->addAction(UiHelper::getIcon("tag", QColor("#EEEEEE"), 16), grp.name);
        connect(actGrp, &QAction::triggered, this, [this, tagName, grp]() {
            TagLexiconService::instance().moveTagToGroup(tagName, grp.id);
            refreshSidebar();
            refreshTags();
        });
    }

    // 2. 从当前组移出（仅在具体组视图有效）
    if (m_activeGroupId > 0) {
        menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE"), 18), "从当前组移出")->setData(1);
    }

    menu.addSeparator();
    menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "删除此标签")->setData(2);

    QAction* act = menu.exec(globalPos);
    if (!act) return;

    if (act->data().toInt() == 1) {
        TagLexiconService::instance().moveTagToGroup(tagName, -1);
        refreshSidebar();
        refreshTags();
    } else if (act->data().toInt() == 2) {
        if (FramelessMessageBox::question(this, "确认删除", QString("确定从词库中彻底删除标签 \"%1\" 吗？").arg(tagName))) {
            TagLexiconService::instance().deleteTag(tagName);
            refreshSidebar();
            refreshTags();
        }
    }
}

void TagManagerDialog::refreshTags() {
    m_masterTags = TagLexiconService::instance().getAllTagNames();

    while (QLayoutItem* item = m_tagsScrollLayout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    QString kw = m_searchEdit->text().trimmed().toLower();
    QStringList filteredTags;

    if (m_activeGroupId == -2) {
        // 常用 / 最近使用
        filteredTags = TagLexiconService::instance().querySuggestions("", 30);
    } else if (m_activeGroupId == -1) {
        // 未分类
        QSet<QString> groupedTags;
        for (const auto& grp : m_allGroups) {
            if (grp.id <= 0) continue;
            for (const auto& t : grp.tags) groupedTags.insert(t.name);
        }
        for (const QString& t : m_masterTags) {
            if (!groupedTags.contains(t)) filteredTags << t;
        }
    } else if (m_activeGroupId > 0) {
        // 自定义分组
        for (const auto& grp : m_allGroups) {
            if (grp.id == m_activeGroupId) {
                for (const auto& t : grp.tags) filteredTags << t.name;
                break;
            }
        }
    } else {
        // 全部标签
        filteredTags = m_masterTags;
    }

    // 关键词过滤
    QStringList finalTags;
    for (const QString& t : filteredTags) {
        if (kw.isEmpty() || t.toLower().contains(kw)) {
            finalTags << t;
        }
    }

    if (finalTags.isEmpty()) return;

    QWidget* groupWidget = new QWidget();
    QVBoxLayout* groupL = new QVBoxLayout(groupWidget);
    groupL->setContentsMargins(0, 0, 0, 0);
    groupL->setSpacing(6);

    QWidget* flowContainer = new QWidget(groupWidget);
    FlowLayout* flowL = new FlowLayout(flowContainer, 0, 6, 6);

    for (const QString& tag : finalTags) {
        QPushButton* btn = new QPushButton(tag, flowContainer);
        btn->setAutoDefault(false);
        btn->setDefault(false);
        btn->setIcon(UiHelper::getIcon("tag_pill", QColor("#888888"), 12));
        btn->setIconSize(QSize(12, 12));
        btn->setCursor(Qt::PointingHandCursor);
        btn->setContextMenuPolicy(Qt::CustomContextMenu);
        btn->setObjectName("TagManagerTagBtn");
        connect(btn, &QWidget::customContextMenuRequested, this, [this, tag](const QPoint& pos) {
            QPushButton* b = qobject_cast<QPushButton*>(sender());
            if (b) showTagContextMenu(tag, b->mapToGlobal(pos));
        });
        flowL->addWidget(btn);
    }
    groupL->addWidget(flowContainer);
    m_tagsScrollLayout->addWidget(groupWidget);
}

void TagManagerDialog::resizeEvent(QResizeEvent* event) {
    FramelessDialog::resizeEvent(event);
}

void TagManagerDialog::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (m_searchEdit && m_searchEdit->hasFocus()) {
            QString kw = m_searchEdit->text().trimmed();
            if (!kw.isEmpty()) {
                createTag(kw);
                m_searchEdit->clear();
            }
        }
        event->accept();
        return;
    }
    FramelessDialog::keyPressEvent(event);
}

void TagManagerDialog::applyTheme() {
    // Style handled in style.qss
}

} // namespace QuarkMeta