#pragma once
#include <QString>
#include <QStringList>

namespace RapidNotes {

class LastOperationManager {
public:
    static LastOperationManager& instance() {
        static LastOperationManager inst;
        return inst;
    }

    bool hasOperation() const { return false; }
    QString displayText() const { return "重复上一次操作"; }
    void recordOperation(const QString& op) { Q_UNUSED(op); }
    void recordMoveToFolder(const QString& destination) { Q_UNUSED(destination); }
};

} // namespace RapidNotes
