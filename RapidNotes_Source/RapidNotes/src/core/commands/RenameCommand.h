#pragma once
#include "ActionCommand.h"
#include "../../util/ShellHelper.h"
#include <QString>

namespace QuarkMeta {

class RenameCommand : public ActionCommand {
public:
    RenameCommand(const QString& oldPath, const QString& newPath)
        : m_oldPath(oldPath), m_newPath(newPath) {}

    void execute() override {}

    void undo() override {
        ShellHelper::renameItem(m_newPath, m_oldPath);
    }

    void redo() override {
        ShellHelper::renameItem(m_oldPath, m_newPath);
    }

    QString description() const override { return "重命名"; }

    bool affectsPath(const QString& path) const override {
        return m_oldPath == path || m_newPath == path;
    }

private:
    QString m_oldPath;
    QString m_newPath;
};

} // namespace QuarkMeta
