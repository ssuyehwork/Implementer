#pragma once
#include "../FramelessDialogBase.h"
#include <QLineEdit>

namespace QuarkMeta {

class FramelessInputDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit FramelessInputDialog(const QString& title, const QString& label,
                                  const QString& initial = "", QWidget* parent = nullptr);
    QString text() const { return m_edit->text().trimmed(); }
    void setEchoMode(QLineEdit::EchoMode mode);

protected:
    void showEvent(QShowEvent* event) override;

private:
    QLineEdit* m_edit;
};

} // namespace QuarkMeta
