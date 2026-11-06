#pragma once
#include <QStringList>

namespace RapidNotes {
class DiskTrashService {
public:
    static DiskTrashService& instance() {
        static DiskTrashService s_inst;
        return s_inst;
    }
};
}
