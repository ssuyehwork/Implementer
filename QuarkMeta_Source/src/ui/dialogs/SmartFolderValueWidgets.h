#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QSlider>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QDate>
#include "../../meta/SmartFolderDao.h"

namespace QuarkMeta {

class TagPillContainer;

class ISmartFolderValueWidget : public QWidget {
    Q_OBJECT
public:
    explicit ISmartFolderValueWidget(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual ~ISmartFolderValueWidget() = default;

    virtual void getRuleValues(SmartFolderRule& rule) const = 0;
    virtual void setRuleValues(const SmartFolderRule& rule) = 0;

signals:
    void valueChanged();
};

class TextValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit TextValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QLineEdit* m_edit = nullptr;
};

class StarRatingValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit StarRatingValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QComboBox* m_combo = nullptr;
};

class SingleDateValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit SingleDateValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QDateEdit* m_dateEdit = nullptr;
};

class DateRangeValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit DateRangeValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QDateEdit* m_date1Edit = nullptr;
    QDateEdit* m_date2Edit = nullptr;
};

class RecentDaysValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit RecentDaysValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QSpinBox* m_spinBox = nullptr;
};

class NumericValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit NumericValueWidget(const QString& suffix = QString(), QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QSpinBox* m_spinBox = nullptr;
};

class NumericRangeValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit NumericRangeValueWidget(const QString& suffix = QString(), QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QSpinBox* m_spin1 = nullptr;
    QSpinBox* m_spin2 = nullptr;
};

class FileSizeValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit FileSizeValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QSpinBox* m_spin = nullptr;
    QComboBox* m_unitCombo = nullptr;
};

class FileSizeRangeValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit FileSizeRangeValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QSpinBox* m_spin1 = nullptr;
    QSpinBox* m_spin2 = nullptr;
    QComboBox* m_unitCombo = nullptr;
};

class ManualColorValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit ManualColorValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    QComboBox* m_combo = nullptr;
};

class AutoColorValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit AutoColorValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    void updateBtnStyle();
    void openColorWheelPopup();

    QPushButton* m_colorBtn = nullptr;
    QSpinBox* m_toleranceSpin = nullptr;
    QString m_colorHex = "#FF0000";
};

class TagPillValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit TagPillValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;

private:
    TagPillContainer* m_tagPillContainer = nullptr;
};

class DummyValueWidget : public ISmartFolderValueWidget {
    Q_OBJECT
public:
    explicit DummyValueWidget(QWidget* parent = nullptr);
    void getRuleValues(SmartFolderRule& rule) const override;
    void setRuleValues(const SmartFolderRule& rule) override;
};

} // namespace QuarkMeta
