#pragma once

#include <QAbstractItemView>
#include <QList>
#include <vector>

namespace QuarkMeta {

class SectionProxyModel;

class JustifiedView : public QAbstractItemView {
    Q_OBJECT
public:
    enum LayoutMode { JustifiedMode, GridMode };

    explicit JustifiedView(QWidget* parent = nullptr);

    void setTargetRowHeight(int h);
    void setAspectRatioRole(int role);
    void setLayoutMode(LayoutMode mode);
    LayoutMode layoutMode() const;

    /**
     * @brief 设置右侧安全保留区宽度（避让字母索引通道，彻底杜绝网格/自适应卡片与右缘索引发生重叠遮挡）
     */
    void setRightReservedWidth(int width);
    int rightReservedWidth() const { return m_rightReservedWidth; }

    int totalHeight() const { return m_totalHeight; }

    // 🚀【物理契约】：彻底切断 QAbstractItemView 对父容器的尺寸顶推
    QSize minimumSizeHint() const override { return QSize(50, 50); }
    QSize sizeHint() const override { return QSize(230, m_totalHeight); }

signals:
    void totalHeightChanged(int height);
    void layoutFinished();
    void pathsDropped(const QStringList& paths, const QModelIndex& targetIndex, Qt::DropAction action = Qt::CopyAction);

public:
    QList<int> rowsInRange(int top, int bottom) const;
    bool isLayoutReady() const;
    QRect visualRect(const QModelIndex& index) const override;
    void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible) override;
    QModelIndex indexAt(const QPoint& point) const override;

    void setSectionModel(SectionProxyModel* secModel) { m_sectionModel = secModel; }
    SectionProxyModel* sectionModel() const { return m_sectionModel; }

    void setShowEmptyHint(bool show) {
        if (m_showEmptyHint != show) {
            m_showEmptyHint = show;
            viewport()->update();
        }
    }
    bool showEmptyHint() const { return m_showEmptyHint; }

    void reset() override;
    void doItemsLayout() override;
    void setModel(QAbstractItemModel* model) override;

protected slots:
    void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles = QList<int>()) override;
    void rowsInserted(const QModelIndex& parent, int start, int end) override;
    void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) override;
    void onLayoutTimerTimeout();

protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    int horizontalOffset() const override;
    int verticalOffset() const override;
    bool isIndexHidden(const QModelIndex& index) const override;
    void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command) override;
    QRegion visualRegionForSelection(const QItemSelection& selection) const override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void updateGeometries() override;

private:
    void doLayout();
    void scheduleLayout();

    struct ItemGeometry {
        QRect rect;
        int index;
        bool isHeader = false;
        QString headerText;
        bool isCollapsed = false;
        bool isFolderHeader = false;
    };
    std::vector<ItemGeometry> m_geometries;
    int m_totalHeight = 0;
    int m_targetRowHeight = 128;
    int m_aspectRatioRole = Qt::UserRole + 2;
    int m_anchorRow = -1;
    int m_rightReservedWidth = 0; // 🚀 右侧安全保留区
    
    QPoint m_dragStartPos;
    bool m_isDraggingSelection = false;
    QRect m_selectionRect;

    LayoutMode m_layoutMode = JustifiedMode;
    QTimer* m_layoutTimer = nullptr;
    bool m_layoutDirty = false;
    bool m_showEmptyHint = true;
    SectionProxyModel* m_sectionModel = nullptr;
};

} // namespace QuarkMeta