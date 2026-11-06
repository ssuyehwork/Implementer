#pragma once
#include <QStringList>
#include <functional>
#include <memory>
#include <QMap>
#include <QSet>

namespace RapidNotes {

struct DiskIoContext {
    QStringList sources;
    QString destination;
    bool isMove = false;
    bool overwriteAll = false;
    bool autoRenameAll = false;
    QSet<QString> overwriteFiles;
    QSet<QString> autoRenameFiles;
    bool recordUndo = true;
    std::shared_ptr<QMap<QString, QString>> actualDestMap;
};

class DiskIoService {
public:
    static DiskIoService& instance() {
        static DiskIoService inst;
        return inst;
    }

    void executeAsync(const DiskIoContext& ctx, std::function<void(bool)> callback) {
        Q_UNUSED(ctx);
        if (callback) callback(true);
    }
};

} // namespace RapidNotes
