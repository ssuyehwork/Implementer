#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QIcon>
#include "FavoritePanel.h"

namespace QuarkMeta {

class HistoryPanel : public QWidget {
    Q_OBJECT

public:
    explicit HistoryPanel(QWidget* parent = nullptr);
    ~HistoryPanel() override = default;

    void loadHistory();
    void filterTreeNodes(const QString& keyword);

signals:
    void requestLocateFile(const QString& path);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onContextMenuRequested(const QPoint& pos);
    void onHistoryItemAdded(const QString& path);
    void onHistoryItemRemoved(const QString& path);
    void onHistoryCleared();

private:
    void initUi();
    QTreeWidgetItem* createHistoryItem(const QString& path);
    void updateItemThumbnail(QTreeWidgetItem* item, const QString& path);

    QVBoxLayout* m_mainLayout = nullptr;
    QTreeWidget* m_treeWidget = nullptr;
    bool m_isLoaded = false;
    QString m_lastHoveredPath;
};

} // namespace QuarkMeta
