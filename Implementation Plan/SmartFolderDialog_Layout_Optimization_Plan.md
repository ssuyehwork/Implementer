# 智能文件夹规则配置对话框布局自适应与防截断优化方案

## 1. 需求与设计概要
在智能文件夹规则配置对话框（`SmartFolderDialog` / `SmartFolderRuleRow`）中，当前存在由于硬编码尺寸与不合理的布局拉伸分配导致的文字截断与界面局促问题：
1. **下拉框宽度过窄与文字截断**：
   - 运算符下拉框 `m_opCombo` 和类型下拉框 `m_typeCombo` 宽度受限，导致“是文件夹”、“符合以下任一条件 (OR)”等长文本被严重截断显示为“是文…”；
   - 解决方案：为 `m_typeCombo`（最小宽度 100px）与 `m_opCombo`（最小宽度 125px，并启用 `setSizeAdjustPolicy(QComboBox::AdjustToContents)`）设置充足舒适的自适应最小宽度，杜绝文字截断。
2. **空值控件 (DummyValueWidget) 占用冗余空间问题**：
   - 当规则为“文件夹”（操作符为“是文件夹/不是文件夹”）或“为空/不为空”等无需参数输入的条件时，`m_valueWidget` 采用 `DummyValueWidget`，但在布局中仍被分配了 `stretch = 1`，导致右侧出现无意义的巨大空框，而左侧下拉框受到压迫；
   - 解决方案：动态根据 `m_valueWidget` 是否为 `DummyValueWidget` 调整其布局拉伸系数与显隐（若为 Dummy 则不设置拉伸 `stretch = 0` 并设置固定宽度 0，让剩余空间舒展），给左侧类型与操作符下拉框更自如的布局空间。
3. **窗口整体尺寸舒展**：
   - 对话框基准宽度从狭窄局促的 `520px` 舒展扩展至 `600px`（`adjustAdaptiveSize()` 中的基准宽度同步调整为 `600px`），消除狭小压抑感，为各类复合控件留出充足的呼吸感。

---

## 2. 涉及文件清单
1. **修改：** `QuarkMeta_Source/src/ui/dialogs/SmartFolderRuleRow.cpp`
2. **修改：** `QuarkMeta_Source/src/ui/dialogs/SmartFolderDialog.cpp`
3. **修改：** `QuarkMeta_Source/resources/style.qss`

---

## 3. 详细代码变更与 SEARCH / REPLACE

### 变更 1：`QuarkMeta_Source/src/ui/dialogs/SmartFolderRuleRow.cpp`
设置下拉框自适应策略与最小宽度：
```diff
<<<<<<< SEARCH
    m_typeCombo = new QComboBox(this);
    m_typeCombo->setObjectName("SmartFolderComboBox");
    m_typeCombo->addItem("星级", static_cast<int>(SmartRuleType::Rating));
=======
    m_typeCombo = new QComboBox(this);
    m_typeCombo->setObjectName("SmartFolderComboBox");
    m_typeCombo->setMinimumWidth(100);
    m_typeCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_typeCombo->addItem("星级", static_cast<int>(SmartRuleType::Rating));
>>>>>>> REPLACE
```
以及 `m_opCombo`：
```diff
<<<<<<< SEARCH
    m_opCombo = new QComboBox(this);
    m_opCombo->setObjectName("SmartFolderComboBox");

    m_btnDelete = new QPushButton(this);
=======
    m_opCombo = new QComboBox(this);
    m_opCombo->setObjectName("SmartFolderComboBox");
    m_opCombo->setMinimumWidth(125);
    m_opCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    m_btnDelete = new QPushButton(this);
>>>>>>> REPLACE
```
在 `updateValueWidget()` 中，根据值控件类型分配 stretch 系数：
```diff
<<<<<<< SEARCH
    m_valueWidget = newWidget;
    m_valueWidget->setRuleValues(m_currentRule);
    m_layout->insertWidget(2, m_valueWidget, 1);
=======
    m_valueWidget = newWidget;
    m_valueWidget->setRuleValues(m_currentRule);
    bool isDummy = (qobject_cast<DummyValueWidget*>(m_valueWidget) != nullptr);
    m_layout->insertWidget(2, m_valueWidget, isDummy ? 0 : 1);
>>>>>>> REPLACE
```

### 变更 2：`QuarkMeta_Source/src/ui/dialogs/SmartFolderDialog.cpp`
基准宽度调整为 600：
```diff
<<<<<<< SEARCH
void SmartFolderDialog::setupUi() {
    resize(520, 420);

    QWidget* bodyWidget = getContentArea();
=======
void SmartFolderDialog::setupUi() {
    resize(600, 420);

    QWidget* bodyWidget = getContentArea();
>>>>>>> REPLACE
```
以及 `adjustAdaptiveSize()`：
```diff
<<<<<<< SEARCH
    // 测量除了滚动区以外的固定控件高度（顶层布局在 520 宽度下的高度 - 滚动区当前高度）
    int layoutH = layout()->heightForWidth(520);
    int fixedH = layoutH - m_scrollArea->height();

    // 1. 假设无需滚动条，计算可用规则列表宽度
    // 520 (对话框宽度) - 20*2 (getContentArea 边距)
    int availRulesWidthNoScroll = 520 - 40;
=======
    // 测量除了滚动区以外的固定控件高度（顶层布局在 600 宽度下的高度 - 滚动区当前高度）
    int layoutH = layout()->heightForWidth(600);
    int fixedH = layoutH - m_scrollArea->height();

    // 1. 假设无需滚动条，计算可用规则列表宽度
    // 600 (对话框宽度) - 20*2 (getContentArea 边距)
    int availRulesWidthNoScroll = 600 - 40;
>>>>>>> REPLACE
```
以及：
```diff
<<<<<<< SEARCH
    int finalH = qMin(totalDesiredH, maxScreenH);
    finalH = qMax(420, finalH);

    resize(520, finalH);
}
=======
    int finalH = qMin(totalDesiredH, maxScreenH);
    finalH = qMax(420, finalH);

    resize(600, finalH);
}
>>>>>>> REPLACE
```

### 变更 3：`QuarkMeta_Source/resources/style.qss`
优化下拉框内边距与箭头区域，防止箭头与右侧文字产生挤压遮挡：
```diff
<<<<<<< SEARCH
QComboBox#SmartFolderComboBox {
    background-color: #252526;
    color: #EEEEEE;
    border: 1px solid #333333;
    border-radius: 4px;
    padding: 2px 8px;
    min-height: 24px;
}
=======
QComboBox#SmartFolderComboBox {
    background-color: #252526;
    color: #EEEEEE;
    border: 1px solid #333333;
    border-radius: 4px;
    padding: 2px 22px 2px 8px;
    min-height: 24px;
}
>>>>>>> REPLACE
```
