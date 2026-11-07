#include "AutoColorGroup.h"
#include "components/ColorWheel.h"
#include "components/ColorBlock.h"
#include "components/FlowLayout.h"
#include "ToolTipOverlay.h"
#include "UiHelper.h"
#include "AppConfig.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QEvent>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QCursor>

namespace QuarkMeta {

namespace {

struct SavedColorItem {
    QString hex;
    int hue = 0;
    int precision = 50;
    int area = 0;

    QString toString() const {
        return QString("%1|%2|%3|%4").arg(hex.toUpper()).arg(hue).arg(precision).arg(area);
    }

    static SavedColorItem fromString(const QString& str) {
        SavedColorItem item;
        QStringList parts = str.split('|');
        if (parts.size() >= 1) item.hex = parts[0].toUpper();
        if (parts.size() >= 2) item.hue = parts[1].toInt();
        if (parts.size() >= 3) item.precision = parts[2].toInt();
        if (parts.size() >= 4) item.area = parts[3].toInt();
        return item;
    }

    bool operator==(const SavedColorItem& other) const {
        return hex.compare(other.hex, Qt::CaseInsensitive) == 0 &&
               hue == other.hue &&
               precision == other.precision &&
               area == other.area;
    }
};

QList<SavedColorItem> loadSavedColors() {
    QList<SavedColorItem> list;
    QStringList rawList = AppConfig::instance().getValue("Filter/SavedColors").toStringList();
    for (const QString& str : rawList) {
        if (!str.trimmed().isEmpty()) {
            list.append(SavedColorItem::fromString(str));
        }
    }
    return list;
}

void saveSavedColors(const QList<SavedColorItem>& list) {
    QStringList rawList;
    for (const auto& item : list) {
        rawList.append(item.toString());
    }
    AppConfig::instance().setValue("Filter/SavedColors", rawList);
}

void addSavedColor(const SavedColorItem& item) {
    QList<SavedColorItem> list = loadSavedColors();
    for (const auto& existing : list) {
        if (existing == item) return;
    }
    list.append(item);
    saveSavedColors(list);
}

void removeSavedColor(int index) {
    QList<SavedColorItem> list = loadSavedColors();
    if (index >= 0 && index < list.size()) {
        list.removeAt(index);
        saveSavedColors(list);
    }
}

class SavedColorFilter : public QObject {
public:
    SavedColorFilter(int index, const SavedColorItem& item, std::function<void(int)> onDelete, QObject* parent = nullptr)
        : QObject(parent), m_index(index), m_item(item), m_onDelete(onDelete) {}

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        QWidget* widget = qobject_cast<QWidget*>(watched);
        if (!widget) return false;

        if (event->type() == QEvent::Enter || event->type() == QEvent::MouseMove) {
            QString tip = QString("%1\n精准度: %2%\n占比: %3%")
                              .arg(m_item.hex.toUpper())
                              .arg(m_item.precision)
                              .arg(m_item.area);
            ToolTipOverlay::instance()->showText(QCursor::pos(), tip, 0);
            return false;
        } else if (event->type() == QEvent::Leave) {
            ToolTipOverlay::hideTip();
            return false;
        } else if (event->type() == QEvent::ContextMenu) {
            QContextMenuEvent* ce = static_cast<QContextMenuEvent*>(event);
            QMenu menu(widget);
            UiHelper::applyMenuStyle(&menu);
            QAction* actDel = menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 16), "删除");
            if (menu.exec(ce->globalPos()) == actDel) {
                ToolTipOverlay::hideTip();
                if (m_onDelete) m_onDelete(m_index);
            }
            return true;
        }
        return false;
    }

private:
    int m_index;
    SavedColorItem m_item;
    std::function<void(int)> m_onDelete;
};

class SliderToolTipFilter : public QObject {
public:
    explicit SliderToolTipFilter(QObject* parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        QSlider* slider = qobject_cast<QSlider*>(watched);
        if (!slider) return false;

        switch (event->type()) {
            case QEvent::MouseMove:
            case QEvent::MouseButtonPress: {
                QPoint globalPos = QCursor::pos();
                ToolTipOverlay::instance()->showText(globalPos, QString("%1%").arg(slider->value()), 0);
                break;
            }
            case QEvent::MouseButtonRelease:
            case QEvent::Leave: {
                ToolTipOverlay::hideTip();
                break;
            }
            default:
                break;
        }
        return false;
    }
};

} // namespace

void AutoColorGroup::pushRecent(const QString& hex) {
    if (hex.isEmpty()) return;
    QString hexUpper = hex.toUpper();
    if (!hexUpper.startsWith('#')) {
        hexUpper = "#" + hexUpper;
    }

    QStringList recent = AppConfig::instance().getValue("Filter/RecentColors").toStringList();
    recent.removeAll(hexUpper);
    recent.prepend(hexUpper);
    while (recent.size() > 50) {
        recent.removeLast();
    }
    AppConfig::instance().setValue("Filter/RecentColors", recent);
}

void AutoColorGroup::populate(QWidget* parentWidget,
                             QVBoxLayout* contentLayout,
                             FilterStateModel* filterModel,
                             std::function<void()> requestRebuild) {
    if (!contentLayout || !filterModel) return;

    QWidget* container = new QWidget(parentWidget ? parentWidget : contentLayout->parentWidget());
    container->setObjectName("AutoColorGroupContainer");
    QVBoxLayout* groupLayout = new QVBoxLayout(container);
    groupLayout->setContentsMargins(0, 0, 0, 0);
    groupLayout->setSpacing(4);

    const FilterState currentState = filterModel->state();

    // 0. 色码输入框行 (位于色轮上方)
    QWidget* editRow = new QWidget(container);
    QHBoxLayout* editLayout = new QHBoxLayout(editRow);
    editLayout->setContentsMargins(5, 6, 5, 4);
    editLayout->setSpacing(0);

    QLineEdit* editColorHex = new QLineEdit(editRow);
    editColorHex->setObjectName("FilterSearchEdit");
    editColorHex->setFixedHeight(22);
    editColorHex->setClearButtonEnabled(true);
    editColorHex->setPlaceholderText("例: #350D0D 或 350D0D");

    if (!currentState.colors.isEmpty()) {
        editColorHex->setText(currentState.colors.first());
    }

    auto applyHexFilter = [filterModel, editColorHex]() {
        QString raw = editColorHex->text().trimmed();
        if (raw.isEmpty()) {
            FilterState st = filterModel->state();
            if (!st.colors.isEmpty()) {
                st.colors.clear();
                filterModel->setState(st);
            }
            return;
        }

        QString hex = raw;
        if (!hex.startsWith('#') && (hex.length() == 6 || hex.length() == 3)) {
            hex.prepend('#');
        }

        QColor col(hex);
        if (col.isValid()) {
            QString formattedHex = col.name().toUpper();
            pushRecent(formattedHex);

            FilterState st = filterModel->state();
            st.colors = QStringList{ formattedHex };
            int h = col.hsvHue();
            if (h >= 0) {
                st.colorHue = h;
                AppConfig::instance().setValue("Filter/ColorHue", h);
            }
            filterModel->setState(st);
        }
    };

    QObject::connect(editColorHex, &QLineEdit::returnPressed, container, applyHexFilter);
    QObject::connect(editColorHex, &QLineEdit::textChanged, container, [filterModel](const QString& text) {
        if (text.trimmed().isEmpty()) {
            FilterState st = filterModel->state();
            if (!st.colors.isEmpty()) {
                st.colors.clear();
                filterModel->setState(st);
            }
        }
    });

    editLayout->addWidget(editColorHex);
    groupLayout->addWidget(editRow);

    // 1. 色轮 ColorWheel 居中布局
    QWidget* wheelContainer = new QWidget(container);
    QHBoxLayout* wheelLayout = new QHBoxLayout(wheelContainer);
    wheelLayout->setContentsMargins(8, 4, 8, 4);
    wheelLayout->setSpacing(0);

    ColorWheel* colorWheel = new ColorWheel(wheelContainer);
    colorWheel->setValues(currentState.colorHue, currentState.colorPrecision);
    wheelLayout->addWidget(colorWheel, 0, Qt::AlignCenter);
    groupLayout->addWidget(wheelContainer);

    // 2. "占比:" 行
    QWidget* areaRow = new QWidget(container);
    QHBoxLayout* areaLayout = new QHBoxLayout(areaRow);
    areaLayout->setContentsMargins(10, 4, 10, 4);
    areaLayout->setSpacing(8);

    QLabel* areaLabel = new QLabel("占比:", areaRow);
    areaLabel->setObjectName("FilterColorSliderLabel");
    QSlider* areaSlider = new QSlider(Qt::Horizontal, areaRow);
    areaSlider->setObjectName("FilterColorSlider");
    areaSlider->setRange(0, 100);
    areaSlider->setValue(currentState.minColorArea);

    areaLayout->addWidget(areaLabel);
    areaLayout->addWidget(areaSlider, 1);
    groupLayout->addWidget(areaRow);

    // 色轮松手响应
    QObject::connect(colorWheel, &ColorWheel::changed, container, [filterModel, colorWheel, areaSlider, requestRebuild](int hue, int precision) {
        QColor col = QColor::fromHsv(hue, 255, 255);
        QString hex = col.name().toUpper();
        pushRecent(hex);

        int area = areaSlider->value();
        AppConfig::instance().setValue("Filter/ColorHue", hue);
        AppConfig::instance().setValue("Filter/ColorPrecision", precision);
        AppConfig::instance().setValue("Filter/MinColorArea", area);

        FilterState st = filterModel->state();
        st.colors = QStringList{ hex };
        st.colorHue = hue;
        st.colorPrecision = precision;
        st.minColorArea = area;
        filterModel->setState(st);

        if (requestRebuild) requestRebuild();
    });

    // 占比防抖 100ms 定时器
    QTimer* debounceTimer = new QTimer(container);
    debounceTimer->setSingleShot(true);
    debounceTimer->setInterval(100);

    QObject::connect(areaSlider, &QSlider::valueChanged, container, [debounceTimer]() {
        debounceTimer->start();
    });

    QObject::connect(debounceTimer, &QTimer::timeout, container, [filterModel, areaSlider]() {
        int area = areaSlider->value();
        AppConfig::instance().setValue("Filter/MinColorArea", area);

        FilterState st = filterModel->state();
        st.minColorArea = area;
        filterModel->setState(st);
    });

    // 占比滑条事件过滤器（ToolTip）
    SliderToolTipFilter* tipFilter = new SliderToolTipFilter(container);
    areaSlider->installEventFilter(tipFilter);

    // 3. 小标题 "已保存" + 保存小按钮 "+"
    QWidget* savedHeaderRow = new QWidget(container);
    QHBoxLayout* savedHdrLayout = new QHBoxLayout(savedHeaderRow);
    savedHdrLayout->setContentsMargins(8, 4, 8, 0);
    savedHdrLayout->setSpacing(6);

    QLabel* lblSaved = new QLabel("已保存", savedHeaderRow);
    lblSaved->setObjectName("FilterColorSubTitle");
    lblSaved->setObjectName("AutoColorGroup_lblSaved_1");

    QPushButton* btnAddSaved = new QPushButton("+", savedHeaderRow);
    btnAddSaved->setFixedSize(16, 16);
    btnAddSaved->setCursor(Qt::PointingHandCursor);
    btnAddSaved->setToolTip("保存当前颜色筛选设置");
    btnAddSaved->setStyleSheet("QPushButton { font-weight: bold; font-size: 12px; padding: 0px; border: 1px solid #444; border-radius: 3px; background: transparent; color: #BBB; }"
                               "QPushButton:hover { background: #3E3E42; color: #FFF; }"
                               "QPushButton:disabled { color: #555; border-color: #333; }");

    bool hasSelectedColor = !currentState.colors.isEmpty();
    btnAddSaved->setEnabled(hasSelectedColor);

    savedHdrLayout->addWidget(lblSaved);
    savedHdrLayout->addWidget(btnAddSaved);
    savedHdrLayout->addStretch(1);
    groupLayout->addWidget(savedHeaderRow);

    QObject::connect(btnAddSaved, &QPushButton::clicked, container, [filterModel, requestRebuild]() {
        FilterState st = filterModel->state();
        if (!st.colors.isEmpty()) {
            SavedColorItem item;
            item.hex = st.colors.first().toUpper();
            item.hue = st.colorHue;
            item.precision = st.colorPrecision;
            item.area = st.minColorArea;
            addSavedColor(item);
            if (requestRebuild) requestRebuild();
        }
    });

    QList<SavedColorItem> savedItems = loadSavedColors();
    if (!savedItems.isEmpty()) {
        QWidget* savedColorWidget = new QWidget(container);
        FlowLayout* savedFlow = new FlowLayout(savedColorWidget, 0, 2, 2);
        savedFlow->setContentsMargins(8, 0, 8, 0);

        for (int i = 0; i < savedItems.size(); ++i) {
            const auto& sItem = savedItems[i];
            ColorBlock* cb = new ColorBlock(QColor(sItem.hex), savedColorWidget);
            cb->setCount(-1);
            cb->setChecked(currentState.colors.contains(sItem.hex.toUpper()));

            SavedColorFilter* filter = new SavedColorFilter(i, sItem, [requestRebuild](int idx) {
                removeSavedColor(idx);
                if (requestRebuild) requestRebuild();
            }, savedColorWidget);
            cb->installEventFilter(filter);

            QObject::connect(cb, &ColorBlock::clicked, savedColorWidget, [filterModel, colorWheel, areaSlider, requestRebuild, sItem](const QColor&) {
                pushRecent(sItem.hex);

                AppConfig::instance().setValue("Filter/ColorHue", sItem.hue);
                AppConfig::instance().setValue("Filter/ColorPrecision", sItem.precision);
                AppConfig::instance().setValue("Filter/MinColorArea", sItem.area);

                FilterState st = filterModel->state();
                st.colors = QStringList{ sItem.hex.toUpper() };
                st.colorHue = sItem.hue;
                st.colorPrecision = sItem.precision;
                st.minColorArea = sItem.area;

                colorWheel->setValues(st.colorHue, st.colorPrecision);
                areaSlider->setValue(st.minColorArea);

                filterModel->setState(st);
                if (requestRebuild) requestRebuild();
            });

            savedFlow->addWidget(cb);
        }
        groupLayout->addWidget(savedColorWidget);
    }

    // 4. 小标题 "标准色系" + 12 个色块
    QLabel* lblStandard = new QLabel("标准色系", container);
    lblStandard->setObjectName("FilterColorSubTitle");
    groupLayout->addWidget(lblStandard);

    QWidget* stdColorWidget = new QWidget(container);
    FlowLayout* stdFlow = new FlowLayout(stdColorWidget, 0, 2, 2);
    stdFlow->setContentsMargins(8, 0, 8, 0);

    static const QStringList standardHexes = {
        "#E24B4A", "#EF9F27", "#FECF0E", "#639922", "#1D9E75", "#378ADD",
        "#7F77DD", "#E91E63", "#000000", "#808080", "#FFFFFF", "#795548"
    };

    auto handleColorBlockClick = [filterModel, colorWheel, requestRebuild](const QString& hex) {
        QString hexUpper = hex.toUpper();
        QColor col(hexUpper);
        FilterState st = filterModel->state();
        if (st.colors.contains(hexUpper)) {
            st.colors.removeAll(hexUpper);
        } else {
            st.colors = QStringList{ hexUpper };
            pushRecent(hexUpper);
        }

        if (col.isValid()) {
            int h = col.hsvHue();
            if (h >= 0) {
                st.colorHue = h;
            }
        }

        colorWheel->setValues(st.colorHue, st.colorPrecision);
        filterModel->setState(st);
        if (requestRebuild) requestRebuild();
    };

    for (const QString& hex : standardHexes) {
        ColorBlock* cb = new ColorBlock(QColor(hex), stdColorWidget);
        cb->setCount(-1);
        cb->setChecked(currentState.colors.contains(hex.toUpper()));
        QObject::connect(cb, &ColorBlock::clicked, stdColorWidget, [handleColorBlockClick, hex](const QColor&) {
            handleColorBlockClick(hex);
        });
        stdFlow->addWidget(cb);
    }
    groupLayout->addWidget(stdColorWidget);

    // 5. 小标题 "最近筛选" + 最近使用色块
    QStringList recentColors = AppConfig::instance().getValue("Filter/RecentColors").toStringList();
    if (!recentColors.isEmpty()) {
        QLabel* lblRecent = new QLabel("最近筛选", container);
        lblRecent->setObjectName("FilterColorSubTitleRecent");
        groupLayout->addWidget(lblRecent);

        QWidget* recentColorWidget = new QWidget(container);
        FlowLayout* recentFlow = new FlowLayout(recentColorWidget, 0, 2, 2);
        recentFlow->setContentsMargins(8, 0, 8, 0);

        for (const QString& hex : recentColors) {
            ColorBlock* cb = new ColorBlock(QColor(hex), recentColorWidget);
            cb->setCount(-1);
            cb->setChecked(currentState.colors.contains(hex.toUpper()));
            QObject::connect(cb, &ColorBlock::clicked, recentColorWidget, [handleColorBlockClick, hex](const QColor&) {
                handleColorBlockClick(hex);
            });
            recentFlow->addWidget(cb);
        }
        groupLayout->addWidget(recentColorWidget);
    }

    contentLayout->addWidget(container);
}

} // namespace QuarkMeta
