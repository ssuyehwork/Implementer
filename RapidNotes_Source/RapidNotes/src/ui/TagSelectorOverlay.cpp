#include "TagSelectorOverlay.h"
#include "UiHelper.h"
#include "../core/TagLexiconService.h"
#include "../core/AppConfig.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QScreen>
#include <QScrollBar>
#include <QWindow>
#include <QStyle>

namespace QuarkMeta {

TagSelectorOverlay::TagSelectorOverlay(const QStringList& initialSelected, QWidget* parent, TagTriggerSource source)
    : TagSelectorOverlay(initialSelected, QStringList{}, parent, source)
{
}

TagSelectorOverlay::TagSelectorOverlay(const QStringList& initialSelected, const QStringList& candidatePaths, QWidget* parent, TagTriggerSource source)
    : QFrame(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint),
      m_triggerSource(source),
      m_selectedTags(initialSelected),
      m_initialTags(initialSelected)
{
    setObjectName("TagSelectorOverlay");
    setFrameShape(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAttribute(Qt::WA_DeleteOnClose, false);

    m_framelessHelper = FramelessWindowHelper::apply(this, nullptr);

    initUi();
    loadRecentAndRecommendedTags(candidatePaths);

    m_searchEdit->installEventFilter(this);

    if (qApp) {
        qApp->installEventFilter(this);
    }
}

TagSelectorOverlay::~TagSelectorOverlay() {
    if (qApp) {
        qApp->removeEventFilter(this);
    }
    m_swallowingClick = false;
}

bool TagSelectorOverlay::isInternalWidget(QObject* obj) const {
    if (!obj) return false;

    // 1. 原生窗口层 (Qt 6 QPA QWidgetWindow / QWindow 派发阶段)
    if (QWindow* win = qobject_cast<QWindow*>(obj)) {
        QWindow* overlayWin = const_cast<TagSelectorOverlay*>(this)->windowHandle();
        if (!overlayWin) return false;

        QWindow* currWin = win;
        while (currWin) {
            if (currWin == overlayWin) {
                return true;
            }
            currWin = currWin->parent() ? currWin->parent() : currWin->transientParent();
        }
        return false;
    }

    // 2. 控件树层 (QWidget 控件链向上回溯)
    QObject* current = obj;
    while (current) {
        if (current == this) {
            return true;
        }
        if (QWidget* w = qobject_cast<QWidget*>(current)) {
            current = w->parentWidget() ? w->parentWidget() : w->parent();
        } else {
            current = current->parent();
        }
    }
    return false;
}

void TagSelectorOverlay::closeOverlay() {
    if (m_isClosing) return;
    m_isClosing = true;

    if (!m_hasCommitted) {
        m_hasCommitted = true;

        // 仅在来源为 MetaPanel 时，失焦关闭才承担落盘责任
        if (m_triggerSource == TagTriggerSource::MetaPanel) {
            // 批量记录真正新增的有效标签到“最近使用”
            for (const QString& tag : m_selectedTags) {
                if (!m_initialTags.contains(tag)) {
                    recordRecentTag(tag);
                }
            }
            // 向外部发射原子提交落盘信号
            emit tagsCommitted(m_selectedTags);
        }
    }

    emit overlayClosed();
    close();
    deleteLater();
}

void TagSelectorOverlay::hideEvent(QHideEvent* event) {
    QFrame::hideEvent(event);
}

void TagSelectorOverlay::closeEvent(QCloseEvent* event) {
    QFrame::closeEvent(event);
}

void TagSelectorOverlay::showEvent(QShowEvent* event) {
    QFrame::showEvent(event);
    if (m_searchEdit) {
        m_searchEdit->setFocus();
        m_searchEdit->selectAll();
    }
}

void TagSelectorOverlay::initUi() {
    QVBoxLayout* mainL = new QVBoxLayout(this);
    mainL->setContentsMargins(10, 10, 10, 10);
    mainL->setSpacing(8);

    // 1. 顶部搜索框 (唤醒自动定焦)
    QHBoxLayout* topSearchLayout = new QHBoxLayout();
    topSearchLayout->setContentsMargins(0, 0, 0, 0);
    topSearchLayout->setSpacing(6);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索...");
    m_searchEdit->setFixedHeight(28);
    m_searchEdit->setObjectName("TagSelectorSearchEdit");
    UiHelper::setupLineEditContextMenu(m_searchEdit);
    connect(m_searchEdit, &QLineEdit::textChanged, this, [this]() {
        filterAndPopulate();
    });
    topSearchLayout->addWidget(m_searchEdit, 1);
    mainL->addLayout(topSearchLayout);

    // 2. 智能折叠分组主体滚动区
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setObjectName("TagSelectorScrollArea");

    m_bodyContainer = new QWidget(m_scrollArea);
    m_bodyLayout = new QVBoxLayout(m_bodyContainer);
    m_bodyLayout->setContentsMargins(4, 4, 4, 4);
    m_bodyLayout->setSpacing(10);

    // 第一组: 推荐
    m_recHeaderBtn = new QPushButton(m_bodyContainer);
    m_recHeaderBtn->setObjectName("TagSelectorHeaderBtn");
    m_recHeaderBtn->setCursor(Qt::PointingHandCursor);
    connect(m_recHeaderBtn, &QPushButton::clicked, this, [this]() {
        m_recExpanded = !m_recExpanded;
        m_recSectionWidget->setVisible(m_recExpanded);
    });
    m_bodyLayout->addWidget(m_recHeaderBtn);

    m_recSectionWidget = new QWidget(m_bodyContainer);
    m_recFlowLayout = new FlowLayout(m_recSectionWidget, 0, 6, 6);
    m_bodyLayout->addWidget(m_recSectionWidget);

    // 第二组: 最近使用 (最多 50 个)
    m_recentHeaderBtn = new QPushButton(m_bodyContainer);
    m_recentHeaderBtn->setObjectName("TagSelectorHeaderBtn");
    m_recentHeaderBtn->setCursor(Qt::PointingHandCursor);
    connect(m_recentHeaderBtn, &QPushButton::clicked, this, [this]() {
        m_recentExpanded = !m_recentExpanded;
        m_recentSectionWidget->setVisible(m_recentExpanded);
    });
    m_bodyLayout->addWidget(m_recentHeaderBtn);

    m_recentSectionWidget = new QWidget(m_bodyContainer);
    m_recentFlowLayout = new FlowLayout(m_recentSectionWidget, 0, 6, 6);
    m_bodyLayout->addWidget(m_recentSectionWidget);

    m_bodyLayout->addStretch(1);
    m_scrollArea->setWidget(m_bodyContainer);
    mainL->addWidget(m_scrollArea, 1);

    setMinimumSize(300, 220);
    QSize savedSize = AppConfig::instance().getValue("TagSelectorOverlay/Size", QSize(400, 260)).toSize();
    resize(savedSize.expandedTo(QSize(300, 220)));
}

void TagSelectorOverlay::loadRecentAndRecommendedTags(const QStringList& candidatePaths) {
    m_recentTags = AppConfig::instance().getValue("TagSelectorOverlay/RecentTags", QStringList()).toStringList();

    m_recommendedTags.clear();
    QSet<QString> seen;
    static const QRegularExpression splitRegex("[_\\-\\s\\+\\.\\(\\)\\[\\]【】]+");

    for (const QString& path : candidatePaths) {
        if (path.isEmpty() || path.startsWith("trash://")) continue;
        QFileInfo info(path);
        QString baseName = info.completeBaseName();
        if (baseName.isEmpty()) continue;

        QStringList tokens = baseName.split(splitRegex, Qt::SkipEmptyParts);
        for (QString token : tokens) {
            token = token.trimmed();
            if (token.isEmpty()) continue;
            bool isPureNum = false;
            token.toInt(&isPureNum);
            if (isPureNum) continue;

            if (!seen.contains(token)) {
                seen.insert(token);
                m_recommendedTags.append(token);
            }
        }
    }

    filterAndPopulate();
}

void TagSelectorOverlay::filterAndPopulate() {
    QString kw = m_searchEdit ? m_searchEdit->text().trimmed().toLower() : "";

    auto clearLayout = [](FlowLayout* layout) {
        if (!layout) return;
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
    };
    clearLayout(m_recFlowLayout);
    clearLayout(m_recentFlowLayout);

    // 1. 渲染【第一组：推荐】
    QStringList filteredRec;
    for (const QString& t : m_recommendedTags) {
        if (kw.isEmpty() || t.toLower().contains(kw)) {
            filteredRec.append(t);
        }
    }
    m_recHeaderBtn->setText(QString(" 推荐 (%1)").arg(filteredRec.size()));
    m_recHeaderBtn->setVisible(!filteredRec.isEmpty());
    m_recSectionWidget->setVisible(m_recExpanded && !filteredRec.isEmpty());

    for (const QString& recTag : filteredRec) {
        QPushButton* btn = new QPushButton(m_recSectionWidget);
        btn->setObjectName("TagOverlayRecBtn");
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(24);

        bool isSelected = m_selectedTags.contains(recTag);
        btn->setChecked(isSelected);
        btn->setProperty("selected", isSelected ? "true" : "false");
        btn->setText(recTag);
        btn->setIcon(UiHelper::getIcon(isSelected ? "check" : "star_filled", isSelected ? QColor("#FFFFFF") : QColor("#378ADD"), 12));
        btn->setIconSize(QSize(12, 12));

        m_recFlowLayout->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, [this, recTag]() {
            toggleTagSelection(recTag);
        });
    }

    // 2. 渲染【第二组：最近使用 (限制 50 个)】
    QStringList filteredRecent;
    for (const QString& t : m_recentTags) {
        if (kw.isEmpty() || t.toLower().contains(kw)) {
            filteredRecent.append(t);
        }
    }
    m_recentHeaderBtn->setText(QString(" 最近使用 (%1)").arg(filteredRecent.size()));

    for (const QString& tag : filteredRecent) {
        QPushButton* btn = new QPushButton(m_recentSectionWidget);
        btn->setObjectName("TagOverlayRecentBtn");
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(24);

        bool isSelected = m_selectedTags.contains(tag);
        btn->setChecked(isSelected);
        btn->setProperty("selected", isSelected ? "true" : "false");
        btn->setText(tag);
        btn->setIcon(UiHelper::getIcon(isSelected ? "check" : "clock", isSelected ? QColor("#FFFFFF") : QColor("#888888"), 12));
        btn->setIconSize(QSize(12, 12));

        m_recentFlowLayout->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, [this, tag]() {
            toggleTagSelection(tag);
        });
    }
}

void TagSelectorOverlay::toggleTagSelection(const QString& tag) {
    QString cleanTag = tag.trimmed();
    if (cleanTag.isEmpty()) return;

    // 纯内存维护：严禁在此执行任何 SQLite 写入或磁盘落盘
    if (m_selectedTags.contains(cleanTag)) {
        m_selectedTags.removeAll(cleanTag);
    } else {
        m_selectedTags.append(cleanTag);
    }

    // 就地刷新按钮选中状态，杜绝粗暴销毁重建导致焦点瞬间丢失与窗口失活误杀
    updateTagButtonsSelection();

    // 实时通知前端界面同步刷新标签胶囊 (零 I/O 纯内存)
    emit selectionChanged(m_selectedTags);
}

void TagSelectorOverlay::updateTagButtonsSelection() {
    auto updateLayout = [this](FlowLayout* layout, bool isRec) {
        if (!layout) return;
        for (int i = 0; i < layout->count(); ++i) {
            QLayoutItem* item = layout->itemAt(i);
            if (!item || !item->widget()) continue;
            QPushButton* btn = qobject_cast<QPushButton*>(item->widget());
            if (!btn) continue;

            const QString tag = btn->text();
            const bool isSelected = m_selectedTags.contains(tag);
            if (btn->isChecked() != isSelected) {
                btn->setChecked(isSelected);
            }
            btn->setProperty("selected", isSelected ? "true" : "false");
            btn->style()->unpolish(btn);
            btn->style()->polish(btn);

            if (isRec) {
                btn->setIcon(UiHelper::getIcon(isSelected ? "check" : "star_filled", isSelected ? QColor("#FFFFFF") : QColor("#378ADD"), 12));
            } else {
                btn->setIcon(UiHelper::getIcon(isSelected ? "check" : "clock", isSelected ? QColor("#FFFFFF") : QColor("#888888"), 12));
            }
        }
    };

    updateLayout(m_recFlowLayout, true);
    updateLayout(m_recentFlowLayout, false);
}

void TagSelectorOverlay::recordRecentTag(const QString& tag) {
    if (tag.isEmpty()) return;
    m_recentTags.removeAll(tag);
    m_recentTags.prepend(tag);
    while (m_recentTags.size() > 50) {
        m_recentTags.removeLast();
    }
    AppConfig::instance().setValue("TagSelectorOverlay/RecentTags", m_recentTags);
}

void TagSelectorOverlay::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    if (isVisible()) {
        AppConfig::instance().setValue("TagSelectorOverlay/Size", size());
    }
}

void TagSelectorOverlay::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
        return;
    }
    QFrame::mousePressEvent(event);
}

void TagSelectorOverlay::mouseMoveEvent(QMouseEvent* event) {
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPosition().toPoint() - m_dragPos;
        QWidget* parentWin = parentWidget() ? parentWidget()->window() : nullptr;
        if (parentWin) {
            QRect parentGeom = parentWin->frameGeometry();
            int minX = parentGeom.left();
            int maxX = qMax(minX, parentGeom.right() - width());
            int minY = parentGeom.top();
            int maxY = qMax(minY, parentGeom.bottom() - height());
            newPos.setX(qBound(minX, newPos.x(), maxX));
            newPos.setY(qBound(minY, newPos.y(), maxY));
        }
        move(newPos);
        event->accept();
        return;
    }
    QFrame::mouseMoveEvent(event);
}

void TagSelectorOverlay::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
        return;
    }
    QFrame::mouseReleaseEvent(event);
}

void TagSelectorOverlay::changeEvent(QEvent* event) {
    if (event->type() == QEvent::ActivationChange) {
        QWidget* parentWin = parentWidget() ? parentWidget()->window() : nullptr;
        bool isAnyWindowActive = isActiveWindow() || (parentWin && parentWin->isActiveWindow());
        if (!isAnyWindowActive && QGuiApplication::applicationState() == Qt::ApplicationInactive) {
            closeOverlay();
        }
    }
    QFrame::changeEvent(event);
}

bool TagSelectorOverlay::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
        if (isVisible() && !m_isClosing) {
            if (!isInternalWidget(obj)) {
                m_swallowingClick = true;
                closeOverlay();
                return true;
            }
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        if (m_swallowingClick) {
            m_swallowingClick = false;
            if (qApp) {
                qApp->removeEventFilter(this);
            }
            return true;
        }
    }

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);

        if (ke->key() == Qt::Key_Escape) {
            closeOverlay();
            ke->accept();
            return true;
        }

        if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
            if (m_searchEdit && m_searchEdit->hasFocus()) {
                QString kw = m_searchEdit->text().trimmed();
                if (!kw.isEmpty()) {
                    toggleTagSelection(kw);
                    m_searchEdit->clear();
                }
                ke->accept();
                return true;
            }
        }
    }
    return QFrame::eventFilter(obj, event);
}

bool TagSelectorOverlay::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
    if (m_framelessHelper && m_framelessHelper->handleNativeEvent(message, result)) {
        return true;
    }
    return QFrame::nativeEvent(eventType, message, result);
}

} // namespace QuarkMeta