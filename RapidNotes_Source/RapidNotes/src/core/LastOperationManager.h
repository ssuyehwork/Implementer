#pragma once
#include <QString>

namespace RapidNotes {
class LastOperationManager {
public:
    static LastOperationManager& instance() {
        static LastOperationManager s_inst;
        return s_inst;
    }
    void recordOperation(const QString& op) { Q_UNUSED(op); }
};
}
