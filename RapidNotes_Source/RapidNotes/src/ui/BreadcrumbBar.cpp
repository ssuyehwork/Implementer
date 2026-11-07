#include "BreadcrumbBar.h"
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include "UiHelper.h"

namespace QuarkMeta {

BreadcrumbNodeButton::BreadcrumbNodeButton(const QString& name, const QString& fullPath, QWidget* parent)
    : QPushButton(name, parent), m_fullPath(fullPath) {
    setAcceptDrops(true);
}

void BreadcrumbNodeButton::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        QPushButton::dragEnterEvent(event);
    }
}

void BreadcrumbNodeButton::dropEvent(QDropEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        QStringList paths;
        for (const QUrl& url : event->mimeData()->urls()) {
            QString p = url.toLocalFile();
            if (!p.isEmpty()) paths.append(p);
        }
        if (!paths.isEmpty()) {
            emit pathsDroppedNode(paths, m_fullPath);
            event->acceptProposedAction();
            return;
        }
    }
    QPushButton::dropEvent(event);
}

BreadcrumbBar::BreadcrumbBar(QWidget* parent) : QWidget(parent) {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(8, 0, 8, 0);
    m_layout->setSpacing(2);

    setCursor(Qt::PointingHandCursor);
}

void BreadcrumbBar::setPath(const QString& path) {
    m_currentPath = path;
    m_nodes.clear();

    if (path == "computer://") {
        m_nodes.append(PathNode("此电脑", "computer://"));
    } else if (path.contains(" → ")) {
        QStringList parts = path.split(" → ", Qt::SkipEmptyParts);
        for (const QString& part : parts) {
            m_nodes.append(PathNode(part, path));
        }
    } else {
        QString normPath = QDir::toNativeSeparators(path);
        QStringList parts = normPath.split(QDir::separator(), Qt::SkipEmptyParts);

        QString currentBuildPath;
        if (normPath.contains(":") && normPath.indexOf(":") == 1) {
            QString drive = normPath.left(3); // "C:\"
            m_nodes.append(PathNode(drive, drive));
            currentBuildPath = drive;

            if (parts.size() > 0 && parts[0].contains(":")) {
                parts.removeFirst();
            }
        }

        for (qsizetype i = 0; i < parts.size(); ++i) {
            if (!currentBuildPath.endsWith(QDir::separator())) {
                currentBuildPath += QDir::separator();
            }
            currentBuildPath += parts[i];
            m_nodes.append(PathNode(parts[i], currentBuildPath));
        }
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
    int availableWidth = width() - 32; // 留出左右边距与容差
    if (availableWidth <= 0) availableWidth = 300;

    // 计算包含所有节点时的预估总宽度
    int totalWidth = 0;
    for (int i = 0; i < m_nodes.size(); ++i) {
        totalWidth += fm.horizontalAdvance(m_nodes[i].name) + 24; // 按钮 padding
        if (i > 0) totalWidth += 14; // 分隔符宽度
    }

    if (totalWidth <= availableWidth || m_nodes.size() <= 2) {
        // 未超长或节点极少：完整呈现全部路径，打上“未截断”标记
        m_isElided = false;
        for (int i = 0; i < m_nodes.size(); ++i) {
            if (i > 0) {
                QLabel* sep = new QLabel(this);
                sep->setPixmap(UiHelper::getIcon("chevron_right", QColor("#AAAAAA"), 12).pixmap(12, 12));
                sep->setObjectName("BreadcrumbSep");
                m_layout->addWidget(sep);
            }
            addLevel(m_nodes[i].name, m_nodes[i].fullPath);
        }
    } else {
        // 超长：中间超出部分截断并显示 ... 按钮，打上“已截断”标记
        m_isElided = true;

        int headCount = 1;
        if (m_nodes.size() > 3) headCount = 2; // 较长路径保留前两级（例如 C:\ > Users）

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

        // 渲染头部保留节点
        for (int i = 0; i < headCount; ++i) {
            if (i > 0) {
                QLabel* sep = new QLabel(this);
                sep->setPixmap(UiHelper::getIcon("chevron_right", QColor("#AAAAAA"), 12).pixmap(12, 12));
                sep->setObjectName("BreadcrumbSep");
                m_layout->addWidget(sep);
            }
            addLevel(m_nodes[i].name, m_nodes[i].fullPath);
        }

        // 渲染 "..." 省略按钮
        QLabel* sepEllipsis = new QLabel(this);
        sepEllipsis->setPixmap(UiHelper::getIcon("chevron_right", QColor("#AAAAAA"), 12).pixmap(12, 12));
        sepEllipsis->setObjectName("BreadcrumbSep");
        m_layout->addWidget(sepEllipsis);

        QPushButton* btnMore = new QPushButton("...", this);
        btnMore->setFixedHeight(24);
        btnMore->setObjectName("BreadcrumbNodeBtn");
        connect(btnMore, &QPushButton::clicked, this, &BreadcrumbBar::blankAreaClicked);
        m_layout->addWidget(btnMore);

        // 渲染尾部保留节点
        for (int i = visibleTailIndex; i < m_nodes.size(); ++i) {
            QLabel* sep = new QLabel(this);
            sep->setPixmap(UiHelper::getIcon("chevron_right", QColor("#AAAAAA"), 12).pixmap(12, 12));
            sep->setObjectName("BreadcrumbSep");
            m_layout->addWidget(sep);
            addLevel(m_nodes[i].name, m_nodes[i].fullPath);
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

void BreadcrumbBar::addLevel(const QString& name, const QString& fullPath) {
    BreadcrumbNodeButton* btn = new BreadcrumbNodeButton(name, fullPath, this);
    btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    btn->setFixedHeight(24);
    btn->setObjectName("BreadcrumbNodeBtn");

    btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(btn, &QPushButton::clicked, [this, fullPath]() {
        emit pathClicked(fullPath);
    });
    connect(btn, &QPushButton::customContextMenuRequested, [this, btn, fullPath](const QPoint& pos) {
        emit favoriteToggleRequested(fullPath, btn->mapToGlobal(pos));
    });
    connect(btn, &BreadcrumbNodeButton::pathsDroppedNode, this, [this](const QStringList& paths, const QString& targetPath) {
        emit pathsDroppedNode(paths, targetPath);
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

} // namespace QuarkMeta