#include "ContentFileOpsHandler.h"
#include "../ContentPanel.h"
#include "../ViewDragDropHelper.h"
#include "../../core/FileCreationService.h"
#include "../ColumnViewWidget.h"
#include "../ToolTipOverlay.h"
#include "../BatchRenameDialog.h"
#include "../FileCollisionDialog.h"
#include "../models/DiskItemModel.h"
#include "../../core/AppConfig.h"
#include "../../core/ClipboardService.h"
#include "../../core/NavigationHistoryService.h"
#include "../../core/LastOperationManager.h"
#include "../../util/DiskIoService.h"
#include "../../meta/LibraryService.h"

#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QApplication>
#include <QPointer>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QMetaObject>

namespace QuarkMeta {

ContentFileOpsHandler::ContentFileOpsHandler(ContentPanel* panel)
    : QObject(panel), m_panel(panel) {}

void ContentFileOpsHandler::createNewItem(const QString& type) {
    FileCreationService::instance().createNewItem(m_panel, type);
}

void ContentFileOpsHandler::performBatchRename() {
    if (!m_panel) return;
    std::vector<std::wstring> originalPaths;
    for (const auto& idx : m_panel->getSelectedIndexes()) {
        if (idx.column() == static_cast<int>(FileListColumn::Name)) {
            QString p = idx.data(PathRole).toString();
            if (!p.isEmpty()) {
                originalPaths.push_back(QDir::toNativeSeparators(p).toStdWString());
            }
        }
    }
    if (originalPaths.empty()) return;

    // 收集根窗格与所有子窗格（去重）
    QList<ContentPanel*> targetPanes;
    ContentPanel* root = m_panel->rootPane();
    if (root) {
        targetPanes.append(root);
        for (ContentPanel* pane : root->panes()) {
            if (pane && !targetPanes.contains(pane)) {
                targetPanes.append(pane);
            }
        }
    } else if (m_panel) {
        targetPanes.append(m_panel);
    }

    struct WatchSuppressor {
        QList<ContentPanel*> panes;
        explicit WatchSuppressor(const QList<ContentPanel*>& p) : panes(p) {
            for (ContentPanel* cp : panes) {
                if (cp) cp->beginWatchSuppress();
            }
        }
        ~WatchSuppressor() {
            for (ContentPanel* cp : panes) {
                if (cp) cp->endWatchSuppress();
            }
        }
    } suppressor(targetPanes);

    BatchRenameDialog dlg(originalPaths, m_panel);
    if (dlg.exec() == QDialog::Accepted) {
        for (ContentPanel* cp : targetPanes) {
            if (cp) cp->refreshAll();
        }
    }
}

bool ContentFileOpsHandler::resolvePasteDestination() {
    if (!m_panel) return false;
    if (m_panel->getCurrentCategoryType() == "trash") {
        ToolTipOverlay::instance()->showText(QCursor::pos(), "当前视图为回收站，不支持粘贴或拖拽导入新项目", 2000, QColor("#e81123"));
        return false;
    }
    QString currentPath = m_panel->activePath();
    if (currentPath.isEmpty() || currentPath == "computer://") {
        ToolTipOverlay::instance()->showText(QCursor::pos(), "粘贴失败：当前未处于任何有效目录中", 2000, QColor("#e81123"));
        return false;
    }
    return true;
}

void ContentFileOpsHandler::onPathsDropped(const QStringList& paths, const QModelIndex& targetIndex, const QString& targetDirOverride, Qt::DropAction action) {
    if (!m_panel || paths.isEmpty()) return;

    // 1. 目标窗格分类视图拖放重定向逻辑
    QString catType = m_panel->getCurrentCategoryType();
    int catId = m_panel->currentCategoryId();

    if (catType == "library" && (catId > 0 || catId == -2)) {
        // 目标为普通分类：执行入库/归类到该分类
        bool ok = LibraryService::instance().addPathsToCategory(catId, paths);
        if (ok) {
            ToolTipOverlay::instance()->showToast(QString("已成功归入“%1”分类").arg(m_panel->currentCategoryTitle()), 2000, QColor("#2ecc71"));
            m_panel->refreshAll();
        } else {
            ToolTipOverlay::instance()->showToast("归入分类失败", 2000, QColor("#e81123"));
        }
        return;
    } else if (catType == "smart_folder" || catType == "library") {
        // 目标为智能文件夹或全量/系统分类：兜底入库归属于“未分类”
        bool ok = LibraryService::instance().addPathsToCategory(-2, paths);
        if (ok) {
            ToolTipOverlay::instance()->showToast("项目已入库到“未分类”", 2500, QColor("#3498db"));
            m_panel->refreshAll();
        } else {
            ToolTipOverlay::instance()->showToast("入库“未分类”失败", 2000, QColor("#e81123"));
        }
        return;
    }

    QString baseDir = !targetDirOverride.isEmpty() ? targetDirOverride : m_panel->currentPath();
    if (baseDir.isEmpty() || baseDir == "computer://") return;

    QString destDir = baseDir;

    if (targetIndex.isValid()) {
        QString targetPath = targetIndex.data(PathRole).toString();
        if (!targetPath.isEmpty() && QFileInfo(targetPath).isDir()) {
            destDir = targetPath;
        }
    }

    qDebug() << "[ColumnView DragDrop Debug] Sources:" << paths 
             << "| TargetDirOverride:" << targetDirOverride 
             << "| Final DestDir:" << destDir
             << "| DropAction:" << action;

    bool isCopyOperation = (action == Qt::CopyAction) || (QApplication::keyboardModifiers() & Qt::ControlModifier);
    bool isMove = !isCopyOperation;

    if (!destDir.isEmpty() && destDir != "computer://") {
        NavigationHistoryService::recordRecentVisitedFolder(QDir::toNativeSeparators(destDir).toStdWString());
        AppConfig::instance().setValue("RecentVisited/LastDragDropDestination", destDir);
        AppConfig::instance().sync();
        if (isMove) {
            LastOperationManager::instance().recordMoveToFolder(destDir);
        }
    }

    // 0. 原地/同目录拖放保护与 Ctrl+Drag 副本创建门禁
    QStringList externalPaths;
    bool isDuplicateCopy = false;

    for (const QString& src : paths) {
        QFileInfo srcInfo(src);
        bool isSameDirectory = (QDir::cleanPath(srcInfo.absolutePath()) == QDir::cleanPath(QDir(destDir).absolutePath()));

        if (isSameDirectory) {
            if (isCopyOperation) {
                // 判断 Ctrl+Drag 拖拽物理距离（全物理坐标对比），防止 Ctrl+Click 多选误触（门禁设为 50px）
                QPoint dragStartPos = ViewDragDropHelper::lastDragStartPos();
                QPoint dropPos = QCursor::pos();
                int dragDistance = (dragStartPos.isNull()) ? 100 : (dropPos - dragStartPos).manhattanLength();

                if (dragDistance >= 50) {
                    externalPaths.append(src);
                    isDuplicateCopy = true;
                }
            }
        } else {
            externalPaths.append(src);
        }
    }

    if (externalPaths.isEmpty()) {
        // 全为同目录内自拖放且未触发 Ctrl+Drag 副本创建门禁，静默处理
        return;
    }

    // 1. 仅对非同目录副本创建的项目检测目标文件夹中的同名冲突文件
    QStringList conflictingSources;
    if (!isDuplicateCopy) {
        for (const QString& src : externalPaths) {
            QString fileName = QFileInfo(src).fileName();
            QString destPath = QDir(destDir).filePath(fileName);
            if (QFile::exists(destPath)) {
                conflictingSources.append(src);
            }
        }
    }

    DiskIoContext ioCtx;
    ioCtx.sources = externalPaths;
    ioCtx.destination = destDir;
    ioCtx.isMove = isMove;

    if (isDuplicateCopy) {
        // 同目录 Ctrl+Drag 专属意图：强制自动追加序号重命名（如 filename-1.ext），绕过冲突弹窗
        ioCtx.autoRenameAll = true;
        ioCtx.isMove = false;
    }

    if (!conflictingSources.isEmpty()) {
        QStringList activeSources = externalPaths;
        int remainingConflicts = conflictingSources.size();

        for (int i = 0; i < conflictingSources.size(); ++i) {
            const QString& srcFile = conflictingSources.at(i);
            FileCollisionDialog dialog(srcFile, destDir, remainingConflicts--, m_panel);

            if (dialog.exec() != QDialog::Accepted) {
                return; // 用户取消
            }

            CollisionResolveAction resolveAct = dialog.selectedAction();
            bool applyToAll = dialog.applyToAll();

            if (resolveAct == CollisionResolveAction::Cancel) {
                return;
            }

            if (applyToAll) {
                if (resolveAct == CollisionResolveAction::AutoResolve) {
                    ioCtx.autoRenameAll = true;
                } else if (resolveAct == CollisionResolveAction::Replace) {
                    ioCtx.overwriteAll = true;
                } else if (resolveAct == CollisionResolveAction::Skip) {
                    for (int j = i; j < conflictingSources.size(); ++j) {
                        activeSources.removeOne(conflictingSources.at(j));
                    }
                }
                break;
            } else {
                if (resolveAct == CollisionResolveAction::AutoResolve) {
                    ioCtx.autoRenameFiles.insert(srcFile);
                } else if (resolveAct == CollisionResolveAction::Replace) {
                    ioCtx.overwriteFiles.insert(srcFile);
                } else if (resolveAct == CollisionResolveAction::Skip) {
                    activeSources.removeOne(srcFile);
                }
            }
        }

        if (activeSources.isEmpty()) {
            ToolTipOverlay::instance()->showText(QCursor::pos(), "已跳过所有同名文件", 1500, QColor("#378ADD"));
            return;
        }
        ioCtx.sources = activeSources;
    }

    QPointer<ContentPanel> weakPanel(m_panel);
    DiskIoService::instance().executeAsync(ioCtx, [weakPanel](bool success) {
        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakPanel, success]() {
            if (weakPanel && success) {
                ContentPanel* root = weakPanel->rootPane();
                if (root) {
                    root->refreshAll();
                    for (ContentPanel* pane : root->panes()) {
                        if (pane && pane != root) {
                            pane->refreshAll();
                        }
                    }
                } else {
                    weakPanel->refreshAll();
                }
            }
        });
    });
}

} // namespace QuarkMeta
