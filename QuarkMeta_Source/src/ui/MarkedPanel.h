#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QLabel>

namespace QuarkMeta {

class MarkedPanel : public QWidget {
    Q_OBJECT

public:
    explicit MarkedPanel(QWidget* parent = nullptr);
    ~MarkedPanel() override = default;

    void loadMarkedFolders();
    void filterTreeNodes(const QString& keyword);

signals:
    void requestLocateFolder(const QString& path);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onContextMenuRequested(const QPoint& pos);
    void onMarkedFoldersChanged();

private:
    void initUi();
    QTreeWidgetItem* createMarkedItem(const QString& path, bool active);
    void handleDropPaths(const QMimeData* mimeData, const QPoint& pos, bool isViewportPos);
    void showDragOverlay(const QString& hoverTargetPath = QString());
    void hideDragOverlay();
    void updateDragOverlayText();

    QVBoxLayout* m_mainLayout = nullptr;
    QTreeWidget* m_treeWidget = nullptr;

    QWidget* m_dragOverlay = nullptr;
    QLabel* m_dragOverlayLabel = nullptr;
    QStringList m_activeTargetNames;
    QString m_hoverTargetPath;
};

} // namespace QuarkMeta
