#pragma once

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include "../../meta/SmartFolderDao.h"
#include "SmartFolderValueWidgets.h"

namespace QuarkMeta {

class SmartFolderRuleRow : public QWidget {
    Q_OBJECT
public:
    explicit SmartFolderRuleRow(QWidget* parent = nullptr);
    explicit SmartFolderRuleRow(const SmartFolderRule& rule, QWidget* parent = nullptr);
    ~SmartFolderRuleRow() override = default;

    SmartFolderRule getRule() const;
    void setRule(const SmartFolderRule& rule);

signals:
    void removeRequested(SmartFolderRuleRow* row);
    void rowHeightChanged();
    void ruleChanged();

private slots:
    void onTypeChanged();
    void onOpChanged();

private:
    void initUi();
    void updateValueWidget();

    QComboBox* m_typeCombo = nullptr;
    QComboBox* m_opCombo = nullptr;
    ISmartFolderValueWidget* m_valueWidget = nullptr;
    QPushButton* m_btnDelete = nullptr;
    QHBoxLayout* m_layout = nullptr;
    SmartFolderRule m_currentRule;
};

} // namespace QuarkMeta
