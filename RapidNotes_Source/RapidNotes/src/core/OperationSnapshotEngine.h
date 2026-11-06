#pragma once
#include <QString>

namespace RapidNotes {
class OperationSnapshotEngine {
public:
    static OperationSnapshotEngine& instance() {
        static OperationSnapshotEngine s_inst;
        return s_inst;
    }
};
}
