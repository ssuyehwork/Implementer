#pragma once
#include <QString>

namespace RapidNotes {
class EncryptionManager {
public:
    static EncryptionManager& instance() {
        static EncryptionManager s_inst;
        return s_inst;
    }
    bool isEncrypted(const QString& path) { Q_UNUSED(path); return false; }
};
}
