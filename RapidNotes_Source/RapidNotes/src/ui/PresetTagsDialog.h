#pragma once

#include "FramelessDialog.h"
#include "TagPillContainer.h"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

namespace RapidNotes {

class PresetTagsDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit PresetTagsDialog(int categoryId, QWidget* parent = nullptr);
    ~PresetTagsDialog() override = default;

protected:
    bool event(QEvent* event) override;

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    void initUi();
    void loadTags();

    int m_categoryId;
    QString m_categoryName;
    QStringList m_presetTags;
    QStringList m_initialPresetTags;

    QLineEdit* m_folderNameEdit = nullptr;
    TagPillContainer* m_tagContainer = nullptr;
};

} // namespace RapidNotes
