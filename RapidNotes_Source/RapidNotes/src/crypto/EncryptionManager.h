#pragma once
#include <QString>
#include <QStringList>
#include <string>
#include <functional>

namespace RapidNotes {

class EncryptionManager {
public:
    static EncryptionManager& instance() {
        static EncryptionManager s_inst;
        return s_inst;
    }

    bool isEncrypted(const QString& path) { Q_UNUSED(path); return false; }
    void encryptBatchAsync(const QStringList& targets, const std::string& password, std::function<void(bool success)> onFinished) {
        Q_UNUSED(targets); Q_UNUSED(password);
        if (onFinished) onFinished(true);
    }
    void decryptBatchAsync(const QStringList& targets, const std::string& password, std::function<void(bool anySuccess)> onFinished) {
        Q_UNUSED(targets); Q_UNUSED(password);
        if (onFinished) onFinished(true);
    }
    void changePasswordBatchAsync(const QStringList& targets, const std::string& oldPassword, const std::string& newPassword, std::function<void(bool anySuccess)> onFinished) {
        Q_UNUSED(targets); Q_UNUSED(oldPassword); Q_UNUSED(newPassword);
        if (onFinished) onFinished(true);
    }
};

} // namespace RapidNotes
