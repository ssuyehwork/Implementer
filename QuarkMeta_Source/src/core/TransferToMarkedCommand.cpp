#include "TransferToMarkedCommand.h"
#include "DiskTrashService.h"
#include "../ui/ToolTipOverlay.h"
#include "../util/ShellHelper.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>

namespace QuarkMeta {

TransferToMarkedCommand::TransferToMarkedCommand(
    const QStringList& sourcePaths,
    const QMap<QString, QVector<TargetResult>>& copyResults)
    : m_sourcePaths(sourcePaths),
      m_copyResults(copyResults)
{
}

QString TransferToMarkedCommand::description() const {
    return QString("分发迁移至标记目的地 (%1 项)").arg(m_sourcePaths.size());
}

bool TransferToMarkedCommand::affectsPath(const QString& path) const {
    for (const QString& src : m_sourcePaths) {
        if (src.compare(path, Qt::CaseInsensitive) == 0) return true;
    }
    for (auto it = m_copyResults.begin(); it != m_copyResults.end(); ++it) {
        for (const auto& res : it.value()) {
            if (res.createdCopyPath.compare(path, Qt::CaseInsensitive) == 0) return true;
        }
    }
    return false;
}

void TransferToMarkedCommand::undo() {
    QStringList pathsToTrash;
    int failedCount = 0;
    bool autoRenamed = false;

    // a. 收集所有 isMoved == false 的副本路径送入回收站
    for (auto it = m_copyResults.begin(); it != m_copyResults.end(); ++it) {
        for (const auto& res : it.value()) {
            if (!res.isMoved) {
                if (!res.createdCopyPath.isEmpty() && QFile::exists(res.createdCopyPath)) {
                    pathsToTrash << res.createdCopyPath;
                } else {
                    failedCount++;
                }
            }
        }
    }
    if (!pathsToTrash.isEmpty()) {
        if (!DiskTrashService::moveToDiskTrash(pathsToTrash)) {
            failedCount += pathsToTrash.size();
        }
    }

    // b. 对每个源文件对应的 isMoved == true 的项，以移动方式移回源目录
    for (auto it = m_copyResults.begin(); it != m_copyResults.end(); ++it) {
        const QString& origSrcPath = it.key();
        QString origDir = QFileInfo(origSrcPath).absolutePath();
        QString origFileName = QFileInfo(origSrcPath).fileName();

        for (auto& res : it.value()) {
            if (res.isMoved) {
                if (res.createdCopyPath.isEmpty() || !QFile::exists(res.createdCopyPath)) {
                    failedCount++;
                    continue;
                }

                QMap<QString, QString> outDestMap;
                bool ok = ShellHelper::copyOrMoveItems(
                    {res.createdCopyPath},
                    origDir,
                    true,  // isMove
                    false, // overwriteAll
                    true,  // autoRenameAll
                    {},    // overwriteFiles
                    {},    // autoRenameFiles
                    &outDestMap
                );

                if (!ok || !outDestMap.contains(res.createdCopyPath)) {
                    failedCount++;
                } else {
                    QString restoredPath = outDestMap.value(res.createdCopyPath);
                    if (QFileInfo(restoredPath).fileName().compare(origFileName, Qt::CaseInsensitive) != 0) {
                        autoRenamed = true;
                    }
                    // d. 还原成功后，更新该记录里的路径，使 redo 用得上
                    res.createdCopyPath = restoredPath;
                }
            }
        }
    }

    if (autoRenamed) {
        ToolTipOverlay::instance()->showToast("原位置已被占用，还原的文件已自动更名保留", 3000, QColor("#378ADD"));
    }

    // c. 任意路径不存在或移动失败，累计数量，结束后弹出一个 Toast 说明"有 N 项未能还原"，不得静默。
    if (failedCount > 0) {
        ToolTipOverlay::instance()->showToast(QString("有 %1 项未能还原").arg(failedCount), 3000, QColor("#e81123"));
    }
}

void TransferToMarkedCommand::redo() {
    if (m_isFirstRun) {
        m_isFirstRun = false;
        return;
    }

    int failedCount = 0;

    // 对每个源文件：先把源路径复制到所有 isMoved == false 的目的地，最后移动到 isMoved == true 的目的地，全部 autoRenameAll = true
    for (auto it = m_copyResults.begin(); it != m_copyResults.end(); ++it) {
        const QString& origSrcPath = it.key();

        // 找到当前可作为源文件的路径（通常是 isMoved 还原后更新的路径，若其存在则优先使用，否则检查 origSrcPath）
        QString currentSrcPath;
        for (const auto& res : it.value()) {
            if (res.isMoved) {
                if (QFile::exists(res.createdCopyPath)) {
                    currentSrcPath = res.createdCopyPath;
                }
                break;
            }
        }
        if (currentSrcPath.isEmpty()) {
            if (QFile::exists(origSrcPath)) {
                currentSrcPath = origSrcPath;
            }
        }

        if (currentSrcPath.isEmpty() || !QFile::exists(currentSrcPath)) {
            // 源路径不存在
            failedCount += it.value().size();
            continue;
        }

        // a. 先复制到所有 isMoved == false 的目的地
        for (auto& res : it.value()) {
            if (!res.isMoved) {
                QMap<QString, QString> outDestMap;
                bool ok = ShellHelper::copyOrMoveItems(
                    {currentSrcPath},
                    res.targetDir,
                    false, // isMove = false (copy)
                    false,
                    true,  // autoRenameAll = true
                    {},
                    {},
                    &outDestMap
                );
                if (ok && outDestMap.contains(currentSrcPath)) {
                    res.createdCopyPath = outDestMap.value(currentSrcPath);
                } else {
                    failedCount++;
                }
            }
        }

        // 最后移动到 isMoved == true 的目的地
        for (auto& res : it.value()) {
            if (res.isMoved) {
                QMap<QString, QString> outDestMap;
                bool ok = ShellHelper::copyOrMoveItems(
                    {currentSrcPath},
                    res.targetDir,
                    true,  // isMove = true (move)
                    false,
                    true,  // autoRenameAll = true
                    {},
                    {},
                    &outDestMap
                );
                if (ok && outDestMap.contains(currentSrcPath)) {
                    res.createdCopyPath = outDestMap.value(currentSrcPath);
                } else {
                    failedCount++;
                }
            }
        }
    }

    if (failedCount > 0) {
        ToolTipOverlay::instance()->showToast(QString("有 %1 项未能重新分发").arg(failedCount), 3000, QColor("#e81123"));
    }
}

} // namespace QuarkMeta
