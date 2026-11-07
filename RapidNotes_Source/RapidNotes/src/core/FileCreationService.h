#pragma once

#include <QObject>
#include <QString>

namespace QuarkMeta {

class ContentPanel;

class FileCreationService : public QObject {
    Q_OBJECT

public:
    static FileCreationService& instance();

    bool createNewItem(ContentPanel* panel, const QString& type = "folder");

private:
    explicit FileCreationService(QObject* parent = nullptr);
    ~FileCreationService() override = default;
};

} // namespace QuarkMeta
