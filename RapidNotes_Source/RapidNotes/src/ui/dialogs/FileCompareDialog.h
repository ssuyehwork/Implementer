#pragma once
#include <QDialog>
#include <QStringList>

namespace RapidNotes {
class FileCompareDialog : public QDialog {
    Q_OBJECT
public:
    explicit FileCompareDialog(const QStringList& files, QWidget* parent = nullptr)
        : QDialog(parent) { Q_UNUSED(files); }
};
}
