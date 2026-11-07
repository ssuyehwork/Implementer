#pragma once
#include "ActionCommand.h"
#include "../../meta/MetadataManager.h"
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QFile>

namespace QuarkMeta {

class SecureDeleteCommand : public ActionCommand {
public:
    explicit SecureDeleteCommand(const QStringList& paths) : m_targetPaths(paths) {}

    void execute() override {
        for (const auto& path : m_targetPaths) {
            QFileInfo info(path);
            if (info.isDir()) {
                QDir dir(path);
                dir.removeRecursively();
            } else {
                QFile::remove(path);
            }
            MetadataManager::instance().removeMetadataSync(path.toStdWString());
        }
    }

    void undo() override {}

    void redo() override {
        execute();
    }

    QString description() const override { return "安全物理删除"; }

    bool affectsPath(const QString& path) const override {
        for (const auto& p : m_targetPaths) {
            if (p == path) return true;
        }
        return false;
    }

private:
    QStringList m_targetPaths;
};

} // namespace QuarkMeta
