# 回收站快捷键永久删除与安全防护实施方案 (RecycleBin Delete Shortcuts Plan)

**方案版本**：v2.1
**日期**：2026-11-06  
**关联 Memories.md 条目**：`[新增记录] 内容面板回收站快捷键永久删除与防误触安全校验设计理念 (Recycle Bin Delete Shortcuts)`

---

## 已读物理证据清单（第 1 关：Gate 1）

| 文件 | 确认已读 | 关键物理签名 |
|---|---|---|
| `QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp` | ✅ | `keyEvent->key() == Qt::Key_Delete || ((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == Qt::Key_Space)`（第 612-619 行） |
| `QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp` | ✅ | `#include "../../core/PermanentDeleteService.h"` 已在第 11 行显式包含；`#include "../ToolTipOverlay.h"` 已在第 6 行显式包含 |
| `QuarkMeta_Source/src/ui/PanelMediator.cpp` | ✅ | `connect(shortcutController, &AppShortcutController::moveToTrashRequested, ...)`（第 280-288 行） |
| `QuarkMeta_Source/src/ui/ContentPanel.h` | ✅ | `QString getCurrentCategoryType() const;`（第 121 行，public）及 `QString currentPath() const;`（第 111 行，public） |
| `QuarkMeta_Source/src/core/PermanentDeleteService.h` | ✅ | `bool execute(const QStringList& paths, QWidget* parentWidget = nullptr, bool isSecureShred = true);`（第 23 行） |
| `QuarkMeta_Source/src/ui/ToolTipOverlay.h` | ✅ | `void showToast(const QString& text, int timeout = 1500, const QColor& borderColor = QColor("#2ecc71"), QWidget* targetWidget = nullptr);`（第 51 行） |

**防重复造轮子与根因检索记录**：
- Windows 系统级键盘钩子接管了全局 `Ctrl+Space`，并通过 `AppShortcutController::moveToTrashRequested` 派发至 `PanelMediator`。
- 本地视图键盘事件在 `ContentKeyHandler.cpp` 中分发处理。
- 根因分析：此前只在 `ContentKeyHandler.cpp` 中增加了回收站分支分流，但由于全局键盘钩子在按下 `Ctrl+Space` 时优先触发了 `AppShortcutController::moveToTrashRequested` 信号并在 `PanelMediator.cpp` 中直接调用 `TrashService::instance().moveToTrash(...)`，导致未选中项时不仅无法触发 `ContentKeyHandler` 的防护提示，且未针对回收站场景分流。
- 解决策略：在 `PanelMediator.cpp` 的 `moveToTrashRequested` 响应槽中同步补齐场景感知分流与防误触拦截逻辑，确保无论通过本地按键还是全局钩子触发 `Ctrl+Space`，均与 `Del` / `Shift+Del` 保持完全一致的回收站永久删除与未选中拦截行为。

---

## Chapter 1: Overview（概述与根因分析）

### 问题根因
1. `ContentKeyHandler.cpp` 处理视图内的 `Del`、`Shift+Del` 与 `Ctrl+Space` 按键；
2. 全局键盘钩子 `LocalShortcutInterceptor` 拦截了 `Ctrl+Space` 并通知 `AppShortcutController` 发出 `moveToTrashRequested` 信号；
3. `PanelMediator.cpp` 在接收到 `moveToTrashRequested` 信号时，缺乏回收站场景感知：若未选中任何项时，不会执行任何操作也没有弹出防误触提示；若在回收站有选中项时，调用了移入回收站而非永久删除。

### 设计目标
1. **在 `ContentKeyHandler.cpp` 中**：保持删除键位与组合键的回收站场景感知与未选中项拦截；
2. **在 `PanelMediator.cpp` 中**：
   - 引入 `#include "../core/PermanentDeleteService.h"`；
   - 在 `moveToTrashRequested` 槽函数中增加回收站判断：
     - 若当前处于回收站视图：
       - 若有选中项：调用 `PermanentDeleteService::instance().execute(sel, target)` 执行永久删除；
       - 若无选中项：调用 `ToolTipOverlay::instance()->showToast("请先选择要永久删除的项目", 2000, QColor("#e81123"), target)` 弹出拦截提示；
     - 若处于普通视图：
       - 若有选中项：保持原样调用 `TrashService::instance().moveToTrash(sel, target)`；
       - 若无选中项：不执行操作（保持原样）；
3. 严格遵循 Clean Architecture 与 SSOT，实现行为 100% 统一。

---

## Chapter 2: Modified Files List（影响文件清单）

| 操作类型 | 文件相对路径 |
|---|---|
| **修改** | `QuarkMeta_Source/src/ui/controllers/ContentKeyHandler.cpp` |
| **修改** | `QuarkMeta_Source/src/ui/PanelMediator.cpp` |
| 不涉及 | `CMakeLists.txt`（无文件增删） |

---

## Chapter 3: Detailed Line-by-Line Changes（精准替换块）

### 3.1 `QuarkMeta_Source/src/ui/PanelMediator.cpp` — 引入 PermanentDeleteService 头文件

```
<<<<<<< SEARCH
#include "../core/NavigationService.h"
#include "../core/TrashService.h"
=======
#include "../core/NavigationService.h"
#include "../core/TrashService.h"
#include "../core/PermanentDeleteService.h"
>>>>>>> REPLACE
```

### 3.2 `QuarkMeta_Source/src/ui/PanelMediator.cpp` — 全局 moveToTrashRequested 响应增加回收站场景与防误触拦截

```
<<<<<<< SEARCH
        connect(shortcutController, &AppShortcutController::moveToTrashRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                QStringList sel = target->getSelectedPaths();
                if (!sel.isEmpty()) {
                    TrashService::instance().moveToTrash(sel, target);
                }
            }
        });
=======
        connect(shortcutController, &AppShortcutController::moveToTrashRequested, this, [this, contentPanel]() {
            ContentPanel* target = m_activeContentPanel ? m_activeContentPanel.data() : contentPanel;
            if (target) {
                bool isTrashView = (target->getCurrentCategoryType() == "trash" || target->currentPath() == "trash://");
                QStringList sel = target->getSelectedPaths();
                if (isTrashView) {
                    if (!sel.isEmpty()) {
                        PermanentDeleteService::instance().execute(sel, target);
                    } else {
                        ToolTipOverlay::instance()->showToast("请先选择要永久删除的项目", 2000, QColor("#e81123"), target);
                    }
                } else {
                    if (!sel.isEmpty()) {
                        TrashService::instance().moveToTrash(sel, target);
                    }
                }
            }
        });
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
| 回收站状态识别 | `target->getCurrentCategoryType()` / `target->currentPath()` | `ContentPanel` 官方既有接口 |
| 安全提示气泡 | `ToolTipOverlay::instance()->showToast(...)` | `ToolTipOverlay.h` 官方接口 |

---

## Chapter 6: Data SSOT Declaration Table（数据单一事实来源声明表）

| 数据项 | SSOT 来源 | 派生/消费点 | 同步方向 |
|---|---|---|---|
| 回收站视图状态 | `target->getCurrentCategoryType()` / `target->currentPath()` | `PanelMediator` / `ContentKeyHandler` | 只读判定 |
| 当前选中项路径集 | `target->getSelectedPaths()` | `PermanentDeleteService::execute` | 只读传递 |

---

## Chapter 7: Interface & Visibility Consistency Check（接口签名与访问级别一致性核查）

| 调用 | 物理签名位置 | 访问级别 |
|---|---|---|
| `target->getCurrentCategoryType()` | `ContentPanel.h` 第 121 行，`public` | ✅ public |
| `target->currentPath()` | `ContentPanel.h` 第 111 行，`public` | ✅ public |
| `target->getSelectedPaths()` | `ContentPanel.h` 第 199 行，`public` | ✅ public |
| `PermanentDeleteService::instance().execute(...)` | `PermanentDeleteService.h` 第 23 行，`public` | ✅ public |
| `ToolTipOverlay::instance()->showToast(...)` | `ToolTipOverlay.h` 第 51 行，`public` | ✅ public |

---

## Chapter 8: Header Chain & Inclusion Closure Check（头文件包含链与类型完整性检查）

| 类型/API | `#include` 状态 |
|---|---|
| `PermanentDeleteService` | `#include "../core/PermanentDeleteService.h"` ✅（`PanelMediator.cpp` 显式包含） |
| `TrashService` | `#include "../core/TrashService.h"` ✅（`PanelMediator.cpp` 已有） |
| `ToolTipOverlay` | `#include "ToolTipOverlay.h"` ✅（`PanelMediator.cpp` 已有） |
| `ContentPanel` | `#include "ContentPanel.h"` ✅（`PanelMediator.cpp` 已有） |

---

## Chapter 9: Compliance & Quality Self-Check Table（合规与质量核对表）

| 编号 | 检查项 | 状态 | 位置/说明 |
|---|---|---|---|
| 1 | 方案已通过第 0 关需求确认 | ✅ 满足 | 用户明确要求 Ctrl+Space 场景感知分流 |
| 2 | 物理读取所有涉及文件（Gate 1） | ✅ 满足 | 见已读物理证据清单 |
| 3 | SEARCH 块原文真实存在且唯一匹配 | ✅ 满足 | 经过脚本精确匹配校验 |
| 4 | 新增接口声明、定义、调用点签名一致 | ✅ 满足 | 纯槽函数内部逻辑分流，无新增公开 API |
| 5 | 被修改的 `#include` 间接包含链闭合 | ✅ 满足 | 显式包含 `PermanentDeleteService.h` |
| 6 | 新建/删除文件已在 CMakeLists.txt 同步 | ✅ 不涉及 | 纯既有文件逻辑更新，无文件增删 |
| 7 | 跨类调用访问级别均为 public | ✅ 满足 | 全量核查通过 |
| 8 | 方案没有触及方案范围外的文件 | ✅ 满足 | 仅涉及目标文件 `PanelMediator.cpp` 与 `ContentKeyHandler.cpp` |
| 9 | 未采用笨拙浪费计算资源的手法 | ✅ 满足 | O(1) 逻辑分支分流，零冗余开销 |
| 10 | 代码中无打补丁行为 | ✅ 满足 | 在既有按键中枢中完成上下文分流 |
| 11 | 遵循 Memories.md 规范 | ✅ 满足 | 写入记忆真理源 |
| 12 | ToolTipOverlay 不追随鼠标 | ✅ 满足 | 使用 `showToast` 固定在 `target` 顶部居中 |
| 13 | 100% 语义化矢量图标 | ✅ 不涉及 | 无图标变更 |
| 14 | 异步回调按值捕获 | ✅ 不涉及 | 纯同步派发 |
| 15 | 无 Lambda 循环引用 | ✅ 不涉及 | 无循环引用 |
| 16 | 后台线程 UI 操作回归主线程 | ✅ 满足 | 信号槽在 GUI 主线程触发 |
| 17 | Qt 信号槽使用 C++11 函数指针语法 | ✅ 满足 | 沿用函数指针语法 |
| 18 | 对外 `.h` 签名冻结 | ✅ 满足 | 零修改 `.h` 文件，100% 冻结 |

---

## Chapter 10: Behavior Preservation & Zero-Value-Alteration Table（行为零篡改与数值零篡改声明表）

### 行为零篡改声明

| 既有行为 | 本方案影响 | 结论 |
|---|---|---|
| 普通物理目录下的 `moveToTrashRequested` 移入回收站 | `isTrashView` 为 false，完全走原逻辑 | ✅ 100% 原样保留 |
| 普通物理目录下的 `Del` / `Shift+Del` | 完全不触及 | ✅ 100% 原样保留 |

### 数值零篡改声明

| 数值类型 | 值 | 说明 |
|---|---|---|
| 提示框颜色 Hex | `#e81123` | 项目标准警示/错误红 |
| 提示时长 | `2000` ms | 项目标准 Toast 显示时长 |
