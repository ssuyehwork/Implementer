#include "FilterPanel.h"
#include "components/ColorDot.h"
#include "../meta/MarkedFolderService.h"
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include "ThumbnailStatusGroup.h"
#include "DuplicateStatusGroup.h"
#include "LinkStatusGroup.h"
#include "NoteStatusGroup.h"
#include "TagStatusGroup.h"
#include "AspectRatioGroup.h"
#include "FileSizeGroup.h"
#include "AutoColorGroup.h"
#include "ColorLabelGroup.h"
#include "RatingGroup.h"
#include "FileTypeGroup.h"
#include "CreateDateGroup.h"
#include "ModifyDateGroup.h"
#include "../core/AppConfig.h"
#include <QSet>
#include <QDate>
#include "ToolTipOverlay.h"
#include "UiHelper.h"
#include "StyleLibrary.h"
#include "ColorPicker.h"
#include "SearchHistoryPanel.h"
#include "../core/SearchHistoryService.h"
#include <QPushButton>
#include <QMouseEvent>
#include <QCursor>
#include <QGuiApplication>
#include <QScreen>
#include <QPainter>
#include <QComboBox>
#include <QButtonGroup>

using namespace QuarkMeta::Style;

namespace QuarkMeta {


QMap<QString, QColor> FilterPanel::s_colorMap() {
    QMap<QString, QColor> map;
    for (const auto& item : Style::getColorPalette()) {
        map.insert(item.hex, item.color);
    }
    map.insert("#000000", QColor("#000000"));
    map.insert("#FFFFFF", QColor("#FFFFFF"));
    return map;
}


void FilterPanel::syncUIFromFilterState() {
    updateHeaderStatus();
    
    FilterState currentSt = currentFilter();

    QList<StyledCheckBox*> allCheckBoxes = findChildren<StyledCheckBox*>();
    for (auto* cb : allCheckBoxes) {
        ClickableRow* row = qobject_cast<ClickableRow*>(cb->parentWidget());
        if (!row) continue;

        QVariant keyProp = row->property("rowKey");
        if (keyProp.isValid() && !keyProp.toString().isEmpty()) {
            bool shouldCheck = isRowKeyChecked(keyProp.toString(), currentSt);
            cb->blockSignals(true);
            cb->setChecked(shouldCheck);
            cb->blockSignals(false);
            continue;
        }
        
        QLabel* labelWidget = row->findChild<QLabel*>();
        if (!labelWidget) continue;
        
        QString text = labelWidget->text();
        bool shouldCheck = false;
        
        QVariant ratingProp = row->property("ratingValue");
        if (ratingProp.isValid()) {
            shouldCheck = currentSt.ratings.contains(ratingProp.toInt());
        }
        else if (text == "无评级") shouldCheck = currentSt.ratings.contains(0);
        else if (text.contains("★")) shouldCheck = currentSt.ratings.contains(text.count("★"));
        
        else if (text == "无色标") shouldCheck = (currentSt.manualColors.contains("无色标") || currentSt.manualColors.contains(""));
        else if (currentSt.manualColors.contains(text)) shouldCheck = true;

        else if (currentSt.types.contains(text)) shouldCheck = true;
        else if (currentSt.createDates.contains(text)) shouldCheck = true;
        else if (currentSt.modifyDates.contains(text)) shouldCheck = true;
        
        else if (text == "有链接") shouldCheck = (currentSt.linkPresence == FilterState::Yes);
        else if (text == "无链接") shouldCheck = (currentSt.linkPresence == FilterState::No);
        else if (text == "有备注") shouldCheck = (currentSt.notePresence == FilterState::Yes);
        else if (text == "已标签") shouldCheck = (currentSt.tagPresence == FilterState::Yes);
        else if (text == "未标签") shouldCheck = (currentSt.tagPresence == FilterState::No);
        else if (text == "重复项") shouldCheck = (currentSt.duplicatePresence == FilterState::DuplicateOnly);
        else if (text == "未重复") shouldCheck = (currentSt.duplicatePresence == FilterState::UniqueOnly);
        else if (text == "无备注") shouldCheck = (currentSt.notePresence == FilterState::No);
        else if (text == "横图") shouldCheck = (currentSt.ratio == FilterState::Horizontal);
        else if (text == "竖图") shouldCheck = (currentSt.ratio == FilterState::Vertical);
        else if (text == "方形") shouldCheck = (currentSt.ratio == FilterState::Square);
        else if (text == "16:9") shouldCheck = (currentSt.ratio == FilterState::Ratio169);
        else if (text == "有缩略图") shouldCheck = (currentSt.thumbnailPresence == FilterState::HasThumbnail);
        else if (text == "无缩略图 (提取失败)" || text == "无缩略图 (失败/跳过)") shouldCheck = (currentSt.thumbnailPresence == FilterState::NoThumbnail);

        cb->blockSignals(true);
        cb->setChecked(shouldCheck);
        cb->blockSignals(false);
    }

    QList<ColorBlock*> allBlocks = findChildren<ColorBlock*>();
    for (auto* block : allBlocks) {
        block->blockSignals(true);
        block->setChecked(currentSt.colors.contains(block->color().name().toUpper()));
        block->blockSignals(false);
    }
}

FilterPanel::FilterPanel(QWidget* parent) : QFrame(parent) {
    setAcceptDrops(true);
    m_filterModel = new FilterStateModel(this);
    m_statsEngine = new ScanStatsEngine(this);

    connect(m_filterModel, &FilterStateModel::stateChanged, this, [this](const FilterState& st) {
        m_filter = st;
        emit filterChanged(st);
        updateHeaderStatus();
    });

    setContextMenuPolicy(Qt::CustomContextMenu);

    setObjectName("FilterContainer");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(230);
    // FilterPanel style in style.qss

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    QWidget* topBar = new QWidget(this);
    topBar->setObjectName("ContainerHeader");
    topBar->setFixedHeight(32);
    QHBoxLayout* topL = new QHBoxLayout(topBar);
    topL->setContentsMargins(15, 0, 5, 0);
    topL->setSpacing(5);

    m_iconLabel = new QLabel(topBar);
    topL->addWidget(m_iconLabel);

    m_titleLabel = new QLabel("筛选", topBar);
    m_titleLabel->setObjectName("FilterPanelTitleLabel");
    topL->addWidget(m_titleLabel);

    m_btnClearAll = new QPushButton(topBar);
    m_btnClearAll->setObjectName("FilterHeaderBtn");
    m_btnClearAll->setFixedSize(24, 24);
    m_btnClearAll->setIcon(UiHelper::getIcon("reset_filter", QColor("#B0B0B0")));
    m_btnClearAll->setIconSize(QSize(16, 16));
    m_btnClearAll->setFlat(true);
    m_btnClearAll->setCursor(Qt::PointingHandCursor);
    m_btnClearAll->setProperty("tooltipText", "重置筛选条件（Ctrl + D）");
    m_btnClearAll->installEventFilter(this);
    connect(m_btnClearAll, &QPushButton::clicked, this, [this]() {
        clearAllFilters(true);
        emit resetFilterRequested();
    });

    m_btnPin = new QPushButton(topBar);
    m_btnPin->setObjectName("FilterHeaderBtn");
    m_btnPin->setFixedSize(24, 24);
    m_btnPin->setIcon(UiHelper::getIcon("pin_tilted", QColor("#B0B0B0")));
    m_btnPin->setIconSize(QSize(16, 16));
    m_btnPin->setFlat(true);
    m_btnPin->setCursor(Qt::PointingHandCursor);
    m_btnPin->setProperty("tooltipText", "锁定当前筛选条件");
    m_btnPin->installEventFilter(this);
    connect(m_btnPin, &QPushButton::clicked, this, [this]() {
        m_isFilterPinned = !m_isFilterPinned;
        if (m_isFilterPinned) {
            m_btnPin->setIcon(UiHelper::getIcon("pin", Style::ActiveOrange));
            m_btnPin->setProperty("tooltipText", "当前筛选条件已锁定（目录切换不重置）");
        } else {
            m_btnPin->setIcon(UiHelper::getIcon("pin_tilted", QColor("#B0B0B0")));
            m_btnPin->setProperty("tooltipText", "锁定当前筛选条件");
        }
    });

    m_btnToggleGroups = new QPushButton(topBar);
    m_btnToggleGroups->setObjectName("FilterHeaderBtn");
    m_btnToggleGroups->setFixedSize(24, 24);
    m_btnToggleGroups->setIconSize(QSize(16, 16));
    m_btnToggleGroups->setFlat(true);
    m_btnToggleGroups->setCursor(Qt::PointingHandCursor);
    m_btnToggleGroups->installEventFilter(this);
    connect(m_btnToggleGroups, &QPushButton::clicked, this, &FilterPanel::onToggleAllGroupsClicked);

    topL->addStretch();
    topL->addWidget(m_btnPin, 0, Qt::AlignVCenter);
    topL->addWidget(m_btnToggleGroups, 0, Qt::AlignVCenter);
    topL->addWidget(m_btnClearAll, 0, Qt::AlignVCenter);
    m_mainLayout->addWidget(topBar);

    updateHeaderStatus();

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setObjectName("FilterScrollArea");
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setWidgetResizable(true);

    m_container = new QWidget(m_scrollArea);
    m_container->setObjectName("FilterContainerWidget");
    m_containerLayout = new QVBoxLayout(m_container);
    m_containerLayout->setContentsMargins(0, 0, 0, 10); 
    m_containerLayout->setSpacing(0);
    m_containerLayout->addStretch();

    m_scrollArea->setWidget(m_container);
    m_mainLayout->addWidget(m_scrollArea, 1);

    m_historyPanel = new SearchHistoryPanel(this);

    connect(this, &FilterPanel::filterChanged, this, &FilterPanel::updateHeaderStatus);
}

void FilterPanel::saveFilterHistory(const QString& key, const QString& text) {
    if (text.trimmed().isEmpty()) return;
    SearchHistoryService::instance().appendSearch(key, text);
}

QStringList FilterPanel::getFilterHistory(const QString& key) const {
    return SearchHistoryService::instance().getHistory(key);
}

bool FilterPanel::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonDblClick) {
        QLineEdit* edit = qobject_cast<QLineEdit*>(watched);
        if (edit && edit->objectName() == "FilterSearchEdit") {
            QString key;
            if (edit == m_editType) key = "Type";
            else if (edit == m_editCreateDate) key = "CreateDate";
            else if (edit == m_editModifyDate) key = "ModifyDate";

            if (!key.isEmpty()) {
                m_historyPanel->setCategory(key);
                QStringList history = getFilterHistory(key);
                m_historyPanel->setHistory(history, "最近搜索");
                
                m_historyPanel->disconnect(this);

                connect(m_historyPanel, &SearchHistoryPanel::historyItemClicked, this, [this, edit, key](const QString& text) {
                    edit->setText(text);
                    FilterState st = m_filterModel->state();
                    if (edit == m_editType) st.typeFilterText = text;
                    else if (edit == m_editCreateDate) st.createDateFilterText = text;
                    else if (edit == m_editModifyDate) st.modifyDateFilterText = text;

                    saveFilterHistory(key, text);
                    m_filterModel->setState(st);
                    m_historyPanel->hide();
                });

                m_historyPanel->showBelow(edit);
                return true;
            }
        }
    }

    if (event->type() == QEvent::ToolTip) {
        QString text = watched->property("tooltipText").toString();
        if (!text.isEmpty()) {
            ToolTipOverlay::instance()->showText(QCursor::pos(), text, 0);
            return true;
        }
    } else if (event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonPress) {
        ToolTipOverlay::hideTip();
    }
    
    return QWidget::eventFilter(watched, event);
}

void FilterPanel::clearStats() {
    m_currentStats = QuarkMeta::ScanStats();
    m_ratingCounts.clear();
    m_colorCounts.clear();
    m_typeCounts.clear();
    m_createDateCounts.clear();
    m_modifyDateCounts.clear();
    m_emptyFolderCount = 0;
    syncUIFromFilterState();
}

void FilterPanel::populateStats(const QuarkMeta::ScanStats& stats) {
    if (m_statsEngine) {
        m_statsEngine->updateStats(stats);
    }
    if (m_currentStats == stats) {
        return;
    }
    m_currentStats = stats;
    populate(stats.ratingCounts, stats.colorCounts, stats.typeCounts,
             stats.createDateCounts, stats.modifyDateCounts, stats.emptyFolderCount);
}

void FilterPanel::populate(
    const QMap<int, int>&       ratingCounts,
    const QMap<QString, int>&   colorCounts,
    const QMap<QString, int>&   typeCounts,
    const QMap<QString, int>&   createDateCounts,
    const QMap<QString, int>&   modifyDateCounts,
    int                         emptyFolderCount)
{
    if (ratingCounts.isEmpty() && colorCounts.isEmpty() &&
        typeCounts.isEmpty() && createDateCounts.isEmpty() && modifyDateCounts.isEmpty() &&
        emptyFolderCount == 0 &&
        m_filter.typeFilterText.isEmpty() && m_filter.createDateFilterText.isEmpty() &&
        m_filter.modifyDateFilterText.isEmpty()) {
        return;
    }

    auto getNonZeroColorKeys = [](const QMap<QString, int>& counts) {
        QSet<QString> keys;
        for (auto it = counts.begin(); it != counts.end(); ++it) {
            if (it.value() > 0) keys.insert(it.key());
        }
        return keys;
    };

    bool structureChanged = (m_ratingCounts.keys() != ratingCounts.keys() ||
                             getNonZeroColorKeys(m_colorCounts) != getNonZeroColorKeys(colorCounts) ||
                             m_typeCounts.keys() != typeCounts.keys() ||
                             m_createDateCounts.keys() != createDateCounts.keys() ||
                             m_modifyDateCounts.keys() != modifyDateCounts.keys() ||
                             m_emptyFolderCount != emptyFolderCount);

    m_ratingCounts     = ratingCounts;
    m_colorCounts      = colorCounts;
    m_typeCounts       = typeCounts;
    m_createDateCounts = createDateCounts;
    m_modifyDateCounts = modifyDateCounts;
    m_emptyFolderCount = emptyFolderCount;

    if (structureChanged) {
        rebuildGroups();
    } else {
        syncUIFromFilterState();
        QList<ClickableRow*> rows = m_container->findChildren<ClickableRow*>();
        for (auto* row : rows) {
             QVariant keyProp = row->property("rowKey");
             if (keyProp.isValid() && !keyProp.toString().isEmpty()) {
                 QLabel* cntLabel = row->findChild<QLabel*>("FilterItemCountLabel");
                 if (cntLabel) {
                     cntLabel->setText(QString::number(countForRowKey(keyProp.toString())));
                 }
                 continue;
             }

             QList<QLabel*> labels = row->findChildren<QLabel*>();
             if (labels.size() >= 2) {
                 QLabel* cntLabel = labels.last();
                 QLabel* nameLabel = labels.at(labels.size() - 2);
                 QString name = nameLabel->text();
                 
                 int count = 0;
                 QVariant ratingProp = row->property("ratingValue");
                 if (ratingProp.isValid()) {
                     count = m_ratingCounts.value(ratingProp.toInt(), 0);
                 }
                 else if (name == "无评级") count = m_ratingCounts.value(0, 0);
                 else if (name.contains("★")) count = m_ratingCounts.value(name.count("★"), 0);
                 else if (name == "空文件夹") count = m_emptyFolderCount;
                 else if (name == "文件夹") count = m_typeCounts.value("folder", 0);
                 else if (name == "文件") count = m_typeCounts.value("file", 0);
                 else if (m_typeCounts.contains(name)) count = m_typeCounts.value(name, 0);
                 else if (m_createDateCounts.contains(name)) count = m_createDateCounts.value(name, 0);
                 else if (m_modifyDateCounts.contains(name)) count = m_modifyDateCounts.value(name, 0);
                 else if (name == "无色标") count = m_colorCounts.value("", m_colorCounts.value("无色标", 0));
                 else if (name == "未重复") count = m_currentStats.uniqueCount;
                 else if (name == "重复项") count = m_currentStats.duplicateCount;
                 else if (name == "横图") count = m_currentStats.ratioHorizontalCount;
                 else if (name == "竖图") count = m_currentStats.ratioVerticalCount;
                 else if (name == "方形") count = m_currentStats.ratioSquareCount;
                 else if (name == "16:9") count = m_currentStats.ratio169Count;
                 else if (name == "有链接") count = m_currentStats.hasLinkCount;
                 else if (name == "无链接") count = m_currentStats.noLinkCount;
                 else if (name == "有备注") count = m_currentStats.hasNoteCount;
                 else if (name == "无备注") count = m_currentStats.noNoteCount;
                 else if (name == "已标签") count = m_currentStats.hasTagCount;
                 else if (name == "未标签") count = m_currentStats.noTagCount;
                 else if (name == "有缩略图") count = m_currentStats.hasThumbnailCount;
                 else if (name == "无缩略图 (提取失败)" || name == "无缩略图 (失败/跳过)") count = m_currentStats.noThumbnailCount;
                 else {
                     for (const auto& item : Style::getColorPalette()) {
                         if (item.name == name || item.hex == name) {
                             count = m_colorCounts.value(item.hex, m_colorCounts.value(item.name, 0));
                             break;
                         }
                     }
                 }

                 cntLabel->setText(QString::number(count));
             }
        }
    }
}

void FilterPanel::rebuildDateCheckboxes(bool isCreateDate, bool descending) {
    QVBoxLayout* layout = isCreateDate ? m_createDateLayout : m_modifyDateLayout;
    if (!layout) return;

    while (layout->count() > 1) {
        QLayoutItem* item = layout->takeAt(1);
        if (item->widget()) {
            item->widget()->hide();
            item->widget()->setParent(nullptr);
            item->widget()->deleteLater();
        }
        delete item;
    }

    const QMap<QString, int>& counts = isCreateDate ? m_createDateCounts : m_modifyDateCounts;
    FilterState currentSt = m_filterModel->state();
    QStringList& selected = isCreateDate ? currentSt.createDates : currentSt.modifyDates;

    QStringList dates = counts.keys();
    std::sort(dates.begin(), dates.end(), [descending](const QString& a, const QString& b) {
        QDate dateA = QDate::fromString(a, "dd-MM-yyyy");
        QDate dateB = QDate::fromString(b, "dd-MM-yyyy");
        if (dateA.isValid() && dateB.isValid()) {
            return descending ? (dateA > dateB) : (dateA < dateB);
        }
        return descending ? (a > b) : (a < b);
    });

    for (const QString& d : dates) {
        QString rowKey = isCreateDate ? ("createDate:" + d) : ("modifyDate:" + d);
        QCheckBox* cb = addFilterRow(layout, d, counts[d], Qt::transparent, rowKey);
        cb->blockSignals(true);
        cb->setChecked(selected.contains(d));
        cb->blockSignals(false);
        connect(cb, &QCheckBox::toggled, this, [this, isCreateDate, d](bool on) {
            FilterState st = m_filterModel->state();
            QStringList& targetList = isCreateDate ? st.createDates : st.modifyDates;
            if (on) { if (!targetList.contains(d)) targetList.append(d); }
            else targetList.removeAll(d);
            m_filterModel->setState(st);
        });
    }

    if (m_scrollArea && m_scrollArea->widget()) {
        m_scrollArea->widget()->updateGeometry();
    }
    update();
}

void FilterPanel::rebuildGroups() {
    int savedScrollY = (m_scrollArea && m_scrollArea->verticalScrollBar()) 
                           ? m_scrollArea->verticalScrollBar()->value() : 0;

    updateHeaderStatus();
    m_groupHeaders.clear();

    m_editType = nullptr;
    m_editCreateDate = nullptr;
    m_editModifyDate = nullptr;
    m_createDateLayout = nullptr;
    m_modifyDateLayout = nullptr;

    while (m_containerLayout->count() > 1) {
        QLayoutItem* item = m_containerLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->hide();
            item->widget()->setParent(nullptr);
            item->widget()->deleteLater();
        }
        delete item;
    }

    FilterState currentSt = m_filterModel->state();

    // ── 1. 颜色标记 (移至首位，在标签组上方) ────────────
    {
        QVBoxLayout* gl = nullptr;
        QHBoxLayout* hdrLayout = nullptr;
        QWidget* g = buildGroup("颜色标记", gl, &hdrLayout);

        AutoColorGroup::populate(g, gl, m_filterModel, [this]() {
            rebuildGroups();
        });

        ColorLabelGroup::populate(g, gl, m_filterModel, m_colorCounts, currentSt,
            [this](QVBoxLayout* layout, const QString& label, int count, const QColor& color, const QString& rowKey) {
                return addFilterRow(layout, label, count, color, rowKey);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 2. 标签 ──────────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QWidget* g = buildGroup("标签", gl);

        TagStatusGroup::populate(g, gl, m_filterModel, m_currentStats, currentSt,
            [this](QVBoxLayout* layout, const QString& label, int count, const QString& rowKey) {
                return addFilterRow(layout, label, count, QColor(), rowKey);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 3. 评级 ──────────────────────────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QWidget* g = buildGroup("评级", gl);

        RatingGroup::populate(g, gl, m_filterModel, m_ratingCounts, currentSt,
            [this](QVBoxLayout* layout, const QString& label, int count) {
                return addFilterRow(layout, label, count);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 4. 文件类型 ──────────────────────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QWidget* g = buildGroup("文件类型", gl);

        m_editType = FileTypeGroup::populate(this, g, gl, m_filterModel, currentSt,
            m_emptyFolderCount, m_typeCounts,
            [this](QVBoxLayout* layout, const QString& label, int count) {
                return addFilterRow(layout, label, count);
            },
            [this](const QString& key, const QString& text) {
                saveFilterHistory(key, text);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 5. 创建日期 ──────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QHBoxLayout* hdrLayout = nullptr;
        QWidget* g = buildGroup("创建日期", gl, &hdrLayout);
        m_createDateLayout = gl;

        m_editCreateDate = CreateDateGroup::populate(this, g, hdrLayout, gl, m_filterModel, currentSt,
            m_createDateDesc,
            [this](bool isCreateDate, bool descending) {
                rebuildDateCheckboxes(isCreateDate, descending);
            },
            [this](const QString& key, const QString& text) {
                saveFilterHistory(key, text);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 6. 修改日期 ──────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QHBoxLayout* hdrLayout = nullptr;
        QWidget* g = buildGroup("修改日期", gl, &hdrLayout);
        m_modifyDateLayout = gl;

        m_editModifyDate = ModifyDateGroup::populate(this, g, hdrLayout, gl, m_filterModel, currentSt,
            m_modifyDateDesc,
            [this](bool isCreateDate, bool descending) {
                rebuildDateCheckboxes(isCreateDate, descending);
            },
            [this](const QString& key, const QString& text) {
                saveFilterHistory(key, text);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 7. 链接 ──────────────────────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QWidget* g = buildGroup("链接", gl);

        LinkStatusGroup::populate(g, gl, m_filterModel, m_currentStats, currentSt,
            [this](QVBoxLayout* layout, const QString& label, int count, const QString& rowKey) {
                return addFilterRow(layout, label, count, QColor(), rowKey);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 8. 备注 ──────────────────────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QWidget* g = buildGroup("备注", gl);

        NoteStatusGroup::populate(g, gl, m_filterModel, m_currentStats, currentSt,
            [this](QVBoxLayout* layout, const QString& label, int count, const QString& rowKey) {
                return addFilterRow(layout, label, count, QColor(), rowKey);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 9. 文件大小 ──────────────────────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QWidget* g = buildGroup("文件大小", gl);

        FileSizeGroup::populate(g, gl, m_filterModel);

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 11. 图像比例 ──────────────────────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QWidget* g = buildGroup("图像比例", gl);

        AspectRatioGroup::populate(g, gl, m_filterModel, m_currentStats, currentSt,
            [this](QVBoxLayout* layout, const QString& label, int count, const QString& rowKey) {
                return addFilterRow(layout, label, count, QColor(), rowKey);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 12. 重复状态 ───────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QWidget* g = buildGroup("重复状态", gl);

        DuplicateStatusGroup::populate(gl, m_filterModel, m_currentStats, currentSt,
            [this](QVBoxLayout* layout, const QString& label, int count, const QString& rowKey) {
                return addFilterRow(layout, label, count, QColor(), rowKey);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    // ── 13. 缩略图状态 ───────────────────────────
    {
        QVBoxLayout* gl = nullptr;
        QWidget* g = buildGroup("缩略图状态", gl);

        ThumbnailStatusGroup::populate(gl, m_filterModel, m_currentStats, currentSt,
            [this](QVBoxLayout* layout, const QString& label, int count, const QString& rowKey) {
                return addFilterRow(layout, label, count, QColor(), rowKey);
            });

        m_containerLayout->insertWidget(m_containerLayout->count() - 1, g);
    }

    if (m_scrollArea && m_scrollArea->verticalScrollBar()) {
        m_scrollArea->verticalScrollBar()->setValue(savedScrollY);
    }
}

QWidget* FilterPanel::buildGroup(const QString& title, QVBoxLayout*& outContentLayout,
                                  QHBoxLayout** outHdrLayout) {
    QWidget* wrapper = new QWidget(m_container);
    wrapper->setAttribute(Qt::WA_StyledBackground, true);
    wrapper->setObjectName("FilterGroupWrapper");
    QVBoxLayout* wl = new QVBoxLayout(wrapper);
    wl->setContentsMargins(0, 0, 0, 0);
    wl->setSpacing(0);

    QWidget* hdrRow = new QWidget(wrapper);
    hdrRow->setObjectName("FilterGroupHdrRow");
    if (m_groupHeaders.isEmpty()) {
        hdrRow->setProperty("isFirst", true);
    }
    hdrRow->setFixedHeight(24);
    hdrRow->setAttribute(Qt::WA_StyledBackground, true);

    QHBoxLayout* hdrRowLayout = new QHBoxLayout(hdrRow);
    hdrRowLayout->setContentsMargins(0, 0, 0, 0);
    hdrRowLayout->setSpacing(0);

    QPushButton* hdr = new QPushButton(title, hdrRow);
    hdr->setObjectName("FilterGroupHdrBtn");
    hdr->setCheckable(true);

    bool isCollapsed = AppConfig::instance().getValue(QString("FilterPanel/Collapsed_%1").arg(title), false).toBool();
    hdr->setChecked(!isCollapsed);

    hdr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hdr->setFixedHeight(24);
    hdrRowLayout->addWidget(hdr);

    if (outHdrLayout) *outHdrLayout = hdrRowLayout;

    QWidget* content = new QWidget(wrapper);
    content->setAttribute(Qt::WA_StyledBackground, true);
    content->setObjectName("FilterGroupContent");
    outContentLayout = new QVBoxLayout(content);
    outContentLayout->setContentsMargins(0, 0, 0, 0);
    outContentLayout->setSpacing(0);
    content->setVisible(!isCollapsed);

    connect(hdr, &QPushButton::toggled, this, [title, content](bool checked) {
        content->setVisible(checked);
        AppConfig::instance().setValue(QString("FilterPanel/Collapsed_%1").arg(title), !checked);
    });

    m_groupHeaders.append(hdr);
    bool allCollapsed = AppConfig::instance().getValue("FilterPanel/AllGroupsCollapsed", false).toBool();
    if (allCollapsed) {
        hdr->setChecked(false);
    }

    wl->addWidget(hdrRow);
    wl->addWidget(content);
    return wrapper;
}

bool FilterPanel::isRowKeyChecked(const QString& key, const FilterState& st) const {
    int colonIdx = key.indexOf(':');
    if (colonIdx == -1) return false;

    QString prefix = key.left(colonIdx);
    QString value = key.mid(colonIdx + 1);

    if (prefix == "rating") {
        return st.ratings.contains(value.toInt());
    } else if (prefix == "color") {
        if (st.manualColors.contains(value)) return true;
        QString valUpper = value.toUpper();
        if (!valUpper.isEmpty() && st.manualColors.contains(valUpper)) return true;
        for (const auto& item : Style::getColorPalette()) {
            if ((item.hex.toUpper() == valUpper || item.hex == value) && st.manualColors.contains(item.name)) return true;
        }
        return false;
    } else if (prefix == "type") {
        return st.types.contains(value);
    } else if (prefix == "createDate") {
        return st.createDates.contains(value);
    } else if (prefix == "modifyDate") {
        return st.modifyDates.contains(value);
    } else if (prefix == "tag") {
        if (value == "yes") return st.tagPresence == FilterState::Yes;
        if (value == "no") return st.tagPresence == FilterState::No;
    } else if (prefix == "link") {
        if (value == "yes") return st.linkPresence == FilterState::Yes;
        if (value == "no") return st.linkPresence == FilterState::No;
    } else if (prefix == "note") {
        if (value == "yes") return st.notePresence == FilterState::Yes;
        if (value == "no") return st.notePresence == FilterState::No;
    } else if (prefix == "ratio") {
        if (value == "h") return st.ratio == FilterState::Horizontal;
        if (value == "v") return st.ratio == FilterState::Vertical;
        if (value == "sq") return st.ratio == FilterState::Square;
        if (value == "169") return st.ratio == FilterState::Ratio169;
    } else if (prefix == "dup") {
        if (value == "only") return st.duplicatePresence == FilterState::DuplicateOnly;
        if (value == "unique") return st.duplicatePresence == FilterState::UniqueOnly;
    } else if (prefix == "thumb") {
        if (value == "has") return st.thumbnailPresence == FilterState::HasThumbnail;
        if (value == "none") return st.thumbnailPresence == FilterState::NoThumbnail;
    }
    return false;
}

int FilterPanel::countForRowKey(const QString& key) const {
    int colonIdx = key.indexOf(':');
    if (colonIdx == -1) return 0;

    QString prefix = key.left(colonIdx);
    QString value = key.mid(colonIdx + 1);

    if (prefix == "rating") {
        return m_ratingCounts.value(value.toInt(), 0);
    } else if (prefix == "color") {
        QString name;
        for (const auto& item : Style::getColorPalette()) {
            if (item.hex == value) {
                name = item.name;
                break;
            }
        }
        return m_colorCounts.value(value, m_colorCounts.value(name, 0));
    } else if (prefix == "type") {
        if (value == "空文件夹") return m_emptyFolderCount;
        return m_typeCounts.value(value, 0);
    } else if (prefix == "createDate") {
        return m_createDateCounts.value(value, 0);
    } else if (prefix == "modifyDate") {
        return m_modifyDateCounts.value(value, 0);
    } else if (prefix == "tag") {
        if (value == "yes") return m_currentStats.hasTagCount;
        if (value == "no") return m_currentStats.noTagCount;
    } else if (prefix == "link") {
        if (value == "yes") return m_currentStats.hasLinkCount;
        if (value == "no") return m_currentStats.noLinkCount;
    } else if (prefix == "note") {
        if (value == "yes") return m_currentStats.hasNoteCount;
        if (value == "no") return m_currentStats.noNoteCount;
    } else if (prefix == "ratio") {
        if (value == "h") return m_currentStats.ratioHorizontalCount;
        if (value == "v") return m_currentStats.ratioVerticalCount;
        if (value == "sq") return m_currentStats.ratioSquareCount;
        if (value == "169") return m_currentStats.ratio169Count;
    } else if (prefix == "dup") {
        if (value == "only") return m_currentStats.duplicateCount;
        if (value == "unique") return m_currentStats.uniqueCount;
    } else if (prefix == "thumb") {
        if (value == "has") return m_currentStats.hasThumbnailCount;
        if (value == "none") return m_currentStats.noThumbnailCount;
    }
    return 0;
}

QCheckBox* FilterPanel::addFilterRow(QVBoxLayout* layout, const QString& label, int count, const QColor& dotColor, const QString& rowKey) {
    StyledCheckBox* cb = new StyledCheckBox();

    ClickableRow* row = new ClickableRow(cb);
    if (!rowKey.isEmpty()) {
        row->setProperty("rowKey", rowKey);
    }
    row->setFixedHeight(24);

    QHBoxLayout* rl = new QHBoxLayout(row);
    rl->setContentsMargins(5, 0, 5, 0);
    rl->setSpacing(5);
    rl->addWidget(cb);

    if (dotColor.isValid() && dotColor != Qt::transparent) {
        ColorDot* dot = new ColorDot(dotColor, row);
        rl->addWidget(dot);
    }

    QLabel* lbl = new QLabel(label, row);
    lbl->setObjectName("FilterItemLabel");
    rl->addWidget(lbl, 1);

    QLabel* cnt = new QLabel(QString::number(count), row);
    cnt->setObjectName("FilterItemCountLabel");
    cnt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    rl->addWidget(cnt);

    layout->addWidget(row);
    return cb;
}

void FilterPanel::clearAllFilters(bool force) {
    if (!force && m_isFilterPinned) {
        return;
    }

    if (force && m_isFilterPinned) {
        m_isFilterPinned = false;
        if (m_btnPin) {
            m_btnPin->setIcon(UiHelper::getIcon("pin_tilted", QColor("#B0B0B0")));
            m_btnPin->setProperty("tooltipText", "锁定当前筛选条件");
        }
    }

    if (m_filterModel) {
        m_filterModel->reset(force);
    }

    if (m_editType) m_editType->clear();
    if (m_editCreateDate) m_editCreateDate->clear();
    if (m_editModifyDate) m_editModifyDate->clear();
    
    syncUIFromFilterState();
}

void FilterPanel::updateHeaderStatus() {
    if (!m_iconLabel || !m_titleLabel || !m_btnClearAll || !m_btnToggleGroups) return;
    
    bool active = m_filterModel ? !m_filterModel->state().isEmpty() : !m_filter.isEmpty();
    
    QColor defaultGray = QColor("#888888");
    QColor brandYellow = QColor("#f1c40f");
    m_iconLabel->setPixmap(UiHelper::getIcon("filter_funnel_outline", defaultGray, 18).pixmap(18, 18));
    m_titleLabel->style()->unpolish(m_titleLabel);
    m_titleLabel->style()->polish(m_titleLabel);
    m_titleLabel->update();

    QColor btnColor = active ? brandYellow : QColor("#B0B0B0");
    m_btnClearAll->setIcon(UiHelper::getIcon("reset_filter", btnColor));

    bool allCollapsed = AppConfig::instance().getValue("FilterPanel/AllGroupsCollapsed", false).toBool();
    m_btnToggleGroups->setIcon(UiHelper::getIcon(allCollapsed ? "chevrons_up" : "chevrons_down", QColor("#B0B0B0"), 16));
    m_btnToggleGroups->setProperty("tooltipText", allCollapsed ? "展开所有分组" : "折叠所有分组");
}

void FilterPanel::showDragOverlay(int activeCount) {
    Q_UNUSED(activeCount);
    if (!m_dragOverlayWidget) {
        m_dragOverlayWidget = new QWidget(this);
        m_dragOverlayWidget->setObjectName("DragOverlayWidget");
        m_dragOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

        QVBoxLayout* layout = new QVBoxLayout(m_dragOverlayWidget);
        layout->setContentsMargins(10, 10, 10, 10);
        m_dragOverlayLabel = new QLabel(m_dragOverlayWidget);
        m_dragOverlayLabel->setAlignment(Qt::AlignCenter);
        m_dragOverlayLabel->setStyleSheet("color: #FFFFFF; font-size: 13px; font-weight: bold; line-height: 1.4;");
        layout->addWidget(m_dragOverlayLabel);
    }

    auto activeFolders = MarkedFolderService::instance().getActiveMarked();
    if (!activeFolders.isEmpty()) {
        QStringList lines;
        lines << QString("项目即将分发至 %1 个目标文件夹").arg(activeFolders.size());
        lines << ""; // 空行打散间距
        for (const auto& folderPath : activeFolders) {
            QString name = QFileInfo(folderPath).fileName();
            if (name.isEmpty()) name = folderPath;
            lines << name;
        }
        m_dragOverlayLabel->setText(lines.join("\n"));
    } else {
        m_dragOverlayLabel->setText("尚未标记目的地");
    }

    m_dragOverlayWidget->setGeometry(rect());
    m_dragOverlayWidget->show();
    m_dragOverlayWidget->raise();
}

void FilterPanel::hideDragOverlay() {
    if (m_dragOverlayWidget) {
        m_dragOverlayWidget->hide();
    }
}

void FilterPanel::installRecursiveDragEventFilter(QWidget* widget) {
    if (!widget) return;
    widget->setAcceptDrops(true);
    widget->installEventFilter(this);
    for (QObject* child : widget->children()) {
        if (QWidget* childWidget = qobject_cast<QWidget*>(child)) {
            installRecursiveDragEventFilter(childWidget);
        }
    }
}

void FilterPanel::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        int activeCount = MarkedFolderService::instance().getActiveMarked().size();
        showDragOverlay(activeCount);
        if (activeCount > 0) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
        return;
    }
    QFrame::dragEnterEvent(event);
}

void FilterPanel::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        int activeCount = MarkedFolderService::instance().getActiveMarked().size();
        showDragOverlay(activeCount);
        if (activeCount > 0) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
        return;
    }
    QFrame::dragMoveEvent(event);
}

void FilterPanel::dragLeaveEvent(QDragLeaveEvent* event) {
    hideDragOverlay();
    QFrame::dragLeaveEvent(event);
}

void FilterPanel::dropEvent(QDropEvent* event) {
    hideDragOverlay();
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        QStringList paths;
        for (const QUrl& url : event->mimeData()->urls()) {
            QString localPath = url.toLocalFile();
            if (!localPath.isEmpty()) {
                paths.append(localPath);
            }
        }

        int activeCount = MarkedFolderService::instance().getActiveMarked().size();
        if (activeCount > 0) {
            MarkedFolderService::instance().executeTransferToMarked(paths);
            event->acceptProposedAction();
        } else {
            ToolTipOverlay::instance()->showToast("尚未标记目的地，请先标记文件夹", 2500, QColor("#e81123"));
            event->ignore();
        }
        return;
    }
    QFrame::dropEvent(event);
}

void FilterPanel::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    if (m_dragOverlayWidget && m_dragOverlayWidget->isVisible()) {
        m_dragOverlayWidget->setGeometry(rect());
    }
}

void FilterPanel::onToggleAllGroupsClicked() {
    bool currentlyCollapsed = AppConfig::instance().getValue("FilterPanel/AllGroupsCollapsed", false).toBool();
    bool targetCollapsed = !currentlyCollapsed;

    for (QPushButton* hdr : m_groupHeaders) {
        if (hdr) hdr->setChecked(!targetCollapsed);
    }

    AppConfig::instance().setValue("FilterPanel/AllGroupsCollapsed", targetCollapsed);
    updateHeaderStatus();
}

void FilterPanel::setMirrorSource(bool isMirror) {
    Q_UNUSED(isMirror);
}

void FilterPanel::applySmartFolderRecord(const SmartFolderRecord& record) {
    if (!m_filterModel) return;

    FilterState st;
    for (const auto& rule : record.rules) {
        if (rule.type == SmartRuleType::Rating) {
            bool ok = false;
            int r = rule.value.toInt(&ok);
            if (ok) st.ratings.append(r);
        } else if (rule.type == SmartRuleType::Color) {
            if (!rule.value.isEmpty()) st.manualColors.append(rule.value);
        } else if (rule.type == SmartRuleType::AutoColor) {
            if (!rule.value.isEmpty()) st.colors.append(rule.value);
        } else if (rule.type == SmartRuleType::Extension) {
            if (!rule.value.isEmpty()) st.types.append(rule.value);
        } else if (rule.type == SmartRuleType::Name) {
            st.typeFilterText = rule.value;
        }
    }

    m_filterModel->setState(st);
    syncUIFromFilterState();
}

void FilterPanel::selectColor(const QColor& color) {
    QString hex = color.name().toUpper();
    AutoColorGroup::pushRecent(hex);
    
    FilterState st = m_filterModel->state();
    st.colors.clear();
    st.colors.append(hex);

    int hue = color.hsvHue();
    if (hue != -1) {
        st.colorHue = hue;
    }

    m_filterModel->setState(st);
    rebuildGroups();
}

} // namespace QuarkMeta
