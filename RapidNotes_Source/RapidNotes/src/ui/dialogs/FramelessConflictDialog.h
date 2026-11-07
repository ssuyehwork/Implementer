#pragma once
#include "../FramelessDialogBase.h"

namespace QuarkMeta {

class FramelessConflictDialog : public FramelessDialog {
    Q_OBJECT
public:
    enum Choice {
        Cancel = 0,
        AutoRename = 1,
        Replace = 2
    };

    explicit FramelessConflictDialog(const QString& title, const QString& message, QWidget* parent = nullptr);

    Choice chosenAction() const { return m_choice; }

private:
    Choice m_choice = Cancel;
};

} // namespace QuarkMeta
