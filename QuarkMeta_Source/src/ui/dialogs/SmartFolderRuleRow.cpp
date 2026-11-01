#include "SmartFolderRuleRow.h"
#include "../UiHelper.h"

namespace QuarkMeta {

SmartFolderRuleRow::SmartFolderRuleRow(QWidget* parent) : QWidget(parent) {
    initUi();
}

SmartFolderRuleRow::SmartFolderRuleRow(const SmartFolderRule& rule, QWidget* parent) : QWidget(parent) {
    initUi();
    setRule(rule);
}

void SmartFolderRuleRow::initUi() {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 2, 0, 2);
    m_layout->setSpacing(8);

    m_typeCombo = new QComboBox(this);
    m_typeCombo->setObjectName("SmartFolderComboBox");
    m_typeCombo->addItem("评分", static_cast<int>(SmartRuleType::Rating));
    m_typeCombo->addItem("名称", static_cast<int>(SmartRuleType::Name));
    m_typeCombo->addItem("扩展名", static_cast<int>(SmartRuleType::Extension));
    m_typeCombo->addItem("标签", static_cast<int>(SmartRuleType::Tag));
    m_typeCombo->addItem("文件夹", static_cast<int>(SmartRuleType::Folder));
    m_typeCombo->addItem("文件夹名", static_cast<int>(SmartRuleType::FolderName));
    m_typeCombo->addItem("颜色标记", static_cast<int>(SmartRuleType::Color));
    m_typeCombo->addItem("图像颜色", static_cast<int>(SmartRuleType::AutoColor));
    m_typeCombo->addItem("添加日期", static_cast<int>(SmartRuleType::AddedDate));
    m_typeCombo->addItem("修改日期", static_cast<int>(SmartRuleType::ModifiedDate));
    m_typeCombo->addItem("创建日期", static_cast<int>(SmartRuleType::CreatedDate));
    m_typeCombo->addItem("链接", static_cast<int>(SmartRuleType::Link));
    m_typeCombo->addItem("注释", static_cast<int>(SmartRuleType::Note));
    m_typeCombo->addItem("宽度", static_cast<int>(SmartRuleType::Width));
    m_typeCombo->addItem("高度", static_cast<int>(SmartRuleType::Height));
    m_typeCombo->addItem("文件大小", static_cast<int>(SmartRuleType::FileSize));

    m_opCombo = new QComboBox(this);
    m_opCombo->setObjectName("SmartFolderComboBox");

    m_btnDelete = new QPushButton(this);
    m_btnDelete->setFixedSize(24, 24);
    m_btnDelete->setIcon(UiHelper::getIcon("close", QColor("#CCCCCC"), 14));
    m_btnDelete->setObjectName("SmartFolderRowDeleteBtn");

    m_layout->addWidget(m_typeCombo);
    m_layout->addWidget(m_opCombo);

    m_layout->addWidget(m_btnDelete);

    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        onTypeChanged();
        emit ruleChanged();
    });
    connect(m_opCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        onOpChanged();
        emit ruleChanged();
    });
    connect(m_btnDelete, &QPushButton::clicked, this, [this]() {
        emit removeRequested(this);
    });

    onTypeChanged();
}

void SmartFolderRuleRow::onTypeChanged() {
    SmartRuleType type = static_cast<SmartRuleType>(m_typeCombo->currentData().toInt());

    // 切换类型时重置当前规则缓存数据，防止上个类型的残留数值带入新控件
    m_currentRule = SmartFolderRule();
    m_currentRule.type = type;

    m_opCombo->blockSignals(true);
    m_opCombo->clear();

    switch (type) {
    case SmartRuleType::Rating:
        m_opCombo->addItem("大于等于", static_cast<int>(SmartRuleOperator::GreaterOrEqual));
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("小于等于", static_cast<int>(SmartRuleOperator::LessOrEqual));
        break;

    case SmartRuleType::Extension:
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        break;

    case SmartRuleType::Folder:
        m_opCombo->addItem("是文件夹", static_cast<int>(SmartRuleOperator::IsFolder));
        m_opCombo->addItem("不是文件夹", static_cast<int>(SmartRuleOperator::IsNotFolder));
        break;

    case SmartRuleType::Color:
    case SmartRuleType::AutoColor:
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("不包含", static_cast<int>(SmartRuleOperator::NotContains));
        break;

    case SmartRuleType::Tag:
        m_opCombo->addItem("包含", static_cast<int>(SmartRuleOperator::Contains));
        m_opCombo->addItem("不包含", static_cast<int>(SmartRuleOperator::NotContains));
        break;

    case SmartRuleType::Name:
    case SmartRuleType::FolderName:
    case SmartRuleType::Link:
    case SmartRuleType::Note:
        m_opCombo->addItem("包含", static_cast<int>(SmartRuleOperator::Contains));
        m_opCombo->addItem("不包含", static_cast<int>(SmartRuleOperator::NotContains));
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("开头是", static_cast<int>(SmartRuleOperator::StartsWith));
        m_opCombo->addItem("结尾是", static_cast<int>(SmartRuleOperator::EndsWith));
        m_opCombo->addItem("为空", static_cast<int>(SmartRuleOperator::IsEmpty));
        m_opCombo->addItem("不为空", static_cast<int>(SmartRuleOperator::IsNotEmpty));
        break;

    case SmartRuleType::AddedDate:
    case SmartRuleType::ModifiedDate:
    case SmartRuleType::CreatedDate:
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("早于", static_cast<int>(SmartRuleOperator::Before));
        m_opCombo->addItem("晚于", static_cast<int>(SmartRuleOperator::After));
        m_opCombo->addItem("介于", static_cast<int>(SmartRuleOperator::Between));
        m_opCombo->addItem("最近 N 天", static_cast<int>(SmartRuleOperator::RecentDays));
        break;

    case SmartRuleType::Width:
    case SmartRuleType::Height:
    case SmartRuleType::FileSize:
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("大于等于", static_cast<int>(SmartRuleOperator::GreaterOrEqual));
        m_opCombo->addItem("小于等于", static_cast<int>(SmartRuleOperator::LessOrEqual));
        m_opCombo->addItem("介于", static_cast<int>(SmartRuleOperator::Between));
        break;
    }

    m_opCombo->blockSignals(false);
    updateValueWidget();
}

void SmartFolderRuleRow::onOpChanged() {
    updateValueWidget();
}

void SmartFolderRuleRow::updateValueWidget() {
    SmartRuleType type = static_cast<SmartRuleType>(m_typeCombo->currentData().toInt());
    SmartRuleOperator op = static_cast<SmartRuleOperator>(m_opCombo->currentData().toInt());

    // Save current values if m_valueWidget is valid
    if (m_valueWidget) {
        m_valueWidget->getRuleValues(m_currentRule);
        m_layout->removeWidget(m_valueWidget);
        m_valueWidget->hide();
        m_valueWidget->setParent(nullptr);
        m_valueWidget->deleteLater();
        m_valueWidget = nullptr;
    }

    ISmartFolderValueWidget* newWidget = nullptr;

    if (op == SmartRuleOperator::IsEmpty || op == SmartRuleOperator::IsNotEmpty ||
        op == SmartRuleOperator::IsFolder || op == SmartRuleOperator::IsNotFolder) {
        newWidget = new DummyValueWidget(this);
    } else if (type == SmartRuleType::Rating) {
        newWidget = new StarRatingValueWidget(this);
    } else if (type == SmartRuleType::Color) {
        newWidget = new ManualColorValueWidget(this);
    } else if (type == SmartRuleType::AutoColor) {
        newWidget = new AutoColorValueWidget(this);
    } else if (type == SmartRuleType::Tag) {
        newWidget = new TagPillValueWidget(this);
    } else if (type == SmartRuleType::AddedDate || type == SmartRuleType::ModifiedDate || type == SmartRuleType::CreatedDate) {
        if (op == SmartRuleOperator::Between) {
            newWidget = new DateRangeValueWidget(this);
        } else if (op == SmartRuleOperator::RecentDays) {
            newWidget = new RecentDaysValueWidget(this);
        } else {
            newWidget = new SingleDateValueWidget(this);
        }
    } else if (type == SmartRuleType::Width || type == SmartRuleType::Height) {
        QString suffix = "px";
        if (op == SmartRuleOperator::Between) {
            newWidget = new NumericRangeValueWidget(suffix, this);
        } else {
            newWidget = new NumericValueWidget(suffix, this);
        }
    } else if (type == SmartRuleType::FileSize) {
        if (op == SmartRuleOperator::Between) {
            newWidget = new FileSizeRangeValueWidget(this);
        } else {
            newWidget = new FileSizeValueWidget(this);
        }
    } else {
        newWidget = new TextValueWidget(this);
    }

    m_valueWidget = newWidget;
    m_valueWidget->setRuleValues(m_currentRule);
    m_layout->insertWidget(2, m_valueWidget, 1);

    connect(m_valueWidget, &ISmartFolderValueWidget::valueChanged, this, [this, type]() {
        // 如果是评分规则选了 0 星，锁定运算符为 Equals
        if (type == SmartRuleType::Rating) {
            SmartFolderRule testRule;
            m_valueWidget->getRuleValues(testRule);
            if (testRule.value.toInt() == 0) {
                int eqIdx = m_opCombo->findData(static_cast<int>(SmartRuleOperator::Equals));
                if (eqIdx >= 0 && m_opCombo->currentIndex() != eqIdx) {
                    m_opCombo->setCurrentIndex(eqIdx);
                }
            }
        }
        updateGeometry();
        emit rowHeightChanged();
        emit ruleChanged();
    });

    updateGeometry();
    emit rowHeightChanged();
}

SmartFolderRule SmartFolderRuleRow::getRule() const {
    SmartFolderRule rule = m_currentRule;
    rule.type = static_cast<SmartRuleType>(m_typeCombo->currentData().toInt());
    rule.op = static_cast<SmartRuleOperator>(m_opCombo->currentData().toInt());

    if (m_valueWidget) {
        m_valueWidget->getRuleValues(rule);
    }
    return rule;
}

void SmartFolderRuleRow::setRule(const SmartFolderRule& rule) {
    m_currentRule = rule;

    m_typeCombo->blockSignals(true);
    int typeIdx = m_typeCombo->findData(static_cast<int>(rule.type));
    if (typeIdx >= 0) {
        m_typeCombo->setCurrentIndex(typeIdx);
    }
    m_typeCombo->blockSignals(false);

    // repopulates m_opCombo without wiping m_currentRule
    SmartRuleType type = rule.type;
    m_opCombo->blockSignals(true);
    m_opCombo->clear();

    switch (type) {
    case SmartRuleType::Rating:
        m_opCombo->addItem("大于等于", static_cast<int>(SmartRuleOperator::GreaterOrEqual));
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("小于等于", static_cast<int>(SmartRuleOperator::LessOrEqual));
        break;

    case SmartRuleType::Extension:
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        break;

    case SmartRuleType::Folder:
        m_opCombo->addItem("是文件夹", static_cast<int>(SmartRuleOperator::IsFolder));
        m_opCombo->addItem("不是文件夹", static_cast<int>(SmartRuleOperator::IsNotFolder));
        break;

    case SmartRuleType::Color:
    case SmartRuleType::AutoColor:
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("不包含", static_cast<int>(SmartRuleOperator::NotContains));
        break;

    case SmartRuleType::Tag:
        m_opCombo->addItem("包含", static_cast<int>(SmartRuleOperator::Contains));
        m_opCombo->addItem("不包含", static_cast<int>(SmartRuleOperator::NotContains));
        break;

    case SmartRuleType::Name:
    case SmartRuleType::FolderName:
    case SmartRuleType::Link:
    case SmartRuleType::Note:
        m_opCombo->addItem("包含", static_cast<int>(SmartRuleOperator::Contains));
        m_opCombo->addItem("不包含", static_cast<int>(SmartRuleOperator::NotContains));
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("开头是", static_cast<int>(SmartRuleOperator::StartsWith));
        m_opCombo->addItem("结尾是", static_cast<int>(SmartRuleOperator::EndsWith));
        m_opCombo->addItem("为空", static_cast<int>(SmartRuleOperator::IsEmpty));
        m_opCombo->addItem("不为空", static_cast<int>(SmartRuleOperator::IsNotEmpty));
        break;

    case SmartRuleType::AddedDate:
    case SmartRuleType::ModifiedDate:
    case SmartRuleType::CreatedDate:
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("早于", static_cast<int>(SmartRuleOperator::Before));
        m_opCombo->addItem("晚于", static_cast<int>(SmartRuleOperator::After));
        m_opCombo->addItem("介于", static_cast<int>(SmartRuleOperator::Between));
        m_opCombo->addItem("最近 N 天", static_cast<int>(SmartRuleOperator::RecentDays));
        break;

    case SmartRuleType::Width:
    case SmartRuleType::Height:
    case SmartRuleType::FileSize:
        m_opCombo->addItem("等于", static_cast<int>(SmartRuleOperator::Equals));
        m_opCombo->addItem("大于等于", static_cast<int>(SmartRuleOperator::GreaterOrEqual));
        m_opCombo->addItem("小于等于", static_cast<int>(SmartRuleOperator::LessOrEqual));
        m_opCombo->addItem("介于", static_cast<int>(SmartRuleOperator::Between));
        break;
    }

    int opIdx = m_opCombo->findData(static_cast<int>(rule.op));
    if (opIdx >= 0) {
        m_opCombo->setCurrentIndex(opIdx);
    }
    m_opCombo->blockSignals(false);

    updateValueWidget();
    if (m_valueWidget) {
        m_valueWidget->setRuleValues(rule);
    }
}

} // namespace QuarkMeta
