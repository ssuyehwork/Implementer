# 回收站快捷键永久删除与安全防护实施方案 (RecycleBin Delete Shortcuts Plan)

**方案版本**：v2.0
**日期**：2026-11-06  
**关联 Memories.md 条目**：`[新增记录] 内容面板回收站快捷键永久删除与防误触安全校验设计理念 (Recycle Bin Delete Shortcuts)`

---

## 已读物理证据清单（第 1 关：Gate 1）

| 文件 | 确认已读 | 关键物理签名 |
|---|---|---|
| `QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp` | ✅ | `keyEvent->key() == Qt::Key_Delete || ((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == Qt::Key_Space)`（第 612-619 行） |
| `QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp` | ✅ | `#include "../../core/PermanentDeleteService.h"` 已在第 11 行显式包含；`#include "../ToolTipOverlay.h"` 已在第 6 行显式包含 |
| `QuarkMeta_Source/src/ui/ContentPanel.h` | ✅ | `QString getCurrentCategoryType() const;`（第 121 行，public）及 `QString currentPath() const;`（第 111 行，public） |
| `QuarkMeta_Source/src/core/PermanentDeleteService.h` | ✅ | `bool execute(const QStringList& paths, QWidget* parentWidget = nullptr, bool isSecureShred = true);`（第 23 行） |
| `QuarkMeta_Source/src/ui/ToolTipOverlay.h` | ✅ | `void showToast(const QString& text, int timeout = 1500, const QColor& borderColor = QColor("#2ecc71"), QWidget* targetWidget = nullptr);`（第 51 行） |

**防重复造轮子与根因检索记录**：
- `ContentKeyHandler.cpp` 为内容面板所有键盘事件分发的核心入口。在该处现有的删除键位处理分支中加入回收站视图判定与选中项状态校验，即可统一拦截分流，完全复用官方既有服务 `PermanentDeleteService` 和既有提示组件 `ToolTipOverlay`，无需另起炉灶。

---

## Chapter 1: Overview（概述与根因分析）

### 问题根因
现有的 `ContentKeyHandler.cpp` 中，删除快捷键处理并未判断当前是否处于“回收站视图”：
1. 遇到 `Shift+Delete` 时执行永久删除；
2. 遇到 `Delete` 或 `Ctrl+Space` 时，无论是否在回收站，均调用 `TrashService::instance().moveToTrash(...)`，导致对回收站中的已废弃项目再次执行“移入回收站”操作，逻辑脱节且无效；
3. 用户在回收站中期望使用 `Del`、`Shift+Del` 或 `Ctrl+Space` 快捷键执行“永久删除”；
4. 若在回收站未选中项目时误触快捷键，若直接触发清空操作存在严重误操作隐患，因此必须增加安全防护拦截机制。

### 设计目标
1. 在 `ContentKeyHandler.cpp` 的删除键位处理逻辑中，增加回收站视图感知守卫；
2. **在回收站视图下**：
   - 只要按下 `Del`、`Shift+Del` 或 `Ctrl+Space` 任意一个快捷键：
     - 若当前**有选中项**，统一执行**永久删除选中项**（调用 `PermanentDeleteService::instance().execute(selectedPaths, m_panel)`）；
     - 若当前**未选中项目**（空选或空白处），执行**防误触拦截**，禁止清空回收站，并通过统一的暗黑风格轻提示 `ToolTipOverlay::instance()->showToast("请先选择要永久删除的项目", 2000, QColor("#e81123"), m_panel)` 提示用户；
3. **在常规普通目录下**：
   - 保持既有行为 100% 零篡改（`Del` / `Ctrl+Space` 移入回收站，`Shift+Del` 永久删除）；
4. 严格遵循 Clean Architecture 与 SSOT，保持代码内聚与单一变量修改。

---

## Chapter 2: Modified Files List（影响文件清单）

| 操作类型 | 文件相对路径 |
|---|---|
| **修改** | `QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp` |
| 不涉及 | `CMakeLists.txt`（无文件增删） |

---

## Chapter 3: Detailed Line-by-Line Changes（精准替换块）

### 3.1 `QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp` — 删除键位增加回收站视图感知分流与防误触拦截

```
<<<<<<< SEARCH
    if (keyEvent->key() == Qt::Key_Delete || ((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == Qt::Key_Space)) {
        if (keyEvent->modifiers() & Qt::ShiftModifier) {
            PermanentDeleteService::instance().execute(m_panel->getSelectedPaths(), m_panel);
        } else {
            TrashService::instance().moveToTrash(m_panel->getSelectedPaths(), m_panel);
        }
        return true;
    }
=======
    if (keyEvent->key() == Qt::Key_Delete || ((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == Qt::Key_Space)) {
        bool isTrashView = (m_panel->getCurrentCategoryType() == "trash" || m_panel->currentPath() == "trash://");
        if (isTrashView) {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            if (!selectedPaths.isEmpty()) {
                PermanentDeleteService::instance().execute(selectedPaths, m_panel);
            } else {
                ToolTipOverlay::instance()->showToast("请先选择要永久删除的项目", 2000, QColor("#e81123"), m_panel);
            }
        } else {
            if (keyEvent->modifiers() & Qt::ShiftModifier) {
                PermanentDeleteService::instance().execute(m_panel->getSelectedPaths(), m_panel);
            } else {
                TrashService::instance().moveToTrash(m_panel->getSelectedPaths(), m_panel);
            }
        }
        return true;
    }
>>>>>>> REPLACE
```

---

## Chapter 4: Build & Verification Steps（构建与运行时验收标准）

### 编译验证步骤
```bash
cmake --build . --config Release --target QuarkMeta
```
预期：**0 errors, 0 warnings**

### 运行时验收标准

| 场景 | 操作步骤 | 预期结果 |
|---|---|---|
| 回收站有选中项按 Del | 选中 1 个或多个回收站项，按 `Del` | 弹出永久删除二次确认弹窗，确认后物理销毁所选项，面板原地刷新 |
| 回收站有选中项按 Shift+Del | 选中 1 个或多个回收站项，按 `Shift+Del` | 弹出永久删除二次确认弹窗，确认后物理销毁所选项，面板原地刷新 |
| 回收站有选中项按 Ctrl+Space | 选中 1 个或多个回收站项，按 `Ctrl+Space` | 弹出永久删除二次确认弹窗，确认后物理销毁所选项，面板原地刷新 |
| 回收站无选中项按 Del | 点击回收站空白处（未选中任何项），按 `Del` | 弹出 Toast 轻提示“请先选择要永久删除的项目”，不触发清空，防止误操作 |
| 回收站无选中项按 Shift+Del | 未选中任何项，按 `Shift+Del` | 弹出 Toast 轻提示“请先选择要永久删除的项目”，不触发清空，防止误操作 |
| 回收站无选中项按 Ctrl+Space | 未选中任何项，按 `Ctrl+Space` | 弹出 Toast 轻提示“请先选择要永久删除的项目”，不触发清空，防止误操作 |
| 普通文件夹按 Del / Ctrl+Space | 在正常目录选中文件，按 `Del` 或 `Ctrl+Space` | 正常移入回收站，原有功能不受任何影响 |
| 普通文件夹按 Shift+Del | 在正常目录选中文件，按 `Shift+Del` | 正常执行永久删除，原有功能不受任何影响 |

---

## Chapter 5: Official API Reuse & Anti-Redundancy（官方 API 复用与防另起炉灶）

| 功能 | 复用的官方 SSOT 入口 | 位置 |
|---|---|---|
| 永久删除选中项 | `PermanentDeleteService::instance().execute(...)` | `PermanentDeleteService.h` 官方接口 |
| 移入回收站 | `TrashService::instance().moveToTrash(...)` | `TrashService.h` 官方接口 |
| 回收站状态识别 | `m_panel->getCurrentCategoryType()` / `m_panel->currentPath()` | `ContentPanel` 官方既有接口 |
| 安全提示气泡 | `ToolTipOverlay::instance()->showToast(...)` | `ToolTipOverlay.h` 官方接口 |

---

## Chapter 6: Data SSOT Declaration Table（数据单一事实来源声明表）

| 数据项 | SSOT 来源 | 派生/消费点 | 同步方向 |
|---|---|---|---|
| 回收站视图状态 | `m_panel->getCurrentCategoryType()` / `m_panel->currentPath()` | `ContentKeyHandler::handleKeyPress` | 只读判定 |
| 当前选中项路径集 | `m_panel->getSelectedPaths()` | `PermanentDeleteService::execute` | 只读传递 |

---

## Chapter 7: Interface & Visibility Consistency Check（接口签名与访问级别一致性核查）

| 调用 | 物理签名位置 | 访问级别 |
|---|---|---|
| `m_panel->getCurrentCategoryType()` | `ContentPanel.h` 第 121 行，`public` | ✅ public |
| `m_panel->currentPath()` | `ContentPanel.h` 第 111 行，`public` | ✅ public |
| `m_panel->getSelectedPaths()` | `ContentPanel.h` 第 199 行，`public` | ✅ public |
| `PermanentDeleteService::instance().execute(...)` | `PermanentDeleteService.h` 第 23 行，`public` | ✅ public |
| `ToolTipOverlay::instance()->showToast(...)` | `ToolTipOverlay.h` 第 51 行，`public` | ✅ public |

---

## Chapter 8: Header Chain & Inclusion Closure Check（头文件包含链与类型完整性检查）

| 类型/API | `#include` 状态 |
|---|---|
| `PermanentDeleteService` | `#include "../../core/PermanentDeleteService.h"` ✅（`ContentKeyHandler.cpp` 第 11 行已有） |
| `TrashService` | `#include "../../core/TrashService.h"` ✅（`ContentKeyHandler.cpp` 第 10 行已有） |
| `ContentPanel` | `#include "../ContentPanel.h"` ✅（`ContentKeyHandler.cpp` 第 2 行已有） |
| `ToolTipOverlay` | `#include "../ToolTipOverlay.h"` ✅（`ContentKeyHandler.cpp` 第 6 行已有） |

---

## Chapter 9: Compliance & Quality Self-Check Table（合规与质量核对表）

| 编号 | 检查项 | 状态 | 位置/说明 |
|---|---|---|---|
| 1 | 方案已通过第 0 关需求确认 | ✅ 满足 | 用户明确表示“达成共识” |
| 2 | 物理读取所有涉及文件（Gate 1） | ✅ 满足 | 见已读物理证据清单 |
| 3 | SEARCH 块原文真实存在且唯一匹配 | ✅ 满足 | 通过脚本唯一校验 |
| 4 | 新增接口声明、定义、调用点签名一致 | ✅ 满足 | 纯内部逻辑分流，无新增公开 API |
| 5 | 被修改的 `#include` 间接包含链闭合 | ✅ 满足 | 头文件依赖完整闭合 |
| 6 | 新建/删除文件已在 CMakeLists.txt 同步 | ✅ 不涉及 | 纯内部逻辑修改，无文件增删 |
| 7 | 跨类调用访问级别均为 public | ✅ 满足 | 全量核查通过 |
| 8 | 方案没有触及方案范围外的文件 | ✅ 满足 | 仅涉及 1 个目标文件 `ContentKeyHandler.cpp` |
| 9 | 未采用笨拙浪费计算资源的手法 | ✅ 满足 | O(1) 逻辑分支分流，零冗余开销 |
| 10 | 代码中无打补丁行为 | ✅ 满足 | 直接在既有键盘映射核心方法中进行上下文感知分支补全 |
| 11 | 遵循 Memories.md 规范 | ✅ 满足 | 已同步写入核心记忆真理源 |
| 12 | ToolTipOverlay 不追随鼠标 | ✅ 满足 | 使用 `showToast` 固定在 `m_panel` 顶部居中，不追随鼠标位置 |
| 13 | 100% 语义化矢量图标 | ✅ 不涉及 | 无菜单 UI 变更 |
| 14 | 异步回调按值捕获 | ✅ 不涉及 | 纯同步派发 |
| 15 | 无 Lambda 循环引用 | ✅ 不涉及 | 无 Lambda |
| 16 | 后台线程 UI 操作回归主线程 | ✅ 满足 | 键盘事件在 GUI 主线程分发 |
| 17 | Qt 信号槽使用 C++11 函数指针语法 | ✅ 不涉及 | 无信号槽变更 |
| 18 | 对外 `.h` 签名冻结 | ✅ 满足 | 零修改 `.h` 文件，100% 冻结 |

---

## Chapter 10: Behavior Preservation & Zero-Value-Alteration Table（行为零篡改与数值零篡改声明表）

### 行为零篡改声明

| 既有行为 | 本方案影响 | 结论 |
|---|---|---|
| 普通物理目录下的 `Del` / `Ctrl+Space` 移入回收站 | `isTrashView` 为 false，完全走原逻辑 | ✅ 100% 原样保留 |
| 普通物理目录下的 `Shift+Del` 永久删除 | `isTrashView` 为 false，完全走原逻辑 | ✅ 100% 原样保留 |
| 复制、剪切、粘贴、重命名等其他快捷键 | 完全不触及 | ✅ 100% 原样保留 |

### 数值零篡改声明

| 数值类型 | 值 | 说明 |
|---|---|---|
| 按键枚举 | `Qt::Key_Delete`、`Qt::Key_Space`、`Qt::ControlModifier`、`Qt::ShiftModifier` | 100% 沿用既有按键映射 |
| 提示框颜色 Hex | `#e81123` | 项目标准警示/错误红 |
| 提示时长 | `2000` ms | 项目标准 Toast 显示时长 |
