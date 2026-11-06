#pragma once
#include <QDialog>
#include <QStringList>

namespace RapidNotes {
class CreateFolderWithItemsDialog : public QDialog {
    Q_OBJECT
public:
    explicit CreateFolderWithItemsDialog(const QString& currentPath, const QStringList& selectedPaths, QWidget* parent = nullptr)
        : QDialog(parent) { Q_UNUSED(currentPath); Q_UNUSED(selectedPaths); }
};
}
