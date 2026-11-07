#include "ContentPaneSplitManager.h"
#include "../ContentPanel.h"
#include "../ContentHeaderWidget.h"
#include "../TabBarWidget.h"
#include "ToolTipOverlay.h"
#include <QHBoxLayout>
#include <QCursor>
#include <QVBoxLayout>
#include <QStyle>
#include <QDir>

namespace QuarkMeta {

ContentPaneSplitManager::ContentPaneSplitManager(ContentPanel* panel)
    : QObject(panel), m_panel(panel)
{
}

ContentPanel* ContentPaneSplitManager::secondaryContentPanel() const {
    QList<ContentPanel*> list = panes();
    for (ContentPanel* p : list) {
        if (p != m_panel) return p;
    }
    return nullptr;
}

ContentPanel* ContentPaneSplitManager::rootPane() const {
    return m_rootPane ? m_rootPane : m_panel;
}

bool ContentPaneSplitManager::isSplitMode() const {
    if (rootPane() != m_panel) {
        return rootPane()->splitManager()->isSplitMode();
    }
    return paneCount() > 1;
}

Qt::Orientation ContentPaneSplitManager::splitOrientation() const {
    if (rootPane() != m_panel) {
        return rootPane()->splitManager()->splitOrientation();
    }
    if (m_rootNode && m_rootNode->isSplitter) {
        return m_rootNode->orientation;
    }
    return m_splitOrientation;
}

void ContentPaneSplitManager::collectPanesDepthFirst(PaneNode* node, QList<ContentPanel*>& list) const {
    if (!node) return;
    if (!node->isSplitter) {
        if (node->panel) list.append(node->panel);
    } else {
        for (PaneNode* child : node->children) {
            collectPanesDepthFirst(child, list);
        }
    }
}

void ContentPaneSplitManager::collectNodesDepthFirst(PaneNode* node, QList<PaneNode*>& list) const {
    if (!node) return;
    list.append(node);
    if (node->isSplitter) {
        for (PaneNode* child : node->children) {
            collectNodesDepthFirst(child, list);
        }
    }
}

ContentPaneSplitManager::PaneNode* ContentPaneSplitManager::findNodeForPanel(PaneNode* node, ContentPanel* panel) const {
    if (!node || !panel) return nullptr;
    if (!node->isSplitter && node->panel == panel) return node;
    if (node->isSplitter) {
        for (PaneNode* child : node->children) {
            PaneNode* res = findNodeForPanel(child, panel);
            if (res) return res;
        }
    }
    return nullptr;
}

QList<ContentPanel*> ContentPaneSplitManager::panes() const {
    if (rootPane() != m_panel) {
        return rootPane()->splitManager()->panes();
    }
    QList<ContentPanel*> list;
    if (m_rootNode) {
        collectPanesDepthFirst(m_rootNode, list);
        list.removeAll(m_panel);
    }
    return list;
}

int ContentPaneSplitManager::paneCount() const {
    if (rootPane() != m_panel) {
        return rootPane()->splitManager()->paneCount();
    }
    QList<ContentPanel*> list;
    if (m_rootNode) {
        collectPanesDepthFirst(m_rootNode, list);
    } else {
        list.append(m_panel);
    }
    return list.size();
}

void ContentPaneSplitManager::deleteNodeRecursive(PaneNode* node) {
    if (!node) return;
    for (PaneNode* child : node->children) {
        deleteNodeRecursive(child);
    }
    delete node;
}

void ContentPaneSplitManager::normalizeTree(PaneNode* node) {
    if (!node) return;

    if (node->isSplitter) {
        for (int i = node->children.size() - 1; i >= 0; --i) {
            normalizeTree(node->children[i]);
        }

        // 规范化 1：展平相同方向的子 Splitter 节点，并重建 QSplitter 挂载关系
        QList<PaneNode*> newChildren;
        for (PaneNode* child : node->children) {
            if (child && child->isSplitter && child->orientation == node->orientation) {
                if (node->splitter && child->splitter) {
                    int childIdx = node->splitter->indexOf(child->splitter);
                    for (int g = 0; g < child->children.size(); ++g) {
                        PaneNode* grandChild = child->children[g];
                        if (!grandChild) continue;
                        grandChild->parent = node;
                        newChildren.append(grandChild);

                        QWidget* gWidget = grandChild->isSplitter ? static_cast<QWidget*>(grandChild->splitter.data()) : grandChild->container.data();
                        if (gWidget) {
                            if (childIdx >= 0) {
                                node->splitter->insertWidget(childIdx + g, gWidget);
                            } else {
                                node->splitter->addWidget(gWidget);
                            }
                        }
                    }
                    child->children.clear();
                    child->splitter->deleteLater();
                    delete child;
                }
            } else if (child) {
                newChildren.append(child);
            }
        }
        node->children = newChildren;

        // 规范化 2：如果 Splitter 只剩 1 个子节点，替换为该子节点，并将组件正确重挂至上级
        if (node->children.size() == 1) {
            PaneNode* soleChild = node->children.first();
            if (soleChild) {
                soleChild->parent = node->parent;

                QWidget* soleWidget = soleChild->isSplitter ? static_cast<QWidget*>(soleChild->splitter.data()) : soleChild->container.data();

                if (node->parent) {
                    int idx = node->parent->children.indexOf(node);
                    if (idx >= 0) {
                        node->parent->children[idx] = soleChild;
                        if (node->parent->splitter && soleWidget) {
                            node->parent->splitter->replaceWidget(idx, soleWidget);
                        }
                    }
                } else {
                    m_rootNode = soleChild;
                    if (m_panel && m_panel->mainLayout() && soleWidget) {
                        if (node->splitter) {
                            m_panel->mainLayout()->removeWidget(node->splitter);
                        }
                        m_panel->mainLayout()->addWidget(soleWidget, 1);
                        soleWidget->show();
                    }
                }

                node->children.clear();
                if (node->splitter) {
                    node->splitter->deleteLater();
                }
                delete node;
            }
        }
    }
}

PaneTreeNode ContentPaneSplitManager::exportNodeRecursive(PaneNode* node) const {
    PaneTreeNode tn;
    if (!node) return tn;

    tn.isSplitter = node->isSplitter;
    tn.orientation = node->orientation;
    tn.isPrimary = node->isPrimary;
    if (!node->isSplitter && node->panel) {
        tn.path = node->panel->currentPath();
    }

    if (node->isSplitter) {
        for (PaneNode* child : node->children) {
            tn.children.append(exportNodeRecursive(child));
        }
    }

    return tn;
}

ContentPaneSplitManager::SplitEvaluationResult ContentPaneSplitManager::evaluateDropAtPosition(const QPoint& globalPos, ContentPanel* sourcePane) {
    SplitEvaluationResult res;
    if (rootPane() != m_panel) {
        return rootPane()->splitManager()->evaluateDropAtPosition(globalPos, sourcePane);
    }

    QList<ContentPanel*> allPanes;
    if (m_rootNode) {
        collectPanesDepthFirst(m_rootNode, allPanes);
    } else {
        allPanes.append(m_panel);
    }

    for (ContentPanel* target : allPanes) {
        if (!target || !target->isVisible()) continue;
        if (sourcePane && target == sourcePane) continue; // 不能落在自己身上

        QPoint localPos = target->mapFromGlobal(globalPos);
        QRect targetRect = target->rect();
        if (!targetRect.contains(localPos)) continue;

        int w = targetRect.width();
        int h = targetRect.height();
        if (w <= 0 || h <= 0) continue;

        double relX = static_cast<double>(localPos.x()) / w;
        double relY = static_cast<double>(localPos.y()) / h;

        bool isValid = false;
        Qt::Orientation ori = Qt::Horizontal;
        bool insertBefore = false;
        QRect highlightLocal;

        if (relX < 0.25) {
            isValid = true;
            ori = Qt::Horizontal;
            insertBefore = true;
            highlightLocal = QRect(0, 0, w / 2, h);
        } else if (relX > 0.75) {
            isValid = true;
            ori = Qt::Horizontal;
            insertBefore = false;
            highlightLocal = QRect(w / 2, 0, w / 2, h);
        } else if (relY < 0.25) {
            isValid = true;
            ori = Qt::Vertical;
            insertBefore = true;
            highlightLocal = QRect(0, 0, w, h / 2);
        } else if (relY > 0.75) {
            isValid = true;
            ori = Qt::Vertical;
            insertBefore = false;
            highlightLocal = QRect(0, h / 2, w, h / 2);
        }

        if (isValid) {
            // 最小尺寸校验：验证插入后目标区域空间是否足够
            int reqWidth = (ori == Qt::Horizontal) ? ContentPanel::kMinPaneWidth * 2 : ContentPanel::kMinPaneWidth;
            int reqHeight = (ori == Qt::Vertical) ? ContentPanel::kMinPaneHeight * 2 : ContentPanel::kMinPaneHeight;
            if (w < reqWidth || h < reqHeight) {
                isValid = false;
            }
        }

        if (isValid) {
            res.isValid = true;
            res.orientation = ori;
            res.insertBefore = insertBefore;
            res.targetPanel = target;

            QPoint globalTopLeft = target->mapToGlobal(highlightLocal.topLeft());
            QPoint hostTopLeft = m_panel->mapFromGlobal(globalTopLeft);
            res.highlightRect = QRect(hostTopLeft, highlightLocal.size());
            return res;
        }
    }

    return res;
}

ContentPaneSplitManager::SplitEvaluationResult ContentPaneSplitManager::evaluateSplitDrop(const QPoint& pos, const QSize& refSize) {
    SplitEvaluationResult res;
    int w = refSize.width();
    int h = refSize.height();
    if (w <= 0 || h <= 0) return res;

    if (pos.x() < w * 0.25) {
        res.isValid = true;
        res.orientation = Qt::Horizontal;
        res.insertBefore = true;
        res.highlightRect = QRect(0, 0, w / 2, h);
    } else if (pos.x() > w * 0.75) {
        res.isValid = true;
        res.orientation = Qt::Horizontal;
        res.insertBefore = false;
        res.highlightRect = QRect(w / 2, 0, w / 2, h);
    } else if (pos.y() < h * 0.25) {
        res.isValid = true;
        res.orientation = Qt::Vertical;
        res.insertBefore = true;
        res.highlightRect = QRect(0, 0, w, h / 2);
    } else if (pos.y() > h * 0.75) {
        res.isValid = true;
        res.orientation = Qt::Vertical;
        res.insertBefore = false;
        res.highlightRect = QRect(0, h / 2, w, h / 2);
    }
    return res;
}

void ContentPaneSplitManager::movePane(ContentPanel* sourcePane, ContentPanel* targetPane, Qt::Orientation orientation, bool insertBefore) {
    if (!sourcePane || !targetPane || sourcePane == targetPane) return;

    if (rootPane() != m_panel) {
        rootPane()->splitManager()->movePane(sourcePane, targetPane, orientation, insertBefore);
        return;
    }

    PaneNode* sourceLeaf = findNodeForPanel(m_rootNode, sourcePane);
    PaneNode* targetLeaf = findNodeForPanel(m_rootNode, targetPane);
    if (!sourceLeaf || !targetLeaf) return;

    // 从树中脱离 sourceLeaf
    PaneNode* srcParent = sourceLeaf->parent;
    if (srcParent) {
        srcParent->children.removeOne(sourceLeaf);
        sourceLeaf->parent = nullptr;
    }

    // 插入到 targetLeaf 所在位置
    PaneNode* tgtParent = targetLeaf->parent;
    if (tgtParent && tgtParent->isSplitter && tgtParent->orientation == orientation) {
        int idx = tgtParent->children.indexOf(targetLeaf);
        int insertPos = insertBefore ? idx : idx + 1;
        sourceLeaf->parent = tgtParent;
        tgtParent->children.insert(insertPos, sourceLeaf);
        if (tgtParent->splitter && sourceLeaf->container) {
            tgtParent->splitter->insertWidget(insertPos, sourceLeaf->container);
        }
    } else {
        QSplitter* newSplitter = new QSplitter(orientation, m_panel);
        newSplitter->setHandleWidth(5);
        newSplitter->setChildrenCollapsible(false);

        PaneNode* newSplitterNode = new PaneNode();
        newSplitterNode->isSplitter = true;
        newSplitterNode->orientation = orientation;
        newSplitterNode->splitter = newSplitter;
        newSplitterNode->parent = tgtParent;

        if (tgtParent) {
            int idx = tgtParent->children.indexOf(targetLeaf);
            tgtParent->children[idx] = newSplitterNode;
            if (tgtParent->splitter) {
                tgtParent->splitter->replaceWidget(idx, newSplitter);
            }
        } else {
            m_rootNode = newSplitterNode;
            m_panel->mainLayout()->addWidget(newSplitter, 1);
        }

        targetLeaf->parent = newSplitterNode;
        sourceLeaf->parent = newSplitterNode;

        if (insertBefore) {
            newSplitterNode->children.append(sourceLeaf);
            newSplitterNode->children.append(targetLeaf);
            if (sourceLeaf->container) newSplitter->addWidget(sourceLeaf->container);
            if (targetLeaf->container) newSplitter->addWidget(targetLeaf->container);
            else if (targetLeaf->splitter) newSplitter->addWidget(targetLeaf->splitter);
        } else {
            newSplitterNode->children.append(targetLeaf);
            newSplitterNode->children.append(sourceLeaf);
            if (targetLeaf->container) newSplitter->addWidget(targetLeaf->container);
            else if (targetLeaf->splitter) newSplitter->addWidget(targetLeaf->splitter);
            if (sourceLeaf->container) newSplitter->addWidget(sourceLeaf->container);
        }
    }

    normalizeTree(m_rootNode);
    redistributePaneSizes();
    updateContainerMinimumWidth();
    refreshActiveIndicators();
    notifyLayoutChanged();
}

void ContentPaneSplitManager::splitPaneAtTarget(ContentPanel* targetPane, Qt::Orientation orientation, const QString& secondaryPath, bool insertBefore) {
    if (!targetPane) {
        splitPane(orientation, secondaryPath, insertBefore);
        return;
    }
    m_activePaneForSplit = targetPane;
    splitPane(orientation, secondaryPath, insertBefore);
}

ContentPaneSplitManager::PaneNode* ContentPaneSplitManager::importNodeRecursive(const PaneTreeNode& treeNode, QWidget* parentWidget) {
    if (!m_panel) return nullptr;

    PaneNode* node = new PaneNode();
    node->isSplitter = treeNode.isSplitter;
    node->orientation = treeNode.orientation;
    node->isPrimary = treeNode.isPrimary;

    if (node->isSplitter) {
        QSplitter* splitter = new QSplitter(node->orientation, parentWidget);
        splitter->setHandleWidth(5);
        splitter->setChildrenCollapsible(false);
        node->splitter = splitter;

        for (const auto& childTN : treeNode.children) {
            PaneNode* childNode = importNodeRecursive(childTN, splitter);
            if (childNode) {
                childNode->parent = node;
                node->children.append(childNode);
                if (childNode->container) {
                    splitter->addWidget(childNode->container);
                } else if (childNode->splitter) {
                    splitter->addWidget(childNode->splitter);
                }
            }
        }
    } else {
        if (node->isPrimary) {
            node->panel = m_panel;
            if (!m_primaryPaneContainer) {
                m_primaryPaneContainer = new QFrame(parentWidget);
                m_primaryPaneContainer->setObjectName("EditorContainer");
                m_primaryPaneContainer->setAttribute(Qt::WA_StyledBackground, true);
                m_primaryPaneContainer->setMinimumWidth(ContentPanel::kMinPaneWidth);
                QVBoxLayout* primLayout = new QVBoxLayout(m_primaryPaneContainer);
                primLayout->setContentsMargins(0, 0, 0, 0);
                primLayout->setSpacing(0);

                if (m_panel->headerWidget()) {
                    m_panel->mainLayout()->removeWidget(m_panel->headerWidget());
                    primLayout->addWidget(m_panel->headerWidget());
                }
                if (m_panel->viewStack()) {
                    m_panel->mainLayout()->removeWidget(m_panel->viewStack());
                    primLayout->addWidget(m_panel->viewStack(), 1);
                }
            } else {
                m_primaryPaneContainer->setParent(parentWidget);
            }
            m_primaryPaneContainer->show();
            node->container = m_primaryPaneContainer;
            if (!treeNode.path.isEmpty()) {
                m_panel->loadDirectory(treeNode.path);
            }
        } else {
            QWidget* container = new QWidget(parentWidget);
            QVBoxLayout* layout = new QVBoxLayout(container);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);

            ContentPanel* newPane = new ContentPanel(container);
            container->setMinimumWidth(ContentPanel::kMinPaneWidth);
            newPane->setIsSecondaryPane(true);
            if (newPane->splitManager()) {
                newPane->splitManager()->m_rootPane = m_panel;
            }
            newPane->setViewMode(m_panel->currentViewMode());

            connect(newPane, &ContentPanel::closePaneRequested, m_panel, [this, newPane]() {
                closePane(newPane);
            });
            connect(newPane, &ContentPanel::directorySelected, m_panel, [this, newPane](const QString& path) {
                newPane->setActivePane(true);
                newPane->loadDirectory(path);
                emit m_panel->dualPanePathsChanged(m_panel->currentPath(), path);
                emit newPane->panelActivated(newPane);
                notifyLayoutChanged();
            });

            layout->addWidget(newPane);
            node->container = container;
            node->panel = newPane;

            if (!treeNode.path.isEmpty()) {
                newPane->loadDirectory(treeNode.path);
            }
            emit m_panel->secondaryPaneCreated(newPane);
        }
    }

    return node;
}

void ContentPaneSplitManager::splitPane(Qt::Orientation orientation, const QString& secondaryPath, bool insertBefore) {
    if (!m_panel) return;

    if (rootPane() != m_panel) {
        rootPane()->splitManager()->splitPane(orientation, secondaryPath, insertBefore);
        return;
    }

    if (paneCount() >= ContentPanel::kMaxPanes) {
        ContentPanel* activeTarget = m_activePaneForSplit ? m_activePaneForSplit : m_panel;
        if (!secondaryPath.isEmpty() && activeTarget) {
            activeTarget->loadDirectory(secondaryPath);
        }
        return;
    }

    m_isSplit = true;
    m_panel->setProperty("isHostPanel", "true");
    m_panel->style()->unpolish(m_panel);
    m_panel->style()->polish(m_panel);

    // 如果还没有根节点，初始化主窗格 Leaf 节点
    if (!m_rootNode) {
        m_primaryPaneContainer = new QFrame(m_panel);
        m_primaryPaneContainer->setObjectName("EditorContainer");
        m_primaryPaneContainer->setAttribute(Qt::WA_StyledBackground, true);
        m_primaryPaneContainer->setMinimumWidth(ContentPanel::kMinPaneWidth);
        QVBoxLayout* primLayout = new QVBoxLayout(m_primaryPaneContainer);
        primLayout->setContentsMargins(0, 0, 0, 0);
        primLayout->setSpacing(0);

        if (m_panel->headerWidget()) {
            m_panel->mainLayout()->removeWidget(m_panel->headerWidget());
            primLayout->addWidget(m_panel->headerWidget());
        }
        if (m_panel->viewStack()) {
            m_panel->mainLayout()->removeWidget(m_panel->viewStack());
            primLayout->addWidget(m_panel->viewStack(), 1);
        }

        m_rootNode = new PaneNode();
        m_rootNode->isSplitter = false;
        m_rootNode->isPrimary = true;
        m_rootNode->panel = m_panel;
        m_rootNode->container = m_primaryPaneContainer;
    }

    // 找到放置目标节点的 Leaf 节点（默认为当前活跃窗格）
    ContentPanel* targetPanel = m_activePaneForSplit ? m_activePaneForSplit : m_panel;
    PaneNode* targetLeaf = findNodeForPanel(m_rootNode, targetPanel);
    if (!targetLeaf) targetLeaf = m_rootNode;

    // 创建新的副窗格及其 Leaf 节点
    QWidget* newContainer = new QWidget(m_panel);
    QVBoxLayout* newLayout = new QVBoxLayout(newContainer);
    newLayout->setContentsMargins(0, 0, 0, 0);
    newLayout->setSpacing(0);

    ContentPanel* newPane = new ContentPanel(newContainer);
    newContainer->setMinimumWidth(ContentPanel::kMinPaneWidth);
    newPane->setIsSecondaryPane(true);
    if (newPane->splitManager()) {
        newPane->splitManager()->m_rootPane = m_panel;
    }
    newPane->setViewMode(m_panel->currentViewMode());

    connect(newPane, &ContentPanel::closePaneRequested, m_panel, [this, newPane]() {
        closePane(newPane);
    });
    connect(newPane, &ContentPanel::directorySelected, m_panel, [this, newPane](const QString& path) {
        newPane->setActivePane(true);
        newPane->loadDirectory(path);
        emit m_panel->dualPanePathsChanged(m_panel->currentPath(), path);
        emit newPane->panelActivated(newPane);
        notifyLayoutChanged();
    });

    newLayout->addWidget(newPane);
    newPane->loadDirectory(!secondaryPath.isEmpty() ? secondaryPath : targetPanel->currentPath());

    PaneNode* newLeaf = new PaneNode();
    newLeaf->isSplitter = false;
    newLeaf->isPrimary = false;
    newLeaf->panel = newPane;
    newLeaf->container = newContainer;

    // 根据方向与目标 Leaf 的父 Splitter 进行融合插入
    PaneNode* parentNode = targetLeaf->parent;
    if (parentNode && parentNode->isSplitter && parentNode->orientation == orientation) {
        int idx = parentNode->children.indexOf(targetLeaf);
        int insertPos = insertBefore ? idx : idx + 1;
        newLeaf->parent = parentNode;
        parentNode->children.insert(insertPos, newLeaf);
        if (parentNode->splitter) {
            parentNode->splitter->insertWidget(insertPos, newContainer);
        }
    } else {
        // 创建新的子 QSplitter
        QSplitter* newSplitter = new QSplitter(orientation, m_panel);
        newSplitter->setHandleWidth(5);
        newSplitter->setChildrenCollapsible(false);

        PaneNode* newSplitterNode = new PaneNode();
        newSplitterNode->isSplitter = true;
        newSplitterNode->orientation = orientation;
        newSplitterNode->splitter = newSplitter;
        newSplitterNode->parent = parentNode;

        if (parentNode) {
            int idx = parentNode->children.indexOf(targetLeaf);
            parentNode->children[idx] = newSplitterNode;
            if (parentNode->splitter) {
                parentNode->splitter->replaceWidget(idx, newSplitter);
            }
        } else {
            m_rootNode = newSplitterNode;
            m_panel->mainLayout()->addWidget(newSplitter, 1);
        }

        targetLeaf->parent = newSplitterNode;
        newLeaf->parent = newSplitterNode;

        if (insertBefore) {
            newSplitterNode->children.append(newLeaf);
            newSplitterNode->children.append(targetLeaf);
            newSplitter->addWidget(newContainer);
            if (targetLeaf->container) {
                newSplitter->addWidget(targetLeaf->container);
            } else if (targetLeaf->splitter) {
                newSplitter->addWidget(targetLeaf->splitter);
            }
        } else {
            newSplitterNode->children.append(targetLeaf);
            newSplitterNode->children.append(newLeaf);
            if (targetLeaf->container) {
                newSplitter->addWidget(targetLeaf->container);
            } else if (targetLeaf->splitter) {
                newSplitter->addWidget(targetLeaf->splitter);
            }
            newSplitter->addWidget(newContainer);
        }
    }

    normalizeTree(m_rootNode);
    redistributePaneSizes();
    emit m_panel->secondaryPaneCreated(newPane);
    updateContainerMinimumWidth();
    refreshActiveIndicators();
    notifyLayoutChanged();
}

void ContentPaneSplitManager::notifyLayoutChanged() {
    if (m_blockLayoutSignals) return;

    if (!m_layoutDebounceTimer) {
        m_layoutDebounceTimer = new QTimer(this);
        m_layoutDebounceTimer->setSingleShot(true);
        m_layoutDebounceTimer->setInterval(0);
        connect(m_layoutDebounceTimer, &QTimer::timeout, this, [this]() {
            if (!m_blockLayoutSignals) {
                emit layoutChanged();
            }
        });
    }

    if (!m_layoutDebounceTimer->isActive()) {
        m_layoutDebounceTimer->start();
    }
}

void ContentPaneSplitManager::updateContainerMinimumWidth() {
    bool isVert = (m_splitOrientation == Qt::Vertical);
    int minW = isVert ? 0 : ContentPanel::kMinPaneWidth;
    int minH = isVert ? ContentPanel::kMinPaneHeight : 0;

    if (m_primaryPaneContainer) {
        m_primaryPaneContainer->setMinimumWidth(minW);
        m_primaryPaneContainer->setMinimumHeight(minH);
    }
}

void ContentPaneSplitManager::setSplitOrientation(Qt::Orientation target) {
    if (rootPane() != m_panel) {
        rootPane()->splitManager()->setSplitOrientation(target);
        return;
    }

    if (m_splitOrientation == target) return;

    m_splitOrientation = target;
    if (m_rootNode && m_rootNode->isSplitter) {
        m_rootNode->orientation = target;
        if (m_rootNode->splitter) {
            m_rootNode->splitter->setOrientation(target);
        }
    }

    updateContainerMinimumWidth();
    redistributePaneSizes();
    notifyLayoutChanged();
}

void ContentPaneSplitManager::closePane(ContentPanel* pane) {
    if (rootPane() != m_panel) {
        rootPane()->splitManager()->closePane(pane);
        return;
    }

    if (m_activePaneForSplit == pane) {
        m_activePaneForSplit = nullptr;
    }

    // 如果只有一个窗格或试图关闭主窗格，且存在副窗格，则先由首个副窗格接管主窗格内容
    if (pane == m_panel) {
        QList<ContentPanel*> currentPanes = panes();
        if (!currentPanes.isEmpty()) {
            ContentPanel* firstSecondary = currentPanes.first();
            QString secondaryPath = firstSecondary->currentPath();
            m_panel->setViewMode(firstSecondary->currentViewMode());
            m_panel->loadDirectory(secondaryPath, firstSecondary->isRecursive());
            closePane(firstSecondary);
            return;
        }
    }

    PaneNode* targetLeaf = findNodeForPanel(m_rootNode, pane);
    if (!targetLeaf) return;

    PaneNode* parentNode = targetLeaf->parent;

    if (targetLeaf->container) {
        targetLeaf->container->deleteLater();
    }

    if (parentNode) {
        parentNode->children.removeOne(targetLeaf);
        delete targetLeaf;
        normalizeTree(m_rootNode);
    } else {
        deleteNodeRecursive(m_rootNode);
        m_rootNode = nullptr;
    }

    if (paneCount() <= 1) {
        m_isSplit = false;
        m_panel->setProperty("isHostPanel", "false");
        m_panel->style()->unpolish(m_panel);
        m_panel->style()->polish(m_panel);

        deleteNodeRecursive(m_rootNode);
        m_rootNode = nullptr;

        if (m_primaryPaneContainer) {
            if (m_panel->headerWidget() && m_primaryPaneContainer->layout()) {
                m_primaryPaneContainer->layout()->removeWidget(m_panel->headerWidget());
                m_panel->headerWidget()->setParent(m_panel);
            }
            if (m_panel->viewStack() && m_primaryPaneContainer->layout()) {
                m_primaryPaneContainer->layout()->removeWidget(m_panel->viewStack());
                m_panel->viewStack()->setParent(m_panel);
            }
            m_primaryPaneContainer->deleteLater();
            m_primaryPaneContainer = nullptr;
        }

        if (m_panel->headerWidget()) {
            m_panel->mainLayout()->addWidget(m_panel->headerWidget());
            m_panel->headerWidget()->show();
        }
        if (m_panel->viewStack()) {
            m_panel->mainLayout()->addWidget(m_panel->viewStack(), 1);
            m_panel->viewStack()->show();
        }
        emit m_panel->secondaryPaneClosed();
        emit m_panel->directorySelected(m_panel->currentPath());
    } else {
        redistributePaneSizes();
    }

    updateContainerMinimumWidth();
    refreshActiveIndicators();
    notifyLayoutChanged();
}

void ContentPaneSplitManager::closeSecondaryPane() {
    if (rootPane() != m_panel) {
        rootPane()->splitManager()->closeSecondaryPane();
        return;
    }

    QList<ContentPanel*> allPanes = panes();
    if (allPanes.isEmpty()) return;

    ContentPanel* target = m_activePaneForSplit ? m_activePaneForSplit : allPanes.last();
    if (target == m_panel) {
        target = allPanes.last();
    }
    closePane(target);
}

void ContentPaneSplitManager::redistributePaneSizes() {
    if (!m_rootNode) return;
    QList<PaneNode*> allNodes;
    collectNodesDepthFirst(m_rootNode, allNodes);
    for (PaneNode* node : allNodes) {
        if (node && node->isSplitter && node->splitter) {
            int count = node->children.size();
            if (count > 0) {
                int handleW = node->splitter->handleWidth();
                int total = (node->orientation == Qt::Horizontal)
                    ? (node->splitter->width() - handleW * (count - 1))
                    : (node->splitter->height() - handleW * (count - 1));
                if (total <= 0) total = (node->orientation == Qt::Horizontal) ? m_panel->width() : m_panel->height();
                int each = qMax(1, total / count);
                QList<int> sizes;
                for (int i = 0; i < count; ++i) {
                    sizes << each;
                }
                node->splitter->setSizes(sizes);
            }
        }
    }
}

TabSplitState ContentPaneSplitManager::exportSplitState() const {
    TabSplitState state;
    if (rootPane() != m_panel) {
        return rootPane()->splitManager()->exportSplitState();
    }

    QList<ContentPanel*> allPanes;
    if (m_rootNode) {
        collectPanesDepthFirst(m_rootNode, allPanes);
        state.rootNode = exportNodeRecursive(m_rootNode);
    } else {
        allPanes.append(m_panel);
        PaneTreeNode leaf;
        leaf.isSplitter = false;
        leaf.path = m_panel->currentPath();
        leaf.isPrimary = true;
        state.rootNode = leaf;
    }

    state.orientation = splitOrientation();

    for (int i = 0; i < allPanes.size(); ++i) {
        ContentPanel* p = allPanes[i];
        QString path = p ? p->currentPath().trimmed() : "";
        if (!path.isEmpty()) {
            state.panePaths.append(path);
        }
        if (p == m_panel) {
            state.primaryIndex = state.panePaths.isEmpty() ? 0 : state.panePaths.size() - 1;
        }
        if (m_activePaneForSplit == p || (m_activePaneForSplit == nullptr && p == m_panel)) {
            state.activePaneIndex = state.panePaths.isEmpty() ? 0 : state.panePaths.size() - 1;
        }
    }
    state.isSplit = (state.panePaths.size() > 1);

    return state;
}

void ContentPaneSplitManager::restoreSplitState(const TabSplitState& state) {
    if (rootPane() != m_panel) {
        rootPane()->splitManager()->restoreSplitState(state);
        return;
    }

    m_blockLayoutSignals = true;

    // 1. 清理现有所有副窗格与窗格树
    QList<ContentPanel*> existingPanes = panes();
    for (ContentPanel* p : existingPanes) {
        closePane(p);
    }

    if (m_rootNode) {
        deleteNodeRecursive(m_rootNode);
        m_rootNode = nullptr;
    }

    // 2. 如果序列化数据包含分屏树结构（isSplit == true），则按树层次还原；否则为单窗格
    if (state.isSplit && (state.rootNode.isSplitter || !state.rootNode.children.isEmpty())) {
        m_rootNode = importNodeRecursive(state.rootNode, m_panel);
        if (m_rootNode) {
            if (m_rootNode->splitter) {
                m_panel->mainLayout()->addWidget(m_rootNode->splitter, 1);
                m_rootNode->splitter->show();
            } else if (m_rootNode->container) {
                m_panel->mainLayout()->addWidget(m_rootNode->container, 1);
                m_rootNode->container->show();
            }
        }
    } else {
        // 兼容与普通单窗格/新建标签页/旧平铺配置恢复
        if (m_primaryPaneContainer) {
            if (m_panel->headerWidget() && m_primaryPaneContainer->layout()) {
                m_primaryPaneContainer->layout()->removeWidget(m_panel->headerWidget());
            }
            if (m_panel->viewStack() && m_primaryPaneContainer->layout()) {
                m_primaryPaneContainer->layout()->removeWidget(m_panel->viewStack());
            }
            m_primaryPaneContainer->deleteLater();
            m_primaryPaneContainer = nullptr;
        }
        if (m_panel->headerWidget()) {
            m_panel->mainLayout()->addWidget(m_panel->headerWidget());
            m_panel->headerWidget()->show();
        }
        if (m_panel->viewStack()) {
            m_panel->mainLayout()->addWidget(m_panel->viewStack(), 1);
            m_panel->viewStack()->show();
        }

        if (!state.panePaths.isEmpty()) {
            int primIdx = qBound(0, state.primaryIndex, state.panePaths.size() - 1);
            m_panel->loadDirectory(state.panePaths[primIdx]);

            if (state.isSplit && state.panePaths.size() > 1) {
                for (int i = primIdx - 1; i >= 0; --i) {
                    splitPane(state.orientation, state.panePaths[i], true);
                }
                for (int i = primIdx + 1; i < state.panePaths.size(); ++i) {
                    splitPane(state.orientation, state.panePaths[i], false);
                }
            }
        }
    }

    // 3. 恢复激活窗格
    QList<ContentPanel*> allPanes;
    if (m_rootNode) {
        collectPanesDepthFirst(m_rootNode, allPanes);
    } else {
        allPanes.append(m_panel);
    }

    int activeIdx = qBound(0, state.activePaneIndex, allPanes.size() - 1);
    if (activeIdx >= 0 && activeIdx < allPanes.size() && allPanes[activeIdx]) {
        allPanes[activeIdx]->setActivePane(true);
    } else {
        m_panel->setActivePane(true);
    }

    m_isSplit = (paneCount() > 1);
    updateContainerMinimumWidth();
    refreshActiveIndicators();

    m_blockLayoutSignals = false;
    notifyLayoutChanged();
}

void ContentPaneSplitManager::setActivePane(bool active) {
    ContentPanel* root = rootPane();
    if (root && root->splitManager()) {
        if (active) {
            root->splitManager()->m_activePaneForSplit = m_panel;
        }
        root->splitManager()->refreshActiveIndicators();
    }
}

void ContentPaneSplitManager::refreshActiveIndicators() {
    if (rootPane() != m_panel) {
        rootPane()->splitManager()->refreshActiveIndicators();
        return;
    }

    auto setPaneActiveProperty = [](QWidget* widget, bool shown) {
        if (!widget) return;
        QString valStr = shown ? "true" : "false";
        if (widget->property("activePane").toString() != valStr) {
            widget->setProperty("activePane", valStr);
            if (widget->style()) {
                widget->style()->unpolish(widget);
                widget->style()->polish(widget);
            }
        }
    };

    bool isSplit = isSplitMode();

    // 根窗格激活判定
    bool rootActive = (m_activePaneForSplit == nullptr || m_activePaneForSplit == m_panel);
    bool rootShown = isSplit && rootActive;

    if (m_primaryPaneContainer) {
        setPaneActiveProperty(m_primaryPaneContainer, rootShown);
    }
    setPaneActiveProperty(m_panel, false);
    if (m_panel) {
        m_panel->updateHeaderTitle();
        if (m_panel->headerWidget()) {
            m_panel->headerWidget()->setActive(rootShown);
        }
    }

    // 副窗格激活判定
    QList<ContentPanel*> secPanes = panes();
    for (ContentPanel* pane : secPanes) {
        if (!pane) continue;
        bool paneActive = (m_activePaneForSplit == pane);
        bool paneShown = isSplit && paneActive;

        pane->updateHeaderTitle();
        setPaneActiveProperty(pane, paneShown);
        if (pane->headerWidget()) {
            pane->headerWidget()->setActive(paneShown);
        }
    }
}

void ContentPaneSplitManager::updateDragOverlayGlobal(const QPoint& globalPos, ContentPanel* sourcePane) {
    if (rootPane() != m_panel) {
        rootPane()->splitManager()->updateDragOverlayGlobal(globalPos, sourcePane);
        return;
    }

    SplitEvaluationResult eval = evaluateDropAtPosition(globalPos, sourcePane);
    if (!eval.isValid) {
        hideDragOverlay();
        return;
    }

    if (!m_dragOverlayWidget) {
        m_dragOverlayWidget = new QWidget(m_panel);
        m_dragOverlayWidget->setObjectName("DragOverlayWidget");
        m_dragOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    m_dragOverlayWidget->setGeometry(eval.highlightRect);
    m_dragOverlayWidget->show();
    m_dragOverlayWidget->raise();
}

void ContentPaneSplitManager::hideDragOverlay() {
    if (m_dragOverlayWidget) {
        m_dragOverlayWidget->hide();
    }
}

} // namespace QuarkMeta
