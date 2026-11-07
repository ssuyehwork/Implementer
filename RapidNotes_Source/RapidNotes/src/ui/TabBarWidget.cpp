#include "TabBarWidget.h"
#include "ContentPanel.h"
#include "UiHelper.h"
#include "StyleLibrary.h"
#include "ColorPicker.h"
#include "HoverEventFilter.h"
#include "../core/AppConfig.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStyle>
#include <QDateTime>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QWidgetAction>
#include <QGridLayout>
#include <QFileInfo>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QUrl>
#include <QApplication>
#include <QScrollBar>

namespace RapidNotes {

static const int kTabFixedDecorationsWidth = 56; // 8+14+6+6+16+6

TabItemButton::TabItemButton(int index, QWidget* parent)
    : QPushButton(parent), m_index(index) {
    setObjectName("TabItem");
    setFocusPolicy(Qt::NoFocus);
    setFixedHeight(28);
    setMaximumWidth(QWIDGETSIZE_MAX); // 彻底放开上限，归一化交给内容驱动
    setMinimumWidth(50);
    setCursor(Qt::PointingHandCursor);

    QHBoxLayout* itemLayout = new QHBoxLayout(this);
    itemLayout->setContentsMargins(8, 0, 6, 0);
    itemLayout->setSpacing(6);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setObjectName("TabIconLabel");
    m_iconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_iconLabel->setFixedSize(14, 14);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("TabTitleLabel");
    m_titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_btnClose = new QPushButton(this);
    m_btnClose->setObjectName("TabCloseBtn");
    m_btnClose->setFocusPolicy(Qt::NoFocus);
    m_btnClose->setFixedSize(16, 16);
    m_btnClose->setIcon(UiHelper::getIcon("close", QColor("#888888")));
    m_btnClose->setIconSize(QSize(10, 10));

    connect(m_btnClose, &QPushButton::clicked, this, [this]() {
        emit closeClicked(m_index);
    });

    itemLayout->addWidget(m_iconLabel, 0, Qt::AlignVCenter);
    itemLayout->addWidget(m_titleLabel, 1, Qt::AlignVCenter);
    itemLayout->addWidget(m_btnClose, 0, Qt::AlignVCenter);
}

void TabItemButton::setTabTitle(const QString& title) {
    m_rawTitle = title;
    updateElidedTitle();
}

void TabItemButton::updateElidedTitle() {
    if (!m_titleLabel || m_rawTitle.isEmpty()) return;
    int availWidth = width() - 44;
    if (availWidth < 10) availWidth = 10;
    QFontMetrics fm(m_titleLabel->font());
    m_titleLabel->setText(fm.elidedText(m_rawTitle, Qt::ElideRight, availWidth));
}

void TabItemButton::resizeEvent(QResizeEvent* event) {
    QPushButton::resizeEvent(event);
    updateElidedTitle();
}

void TabItemButton::setTabIcon(const QIcon& icon) {
    if (m_iconLabel) {
        m_iconLabel->setPixmap(icon.pixmap(14, 14));
    }
}

void TabItemButton::setActive(bool active) {
    setProperty("active", active);
    if (m_titleLabel) {
        m_titleLabel->setProperty("active", active);
        m_titleLabel->style()->unpolish(m_titleLabel);
        m_titleLabel->style()->polish(m_titleLabel);
    }
    style()->unpolish(this);
    style()->polish(this);
}

void TabItemButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPos = event->pos();
        m_isDragging = false;
        event->accept();
        return;
    } else if (event->button() == Qt::MiddleButton) {
        emit middleClicked(m_index);
        event->accept();
        return;
    }
    QPushButton::mousePressEvent(event);
}

void TabItemButton::mouseMoveEvent(QMouseEvent* event) {
    if ((event->buttons() & Qt::LeftButton) && !m_dragStartPos.isNull()) {
        if ((event->pos() - m_dragStartPos).manhattanLength() >= QApplication::startDragDistance()) {
            m_isDragging = true;
            QDrag* drag = new QDrag(this);
            QMimeData* mimeData = new QMimeData();
            mimeData->setData("application/x-rapidnotes-tabindex", QByteArray::number(m_index));

            TabBarWidget* tabBar = nullptr;
            QWidget* current = parentWidget();
            while (current) {
                tabBar = qobject_cast<TabBarWidget*>(current);
                if (tabBar) break;
                current = current->parentWidget();
            }

            if (tabBar) {
                QString url = tabBar->tabUrl(m_index);
                if (!url.isEmpty()) {
                    mimeData->setData("application/x-rapidnotes-taburl", url.toUtf8());
                    mimeData->setText(url);
                }
            }
            drag->setMimeData(mimeData);
            drag->setPixmap(grab());

            QPoint clampedHotSpot = m_dragStartPos;
            clampedHotSpot.setX(qBound(0, clampedHotSpot.x(), width() - 1));
            clampedHotSpot.setY(qBound(0, clampedHotSpot.y(), height() - 1));
            drag->setHotSpot(clampedHotSpot);

            drag->exec(Qt::MoveAction);
            m_dragStartPos = QPoint();
            return;
        }
    }
    QPushButton::mouseMoveEvent(event);
}

void TabItemButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (!m_isDragging) {
            emit tabClicked(m_index);
        }
        m_isDragging = false;
        event->accept();
        return;
    }
    QPushButton::mouseReleaseEvent(event);
}

void TabItemButton::contextMenuEvent(QContextMenuEvent* event) {
    emit customContextMenuRequested(m_index, event->globalPos());
    event->accept();
}

TabBarWidget::TabBarWidget(QWidget* parent, HoverEventFilter* hoverFilter)
    : QWidget(parent), m_hoverFilter(hoverFilter) {
    setObjectName("TabBarWidget");
    setAttribute(Qt::WA_StyledBackground, true);
    setAcceptDrops(true);
    setFixedHeight(30);

    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setObjectName("TabBarScrollArea");
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setFocusPolicy(Qt::NoFocus);

    m_tabsContainer = new QWidget(m_scrollArea);
    m_tabsContainer->setObjectName("TabBarContainer");

    m_tabsContainerLayout = new QHBoxLayout(m_tabsContainer);
    m_tabsContainerLayout->setContentsMargins(0, 0, 0, 0);
    m_tabsContainerLayout->setSpacing(4);

    m_tabsLayout = new QHBoxLayout();
    m_tabsLayout->setContentsMargins(0, 2, 0, 0);
    m_tabsLayout->setSpacing(2);

    m_btnNewTab = new QPushButton(m_tabsContainer);
    m_btnNewTab->setFocusPolicy(Qt::NoFocus);
    m_btnNewTab->setAttribute(Qt::WA_Hover, true);
    m_btnNewTab->setFixedSize(22, 22);
    m_btnNewTab->setIcon(UiHelper::getIcon("add", QColor("#EEEEEE")));
    m_btnNewTab->setIconSize(QSize(14, 14));
    m_btnNewTab->setObjectName("NewTabBtn");
    m_btnNewTab->setProperty("tooltipText", "新建标签页 (Ctrl+T)");
    if (m_hoverFilter) {
        m_btnNewTab->installEventFilter(m_hoverFilter);
    }

    connect(m_btnNewTab, &QPushButton::clicked, this, [this]() {
        addTab("全部数据", "library://-1", true);
        emit newTabRequested();
    });

    m_tabsContainerLayout->addLayout(m_tabsLayout);
    m_tabsContainerLayout->addWidget(m_btnNewTab, 0, Qt::AlignVCenter);
    m_tabsContainerLayout->addStretch(1);

    m_scrollArea->setWidget(m_tabsContainer);
    m_mainLayout->addWidget(m_scrollArea, 1);

    addTab("全部数据", "library://-1", true);
}

void TabBarWidget::wheelEvent(QWheelEvent* event) {
    if (m_scrollArea && m_scrollArea->horizontalScrollBar()) {
        int delta = event->angleDelta().y();
        if (delta == 0) delta = event->angleDelta().x();
        if (delta != 0) {
            m_scrollArea->horizontalScrollBar()->setValue(
                m_scrollArea->horizontalScrollBar()->value() - delta);
            event->accept();
            return;
        }
    }
    QWidget::wheelEvent(event);
}

void TabBarWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    applyDiffUpdate();
}

static QString cleanNameForTab(const QString& path) {
    if (path.isEmpty() || path == "library://-1") return "全部数据";
    if (path == "library://-2") return "未分类";
    if (path == "trash://") return "回收站";
    if (path.startsWith("library://") || path.startsWith("smartfolder://")) {
        return "分类";
    }
    QString cleanP = QDir::cleanPath(path);
    QFileInfo fi(cleanP);
    if (fi.isRoot() || cleanP.endsWith(":\\") || cleanP.endsWith(":/") || (cleanP.length() == 2 && cleanP.endsWith(':'))) {
        return cleanP;
    }
    QString fn = fi.fileName();
    return fn.isEmpty() ? cleanP : fn;
}

static int paneCountForTab(const TabInfo& tab) {
    return (tab.splitState.isSplit && !tab.splitState.panePaths.isEmpty())
        ? tab.splitState.panePaths.size()
        : 1;
}

void TabBarWidget::addTab(const QString& title, const QString& url, bool switchToNew) {
    TabInfo info;
    info.id = QString::number(QDateTime::currentMSecsSinceEpoch()) + "_" + QString::number(m_tabs.size());
    info.title = title.isEmpty() ? "全部数据" : title;
    info.url = url.isEmpty() ? "library://-1" : url;
    info.active = false;

    m_tabs.append(info);
    if (switchToNew || m_currentIndex == -1) {
        setCurrentIndex(m_tabs.size() - 1, true);
    } else {
        applyDiffUpdate();
    }
    saveStateToConfig();
}

void TabBarWidget::closeTab(int index) {
    if (index < 0 || index >= m_tabs.size()) return;
    if (m_tabs.size() <= 1) {
        m_tabs[0].title = "全部数据";
        m_tabs[0].url = "library://-1";
        m_tabs[0].splitState = TabSplitState();
        applyDiffUpdate();
        emit currentTabChanged(0, "library://-1");
        saveStateToConfig();
        return;
    }

    m_closedTabsHistory.append(m_tabs[index]);
    m_tabs.removeAt(index);
    if (index < m_currentIndex) {
        m_currentIndex--;
    } else if (m_currentIndex >= m_tabs.size()) {
        m_currentIndex = m_tabs.size() - 1;
    }
    setCurrentIndex(m_currentIndex, true);
    emit tabClosed(index);
    saveStateToConfig();
}

void TabBarWidget::closeOtherTabs(int index) {
    if (index < 0 || index >= m_tabs.size()) return;
    TabInfo target = m_tabs[index];
    for (int i = 0; i < m_tabs.size(); ++i) {
        if (i != index) m_closedTabsHistory.append(m_tabs[i]);
    }
    m_tabs.clear();
    m_tabs.append(target);
    setCurrentIndex(0, true);
}

void TabBarWidget::closeRightTabs(int index) {
    if (index < 0 || index >= m_tabs.size() - 1) return;
    while (m_tabs.size() > index + 1) {
        m_closedTabsHistory.append(m_tabs.takeAt(index + 1));
    }
    if (m_currentIndex > index) {
        m_currentIndex = index;
    }
    setCurrentIndex(m_currentIndex, true);
}

void TabBarWidget::duplicateTab(int index) {
    if (index < 0 || index >= m_tabs.size()) return;
    const auto& src = m_tabs[index];
    addTab(src.title, src.url, true);
}

void TabBarWidget::mergeTab(int sourceIndex, int targetIndex) {
    if (sourceIndex < 0 || sourceIndex >= m_tabs.size()) return;
    if (targetIndex < 0 || targetIndex >= m_tabs.size()) return;
    if (sourceIndex == targetIndex) return;

    const auto& sourceTab = m_tabs[sourceIndex];
    const auto& targetTab = m_tabs[targetIndex];

    QStringList sourcePaths = (sourceTab.splitState.isSplit && !sourceTab.splitState.panePaths.isEmpty())
        ? sourceTab.splitState.panePaths
        : QStringList{sourceTab.url};

    QStringList targetPaths = (targetTab.splitState.isSplit && !targetTab.splitState.panePaths.isEmpty())
        ? targetTab.splitState.panePaths
        : QStringList{targetTab.url};

    QStringList mergedPaths = targetPaths + sourcePaths;
    TabSplitState mergedState;
    mergedState.isSplit = true;
    mergedState.orientation = targetTab.splitState.isSplit ? targetTab.splitState.orientation : Qt::Horizontal;
    mergedState.panePaths = mergedPaths;
    mergedState.primaryIndex = 0;
    mergedState.activePaneIndex = 0;

    m_tabs[targetIndex].splitState = mergedState;
    m_tabs[targetIndex].url = mergedPaths.first();

    QStringList formattedNames;
    for (const QString& p : mergedPaths) {
        formattedNames.append(cleanNameForTab(p));
    }
    m_tabs[targetIndex].title = formattedNames.join(" | ");

    m_tabs.removeAt(sourceIndex);

    int newTargetIdx = targetIndex;
    if (sourceIndex < targetIndex) newTargetIdx--;

    setCurrentIndex(newTargetIdx, true);
    saveStateToConfig();
}

void TabBarWidget::splitTab(int tabIndex) {
    if (tabIndex < 0 || tabIndex >= m_tabs.size()) return;

    TabInfo tab = m_tabs[tabIndex];
    QStringList paths = (tab.splitState.isSplit && !tab.splitState.panePaths.isEmpty())
        ? tab.splitState.panePaths
        : QStringList{tab.url};

    if (paths.size() <= 1) return;

    int addedCount = paths.size() - 1;

    m_tabs[tabIndex].url = paths.first();
    m_tabs[tabIndex].title = cleanNameForTab(paths.first());
    m_tabs[tabIndex].splitState = TabSplitState();

    for (int i = 1; i < paths.size(); ++i) {
        TabInfo newTab;
        newTab.id = QString::number(QDateTime::currentMSecsSinceEpoch()) + "_" + QString::number(m_tabs.size()) + "_" + QString::number(i);
        newTab.url = paths[i];
        newTab.title = cleanNameForTab(paths[i]);
        newTab.active = false;
        m_tabs.insert(tabIndex + i, newTab);
    }

    if (m_currentIndex > tabIndex) {
        m_currentIndex += addedCount;
    }

    setCurrentIndex(m_currentIndex, true);
    saveStateToConfig();
}

void TabBarWidget::restoreLastClosedTab() {
    if (m_closedTabsHistory.isEmpty()) return;
    TabInfo lastTab = m_closedTabsHistory.takeLast();
    addTab(lastTab.title, lastTab.url, true);
}

void TabBarWidget::setCurrentIndex(int index, bool forceNotify) {
    if (index < 0 || index >= m_tabs.size()) return;
    bool indexChanged = (m_currentIndex != index);
    if (indexChanged && m_currentIndex >= 0 && m_currentIndex < m_tabs.size()) {
        emit tabAboutToChange(m_currentIndex);
    }
    m_currentIndex = index;
    for (int i = 0; i < m_tabs.size(); ++i) {
        m_tabs[i].active = (i == m_currentIndex);
    }
    
    // 🚀【归一化收口】：所有状态变更一律通过 applyDiffUpdate 重新按内容测量并重排！
    applyDiffUpdate();

    if (indexChanged || forceNotify) {
        emit currentTabChanged(m_currentIndex, m_tabs[m_currentIndex].url);
    }
    saveStateToConfig();
}

void TabBarWidget::updateSplitTabTitle(const TabSplitState& state) {
    if (m_currentIndex < 0 || m_currentIndex >= m_tabs.size()) return;

    auto& currentTab = m_tabs[m_currentIndex];

    // 🛡️ 门禁：严禁单窗格状态反噬多窗格合并状态
    if (currentTab.splitState.isSplit && currentTab.splitState.panePaths.size() > 1 && !state.isSplit) {
        return;
    }

    currentTab.splitState = state;

    if (state.isSplit && !state.panePaths.isEmpty()) {
        QStringList nameList;
        for (const QString& p : state.panePaths) {
            if (!p.isEmpty()) nameList.append(cleanNameForTab(p));
        }
        if (!nameList.isEmpty()) {
            currentTab.title = nameList.join(" | ");
        }
        int primIdx = qBound(0, state.primaryIndex, state.panePaths.size() - 1);
        currentTab.url = state.panePaths[primIdx];
    } else if (!state.panePaths.isEmpty()) {
        currentTab.title = cleanNameForTab(state.panePaths.first());
        currentTab.url = state.panePaths.first();
    }

    applyDiffUpdate();
    saveStateToConfig();
}

void TabBarWidget::selectNextTab() {
    if (m_tabs.isEmpty()) return;
    setCurrentIndex((m_currentIndex + 1) % m_tabs.size(), true);
}

void TabBarWidget::selectPreviousTab() {
    if (m_tabs.isEmpty()) return;
    setCurrentIndex((m_currentIndex - 1 + m_tabs.size()) % m_tabs.size(), true);
}

void TabBarWidget::saveStateToConfig() {
    if (m_isInitializing) return;

    QJsonArray tabArray;
    for (const auto& tab : m_tabs) {
        QJsonObject obj;
        obj["title"] = tab.title;
        obj["url"] = tab.url;
        obj["color"] = tab.color;
        obj["iconKey"] = tab.iconKey;

        std::function<QJsonObject(const PaneTreeNode&)> serializeTreeNode = [&](const PaneTreeNode& node) -> QJsonObject {
            QJsonObject nObj;
            nObj["isSplitter"] = node.isSplitter;
            nObj["orientation"] = static_cast<int>(node.orientation);
            nObj["path"] = node.path;
            nObj["isPrimary"] = node.isPrimary;
            if (node.isSplitter) {
                QJsonArray childrenArray;
                for (const auto& child : node.children) {
                    childrenArray.append(serializeTreeNode(child));
                }
                nObj["children"] = childrenArray;
            }
            return nObj;
        };

        QJsonObject splitObj;
        splitObj["orientation"] = static_cast<int>(tab.splitState.orientation);
        QJsonArray pathsArray;
        for (const QString& p : tab.splitState.panePaths) pathsArray.append(p);
        splitObj["panePaths"] = pathsArray;
        splitObj["primaryIndex"] = tab.splitState.primaryIndex;
        splitObj["activePaneIndex"] = tab.splitState.activePaneIndex;
        splitObj["isSplit"] = tab.splitState.isSplit;
        splitObj["treeNode"] = serializeTreeNode(tab.splitState.rootNode);
        obj["splitState"] = splitObj;

        tabArray.append(obj);
    }

    QJsonObject stateObj;
    stateObj["tabs"] = tabArray;
    stateObj["currentIndex"] = m_currentIndex;

    QString jsonStr = QString::fromUtf8(QJsonDocument(stateObj).toJson(QJsonDocument::Compact));
    AppConfig::instance().setValue("TabBar/SavedState", jsonStr);
}

bool TabBarWidget::restoreStateFromConfig() {
    m_isInitializing = true;
    QString jsonStr = AppConfig::instance().getValue("TabBar/SavedState").toString();
    if (jsonStr.isEmpty()) {
        m_isInitializing = false;
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isObject()) {
        m_isInitializing = false;
        return false;
    }

    QJsonObject stateObj = doc.object();
    QJsonArray tabArray = stateObj["tabs"].toArray();
    if (tabArray.isEmpty()) {
        m_isInitializing = false;
        return false;
    }

    m_tabs.clear();
    for (const auto& val : tabArray) {
        QJsonObject obj = val.toObject();
        TabInfo info;
        info.id = QString::number(QDateTime::currentMSecsSinceEpoch()) + "_" + QString::number(m_tabs.size());
        info.title = obj["title"].toString("全部数据");
        info.url = obj["url"].toString("library://-1");
        info.color = obj["color"].toString();
        info.iconKey = obj["iconKey"].toString();
        info.active = false;

        std::function<PaneTreeNode(const QJsonObject&)> deserializeTreeNode = [&](const QJsonObject& nObj) -> PaneTreeNode {
            PaneTreeNode node;
            node.isSplitter = nObj["isSplitter"].toBool(false);
            node.orientation = static_cast<Qt::Orientation>(nObj["orientation"].toInt(static_cast<int>(Qt::Horizontal)));
            node.path = nObj["path"].toString();
            node.isPrimary = nObj["isPrimary"].toBool(false);
            if (node.isSplitter && nObj.contains("children")) {
                QJsonArray childrenArray = nObj["children"].toArray();
                for (const auto& cVal : childrenArray) {
                    node.children.append(deserializeTreeNode(cVal.toObject()));
                }
            }
            return node;
        };

        if (obj.contains("splitState") && obj["splitState"].isObject()) {
            QJsonObject splitObj = obj["splitState"].toObject();
            info.splitState.orientation = static_cast<Qt::Orientation>(splitObj["orientation"].toInt(static_cast<int>(Qt::Horizontal)));
            info.splitState.primaryIndex = splitObj["primaryIndex"].toInt(0);
            info.splitState.activePaneIndex = splitObj["activePaneIndex"].toInt(0);
            info.splitState.isSplit = splitObj["isSplit"].toBool(false);
            QJsonArray pathsArray = splitObj["panePaths"].toArray();
            for (const auto& pVal : pathsArray) info.splitState.panePaths.append(pVal.toString());
            if (splitObj.contains("treeNode") && splitObj["treeNode"].isObject()) {
                info.splitState.rootNode = deserializeTreeNode(splitObj["treeNode"].toObject());
            }
        }

        m_tabs.append(info);
    }

    int savedIndex = stateObj["currentIndex"].toInt(0);
    if (savedIndex < 0 || savedIndex >= m_tabs.size()) savedIndex = 0;

    m_currentIndex = savedIndex;
    for (int i = 0; i < m_tabs.size(); ++i) {
        m_tabs[i].active = (i == m_currentIndex);
    }

    applyDiffUpdate();
    m_isInitializing = false;

    if (m_currentIndex >= 0 && m_currentIndex < m_tabs.size()) {
        emit currentTabChanged(m_currentIndex, m_tabs[m_currentIndex].url);
    }
    return true;
}

void TabBarWidget::openInNewTab(const QString& rawPath) {
    if (rawPath.isEmpty()) return;
    QString cleanTarget = (rawPath == "computer://" || rawPath == "trash://") ? rawPath : QDir::cleanPath(rawPath);
    addTab(cleanNameForTab(cleanTarget), cleanTarget, true);
}

void TabBarWidget::openInExistingTab(int tabIndex, const QString& path) {
    if (tabIndex < 0 || tabIndex >= m_tabs.size() || path.trimmed().isEmpty()) return;

    setCurrentIndex(tabIndex, true);
    QWidget* windowWidget = window();
    ContentPanel* contentPanel = windowWidget ? windowWidget->findChild<ContentPanel*>() : nullptr;
    if (contentPanel) {
        contentPanel->splitPane(Qt::Horizontal, path, false);
    }
}

void TabBarWidget::openOrFocusTab(const QString& rawPath) {
    if (rawPath.isEmpty()) return;
    QString cleanTarget = QDir::cleanPath(rawPath);

    for (int i = 0; i < m_tabs.size(); ++i) {
        if (QDir::cleanPath(m_tabs[i].url) == cleanTarget) {
            setCurrentIndex(i, true);
            return;
        }
    }

    addTab(cleanNameForTab(cleanTarget), cleanTarget, true);
}

void TabBarWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData() && (event->mimeData()->hasFormat("application/x-rapidnotes-tabindex") || event->mimeData()->hasUrls())) {
        event->acceptProposedAction();
    } else {
        QWidget::dragEnterEvent(event);
    }
}

void TabBarWidget::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData() && (event->mimeData()->hasFormat("application/x-rapidnotes-tabindex") || event->mimeData()->hasUrls())) {
        event->acceptProposedAction();
    } else {
        QWidget::dragMoveEvent(event);
    }
}

void TabBarWidget::dropEvent(QDropEvent* event) {
    if (event->mimeData()) {
        if (event->mimeData()->hasFormat("application/x-rapidnotes-tabindex")) {
            int fromIdx = event->mimeData()->data("application/x-rapidnotes-tabindex").toInt();
            if (fromIdx >= 0 && fromIdx < m_tabs.size()) {
                QPoint dropPos = event->position().toPoint();
                int toIdx = m_tabs.size() - 1;
                for (int i = 0; i < m_tabWidgets.size(); ++i) {
                    if (dropPos.x() < m_tabWidgets[i]->geometry().center().x()) {
                        toIdx = i;
                        break;
                    }
                }

                if (fromIdx != toIdx) {
                    TabInfo movedTab = m_tabs.takeAt(fromIdx);
                    m_tabs.insert(toIdx, movedTab);

                    if (m_currentIndex == fromIdx) {
                        m_currentIndex = toIdx;
                    } else if (m_currentIndex > fromIdx && m_currentIndex <= toIdx) {
                        m_currentIndex--;
                    } else if (m_currentIndex < fromIdx && m_currentIndex >= toIdx) {
                        m_currentIndex++;
                    }

                    for (int i = 0; i < m_tabs.size(); ++i) {
                        m_tabs[i].active = (i == m_currentIndex);
                    }

                    applyDiffUpdate();
                    saveStateToConfig();
                }
            }
            event->acceptProposedAction();
            return;
        } else if (event->mimeData()->hasUrls()) {
            for (const QUrl& url : event->mimeData()->urls()) {
                QString path = url.toLocalFile();
                if (!path.isEmpty() && QFileInfo(path).isDir()) {
                    QPoint dropPos = event->position().toPoint();
                    int hitTabIdx = -1;
                    for (int i = 0; i < m_tabWidgets.size(); ++i) {
                        if (m_tabWidgets[i] && m_tabWidgets[i]->geometry().contains(dropPos)) {
                            hitTabIdx = i;
                            break;
                        }
                    }

                    if (hitTabIdx != -1) {
                        emit folderDroppedOnTab(hitTabIdx, path);
                    } else {
                        openOrFocusTab(path);
                    }
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }
    QWidget::dropEvent(event);
}

void TabBarWidget::updateCurrentTabTitle(const QString& title, const QString& url) {
    if (m_currentIndex < 0 || m_currentIndex >= m_tabs.size()) return;

    auto& currentTab = m_tabs[m_currentIndex];

    // 🛡️ 门禁：多分屏合并标签绝不降级，且路径安全更新
    if (currentTab.splitState.isSplit && currentTab.splitState.panePaths.size() > 1) {
        int activeIdx = qBound(0, currentTab.splitState.activePaneIndex, currentTab.splitState.panePaths.size() - 1);
        currentTab.splitState.panePaths[activeIdx] = url;

        QStringList nameList;
        for (const QString& p : currentTab.splitState.panePaths) {
            if (!p.isEmpty()) nameList.append(cleanNameForTab(p));
        }
        if (!nameList.isEmpty()) {
            currentTab.title = nameList.join(" | ");
        }
        currentTab.url = url;
    } else {
        QString folderName = title;
        if (folderName.startsWith("库 → ")) {
            folderName = folderName.mid(4);
        }
        if (url == "library://-1" || url == "computer://" || url.isEmpty()) {
            folderName = "全部数据";
        } else if (url == "library://-2") {
            folderName = "未分类";
        } else if (!url.startsWith("library://") && !url.startsWith("smartfolder://") && (title.contains("/") || title.contains("\\"))) {
            folderName = cleanNameForTab(url);
        }
        currentTab.title = folderName.isEmpty() ? "全部数据" : folderName;
        currentTab.url = url;
    }

    applyDiffUpdate();
    saveStateToConfig();
}

void TabBarWidget::showTabContextMenu(int index, const QPoint& globalPos) {
    if (index < 0 || index >= m_tabs.size()) return;

    const auto& tab = m_tabs[index];

    QMenu menu(this);
    menu.setObjectName("TabContextMenu");
    UiHelper::applyMenuStyle(&menu);

    QString curColorHex = tab.color.isEmpty() ? "#888888" : tab.color;
    QWidgetAction* colorPickerAction = new QWidgetAction(&menu);
    ColorStripPicker* colorPickerWidget = new ColorStripPicker(curColorHex, &menu);
    colorPickerAction->setDefaultWidget(colorPickerWidget);
    menu.addAction(colorPickerAction);

    QMenu* iconMenu = menu.addMenu(UiHelper::getIcon(tab.iconKey.isEmpty() ? "folder_filled" : tab.iconKey, QColor("#EEEEEE")), "切换图标");
    UiHelper::applyMenuStyle(iconMenu);

    QWidgetAction* pickerAction = new QWidgetAction(iconMenu);
    QWidget* pickerWidget = new QWidget(iconMenu);
    QGridLayout* pickerLayout = new QGridLayout(pickerWidget);
    pickerLayout->setContentsMargins(6, 6, 6, 6);
    pickerLayout->setSpacing(6);

    static const QList<QPair<QString, QString>> builtInIcons = {
        {"默认文件夹", "folder_filled"}, {"照片媒体", "image_filled"}, {"相册图片", "image_picture"},
        {"时钟历史", "clock_filled"}, {"星标收藏", "star_filled"}, {"实心星标", "star_001"},
        {"空心星标", "star_002"}, {"爱心常用", "heart_filled"}, {"加密安全", "lock_filled"},
        {"图书文档", "book"}, {"附加文档", "document_attach"}, {"配置管理", "settings_filled"},
        {"网络球体", "globe_filled"}, {"主页主路径", "home_filled"}, {"标签标记", "tag_filled"},
        {"书签指示", "bookmark_filled"}, {"音频音乐", "music_filled"}, {"视频影视", "video_filled"},
        {"摄影相机", "camera_filled"}, {"盾牌防护", "shield_filled"}, {"物理硬盘", "hard_drive"},
        {"云端同步", "cloud_filled"}, {"闪电极速", "zap_filled"}, {"魔法火花", "sparkles_filled"},
        {"旗帜标记", "flag_filled"}, {"旗帜标示", "flag"}, {"礼物珍藏", "gift_filled"},
        {"奖星勋章", "award_filled"}, {"回收废弃", "trash_filled"}, {"邮件通信", "mail_filled"},
        {"消息通知", "message_filled"}, {"电话联系", "phone_filled"}, {"地理定位", "map_pin_filled"},
        {"日光白天", "sun_filled"}, {"夜间月亮", "moon_filled"}, {"日历日程", "calendar_filled"},
        {"今日任务", "today_filled"}, {"九宫网格", "grid_filled"}, {"布局排版", "layout_filled"},
        {"数据表格", "table_filled"}, {"磁盘保存", "save_filled"}, {"魔棒工具", "wand_filled"},
        {"附件剪辑", "paperclip"}, {"归档文件", "archive"},
        {"OneNote笔记", "onenote"}, {"下载中心", "download"}
    };

    QColor catColor = QColor(curColorHex);
    QList<QPair<QPushButton*, QString>> iconButtons;
    int row = 0, col = 0;
    for (const auto& pair : builtInIcons) {
        QString iconKey = pair.second;
        QPushButton* btn = new QPushButton(pickerWidget);
        btn->setFixedSize(28, 28);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setObjectName("FavPickerIconBtn");
        btn->setIcon(UiHelper::getIcon(iconKey, catColor, 18));
        btn->setIconSize(QSize(18, 18));
        pickerLayout->addWidget(btn, row, col);

        iconButtons.append({btn, iconKey});

        connect(btn, &QPushButton::clicked, this, [this, index, iconKey]() {
            if (index >= 0 && index < m_tabs.size()) {
                m_tabs[index].iconKey = iconKey;
                applyDiffUpdate();
                emit tabIconChanged(index, m_tabs[index].url, iconKey);
            }
        });

        col++;
        if (col >= 5) { col = 0; row++; }
    }

    pickerWidget->setLayout(pickerLayout);
    pickerAction->setDefaultWidget(pickerWidget);
    iconMenu->addAction(pickerAction);

    connect(colorPickerWidget, &ColorStripPicker::colorSelected, this, [this, index, iconMenu, iconButtons](const QString& hexColor) {
        if (index < 0 || index >= m_tabs.size()) return;

        QString finalColor = hexColor.isEmpty() ? "#888888" : hexColor.toUpper();
        m_tabs[index].color = finalColor;
        QString iconKey = m_tabs[index].iconKey.isEmpty() ? "folder_filled" : m_tabs[index].iconKey;

        iconMenu->setIcon(UiHelper::getIcon(iconKey, QColor(finalColor)));
        for (const auto& btnPair : iconButtons) {
            btnPair.first->setIcon(UiHelper::getIcon(btnPair.second, QColor(finalColor), 18));
        }

        applyDiffUpdate();
        emit tabColorChanged(index, m_tabs[index].url, finalColor);
    });

    menu.addSeparator();

    QAction* actRefresh = menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 16), "重新加载 (F5)");
    QAction* actDuplicate = menu.addAction(UiHelper::getIcon("copy", QColor("#EEEEEE"), 16), "复制标签页");

    QMenu* mergeMenu = menu.addMenu(UiHelper::getIcon("layout_filled", QColor("#EEEEEE"), 16), "合并到");
    UiHelper::applyMenuStyle(mergeMenu);
    for (int i = 0; i < m_tabs.size(); ++i) {
        if (i != index) {
            QAction* mergeAct = mergeMenu->addAction(m_tabs[i].title);
            connect(mergeAct, &QAction::triggered, this, [this, index, i]() {
                emit mergeRequested(index, i);
            });
        }
    }
    mergeMenu->setEnabled(m_tabs.size() > 1);

    int paneCount = (index >= 0 && index < m_tabs.size()) ? paneCountForTab(m_tabs[index]) : 1;
    QAction* actSplit = menu.addAction(UiHelper::getIcon("grid_filled", QColor("#EEEEEE"), 16), "拆分标签页");
    actSplit->setEnabled(paneCount > 1);
    connect(actSplit, &QAction::triggered, this, [this, index]() {
        emit splitRequested(index);
    });

    menu.addSeparator();
    QAction* actClose = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE"), 16), "关闭标签页 (Ctrl+W)");
    QAction* actCloseOthers = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE"), 16), "关闭其他标签页");
    QAction* actCloseRight = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE"), 16), "关闭右侧标签页");
    menu.addSeparator();
    QAction* actRestore = menu.addAction(UiHelper::getIcon("history", QColor("#EEEEEE"), 16), "重新打开关闭的标签页 (Ctrl+Shift+T)");

    actRestore->setEnabled(!m_closedTabsHistory.isEmpty());
    actCloseRight->setEnabled(index < m_tabs.size() - 1);
    actCloseOthers->setEnabled(m_tabs.size() > 1);

    connect(actRefresh, &QAction::triggered, this, [this]() { emit refreshRequested(); });
    connect(actDuplicate, &QAction::triggered, this, [this, index]() { duplicateTab(index); });
    connect(actClose, &QAction::triggered, this, [this, index]() { closeTab(index); });
    connect(actCloseOthers, &QAction::triggered, this, [this, index]() { closeOtherTabs(index); });
    connect(actCloseRight, &QAction::triggered, this, [this, index]() { closeRightTabs(index); });
    connect(actRestore, &QAction::triggered, this, [this]() { restoreLastClosedTab(); });

    menu.exec(globalPos);
}

void TabBarWidget::applyDiffUpdate() {
    int targetCount = m_tabs.size();

    // 1. 同步组件实例数量
    while (m_tabWidgets.size() > targetCount) {
        TabItemButton* lastBtn = m_tabWidgets.takeLast();
        m_tabsLayout->removeWidget(lastBtn);
        lastBtn->deleteLater();
    }
    for (int i = m_tabWidgets.size(); i < targetCount; ++i) {
        TabItemButton* tabItem = new TabItemButton(i, m_tabsContainer);

        connect(tabItem, &TabItemButton::closeClicked, this, [this](int idx) { closeTab(idx); });
        connect(tabItem, &TabItemButton::middleClicked, this, [this](int idx) { closeTab(idx); });
        connect(tabItem, &TabItemButton::tabClicked, this, [this](int idx) { setCurrentIndex(idx, false); });
        connect(tabItem, &TabItemButton::customContextMenuRequested, this, [this](int idx, const QPoint& globalPos) {
            showTabContextMenu(idx, globalPos);
        });

        m_tabWidgets.append(tabItem);
        m_tabsLayout->addWidget(tabItem);
    }

    // 2. 纯内容驱动自适应测量（彻底解除 300/600 限制，内容有多长就占多长）
    QVector<int> naturalWidths(targetCount);
    int totalNaturalWidth = 0;

    for (int i = 0; i < targetCount; ++i) {
        QFontMetrics fm(m_tabWidgets[i]->font());
        int textW = fm.horizontalAdvance(m_tabs[i].title);
        int needed = textW + kTabFixedDecorationsWidth;
        if (needed < 60) needed = 60; // 仅设最低兜底，上不封顶

        naturalWidths[i] = needed;
        totalNaturalWidth += needed;
    }

    if (targetCount > 1) {
        totalNaturalWidth += (targetCount - 1) * m_tabsLayout->spacing();
    }

    // 3. 视口空间与压缩计算
    int viewportWidth = m_scrollArea->viewport()->width();
    int availableWidth = (viewportWidth > 200) ? (viewportWidth - 36) : (width() - 36);

    double scale = (totalNaturalWidth > availableWidth && totalNaturalWidth > 0)
        ? static_cast<double>(availableWidth) / totalNaturalWidth
        : 1.0;

    // 4. 统一渲染状态与几何尺寸
    for (int i = 0; i < targetCount; ++i) {
        auto& tab = m_tabs[i];
        TabItemButton* tabBtn = m_tabWidgets[i];

        int finalWidth = (scale < 1.0)
            ? qMax(50, static_cast<int>(naturalWidths[i] * scale))
            : naturalWidths[i];

        tabBtn->setIndex(i);
        tabBtn->setTabTitle(tab.title);
        tabBtn->setFixedWidth(finalWidth);

        QColor iconColor = !tab.color.isEmpty() ? QColor(tab.color) : (tab.active ? QColor("#EEEEEE") : QColor("#888888"));
        QString iconKey = !tab.iconKey.isEmpty() ? tab.iconKey : (tab.url.startsWith("computer://") ? "computer" : "folder_filled");
        tabBtn->setTabIcon(UiHelper::getIcon(iconKey, iconColor));
        tabBtn->setActive(tab.active);
    }

    if (m_currentIndex >= 0 && m_currentIndex < m_tabWidgets.size()) {
        m_scrollArea->ensureWidgetVisible(m_tabWidgets[m_currentIndex], 10, 0);
    }
}

} // namespace RapidNotes