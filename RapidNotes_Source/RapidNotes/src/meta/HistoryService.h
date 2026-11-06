#pragma once
#include <QString>

namespace RapidNotes {
class HistoryService {
public:
    static HistoryService& instance() {
        static HistoryService s_inst;
        return s_inst;
    }
    void recordFileAccess(const QString& path) { Q_UNUSED(path); }
};
}
