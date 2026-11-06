#pragma once
#include <QDialog>

namespace RapidNotes {
class FileCollisionDialog : public QDialog {
    Q_OBJECT
public:
    explicit FileCollisionDialog(QWidget* parent = nullptr) : QDialog(parent) {}
};
}
