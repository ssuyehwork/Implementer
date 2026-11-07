#pragma once
#include <QPushButton>

namespace QuarkMeta {

class FolderButton : public QPushButton {
    Q_OBJECT
public:
    explicit FolderButton(const QString& folderPath, QWidget* parent = nullptr);
    QString folderPath() const { return m_folderPath; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QString m_folderPath;
    QString m_folderName;
};

} // namespace QuarkMeta
