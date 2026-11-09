#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QIcon>
#include "FavoritePanel.h"

namespace QuarkMeta {

class HistoryItemDelegate;

class HistoryPanel : public QWidget {
    Q_OBJECT

public:
    explicit HistoryPanel(QWidget* parent = nullptr);
    ~HistoryPanel() override = default;

    void loadHistory();
    void filterTreeNodes(const QString& keyword);
    void activateCurrentItem();

signals:
    void requestLocateFile(const QString& path);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onContextMenuRequested(const QPoint& pos);
    void onHistoryItemAdded(const QString& path);
    void onHistoryItemRemoved(const QString& path);
    void onHistoryCleared();

private:
    void initUi();
    QTreeWidgetItem* createHistoryItem(const QString& path);
    void requestThumbnailExtraction(const QString& path);
    void updateItemThumbnail(const QString& path, const QPixmap& pix);

    QVBoxLayout* m_mainLayout = nullptr;
    QTreeWidget* m_treeWidget = nullptr;
    HistoryItemDelegate* m_delegate = nullptr;
    bool m_isLoaded = false;
    QString m_lastHoveredPath;
};

} // namespace QuarkMeta
