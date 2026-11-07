#include "BatchRenameDialog.h"
#include "RuleRow.h"
#include "UiHelper.h"
#include "PresetManager.h"
#include "UndoToastOverlay.h"
#include "ShellIconManager.h"
#include "../util/DiskMediaExtractor.h"
#include "../core/BatchRenameService.h"
#include "../meta/MetadataManager.h"
#include <QHeaderView>
#include <QPainterPath>
#include "FramelessFileDialog.h"
#include "FramelessDialog.h"
#include "dialogs/FramelessInputDialog.h"
#include "ToolTipOverlay.h"
#include <QSplitter>
#include <QFileInfo>
#include <QTimer>
#include <QDir>
#include <QLabel>
#include <QFile>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QRadioButton>
#include <QScrollArea>
#include "../core/AppConfig.h"
#include "../core/UndoManager.h"
#include "../core/BasicCommands.h"
#include "../core/OperationSnapshotEngine.h"

namespace QuarkMeta {

BatchRenameDialog::BatchRenameDialog(const std::vector<std::wstring>& originalPaths, QWidget* parent)
    : FramelessDialog("批量重命名 - QuarkMeta", parent), m_originalPaths(originalPaths) {
    setMinimumSize(900, 500);
    resize(1220, 650);
    initContent();
    applyTheme();

    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setSingleShot(true);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &BatchRenameDialog::doAutoSave);

    initTableItems();

    m_isInitializing = true;
    loadPresetsFromConfig();

    QString lastRules = AppConfig::instance().getValue("LastBatchRenameRules").toString();
    if (!lastRules.isEmpty()) {
        applyRulesJson(lastRules);
    }

    if (m_ruleRows.isEmpty()) {
        onAddRow();
    }
    m_isInitializing = false;

    updatePreview();
}

void BatchRenameDialog::initContent() {
    // 隐藏 Pin, Min, Max，仅保留 Close 按钮
    setVisibleButtons(Close);

    // 导入/导出按钮嵌入标题栏 (标注③)
    m_btnImportPreset = new QPushButton("导入...", this);
    m_btnExportPreset = new QPushButton("导出...", this);
    m_btnImportPreset->setObjectName("BatchRenameSecBtn");
    m_btnExportPreset->setObjectName("BatchRenameSecBtn");
    m_btnImportPreset->setFixedSize(56, 22);
    m_btnExportPreset->setFixedSize(56, 22);
    m_btnImportPreset->setCursor(Qt::PointingHandCursor);
    m_btnExportPreset->setCursor(Qt::PointingHandCursor);

    if (m_titleLayout && m_closeBtn) {
        int closeIdx = m_titleLayout->indexOf(m_closeBtn);
        if (closeIdx >= 0) {
            m_titleLayout->insertWidget(closeIdx, m_btnImportPreset);
            m_titleLayout->insertWidget(closeIdx + 1, m_btnExportPreset);
        } else {
            m_titleLayout->addWidget(m_btnImportPreset);
            m_titleLayout->addWidget(m_btnExportPreset);
        }
    }

    QHBoxLayout* rootL = new QHBoxLayout(m_contentArea);
    rootL->setContentsMargins(15, 15, 15, 15);
    rootL->setSpacing(0);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, m_contentArea);
    splitter->setChildrenCollapsible(false);
    splitter->setObjectName("BatchRenameSplitter");

    // ================= 左侧：配置区 =================
    QWidget* leftPanel = new QWidget(splitter);
    leftPanel->setMinimumWidth(380);
    leftPanel->setMaximumWidth(480);
    QVBoxLayout* configL = new QVBoxLayout(leftPanel);
    configL->setContentsMargins(0, 0, 10, 0);
    configL->setSpacing(15);

    // 1. 预设区 (标注④: 替换为 储存 / 删除 两个按钮)
    QGroupBox* presetGroup = new QGroupBox("预设", leftPanel);
    QHBoxLayout* presetL = new QHBoxLayout(presetGroup);
    presetL->setContentsMargins(10, 5, 10, 5);
    presetL->setSpacing(5);

    m_presetCombo = new QComboBox(presetGroup);
    m_presetCombo->addItem("默认设置");
    m_presetCombo->setFixedHeight(25);
    presetL->addWidget(m_presetCombo, 1);

    m_btnSavePreset = new QPushButton("储存", presetGroup);
    m_btnSavePreset->setObjectName("BatchRenameSecBtn");
    m_btnSavePreset->setFixedHeight(25);
    m_btnSavePreset->setFixedWidth(56);
    m_btnSavePreset->setCursor(Qt::PointingHandCursor);

    m_btnDeletePreset = new QPushButton("删除", presetGroup);
    m_btnDeletePreset->setObjectName("BatchRenameSecBtn");
    m_btnDeletePreset->setFixedHeight(25);
    m_btnDeletePreset->setFixedWidth(56);
    m_btnDeletePreset->setCursor(Qt::PointingHandCursor);
    m_btnDeletePreset->setEnabled(false);

    presetL->addWidget(m_btnSavePreset);
    presetL->addWidget(m_btnDeletePreset);
    configL->addWidget(presetGroup);

    // 2. 目标文件夹
    QGroupBox* targetGroup = new QGroupBox("目标文件夹", leftPanel);
    QVBoxLayout* targetL = new QVBoxLayout(targetGroup);
    m_rbRename = new QRadioButton("在同一文件夹中重命名", targetGroup);
    m_rbMove = new QRadioButton("移动到其他文件夹", targetGroup);
    m_rbCopy = new QRadioButton("复制到其他文件夹", targetGroup);
    m_rbRename->setChecked(true);
    targetL->addWidget(m_rbRename);
    targetL->addWidget(m_rbMove);
    targetL->addWidget(m_rbCopy);

    QHBoxLayout* pathL = new QHBoxLayout();
    m_targetPathEdit = new QLineEdit(targetGroup);
    m_targetPathEdit->setPlaceholderText("选择目标文件夹...");
    m_targetPathEdit->setFixedHeight(25);
    m_targetPathEdit->setEnabled(false);
    m_btnBrowse = new QPushButton("浏览...", targetGroup);
    m_btnBrowse->setObjectName("BatchRenameSecBtn");
    m_btnBrowse->setFixedSize(80, 25);
    m_btnBrowse->setEnabled(false);
    pathL->addWidget(m_targetPathEdit);
    pathL->addWidget(m_btnBrowse);
    targetL->addLayout(pathL);

    configL->addWidget(targetGroup);

    // 3. 新文件名 (规则构造器)
    QGroupBox* rulesGroup = new QGroupBox("新文件名", leftPanel);
    rulesGroup->setObjectName("BatchRenameRulesGroup");

    QVBoxLayout* rulesGroupL = new QVBoxLayout(rulesGroup);
    rulesGroupL->setContentsMargins(4, 2, 4, 4);
    rulesGroupL->setSpacing(0);

    QScrollArea* scroll = new QScrollArea(rulesGroup);
    scroll->setWidgetResizable(true);
    scroll->setAlignment(Qt::AlignTop);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("BatchRenameScroll");

    m_rulesContainer = new QWidget(scroll);
    m_rulesContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    m_rulesLayout = new QVBoxLayout(m_rulesContainer);
    m_rulesLayout->setContentsMargins(0, 0, 0, 0);
    m_rulesLayout->setSpacing(2);

    scroll->setWidget(m_rulesContainer);
    rulesGroupL->addWidget(scroll);
    configL->addWidget(rulesGroup, 1);

    // 4. 左侧底部主执行按钮
    QHBoxLayout* execBtnLayout = new QHBoxLayout();
    m_btnExecute = new QPushButton("重命名", leftPanel);
    m_btnExecute->setFixedSize(120, 36);
    m_btnExecute->setCursor(Qt::PointingHandCursor);
    m_btnExecute->setObjectName("BatchRenameExecuteBtn");
    execBtnLayout->addStretch();
    execBtnLayout->addWidget(m_btnExecute);
    execBtnLayout->addStretch();

    configL->addLayout(execBtnLayout);

    // ================= 右侧：实时预览面板 (自适应弹性充满) =================
    m_table = new QTableWidget(splitter);
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels({"当前文件名", "重命名后名称"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(28);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_table->setIconSize(QSize(20, 20));
    m_table->setFocusPolicy(Qt::NoFocus);
    m_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    splitter->addWidget(leftPanel);
    splitter->addWidget(m_table);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({420, 780});

    rootL->addWidget(splitter);

    // Connections
    connect(m_rbRename, &QRadioButton::toggled, [this](bool checked){
        m_targetPathEdit->setEnabled(!checked);
        m_btnBrowse->setEnabled(!checked);
    });
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BatchRenameDialog::onPresetComboChanged);
    connect(m_btnBrowse, &QPushButton::clicked, this, &BatchRenameDialog::onBrowseTarget);
    connect(m_btnExecute, &QPushButton::clicked, this, &BatchRenameDialog::onExecute);
    connect(m_btnImportPreset, &QPushButton::clicked, this, &BatchRenameDialog::onImportPreset);
    connect(m_btnExportPreset, &QPushButton::clicked, this, &BatchRenameDialog::onExportPreset);
    connect(m_btnSavePreset, &QPushButton::clicked, this, &BatchRenameDialog::onSavePreset);
    connect(m_btnDeletePreset, &QPushButton::clicked, this, &BatchRenameDialog::onDeleteCurrentPreset);
}

void BatchRenameDialog::applyTheme() {
    // Style handled in style.qss
}

void BatchRenameDialog::onAddRow(RuleRow* targetRow) {
    RuleRow* row = new RuleRow(m_rulesContainer);

    int insertIndex = -1;
    if (targetRow) {
        insertIndex = m_ruleRows.indexOf(targetRow);
    }

    if (insertIndex >= 0 && insertIndex < m_ruleRows.size()) {
        m_rulesLayout->insertWidget(insertIndex + 1, row);
        m_ruleRows.insert(insertIndex + 1, row);
    } else {
        m_rulesLayout->addWidget(row);
        m_ruleRows.append(row);
    }

    connect(row, &RuleRow::changed, this, &BatchRenameDialog::updatePreview);
    connect(row, &RuleRow::changed, this, &BatchRenameDialog::scheduleAutoSave);
    connect(row, &RuleRow::addRequested, this, [this, row]() {
        onAddRow(row);
    });
    connect(row, &RuleRow::addRequested, this, &BatchRenameDialog::scheduleAutoSave);
    connect(row, &RuleRow::removeRequested, [this, row]() {
        if (m_ruleRows.size() > 1) {
            m_ruleRows.removeOne(row);
            row->deleteLater();
            updatePreview();
            scheduleAutoSave();
        }
    });

    updatePreview();
}

void BatchRenameDialog::initTableItems() {
    m_table->setRowCount(static_cast<int>(m_originalPaths.size()));
    for (int i = 0; i < static_cast<int>(m_originalPaths.size()); ++i) {
        QString oldPath = QString::fromStdWString(m_originalPaths[static_cast<size_t>(i)]);
        QFileInfo info(oldPath);

        // 🚀 1:1 标准正方形中心裁切 (Center Crop) + 3px 微圆角，消除上下/左右黑边与留白
        QIcon fileIcon;
        QString thumbPath = DiskMediaExtractor::getDiskThumbCachePath(oldPath);
        if (QFile::exists(thumbPath)) {
            QPixmap pix(thumbPath);
            if (!pix.isNull() && pix.width() > 0 && pix.height() > 0) {
                int minSide = qMin(pix.width(), pix.height());
                QRect cropRect((pix.width() - minSide) / 2, (pix.height() - minSide) / 2, minSide, minSide);
                QPixmap cropped = pix.copy(cropRect);

                QPixmap target(48, 48);
                target.fill(Qt::transparent);
                QPainter painter(&target);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
                QPainterPath clipPath;
                clipPath.addRoundedRect(QRectF(0, 0, 48, 48), 6.0, 6.0);
                painter.setClipPath(clipPath);
                painter.drawPixmap(QRect(0, 0, 48, 48), cropped);
                painter.end();

                fileIcon = QIcon(target);
            }
        }
        if (fileIcon.isNull()) {
            fileIcon = ShellIconManager::getFileIconFast(oldPath, info.isDir(), info.suffix().toLower());
        }

        auto* itemOld = new QTableWidgetItem(fileIcon, info.fileName());
        itemOld->setForeground(QColor("#B0B0B0"));
        m_table->setItem(i, 0, itemOld);

        auto* itemNew = new QTableWidgetItem();
        itemNew->setForeground(QColor("#2ecc71"));
        m_table->setItem(i, 1, itemNew);
    }
}

void BatchRenameDialog::updatePreview() {
    if (m_isInitializing) return; // 初始化加载规则期间直接拦截，不跑无用计算

    std::vector<RenameRule> rules;
    for (auto* row : m_ruleRows) {
        if (row) rules.push_back(row->getRule());
    }

    auto newNames = BatchRenameService::instance().computePreview(m_originalPaths, rules);
    int total = static_cast<int>(newNames.size());

    if (!newNames.empty()) {
        m_firstNewName = QString::fromStdWString(newNames[0]);
    }

    // 仅更新第 1 列的新名称文本，耗时 < 0.1ms，打字极度丝滑！
    for (int i = 0; i < total; ++i) {
        QTableWidgetItem* itemNew = m_table->item(i, 1);
        if (itemNew) {
            itemNew->setText(QString::fromStdWString(newNames[static_cast<size_t>(i)]));
        }
    }
}

void BatchRenameDialog::onImportPreset() {
    QString path = FramelessFileDialog::getOpenFileName(this, "导入重命名预设", "", "JSON Files (*.json)");
    if (path.isEmpty()) return;

    int unrecognizedCount = 0;
    auto rules = PresetManager::importFromFile(path, &unrecognizedCount);
    if (!rules.empty() || unrecognizedCount > 0) {
        // 清空现有规则
        while (m_ruleRows.size() > 0) {
            auto* row = m_ruleRows.takeAt(0);
            row->deleteLater();
        }

        for (const auto& rule : rules) {
            onAddRow();
            m_ruleRows.last()->setRule(rule);
        }
        if (m_ruleRows.isEmpty()) {
            onAddRow();
        }
        if (unrecognizedCount > 0) {
            ToolTipOverlay::instance()->showToast(QString("有 %1 条规则无法识别，已跳过").arg(unrecognizedCount), 2000, QColor("#e81123"), this);
        }
        scheduleAutoSave();
        updatePreview();
    }
}

void BatchRenameDialog::onExportPreset() {
    QString path = FramelessFileDialog::getSaveFileName(this, "导出重命名预设", "Preset.json", "JSON Files (*.json)");
    if (path.isEmpty()) return;

    std::vector<RenameRule> rules;
    for (auto* row : m_ruleRows) {
        rules.push_back(row->getRule());
    }

    if (!PresetManager::exportToFile(path, rules)) {
        ToolTipOverlay::instance()->showToast("导出失败", 2000, QColor("#e81123"), this);
    }
}

void BatchRenameDialog::loadPresetsFromConfig() {
    QSignalBlocker blocker(m_presetCombo);
    m_presetCombo->clear();
    m_presetCombo->addItem("默认设置");

    QVariantMap presetsMap = AppConfig::instance().getValue("BatchRenamePresetsMap").toMap();
    for (auto it = presetsMap.constBegin(); it != presetsMap.constEnd(); ++it) {
        if (it.key() != "默认设置") {
            m_presetCombo->addItem(it.key());
        }
    }
}

void BatchRenameDialog::applyRulesJson(const QString& jsonStr) {
    while (!m_ruleRows.isEmpty()) {
        auto* row = m_ruleRows.takeAt(0);
        row->deleteLater();
    }

    int unrecognizedCount = 0;
    auto rules = PresetManager::deserializeRules(jsonStr, &unrecognizedCount);
    for (const auto& rule : rules) {
        onAddRow();
        m_ruleRows.last()->setRule(rule);
    }
    if (m_ruleRows.isEmpty()) {
        onAddRow();
    }
    if (unrecognizedCount > 0) {
        ToolTipOverlay::instance()->showToast(QString("有 %1 条规则无法识别，已跳过").arg(unrecognizedCount), 2000, QColor("#e81123"), this);
    }
    updatePreview();
}

void BatchRenameDialog::onPresetComboChanged(int index) {
    if (m_isInitializing) return;

    QString name = m_presetCombo->currentText();
    if (index <= 0 || name == "默认设置") {
        m_btnDeletePreset->setEnabled(false);
        QString lastRules = AppConfig::instance().getValue("LastBatchRenameRules").toString();
        if (!lastRules.isEmpty()) {
            applyRulesJson(lastRules);
        }
        return;
    }

    m_btnDeletePreset->setEnabled(true);
    QVariantMap presetsMap = AppConfig::instance().getValue("BatchRenamePresetsMap").toMap();
    if (presetsMap.contains(name)) {
        QString rulesJson = presetsMap.value(name).toString();
        applyRulesJson(rulesJson);
    }
}

void BatchRenameDialog::onSavePreset() {
    std::vector<RenameRule> rules;
    for (auto* row : m_ruleRows) {
        if (row) rules.push_back(row->getRule());
    }
    if (rules.empty()) return;

    QString defaultName = (m_presetCombo->currentIndex() > 0) ? m_presetCombo->currentText() : "";
    FramelessInputDialog dlg("保存重命名预设", "请输入预设名称:", defaultName, this);
    if (dlg.exec() == QDialog::Accepted) {
        QString name = dlg.text().trimmed();
        if (name.isEmpty() || name == "默认设置") return;

        QString compactJson = PresetManager::serializeRules(rules);
        QVariantMap map = AppConfig::instance().getValue("BatchRenamePresetsMap").toMap();
        map.insert(name, compactJson);
        AppConfig::instance().setValue("BatchRenamePresetsMap", map);

        loadPresetsFromConfig();
        int idx = m_presetCombo->findText(name);
        if (idx >= 0) {
            m_presetCombo->setCurrentIndex(idx);
        }
    }
}

void BatchRenameDialog::onDeleteCurrentPreset() {
    int index = m_presetCombo->currentIndex();
    QString name = m_presetCombo->currentText();
    if (index <= 0 || name == "默认设置") return;

    QVariantMap map = AppConfig::instance().getValue("BatchRenamePresetsMap").toMap();
    map.remove(name);
    AppConfig::instance().setValue("BatchRenamePresetsMap", map);

    loadPresetsFromConfig();
    m_presetCombo->setCurrentIndex(0);
}

void BatchRenameDialog::scheduleAutoSave() {
    if (m_autoSaveTimer) m_autoSaveTimer->start(500);
}

void BatchRenameDialog::doAutoSave() {
    std::vector<RenameRule> rules;
    for (auto* row : m_ruleRows) {
        rules.push_back(row->getRule());
    }
    QString compactJson = PresetManager::serializeRules(rules);
    AppConfig::instance().setValue("LastBatchRenameRules", compactJson);
}

void BatchRenameDialog::onBrowseTarget() {
    QString dir = FramelessFileDialog::getExistingDirectory(this, "选择目标文件夹");
    if (!dir.isEmpty()) {
        m_targetPathEdit->setText(dir);
    }
}

void BatchRenameDialog::onExecute() {
    std::vector<RenameRule> rules;
    for (auto* row : m_ruleRows) rules.push_back(row->getRule());

    auto newNames = BatchRenameService::instance().computePreview(m_originalPaths, rules);
    if (newNames.empty()) return;

    m_btnExecute->setEnabled(false);

    DiskOperationMode mode = DiskOperationMode::Rename;
    if (m_rbMove->isChecked()) mode = DiskOperationMode::Move;
    else if (m_rbCopy->isChecked()) mode = DiskOperationMode::Copy;

    QString targetDir = m_targetPathEdit->text();
    if (mode != DiskOperationMode::Rename && targetDir.isEmpty()) {
        FramelessMessageBox::warning(this, "错误", "请先选择目标文件夹");
        m_btnExecute->setEnabled(true);
        return;
    }

    QPointer<BatchRenameDialog> safeThis(this);

    BatchRenameService::instance().executeAsync(
        m_originalPaths,
        newNames,
        mode,
        targetDir,
        this->parentWidget(),
        [safeThis](int successCount) {
            if (!safeThis) return;
            safeThis->m_btnExecute->setEnabled(true);

            if (successCount > 0) {
                for (auto* row : safeThis->m_ruleRows) {
                    RenameRule rule = row->getRule();
                    if (rule.type == RenameComponentType::Sequence) {
                        rule.start = rule.start + successCount * rule.step;
                        row->setRule(rule);
                    }
                }
                safeThis->doAutoSave();
            }
            safeThis->accept();
        }
    );
}

} // namespace QuarkMeta
