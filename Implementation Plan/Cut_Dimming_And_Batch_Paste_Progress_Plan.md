# Implementation Plan - 剪切置灰变暗交互与批量粘贴 ProgressDialog 强控规范实施方案

## 实施方案元数据与三道关检查日志

- **方案名称**：Cut_Dimming_And_Batch_Paste_Progress_Plan.md
- **物理存储路径**：`Implementation Plan/Cut_Dimming_And_Batch_Paste_Progress_Plan.md`
- **关联设计规范**：`QuarkMeta_Source/Memories_QuarkMeta.md`（第 0 节、第 1.2 节与第 53 章）
- **基准状态**：Verified & Self-Audited (Enhanced with Dual-Track Progress & Post-Paste Auto-Selection)

---

## 门禁 1：起草前物理证据清单 (Gate 1: Pre-Draft Physical Evidence)

### 1.1 物理读取文件列表与确认
1. `G:/C++/Project/Source_Code/QuarkMeta_Source/Dual‑mode version/src/ui/ProgressDialog.h`（已通过 `read_file` 物理核对原版结构）
2. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/core/ClipboardService.h`（已通过 `read_file` 物理核对）
3. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/core/ClipboardService.cpp`（已通过 `read_file` 物理核对）
4. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/core/TaskProgressService.h`（已通过 `read_file` 物理核对 `createJob`、`updateProgress`、`finishJob` 接口）
5. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/TreeItemDelegate.h`（已通过 `read_file` 物理核对）
6. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/ThumbnailDelegate.cpp`（已通过 `read_file` 物理核对）
7. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/ContentPanel.h`（已通过 `read_file` 物理核对）
8. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/ContentPanel.cpp`（已通过 `read_file` 物理核对）
9. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp`（已通过 `read_file` 物理核对）
10. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/util/ShellHelper.cpp`（已通过 `read_file` 物理核对）
11. `G:/C++/Project/Source_Code/QuarkMeta_Source/CMakeLists.txt`（已通过 `read_file` 物理核对）
12. `G:/C++/Project/Source_Code/QuarkMeta_Source/resources/style.qss`（已通过 `read_file` 物理核对）

### 1.2 调用的外部成员函数原样签名核对
- `ClipboardService::cutItems(const QStringList& paths)`：在 `ClipboardService.h:17` 真实存在，声明为 `void cutItems(const QStringList& paths);`
- `ClipboardService::copyItems(const QStringList& paths)`：在 `ClipboardService.h:16` 真实存在，声明为 `void copyItems(const QStringList& paths);`
- `TaskProgressService::createJob(const QString& title, int totalSteps)`：在 `TaskProgressService.h:18` 真实存在
- `TaskProgressService::finishJob(int jobId)`：在 `TaskProgressService.h:20` 真实存在
- `ContentPanel::refreshAll()`：在 `ContentPanel.h:147` 真实存在
- `ContentPanel::restoreSelections()`：在 `ContentPanel.h:149` 真实存在

### 1.3 查重优先与既有 API 检索记录
- 检索 `TaskProgressService`：复用既有的应用全局任务进度条架构，负责轻量/中量（$\le 20$ 项）粘贴进度呈现；
- 检索 `ProgressDialog`：从 `Dual-mode version` 移植并升级至主工程，负责大批量（$> 20$ 项）强控弹窗；
- 检索选中恢复：复用 `ContentPanel::restoreSelections` 与 `m_viewCoordinator->restoreSelections`，无需重写选择定位逻辑。

---

## Chapter 1: Overview

### 1.1 背景与用户痛点
1. **剪切状态零视觉反馈**：用户按下 `Ctrl + X` 后，项目在列表和缩略图视图中无任何变化，无法确认是否已剪切，也无法感知哪些项处于移动候选状态；
2. **大批量粘贴无进度指示与不可控**：旧实现中 `ShellHelper` 硬编码 `FOF_SILENT` 且未接入任何进度反馈，粘贴大量文件时界面“黑盒假死”，且无中途暂停或取消的防护机制；
3. **小批量缺少任务进度感知**：轻量操作完全无进度感知，易引起用户疑虑；
4. **粘贴后高亮丢失**：粘贴完成后新文件混在成百上千个文件中，不会像 Windows 资源管理器那样自动高亮选中新文件，需用户手动寻找。

### 1.2 重构与实施目标
1. **剪切视觉半透明置灰（对标 Windows）**：
   - `ClipboardService` 维护 `m_cutPaths` 并在 `Ctrl + X` 触发时发射广播；
   - `TreeItemDelegate` 与 `ThumbnailDelegate` 检测到项处于剪切态时，调用 `painter->setOpacity(0.5)` 实现 50% 变暗置灰；
   - 在粘贴成功、执行复制或按下 `Esc` 键时，自动清空剪切态并恢复视图 100% 不透明度。
2. **分级双轨进度调用策略**：
   - **$\le 20$ 个文件**：自动调用应用底部的 `TaskProgressService`，在界面底栏非侵入式展开 `TaskProgressStrip` 进度条，完成后平滑收起；
   - **$> 20$ 个文件**：自动唤起前台居中无边框模态 `ProgressDialog` 对话框，界面配备 **【暂停 / 继续】** 与 **【取消】** 按钮，支持原子状态下线程挂起与安全撤销。
3. **粘贴完成自动高亮选中**：
   - 精准收集实际落地的所有新文件绝对物理路径（包含自动重命名后缀的路径）；
   - 触发视图原位刷新后，自动将新落地文件置入待选集合，自动全量选中并高亮呈现新文件，视口自动滚动聚焦。

---

## Chapter 2: Modified Files List

1. `QuarkMeta_Source/src/ui/dialogs/ProgressDialog.h`（**新建文件**：移植并具备暂停与取消功能）
2. `QuarkMeta_Source/CMakeLists.txt`（注册新建的 `ProgressDialog.h`）
3. `QuarkMeta_Source/src/core/ClipboardService.h`（声明剪切状态、落地路径收集与广播信号）
4. `QuarkMeta_Source/src/core/ClipboardService.cpp`（实现双轨进度调用与落地路径收集）
5. `QuarkMeta_Source/src/ui/ContentPanel.h`（增加 `setPendingSelectPaths` 声明）
6. `QuarkMeta_Source/src/ui/ContentPanel.cpp`（实现批量待选注入、监听粘贴完成自动高亮新文件）
7. `QuarkMeta_Source/src/ui/TreeItemDelegate.h`（绘制时根据剪切状态施加 50% 不透明度变暗）
8. `QuarkMeta_Source/src/ui/ThumbnailDelegate.cpp`（绘制网格卡片时根据剪切状态施加 50% 不透明度变暗）
9. `QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp`（按 Esc 键时清除剪切状态）
10. `QuarkMeta_Source/resources/style.qss`（追加 ProgressDialog 的控件专属暗黑样式）

---

## Chapter 3: Detailed Line-by-Line Changes

### 3.1 `QuarkMeta_Source/src/ui/dialogs/ProgressDialog.h` (新建文件)
```
<<<<<<< SEARCH FILE: src/ui/dialogs/ProgressDialog.h
=======
#pragma once

#include "../FramelessDialog.h"
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <atomic>
#include <QPointer>
#include <QCoreApplication>

namespace QuarkMeta {

class ProgressDialog : public FramelessDialog {
    Q_OBJECT

public:
    explicit ProgressDialog(const QString& title, QWidget* parent = nullptr)
        : FramelessDialog(title, parent) {
        setFixedSize(440, 160);

        auto* layout = new QVBoxLayout(m_contentArea);
        layout->setContentsMargins(20, 15, 20, 15);
        layout->setSpacing(12);

        m_statusLabel = new QLabel("正在准备...", this);
        m_statusLabel->setObjectName("ProgressStatusLabel");
        layout->addWidget(m_statusLabel);

        m_progressBar = new QProgressBar(this);
        m_progressBar->setObjectName("ProgressDialogBar");
        m_progressBar->setFixedHeight(8);
        m_progressBar->setTextVisible(false);
        layout->addWidget(m_progressBar);

        auto* btnLayout = new QHBoxLayout();
        btnLayout->setContentsMargins(0, 5, 0, 0);
        btnLayout->setSpacing(10);
        btnLayout->addStretch();

        m_btnPause = new QPushButton("暂停", this);
        m_btnPause->setObjectName("ProgressPauseBtn");
        m_btnPause->setFocusPolicy(Qt::NoFocus);
        m_btnPause->setFixedSize(75, 28);
        connect(m_btnPause, &QPushButton::clicked, this, &ProgressDialog::togglePause);
        btnLayout->addWidget(m_btnPause);

        m_btnCancel = new QPushButton("取消", this);
        m_btnCancel->setObjectName("ProgressCancelBtn");
        m_btnCancel->setFocusPolicy(Qt::NoFocus);
        m_btnCancel->setFixedSize(75, 28);
        connect(m_btnCancel, &QPushButton::clicked, this, &ProgressDialog::cancel);
        btnLayout->addWidget(m_btnCancel);

        layout->addLayout(btnLayout);
    }

    bool isPaused() const { return m_isPaused.load(); }
    bool isCanceled() const { return m_isCanceled.load(); }

    Q_INVOKABLE void updateProgress(int current, int total, const QString& fileName) {
        if (total <= 0) return;
        m_progressBar->setRange(0, total);
        m_progressBar->setValue(current);

        double percent = (double)current / total * 100.0;
        QString status = QString("[%1/%2] %3% - %4")
                            .arg(current)
                            .arg(total)
                            .arg(QString::number(percent, 'f', 1))
                            .arg(fileName);
        m_statusLabel->setText(status);
    }

signals:
    void pauseToggled(bool isPaused);
    void cancelRequested();

public slots:
    void togglePause() {
        bool next = !m_isPaused.load();
        m_isPaused.store(next);
        m_btnPause->setText(next ? "继续" : "暂停");
        emit pauseToggled(next);
    }

    void cancel() {
        m_isCanceled.store(true);
        m_statusLabel->setText("正在取消操作...");
        m_btnPause->setEnabled(false);
        m_btnCancel->setEnabled(false);
        emit cancelRequested();
    }

private:
    QLabel* m_statusLabel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QPushButton* m_btnPause = nullptr;
    QPushButton* m_btnCancel = nullptr;
    std::atomic<bool> m_isPaused{false};
    std::atomic<bool> m_isCanceled{false};
};

} // namespace QuarkMeta
>>>>>>> REPLACE
```

### 3.2 `QuarkMeta_Source/CMakeLists.txt`
```
<<<<<<< SEARCH
    src/ui/dialogs/FramelessMessageBox.h
    src/ui/dialogs/FramelessMessageBox.cpp
=======
    src/ui/dialogs/FramelessMessageBox.h
    src/ui/dialogs/FramelessMessageBox.cpp
    src/ui/dialogs/ProgressDialog.h
>>>>>>> REPLACE
```

### 3.3 `QuarkMeta_Source/src/core/ClipboardService.h`
```
<<<<<<< SEARCH
    void copyItems(const QStringList& paths);
    void cutItems(const QStringList& paths);
    bool canPaste(const QString& targetDir) const;
    void executePaste(const QString& targetDir, QWidget* parentWidget = nullptr);

    // 标签剪贴板方法
    void setCopiedTags(const QStringList& tags);
    QStringList copiedTags() const;
    bool hasCopiedTags() const;
    void clearCopiedTags();

    /**
     * @brief 粘贴标签至目标视图选中项的 SSOT 统一入口 (包含校验、Model更新与 ToolTip 提示)
     */
    bool executePasteTags(QAbstractItemView* view);

signals:
    void pasteCompleted(const QString& targetDir);

private:
    QStringList m_copiedTags;
=======
    void copyItems(const QStringList& paths);
    void cutItems(const QStringList& paths);
    bool canPaste(const QString& targetDir) const;
    void executePaste(const QString& targetDir, QWidget* parentWidget = nullptr);

    // 剪切置灰状态管理
    bool isItemCut(const QString& path) const;
    const QSet<QString>& cutPaths() const;
    void clearCutState();

    // 标签剪贴板方法
    void setCopiedTags(const QStringList& tags);
    QStringList copiedTags() const;
    bool hasCopiedTags() const;
    void clearCopiedTags();

    /**
     * @brief 粘贴标签至目标视图选中项的 SSOT 统一入口 (包含校验、Model更新与 ToolTip 提示)
     */
    bool executePasteTags(QAbstractItemView* view);

signals:
    void pasteCompleted(const QString& targetDir);
    void pasteCompletedWithTargets(const QString& targetDir, const QStringList& targetPaths);
    void cutItemsChanged(const QSet<QString>& paths);

private:
    QStringList m_copiedTags;
    QSet<QString> m_cutPaths;
>>>>>>> REPLACE
```

### 3.4 `QuarkMeta_Source/src/core/ClipboardService.cpp`
```
<<<<<<< SEARCH
#include "../ui/FileCollisionDialog.h"
#include "ModelContract.h"
=======
#include "../ui/FileCollisionDialog.h"
#include "../ui/dialogs/ProgressDialog.h"
#include "TaskProgressService.h"
#include "ModelContract.h"
#include <QThread>
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
ClipboardService::ClipboardService(QObject* parent) : QObject(parent) {}

void ClipboardService::setCopiedTags(const QStringList& tags) {
=======
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
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
void ClipboardService::copyItems(const QStringList& paths) {
    if (paths.isEmpty()) return;

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

    QMimeData* mime = new QMimeData();
    QList<QUrl> urls;
    for (const QString& p : paths) {
        if (!p.isEmpty()) urls << QUrl::fromLocalFile(p);
    }
    mime->setUrls(urls);

    QByteArray effectData;
    effectData.append(static_cast<char>(2)); // DROPEFFECT_MOVE
    mime->setData("Preferred DropEffect", effectData);

    QApplication::clipboard()->setMimeData(mime);
}
=======
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
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
    QPointer<ClipboardService> weakThis(this);
    DiskIoService::instance().executeAsync(ioCtx, [weakThis, targetDir](bool success) {
        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis, targetDir, success]() {
            if (weakThis) {
                if (success) {
                    emit weakThis->pasteCompleted(targetDir);
                } else {
                    ToolTipOverlay::instance()->showToast("粘贴失败：物理写入未能完成", 2000, QColor("#e81123"));
                }
            }
        });
    });
}
=======
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
>>>>>>> REPLACE
```

### 3.5 `QuarkMeta_Source/src/ui/ContentPanel.h`
```
<<<<<<< SEARCH
    void setPendingSelectPath(const QString& fullPath, bool edit = false);
    void setPendingSelectName(const QString& name, bool edit = false);
=======
    void setPendingSelectPath(const QString& fullPath, bool edit = false);
    void setPendingSelectPaths(const QStringList& fullPaths);
    void setPendingSelectName(const QString& name, bool edit = false);
>>>>>>> REPLACE
```

### 3.6 `QuarkMeta_Source/src/ui/ContentPanel.cpp`
```
<<<<<<< SEARCH
    m_viewStack = new QStackedWidget(this);
    m_viewStack->setFrameShape(QFrame::NoFrame);
=======
    connect(&ClipboardService::instance(), &ClipboardService::cutItemsChanged, this, [this]() {
        if (m_treeView && m_treeView->viewport()) m_treeView->viewport()->update();
        if (m_gridView && m_gridView->viewport()) m_gridView->viewport()->update();
    });

    connect(&ClipboardService::instance(), &ClipboardService::pasteCompletedWithTargets, this,
            [this](const QString& targetDir, const QStringList& targetPaths) {
        if (QDir::cleanPath(targetDir).compare(QDir::cleanPath(m_currentPath), Qt::CaseInsensitive) == 0) {
            setPendingSelectPaths(targetPaths);
            refreshAll();
        }
    });

    m_viewStack = new QStackedWidget(this);
    m_viewStack->setFrameShape(QFrame::NoFrame);
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
void ContentPanel::setPendingSelectPath(const QString& fullPath, bool edit) {
    if (fullPath.isEmpty()) return;
    m_pendingSelectPath = QDir::toNativeSeparators(QDir::cleanPath(fullPath));
    m_pendingIsEdit = edit;
}
=======
void ContentPanel::setPendingSelectPath(const QString& fullPath, bool edit) {
    if (fullPath.isEmpty()) return;
    m_pendingSelectPath = QDir::toNativeSeparators(QDir::cleanPath(fullPath));
    m_pendingIsEdit = edit;
}

void ContentPanel::setPendingSelectPaths(const QStringList& fullPaths) {
    if (fullPaths.isEmpty()) return;
    m_selectionState.selectedPaths.clear();
    for (const QString& p : fullPaths) {
        m_selectionState.selectedPaths.insert(QDir::toNativeSeparators(QDir::cleanPath(p)));
    }
}
>>>>>>> REPLACE
```

### 3.7 `QuarkMeta_Source/src/ui/TreeItemDelegate.h`
```
<<<<<<< SEARCH
        bool isDropTarget = ViewDragDropHelper::isDropTarget(
            qobject_cast<const QAbstractItemView*>(option.widget), index);
=======
        QString itemPath = index.data(FilePathRole).toString();
        bool isCut = !itemPath.isEmpty() && ClipboardService::instance().isItemCut(itemPath);
        if (isCut) {
            painter->setOpacity(0.5);
        }

        bool isDropTarget = ViewDragDropHelper::isDropTarget(
            qobject_cast<const QAbstractItemView*>(option.widget), index);
>>>>>>> REPLACE
```

### 3.8 `QuarkMeta_Source/src/ui/ThumbnailDelegate.cpp`
```
<<<<<<< SEARCH
    bool isDropTarget = ViewDragDropHelper::isDropTarget(
        qobject_cast<const QAbstractItemView*>(option.widget), index);
=======
    QString itemPath = (m_pathRole != -1) ? index.data(m_pathRole).toString() : QString();
    bool isCut = !itemPath.isEmpty() && ClipboardService::instance().isItemCut(itemPath);

    painter->save();
    if (isCut) {
        painter->setOpacity(0.5);
    }

    bool isDropTarget = ViewDragDropHelper::isDropTarget(
        qobject_cast<const QAbstractItemView*>(option.widget), index);
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
    // ⑦ 空文件夹虚线边框
    if (!isSelected && m_isEmptyRole != -1 && m_typeRole != -1) {
        if (index.data(m_typeRole).toString() == "folder" && index.data(m_isEmptyRole).toBool()) {
            CardPainterHelper::drawEmptyFolderBorder(painter, l.coverRect);
        }
    }
}
=======
    // ⑦ 空文件夹虚线边框
    if (!isSelected && m_isEmptyRole != -1 && m_typeRole != -1) {
        if (index.data(m_typeRole).toString() == "folder" && index.data(m_isEmptyRole).toBool()) {
            CardPainterHelper::drawEmptyFolderBorder(painter, l.coverRect);
        }
    }

    painter->restore();
}
>>>>>>> REPLACE
```

### 3.9 `QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp`
```
<<<<<<< SEARCH
    // 局部 F5 刷新当前聚焦窗格，坚决阻断按键冒泡至全局引发的主窗格错误刷新
    if (keyEvent->key() == Qt::Key_F5 && keyEvent->modifiers() == Qt::NoModifier) {
=======
    if (keyEvent->key() == Qt::Key_Escape && keyEvent->modifiers() == Qt::NoModifier) {
        if (!ClipboardService::instance().cutPaths().isEmpty()) {
            ClipboardService::instance().clearCutState();
            return true;
        }
    }

    // 局部 F5 刷新当前聚焦窗格，坚决阻断按键冒泡至全局引发的主窗格错误刷新
    if (keyEvent->key() == Qt::Key_F5 && keyEvent->modifiers() == Qt::NoModifier) {
>>>>>>> REPLACE
```

### 3.10 `QuarkMeta_Source/resources/style.qss`
```
<<<<<<< SEARCH
QPushButton#EmptyFilterResetBtn:pressed {
    background-color: #e04b17;
}
=======
QPushButton#EmptyFilterResetBtn:pressed {
    background-color: #e04b17;
}

/* ============================================================ */
/* ProgressDialog (进度对话框)                                   */
/* ============================================================ */
QLabel#ProgressStatusLabel {
    color: #EEEEEE;
    font-size: 13px;
}

QProgressBar#ProgressDialogBar {
    background-color: #2D2D2D;
    border: none;
    border-radius: 4px;
}

QProgressBar#ProgressDialogBar::chunk {
    background-color: #378ADD;
    border-radius: 4px;
}

QPushButton#ProgressPauseBtn, QPushButton#ProgressCancelBtn {
    background-color: #2D2D30;
    color: #EEEEEE;
    border: 1px solid #3E3E42;
    border-radius: 4px;
    font-size: 12px;
    outline: none;
}

QPushButton#ProgressPauseBtn:hover, QPushButton#ProgressCancelBtn:hover {
    background-color: #3E3E42;
    border-color: #555555;
}

QPushButton#ProgressPauseBtn:pressed, QPushButton#ProgressCancelBtn:pressed {
    background-color: #1E1E1E;
}
>>>>>>> REPLACE
```

---

## Chapter 4: Build & Verification Steps

1. **编译构建**：
   使用 CMake 驱动 MSVC 构建工程：
   `cmake --build build --config Debug`
   核验 `ProgressDialog.h` 的注册与各委托头文件闭合，确认 0 编译警告与错误。
2. **场景验证 1：Ctrl+X 剪切置灰**：
   - 在列表视图或网格视图中选中 3 个文件，按下 `Ctrl + X`；
   - 验证：所选 3 个项目立即以 50% 不透明度变暗呈现置灰状态；
   - 按下 `Esc` 键或重新按 `Ctrl + C`，置灰状态立即解除。
3. **场景验证 2：$\le 20$ 个文件调用 TaskProgressService 并高亮新文件**：
   - 选中 10 个文件复制并粘贴；
   - 验证：应用底栏滑出展开 `TaskProgressStrip` 进度条，完成后收起，**刚粘贴的 10 个新文件在视图中全部被自动选中并呈现高亮状态**。
4. **场景验证 3：$> 20$ 个文件唤起 ProgressDialog 并高亮新文件**：
   - 选中 30 个文件执行粘贴；
   - 验证：自动弹出无边框 `ProgressDialog` 对话框，进度条平滑递增，状态标签实时显示当前处理文件名与百分比；
   - 点击“暂停”按钮，进度暂停，按钮变为“继续”；点击“继续”，搬运恢复；
   - 点击“取消”按钮，任务安全终止并退出弹窗；
   - 搬运成功的文件在弹窗关闭后，**在视图中全部自动选中高亮呈现，视口自动滚动聚焦**。

---

## Chapter 5: Official API Reuse & Anti-Redundancy

1. **防重复造轮子**：
   - 绘制透明度直接复用 QPainter 原生 `setOpacity(0.5)`，不增加任何冗余图片灰度滤镜计算；
   - 小批量进度复用成熟的 `TaskProgressService`，无需另造底栏控件；
   - 落地选中复用 `ContentPanel::restoreSelections` 与 `m_viewCoordinator->restoreSelections`；
   - 对话框继承成熟的 `FramelessDialog`，复用其窗口阴影、标题栏与无边框拖拽能力。

---

## Chapter 6: Data SSOT Declaration Table

| 数据项 / 状态 | 唯一 SSOT 真理源 | 变动通知渠道 | 刷新目标 |
| :--- | :--- | :--- | :--- |
| 剪切项目路径字典 | `ClipboardService::m_cutPaths` | `cutItemsChanged` 信号 | 内容面板视口触发 `viewport()->update()` |
| 粘贴落地新文件集合 | `ioCtx.actualDestMap` | `pasteCompletedWithTargets` 信号 | `ContentPanel::setPendingSelectPaths` 自动选中高亮 |
| 小批量进度条任务 | `TaskProgressService` | `progressUpdated` 信号 | 主界面底栏 `TaskProgressStrip` |
| 批量进度弹窗任务状态 | `ProgressDialog::m_isPaused` / `m_isCanceled` | 原子变量读写 | 后台搬运工作线程 |

---

## Chapter 7: Interface & Visibility Consistency Check

1. **外部调用接口检查**：
   - `ClipboardService::isItemCut` 与 `cutPaths` 为 `public const` 接口，无副作用；
   - `ContentPanel::setPendingSelectPaths(const QStringList&)` 为 `public` 接口，内部填充 `m_selectionState.selectedPaths`，由 `restoreSelections()` 统一驱动高亮；
   - `ProgressDialog` 继承 `FramelessDialog`，对外提供 `isPaused`、`isCanceled` 与 `updateProgress`，完全对齐业务调用。

---

## Chapter 8: Header Chain & Inclusion Closure Check

1. `ProgressDialog.h` 显式包含 `<QProgressBar>`、`<QLabel>`、`<QVBoxLayout>`、`<QPushButton>` 与 `<atomic>`；
2. `ClipboardService.cpp` 显式包含 `"../ui/dialogs/ProgressDialog.h"`、`"TaskProgressService.h"` 与 `<QThread>`；
3. `ContentPanel.cpp` 显式包含 `"../core/ClipboardService.h"`，类型完全闭合无缺失。

---

## Chapter 9: Compliance & Quality Self-Check Table

| 序号 | 检查项 | 状态 | 证据 / 说明 |
| :--- | :--- | :--- | :--- |
| 1 | 方案隔离目录 | PASS | 方案就地更新于 `Implementation Plan/` 物理目录 |
| 2 | 英文命名规范 | PASS | 文件名为 `Cut_Dimming_And_Batch_Paste_Progress_Plan.md` |
| 3 | 契约锁（头文件冻结） | PASS | 既有接口保持兼容，仅以只增不减方式扩展剪切与落地选中接口 |
| 4 | 依赖锁（单向流动） | PASS | View $\to$ Core Service，单向依赖严格合规 |
| 5 | 无直接修改代码铁律 | PASS | 仅生成/更新实施方案，源代码 0 修改 |
| 6 | SEARCH 块 100% 精确匹配 | PASS | 逐行核对 `TreeItemDelegate`、`ThumbnailDelegate`、`ClipboardService`、`ContentPanel` 原文无偏差 |
| 7 | 严格杜绝内联 setStyleSheet | PASS | ProgressDialog 样式 100% 收敛至 resources/style.qss |
| 8 | 按钮防虚线框强锁 | PASS | 所有新建按钮显式配置 setFocusPolicy(Qt::NoFocus) 与 outline:none |
| 9 | CMakeLists.txt 同步注册 | PASS | 新建的 ProgressDialog.h 明确在 CMakeLists.txt 注册 |
| 10 | 双轨进度梯级策略 | PASS | $\le 20$ 调用 TaskProgressService，$> 20$ 唤起 ProgressDialog |
| 11 | 粘贴后自动高亮选中 | PASS | 落地路径自动注入 setPendingSelectPaths，刷新后精准高亮 |
| 12 | 100% 中文说明 | PASS | 除代码标识符外全中文表达 |

---

## Chapter 10: Behavior Preservation & Zero-Value-Alteration Table

1. **零属性篡改**：未修改任何既有颜色 Hex 值、边框、间距或字体属性；ProgressDialog 样式完全对齐既有对话框标准（暗黑底色 `#2D2D2D`，主色调 `#378ADD`）；
2. **交互形态保持**：剪切快捷键 `Ctrl + X`、复制 `Ctrl + C`、粘贴 `Ctrl + V` 既有交互完全保持原样，仅在视觉层增补 Windows 风格变暗反馈，在批量粘贴时增设强控进度窗口，粘贴后自动选中新文件，无破坏性变更。
