#pragma once

#include "FramelessDialog.h"
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

namespace QuarkMeta {

class TextExtensionDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit TextExtensionDialog(QWidget* parent = nullptr);
    ~TextExtensionDialog() override = default;

private slots:
    void onAddExtension();
    void onDeleteExtension();
    void onRestoreDefault();
    void onSaveAndAccept();

private:
    void setupUi();
    void loadCustomExtensions();

    QLineEdit* m_inputEdit{nullptr};
    QPushButton* m_btnAdd{nullptr};
    QListWidget* m_customListWidget{nullptr};
    QPushButton* m_btnDelete{nullptr};
    QPushButton* m_btnRestore{nullptr};
    QPushButton* m_btnCancel{nullptr};
    QPushButton* m_btnSave{nullptr};
};

} // namespace QuarkMeta
