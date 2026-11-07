#pragma once

#include <QObject>
#include <QFrame>
#include <QList>
#include <QPoint>
#include <QSplitter>
#include <QTimer>
#include <QPointer>
#include <QWidget>

#include "../TabBarWidget.h"

namespace QuarkMeta {

class ContentPanel;

class ContentPaneSplitManager : public QObject {
    Q_OBJECT
public:
    explicit ContentPaneSplitManager(ContentPanel* panel);
    ~ContentPaneSplitManager() override = default;

    bool isSplitMode() const;
    Qt::Orientation splitOrientation() const;
    bool isSecondaryPane() const { return m_isSecondaryPane; }
    void setIsSecondaryPane(bool secondary) { m_isSecondaryPane = secondary; }

    ContentPanel* secondaryContentPanel() const;
    /**
     * @brief 获取管理的全部副窗格列表（扁平一维列表）
     * @note 严禁调用方对列表中的各副窗格再次递归调用 panes()。
     */
    QList<ContentPanel*> panes() const;
    int paneCount() const;
    ContentPanel* rootPane() const;

    struct SplitEvaluationResult {
        bool isValid = false;
        Qt::Orientation orientation = Qt::Horizontal;
        bool insertBefore = false;
        QRect highlightRect;
        ContentPanel* targetPanel = nullptr;
    };

    SplitEvaluationResult evaluateDropAtPosition(const QPoint& globalPos, ContentPanel* sourcePane);
    static SplitEvaluationResult evaluateSplitDrop(const QPoint& pos, const QSize& refSize);

    void movePane(ContentPanel* sourcePane, ContentPanel* targetPane, Qt::Orientation orientation, bool insertBefore);
    void splitPane(Qt::Orientation orientation, const QString& secondaryPath = QString(), bool insertBefore = false);
    void splitPaneAtTarget(ContentPanel* targetPane, Qt::Orientation orientation, const QString& secondaryPath = QString(), bool insertBefore = false);
    void closePane(ContentPanel* pane);
    void closeSecondaryPane();
    void redistributePaneSizes();
    void setActivePane(bool active);
    void refreshActiveIndicators();

    struct TabSplitState exportSplitState() const;
    void setSplitOrientation(Qt::Orientation target);
    void restoreSplitState(const struct TabSplitState& state);
    void updateDragOverlayGlobal(const QPoint& globalPos, ContentPanel* sourcePane = nullptr);
    void hideDragOverlay();
    void updateContainerMinimumWidth();
    void notifyLayoutChanged();

signals:
    void activePaneChanged(ContentPanel* panel);
    void layoutChanged();

private:
    QTimer* m_layoutDebounceTimer = nullptr;
    bool m_blockLayoutSignals = false;
    ContentPanel* m_panel = nullptr;
    QSplitter* m_paneSplitter = nullptr;
    QFrame* m_primaryPaneContainer = nullptr;
public:
    // 窗格树节点数据结构
    struct PaneNode {
        bool isSplitter = false;
        Qt::Orientation orientation = Qt::Horizontal;
        QPointer<QSplitter> splitter = nullptr;
        QPointer<QWidget> container = nullptr;
        ContentPanel* panel = nullptr;
        bool isPrimary = false;
        QList<PaneNode*> children;
        PaneNode* parent = nullptr;
    };

    PaneNode* rootNode() const { return m_rootNode; }

private:
    PaneNode* m_rootNode = nullptr;

    void collectPanesDepthFirst(PaneNode* node, QList<ContentPanel*>& list) const;
    void collectNodesDepthFirst(PaneNode* node, QList<PaneNode*>& list) const;
    PaneNode* findNodeForPanel(PaneNode* node, ContentPanel* panel) const;
    void normalizeTree(PaneNode* node);
    void deleteNodeRecursive(PaneNode* node);
    struct PaneTreeNode exportNodeRecursive(PaneNode* node) const;
    PaneNode* importNodeRecursive(const struct PaneTreeNode& treeNode, QWidget* parentWidget);
    ContentPanel* m_rootPane = nullptr;
    ContentPanel* m_activePaneForSplit = nullptr;
    QWidget* m_dragOverlayWidget = nullptr;
    QWidget* m_orientationPreviewWidget = nullptr;
    Qt::Orientation m_splitOrientation = Qt::Horizontal;
    bool m_isSplit = false;
    bool m_isSecondaryPane = false;
};

} // namespace QuarkMeta
