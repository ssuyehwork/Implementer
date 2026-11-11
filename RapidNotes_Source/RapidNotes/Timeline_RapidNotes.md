# RapidNotes 架构演进时间线与代码修改记录 (Timeline)

## 记录日期：2025-05-18

### 核心演进目标
1. 依据最新架构设计理念，彻底废除 RapidNotes 中的文件级伴随侧车 JSON（`.QuarkMeta.json` 与 `.RapidNotes.json`）存储机制，全面解绑对物理目录中 JSON 文件的读写与缓存维护，所有数据操作与元数据管理彻底移交给本地数据库 `inspiration.db`。
2. 修复从旧工程合并/重构到 RapidNotes 过程中的命名空间对齐、头文件路径错位、信号槽参数不匹配以及公共对话框组件（`FramelessMessageBox` / `BatchProgressDialog`）接口缺失等编译错误。

---

### 修改与删除文件清单

#### 1. 物理侧车代码移除（删除）
- **`src/meta/QuarkMetaJson.h`**：[已删除] 废弃原 QuarkMeta 侧车 JSON 抽象定义文件。
- **`src/meta/QuarkMetaJson.cpp`**：[已删除] 废弃原 QuarkMeta 侧车 JSON 解析与序列化实现。

#### 2. 核心与生命周期服务（修改）
- **`src/core/PermanentDeleteService.cpp`**：
  - 移除了对 `QuarkMetaJson.h` 的引用。
  - 删除了文件永久删除流程中对 `RapidNotesJson::removeItemMetadata` 的调用。
- **`src/core/DiskTrashService.cpp`**：
  - 移除了对 `RapidNotesJson.h` 的引用。
  - 删除了回收站恢复与放入时对 `RapidNotesJson::roamItemMetadata` 的调用。
- **`src/core/KeyboardHook.h`**：
  - 修复 `enterPressedInOtherApp` 信号参数声明，赋予默认值 `(bool ctrl = false, bool shift = false, bool alt = false)`，解决 `MessageCaptureHandler::onEnterPressed` 槽函数连接时的静态断言失败 (C2338/C2660/C2672)。

#### 3. 元数据与同步服务（修改）
- **`src/meta/LibrarySyncService.h`** / **`src/meta/LibrarySyncService.cpp`**：
  - 移除了对 `QuarkMetaJsonStore.h` 与 `RapidNotesJsonStore.h` 的依赖。
  - 彻底移除了 `handleJsonChangeEvent` 方法及其关联逻辑，避免在收到元数据变更时回写侧车 JSON。
- **`src/meta/MetadataManager.cpp`**：
  - 移除了对 `RapidNotesJsonStore.h` 的依赖。
  - 清除了所有 `updateItemMeta`、`addTagsBatch`、`setRatingBatch`、`setColorBatch`、`setNoteBatch`、`setURLBatch`、`setPinnedBatch`、`removeTagsBatch`、`migrateFolderCache`、`renameItem` 中向侧车 JSON 同步持久化的调用。
  - 重写了 `getFolderMeta` 与 `setFolderMeta`，解除对目录级侧车 JSON 的读取和落盘。
- **`src/meta/ItemMetaMapper.h`**：
  - 将对 `RapidNotesJson.h` 的依赖重构为对底层通用结构 `MetadataDefs.h` 的依赖。
- **`src/meta/MetaIndexService.cpp`**：
  - 移除了对 `RapidNotesJson.h` 和 `RapidNotesJsonStore.h` 的包含。
  - 清除了扫描物理文件夹时解析与合并侧车 JSON 的代码。
- **`src/meta/DiskNavigatorService.h`** / **`src/meta/DiskNavigatorService.cpp`**：
  - 移除了对 `RapidNotesJson.h` 的引用。
  - 文件夹与条目元数据获取流程全部重构为安全内存/数据库占位逻辑，杜绝解析 `.RapidNotes.json`。
- **`src/meta/MetaCacheDecorator.cpp`**：
  - 移除了对 `RapidNotesJsonStore.h` 的依赖。
  - 解除了 `folderCacheMap` 向侧车 JSON 的读写代理。

#### 4. 工具与辅助模块（修改）
- **`src/util/ShellHelper.cpp`**：
  - 移除了对 `RapidNotesJson.h` 与 `RapidNotesJsonStore.h` 的包含。
  - 移除了 `roamBatchMetadata`（批量漫游）、`flushAllDirtyBuffers`（脏缓冲写盘）以及 `migrateItemMetadata` 对侧车 JSON 的调用。
- **`src/util/DiskMediaExtractor.cpp`**：
  - 移除了对 `RapidNotesJsonStore.h` 的包含。
  - 移除了利用侧车 JSON 缓存标记解析失败缩略图状态的逻辑。
  - 移除了深度解析文件分辨率尺寸后向侧车 JSON 同步写盘的代码。

#### 5. UI 与视图模型层（修改）
- **`src/ui/LibraryPanel.cpp`**：
  - 移除了对 `RapidNotesJsonStore.h` 的无效包含。
- **`src/ui/models/DiskItemModel.h`** / **`src/ui/models/DiskItemModel.cpp`**：
  - 移除了对 `RapidNotesJson.h` 和 `RapidNotesJsonStore.h` 的头文件引用。
  - 移除了异步预加载图片尺寸后向侧车 JSON 同步刷写的逻辑。
- **`src/ui/models/ItemModelBase.h`**：
  - 纠正了 `ItemRecord.h` 头文件包含路径（从绝对虚拟路径 `src/core/ItemRecord.h` 修正为相对路径 `../../core/ItemRecord.h`），修复 C1083 错误。
- **`src/ui/ActivationDialog.cpp`** / **`src/ui/CategoryPasswordDialog.cpp`**：
  - 修正了对基类 `FramelessDialog` 成员变量的访问（将 `m_btnPin` 修正为 `m_pinBtn`），修复 C2065 错误。
- **`src/ui/ToolTipOverlay.h`**：
  - 补充全局命名空间类型别名 `using ToolTipOverlay = RapidNotes::ToolTipOverlay;`，根除跨模块未限定命名空间导致的 C2653 与 C3861 错误。
- **`src/ui/dialogs/FramelessMessageBox.h`** / **`src/ui/dialogs/FramelessMessageBox.cpp`**：
  - 补充继承自 `FramelessDialog` 的实体对话框构造重载，提供 `confirmed` 与 `cancelled` 信号，并导出全局类型别名，全面兼容 `main.cpp` 与 `QuickWindow.cpp` 中的 `dlg.exec()` 实体调用。
- **`src/ui/BatchProgressDialog.h`**：
  - 完善多参构造函数 `(title, label, min, max, parent)`，补充 `setLabelText()`、`wasCanceled()` 以及取消中断事件监听，修复 `FileStorageHelper.cpp` 的 C2039/C2661 错误。

---

## 记录日期：2025-05-19

### 核心演进目标
1. 解决 MSVC 编译报出的 C1083、C2039、C2065、C3861、C2923、C2955 错误，确保跨模块依赖链、接口和类型完全闭合。
2. 践行《Memories_RapidNotes.md》里程碑 1 和里程碑 2 规范，物理清理废弃的列视图（ColumnView）残留代码，将物理盘符栏（DriveBarWidget）切换为纯笔记体系的快捷设置扩展栏（SettingsBarWidget）。
3. 迁移项目时间线文档 `Timeline_RapidNotes.md` 到 RapidNotes 根目录下，与架构设计文档保持同级。

---

### 修改与删除文件清单与原因说明

#### 1. 文档架构规范化（移动）
- **`src/Timeline_RapidNotes.md` -> `Timeline_RapidNotes.md`**：
  - **修改原因**：根据项目架构治理规范，将时间线文档从子目录 `src/` 迁移至 RapidNotes 项目根目录，与 `Memories_RapidNotes.md`、`README.md` 等顶层真理源文档平级，便于查阅与维护。

#### 2. 对话框与头文件冗余清理（删除与修改）
- **`src/ui/SmartFolderDialog.h`**：[已删除]
  - **修改原因**：该文件为历史残留的冗余假头文件，内部引用了物理不存在的 `../core/SmartFolderRecord.h`，引发 C1083 编译错误。项目中正式的无边框智能文件夹对话框实现已规范归口于 `src/ui/dialogs/SmartFolderDialog.h` / `.cpp`。因此彻底删除该冗余文件并在 `CMakeLists.txt` 中同步注销。
- **`CMakeLists.txt`**：
  - **修改原因**：移除了已被删除的废弃头文件 `src/ui/SmartFolderDialog.h`。

#### 3. 悬浮提示与对话框基类接口契约对齐（修改）
- **`src/ui/ToolTipOverlay.h`**：
  - **修改原因**：在 `QuickPreview.h`、`QuickWindow.cpp`、`UnifiedSearchWindow.cpp` 等多处业务调用中，使用了 `ToolTipOverlay::ActivePriority` 优先级参数。但当前 `ToolTipOverlay` 缺失枚举定义与带优先级的重载，导致报出 C2039/C2065 错误。本次添加 `enum Priority { PassivePriority = 0, ActivePriority = 1 };` 及带 `Priority` 的 `showText` 重载，恢复接口兼容。
- **`src/ui/FramelessDialogBase.h` / `src/ui/FramelessDialog.cpp`**：
  - **修改原因**：`ColorPickerWindow`、`FileStorageWindow` 等派生对话框在构造时统一调用 `loadWindowSettings()` 恢复置顶状态与几何信息，但重构后的基类中漏声明了该虚方法，导致 C3861“找不到标识符”错误。本次在基类补充了 `virtual void loadWindowSettings();` 和 `virtual void saveWindowSettings();` 的声明与基于 QSettings 的标准实现。

#### 4. 列视图（ColumnView）废弃代码物理清除（修改）
- **`src/ui/ContentPanel.cpp`**：
  - **修改原因**：根据《Memories_RapidNotes.md》第 19 章里程碑 2 规范，RapidNotes 纯数据库化后界面模式收敛，仅保留“列表视图 (ListView)”和“网格视图 (GridView)”，所有列视图源码（`ColumnViewWidget`、`ColumnViewPane`）已物理删除。但在 `ContentPanel.cpp` 中仍残留了 9 处对 `ColumnView`、`m_columnView`、`ColumnViewWidget`、`ColumnViewPane` 的初始化、信号连接与分支判断，引发大量的 C2065/C2653/C2276/C2530/C3536 编译报错。本次彻底清除了所有残留的 ColumnView 逻辑分支，使内容面板与设计规范 100% 对齐。

#### 5. 任务进度状态快照类型闭合（修改）
- **`src/core/TaskProgressService.h`**：
  - **修改原因**：`StatusTextCoordinator.h` 使用了 `TextSegment`、`TaskProgressSnapshot` 结构体及 `TaskProgressService::snapshotChanged` 信号，但 `TaskProgressService.h` 中此前缺少这几个结构体定义，导致 C2923、C2955、C2065、C4430“QList: TextSegment 不是有效模板类型参数”连锁报错。本次在 `TaskProgressService.h` 中补充了 `TextSegment`（包含文本与高亮标志）、`TaskProgressSnapshot` 结构体以及 `snapshotChanged` 信号声明，彻底消除模板实例化错误。

#### 6. 盘符栏（DriveBarWidget）向设置栏（SettingsBarWidget）重构替换（修改）
- **`src/ui/MainWindow.h` / `src/ui/MainWindow.cpp`**：
  - **修改原因**：根据《Memories_RapidNotes.md》第 14.2 节与第 19 章里程碑 1 规范，QuarkMeta 原有的面向磁盘物理盘符的 `DriveBarWidget` 已被彻底剔除，并更名重构为快捷设置与扩展管理工具栏组件 `SettingsBarWidget`。原 `DriveBarWidget.h` 已不存在，导致 `MainWindow.cpp:8` 报出 C1083 错误。本次将 `MainWindow` 中的头文件包含由 `DriveBarWidget.h` 更新为 `SettingsBarWidget.h`，成员变量由 `m_driveBarWidget` 更新为 `m_settingsBarWidget`，彻底根除物理驱动器旧概念并消除编译报错。
- **`src/ui/SettingsWindow.h` / `src/ui/SettingsWindow.cpp`**：
  - **核验说明**：在移除 `DriveBarWidget.h` 的同时，已全面确认 `SettingsWindow.h` 与 `SettingsWindow.cpp` 的头文件与实现完整性，确保全局设置、热键、外观及插件扩展配置均由 `SettingsWindow` 统一承接管控，并在 `CMakeLists.txt` 中完整注册。

#### 7. 派生窗口基类置顶按钮成员变量对齐（修改）
- **`src/ui/PasswordVerifyDialog.cpp`** / **`src/ui/SettingsWindow.cpp`** / **`src/ui/TagEditDialog.cpp`**：
  - **修改原因**：基类 `FramelessDialog` 中定义的置顶按钮成员变量名为 `m_pinBtn`，而在 `PasswordVerifyDialog`、`SettingsWindow` 与 `TagEditDialog` 中误写为旧名称 `m_btnPin`，导致 MSVC 报出 C2065“m_btnPin 未声明的标识符”错误。本次统一将这些派生类中的 `m_btnPin` 修正为基类实际受保护成员 `m_pinBtn`，使对话框在初始化时正确隐匿置顶按钮，彻底根除 C2065 报错。

---

## 记录日期：2025-05-20

### 核心演进目标
1. 解决 `QuickWindow.cpp` 中 MSVC 编译报出的 C2027（使用了未定义类型“DropTreeView”）、C2446/C2664（DropTreeView 指针无法转换为 QWidget*）及 C2065（未声明标识符）错误。
2. 修复 `FramelessInputDialog` 命名空间未导出导致的 C2760 / C2059 语法错误及 `pwdMenu` 作用域截断报错。
3. 纠正 `QuickWindow` 中筛选面板类型，对齐专用组件 `QuickFilterPanel` 契约并在 `CMakeLists.txt` 中同步注册。

---

### 修改文件清单与原因说明

#### 1. 命名空间全局导出与类型别名闭合（修改）
- **`src/ui/QuickWindow.h` / `src/ui/QuickWindow.cpp`**：
  - **修改原因**：`DropTreeView` 定义于 `namespace RapidNotes` 中，若直接在头文件底部使用 `using RapidNotes::DropTreeView;` 会在包含全局前向声明时引发 MSVC C2874（多次声明）。本次在 `QuickWindow.h` 中将前向声明精确归入 `namespace RapidNotes { class DropTreeView; }` 并使用 `using RapidNotes::DropTreeView;`，同时在 `QuickWindow.cpp` 包含 `DropTreeView.h` 后明确引入 `using RapidNotes::DropTreeView;`，从根本上解决 Incomplete Type（C2027）、指针无法转换为 `QWidget*`/`QObject*`（C2664、C2446）以及无法识别自定义信号（C2065 `notesDropped`）的问题。
- **`src/ui/dialogs/FramelessInputDialog.h` / `src/ui/FramelessDialog.h`**：
  - **修改原因**：`FramelessInputDialog` 定义在 `RapidNotes` 命名空间中，`FramelessDialog.h` 中漏导出了该类型别名，导致 `QuickWindow.cpp:2671` 等处识别不到类型定义并报出 C2760“此处出现意外的 dlg；应为 ;”及级联作用域报错。本次在 `FramelessInputDialog.h` 底部与 `FramelessDialog.h` 中显式导出 `using RapidNotes::FramelessInputDialog;`。

#### 2. 筛选面板契约对齐与构建注册（修改）
- **`src/ui/QuickWindow.h` / `src/ui/QuickWindow.cpp`**：
  - **修改原因**：`QuickWindow` 中原定义使用了主窗口的 `FilterPanel`，导致与调用的 `getCheckedCriteria()`、`updateStats`、无参 `filterChanged` 信号不匹配。本次将其修正为专职组件 `QuickFilterPanel`，包含 `#include "QuickFilterPanel.h"` 并更新连接。
- **`CMakeLists.txt`**：
  - **修改原因**：在构建单元源文件列表中同步注册 `src/ui/QuickFilterPanel.cpp` 与 `src/ui/QuickFilterPanel.h`，确保符号完整链接。

#### 3. 元数据缓存接口调用纠正（修改）
- **`src/meta/MetadataManager.cpp`**：
  - **修改原因**：`addTagsBatch` 与 `removeTagsBatch` 中误调用了不存在的方法 `MetaMemoryCache::instance().get(nPath)`，引发 MSVC C2039“不是 MetaMemoryCache 的成员”报错。本次将其修正为标准接口 `getMeta(nPath)`。

#### 4. 第三方库头文件搜索路径补全（修改）
- **`CMakeLists.txt`**：
  - **修改原因**：`FormatDecoders.cpp` 依赖了 `src/third_party/libtiff/tiffio.h`，但全局 `include_directories` 中此前缺少 `src/third_party/libtiff` 目录，导致 MSVC 报出 C1083“无法打开包括文件: ‘tiffio.h’: No such file or directory”错误。本次在 `include_directories` 中补充注册该路径。

#### 5. 后台任务进度接口对齐与大图生成权限解耦（修改）
- **`src/util/BatchQuickLookService.cpp`**：
  - **修改原因**：代码中使用了未定义的历史类型与函数 `TaskJobOptions`、`TaskProgressJob` 与 `createJobHandle`，导致 MSVC 报出 C2760“此处出现意外的 opts/job；应为 ;”及 C2059 语法错误；同时误写了 `UiHelper::isGraphicsFile`（应为 `ColorPaletteEngine::isGraphicsFile`），引发 C2039/C3861 报错。本次统一对齐现有标准接口。
- **`src/ui/QuickLookWindow.h`**：
  - **修改原因**：`loadOrExtractQuickLookImage` 静态大图提取函数此前声明在 `private` 作用域下，导致 `BatchQuickLookService` 无法跨模块访问并报出 C2248“无法访问 private 成员”错误。本次将其提升至 `public` 访问级别。

#### 6. 筛选面板分组组件构建注册（修改）
- **`CMakeLists.txt`**：
  - **修改原因**：`FilterPanel.cpp` 引用了 `DuplicateStatusGroup::populate`，但 `src/ui/DuplicateStatusGroup.cpp` 与 `DuplicateStatusGroup.h` 此前未在 `CMakeLists.txt` 中注册编译，引发 MSVC 链接器报出 LNK2019 / LNK1120 无法解析的外部符号错误。本次将其同步补充进源文件列表。


### 2026-11-10 外联样式归一化 (style.qss)、QuickWindow 内联样式迁移与 MainWindow 快捷入口

#### 1. 外联样式库归一化与 dark_style.qss 彻底删除（移除 & 归一化）
- **resources/qss/dark_style.qss**: 废弃并彻底删除物理目录 resources/qss 及文件 dark_style.qss，单一可信源归一化至 resources/style.qss。
- **resources/resources.qrc**: 从 Qt 资源清单中移除 qss/dark_style.qss，仅保留 style.qss。
- **src/main.cpp**: 将启动阶段读取的全局样式表路径由 :/qss/dark_style.qss 切换为 :/style.qss。

#### 2. QuickWindow 行内 setStyleSheet 零值迁移至 style.qss（零值搬迁）
- **src/ui/QuickWindow.cpp**: 彻底杜绝 C++ 源码中硬编码内联样式，遵循 Zero-Value-Alteration 原则。将 AppLockWidget 密码锁界面、输入框 QuickWindow_m_pwdEdit、关闭按钮 QuickWindow_m_closeBtn、主容器 QuickWindow_container、搜索框 QuickWindow_m_searchEdit、分类树/系统树、底部分类搜索与标签输入框、自定义工具栏 customToolbar、新建菜单及列表右键上下文菜单内联样式完全搬迁至 resources/style.qss。移除全部 23 处内联 setStyleSheet 调用，并设置对应语义化 objectName。
- **resources/style.qss**: 集中承接 QuickWindow 提取的全部样式规则，并配置针对 QPushButton#QuickWindow_btnAdd_12::menu-indicator 的隐藏规则，去除原“+”号按钮右下角下拉三角箭头，恢复原版居中扁平外观。

#### 3. 新增 MainWindow 快捷打开按钮及跨模块联动（新增 & 连接）
- **src/ui/QuickWindow.h**: 声明信号 signals: void toggleMainWindowRequested();。
- **src/ui/QuickWindow.cpp**: 在垂直工具栏的“+”号按钮下方插入 btnMainWindow 工具按钮，图标使用 SvgIcons 库内置的 window (18px, #aaaaaa)，点击时触发 toggleMainWindowRequested() 信号。
- **src/main.cpp**: 引入 ui/MainWindow.h，连接 quickWin->toggleMainWindowRequested 信号：若主窗口尚未创建则动态实例化，若已存在或刚创建则通过 showNormal() / activateWindow() / raise() 还原并激活至前台。

#### 4. 解决 MainWindow 命名空间导出与主入口符号未声明问题（修改）
- **`src/ui/MainWindow.h`**：
  - **修改原因**：`MainWindow` 声明在 `RapidNotes` 命名空间内，而 `main.cpp` 未使用 `using namespace RapidNotes;`，导致 MSVC 在编译 `main.cpp` 时报出 C2065“MainWindow 未声明的标识符”并引发连锁语法报错。本次在 `MainWindow.h` 底部增加全局类型别名导出 `using RapidNotes::MainWindow;`。


### 2026-11-11 QuickWindow 接入中央大脑架构、批量 SSOT 与 MSVC 动态链接 (/MD) 运行时库修复

#### 1. QuickWindow 接入 CoreEngine 中央大脑与彻底消灭伪批量循环写盘（重构 & 架构解耦）
- **时间轴**：2026-11-11
- **涉及文件**：
  - `src/core/CoreEngine.h`
  - `src/core/CoreEngine.cpp`
  - `src/ui/QuickWindow.h`
  - `src/ui/QuickWindow.cpp`
- **修改原因与设计**：
  1. **中央大脑脱节治理**：`QuickWindow` 此前在拖拽放置笔记至分类、收藏、回收站时，直接跨层穿透调用底层的 `DatabaseManager::instance()`，破坏了单向依赖流动与职责边界；
  2. **消除伪批量循环单条写盘**：在条目拖拽放置逻辑中存在 `for (int id : ids)` 循环调用 `updateNoteState`，造成多次无效磁盘事务与广播风暴。本次在 `CoreEngine` 中扩充 `AppCommandType::MoveToCategory` 命令枚举与执行分支，调用 `DatabaseManager::instance().moveNotesToCategory` 原子批量移动并发布 `CentralEventHub` 的 `MetadataUpdated` 事件；
  3. **收敛至 SSOT**：在 `QuickWindow.h` 中显式包含 `CoreEngine.h` 与 `CentralEventHub.h`，将拖入分类、未分类、回收站统一打包为 `AppCommand` 投递给 `CoreEngine` 统一调度；将拖入收藏直接收敛为 `DatabaseManager::instance().updateNoteStateBatch` 集合式 SQL（`WHERE id IN (...)`）单事务批处理并广播全局事件。

#### 2. MSVC 动态链接与运行时库不匹配 (/MD) 彻底修复（构建治理）
- **时间轴**：2026-11-11
- **涉及文件**：
  - `CMakeLists.txt`
  - `CMakePresets.json`
- **修改原因与设计**：
  1. **消除 LNK2038 与 LNK2005 运行时库冲突**：此前 CMakeLists.txt 全局与 Target 级别强锁 `/MT` 静态运行时库，而官方标准 Qt6 SDK 为 `/MD` 动态构建，导致链接器报出严重的符号多重定义（libcpmt.lib vs msvcprt.lib）；本次将全局及 `RapidNotes`、`RapidNotesWatchdog` Target 属性统一规范为 `MultiThreadedDLL$<$<CONFIG:Debug>:Debug>`；
  2. **拔除静态专用宏**：彻底移除静态 Qt 专属的 `qt_import_plugins(RapidNotes ...)` 调用，杜绝因静态插件初始化 obj 引发的二次符号冲突；
  3. **路径与预设统一**：`CMakeLists.txt` 优先支持 `$ENV{QT_DIR}` 环境变量并默认指向官方动态版 `C:/Qt/6.10.2/msvc2022_64`，同步修正 `CMakePresets.json` 中的 `CMAKE_PREFIX_PATH` 为动态版路径，确保构建与固化运行层部署流程一致。

#### 3. 品牌与 Logo 资产归一化及死代码注销（品牌治理 & 构建清理）
- **时间轴**：2026-11-11
- **涉及文件**：
  - `QuarkMeta.manifest`
  - `QuarkMeta.rc`
  - `CMakeLists.txt`
- **修改原因与设计**：
  1. **品牌资产主次对齐**：贯彻“以 RapidNotes 为主，QuarkMeta 为次”的核心设计理念，消除 Windows 清单中残留的 QuarkMeta 标识与描述；将 `QuarkMeta.manifest` 装配件名称统一更新为 `RapidNotes`，描述更新为 `RapidNotes Desktop Application`，确保任务管理器、UAC 提权窗口及系统属性呈现统一的 RapidNotes 品牌形象；
  2. **修正资源清单引用说明**：将 `QuarkMeta.rc` 中的注释引用同步修正为 `RapidNotes.manifest`；
  3. **注销孤儿死代码**：在 `CMakeLists.txt` 中物理注销未被任何模块引用的 `QuarkApplication.cpp` 与 `QuarkApplication.h` 构建条目，避免编译资源浪费与命名污染。
