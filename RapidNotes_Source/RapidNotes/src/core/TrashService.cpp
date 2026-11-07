#include "TrashService.h"
#include "../core/DiskTrashService.h"
#include "../core/OperationSnapshotEngine.h"
#include "../core/PermanentDeleteService.h"
#include "../meta/TrashRepository.h"
#include "../meta/MetadataManager.h"
#include "../meta/AssociationCascadeService.h"
#include "../ui/ToolTipOverlay.h"
#include <QCursor>

namespace QuarkMeta {

TrashService& TrashService::instance() {
    static TrashService s_instance;
    return s_instance;
}

TrashService::TrashService(QObject* parent) : QObject(parent) {}

bool TrashService::moveToTrash(const QStringList& paths, QWidget* parentWidget) {
    if (paths.isEmpty()) return false;

    QStringList validPaths;
    for (const QString& p : paths) {
        if (!p.isEmpty() && p != "computer://" && p != "trash://" && !p.contains(".QuarkMeta/disk_trash")) {
            validPaths << p;
        }
    }
    if (validPaths.isEmpty()) return false;

    // 捕获受影响项目的完整收藏、库与标记关联快照
    AssetAssociationSnapshot assocSnapshot = AssociationCascadeService::instance().findCascadeAssociations(validPaths);

    bool ok = OperationSnapshotEngine::instance().executeWithSnapshot(
        parentWidget,
        SnapshotOperationType::DeleteToTrash,
        validPaths,
        QString("已将 %1 个项目移入回收站").arg(validPaths.size()),
        [validPaths]() {
            MetadataManager::instance().beginInternalOperation();
            bool res = DiskTrashService::moveToDiskTrash(validPaths);
            if (res) {
                AssociationCascadeService::instance().cleanCascadeAssociations(validPaths);
            }
            MetadataManager::instance().endInternalOperation();
            return res;
        },
        [assocSnapshot](const QVector<AssetItemSnapshot>& beforeState) {
            for (const auto& snap : beforeState) {
                int trashId = -1;
                QString trashPath;
                if (TrashRepository::instance().getDiskTrashRecordByPath(snap.path.toStdWString(), trashId, trashPath)) {
                    DiskTrashService::restoreFromDiskTrash(trashId, trashPath);
                }
            }
            // 物理还原后，恢复收藏夹、库分类挂载与标记关联
            AssociationCascadeService::instance().restoreAssociations(assocSnapshot);
            MetadataManager::instance().notifyUI(MetadataManager::RefreshLevel::FullRebuild);
            return true;
        }
    );

    if (ok) {
        MetadataManager::instance().notifyUI(MetadataManager::RefreshLevel::FullRebuild);
        emit trashOperationCompleted();
    }
    return ok;
}

bool TrashService::restoreItems(const QList<int>& trashIds, QWidget* parentWidget) {
    Q_UNUSED(parentWidget);
    if (trashIds.isEmpty()) return false;

    int successCount = 0;
    for (int id : trashIds) {
        if (DiskTrashService::restoreFromDiskTrash(id, "")) {
            successCount++;
        }
    }
    if (successCount > 0) {
        ToolTipOverlay::instance()->showText(QCursor::pos(), QString("成功还原 %1 个回收站项目").arg(successCount), 1500, QColor("#2ecc71"));
        MetadataManager::instance().notifyUI(MetadataManager::RefreshLevel::FullRebuild);
        emit trashOperationCompleted();
    }
    return successCount > 0;
}

bool TrashService::restoreAll(QWidget* parentWidget) {
    Q_UNUSED(parentWidget);
    bool ok = DiskTrashService::restoreAllDiskTrash();
    if (ok) {
        ToolTipOverlay::instance()->showText(QCursor::pos(), "所有回收站项目已成功还原", 1500, QColor("#2ecc71"));
        MetadataManager::instance().notifyUI(MetadataManager::RefreshLevel::FullRebuild);
        emit trashOperationCompleted();
    }
    return ok;
}

bool TrashService::restoreToDirectory(const QStringList& trashPaths, const QString& targetDir, QWidget* parentWidget) {
    Q_UNUSED(parentWidget);
    if (trashPaths.isEmpty() || targetDir.isEmpty()) return false;

    int successCount = 0;
    for (const QString& p : trashPaths) {
        if (DiskTrashService::restoreToDirectory(p, targetDir)) {
            successCount++;
        }
    }
    if (successCount > 0) {
        ToolTipOverlay::instance()->showText(QCursor::pos(), QString("成功恢复 %1 个项目").arg(successCount), 1500, QColor("#2ecc71"));
        emit trashOperationCompleted();
    }
    return successCount > 0;
}

bool TrashService::emptyTrash(QWidget* parentWidget) {
    // 🚀【统一安全管线】：彻底摒弃静默删除，全权委托给 PermanentDeleteService 走高安全物理销毁流程
    return PermanentDeleteService::instance().executeEmptyTrash(parentWidget);
}

} // namespace QuarkMeta