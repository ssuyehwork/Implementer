#include "SidebarContainerWidget.h"
#include "UiHelper.h"
#include "../core/AppConfig.h"
#include "../core/SearchHistoryService.h"
#include <QMouseEvent>
#include <QStyleOptionTab>
#include <QPainter>

namespace RapidNotes {

SidebarContainerWidget::SidebarContainerWidget(QWidget* parent) : QFrame(parent) {
    setObjectName("SidebarContainerWidget");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(230);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    initUi();
}

void SidebarContainerWidget::initUi() {
    QWidget* header = new QWidget(this);
    header->setObjectName("ContainerHeader");
    header->setFixedHeight(32);

    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(5, 0, 5, 0);
    headerLayout->setSpacing(0);

    // 侧边栏专用选项卡：直接掌控绘制，彻底杜绝 QSS 下样式代理被拦截失效问题，硬锁 7 像素间距
    class SidebarTabBar : public QTabBar {
    public:
        using QTabBar::QTabBar;
    protected:
        QSize tabSizeHint(int index) const override {
            QSize s = QTabBar::tabSizeHint(index);
            if (!tabIcon(index).isNull() && !tabText(index).isEmpty()) {
                s.setWidth(s.width() - 5);
            }
            return s;
        }

        void paintEvent(QPaintEvent* event) override {
            Q_UNUSED(event);
            QPainter painter(this);
            QStyleOptionTab opt;
            for (int i = 0; i < count(); ++i) {
                initStyleOption(&opt, i);
                // 1. 保留 QSS 样式表对背景色、边框、选态与悬停底色的完全渲染
                style()->drawControl(QStyle::CE_TabBarTabShape, &opt, &painter, this);

                // 2. 自定义精确绘制图标与文字，硬锁间隙为 7px
                QRect tr = opt.rect;
                QSize iconSize = opt.iconSize.isValid() ? opt.iconSize : QSize(16, 16);
                int iconWidth = opt.icon.isNull() ? 0 : iconSize.width();
                int textWidth = opt.fontMetrics.horizontalAdvance(opt.text);
                int spacing = (iconWidth > 0 && !opt.text.isEmpty()) ? 7 : 0;
                int totalContentWidth = iconWidth + spacing + textWidth;
                int startX = tr.left() + (tr.width() - totalContentWidth) / 2;

                if (iconWidth > 0) {
                    int iconY = tr.top() + (tr.height() - iconSize.height()) / 2;
                    QRect iconRect(startX, iconY, iconSize.width(), iconSize.height());
                    opt.icon.paint(&painter, iconRect, Qt::AlignCenter);
                }

                QRect textRect(startX + iconWidth + spacing, tr.top(), textWidth + 2, tr.height());
                painter.save();
                painter.setFont(font());
                QColor textColor = (opt.state & QStyle::State_Selected) ? QColor("#FFFFFF") : ((opt.state & QStyle::State_MouseOver) ? QColor("#CCCCCC") : QColor("#AAAAAA"));
                painter.setPen(textColor);
                painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, opt.text);
                painter.restore();
            }
        }
    };

    m_tabBar = new SidebarTabBar(header);
    m_tabBar->setObjectName("SidebarTabBar");
    m_tabBar->setDrawBase(false);
    m_tabBar->setUsesScrollButtons(false);
    m_tabBar->setIconSize(QSize(16, 16));
    m_tabBar->addTab(UiHelper::getIcon("Search-in-library", "#888888", 16), "库");
    m_tabBar->addTab(UiHelper::getIcon("Destination.svg", "#888888", 16), "标记");
    m_tabBar->setCursor(Qt::PointingHandCursor);

    headerLayout->addWidget(m_tabBar);
    headerLayout->addStretch();
    m_mainLayout->addWidget(header);

    m_stackedWidget = new QStackedWidget(this);
    m_libraryPanel = new LibraryPanel(this);
    m_markedPanel = new MarkedPanel(this);

    m_stackedWidget->addWidget(m_libraryPanel);
    m_stackedWidget->addWidget(m_markedPanel);

    m_mainLayout->addWidget(m_stackedWidget, 1);

    // 底部统一搜索过滤框部件创建与配置
    QWidget* filterContainer = new QWidget(this);
    filterContainer->setObjectName("SidebarFilterContainer");
    QVBoxLayout* filterLayout = new QVBoxLayout(filterContainer);
    filterLayout->setContentsMargins(8, 8, 8, 8);
    filterLayout->setSpacing(0);

    m_filterEdit = new QLineEdit(filterContainer);
    m_filterEdit->setObjectName("SidebarFilterEdit");
    m_filterEdit->setPlaceholderText("筛选项目");
    m_filterEdit->setClearButtonEnabled(true);
    m_filterEdit->setFixedHeight(32);
    m_filterEdit->addAction(UiHelper::getIcon("filter_funnel", QColor("#888888"), 14), QLineEdit::LeadingPosition);
    m_filterEdit->installEventFilter(this);
    filterLayout->addWidget(m_filterEdit);

    m_historyPanel = new SearchHistoryPanel(this);
    m_historyPanel->setCategory("sidebar_filter");

    connect(m_historyPanel, &SearchHistoryPanel::historyItemClicked, this, [this](const QString& keyword) {
        if (m_filterEdit) {
            m_filterEdit->setText(keyword);
        }
    });

    connect(m_filterEdit, &QLineEdit::returnPressed, this, [this]() {
        if (!m_filterEdit) return;
        QString text = m_filterEdit->text().trimmed();
        if (!text.isEmpty()) {
            SearchHistoryService::instance().appendSearch("sidebar_filter", text);
        }
    });

    connect(m_filterEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        QString kw = text.trimmed();
        if (m_libraryPanel) {
            if (m_libraryPanel->delegate()) {
                m_libraryPanel->delegate()->setFilterKeyword(kw);
            }
            m_libraryPanel->filterTreeNodes(kw);
            if (m_libraryPanel->treeView() && m_libraryPanel->treeView()->viewport()) {
                m_libraryPanel->treeView()->viewport()->update();
            }
        }
        if (m_markedPanel) {
            m_markedPanel->filterTreeNodes(kw);
        }
    });

    m_mainLayout->addWidget(filterContainer, 0);

    connect(m_tabBar, &QTabBar::currentChanged, this, [this](int index) {
        m_stackedWidget->setCurrentIndex(index);
        if (index == 0 && m_libraryPanel) {
            m_libraryPanel->loadLibrary();
        }
        if (m_filterEdit && !m_filterEdit->text().trimmed().isEmpty()) {
            QString kw = m_filterEdit->text().trimmed();
            if (index == 0 && m_libraryPanel) m_libraryPanel->filterTreeNodes(kw);
            if (index == 1 && m_markedPanel) m_markedPanel->filterTreeNodes(kw);
        }
        AppConfig::instance().setValue("Sidebar/CurrentTabIndex", index);
        AppConfig::instance().sync();
        emit sidebarTabChanged(index);
    });

    int savedIndex = AppConfig::instance().getValue("Sidebar/CurrentTabIndex", 0).toInt();
    if (savedIndex >= 0 && savedIndex < m_tabBar->count()) {
        m_tabBar->setCurrentIndex(savedIndex);
    }
}

bool SidebarContainerWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_filterEdit && event->type() == QEvent::MouseButtonDblClick) {
        if (m_historyPanel && m_filterEdit) {
            QStringList history = SearchHistoryService::instance().getHistory("sidebar_filter");
            if (!history.isEmpty()) {
                m_historyPanel->setHistory(history, "最近筛选");
                m_historyPanel->showBelow(m_filterEdit);
            }
        }
        return true;
    }
    return QFrame::eventFilter(watched, event);
}

} // namespace RapidNotes
