#pragma once

#include "ActionCommand.h"
#include <QStringList>
#include <QMap>
#include <QVector>

namespace QuarkMeta {

/**
 * @brief 标记目的地分发迁移复合撤销命令
 */
class TransferToMarkedCommand : public ActionCommand {
public:
    struct TargetResult {
        QString targetDir;
        QString createdCopyPath;
        bool isMoved = false;
    };

    TransferToMarkedCommand(const QStringList& sourcePaths,
                            const QMap<QString, QVector<TargetResult>>& copyResults);

    void execute() override {}
    void undo() override;
    void redo() override;
    QString description() const override;
    bool affectsPath(const QString& path) const override;

private:
    QStringList m_sourcePaths;
    QMap<QString, QVector<TargetResult>> m_copyResults;
    bool m_isFirstRun = true;
};

} // namespace QuarkMeta
