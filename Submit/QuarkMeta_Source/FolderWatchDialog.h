#pragma once

#include "FramelessDialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDropEvent>
#include <QDragEnterEvent>

namespace QuarkMeta {

class FolderWatchRow : public QWidget {
    Q_OBJECT
public:
    explicit FolderWatchRow(const QString& path = "", QWidget* parent = nullptr);
    QString path() const;
    void setPath(const QString& path);

signals:
    void addRequested();
    void removeRequested();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    QLineEdit* m_pathEdit{nullptr};
    QPushButton* m_btnRemove{nullptr};
    QPushButton* m_btnAdd{nullptr};
};

class FolderWatchDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit FolderWatchDialog(QWidget* parent = nullptr);
    ~FolderWatchDialog() override = default;

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onAddRow(const QString& path = "");
    void onRemoveRow(FolderWatchRow* row);
    void onComplete();

private:
    void setupUi();
    void loadWatchedFolders();

    QVBoxLayout* m_rowsLayout{nullptr};
    QList<FolderWatchRow*> m_rows;
    QPushButton* m_btnCancel{nullptr};
    QPushButton* m_btnComplete{nullptr};
};

} // namespace QuarkMeta
