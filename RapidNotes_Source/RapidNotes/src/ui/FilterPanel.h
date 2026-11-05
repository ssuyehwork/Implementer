#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QPushButton>
#include <QLineEdit>
#include <QSlider>
#include <QMap>
#include <QStringList>
#include <QFrame>
#include "ScanStats.h"
#include "FilterStateModel.h"
#include "ScanStatsEngine.h"
#include "components/StyledCheckBox.h"
#include "components/ClickableRow.h"

#include "components/ColorBlock.h"
#include "components/InlineHueSlider.h"

namespace RapidNotes {

class SearchHistoryPanel;
class NoteStatusGroup;
class TagStatusGroup;
class ColorLabelGroup;
class RatingGroup;
class FileTypeGroup;
class CreateDateGroup;
class ModifyDateGroup;

class FilterPanel : public QFrame {
    Q_OBJECT

public:
    explicit FilterPanel(QWidget* parent = nullptr);
    ~FilterPanel() override = default;

    // 🚀【物理契约】：刚性锁定 230px 下限，杜绝被 QSplitter 挤压偷扣像素
    QSize minimumSizeHint() const override { return QSize(230, 100); }

    void populateStats(const RapidNotes::ScanStats& stats);
    void populate(const RapidNotes::ScanStats& stats) { populateStats(stats); }
    void clearStats();
    void populate(
        const QMap<int, int>&        ratingCounts,
        const QMap<QString, int>&    colorCounts,
        const QMap<QString, int>&    typeCounts,
        const QMap<QString, int>&    createDateCounts,
        const QMap<QString, int>&    modifyDateCounts,
        int                          emptyFolderCount
    );

    FilterState currentFilter() const { return m_filterModel ? m_filterModel->state() : m_filter; }

    void syncUIFromFilterState();
    void selectColor(const QColor& color);
    void setMirrorSource(bool isMirror);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
    void filterChanged(const FilterState& state);

public slots:
    void clearAllFilters(bool force = false);

private:
    void rebuildGroups();
    void updateHeaderStatus();
    void rebuildDateCheckboxes(bool isCreateDate, bool descending);

    QWidget*   buildGroup(const QString& title, QVBoxLayout*& outContentLayout,
                          QHBoxLayout** outHdrLayout = nullptr);
    QCheckBox* addFilterRow(QVBoxLayout* layout, const QString& label,
                            int count, const QColor& dotColor = Qt::transparent,
                            const QString& rowKey = QString());

    bool isRowKeyChecked(const QString& key, const FilterState& st) const;
    int countForRowKey(const QString& key) const;

    static QMap<QString, QColor> s_colorMap();

    FilterStateModel* m_filterModel = nullptr;
    ScanStatsEngine*  m_statsEngine = nullptr;

    FilterState m_filter;
    RapidNotes::ScanStats m_currentStats;

    QMap<int, int>      m_ratingCounts;
    QMap<QString, int>  m_colorCounts;
    QMap<QString, int>  m_typeCounts;
    QMap<QString, int>  m_createDateCounts;
    QMap<QString, int>  m_modifyDateCounts;
    int                 m_emptyFolderCount = 0;

    bool m_createDateDesc = true;
    bool m_modifyDateDesc = true;

    QVBoxLayout*  m_mainLayout      = nullptr;
    QScrollArea*  m_scrollArea      = nullptr;
    QWidget*      m_container       = nullptr;
    QVBoxLayout*  m_containerLayout = nullptr;
    QPushButton*  m_btnPin          = nullptr;
    QPushButton*  m_btnClearAll     = nullptr;
    QPushButton*  m_btnToggleGroups = nullptr;
    QLabel*       m_iconLabel       = nullptr;
    QLabel*       m_titleLabel      = nullptr;

    QList<QPushButton*> m_groupHeaders;

    QLineEdit*    m_editType        = nullptr;
    QLineEdit*    m_editCreateDate  = nullptr;
    QLineEdit*    m_editModifyDate  = nullptr;
    QVBoxLayout*  m_createDateLayout = nullptr;
    QVBoxLayout*  m_modifyDateLayout = nullptr;

    bool          m_isFilterPinned = false;

    SearchHistoryPanel* m_historyPanel = nullptr;

    
    void saveFilterHistory(const QString& key, const QString& text);
    QStringList getFilterHistory(const QString& key) const;

private slots:
    void onToggleAllGroupsClicked();
};

} // namespace RapidNotes