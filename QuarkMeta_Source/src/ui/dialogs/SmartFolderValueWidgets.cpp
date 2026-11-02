#include "SmartFolderValueWidgets.h"
#include "../TagPillContainer.h"
#include "../StyleLibrary.h"
#include "../../ui/SvgIcons.h"
#include "../../ui/UiHelper.h"
#include <QPainter>
#include <QPixmap>
#include <QCalendarWidget>
#include <QPoint>
#include "../components/ColorWheel.h"
#include <QLabel>
#include <QSlider>

namespace QuarkMeta {

// --- TextValueWidget ---
TextValueWidget::TextValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_edit = new QLineEdit(this);
    m_edit->setObjectName("SmartFolderLineEdit");
    m_edit->setPlaceholderText("请输入内容...");
    layout->addWidget(m_edit);
    connect(m_edit, &QLineEdit::textChanged, this, &ISmartFolderValueWidget::valueChanged);
}

void TextValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.value = m_edit->text().trimmed();
}

void TextValueWidget::setRuleValues(const SmartFolderRule& rule) {
    m_edit->setText(rule.value);
}

// --- StarRatingValueWidget ---
StarRatingValueWidget::StarRatingValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_combo = new QComboBox(this);
    m_combo->setObjectName("SmartFolderComboBox");

    m_combo->setIconSize(QSize(100, 18));

    // 添加“无”选项（对应 rating = 0），专门筛选无星级/评分的项目
    m_combo->addItem("无", 0);

    const int starSize = 14;
    const int spacing = 4;
    const int fullIconWidth = 5 * starSize + 4 * spacing; // 86px 固定画布宽度，保证图标严格靠左对齐

    for (int stars = 1; stars <= 5; ++stars) {
        QPixmap pixmap(fullIconWidth, starSize);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        int x = 0;
        for (int i = 0; i < stars; ++i) {
            QIcon icon = UiHelper::getIcon("star_filled", QColor("#C5D1DE"));
            icon.paint(&painter, x, 0, starSize, starSize);
            x += starSize + spacing;
        }

        m_combo->addItem(QIcon(pixmap), "", stars);
    }

    layout->addWidget(m_combo);
    connect(m_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ISmartFolderValueWidget::valueChanged);
}

void StarRatingValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.value = QString::number(m_combo->currentData().toInt());
    rule.num1 = m_combo->currentData().toInt();
}

void StarRatingValueWidget::setRuleValues(const SmartFolderRule& rule) {
    int r = rule.value.toInt();
    if (r == 0 && rule.num1 > 0) r = static_cast<int>(rule.num1);
    int idx = m_combo->findData(r);
    if (idx >= 0) m_combo->setCurrentIndex(idx);
}

// --- SingleDateValueWidget ---
SingleDateValueWidget::SingleDateValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setObjectName("SmartFolderDateEdit");
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    if (m_dateEdit->calendarWidget()) {
        m_dateEdit->calendarWidget()->setMinimumWidth(260);
    }
    layout->addWidget(m_dateEdit);
    connect(m_dateEdit, &QDateEdit::dateChanged, this, &ISmartFolderValueWidget::valueChanged);
}

void SingleDateValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.date1 = m_dateEdit->date().toString("yyyy-MM-dd");
    rule.value = rule.date1;
}

void SingleDateValueWidget::setRuleValues(const SmartFolderRule& rule) {
    QString dStr = !rule.date1.isEmpty() ? rule.date1 : rule.value;
    QDate d = QDate::fromString(dStr, "yyyy-MM-dd");
    if (d.isValid()) {
        m_dateEdit->setDate(d);
    } else {
        m_dateEdit->setDate(QDate::currentDate());
    }
}

// --- DateRangeValueWidget ---
DateRangeValueWidget::DateRangeValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_date1Edit = new QDateEdit(QDate::currentDate(), this);
    m_date1Edit->setObjectName("SmartFolderDateEdit");
    m_date1Edit->setCalendarPopup(true);
    m_date1Edit->setDisplayFormat("yyyy-MM-dd");
    if (m_date1Edit->calendarWidget()) {
        m_date1Edit->calendarWidget()->setMinimumWidth(260);
    }

    m_date2Edit = new QDateEdit(QDate::currentDate(), this);
    m_date2Edit->setObjectName("SmartFolderDateEdit");
    m_date2Edit->setCalendarPopup(true);
    m_date2Edit->setDisplayFormat("yyyy-MM-dd");
    if (m_date2Edit->calendarWidget()) {
        m_date2Edit->calendarWidget()->setMinimumWidth(260);
    }

    layout->addWidget(m_date1Edit);
    QLabel* label = new QLabel("至", this);
    label->setObjectName("SmartFolderLabel");
    layout->addWidget(label);
    layout->addWidget(m_date2Edit);

    connect(m_date1Edit, &QDateEdit::dateChanged, this, &ISmartFolderValueWidget::valueChanged);
    connect(m_date2Edit, &QDateEdit::dateChanged, this, &ISmartFolderValueWidget::valueChanged);
}

void DateRangeValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.date1 = m_date1Edit->date().toString("yyyy-MM-dd");
    rule.date2 = m_date2Edit->date().toString("yyyy-MM-dd");
}

void DateRangeValueWidget::setRuleValues(const SmartFolderRule& rule) {
    QDate d1 = QDate::fromString(rule.date1, "yyyy-MM-dd");
    QDate d2 = QDate::fromString(rule.date2, "yyyy-MM-dd");
    m_date1Edit->setDate(d1.isValid() ? d1 : QDate::currentDate());
    m_date2Edit->setDate(d2.isValid() ? d2 : QDate::currentDate());
}

// --- RecentDaysValueWidget ---
RecentDaysValueWidget::RecentDaysValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_spinBox = new QSpinBox(this);
    m_spinBox->setObjectName("SmartFolderSpinBox");
    m_spinBox->setRange(1, 3650);
    m_spinBox->setValue(7);
    m_spinBox->setSuffix(" 天");
    layout->addWidget(m_spinBox);
    connect(m_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ISmartFolderValueWidget::valueChanged);
}

void RecentDaysValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.days = m_spinBox->value();
    rule.value = QString::number(rule.days);
}

void RecentDaysValueWidget::setRuleValues(const SmartFolderRule& rule) {
    int d = rule.days > 0 ? rule.days : rule.value.toInt();
    m_spinBox->setValue(d > 0 ? d : 7);
}

// --- NumericValueWidget ---
NumericValueWidget::NumericValueWidget(const QString& suffix, QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_spinBox = new QSpinBox(this);
    m_spinBox->setObjectName("SmartFolderSpinBox");
    m_spinBox->setRange(0, 100000);
    m_spinBox->setValue(100);
    if (!suffix.isEmpty()) m_spinBox->setSuffix(" " + suffix);
    layout->addWidget(m_spinBox);
    connect(m_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ISmartFolderValueWidget::valueChanged);
}

void NumericValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.num1 = m_spinBox->value();
    rule.value = QString::number(rule.num1);
}

void NumericValueWidget::setRuleValues(const SmartFolderRule& rule) {
    qint64 v = rule.num1 > 0 ? rule.num1 : rule.value.toLongLong();
    m_spinBox->setValue(static_cast<int>(v));
}

// --- NumericRangeValueWidget ---
NumericRangeValueWidget::NumericRangeValueWidget(const QString& suffix, QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_spin1 = new QSpinBox(this);
    m_spin1->setObjectName("SmartFolderSpinBox");
    m_spin1->setRange(0, 100000);
    m_spin1->setValue(100);
    if (!suffix.isEmpty()) m_spin1->setSuffix(" " + suffix);

    m_spin2 = new QSpinBox(this);
    m_spin2->setObjectName("SmartFolderSpinBox");
    m_spin2->setRange(0, 100000);
    m_spin2->setValue(1000);
    if (!suffix.isEmpty()) m_spin2->setSuffix(" " + suffix);

    layout->addWidget(m_spin1);
    QLabel* label = new QLabel("至", this);
    label->setObjectName("SmartFolderLabel");
    layout->addWidget(label);
    layout->addWidget(m_spin2);

    connect(m_spin1, QOverload<int>::of(&QSpinBox::valueChanged), this, &ISmartFolderValueWidget::valueChanged);
    connect(m_spin2, QOverload<int>::of(&QSpinBox::valueChanged), this, &ISmartFolderValueWidget::valueChanged);
}

void NumericRangeValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.num1 = m_spin1->value();
    rule.num2 = m_spin2->value();
}

void NumericRangeValueWidget::setRuleValues(const SmartFolderRule& rule) {
    m_spin1->setValue(static_cast<int>(rule.num1));
    m_spin2->setValue(static_cast<int>(rule.num2));
}

// --- FileSizeValueWidget ---
FileSizeValueWidget::FileSizeValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_spin = new QSpinBox(this);
    m_spin->setObjectName("SmartFolderSpinBox");
    m_spin->setRange(1, 10240);
    m_spin->setValue(10);

    m_unitCombo = new QComboBox(this);
    m_unitCombo->setObjectName("SmartFolderComboBox");
    m_unitCombo->addItem("MB", "MB");
    m_unitCombo->addItem("KB", "KB");
    m_unitCombo->addItem("GB", "GB");

    layout->addWidget(m_spin);
    layout->addWidget(m_unitCombo);

    connect(m_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ISmartFolderValueWidget::valueChanged);
    connect(m_unitCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ISmartFolderValueWidget::valueChanged);
}

void FileSizeValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.num1 = m_spin->value();
    rule.unit = m_unitCombo->currentData().toString();
}

void FileSizeValueWidget::setRuleValues(const SmartFolderRule& rule) {
    m_spin->setValue(rule.num1 > 0 ? static_cast<int>(rule.num1) : 10);
    int idx = m_unitCombo->findData(rule.unit.isEmpty() ? "MB" : rule.unit);
    if (idx >= 0) m_unitCombo->setCurrentIndex(idx);
}

// --- FileSizeRangeValueWidget ---
FileSizeRangeValueWidget::FileSizeRangeValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_spin1 = new QSpinBox(this);
    m_spin1->setObjectName("SmartFolderSpinBox");
    m_spin1->setRange(1, 10240);
    m_spin1->setValue(1);

    m_spin2 = new QSpinBox(this);
    m_spin2->setObjectName("SmartFolderSpinBox");
    m_spin2->setRange(1, 10240);
    m_spin2->setValue(100);

    m_unitCombo = new QComboBox(this);
    m_unitCombo->setObjectName("SmartFolderComboBox");
    m_unitCombo->addItem("MB", "MB");
    m_unitCombo->addItem("KB", "KB");
    m_unitCombo->addItem("GB", "GB");

    layout->addWidget(m_spin1);
    QLabel* label = new QLabel("至", this);
    label->setObjectName("SmartFolderLabel");
    layout->addWidget(label);
    layout->addWidget(m_spin2);
    layout->addWidget(m_unitCombo);

    connect(m_spin1, QOverload<int>::of(&QSpinBox::valueChanged), this, &ISmartFolderValueWidget::valueChanged);
    connect(m_spin2, QOverload<int>::of(&QSpinBox::valueChanged), this, &ISmartFolderValueWidget::valueChanged);
    connect(m_unitCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ISmartFolderValueWidget::valueChanged);
}

void FileSizeRangeValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.num1 = m_spin1->value();
    rule.num2 = m_spin2->value();
    rule.unit = m_unitCombo->currentData().toString();
}

void FileSizeRangeValueWidget::setRuleValues(const SmartFolderRule& rule) {
    m_spin1->setValue(rule.num1 > 0 ? static_cast<int>(rule.num1) : 1);
    m_spin2->setValue(rule.num2 > 0 ? static_cast<int>(rule.num2) : 100);
    int idx = m_unitCombo->findData(rule.unit.isEmpty() ? "MB" : rule.unit);
    if (idx >= 0) m_unitCombo->setCurrentIndex(idx);
}

// --- ManualColorValueWidget ---
ManualColorValueWidget::ManualColorValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_combo = new QComboBox(this);
    m_combo->setObjectName("SmartFolderComboBox");

    auto palette = Style::getColorPalette();
    for (const auto& item : palette) {
        QPixmap pm(14, 14);
        pm.fill(item.color);
        m_combo->addItem(QIcon(pm), item.name, item.hex);
    }

    layout->addWidget(m_combo);
    connect(m_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ISmartFolderValueWidget::valueChanged);
}

void ManualColorValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.value = m_combo->currentData().toString();
}

void ManualColorValueWidget::setRuleValues(const SmartFolderRule& rule) {
    int idx = m_combo->findData(rule.value);
    if (idx >= 0) {
        m_combo->setCurrentIndex(idx);
    } else if (!rule.value.isEmpty()) {
        // 非标 HEX 动态压入保持原样
        QPixmap pm(14, 14);
        pm.fill(QColor(rule.value));
        m_combo->addItem(QIcon(pm), rule.value, rule.value);
        m_combo->setCurrentIndex(m_combo->count() - 1);
    }
}

// --- SmartFolderColorWheelPopup ---
class SmartFolderColorWheelPopup : public QWidget {
public:
    SmartFolderColorWheelPopup(const QString& initialHex, int initialTol, QWidget* parent = nullptr)
        : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
    {
        setAttribute(Qt::WA_StyledBackground, true);
        setStyleSheet(
            "QWidget { background-color: #1E1E1E; border: 1px solid #3E3E42; border-radius: 6px; }"
            "QLabel { color: #CCCCCC; font-size: 12px; border: none; }"
            "QLineEdit { background: #252526; color: #FFFFFF; border: 1px solid #3E3E42; border-radius: 3px; padding: 2px 4px; }"
            "QSlider::groove:horizontal { height: 4px; background: #3E3E42; border-radius: 2px; }"
            "QSlider::sub-page:horizontal { background: #007ACC; border-radius: 2px; }"
            "QSlider::handle:horizontal { width: 12px; margin-top: -4px; margin-bottom: -4px; border-radius: 6px; background: #FFFFFF; }"
        );

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(8);

        // 1. 复用 ColorWheel 色轮组件
        m_wheel = new ColorWheel(this);
        layout->addWidget(m_wheel, 0, Qt::AlignCenter);

        // 2. Hex 文本显示与输入行
        auto* hexRow = new QHBoxLayout();
        hexRow->setContentsMargins(0, 0, 0, 0);
        hexRow->setSpacing(6);
        auto* hexLabel = new QLabel("颜色:", this);
        m_hexEdit = new QLineEdit(this);
        m_hexEdit->setText(initialHex);
        m_hexEdit->setFixedWidth(90);
        hexRow->addWidget(hexLabel);
        hexRow->addWidget(m_hexEdit);
        hexRow->addStretch();
        layout->addLayout(hexRow);

        // 3. 容差滑块调节行
        auto* tolRow = new QHBoxLayout();
        tolRow->setContentsMargins(0, 0, 0, 0);
        tolRow->setSpacing(6);
        auto* tolLabel = new QLabel("容差:", this);
        m_tolSlider = new QSlider(Qt::Horizontal, this);
        m_tolSlider->setRange(0, 100);
        m_tolSlider->setValue(initialTol);
        m_tolValLabel = new QLabel(QString::number(initialTol), this);
        m_tolValLabel->setFixedWidth(28);
        tolRow->addWidget(tolLabel);
        tolRow->addWidget(m_tolSlider, 1);
        tolRow->addWidget(m_tolValLabel);
        layout->addLayout(tolRow);

        // 初始化色轮值
        QColor initCol(initialHex);
        int hue = initCol.isValid() ? qMax(0, initCol.hsvHue()) : 0;
        m_wheel->setValues(hue, initialTol);

        // 色轮变动响应
        connect(m_wheel, &ColorWheel::changed, this, [this](int h, int p) {
            QColor col = QColor::fromHsv(h, 255, 255);
            QString hex = col.name().toUpper();
            m_hexEdit->setText(hex);
            m_tolSlider->setValue(p);
            m_tolValLabel->setText(QString::number(p));
            if (m_callback) m_callback(hex, p);
        });

        // 容差变动响应
        connect(m_tolSlider, &QSlider::valueChanged, this, [this](int val) {
            m_tolValLabel->setText(QString::number(val));
            m_wheel->setValues(m_wheel->hue(), val);
            if (m_callback) m_callback(m_hexEdit->text().trimmed().toUpper(), val);
        });

        // Hex 输入回车响应
        connect(m_hexEdit, &QLineEdit::returnPressed, this, [this]() {
            QString txt = m_hexEdit->text().trimmed();
            if (!txt.startsWith('#')) txt.prepend('#');
            QColor c(txt);
            if (c.isValid()) {
                int h = qMax(0, c.hsvHue());
                m_wheel->setValues(h, m_tolSlider->value());
                if (m_callback) m_callback(c.name().toUpper(), m_tolSlider->value());
            }
        });
    }

    void setOnColorChanged(std::function<void(const QString&, int)> cb) {
        m_callback = std::move(cb);
    }

private:
    ColorWheel* m_wheel = nullptr;
    QLineEdit* m_hexEdit = nullptr;
    QSlider* m_tolSlider = nullptr;
    QLabel* m_tolValLabel = nullptr;
    std::function<void(const QString&, int)> m_callback;
};

// --- AutoColorValueWidget ---
AutoColorValueWidget::AutoColorValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    m_colorBtn = new QPushButton(this);
    m_colorBtn->setObjectName("SmartFolderAutoColorBtn");
    m_colorBtn->setFixedSize(28, 24);
    m_colorBtn->setCursor(Qt::PointingHandCursor);
    updateBtnStyle();
    connect(m_colorBtn, &QPushButton::clicked, this, &AutoColorValueWidget::openColorWheelPopup);
    layout->addWidget(m_colorBtn);

    m_toleranceSpin = new QSpinBox(this);
    m_toleranceSpin->setObjectName("SmartFolderSpinBox");
    m_toleranceSpin->setRange(0, 100);
    m_toleranceSpin->setValue(50);
    m_toleranceSpin->setPrefix("容差: ");

    layout->addWidget(m_toleranceSpin, 1);

    connect(m_toleranceSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ISmartFolderValueWidget::valueChanged);
}

void AutoColorValueWidget::updateBtnStyle() {
    m_colorBtn->setStyleSheet(QString(
        "QPushButton#SmartFolderAutoColorBtn { "
        "  background-color: %1; "
        "  border: 1px solid #444444; "
        "  border-radius: 4px; "
        "} "
        "QPushButton#SmartFolderAutoColorBtn:hover { "
        "  border: 1px solid #378ADD; "
        "}"
    ).arg(m_colorHex));
    m_colorBtn->setToolTip(m_colorHex);
}

void AutoColorValueWidget::openColorWheelPopup() {
    auto* popup = new SmartFolderColorWheelPopup(m_colorHex, m_toleranceSpin->value(), this);
    popup->setAttribute(Qt::WA_DeleteOnClose);

    QPoint globalPos = m_colorBtn->mapToGlobal(QPoint(0, m_colorBtn->height() + 2));
    popup->move(globalPos);

    popup->setOnColorChanged([this](const QString& hex, int tol) {
        m_colorHex = hex;
        m_toleranceSpin->setValue(tol);
        updateBtnStyle();
        emit valueChanged();
    });

    popup->show();
}

void AutoColorValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.value = m_colorHex;
    rule.tolerance = m_toleranceSpin->value();
}

void AutoColorValueWidget::setRuleValues(const SmartFolderRule& rule) {
    if (!rule.value.isEmpty()) {
        m_colorHex = rule.value.toUpper();
    }
    // 允许 0~100 全区间合法容差精准还原，仅在超出范围时兜底 50
    m_toleranceSpin->setValue((rule.tolerance >= 0 && rule.tolerance <= 100) ? rule.tolerance : 50);
    updateBtnStyle();
}

// --- TagPillValueWidget ---
TagPillValueWidget::TagPillValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_tagPillContainer = new TagPillContainer(this, TagTriggerSource::SmartFolderDialog);
    layout->addWidget(m_tagPillContainer);

    connect(m_tagPillContainer, &TagPillContainer::tagsChanged, this, &ISmartFolderValueWidget::valueChanged);
}

void TagPillValueWidget::getRuleValues(SmartFolderRule& rule) const {
    rule.tags = m_tagPillContainer->tags();
    rule.value = rule.tags.join(",");
}

void TagPillValueWidget::setRuleValues(const SmartFolderRule& rule) {
    m_tagPillContainer->setTags(rule.tags);
}

// --- DummyValueWidget ---
DummyValueWidget::DummyValueWidget(QWidget* parent) : ISmartFolderValueWidget(parent) {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    // 留空，不显示任何遮挡提示控件，由布局自动留空占位
}

void DummyValueWidget::getRuleValues(SmartFolderRule& /*rule*/) const {}
void DummyValueWidget::setRuleValues(const SmartFolderRule& /*rule*/) {}

} // namespace QuarkMeta
