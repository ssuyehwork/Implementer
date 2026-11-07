#pragma once

#include <QWidget>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QPaintEvent>

namespace QuarkMeta {

class ColumnViewWidget;
class ContentPanel;

class ColumnBlankCanvasWidget : public QWidget {
    Q_OBJECT
public:
    explicit ColumnBlankCanvasWidget(ColumnViewWidget* columnView, ContentPanel* contentPanel, QWidget* parent = nullptr);
    ~ColumnBlankCanvasWidget() override = default;

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onContextMenuRequested(const QPoint& pos);

private:
    ColumnViewWidget* m_columnView = nullptr;
    ContentPanel* m_contentPanel = nullptr;
    bool m_isDragHover = false;
};

} // namespace QuarkMeta
