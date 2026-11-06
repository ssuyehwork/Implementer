#pragma once

#include <QObject>
#include <QStringList>
#include <QWidget>

namespace RapidNotes {

class PermanentDeleteService : public QObject {
    Q_OBJECT
public:
    static PermanentDeleteService& instance();

    bool execute(const QStringList& paths, QWidget* parentWidget = nullptr, bool isSecureShred = true);
    bool executeEmptyTrash(QWidget* parentWidget = nullptr, bool isSecureShred = true);

signals:
    void permanentDeleteCompleted();

private:
    explicit PermanentDeleteService(QObject* parent = nullptr);
    ~PermanentDeleteService() override = default;
};

} // namespace RapidNotes
