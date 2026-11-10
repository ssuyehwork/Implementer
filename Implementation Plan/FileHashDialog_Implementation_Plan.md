# 元数据面板哈希值弹窗化与界面布局恢复实施方案

## 1. 需求与设计概要
在元数据面板（MetaPanel）中，单选实体文件点击“查看哈希值”时，不再在物理路径下方挤占空间展开单行文本框（避免长哈希值无法自动换行截断显示），改为：
1. 异步计算完成后弹出专用的无边框深色对话框 `FileHashDialog`（继承自 `FramelessDialog`，标题为“文件哈希值”）。
2. 对话框内部展示：
   - 文件名（灰色弱化提示）与 SHA-256 完整哈希值；
   - 哈希值容器使用可自动换行或自适应多行文本框（如只读 `QTextEdit`），确保 64 位十六进制哈希值完整可视；
   - 底部提供“复制哈希值”按钮和“关闭”按钮；点击“复制哈希值”将 Hash 复制到系统剪贴板并给出 Toast 提示。
3. 元数据面板原有的按钮文案在点击与计算后始终恢复/保持为“查看哈希值”，彻底移除原先临时添加的 `m_hashEdit` 控件及其相关布局和 QSS 冗余，恢复原本清爽整洁的紧凑界面布局。
4. 多选文件（>= 2）场景严格保持调用 `FileCompareDialog` 对比功能不变。

---

## 2. 涉及文件清单
1. **新建：** `QuarkMeta_Source/src/ui/dialogs/FileHashDialog.h`
2. **修改：** `QuarkMeta_Source/CMakeLists.txt`（注册 `src/ui/dialogs/FileHashDialog.h`）
3. **修改：** `QuarkMeta_Source/src/ui/MetaPanel.h`（移除 `m_hashEdit` 成员变量声明）
4. **修改：** `QuarkMeta_Source/src/ui/MetaPanel.cpp`（包含 `FileHashDialog.h`，移除 `m_hashEdit` 相关代码并在计算完成时弹出 `FileHashDialog`）
5. **修改：** `QuarkMeta_Source/resources/style.qss`（添加 `FileHashDialog` 专属样式，清理 `QTextEdit#MetaHashEdit`）

---

## 3. 详细代码变更与 SEARCH / REPLACE

### 变更 1：新建 `QuarkMeta_Source/src/ui/dialogs/FileHashDialog.h`
创建新文件 `QuarkMeta_Source/src/ui/dialogs/FileHashDialog.h`，实现深色风格无边框哈希值弹窗。

### 变更 2：`QuarkMeta_Source/CMakeLists.txt`
```diff
<<<<<<< SEARCH
    src/ui/dialogs/ProgressDialog.h
    src/ui/CardLayoutEngine.h
=======
    src/ui/dialogs/ProgressDialog.h
    src/ui/dialogs/FileHashDialog.h
    src/ui/CardLayoutEngine.h
>>>>>>> REPLACE
```

### 变更 3：`QuarkMeta_Source/src/ui/MetaPanel.h`
```diff
<<<<<<< SEARCH
    QPushButton* m_btnComputeHash = nullptr;
    ElasticEdit* m_hashEdit = nullptr;
    std::shared_ptr<std::atomic<bool>> m_hashCancelToken;
=======
    QPushButton* m_btnComputeHash = nullptr;
    std::shared_ptr<std::atomic<bool>> m_hashCancelToken;
>>>>>>> REPLACE
```

### 变更 4：`QuarkMeta_Source/src/ui/MetaPanel.cpp`
包含 `FileHashDialog.h`：
```diff
<<<<<<< SEARCH
#include "dialogs/FileCompareDialog.h"
#include <QtConcurrent>
=======
#include "dialogs/FileCompareDialog.h"
#include "dialogs/FileHashDialog.h"
#include <QtConcurrent>
>>>>>>> REPLACE
```

移除 `m_hashEdit` 的创建：
```diff
<<<<<<< SEARCH
    pathL->addLayout(actionBtnL);

    // 单个项目哈希自适应展示框（默认隐藏，架构与 m_pathEdit 完全统一）
    m_hashEdit = new ElasticEdit(pathBox);
    m_hashEdit->setReadOnly(true);
    m_hashEdit->setObjectName("MetaHashEdit");
    m_hashEdit->hide();
    pathL->addWidget(m_hashEdit);

    m_containerLayout->addWidget(createCollapsibleSection("物理路径", pathBox, true));
=======
    pathL->addLayout(actionBtnL);

    m_containerLayout->addWidget(createCollapsibleSection("物理路径", pathBox, true));
>>>>>>> REPLACE
```

移除 `setSelectedPaths` 中的 `m_hashEdit` 清理：
```diff
<<<<<<< SEARCH
    // 切换选择时：安全取消正在计算的哈希任务，隐匿哈希框并复位按钮文案
    if (m_hashCancelToken) {
        m_hashCancelToken->store(true);
        m_hashCancelToken.reset();
    }
    if (m_hashEdit) {
        m_hashEdit->clear();
        m_hashEdit->hide();
    }
    if (m_btnComputeHash) {
        m_btnComputeHash->setText("查看哈希值");
    }
=======
    // 切换选择时：安全取消正在计算的哈希任务，复位按钮文案
    if (m_hashCancelToken) {
        m_hashCancelToken->store(true);
        m_hashCancelToken.reset();
    }
    if (m_btnComputeHash) {
        m_btnComputeHash->setText("查看哈希值");
    }
>>>>>>> REPLACE
```

重构 `onComputeHashClicked`：
```diff
<<<<<<< SEARCH
void MetaPanel::onComputeHashClicked() {
    if (m_selectedPaths.isEmpty()) return;

    // 若当前状态已计算完成，点击直接执行复制操作并弹出 Toast
    if (m_btnComputeHash && m_btnComputeHash->text() == "复制哈希值") {
        if (m_hashEdit) {
            QString h = m_hashEdit->toPlainText().trimmed();
            if (!h.isEmpty() && !h.startsWith("无法")) {
                QApplication::clipboard()->setText(h);
                ToolTipOverlay::instance()->showToast("已复制哈希值", 1200, QColor("#2ecc71"));
                return;
            }
        }
    }

    // 过滤出所有选中的非文件夹实体文件
    QStringList validFiles;
    for (const QString& p : m_selectedPaths) {
        if (!p.isEmpty() && !p.contains("://") && !QFileInfo(p).isDir()) {
            validFiles << p;
        }
    }

    if (validFiles.isEmpty()) {
        ToolTipOverlay::instance()->showToast("所选项目不包含可计算哈希的文件", 1500, QColor("#e81123"));
        return;
    }

    // 多选文件（>= 2）：弹出专用 FileCompareDialog
    if (validFiles.size() >= 2) {
        FileCompareDialog dlg(validFiles, this);
        dlg.exec();
        return;
    }

    // 单个文件：在下方 ElasticEdit 自适应框中异步计算并展示
    QString targetFile = validFiles.first();
    if (!m_hashEdit) return;

    m_hashEdit->show();
    m_hashEdit->setPlainText("计算中…");

    if (m_hashCancelToken) {
        m_hashCancelToken->store(true);
    }
    m_hashCancelToken = std::make_shared<std::atomic<bool>>(false);
    auto cancelToken = m_hashCancelToken;

    QPointer<MetaPanel> weakThis(this);
    (void)QtConcurrent::run([weakThis, targetFile, cancelToken]() {
        QString hash = DuplicateDetectorService::computeFullSha256(targetFile, cancelToken.get());
        if (cancelToken->load()) return;

        QMetaObject::invokeMethod(qApp, [weakThis, hash, cancelToken]() {
            if (!weakThis || cancelToken->load()) return;
            if (weakThis->m_hashEdit) {
                weakThis->m_hashEdit->setPlainText(hash.isEmpty() ? "无法读取" : hash);
                // 只有成功计算出哈希值，才将按钮文案切换为“复制哈希值”
                if (!hash.isEmpty() && weakThis->m_btnComputeHash) {
                    weakThis->m_btnComputeHash->setText("复制哈希值");
                }
            }
        });
    });
}
=======
void MetaPanel::onComputeHashClicked() {
    if (m_selectedPaths.isEmpty()) return;

    // 过滤出所有选中的非文件夹实体文件
    QStringList validFiles;
    for (const QString& p : m_selectedPaths) {
        if (!p.isEmpty() && !p.contains("://") && !QFileInfo(p).isDir()) {
            validFiles << p;
        }
    }

    if (validFiles.isEmpty()) {
        ToolTipOverlay::instance()->showToast("所选项目不包含可计算哈希的文件", 1500, QColor("#e81123"));
        return;
    }

    // 多选文件（>= 2）：弹出专用 FileCompareDialog
    if (validFiles.size() >= 2) {
        FileCompareDialog dlg(validFiles, this);
        dlg.exec();
        return;
    }

    // 单个文件：异步计算并在完成后弹出独立的 FileHashDialog 弹窗
    QString targetFile = validFiles.first();

    if (m_btnComputeHash) {
        m_btnComputeHash->setText("正在计算…");
    }

    if (m_hashCancelToken) {
        m_hashCancelToken->store(true);
    }
    m_hashCancelToken = std::make_shared<std::atomic<bool>>(false);
    auto cancelToken = m_hashCancelToken;

    QPointer<MetaPanel> weakThis(this);
    (void)QtConcurrent::run([weakThis, targetFile, cancelToken]() {
        QString hash = DuplicateDetectorService::computeFullSha256(targetFile, cancelToken.get());
        if (cancelToken->load()) return;

        QMetaObject::invokeMethod(qApp, [weakThis, targetFile, hash, cancelToken]() {
            if (!weakThis || cancelToken->load()) return;
            if (weakThis->m_btnComputeHash) {
                weakThis->m_btnComputeHash->setText("查看哈希值");
            }
            if (hash.isEmpty()) {
                ToolTipOverlay::instance()->showToast("无法读取或计算该文件哈希值", 2000, QColor("#e81123"));
                return;
            }
            FileHashDialog dlg(targetFile, hash, weakThis);
            dlg.exec();
        });
    });
}
>>>>>>> REPLACE
```

### 变更 5：`QuarkMeta_Source/resources/style.qss`
```diff
<<<<<<< SEARCH
QTextEdit#MetaPathEdit, QTextEdit#MetaHashEdit {
    background: #1E1E1E;
    border: 1px solid #2A2A2A;
    border-radius: 4px;
    padding: 4px 6px;
    font-size: 11px;
    color: #CCCCCC;
}
=======
QTextEdit#MetaPathEdit {
    background: #1E1E1E;
    border: 1px solid #2A2A2A;
    border-radius: 4px;
    padding: 4px 6px;
    font-size: 11px;
    color: #CCCCCC;
}
>>>>>>> REPLACE
```
