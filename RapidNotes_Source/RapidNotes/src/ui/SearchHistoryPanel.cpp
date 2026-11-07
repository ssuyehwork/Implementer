#include "SearchHistoryPanel.h"
#include "SvgIcons.h"
#include "UiHelper.h"
#include "../core/SearchHistoryService.h"

#include <QCursor>
#include <QApplication>
#include <QMouseEvent>

namespace QuarkMeta {

SearchHistoryPanel::SearchHistoryPanel(QWidget* parent)
    : QFrame(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground, false);

    setObjectName("SearchHistoryPanel");

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(6, 6, 6, 6);
    m_layout->setSpacing(2);

    // 绑定中心化服务信号
    connect(&SearchHistoryService::instance(), &SearchHistoryService::searchHistoryChanged,
            this, &SearchHistoryPanel::onSearchHistoryChanged);

    hide();
}

void SearchHistoryPanel::setHistory(const QStringList& history, const QString& title) {
    m_history = history;
    m_currentTitle = title;
    rebuild();
}

void SearchHistoryPanel::onSearchHistoryChanged(const QString& category, const QStringList& newHistory) {
    if (category == m_category) {
        m_history = newHistory;
        rebuild();
    }
}

void SearchHistoryPanel::rebuild() {
    QLayoutItem* child;
    while ((child = m_layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    if (m_history.isEmpty()) {
        QLabel* empty = new QLabel(QString("暂无%1").arg(m_currentTitle), this);
        empty->setObjectName("HistoryEmptyLabel");
        m_layout->addWidget(empty);
    } else {
        QWidget* titleRow = new QWidget(this);
        titleRow->setObjectName("HistoryTitleRow");
        QHBoxLayout* titleLayout = new QHBoxLayout(titleRow);
        titleLayout->setContentsMargins(4, 0, 4, 0);
        titleLayout->setSpacing(0);

        QLabel* titleLabel = new QLabel(m_currentTitle, titleRow);
        titleLabel->setObjectName("HistoryTitleLabel");

        QPushButton* btnClearAll = new QPushButton("全部清除", titleRow);
        btnClearAll->setFixedHeight(20);
        btnClearAll->setFlat(true);
        btnClearAll->setObjectName("HistoryBtnClearAll");
        connect(btnClearAll, &QPushButton::clicked, this, [this]() {
            SearchHistoryService::instance().clearAll(m_category);
        });

        titleLayout->addWidget(titleLabel);
        titleLayout->addStretch();
        titleLayout->addWidget(btnClearAll);
        m_layout->addWidget(titleRow);

        QFrame* sep = new QFrame(this);
        sep->setFrameShape(QFrame::HLine);
        sep->setObjectName("HistorySep");
        m_layout->addWidget(sep);

        for (int i = 0; i < m_history.size(); ++i) {
            const QString& keyword = m_history[i];

            QWidget* row = new QWidget(this);
            row->setObjectName("HistoryRow");
            row->setCursor(Qt::PointingHandCursor);
            row->setFixedHeight(30);

            QHBoxLayout* rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(6, 0, 4, 0);
            rowLayout->setSpacing(8);

            QLabel* icon = new QLabel(row);
            icon->setPixmap(UiHelper::getIcon("search", QColor("#555555"), 14).pixmap(14, 14));
            icon->setFixedSize(14, 14);

            QLabel* keywordLabel = new QLabel(keyword, row);
            keywordLabel->setObjectName("HistoryItemLabel");

            QPushButton* btnRemove = new QPushButton(row);
            btnRemove->setFixedSize(16, 16);
            btnRemove->setFlat(true);
            btnRemove->setIcon(UiHelper::getIcon("close", QColor("#555555"), 12));
            btnRemove->setIconSize(QSize(12, 12));
            btnRemove->setObjectName("HistoryBtnRemove");
            connect(btnRemove, &QPushButton::clicked, this, [this, keyword]() {
                SearchHistoryService::instance().removeSearch(m_category, keyword);
            });

            rowLayout->addWidget(icon);
            rowLayout->addWidget(keywordLabel, 1);
            rowLayout->addWidget(btnRemove);

            row->installEventFilter(this);
            row->setProperty("keyword", keyword);

            m_layout->addWidget(row);
        }
    }

    adjustSize();
}

void SearchHistoryPanel::showBelow(QWidget* anchor) {
    if (!anchor) return;
    setFixedWidth(anchor->width());
    rebuild();

    QPoint globalAnchorTop = anchor->mapToGlobal(QPoint(0, 0));
    int panelHeight = height();

    QScreen* screen = QGuiApplication::screenAt(globalAnchorTop);
    QRect screenGeo = screen ? screen->availableGeometry() : QRect(0, 0, 1920, 1080);

    int spaceBelow = screenGeo.bottom() - (globalAnchorTop.y() + anchor->height());
    int spaceAbove = globalAnchorTop.y() - screenGeo.top();

    QPoint pos;
    if (spaceBelow < panelHeight + 3 && spaceAbove > spaceBelow) {
        // 下方空间不足时，自动向锚点上方弹出贴合
        pos = QPoint(globalAnchorTop.x(), globalAnchorTop.y() - panelHeight - 3);
    } else {
        // 空间充足时，默认向锚点下方弹出
        pos = QPoint(globalAnchorTop.x(), globalAnchorTop.y() + anchor->height() + 3);
    }

    move(pos);
    show();
    raise();
}

bool SearchHistoryPanel::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget* w = qobject_cast<QWidget*>(obj);
        if (w && w->objectName() == "HistoryRow") {
            QString keyword = w->property("keyword").toString();
            if (!keyword.isEmpty()) {
                emit historyItemClicked(keyword);
            }
            return true;
        }
    }
    return QFrame::eventFilter(obj, event);
}

} // namespace QuarkMeta
