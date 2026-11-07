#pragma once
#include "ActionCommand.h"
#include "../../util/ShellHelper.h"
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

namespace QuarkMeta {

class MoveCommand : public ActionCommand {
public:
    MoveCommand(const QStringList& sourcePaths, const QString& oldDir, const QString& newDir)
        : m_oldDir(oldDir), m_newDir(newDir) {
        for (const QString& p : sourcePaths) {
            m_fileNames << QFileInfo(p).fileName();
        }
    }

    void execute() override {}

    void undo() override {
        QStringList currentPaths;
        for (const QString& name : m_fileNames) {
            currentPaths << QDir(m_newDir).filePath(name);
        }
        ShellHelper::copyOrMoveItems(currentPaths, m_oldDir, true);
    }

    void redo() override {
        QStringList currentPaths;
        for (const QString& name : m_fileNames) {
            currentPaths << QDir(m_oldDir).filePath(name);
        }
        ShellHelper::copyOrMoveItems(currentPaths, m_newDir, true);
    }

    QString description() const override { return "移动文件"; }

    bool affectsPath(const QString& path) const override {
        if (path.startsWith(m_oldDir) || path.startsWith(m_newDir)) return true;
        for (const QString& name : m_fileNames) {
            if (QDir(m_oldDir).filePath(name) == path || QDir(m_newDir).filePath(name) == path) return true;
        }
        return false;
    }

private:
    QStringList m_fileNames;
    QString m_oldDir;
    QString m_newDir;
};

} // namespace QuarkMeta
