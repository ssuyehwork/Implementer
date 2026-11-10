#include "ClipboardService.h"
#include "CoreEngine.h"
#include "TrashService.h"
#include "../util/DiskIoService.h"
#include "../ui/ToolTipOverlay.h"
#include "../ui/FileCollisionDialog.h"
#include "../ui/dialogs/ProgressDialog.h"
#include "TaskProgressService.h"
#include "ModelContract.h"
#include <QtConcurrent>
#include <QThread>
#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QImage>
#include <QCursor>
#include <QCoreApplication>
#include <QPointer>

namespace QuarkMeta {

ClipboardService& ClipboardService::instance() {
    static ClipboardService s_instance;
    return s_instance;
}

ClipboardService::ClipboardService(QObject* parent) : QObject(parent) {
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, [this]() {
        const QMimeData* mime = QApplication::clipboard()->mimeData();
        if (!mime) {
            clearCutState();
            return;
        }
        bool hasMove = false;
        if (mime->hasFormat("Preferred DropEffect")) {
            QByteArray effect = mime->data("Preferred DropEffect");
            if (!effect.isEmpty() && (effect.at(0) & 0x02)) hasMove = true;
        }
        if (!hasMove) {
            clearCutState();
        }
    });
}

bool ClipboardService::isItemCut(const QString& path) const {
    if (path.isEmpty() || m_cutPaths.isEmpty()) return false;
    return m_cutPaths.contains(QDir::cleanPath(path).toLower());
}

const QSet<QString>& ClipboardService::cutPaths() const {
    return m_cutPaths;
}

void ClipboardService::clearCutState() {
    if (!m_cutPaths.isEmpty()) {
        m_cutPaths.clear();
        emit cutItemsChanged(m_cutPaths);
    }
}

void ClipboardService::setCopiedTags(const QStringList& tags) {
    m_copiedTags = tags;
}

QStringList ClipboardService::copiedTags() const {
    return m_copiedTags;
}

bool ClipboardService::hasCopiedTags() const {
    return !m_copiedTags.isEmpty();
}

void ClipboardService::clearCopiedTags() {
    m_copiedTags.clear();
}

bool ClipboardService::executePasteTags(QAbstractItemView* view) {
    if (!view) return false;

    if (m_copiedTags.isEmpty()) {
        ToolTipOverlay::instance()->showToast("剪贴板无有效标签", 1500, QColor("#e81123"));
        return false;
    }

    QAbstractItemModel* model = view->model();
    if (!model) return false;

    auto indexes = view->selectionModel()->selectedIndexes();
    QStringList targetPaths;
    for (const auto& targetIdx : indexes) {
        if (targetIdx.column() == static_cast<int>(FileListColumn::Name) && !targetIdx.data(SectionHeaderRole).toBool()) {
            QString p = targetIdx.data(PathRole).toString();
            if (!p.isEmpty()) {
                targetPaths.append(p);
            }
        }
    }

    if (!targetPaths.isEmpty()) {
        AppCommand cmd;
        cmd.type = AppCommandType::SetTags;
        cmd.targetPaths = targetPaths;
        cmd.params["tags"] = m_copiedTags;
        CoreEngine::instance().executeCommand(cmd);

        ToolTipOverlay::instance()->showToast(QString("已将标签粘贴至 %1 个项目").arg(targetPaths.size()), 1500, QColor("#2ecc71"));
        return true;
    }
    return false;
}

void ClipboardService::copyItems(const QStringList& paths) {
    if (paths.isEmpty()) return;

    clearCutState();

    QMimeData* mime = new QMimeData();
    QList<QUrl> urls;
    for (const QString& p : paths) {
        if (!p.isEmpty()) urls << QUrl::fromLocalFile(p);
    }
    mime->setUrls(urls);
    QApplication::clipboard()->setMimeData(mime);
}

void ClipboardService::cutItems(const QStringList& paths) {
    if (paths.isEmpty()) return;

    m_cutPaths.clear();
    QMimeData* mime = new QMimeData();
    QList<QUrl> urls;
    for (const QString& p : paths) {
        if (!p.isEmpty()) {
            m_cutPaths.insert(QDir::cleanPath(p).toLower());
            urls << QUrl::fromLocalFile(p);
        }
    }
    mime->setUrls(urls);

    QByteArray effectData;
    effectData.append(static_cast<char>(2)); // DROPEFFECT_MOVE
    mime->setData("Preferred DropEffect", effectData);

    QApplication::clipboard()->setMimeData(mime);
    emit cutItemsChanged(m_cutPaths);
}

bool ClipboardService::canPaste(const QString& targetDir) const {
    if (targetDir.isEmpty() || targetDir == "computer://" || targetDir == "trash://" || targetDir.contains("://")) {
        return false;
    }

    QFileInfo destInfo(targetDir);
    if (!destInfo.exists() || !destInfo.isDir() || !destInfo.isWritable()) {
        return false;
    }

    const QMimeData* mime = QApplication::clipboard()->mimeData();
    if (!mime) return false;

    if (mime->hasImage()) {
        QImage img = qvariant_cast<QImage>(mime->imageData());
        if (!img.isNull() && img.width() > 0 && img.height() > 0) return true;
    }

    if (!mime->hasUrls() || mime->urls().isEmpty()) {
        return false;
    }

    bool isCut = false;
    if (mime->hasFormat("Preferred DropEffect")) {
        QByteArray effect = mime->data("Preferred DropEffect");
        if (!effect.isEmpty() && (effect.at(0) & 0x02)) isCut = true;
    }

    QString cleanDest = QDir::toNativeSeparators(QDir::cleanPath(targetDir)).toLower();
    bool hasValidPhysicalSource = false;
    bool isSameDirForCut = true;

    for (const QUrl& url : mime->urls()) {
        if (!url.isLocalFile()) continue;
        QString localPath = url.toLocalFile();
        if (localPath.isEmpty()) continue;

        QFileInfo srcInfo(localPath);
        if (!srcInfo.exists()) continue;

        hasValidPhysicalSource = true;
        QString cleanSrc = QDir::toNativeSeparators(QDir::cleanPath(localPath)).toLower();

        if (srcInfo.isDir()) {
            if (cleanDest == cleanSrc || cleanDest.startsWith(cleanSrc + "\\") || cleanDest.startsWith(cleanSrc + "/")) {
                return false;
            }
        }

        QString srcParent = QDir::toNativeSeparators(QDir::cleanPath(srcInfo.absolutePath())).toLower();
        if (srcParent != cleanDest) {
            isSameDirForCut = false;
        }
    }

    if (!hasValidPhysicalSource) return false;
    if (isCut && isSameDirForCut) return false;

    return true;
}

void ClipboardService::executePaste(const QString& targetDir, QWidget* parentWidget) {
    Q_UNUSED(parentWidget);
    const QMimeData* mime = QApplication::clipboard()->mimeData();
    if (!mime) return;

    if (mime->hasImage() && (!mime->hasUrls() || mime->urls().isEmpty())) {
        QImage img = qvariant_cast<QImage>(mime->imageData());
        if (!img.isNull() && !targetDir.isEmpty() && QDir(targetDir).exists()) {
            QString timeStr = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
            QString baseName = QString("贴图_%1").arg(timeStr);
            QString fileName = baseName + ".png";
            QString fullPath = QDir(targetDir).filePath(fileName);

            int counter = 1;
            while (QFile::exists(fullPath)) {
                fileName = QString("%1_(%2).png").arg(baseName).arg(counter++);
                fullPath = QDir(targetDir).filePath(fileName);
            }

            if (img.save(fullPath, "PNG")) {
                ToolTipOverlay::instance()->showToast("已将剪贴板图片粘贴为新文件", 1500, QColor("#2ecc71"));
                emit pasteCompleted(targetDir);
            }
            return;
        }
    }

    QStringList fromPaths;
    for (const QUrl& url : mime->urls()) {
        if (url.isLocalFile()) fromPaths << url.toLocalFile();
    }
    if (fromPaths.isEmpty()) return;

    bool hasTrashSource = false;
    for (const QString& p : fromPaths) {
        if (p.contains(".QuarkMeta/disk_trash", Qt::CaseInsensitive)) {
            hasTrashSource = true;
            break;
        }
    }

    if (hasTrashSource) {
        TrashService::instance().restoreToDirectory(fromPaths, targetDir);
        emit pasteCompleted(targetDir);
        return;
    }

    bool isMove = false;
    if (mime->hasFormat("Preferred DropEffect")) {
        QByteArray effect = mime->data("Preferred DropEffect");
        if (!effect.isEmpty() && (effect.at(0) & 0x02)) isMove = true;
    }

    // 检测目标文件夹中的同名冲突文件
    QStringList conflictingSources;
    for (const QString& src : fromPaths) {
        QString fileName = QFileInfo(src).fileName();
        QString destPath = QDir(targetDir).filePath(fileName);
        if (QFile::exists(destPath)) {
            conflictingSources.append(src);
        }
    }

    DiskIoContext ioCtx;
    ioCtx.sources = fromPaths;
    ioCtx.destination = targetDir;
    ioCtx.isMove = isMove;

    if (!conflictingSources.isEmpty()) {
        QStringList activeSources = fromPaths;
        int remainingConflicts = conflictingSources.size();

        for (int i = 0; i < conflictingSources.size(); ++i) {
            const QString& srcFile = conflictingSources.at(i);
            FileCollisionDialog dialog(srcFile, targetDir, remainingConflicts--, parentWidget);

            if (dialog.exec() != QDialog::Accepted) {
                return; // 用户点击关闭或取消
            }

            CollisionResolveAction action = dialog.selectedAction();
            bool applyToAll = dialog.applyToAll();

            if (action == CollisionResolveAction::Cancel) {
                return;
            }

            if (applyToAll) {
                // 勾选了“是否应用于全部文件？”，将当前选择应用至剩余所有冲突文件并退出循环
                if (action == CollisionResolveAction::AutoResolve) {
                    ioCtx.autoRenameAll = true;
                } else if (action == CollisionResolveAction::Replace) {
                    ioCtx.overwriteAll = true;
                } else if (action == CollisionResolveAction::Skip) {
                    for (int j = i; j < conflictingSources.size(); ++j) {
                        activeSources.removeOne(conflictingSources.at(j));
                    }
                }
                break; // 结束逐项弹窗循环
            } else {
                // 未勾选“是否应用于全部文件？”，仅对当前单个文件生效，循环继续处理下一个冲突文件
                if (action == CollisionResolveAction::AutoResolve) {
                    ioCtx.autoRenameFiles.insert(srcFile);
                } else if (action == CollisionResolveAction::Replace) {
                    ioCtx.overwriteFiles.insert(srcFile);
                } else if (action == CollisionResolveAction::Skip) {
                    activeSources.removeOne(srcFile);
                }
            }
        }

        if (activeSources.isEmpty()) {
            ToolTipOverlay::instance()->showToast("已跳过所有同名文件", 1500, QColor("#378ADD"));
            return;
        }
        ioCtx.sources = activeSources;
    }

    QPointer<ClipboardService> weakThis(this);
    auto destMap = std::make_shared<QMap<QString, QString>>();
    ioCtx.actualDestMap = destMap;

    if (activeSources.size() > 20) {
        // 模式 1：超过 20 项，调用具备暂停与取消的 ProgressDialog
        auto* dlg = new ProgressDialog(isMove ? "正在移动项目..." : "正在复制项目...", parentWidget);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        QPointer<ProgressDialog> weakDlg(dlg);

        (void)QtConcurrent::run([weakThis, targetDir, ioCtx, weakDlg, destMap]() {
            int total = ioCtx.sources.size();
            int current = 0;
            bool anyFailed = false;

            for (const QString& src : ioCtx.sources) {
                if (!weakDlg || weakDlg->isCanceled()) {
                    break;
                }

                while (weakDlg && weakDlg->isPaused() && !weakDlg->isCanceled()) {
                    QThread::msleep(100);
                }
                if (!weakDlg || weakDlg->isCanceled()) break;

                current++;
                QString fileName = QFileInfo(src).fileName();
                QMetaObject::invokeMethod(weakDlg.data(), "updateProgress",
                                          Qt::QueuedConnection,
                                          Q_ARG(int, current),
                                          Q_ARG(int, total),
                                          Q_ARG(QString, fileName));

                bool ok = ShellHelper::copyOrMoveItems({src}, ioCtx.destination, ioCtx.isMove,
                                                       ioCtx.overwriteAll, ioCtx.autoRenameAll,
                                                       ioCtx.overwriteFiles, ioCtx.autoRenameFiles,
                                                       destMap.get());
                if (!ok) anyFailed = true;
            }

            QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis, targetDir, weakDlg, anyFailed, destMap]() {
                if (weakDlg) weakDlg->accept();
                if (weakThis) {
                    weakThis->clearCutState();
                    QStringList landed = destMap ? destMap->values() : QStringList();
                    if (!anyFailed) {
                        emit weakThis->pasteCompletedWithTargets(targetDir, landed);
                        emit weakThis->pasteCompleted(targetDir);
                    } else {
                        ToolTipOverlay::instance()->showToast("部分项目未能完成写入", 2000, QColor("#e81123"));
                        emit weakThis->pasteCompletedWithTargets(targetDir, landed);
                        emit weakThis->pasteCompleted(targetDir);
                    }
                }
            });
        });

        dlg->exec();
    } else {
        // 模式 2：少于等于 20 项，调用底栏 TaskProgressService 进度条
        int total = activeSources.size();
        int jobId = TaskProgressService::instance().createJob(isMove ? "正在移动项目..." : "正在复制项目...", total);

        DiskIoService::instance().executeAsync(ioCtx, [weakThis, targetDir, jobId, total, destMap](bool success) {
            QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis, targetDir, jobId, total, destMap, success]() {
                TaskProgressService::instance().updateProgress(jobId, total, total, "粘贴完成");
                TaskProgressService::instance().finishJob(jobId);
                if (weakThis) {
                    weakThis->clearCutState();
                    QStringList landed = destMap ? destMap->values() : QStringList();
                    if (success) {
                        emit weakThis->pasteCompletedWithTargets(targetDir, landed);
                        emit weakThis->pasteCompleted(targetDir);
                    } else {
                        ToolTipOverlay::instance()->showToast("粘贴失败：物理写入未能完成", 2000, QColor("#e81123"));
                    }
                }
            });
        });
    }
}

} // namespace QuarkMeta
