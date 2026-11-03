#include "MarkedFolderService.h"
#include "MarkedFolderDao.h"
#include <QAtomicInt>
#include "../ui/UiHelper.h"
#include "../ui/ToolTipOverlay.h"
#include "../ui/PanelMediator.h"
#include "../ui/ContentPanel.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QAction>
#include "../util/ShellHelper.h"
#include "../util/DiskIoService.h"
#include "../util/VolumePathResolver.h"
#include "../core/TaskProgressService.h"
#include "../core/DiskTrashService.h"
#include "../core/UndoManager.h"
#include "../core/TransferToMarkedCommand.h"

namespace QuarkMeta {

MarkedFolderService& MarkedFolderService::instance() {
    static MarkedFolderService s_instance;
    return s_instance;
}

MarkedFolderService::MarkedFolderService(QObject* parent) : QObject(parent) {
    MarkedFolderDao::initTable();
}

bool MarkedFolderService::isMarked(const QString& path) const {
    if (path.isEmpty()) return false;
    QFileInfo fi(path);
    if (!fi.exists() || !fi.isDir()) return false;
    return MarkedFolderDao::isMarked(path);
}

int MarkedFolderService::markedCount() const {
    return MarkedFolderDao::getMarkedCount();
}

bool MarkedFolderService::addMarked(const QString& path) {
    if (path.isEmpty()) return false;
    QFileInfo fi(path);
    if (!fi.exists() || !fi.isDir()) return false;

    if (markedCount() >= 50 && !isMarked(path)) {
        ToolTipOverlay::instance()->showToast("标记数量已达 50 个上限，请先移除部分标记", 2000, QColor("#e81123"));
        return false;
    }

    if (MarkedFolderDao::addMarkedFolder(path)) {
        emit markedFoldersChanged();
        return true;
    }
    return false;
}

bool MarkedFolderService::keepOnlyMarked(const QString& targetPath) {
    if (targetPath.isEmpty()) return false;
    if (MarkedFolderDao::unmarkAllFolders()) {
        bool ok = addMarked(targetPath);
        emit markedFoldersChanged();
        return ok;
    }
    return false;
}

bool MarkedFolderService::executeTransferToMarked(const QStringList& sourcePaths, const QStringList& targetDirs) {
    QStringList resolvedSources = sourcePaths;
    if (resolvedSources.isEmpty()) {
        if (auto* mediator = PanelMediator::instance()) {
            if (auto* panel = mediator->activeContentPanel()) {
                resolvedSources = panel->getSelectedPaths();
            }
        }
    }

    if (resolvedSources.isEmpty()) {
        ToolTipOverlay::instance()->showToast("未选择任何要迁移的文件或文件夹", 2000, QColor("#e81123"));
        return false;
    }

    QStringList activeTargets = targetDirs.isEmpty() ? getActiveMarked() : targetDirs;
    if (activeTargets.isEmpty()) {
        ToolTipOverlay::instance()->showToast("未激活任何标记目的地文件夹 (请先用 Alt+B 标记)", 2500, QColor("#e81123"));
        return false;
    }

    // 预检测有效源路径与目的地列表
    QStringList validSources;
    for (const QString& sp : resolvedSources) {
        if (QFile::exists(sp)) {
            validSources << QDir::cleanPath(sp);
        }
    }
    if (validSources.isEmpty()) return false;

    QStringList validTargets;
    for (const QString& td : activeTargets) {
        QFileInfo tfi(td);
        if (tfi.exists() && tfi.isDir()) {
            validTargets << QDir::cleanPath(td);
        }
    }
    if (validTargets.isEmpty()) {
        ToolTipOverlay::instance()->showToast("所有标记目的地路径均不存在或无效", 2500, QColor("#e81123"));
        return false;
    }

    int totalWorkUnits = validSources.size() * validTargets.size();
    int jobId = TaskProgressService::instance().createJob("分发迁移至标记目的地", totalWorkUnits);

    // 一次性创建共享原子计数器与结果图谱
    auto completedCounter = std::make_shared<QAtomicInt>(0);
    auto sharedCopyResults = std::make_shared<QMap<QString, QVector<TransferToMarkedCommand::TargetResult>>>();
    auto sharedFailure = std::make_shared<QAtomicInt>(0);

    // 使用 std::function 递归链式调度 (Sequential Async Waterfall)，确保前 N-1 个复制目的地全部成功完成后，才触发最后一个目的地的物理移动
    auto dispatchNextTarget = std::make_shared<std::function<void(int, int)>>();
    *dispatchNextTarget = [jobId, completedCounter, totalWorkUnits, sharedCopyResults, sharedFailure, validTargets, validSources, dispatchNextTarget](int srcIdx, int targetIdx) {
        if (srcIdx >= validSources.size()) {
            // 所有源文件与所有目的地处理完毕，在主线程统一结算 UndoManager 与 TaskProgress
            QMetaObject::invokeMethod(QCoreApplication::instance(), [jobId, sharedCopyResults, sharedFailure, validSources]() {
                if (!sharedCopyResults->isEmpty()) {
                    UndoManager::instance().pushCommand(std::make_unique<TransferToMarkedCommand>(
                        validSources, *sharedCopyResults
                    ));
                }
                TaskProgressService::instance().finishJob(jobId);

                if (sharedFailure->loadAcquire() == 0) {
                    ToolTipOverlay::instance()->showToast("已成功分发迁移至目的地", 2000, QColor("#0078d4"));
                } else {
                    ToolTipOverlay::instance()->showToast("分发迁移完成，部分项目未能成功处理", 2500, QColor("#e81123"));
                }
            });
            return;
        }

        const QString& srcPath = validSources[srcIdx];
        const QString& targetDir = validTargets[targetIdx];
        QFileInfo srcInfo(srcPath);
        QString itemName = srcInfo.fileName();

        QString cleanSrc = QDir::cleanPath(srcPath).toLower();
        QString cleanTarget = QDir::cleanPath(targetDir).toLower();

        // 计算下一个目标索引
        int nextSrcIdx = srcIdx;
        int nextTargetIdx = targetIdx + 1;
        if (nextTargetIdx >= validTargets.size()) {
            nextTargetIdx = 0;
            nextSrcIdx++;
        }

        if (cleanSrc == cleanTarget || cleanTarget.startsWith(cleanSrc + "/")) {
            int current = completedCounter->fetchAndAddOrdered(1) + 1;
            TaskProgressService::instance().updateProgress(jobId, current, totalWorkUnits);
            (*dispatchNextTarget)(nextSrcIdx, nextTargetIdx);
            return;
        }

        bool isMoved = (targetIdx == validTargets.size() - 1);
        auto actualDestMap = std::make_shared<QMap<QString, QString>>();

        DiskIoContext ioCtx;
        ioCtx.sources = QStringList{srcPath};
        ioCtx.destination = targetDir;
        // 只有当目标是该源文件的最后一个目的地时，才执行物理剪切/移动；前面的目的地严格执行复制
        ioCtx.isMove = isMoved;
        ioCtx.recordUndo = false;
        ioCtx.autoRenameAll = true;
        ioCtx.actualDestMap = actualDestMap;

        DiskIoService::instance().executeAsync(ioCtx, [jobId, completedCounter, totalWorkUnits, sharedCopyResults, sharedFailure, srcPath, targetDir, isMoved, actualDestMap, nextSrcIdx, nextTargetIdx, dispatchNextTarget](bool targetSuccess) {
            int current = completedCounter->fetchAndAddOrdered(1) + 1;
            if (!targetSuccess) {
                sharedFailure->storeRelease(1);
            } else {
                if (actualDestMap->contains(srcPath)) {
                    QString createdCopyPath = actualDestMap->value(srcPath);
                    (*sharedCopyResults)[srcPath].append({targetDir, createdCopyPath, isMoved});
                }
            }

            QMetaObject::invokeMethod(QCoreApplication::instance(), [jobId, current, totalWorkUnits]() {
                TaskProgressService::instance().updateProgress(jobId, current, totalWorkUnits);
            });

            // 链式触发下一个目的地的异步任务，保证绝对的时序安全！
            (*dispatchNextTarget)(nextSrcIdx, nextTargetIdx);
        });
    };

    // 启动第一个链式任务
    (*dispatchNextTarget)(0, 0);
    return true;
}

bool MarkedFolderService::removeMarked(const QString& path) {
    if (MarkedFolderDao::unmarkFolder(path)) {
        emit markedFoldersChanged();
        return true;
    }
    return false;
}

bool MarkedFolderService::toggleMarked(const QString& path) {
    if (isMarked(path)) {
        return removeMarked(path);
    } else {
        return addMarked(path);
    }
}

bool MarkedFolderService::toggleMarked(const QStringList& paths) {
    if (paths.isEmpty()) return false;
    bool allMarked = true;
    for (const QString& p : paths) {
        if (!isMarked(p)) {
            allMarked = false;
            break;
        }
    }

    bool changed = false;
    if (allMarked) {
        for (const QString& p : paths) {
            if (removeMarked(p)) changed = true;
        }
    } else {
        for (const QString& p : paths) {
            if (!isMarked(p)) {
                if (addMarked(p)) changed = true;
            }
        }
    }
    return changed;
}

QStringList MarkedFolderService::getAllMarked() const {
    return MarkedFolderDao::getAllMarkedFolders();
}

QStringList MarkedFolderService::getActiveMarked() const {
    return MarkedFolderDao::getActiveMarkedFolders();
}

bool MarkedFolderService::removeFolderRecord(const QString& path) {
    if (MarkedFolderDao::removeFolderRecord(path)) {
        emit markedFoldersChanged();
        return true;
    }
    return false;
}

bool MarkedFolderService::clearAllFolderRecords() {
    if (MarkedFolderDao::clearAllFolderRecords()) {
        emit markedFoldersChanged();
        return true;
    }
    return false;
}

bool MarkedFolderService::clearAll() {
    if (MarkedFolderDao::unmarkAllFolders()) {
        emit markedFoldersChanged();
        return true;
    }
    return false;
}

QAction* MarkedFolderService::buildMarkAction(QMenu* parentMenu, const QString& path, QObject* receiver) {
    return buildMarkAction(parentMenu, QStringList{path}, receiver);
}

QAction* MarkedFolderService::buildMarkAction(QMenu* parentMenu, const QStringList& paths, QObject* receiver) {
    if (!parentMenu || paths.isEmpty()) return nullptr;

    QStringList validDirs;
    for (const QString& p : paths) {
        QFileInfo fi(p);
        if (fi.exists() && fi.isDir()) {
            validDirs << p;
        }
    }
    if (validDirs.isEmpty()) return nullptr;

    bool allMarked = true;
    for (const QString& p : validDirs) {
        if (!isMarked(p)) {
            allMarked = false;
            break;
        }
    }

    QString label = allMarked ? "取消标记" : "标记为目标文件夹";
    QIcon icon = UiHelper::getIcon("Destination.svg", QColor("#EEEEEE"), 18);

    QAction* action = parentMenu->addAction(icon, label);
    if (receiver) {
        connect(action, &QAction::triggered, receiver, [this, validDirs]() {
            toggleMarked(validDirs);
        });
    }
    return action;
}

} // namespace QuarkMeta
