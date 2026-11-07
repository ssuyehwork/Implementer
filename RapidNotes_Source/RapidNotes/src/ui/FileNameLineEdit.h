#pragma once

#include <QLineEdit>
#include <QFocusEvent>
#include <QKeyEvent>

namespace QuarkMeta {

/**
 * @brief Standalone LineEdit with autonomous selection and key navigation for file renaming.
 */
class FileNameLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit FileNameLineEdit(QWidget* parent = nullptr);
    void setIsFolder(bool isFolder);

protected:
    void focusInEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    bool m_isFolder = false;
};

} // namespace QuarkMeta
