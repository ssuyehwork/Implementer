#pragma once

#include "FramelessDialog.h"
#include "../../meta/SmartFolderDao.h"
#include "SmartFolderRuleRow.h"
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QEvent>
#include <QScrollArea>
#include <QTimer>

namespace QuarkMeta {

class SmartFolderDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit SmartFolderDialog(QWidget* parent = nullptr);
    explicit SmartFolderDialog(const SmartFolderRecord& record, QWidget* parent = nullptr);
    ~SmartFolderDialog() override = default;

    SmartFolderRecord getRecord() const;

signals:
    void rulesPreviewRequested(const SmartFolderRecord& record);

private slots:
    void onAddRule();
    void onRemoveRule(SmartFolderRuleRow* row);
    void onSaveAndAccept();

private:
    void setupUi();

    SmartFolderRecord m_record;

    QLineEdit* m_nameEdit{nullptr};
    QComboBox* m_matchModeCombo{nullptr};
    QVBoxLayout* m_rulesLayout{nullptr};
    QList<SmartFolderRuleRow*> m_ruleRows;
    QScrollArea* m_scrollArea{nullptr};

    QPushButton* m_btnAddRule{nullptr};
    QPushButton* m_btnCancel{nullptr};
    QPushButton* m_btnSave{nullptr};

    QTimer* m_previewTimer{nullptr};
    void schedulePreview();
    void adjustAdaptiveSize();

protected:
    bool event(QEvent* event) override;
};

} // namespace QuarkMeta
