#include "SmartFolderDialog.h"
#include "../UiHelper.h"
#include "AppConfig.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QJsonDocument>
#include <QJsonArray>
#include <QGuiApplication>
#include <QScreen>

namespace QuarkMeta {

// --- SmartFolderDialog Implementation ---

SmartFolderDialog::SmartFolderDialog(QWidget* parent) : FramelessDialog("智能文件夹规则配置", parent) {
    m_record.name = "新智能文件夹";
    m_record.matchMode = "ALL";
    setupUi();
}

SmartFolderDialog::SmartFolderDialog(const SmartFolderRecord& record, QWidget* parent) : FramelessDialog("智能文件夹规则配置", parent), m_record(record) {
    setupUi();
}

void SmartFolderDialog::setupUi() {
    resize(600, 420);

    QWidget* bodyWidget = getContentArea();
    QVBoxLayout* bodyLayout = new QVBoxLayout(bodyWidget);
    bodyLayout->setContentsMargins(20, 15, 20, 20);
    bodyLayout->setSpacing(12);

    // 智能文件夹名称与匹配模式配置
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setSpacing(12);

    QLabel* nameLabel = new QLabel("名称:", bodyWidget);
    nameLabel->setObjectName("SmartFolderLabel");

    m_nameEdit = new QLineEdit(bodyWidget);
    m_nameEdit->setText(m_record.name);

    QLabel* modeLabel = new QLabel("匹配:", bodyWidget);
    modeLabel->setObjectName("SmartFolderLabel");

    m_matchModeCombo = new QComboBox(bodyWidget);
    m_matchModeCombo->addItem("符合以下所有条件 (AND)", "ALL");
    m_matchModeCombo->addItem("符合以下任一条件 (OR)", "ANY");
    if (m_record.matchMode == "ANY") {
        m_matchModeCombo->setCurrentIndex(1);
    } else {
        m_matchModeCombo->setCurrentIndex(0);
    }

    topLayout->addWidget(nameLabel);
    topLayout->addWidget(m_nameEdit, 1);
    topLayout->addWidget(modeLabel);
    topLayout->addWidget(m_matchModeCombo);

    connect(m_nameEdit, &QLineEdit::textChanged, this, [this]() { schedulePreview(); });
    connect(m_matchModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() { schedulePreview(); });

    bodyLayout->addLayout(topLayout);

    // 分割线
    QFrame* line = new QFrame(bodyWidget);
    line->setFrameShape(QFrame::HLine);
    line->setObjectName("SmartFolderSeparatorLine");
    bodyLayout->addWidget(line);

    // 规则列表区域
    m_scrollArea = new QScrollArea(bodyWidget);
    m_scrollArea->setObjectName("SmartFolderScrollArea");
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget(m_scrollArea);
    m_rulesLayout = new QVBoxLayout(scrollContent);
    m_rulesLayout->setContentsMargins(0, 0, 0, 0);
    m_rulesLayout->setSpacing(6);
    m_rulesLayout->addStretch(1);

    m_scrollArea->setWidget(scrollContent);
    bodyLayout->addWidget(m_scrollArea, 1);

    // 添加规则按钮
    m_btnAddRule = new QPushButton("+ 添加新规则", bodyWidget);
    m_btnAddRule->setFixedHeight(30);

    bodyLayout->addWidget(m_btnAddRule);

    // 底部控制按钮
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addStretch(1);

    m_btnCancel = new QPushButton("取消", bodyWidget);
    m_btnCancel->setFixedSize(80, 32);

    m_btnSave = new QPushButton("保存", bodyWidget);
    m_btnSave->setFixedSize(80, 32);

    btnLayout->addWidget(m_btnCancel);
    btnLayout->addWidget(m_btnSave);

    bodyLayout->addLayout(btnLayout);

    connect(m_btnAddRule, &QPushButton::clicked, this, &SmartFolderDialog::onAddRule);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnSave, &QPushButton::clicked, this, &SmartFolderDialog::onSaveAndAccept);

    // 加载已有规则；若为新建且无指定规则，尝试从 AppConfig 恢复上一次保存的整套模板规则
    if (m_record.rules.isEmpty()) {
        QString lastMatchMode = AppConfig::instance().getValue("SmartFolder/LastMatchMode", "").toString();
        if (!lastMatchMode.isEmpty()) {
            m_record.matchMode = lastMatchMode;
            if (lastMatchMode == "ANY") {
                m_matchModeCombo->setCurrentIndex(1);
            } else {
                m_matchModeCombo->setCurrentIndex(0);
            }
        }

        QString lastRulesJsonStr = AppConfig::instance().getValue("SmartFolder/LastTemplateRules", "").toString();
        if (!lastRulesJsonStr.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(lastRulesJsonStr.toUtf8());
            if (doc.isArray()) {
                QJsonArray arr = doc.array();
                for (const auto& val : arr) {
                    if (val.isObject()) {
                        m_record.rules.append(SmartFolderRule::fromJson(val.toObject()));
                    }
                }
            }
        }
    }

    if (!m_record.rules.isEmpty()) {
        for (const auto& rule : m_record.rules) {
            SmartFolderRuleRow* row = new SmartFolderRuleRow(rule, scrollContent);
            connect(row, &SmartFolderRuleRow::removeRequested, this, &SmartFolderDialog::onRemoveRule);
            connect(row, &SmartFolderRuleRow::rowHeightChanged, this, &SmartFolderDialog::adjustAdaptiveSize);
            connect(row, &SmartFolderRuleRow::ruleChanged, this, &SmartFolderDialog::schedulePreview);
            m_rulesLayout->insertWidget(m_rulesLayout->count() - 1, row);
            m_ruleRows.append(row);
        }
    } else {
        onAddRule();
    }

    adjustAdaptiveSize();
}

void SmartFolderDialog::adjustAdaptiveSize() {
    if (!layout()) return;
    layout()->activate();

    QScreen* targetScreen = window()->screen();
    if (!targetScreen && parentWidget()) {
        targetScreen = parentWidget()->window()->screen();
    }
    if (!targetScreen) {
        targetScreen = QGuiApplication::primaryScreen();
    }

    int maxScreenH = targetScreen ? static_cast<int>(targetScreen->availableGeometry().height() * 0.8) : 800;

    // 测量除了滚动区以外的固定控件高度（顶层布局在 600 宽度下的高度 - 滚动区当前高度）
    int layoutH = layout()->heightForWidth(600);
    int fixedH = layoutH - m_scrollArea->height();

    // 1. 假设无需滚动条，计算可用规则列表宽度
    // 600 (对话框宽度) - 20*2 (getContentArea 边距)
    int availRulesWidthNoScroll = 600 - 40;

    QWidget* scrollContent = m_scrollArea->widget();
    int scrollContentH = 0;
    if (scrollContent && scrollContent->layout()) {
        scrollContentH = scrollContent->layout()->heightForWidth(availRulesWidthNoScroll);
    }

    int totalDesiredH = fixedH + scrollContentH;

    // 2. 如果总高度超过屏幕 80%，必将产生垂直滚动条，此时扣除滚动条宽度重新计算折行高度
    if (totalDesiredH > maxScreenH) {
        int scrollBarW = m_scrollArea->verticalScrollBar()->sizeHint().width();
        if (scrollBarW <= 0) scrollBarW = 12;
        int availRulesWidthWithScroll = availRulesWidthNoScroll - scrollBarW;

        if (scrollContent && scrollContent->layout()) {
            scrollContentH = scrollContent->layout()->heightForWidth(availRulesWidthWithScroll);
        }
        totalDesiredH = fixedH + scrollContentH;
    }

    int finalH = qMin(totalDesiredH, maxScreenH);
    finalH = qMax(420, finalH);

    resize(600, finalH);
}

bool SmartFolderDialog::event(QEvent* event) {
    bool result = FramelessDialog::event(event);
    if (event->type() == QEvent::LayoutRequest) {
        adjustAdaptiveSize();
    }
    return result;
}

void SmartFolderDialog::schedulePreview() {
    if (!m_previewTimer) {
        m_previewTimer = new QTimer(this);
        m_previewTimer->setSingleShot(true);
        connect(m_previewTimer, &QTimer::timeout, this, [this]() {
            emit rulesPreviewRequested(getRecord());
        });
    }
    m_previewTimer->start(150);
}

void SmartFolderDialog::onAddRule() {
    QWidget* scrollContent = m_rulesLayout->parentWidget();
    SmartFolderRuleRow* row = new SmartFolderRuleRow(scrollContent);
    connect(row, &SmartFolderRuleRow::removeRequested, this, &SmartFolderDialog::onRemoveRule);
    connect(row, &SmartFolderRuleRow::rowHeightChanged, this, &SmartFolderDialog::adjustAdaptiveSize);
    connect(row, &SmartFolderRuleRow::ruleChanged, this, &SmartFolderDialog::schedulePreview);
    m_rulesLayout->insertWidget(m_rulesLayout->count() - 1, row);
    m_ruleRows.append(row);
    adjustAdaptiveSize();
    schedulePreview();
}

void SmartFolderDialog::onRemoveRule(SmartFolderRuleRow* row) {
    if (!row) return;
    m_ruleRows.removeOne(row);
    m_rulesLayout->removeWidget(row);
    row->deleteLater();
    adjustAdaptiveSize();
    schedulePreview();
}

void SmartFolderDialog::onSaveAndAccept() {
    m_record.name = m_nameEdit->text().trimmed();
    if (m_record.name.isEmpty()) {
        m_record.name = "新智能文件夹";
    }

    m_record.matchMode = m_matchModeCombo->currentData().toString();
    m_record.rules.clear();

    for (SmartFolderRuleRow* row : m_ruleRows) {
        if (row) {
            m_record.rules.append(row->getRule());
        }
    }

    // 将整套规则模板序列化持久化至 AppConfig
    AppConfig::instance().setValue("SmartFolder/LastMatchMode", m_record.matchMode);
    QJsonArray rulesArr;
    for (const auto& rule : m_record.rules) {
        rulesArr.append(rule.toJson());
    }
    QJsonDocument doc(rulesArr);
    AppConfig::instance().setValue("SmartFolder/LastTemplateRules", QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));

    accept();
}

SmartFolderRecord SmartFolderDialog::getRecord() const {
    return m_record;
}

} // namespace QuarkMeta
