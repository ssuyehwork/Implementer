#pragma once

#include <QWidget>
#include <QListView>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QList>
#include <QLabel>
#include "models/DiskItemModel.h"
#include "models/FilterProxyModel.h"
#include "DropListView.h"
#include "ColumnViewPane.h"

namespace QuarkMeta {

class ContentPanel;

class ColumnViewWidget : public QScrollArea {
    Q_OBJECT
public:
    explicit ColumnViewWidget(ContentPanel* contentPanel = nullptr, QWidget* parent = nullptr);
    ~ColumnViewWidget() override = default;

    void setRootPath(const QString& path);
    void clearAllColumns();

    ColumnViewPane* activePane() const;
    ColumnViewPane* rightmostPane() const;
    ColumnViewPane* findPaneForPath(const QString& targetPath) const;
    ColumnViewPane* findPaneForView(QAbstractItemView* view) const;
    void focusPane(int paneIndex);
    void activatePaneFromBlankClick(int paneIndex);
    void setActivePaneIndex(int newIndex);
    bool containsPath(const QString& path) const;
    void refreshActiveColumn();
    void refreshAllColumns();
    void updateMetadataForPath(const QString& path);
    void applySort(int sortType, Qt::SortOrder sortOrder);
    void scrollToRightmostPane();
    QStringList getSelectedPaths() const;
    QModelIndexList getSelectedIndexes() const;
    void applyFilterState(const FilterState& state);
    void goUpColumn();
    void goUpColumnFromIndex(int paneIndex);
    void clearAllSelections();
    void toggleFolderSectionCollapse();
    void expandFolder(const QString& folderPath, int paneIdx);

    QStringList columnPaths() const {
        QStringList paths;
        for (auto* pane : m_panes) {
            if (pane && !pane->currentPath().isEmpty()) {
                paths.append(pane->currentPath());
            }
        }
        return paths;
    }

signals:
    void pathNavigated(const QString& path);
    void selectionChanged();
    void activeColumnRecordsChanged(const std::vector<ItemRecord>& records);

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void dismissSubColumns(int fromIndex);
    ColumnViewPane* appendColumn(const QString& path);
    void clearOtherSelections(int activePaneIdx);
    void updatePaneWidths();
    void updateParentHighlights();

    ContentPanel* m_contentPanel = nullptr;
    FilterState m_currentFilter;
    int m_activePaneIndex = -1;
    bool m_autoScrollToRight = false;
    QWidget* m_container = nullptr;
    QHBoxLayout* m_layout = nullptr;
    QList<ColumnViewPane*> m_panes;
    QWidget* m_blankCanvasWidget = nullptr;
};

} // namespace QuarkMeta
