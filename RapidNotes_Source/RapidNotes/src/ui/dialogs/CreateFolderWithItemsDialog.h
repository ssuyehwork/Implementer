#pragma once

#include "FramelessDialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

namespace QuarkMeta {

class CreateFolderWithItemsDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit CreateFolderWithItemsDialog(const QString& currentDir, const QStringList& selectedPaths, QWidget* parent = nullptr);
    ~CreateFolderWithItemsDialog() override = default;

private slots:
    void onCreateAndMigrate();

private:
    void setupUi();

    QString m_currentDir;
    QStringList m_selectedPaths;
    QLineEdit* m_nameEdit{nullptr};
    QPushButton* m_btnCancel{nullptr};
    QPushButton* m_btnCreate{nullptr};
};

} // namespace QuarkMeta
