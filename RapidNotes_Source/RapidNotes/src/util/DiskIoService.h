#pragma once
#include <QStringList>

namespace RapidNotes {
class DiskIoService {
public:
    static DiskIoService& instance() {
        static DiskIoService s_inst;
        return s_inst;
    }
};
}
