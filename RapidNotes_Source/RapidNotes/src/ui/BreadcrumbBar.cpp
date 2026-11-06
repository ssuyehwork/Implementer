#include "BreadcrumbBar.h"
#include <QMouseEvent>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "UiHelper.h"
#include "../core/DatabaseManager.h"

namespace RapidNotes {

BreadcrumbNodeButton::BreadcrumbNodeButton(int id, const QString& name, const QString& fullUrl, QWidget* parent)
    : QPushButton(name, parent), m_id(id), m_fullUrl(fullUrl) {
}

BreadcrumbBar::BreadcrumbBar(QWidget* parent) : QWidget(parent) {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(8, 0, 8, 0);
    m_layout->setSpacing(2);
    
    setCursor(Qt::PointingHandCursor);
}

void BreadcrumbBar::setPath(const QString& url) {
    m_currentUrl = url;
    m_nodes.clear();

    if (url.startsWith("library://")) {
        int catId = url.mid(10).toInt();
        if (catId == -1) {
            // 全部数据: 呈现为 库 > 全部数据
            m_nodes.append(PathNode(-1, "库", "library://-1"));
            m_nodes.append(PathNode(-1, "全部数据", "library://-1"));
        } else if (catId == -2) {
            // 未分类: 呈现为 库 > 未分类
            m_nodes.append(PathNode(-1, "库", "library://-1"));
            m_nodes.append(PathNode(-2, "未分类", "library://-2"));
        } else if (catId > 0) {
            // 自底向上递归查找 parent_id 链条
            QList<PathNode> chain;
            int curr = catId;
            QSet<int> visited;
            while (curr > 0 && !visited.contains(curr)) {
                visited.insert(curr);
                QSqlQuery query(DatabaseManager::instance().database());
                query.prepare("SELECT id, name, parent_id FROM categories WHERE id = :id");
                query.bindValue(":id", curr);
                if (query.exec() && query.next()) {
                    int cId = query.value("id").toInt();
                    QString cName = query.value("name").toString();
                    int pId = query.value("parent_id").isNull() ? -1 : query.value("parent_id").toInt();
                    chain.prepend(PathNode(cId, cName, QString("library://%1").arg(cId)));
                    curr = pId;
                } else {
                    break;
                }
            }
            // 头部固定加入“库”（点击回到全部数据）
            m_nodes.append(PathNode(-1, "库", "library://-1"));
            m_nodes.append(chain);
        } else {
            m_nodes.append(PathNode(-1, "库", "library://-1"));
        }
    } else if (url == "trash://") {
        m_nodes.append(PathNode(-999, "回收站", "trash://"));
    } else if (url.contains(" → ")) {
        QStringList parts = url.split(" → ", Qt::SkipEmptyParts);
        for (const QString& part : parts) {
            m_nodes.append(PathNode(-1, part, url));
        }
    } else {
        // 兜底虚拟协议显示
        m_nodes.append(PathNode(-1, url.isEmpty() ? "全部数据" : url, url));
    }

    rebuildBreadcrumbs();
}

void BreadcrumbBar::rebuildBreadcrumbs() {
    clearButtons();
    if (m_nodes.isEmpty()) {
        m_isElided = false;
        return;
    }

    QFontMetrics fm(font());
    int availableWidth = width() - 32;
    if (availableWidth <= 0) availableWidth = 300;

    int totalWidth = 0;
    for (int i = 0; i < m_nodes.size(); ++i) {
        totalWidth += fm.horizontalAdvance(m_nodes[i].name) + 24;
        if (i > 0) totalWidth += 14;
    }

    if (totalWidth <= availableWidth || m_nodes.size() <= 2) {
        m_isElided = false;
        for (int i = 0; i < m_nodes.size(); ++i) {
            if (i > 0) {
                QLabel* sep = new QLabel(this);
                sep->setPixmap(UiHelper::getIcon("chevron_right", QColor("#AAAAAA"), 12).pixmap(12, 12));
                sep->setObjectName("BreadcrumbSep");
                m_layout->addWidget(sep);
            }
            addLevel(m_nodes[i].id, m_nodes[i].name, m_nodes[i].url);
        }
    } else {
        m_isElided = true;
        int headCount = 1;
        if (m_nodes.size() > 3) headCount = 2;

        int visibleTailIndex = m_nodes.size() - 1;
        int usedWidth = 0;
        for (int i = 0; i < headCount; ++i) {
            usedWidth += fm.horizontalAdvance(m_nodes[i].name) + 24 + 14;
        }
        usedWidth += fm.horizontalAdvance("...") + 24 + 14;

        while (visibleTailIndex > headCount) {
            int nodeW = fm.horizontalAdvance(m_nodes[visibleTailIndex].name) + 24 + 14;
            if (usedWidth + nodeW > availableWidth && visibleTailIndex < m_nodes.size() - 1) {
                break;
            }
            usedWidth += nodeW;
            visibleTailIndex--;
        }
        visibleTailIndex++;

        for (int i = 0; i < headCount; ++i) {
            if (i > 0) {
                QLabel* sep = new QLabel(this);
                sep->setPixmap(UiHelper::getIcon("chevron_right", QColor("#AAAAAA"), 12).pixmap(12, 12));
                sep->setObjectName("BreadcrumbSep");
                m_layout->addWidget(sep);
            }
            addLevel(m_nodes[i].id, m_nodes[i].name, m_nodes[i].url);
        }

        QLabel* sepEllipsis = new QLabel(this);
        sepEllipsis->setPixmap(UiHelper::getIcon("chevron_right", QColor("#AAAAAA"), 12).pixmap(12, 12));
        sepEllipsis->setObjectName("BreadcrumbSep");
        m_layout->addWidget(sepEllipsis);

        QPushButton* btnMore = new QPushButton("...", this);
        btnMore->setFixedHeight(24);
        btnMore->setObjectName("BreadcrumbNodeBtn");
        connect(btnMore, &QPushButton::clicked, this, &BreadcrumbBar::blankAreaClicked);
        m_layout->addWidget(btnMore);

        for (int i = visibleTailIndex; i < m_nodes.size(); ++i) {
            QLabel* sep = new QLabel(this);
            sep->setPixmap(UiHelper::getIcon("chevron_right", QColor("#AAAAAA"), 12).pixmap(12, 12));
            sep->setObjectName("BreadcrumbSep");
            m_layout->addWidget(sep);
            addLevel(m_nodes[i].id, m_nodes[i].name, m_nodes[i].url);
        }
    }

    m_layout->addStretch();
}

void BreadcrumbBar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    rebuildBreadcrumbs();
}

void BreadcrumbBar::clearButtons() {
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0))) {
        if (QWidget* w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }
}

void BreadcrumbBar::addLevel(int id, const QString& name, const QString& url) {
    BreadcrumbNodeButton* btn = new BreadcrumbNodeButton(id, name, url, this);
    btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    btn->setFixedHeight(24);
    btn->setObjectName("BreadcrumbNodeBtn");

    connect(btn, &QPushButton::clicked, [this, url]() {
        emit pathClicked(url);
    });

    m_layout->addWidget(btn);
}

void BreadcrumbBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QWidget* child = childAt(event->pos());
        if (!qobject_cast<QPushButton*>(child)) {
            emit blankAreaClicked();
        }
    }
    QWidget::mousePressEvent(event);
}

} // namespace RapidNotes
