#include "AlphabetIndexBar.h"

#include "ContentPanel.h"
#include "DropJustifiedView.h"
#include "DropTreeView.h"
#include "models/SectionProxyModel.h"
#include "../core/ModelContract.h"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QScrollBar>
#include <QSet>
#include <QStyle>
#include <algorithm>

namespace QuarkMeta {

AlphabetIndexBar::AlphabetIndexBar(ContentPanel* panel)
    : QObject(panel), m_panel(panel) {
}

void AlphabetIndexBar::initialize() {
    if (!m_panel || !m_panel->m_viewStack) return;

    if (!m_bar) {
        m_bar = new QWidget(m_panel->m_viewStack);
        m_bar->setObjectName("AlphabetIndexBar");
        m_bar->setAttribute(Qt::WA_StyledBackground, true);
        m_bar->setStyleSheet("#AlphabetIndexBar { background: transparent; }");

        m_layout = new QVBoxLayout(m_bar);
        m_layout->setContentsMargins(0, 0, 0, 0);
        m_layout->setSpacing(kSpacingPx);
        m_layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    update();
    updateGeometry();
}

void AlphabetIndexBar::clearViewRightReserve() {
    if (!m_panel) return;
    if (auto* jv = qobject_cast<JustifiedView*>(m_panel->m_gridView)) {
        jv->setRightReservedWidth(0);
    }
    if (m_panel->m_treeView) {
        m_panel->m_treeView->setRightReserve(0);
    }
}

QString AlphabetIndexBar::classifyIndexKey(const QString& itemName) {
    const QString name = itemName.trimmed();
    if (name.isEmpty()) return QStringLiteral("#");

    const QChar first = name.front().toUpper();
    if (first >= QLatin1Char('A') && first <= QLatin1Char('Z')) {
        return QString(first);
    }
    return QStringLiteral("#");
}

void AlphabetIndexBar::update() {
    if (!m_panel || !m_bar || !m_layout) return;

    QAbstractItemView* view = m_panel->activeItemView();
    QAbstractItemModel* model = m_panel->m_sectionModel;
    if (!view || !model || view->model() != model) {
        m_bar->hide();
        clearViewRightReserve();
        return;
    }

    QSet<QString> availableKeys;
    for (int row = 0; row < model->rowCount(); ++row) {
        const QModelIndex index = model->index(row, 0);
        if (!index.isValid() || index.data(SectionHeaderRole).toBool()) continue;
        availableKeys.insert(classifyIndexKey(index.data(Qt::DisplayRole).toString()));
    }

    QStringList orderedKeys;
    for (QChar letter = QLatin1Char('A'); letter <= QLatin1Char('Z'); letter = QChar(letter.unicode() + 1)) {
        const QString key(letter);
        if (availableKeys.contains(key)) orderedKeys.append(key);
    }
    if (availableKeys.contains(QStringLiteral("#"))) orderedKeys.append(QStringLiteral("#"));

    const QSet<QString> existingKeys(m_buttons.keyBegin(), m_buttons.keyEnd());
    const QSet<QString> desiredKeys(orderedKeys.cbegin(), orderedKeys.cend());
    if (existingKeys != desiredKeys) {
        while (QLayoutItem* item = m_layout->takeAt(0)) {
            if (QWidget* widget = item->widget()) widget->deleteLater();
            delete item;
        }
        m_buttons.clear();

        for (const QString& key : orderedKeys) {
            auto* button = new QPushButton(key, m_bar);
            button->setObjectName(QStringLiteral("AlphabetIndexButton_%1").arg(key));
            button->setFocusPolicy(Qt::NoFocus);
            button->setCursor(Qt::PointingHandCursor);
            button->setFixedSize(20, 17);
            button->setStyleSheet(
                "QPushButton { color: #B8B8B8; background: transparent; border: none; "
                "border-radius: 3px; font-size: 10px; padding: 0px; }"
                "QPushButton:hover { color: #FFFFFF; background: transparent; }"
                "QPushButton[active=\"true\"] { color: #ff551c; font-weight: bold; "
                "background: transparent; }");
            connect(button, &QPushButton::clicked, this, [this, key]() {
                jumpToAlphabetKey(key);
            });
            m_layout->addWidget(button, 0, Qt::AlignHCenter);
            m_buttons.insert(key, button);
        }

        for (auto it = m_cycleCursor.begin(); it != m_cycleCursor.end();) {
            if (!desiredKeys.contains(it.key())) it = m_cycleCursor.erase(it);
            else ++it;
        }
    }

    for (auto it = m_buttons.cbegin(); it != m_buttons.cend(); ++it) {
        it.value()->setProperty("active", it.key() == m_activeKey);
        it.value()->style()->unpolish(it.value());
        it.value()->style()->polish(it.value());
    }

    m_bar->setVisible(!orderedKeys.isEmpty());
    updateGeometry();
}

void AlphabetIndexBar::updateGeometry() {
    if (!m_panel || !m_panel->m_viewStack || !m_bar) return;

    QAbstractItemView* view = m_panel->activeItemView();
    if (!view || view->model() != m_panel->m_sectionModel || m_buttons.isEmpty()) {
        m_bar->hide();
        clearViewRightReserve();
        return;
    }

    if (m_layout) {
        m_layout->setSpacing(kSpacingPx);
        m_layout->activate();
    }

    const QSize hint = m_layout ? m_layout->sizeHint() : QSize(22, 0);
    const int width = qMax(22, hint.width());
    const int height = qMin(m_panel->m_viewStack->height(), hint.height());

    // 垂直滚动条宽度（可见时计入，确保 AlphabetIndexBar 与滚动条之间保持 10px 间距）
    int scrollBarW = 0;
    if (QScrollBar* vBar = view->verticalScrollBar()) {
        if (vBar->isVisible()) {
            scrollBarW = qMax(vBar->width(), vBar->sizeHint().width());
        }
    }

    const int barToScrollGap = kSpacingPx; // AlphabetIndexBar 与滚动条间距 = 10px
    const int viewToBarGap = (m_rightMargin > 0) ? m_rightMargin : kSpacingPx; // 视图右边距与 AlphabetIndexBar 间距 = 10px

    const int x = m_panel->m_viewStack->width() - scrollBarW - barToScrollGap - width;
    const int y = qMax(0, (m_panel->m_viewStack->height() - height) / 2);

    // 视图右侧预留宽度 = 10px(索引条与滚动条间距) + 索引条宽度 + 10px(视图右边距与索引条间距)。
    // 滚动条宽度已由视图侧单独扣除（JustifiedView 排版扣除 scrollBarW，QTreeView 视口自带滚动条占位），
    // 此处绝不重复计入 scrollBarW，否则视图与 AlphabetIndexBar 的 10px 间距会被破坏。
    const int reservedRightW = barToScrollGap + width + viewToBarGap;
    if (auto* jv = qobject_cast<JustifiedView*>(m_panel->m_gridView)) {
        jv->setRightReservedWidth(reservedRightW);
    }
    if (m_panel->m_treeView) {
        m_panel->m_treeView->setRightReserve(reservedRightW);
    }

    m_bar->setGeometry(x, y, width, height);
    m_bar->raise();
    m_bar->show();
}

void AlphabetIndexBar::jumpToAlphabetKey(const QString& key) {
    if (!m_panel || !m_panel->m_sectionModel) return;

    QAbstractItemView* view = m_panel->activeItemView();
    QAbstractItemModel* model = m_panel->m_sectionModel;
    if (!view || view->model() != model) return;

    QList<QModelIndex> matches;
    for (int row = 0; row < model->rowCount(); ++row) {
        const QModelIndex index = model->index(row, 0);
        if (!index.isValid() || index.data(SectionHeaderRole).toBool()) continue;
        if (classifyIndexKey(index.data(Qt::DisplayRole).toString()) == key) {
            matches.append(index);
        }
    }
    if (matches.isEmpty()) return;

    const int next = (m_cycleCursor.value(key, -1) + 1) % matches.size();
    m_cycleCursor.insert(key, next);
    m_activeKey = key;

    const QModelIndex target = matches.at(next);
    if (view->selectionModel()) {
        view->selectionModel()->setCurrentIndex(
            target, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    } else {
        view->setCurrentIndex(target);
    }
    view->scrollTo(target, QAbstractItemView::PositionAtTop);
    update();
}

} // namespace QuarkMeta