#pragma once
#include "ActionCommand.h"
#include <QString>
#include <QStringList>
#include <string>

namespace QuarkMeta {

class ShellProtectionCommand : public ActionCommand {
public:
    ShellProtectionCommand(const QStringList& paths, const std::string& pwd)
        : m_targetPaths(paths), m_pwd(pwd) {}

    void execute() override {}

    void undo() override {}

    void redo() override {
        execute();
    }

    QString description() const override { return "外壳保护"; }

    bool affectsPath(const QString& path) const override {
        for (const auto& p : m_targetPaths) {
            if (p == path) return true;
        }
        return false;
    }

private:
    QStringList m_targetPaths;
    std::string m_pwd;
};

// 保持向下兼容别名
using EncryptCommand = ShellProtectionCommand;

} // namespace QuarkMeta
