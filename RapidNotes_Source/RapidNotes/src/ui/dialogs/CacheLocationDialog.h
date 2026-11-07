#pragma once

#include "FramelessDialog.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

namespace QuarkMeta {

class CacheLocationDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit CacheLocationDialog(QWidget* parent = nullptr);
    ~CacheLocationDialog() override = default;

private slots:
    void onBrowsePath();
    void onRestoreDefault();
    void onSaveAndAccept();

private:
    void setupUi();
    void updatePathDisplay();
    qint64 calculateDirectorySize(const QString& path);

    QLineEdit* m_pathEdit{nullptr};
    QPushButton* m_btnBrowse{nullptr};
    QLabel* m_sizeLabel{nullptr};
    QCheckBox* m_chkMigrateData{nullptr};
    QPushButton* m_btnRestore{nullptr};
    QPushButton* m_btnCancel{nullptr};
    QPushButton* m_btnSave{nullptr};
};

} // namespace QuarkMeta
