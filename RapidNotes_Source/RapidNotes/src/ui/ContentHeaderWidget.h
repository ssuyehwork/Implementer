#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "FilterPanel.h"

namespace QuarkMeta {

/**
 * @brief 内容面板顶部辅助控制工具栏组件
 * 封装标题图标、状态指示、递归显示按钮与隐藏/文件/文件夹过滤器切换按钮
 */
class ContentHeaderWidget : public QWidget {
    Q_OBJECT

public:
    explicit ContentHeaderWidget(QWidget* parent = nullptr);
    ~ContentHeaderWidget() override = default;

    void setTitle(const QString& title);
    void setHeaderIcon(const QString& iconKey);
    void setFilterState(const FilterState& state);
    void setRecursive(bool recursive);
    void setLayersEnabled(bool enabled, const QString& tooltip);
    void setActive(bool active);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void filterStateChanged(const FilterState& state);
    void recursiveToggled(bool recursive);
    void splitViewRequested();

private:
    void initUi();
    class ContentPanel* owningPanel() const;

    QHBoxLayout* m_layout = nullptr;
    QLabel* m_iconLabel = nullptr;
    QLabel* m_titleLabel = nullptr;

    QPushButton* m_btnSplitView = nullptr;
    QPushButton* m_btnToggleHidden = nullptr;
    QPushButton* m_btnLayers = nullptr;

    FilterState m_filterState;
    QPoint m_dragStartPos;
    bool m_isDraggingHeader = false;
};

} // namespace QuarkMeta
