#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "PermanentDeleteService.h"
#include "../ui/dialogs/FramelessMessageBox.h"
#include "../core/TaskProgressService.h"
#include "../ui/ToolTipOverlay.h"
#include "../util/SecureFileEraser.h"
#include "../core/UndoManager.h"
#include "../core/CoreEngine.h"
#include "../core/DiskTrashService.h"
#include "../meta/MetadataManager.h"
#include "../meta/TrashRepository.h"
#include "../meta/QuarkMetaJson.h"
#include "../meta/DiskTrashRepo.h"
#include "../meta/AssociationCascadeService.h"
#include <QtConcurrent>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QPointer>
#include <QCoreApplication>
#include <QCursor>

namespace RapidNotes {

PermanentDeleteService& PermanentDeleteService::instance() {
    static PermanentDeleteService s_instance;
    return s_instance;
}

PermanentDeleteService::PermanentDeleteService(QObject* parent) : QObject(parent) {}

bool PermanentDeleteService::execute(const QStringList& paths, QWidget* parentWidget, bool isSecureShred) {
    if (paths.isEmpty()) return false;

    QString msg = "确定要永久删除选中的项目吗？数据将被物理覆写并彻底抹除，此操作不可恢复。";
    if (!FramelessMessageBox::question(parentWidget, "确认删除", msg)) {
        return false;
    }

    int jobId = TaskProgressService::instance().createJob("正在执行永久删除（深层抹除）...", paths.size());

    QPointer<PermanentDeleteService> weakThis(this);

    MetadataManager::instance().beginInternalOperation();

    (void)QtConcurrent::run([paths, isSecureShred, jobId, weakThis]() {
        int total = paths.size();
        int count = 0;

        for (const QString& p : paths) {
            std::wstring wp = QDir::toNativeSeparators(p).toStdWString();
            
            int trashId = -1;
            QString trashPath;
            bool foundRecord = TrashRepository::instance().getDiskTrashRecordByPath(wp, trashId, trashPath);
            bool isTrashItem = foundRecord || p.contains(".RapidNotes/disk_trash");

            if (isTrashItem) {
                if (isSecureShred) {
                    SecureFileEraser::shredFile(p);
                }
                DiskTrashService::permanentlyDeleteDiskTrash(trashId, p);
                UndoManager::instance().removeCommandsForPath(p);
            } else {
                bool physicalOk = false;
                if (isSecureShred) {
                    physicalOk = SecureFileEraser::shredFile(p);
                } else {
                    QFileInfo info(p);
                    if (info.isDir()) {
                        physicalOk = QDir(p).removeRecursively();
                    } else {
                        physicalOk = QFile::remove(p);
                    }
                }

                if (physicalOk) {
                    RapidNotesJson::removeItemMetadata(p);
                    AppCommand cmd;
                    cmd.type = AppCommandType::DeletePermanently;
                    cmd.targetPaths << QString::fromStdWString(wp);
                    CoreEngine::instance().executeCommand(cmd);
                    UndoManager::instance().removeCommandsForPath(p);
                    AssociationCascadeService::instance().cleanCascadeAssociations(QStringList{p});
                }
            }

            count++;
            TaskProgressService::instance().updateProgress(jobId, count, total, QString("已擦除 %1 / %2 项").arg(count).arg(total));
        }

        TaskProgressService::instance().finishJob(jobId);

        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis]() {
            MetadataManager::instance().endInternalOperation();
            if (weakThis) {
                emit weakThis->permanentDeleteCompleted();
            }
        });
    });

    return true;
}

bool PermanentDeleteService::executeTrashItems(const std::vector<std::pair<int, QString>>& trashItems, QWidget* parentWidget) {
    if (trashItems.empty()) return false;

    QString msg = "确定要永久删除选中的回收站项目吗？数据将被彻底抹除且不可恢复。";
    if (!FramelessMessageBox::question(parentWidget, "确认删除", msg)) {
        return false;
    }

    int jobId = TaskProgressService::instance().createJob("正在销毁回收站项目...", static_cast<int>(trashItems.size()));

    QPointer<PermanentDeleteService> weakThis(this);

    MetadataManager::instance().beginInternalOperation();

    (void)QtConcurrent::run([trashItems, jobId, weakThis]() {
        int total = static_cast<int>(trashItems.size());
        int count = 0;

        for (const auto& item : trashItems) {
            SecureFileEraser::shredFile(item.second);
            DiskTrashService::permanentlyDeleteDiskTrash(item.first, item.second);
            UndoManager::instance().removeCommandsForPath(item.second);
            count++;
            TaskProgressService::instance().updateProgress(jobId, count, total, QString("已销毁 %1 / %2 项").arg(count).arg(total));
        }

        TaskProgressService::instance().finishJob(jobId);

        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis]() {
            MetadataManager::instance().endInternalOperation();
            if (weakThis) {
                emit weakThis->permanentDeleteCompleted();
            }
        });
    });

    return true;
}

bool PermanentDeleteService::executeEmptyTrash(QWidget* parentWidget, bool isSecureShred) {
    auto allTrash = DiskTrashRepo::getAllTrashItems();
    if (allTrash.empty()) {
        ToolTipOverlay::instance()->showText(QCursor::pos(), "回收站为空，无需清理", 1500);
        return true;
    }

    QString msg = QString("确定要清空回收站吗？共有 %1 个项目将被物理覆写并彻底抹除，此操作不可恢复。").arg(allTrash.size());
    if (!FramelessMessageBox::question(parentWidget, "清空回收站确认", msg)) {
        return false;
    }

    int jobId = TaskProgressService::instance().createJob("正在清空回收站（深层物理抹除）...", static_cast<int>(allTrash.size()));

    QPointer<PermanentDeleteService> weakThis(this);

    MetadataManager::instance().beginInternalOperation();

    (void)QtConcurrent::run([allTrash, isSecureShred, jobId, weakThis]() {
        int total = static_cast<int>(allTrash.size());
        int count = 0;

        for (const auto& item : allTrash) {
            QString path = QString::fromStdWString(item.trashPath);
            if (isSecureShred) {
                SecureFileEraser::shredFile(path);
            }
            DiskTrashService::permanentlyDeleteDiskTrash(item.id, path);
            UndoManager::instance().removeCommandsForPath(QString::fromStdWString(item.originalPath));

            count++;
            TaskProgressService::instance().updateProgress(jobId, count, total, QString("已清空 %1 / %2 项").arg(count).arg(total));
        }

        TaskProgressService::instance().finishJob(jobId);

        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis]() {
            MetadataManager::instance().endInternalOperation();
            MetadataManager::instance().notifyUI(MetadataManager::RefreshLevel::FullRebuild);
            if (weakThis) {
                emit weakThis->permanentDeleteCompleted();
            }
        });
    });

    return true;
}

} // namespace RapidNotes