# Implementation Plan: PanelMediator 庞大连接调度系统性解耦重构方案

本实施方案严格遵循工业级 C++/Qt 顶层架构规范、《AGENTS.md》实施界限（严禁直接修改源代码，所有实施方案物理隔离存放于 `Implementation Plan/` 目录）及《Standard Specifications for Implementation Plan.md》，对 `QuarkMeta_Source/src/ui/PanelMediator.cpp` 中长达 900 余行的 `setupConnections` 进行结构化系统解耦与单一职责（SRP）治理，建立各专职协调器（Coordinator/Presenter/Controller/Binder/Router），同时对“当前目标窗格（SSOT）”和“逐窗格连接去重”进行统一化规范，确保重构前后界面行为、提示文案与数据流向完全一致。

---

## 前置物理证据清单 (Gate 1: Pre-Draft Evidence)

### 1. 物理读取与分析文件清单
- `QuarkMeta_Source/src/ui/PanelMediator.h` (共 101 行，定义组件依赖结构体 `PanelMediatorComponents`、中介者类声明与防抖状态结构)
- `QuarkMeta_Source/src/ui/PanelMediator.cpp` (共 1299 行，承担了整个主界面各个控件间的全部信号调度、元数据提取、QuickLook联动、快捷键转发、标签栏分屏等巨型逻辑)
- `QuarkMeta_Source/src/ui/controllers/PaneActivationTracker.h` (声明 `paneInteracted` 信号)
- `QuarkMeta_Source/src/ui/ContentPanel.h` (确认窗格公开方法 `isVisible()`、`rootPane()`、`panes()`、`displayPath()`、`setActivePane()`、`refreshAll()` 等)
- `QuarkMeta_Source/CMakeLists.txt` (显式列出各源文件清单)

### 2. 外部公开接口与契约检索验证 (Public API Freezing)
- `CoreEngine.cpp:244`: `PanelMediator::instance()->activeContentPanel()`
- `MarkedFolderService.cpp:72`: `PanelMediator::instance()->activeContentPanel()`
- `MainWindow.cpp:393, 394, 524`: `m_panelMediator->activeContentPanel()`
- `MainWindow.cpp:217`: `connect(m_panelMediator, &PanelMediator::statusMessageRequested, ...)`
- `NavigationService.cpp` 等多处: `PanelMediator::instance()->locateItem(path)`
- **结论**：`PanelMediator` 的对外接口 `instance()`、`locateItem()`、`activeContentPanel()`、`setupConnections()` 以及信号 `statusMessageRequested`、`activeContentPanelChanged` 的签名与行为保持 100% 冻结兼容。

## Chapter 1: Overview（概述与根因分析）

### 1.1 核心病灶与系统性缺陷深入剖析
当前 `PanelMediator.cpp` 中长达近 900 行的 `setupConnections` 存在严重的职责爆炸：
1. **目标窗格获取缺乏 SSOT（约 20 处分散判断）**：全文件大量充斥 `m_activeContentPanel ? m_activeContentPanel.data() : contentPanel`，甚至部分地方漏掉了 `isVisible()` 判断，导致隐藏窗格被误作为操作目标；
2. **信号重复绑定缺陷**：主窗格在初始化 `if (contentPanel)` 块中连接了一次 `directorySelected`、`requestAddFavorite`、`requestRemoveFavorite`，随后在 `wireContentPanel` 中又连接了一次，导致同一次用户操作触发两次信号处理；
3. **职责混杂严重违背 SRP**：元数据面板更新（空选/单选/多选/30ms防抖）、QuickLook 相邻文件搜寻、快捷键分发、标签栏分屏布局导出、导航与地址栏驱动全堆叠在一个庞大的 Lambda 闭包中，阅读与维护成本极高；
4. **多选元数据计算冗余**：选中大量项目时，即使所有项目的字段早已不一致且共同标签已为空，仍然循环遍历后续全部项目并读取元数据，造成计算浪费。

### 1.2 架构治理设计目标与统一规则
- **统一规则一：当前目标窗格（SSOT，要求可见）**：由 `ActivePaneCoordinator::currentTarget()` 统一提供，当前激活窗格存在且 `isVisible()` 为真则返回它，否则返回主窗格；
- **统一规则二：同一信号对同一窗格只连接一次**：所有窗格（主窗格及动态副窗格）统一通过“逐窗格绑定”函数只连接一次，彻底消除双重连接；
- **统一规则三：逐窗格职责拆分**：将原 `wireContentPanel` 职责解耦至各专用类的 `bindPane(ContentPanel*)` 中，由 `PanelMediator` 进行极薄的递归分发；
- **拆分为 8 个高内聚组件**：
  1. `ActivePaneCoordinator`：窗格激活管理与目标窗格 SSOT；
  2. `MetaPanelPresenter`：元数据面板展示、30ms防抖、多选两阶段性能优化与元数据事件响应；
  3. `MetaEditController`：元数据面板编辑信号到 CoreEngine 命令的翻译；
  4. `QuickLookController`：QuickLook 预览联动、相邻可预览文件查找单一实现与安全删除；
  5. `ShortcutActionRouter`：全局快捷键与标题栏动作路由；
  6. `TabBarCoordinator`：标签页切换、合并拆分、颜色图标更新与分屏状态保存单一实现；
  7. `NavigationCoordinator`：全应用位置显示单一入口、路径导航与刷新；
  8. `FilterStatsBinder`：筛选面板与窗格统计状态同步绑定。

## Chapter 2: Modified Files List（影响文件清单）

### 2.1 新建核心协调器与控制器文件清单（共 8 对，位于 `src/ui/`）
1. `QuarkMeta_Source/src/ui/ActivePaneCoordinator.h`
2. `QuarkMeta_Source/src/ui/ActivePaneCoordinator.cpp`
3. `QuarkMeta_Source/src/ui/MetaPanelPresenter.h`
4. `QuarkMeta_Source/src/ui/MetaPanelPresenter.cpp`
5. `QuarkMeta_Source/src/ui/MetaEditController.h`
6. `QuarkMeta_Source/src/ui/MetaEditController.cpp`
7. `QuarkMeta_Source/src/ui/QuickLookController.h`
8. `QuarkMeta_Source/src/ui/QuickLookController.cpp`
9. `QuarkMeta_Source/src/ui/ShortcutActionRouter.h`
10. `QuarkMeta_Source/src/ui/ShortcutActionRouter.cpp`
11. `QuarkMeta_Source/src/ui/TabBarCoordinator.h`
12. `QuarkMeta_Source/src/ui/TabBarCoordinator.cpp`
13. `QuarkMeta_Source/src/ui/NavigationCoordinator.h`
14. `QuarkMeta_Source/src/ui/NavigationCoordinator.cpp`
15. `QuarkMeta_Source/src/ui/FilterStatsBinder.h`
16. `QuarkMeta_Source/src/ui/FilterStatsBinder.cpp`

### 2.2 允许修改的文件清单
1. `QuarkMeta_Source/src/ui/PanelMediator.h`
2. `QuarkMeta_Source/src/ui/PanelMediator.cpp`
3. `QuarkMeta_Source/CMakeLists.txt`
