#pragma once

#include "FramelessDialog.h"
#include <vector>
#include <string>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QComboBox>
#include <QVBoxLayout>

namespace QuarkMeta {

class RuleRow;

class BatchRenameDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit BatchRenameDialog(const std::vector<std::wstring>& originalPaths, QWidget* parent = nullptr);
    ~BatchRenameDialog() override = default;

    // 🚀【核心接口】：返回重命名后第 1 个新文件名，供内容精准锁定高亮
    QString getFirstNewName() const { return m_firstNewName; }

private slots:
    void onAddRow(RuleRow* targetRow = nullptr);
    void updatePreview();
    void onExecute();
    void onBrowseTarget();
    void onImportPreset();
    void onExportPreset();
    void onSavePreset();
    void onDeleteCurrentPreset();
    void onPresetComboChanged(int index);
    void scheduleAutoSave();
    void doAutoSave();

private:
    void initContent();
    void applyTheme();
    void initTableItems();
    void loadPresetsFromConfig();
    void applyRulesJson(const QString& jsonStr);

    std::vector<std::wstring> m_originalPaths;
    QList<RuleRow*> m_ruleRows;
    QString m_firstNewName;

    QTableWidget* m_table = nullptr;
    QComboBox* m_presetCombo = nullptr;
    QPushButton* m_btnSavePreset = nullptr;
    QPushButton* m_btnDeletePreset = nullptr;
    QPushButton* m_btnImportPreset = nullptr;
    QPushButton* m_btnExportPreset = nullptr;
    QRadioButton* m_rbRename = nullptr;
    QRadioButton* m_rbMove = nullptr;
    QRadioButton* m_rbCopy = nullptr;
    QLineEdit* m_targetPathEdit = nullptr;
    QPushButton* m_btnBrowse = nullptr;
    QPushButton* m_btnExecute = nullptr;

    QWidget* m_rulesContainer = nullptr;
    QVBoxLayout* m_rulesLayout = nullptr;
    QTimer* m_autoSaveTimer = nullptr;
    bool m_isInitializing = false;
};

} // namespace QuarkMeta