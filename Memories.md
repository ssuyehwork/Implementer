# 核心记忆与顶层规范指导文档 (Memories.md)

本文档为 QuarkMeta 项目**唯一权威的历史记忆、全局设计理念、UI 交互哲学与工程架构规范指南**。
在进行任何项目结构调整、UI 改版、功能新增、重构或 Bug 修复时，Agent 与开发者**必须优先读取并无条件遵守本文档所约定的各项规范与红线**。

> **注意**：原 `QuarkMeta-Architecture-Planning.md` 与 `Guide & Preference.md` 已全面废除并无损迁移合并至本文档。从现在起，系统全面以 `Memories.md` 为唯一真理源 (SSOT)。

---

## 0. 全局视觉、品牌规范与核心红线

- **品牌橙色 (BrandOrange)**: 物理色值为 **`#cb7208`**。该颜色仅用于 FERREX 品牌 Logo、标题栏名称文字等品牌识别元素。
- **置顶激活色 (ActiveOrange)**: 物理色值为 **`#ff551c`**。该颜色用于全应用置顶激活按钮、内容面板置顶角标及侧边栏置顶状态。
- **强制解耦**: 品牌色与置顶激活色必须独立，严禁共同引用同一个常量或色值。
- **滑杆交互反馈**: 筛选面板中的百分比滑杆（如“占比”）必须支持实时数值回显，通过 `ToolTipOverlay` 在悬停或滑动时动态展示百分比，并在释放或离开时自动隐藏。
- **彻底杜绝 UI 焦点虚线框与虚线边框铁律**: 
  1. 除了内容面板中“显示空文件夹”可采用虚线边框作为特定视觉标识外，全应用其他所有 UI 控件、按钮、图标与元素（包括所有标题栏控制按钮、工具栏按钮、标签按钮、“+ 添加标签”按钮等）**绝对禁止出现任何形式的虚线框或虚线边框**！
  2. **代码级双重强锁**: 所有新建或修改的 `QPushButton`、`QToolButton` 等控制按钮，在 C++ 代码中**必须强制调用 `setFocusPolicy(Qt::NoFocus)`** 剥夺焦点抢占能力，同时在 QSS 样式中**必须显式声明 `outline: none;`**，双保险彻底从根源上消除 Qt 样式引擎绘制默认 Focus Rect 虚线框的所有可能！
- **图标格式限制**: 全应用所有 UI 元素与控件只允许使用 SVG 格式矢量图标，严禁使用任何文本或特殊符号（如 Unicode 符号、键盘字符等）替代/充当图标！
- **星级表达与真理源契约 (Rating Display SSOT Contract)**: 全软件所有涉及星级评级的 UI 展示、下拉选择框、筛选菜单与规则编辑器（包括但不限于 FilterPanel、SmartFolderDialog 智能文件夹规则配置、属性面板 MetaPanel 等），绝对禁止使用“1星”、“2星”等阿拉伯数字与文字组合，同时也严格遵守“严禁使用 Unicode 符号（如 ★）替代图标”的红线！必须统一使用 SVG 矢量图标（`star_filled`）动态绘制 1~5 颗星的 Pixmap/QIcon 进行渲染展示，保持全软件矢量化与图形 SSOT 通道 100% 物理一致。

---

## 1. UI 样式与主题管理规范

### 1.1 核心职责分工 (QSS + ThemeManager)
在本项目中，全局 UI 样式与主题控制遵循“**QSS 负责静态样式声明 + ThemeManager.cpp 负责代码层控制与统一加载**”的分工架构：

- **`resources/style.qss` ( 静态样式真理源 )**：
  负责集中定义应用中所有**全局统一的静态 UI 样式**，包括但不限于：
  * 顶层窗口与基础容器背景色、文字颜色；
  * 五大实体栏区卡片底板、边框与 5px 实体物理切缝 (`margin: 0px 2px`)；
  * 全局统一 `QMenu` 菜单样式 (背景、项 hover/selected、分隔线)；
  * 全局统一滚动条 (`QScrollBar`) 样式；
  * 全局统一输入框 (`QLineEdit`, `QTextEdit`, `QPlainTextEdit`) 样式；
  * 全局 `QTreeView` 列表视图暗色斑马纹背景与选中项高亮；
  * 全局 `QCheckBox` 复选框样式。

- **`src/ui/ThemeManager.cpp` ( 样式加载与底层控制入口 )**：
  用于处理 QSS 格式无法独立完成或表达的代码层控制逻辑，包括但不限于：
  * **全局样式加载**：提供 `initialize(app)` 与 `getGlobalStyleSheet()` 入口，将 `:/style.qss` 集中读取并注入到全局 `QApplication`；
  * **原生窗口属性控制**：针对右键/托盘菜单等控件设置 `Qt::WA_TranslucentBackground`（背景半透明）、`Qt::FramelessWindowHint`（无边框）及 `Qt::NoDropShadowWindowHint`（消除原生阴影）等代码层窗口属性；
  * **动态覆盖与集中入口**：提供统一的 C++ 方法入口（如 `applyMenuStyle`）以便对特定场景实施局部覆盖与集中管理。

### 1.2 UI 样式开发规范与红线
1. **禁止内联硬编码**：在后续开发与重构中，**严格禁止在控件 C++ 代码中随意调用 `setStyleSheet(...)` 采用内联方式硬编码样式**。
2. **全局统一修改**：凡涉及通用控件外观、颜色、边距、圆角等静态样式调整，一律在 `resources/style.qss` 中进行集中修改或扩充选择器。
3. **全局样式表注入对象规范（作用域红线）**：全局 QSS 样式表**必须且只能注入至 `QApplication` 实例**（通过 `ThemeManager::initialize(app)`），**绝对禁止仅挂载在 `MainWindow` 窗口实例上**。因为 Qt 样式表仅沿着父子部件树（Widget Tree）向下级联继承，若挂在 `MainWindow` 上，会导致所有 `parent = nullptr` 的独立顶层窗口（如 `QuickLookWindow`、各类无边框对话框）无法继承 QSS，退化为原生控件外观。
4. **极少数例外**：仅在纯运行时根据用户输入/数据动态计算生成的数值色彩（如 `MetaRatingColorWidget` 依据动态 hex 色彩绘制）时，才允许在 C++ 中进行必要的高效动态处理。

### 1.3 顶层 UI 设计哲学与菜单/输入框规范
1. **全局统一视觉与零差异体验**：
   应用中所有弹出的右键菜单（上下文菜单）、托盘菜单及系统控制菜单，必须在外观样式、几何边距、高亮颜色与交互动画上保持 100% 绝对一致，杜绝任何窗口或面板回退至原生或不一致样式。
2. **100% 语义化矢量图标契约 (Zero Unadorned Text)**：
   全软件所有右键菜单与上下文菜单项，**必须为每一个菜单项配置与功能语义精准匹配的矢量图标**，严禁出现任何纯文本裸露项。
   - **语义精准契约**：图标必须严格对齐操作功能（如打开用文件夹/应用开启类图标、重命名用编辑类图标、复制用双页类图标、删除用垃圾桶类图标），严禁乱用或滥用无关图标；
   - **格式唯一限制**：全软件图标统一且只能使用 SVG 矢量格式，严格禁止使用任何字符、Unicode 符号或文本标点充当/替代图标；
   - **图标缺失补全**：若系统中缺失对应语义的图标，必须在矢量图标库中创建并注册相符的 SVG 图标定义。
3. **像素级控制与统一间距布局**：
   - 全局菜单中，图标与文本之间的物理间距统一并精确锁定为 **`10px`**；
   - 悬停/选中状态颜色统一对标 **`#3E3E42`**；
   - 具有状态属性的操作（如置顶/取消置顶、收藏/取消收藏）必须进行状态感知与动态图标形态联动；
   - **无彩色单色绝对统一（绝对禁止例外）**：全局所有右键菜单与上下文菜单项图标**统一且仅可采用同一个中性单色设计 (`#EEEEEE`)**，绝对禁止在右键菜单任何图标上出现或使用任何特殊色彩/着色（包括但不限于 `#ff551c`、红、黄、蓝、绿等），绝对不存在任何例外场景（例如“标记为目标文件夹”项也必须使用 `#EEEEEE` 中性单色），确保界面视觉沉静高档。
4. **输入框清除按钮响应式呈现与原生契约**：
   - 每个可编辑的单行输入框必须且只能配置 Qt 原生的 `setClearButtonEnabled(true)`，杜绝脑补另创自定义清除按钮；
   - 全系统中所有具备清除能力的单行文本输入框（如全局搜索栏、标签/日期/类型筛选输入框、路径编辑框），其清除按钮 (×) 必须严格遵循“有文本时动态显示、无文本时绝对隐匿”的响应式契约。严禁在输入框为空（包括呈现 Placeholder 占位文本）时显示清除按钮。
5. **文本输入框应用专属上下文菜单契约 (App-Exclusive LineEdit Context Menu Contract)**：
   全软件所有文本输入框（包括单行 `QLineEdit` 与多行 `QTextEdit`）严格禁止弹出操作系统默认的原生英文右键菜单。必须统一拦截并替换为 QuarkMeta 应用专属暗色右键菜单（完整提供 `撤销 Ctrl+Z`、`重做 Ctrl+Y`、`剪切 Ctrl+X`、`复制 Ctrl+C`、`粘贴 Ctrl+V`、`删除`、`全选 Ctrl+A`），且每一个菜单项必须配备 100% 语义匹配的中性单色 SVG 矢量图标、物理 `10px` 图文间距与 QuarkMeta 统一暗色视觉样式（`UiHelper::applyMenuStyle`）。
6. **规则构造器紧凑按钮与控件样式契约 (Rule Builder Controls Style Contract)**：
   批量创建与批量重命名界面中的规则构造器控件（`RuleRow` / `CreateRuleRow`），其加减号控制按钮（`QPushButton#RuleDeleteBtn`）、规则下拉框（`QComboBox#RuleCombo`）与文本/数字输入框必须保持严格统一的紧凑视觉规范：
   - 加减号按钮（`-` / `+`）必须具备 1px 明确的灰边框 (`1px solid #434343`)、14px 加粗文字与像素级居中对齐，严禁出现文本溢出裁剪或无边框坍塌；
   - 规则下拉框与输入框高度锁定为 25px，具备 1px 实线边框 (`#444`) 与 4px 圆角，下拉弹出列表统一使用 QuarkMeta 深色背景 (`#2D2D2D`) 与 `#3E3E42` 项选中高亮。
7. **Tooltip 提示控件限制**:
   全应用**严禁使用 QWidget 原生 `setToolTip(...)` 硬编码悬浮提示**，悬浮提示只可使用统一的 `ToolTipOverlay` 控件（结合 `m_hoverFilter` 事件过滤器）实现感知与展示，以确保主题视觉一致性与平滑交互体验。
8. **“在新标签页中打开”图标矢量键规范契约 (“Open in New Tab” Vector Icon Specification Contract)**：
   全软件所有右键上下文菜单（包括“标记”面板 `MarkedPanel`、库面板 `LibraryPanel`、收藏夹 `FavoritePanel` 等），针对“在新标签页中打开”菜单项，其矢量图标必须统一绑定且仅可使用 **`Folder-add`**（严禁使用裸 `add` 或 `plus` 等泛化图标），确保图标语义精准对齐新建 Tab/文件夹卡片，且图标颜色无条件严格遵循 `#EEEEEE` 中性单色红线。
9. **星级表达与真理源契约 (Rating Display SSOT Contract)**：
   全软件所有涉及星级评级的 UI 展示、下拉选择框、筛选菜单与规则编辑器（包括但不限于 FilterPanel、SmartFolderDialog 智能文件夹规则配置、属性面板 MetaPanel 等），绝对禁止使用“1星”、“2星”等阿拉伯数字与文字组合，同时也严格遵守“严禁使用 Unicode 符号（如 ★）替代图标”的红线！必须统一使用 SVG 矢量图标（`star_filled`）动态绘制 1~5 颗星的 Pixmap/QIcon 进行渲染展示，保持全软件矢量化与图形 SSOT 通道 100% 物理一致。
   - **配色与背景样式规范**：所有星级评级控件（及 QuickLook 星级 Toast 提示等）的底色/胶囊背景必须统一采用深灰色（如 `#2B2B2B` / `#333333`），星级图标本体则统一填充中性灰色（如 `#AAAAAA` / `#EEEEEE`），确保全软件暗色视觉沉静统一。
11. **颜色标记表达与真理源契约 (Color Display SSOT Contract)**：
   在 `QuickLookWindow` 及全软件进行颜色标记变更时， Toast 反馈绝对禁止使用 `"已应用颜色标记"` 等抽象纯文本！必须采用图形化渲染方式：在深灰色（`#2B2B2B`）Toast 胶囊气泡中，直接绘制并展示对应颜色的实心圆点/色块（如红 `#e74c3c`、橙 `#e67e22`、黄 `#f1c40f`、绿 `#2ecc71`、青 `#1abc9c`、蓝 `#3498db`、紫 `#9b59b6`、灰 `#95a5a6`），与星级 Toast 的图形化胶囊样式保持 100% 视觉与物理一致。
10. **“在资源管理器中显示/打开”场景语义契约 (Show/Open in Explorer Semantics Contract)**：
   全软件右键菜单中调用 Windows 资源管理器的菜单项必须严格按交互场景区分文案与行为：
   - **选中文件或文件夹项**：菜单文案统一为 **`在“资源管理器”中显示`**，调用 `ShellHelper::openInExplorer(path)` 执行 `explorer /select, <path>`（在资源管理器中定位并高亮选中该项）；
   - **在空白处右键**：菜单文案统一为 **`在“资源管理器”中打开`**，调用 `ShellHelper::openDirectoryInExplorer(currentPath)` 执行 `explorer <currentPath>`（直接进入当前目录）；
   - 统一由 `ContextMenuFactory::buildShowInExplorerAction(menu, path, isBlankSpace, receiver)` 工厂方法闭环构建，保证全软件行为与 Windows 原生习惯 100% 物理一致。

---

## 2. 界面组件与布局物理规范

### 2.1 标题栏容器 (TitleBar)
- **高度 (Height)**: 物理高度固定为 `34px`。
- **布局边距 (ContentsMargins)**: `(5, 0, 5, 0)`（左侧边距 `5px`，右侧物理对齐 `5px`）。
- **全局间距 (Spacing)**: `5px` (应用名与按钮组之间的间距)。

### 2.2 按钮组容器 (TitleBarButtons Container)
- **布局间距 (Spacing)**: 按钮与按钮之间的物理间距固定为 `5px`。
- **布局边距 (ContentsMargins)**: `(0, 0, 0, 0)`。

### 2.3 按钮物理参数 (Button Parameters)
- **外框尺寸 (FixedSize)**: `24x24px`。
- **图标尺寸 (IconSize)**: `18x18px`。
- **圆角 (BorderRadius)**: `4px`。
- **背景样式**:
  - 默认状态: `transparent` (透明)；
  - 悬停状态 (Hover): `#3E3E42` (关闭按钮除外)；
  - 按下状态 (Pressed): `#4E4E52`。

### 2.4 特殊按钮与交互规范
- **关闭按钮 (Close Button)**:
  - **全应用标准**: 所有界面（主窗口、面板、对话框、标签块）的关闭按钮必须保持视觉一致性；
  - **背景颜色**: 默认状态固定为 `ErrorRed` (`#e81123`)，持续显示红色高亮；
  - **悬停状态 (Hover)**: `ErrorRed` (`#e81123`)；
  - **按下状态 (Pressed)**: `#A50000`；
  - **圆角**: `4px`。
- **置顶按钮与窗口置顶逻辑 (Pin Button & Window Pinning)**:
  - **激活颜色**: 选中状态下图标颜色必须切换为唯一合法色值 **`#ff551c`** (`ActiveOrange`)；
  - **底层绝对法则**: 窗口置顶逻辑**只许可使用 Win32 原生 API `SetWindowPos(HWND_TOPMOST / HWND_NOTOPMOST)`**！**严禁使用其他任何方式**（严禁使用 `setWindowFlags`，严禁使用 `Qt::WindowStaysOnTopHint` 等），确保置顶时零闪烁、不改变窗口句柄与尺寸，绝对对齐 Windows 平台最高堆叠层级标准！
- **同步按钮 (Sync Button)**:
  - **状态联动**: 存在待同步元数据时，图标强制显示为 `ErrorRed`；同步完成后恢复为 `TextMain`。
- **标题栏按钮交互**: 所有标题栏按钮必须开启 `Qt::WA_Hover` 属性以触发悬停事件。必须安装 `m_hoverFilter` 事件过滤器以支持全局 ToolTip 悬浮提醒。新建按钮 (+) 采用手动 `popup` 菜单模式，严禁使用 `setMenu` 以免破坏图标的绝对居中对齐。

### 2.5 全局 QMenu 菜单规范 (ThemeManager)
- **菜单背景**: `#252526` (边界 `#333333`，圆角 6px)；
- **选中/高亮背景色 (Item Hover/Selected)**: 全局项目与菜单 hover 背景色统一为 **`#3E3E42`**（或 `#2A2D2E`），选中高亮主题蓝色统一为 **`#378ADD`**（15% 透明度，即 `QColor("#378ADD"); bg.setAlphaF(0.15f);`）；侧边栏 (`FavoritePanel`、`LibraryPanel`、`NavPanel`) 与右侧内容区域 (`TreeItemDelegate`、`ThumbnailDelegate`、`ColumnItemDelegate`) 项目选中时背景色必须严格统一在底层绘制 **`#378ADD` 15% 半透明蓝色**，确保图标与文本清晰位于高亮层之上，绝对禁止出现强行覆盖图标的不透明硬色块或硬编码暗灰色 (`#37373D`)；
- **图标与文本间距 (Icon-Text Spacing)**: 图标与文本之间的物理间距统一并精确锁定为 **`10px`**（通过 `QMenu::icon { margin-right: 10px; }` 控制）。

### 2.6 分栏布局初始化与尺寸计算规范 (PanelLayoutManager)
- **`initLayout()` 单一调用收敛**: `PanelLayoutManager::initLayout()` 中，`updateDynamicMinimumSize()` **必须且只能**在 `QTimer::singleShot(0, ...)` 延迟回调函数内部调用唯一一次。
- **严禁同步二次调用**: 严禁在 `initLayout()` 函数末尾同步调用 `updateDynamicMinimumSize()`。由于 `setMinimumWidth()` 会在被调用且当前窗口宽度小于新设最小值时立即撑大窗口，两次调用时机不同计算值不一致会导致应用每次启动时主窗口被莫名撑大。
- **独立场景不连带**: `resetSplitterLayout()`、`setPanelVisible()`、`toggleImmersiveMode()` 等各自独立场景中调用一次 `updateDynamicMinimumSize()` 为正常行为，保持独立性。

---

## 3. UI 交互体验与渲染防抖规范

### 3.1 UI 异步加载与防闪烁规范
- **原则**: 在内容面板（`ContentPanel`）进行异步数据扫描（如物理目录扫描、数据库分类查询）前，**禁止**先行调用 `m_model->clear()`。
- **目的**: 避免在数据就绪前的空窗期内出现“白屏/黑屏”视觉抖动，保留旧数据直至新数据通过 `setRecords` 实现毫秒级原子替换。
- **例外**: 当目标路径列表确定为空（如路径不存在或搜索重置）时，必须执行同步 `clear()` 以反馈真实状态。
- **竞态保护**: 加载流程必须绑定 `m_loadRequestId`。在异步回调中，必须校验回调携带的 ID 是否与当前面板 ID 一致，否则丢弃结果以防止快速切换导致的数据串扰。

### 3.2 缩略图平滑加载规范 (Plan-108)
- **原则**: 针对图形文件（图像、SVG等），在异步加载缩略图期间，`data()` 接口必须返回空图标 (`QIcon()`)。
- **目的**: 拦截 Delegate 的默认图标绘制逻辑，防止出现“系统图标 -> 缩略图”的二段式闪烁抖动。
- **视觉反馈**: `ThumbnailDelegate` 必须通过检测空图标状态，在单元格区域绘制轻量的灰色圆角矩形 (`#3A3A3A`) 作为占位背景，确保从占位态到内容态的过渡平滑且不突兀。

### 3.3 快速预览 (QuickLook) 规范 (Plan-109)
- **拦截机制**: 必须采用“黑名单拦截+白名单准入”的双重防御机制。严禁预览文件夹、安装包 (.msi, .exe), 系统库 (.dll, .sys) 及各类压缩包 (.zip, .7z 等)。
- **画质**: 预览标准图像 (jpg, png, webp 等) 时必须加载全分辨率原图，并全程开启 `SmoothPixmapTransform` 以消除锯齿。
- **性能红线**: `renderImage` 在加载原图前必须检查文件物理大小。若超过 50MB，必须自动降级调用高清缩略图引擎以确保 UI 响应性能。
- **样式**: 预览窗口内的滚动条样式必须严格遵循全局规范：宽度 10px、圆角 3px、背景透明、Handle 颜色对齐 `BorderColor` (#333333)。
- **右键菜单规范与功能完备性**:
  - “打开方式”必须严格委托 `ContextMenuFactory::buildOpenWithMenu` 构建级联子菜单，保持全软件一致；
  - 必须完整集成三大辅助功能项：
    1. **`ColorStripPicker` 颜色选择栏**（包含禁用/取消颜色及 8 种标注颜色）；
    2. **`ContextMenuFactory::buildPinToggleAction` 置顶功能**（感知窗口 `isPinned` 状态，动态切换置顶 / 取消置顶）；
    3. **`LibraryService::instance().buildLibraryAction` 库管理动作**（根据在库状态呈现“添加至‘库’”或“从‘库’中移除”）。
- **Toast 提示定位**: 在 QuickLook 界面设定星级/颜色时，通过 `ToolTipOverlay` 弹出 Toast 提示。`ToolTipOverlay` 坐标以 `QuickLookWindow` 为基准居中定位：水平 X 居中 (`globalTopLeft.x() + (windowWidth - tipWidth) / 2`)，垂直 Y 固定于窗口顶部下方 10px (`globalTopLeft.y() + 10`)。

### 3.4 列视图（ColumnView）双击空白处交互与逻辑规范
- **以“最后一列/当前视角”为锚点**: 在列视图（`ColumnView`）中，无论展开了多少列分栏，在任何列分栏的空白处或视图最右侧的背景留白区域双击左键，始终以最后一列（最右侧列）当前展示的文件夹数据为锚点执行回退上一级。
- **父列高亮持续保留**: 退回上一级（即移除最右侧分栏列）后，上一级列中原先选中的文件夹节点必须**持续保持高亮选中状态**，方便用户无须重新查找即可再次点击进入。
- **逐级精准回退与单列降级**: 每次双击空白处/背景留白处均以当前最后一列数据为基准向上一级逐级退回；当列视图仅剩一列（根列）时，继续双击空白处则自动降级触发全局 `NavigationService::instance().goUp()`。

---

## 4. 检索与线程安全规范

### 4.1 范围感知搜索规范 (Scope-Aware Search)
- 搜索请求统一通过 `CoreController::performSearch` 转发，实时绑定顶部蓝色提示线 (Focus Line) 位置；分类模式下限定于当前分类及子类，导航模式下限定于当前物理磁盘路径及子目录。

### 4.2 媒体提取管道线程安全边界
- 后台提取管道中，任何触碰 `QSvgRenderer` / `QPainter` / `QPixmap` / `QIcon` 等 Qt Gui 模块 API 的代码段，必须用 `DiskMediaExtractor::s_qtGuiMutex` 显式串行化保护。
- Qt Gui 模块 API 不保证线程安全，并发访问会导致内部缓存越界写入引发进程崩溃 (`0xC0000005`)。
- 文件 I/O、哈希计算、数据库读写等与 Qt Gui 无关的部分维持并行。

---

## 5. 上下文菜单控制协调层 (ContextMenu Architecture)

根据 QuarkMeta 标准五层 Clean Architecture 规范，右键上下文菜单属于 **【3. 控制协调层 (Controller / Mediator Layer)】** 的核心职责：

1. **控制层职责与组装解耦**：
   上下文菜单的构建与分发统一由对应的 Controller（如 `ContentContextMenu`）承载。视图呈现层（View）仅负责捕获右键事件并透传坐标，禁止在视图类内部混杂菜单项硬编码与业务分发。
2. **状态感知与响应式动作 (State-Aware Actions)**：
   Controller 在构建菜单项时，必须通过领域层（Domain Service / Core Engine）或模型接口获取当前选中实体的权威状态（SSOT），根据置顶状态、收藏状态、锁状态动态生成精准的语义图标与动作文本。
3. **交互与阻断机制 (Non-Blocking Preview & Modal Actions)**：
   涉及连点、就地预览或连续选色的复杂子菜单（如色彩选择条、图标选择网格），必须采用即时数据更新与流畅退出机制，避免不必要的菜单强行关闭，保障流畅的交互节奏。

---

## 6. 全局快捷键契约与窗口关闭规范 (Keyboard Shortcuts & Window Dismissal Architecture)

为确保系统交互习惯符合桌面应用最佳实践，全系统必须遵循以下**全局快捷键与窗口关闭设计规范**：

1. **`Ctrl + W` 窗口/对话框通用关闭契约**：
   - **统一关闭响应**：全系统中所有独立窗口（如 `QuickLookWindow`）、模态/非模态对话框（如 `TagManagerDialog`）、设置弹窗与自定义 Overlay 浮窗，必须统一将 `Ctrl + W` 绑定为窗口关闭/隐藏/取消（Close / Reject / Dismiss）的标准动作；
   - **焦点感知优先**：`Ctrl + W` 快捷键捕获必须遵循当前获得焦点的最顶层激活窗口（Active Top-Level Window / Focus Widget）优先原则，按下时仅触发当前处于激活状态的窗口关闭，严禁跨层或误关背景主窗口；
   - **零副作用退出**：按下 `Ctrl + W` 关闭窗口时，必须优雅释放该窗口持有的临时资源或事件监听器，确保状态同步更新，不留残余浮窗与僵尸对象。

---

## 7. 对话框与弹窗顶层 HWND 激活与事件循环治理规范 (Dialog & HWND Architecture)

为彻底杜绝弹窗销毁后主窗口失去响应、非客户区 `WM_NCHITTEST` 命中检测失效、标题栏无法拖拽及光标滞留在手型状态等底层 HWND 激活失步问题，全系统必须无条件遵守以下**对话框与弹窗顶层架构治理规范**：

1. **【父级溯源铁律】全软件对话框 Parent 绑定契约 (Root HWND Binding Contract)**：
   全软件所有 `QDialog`、`FramelessDialog` 或 Overlay 浮窗在创建/弹出时，其 `parent` 必须且只能绑定为**顶层 `MainWindow`（或 `window()`）**，绝对禁止将 `DriveBarWidget`、`ContentPanel` 等局部 ToolBar 或 Child Widget 作为 Parent 传入。确保 Win32 消息循环在对话框销毁退栈时，能够 100% 精准恢复主窗口 HWND 的激活 (`SetActiveWindow`) 与输入使能 (`EnableWindow`) 状态。
2. **【模态嵌套禁令】去 `exec()` 阻塞与平原化事件循环 (Flattened Event Loop Contract)**：
   严禁在 `exec()` 运行期间再次嵌套调用二级 `exec()`（如在对话框内部再次弹出 `exec()` 阻断弹窗）。复杂管理面板（如 `TagManagerDialog`）应当向非模态 / Inline 内联编辑交互演进；对话框内部的二次输入/确认交互必须使用嵌入式 Inline 控件或异步响应，确保 C++ 调用栈与 Qt 事件循环始终保持平滑化，消除堆栈交织死锁风险。
3. **【HWND 激活与 Win32 拖拽安全隔离契约】**：
   统一无边框对话框的生命周期与拖拽机制，严格禁止在模态阻塞事件循环中通过 `SendMessage(WM_NCLBUTTONDOWN)` 强抢线程控制权。对话框在 `reject()` / `accept()` 退出时，必须显式做好 HWND 状态清理与光标形态复位（复位为 `Qt::ArrowCursor`），保障主窗口非客户区拉伸与拖拽机制的完美平滑。
4. **【框架级顶层入口自愈契约 (QuarkApplication Self-Healing Framework Contract)】**：
   全系统统一使用自定义 `QuarkApplication` 继承并替代 `QApplication`。通过重写虚函数 `notify(QObject* receiver, QEvent* event)`，在框架事件派发的最源头实时监听全局所有 `QWindow` 及 `QDialog` 窗口的 `QEvent::Close` 与 `QEvent::Hide` 事件。每当任何弹窗或窗口关闭/隐藏的第 0 毫秒，由框架层自动且强制执行全局清场动作：彻底弹栈全局 `QGuiApplication::overrideCursor()`、释放孤儿鼠标抓取 (`releaseMouse()`)、解开 Win32 原生捕获锁 (`::ReleaseCapture()`) 并通过 `QCursor::setPos(QCursor::pos())` 驱动 Windows DWM 布局重新进行 WM_SETCURSOR Hit-Test 判定。全软件所有界面 100% 无感实现框架级自愈。
5. **【架构绝对归一化与零补丁契约 (Unified Clean Architecture & Zero-Patch Contract)】**：
   全软件在面对跨模块横切关注点（如输入焦点收回、鼠标抓取释放、光标形态复位、主题与样式派发）时，**必须且只能通过最顶层的框架门禁（如 `QuarkApplication`）或中介协调层（Mediator）做统一高内聚治理**。绝对禁止在下层具体的子对话框、局部面板或叶子 Widget 控件内部手写“擦屁股”式的碎片化补丁代码，彻底抹除打地鼠式的补丁遗留，从架构源头上降低全生命周期的维护与重构成本。

---

## 8. 文件冲突处理交互与对话框规范 (File Collision Resolution Architecture)

在批量文件粘贴或移动场景下，系统必须遵循标准且高辨识度的文件冲突处理规范：

1. **Adobe Bridge 风格直观冲突处理选项契约**：
   冲突选项包含“自动解析”、“替换”、“跳过”、“取消”四个标准椭圆胶囊按钮：
   - **自动解析**：同时保留目标文件，新写入的文件按标准递增规则编号重命名自动保留（如 `文件 (1).ext`）；
   - **替换**：直接覆盖目标文件夹中的同名文件；
   - **跳过**：忽略当前同名文件，不进行复制/移动；
   - **取消**：终止当前粘贴/移动操作。
   - **全部文件复选框**：左侧提供 `是否应用于全部文件？`，勾选后将当前选项策略应用至同批次所有冲突文件。
2. **全局统一冲突感知与 Apply to All 规则**：
   - 冲突对话框必须顶部清晰呈现当前操作路径与冲突数量 Stack（如 `正在将 X 个项目从 [源目录] 复制到 [目标目录]` 以及 `目标包含 X 个同名文件`）；
   - 提供 `为所有冲突执行此操作` (Apply to All) 动态复选框，勾选后对同批次后续所有同名冲突自动应用相同策略，杜绝连续弹窗打扰用户；
   - 样式与窗口必须继承 `FramelessDialog` 无边框优雅视觉体系，禁止原生窗口风格倒退。

---

## 8.1 跨视图拖拽目标悬停高亮与分栏视图拖拽对齐规范 (Drag & Drop Target Alignment Architecture)

为确保全软件所有内容视图（网格、列表、树状、分栏）在拖拽操作中具备一致且直观的交互体验，必须遵守以下规范：

1. **跨视图悬停高亮统一与 Model 零侵入契约 (Unified Drag Target Highlight Contract)**：
   - 拖拽过程中的悬停目标状态统一通过 `ViewDragDropHelper` 静态维持（`s_hoverView` 与 `s_hoverIndex`），绝对禁止向 `ItemModel` 数据层（如 `setData` / `IsDropTargetRole`）写入临时绘制状态；
   - 悬停目标改变时通过 `ViewDragDropHelper::handleDragMove` 触发对应视图 `viewport()->update()`；拖拽离开 (`dragLeaveEvent`) 或放下 (`dropEvent`) 时必须调用 `clearHover` 即时清理悬停高亮；
   - 所有 Delegate（`TreeItemDelegate`、`ThumbnailDelegate`、`ColumnItemDelegate`）在绘制背景时统一调用 `ViewDragDropHelper::isDropTarget(view, index)` 判定，绘制统一的高亮背景色（`#3498db`，透明度 `0.35f`），确保全视图高亮色彩、透明度与绘制优先级 100% 绝对一致。
2. **分栏视图跨列拖拽目标对齐与原地刷新保护契约 (Column View Cross-Column Drop & In-Place Refresh Contract)**：
   - 分栏视图（Miller Columns 架构）中每一列面板 (`ColumnViewPane`) 在响应 `DropListView::pathsDropped` 拖放信号时，必须明确向 `ContentPanel::onPathsDropped` 与 `ContentFileOpsHandler::onPathsDropped` 传递当前列的专属路径 `targetDirOverride` (`m_path`) 与专属代理模型 `sourceModelOverride` (`m_proxyModel`)，避免目标路径错退回全面板最右侧路径 (`m_panel->currentPath()`) 导致自我拖放阻断失败；
   - **分栏视图级联列展现保护**：拖放异步 I/O 任务完成后，系统必须调用 `weakPanel->refreshAll()`（映射至 `m_columnView->refreshAllColumns()`）进行数据原地重新载入，绝对禁止调用全量 `loadDirectory(...)` 重置列堆栈，彻底保护第 4 列及后续展开的更深层子列不被强制关停或清空。

---

## 9. 分列视图交互与编辑触发控制规范 (Column View Architecture & Edit Trigger Contract)

为确保分列视图（Miller Columns 架构）具备极致流畅、符合桌面系统习惯且与其他视图高一致的交互体验，全系统必须遵守以下**分列视图交互与编辑控制规范**：

1. **编辑触发器彻底封禁契约 (No Edit Triggers Contract)**：
   分列视图 (`ColumnViewWidget` / `ColumnViewPane`) 内部所有子级视图控件 (`QListView`) 必须强制配置 `setEditTriggers(QAbstractItemView::NoEditTriggers)`，彻底阻断 Qt 默认双击或连击唤起行内重命名文本框（`QLineEdit`）的行为，杜绝误触重命名编辑框。
2. **Miller Columns 级联交互与导航契约**：
   - **单击文件夹**：高亮选中当前项目，并即时在右侧级联卡片区域加载并呈现下一级目录列；
   - **双击文件夹**：级联展开右侧子列视图，并同步更新全局当前活动路径，绝不进入行内编辑框；
   - **双击文件**：触发文件激活/打开操作，关闭后级子列并触发关联应用。
3. **分栏视图独立极简单行渲染代理契约 (Column View Dedicated Clean Single-Row Delegate Contract)**：
   分列视图（Miller Columns 架构）物理空间受限，其专用渲染代理 `ColumnItemDelegate` 必须保持绝对极简的视觉呈现。每一项（高度与目录导航 QSS 统一对齐为 28px）采用纯净单行横向布局：左侧 8px 留白、20x20px 图标/缩略图垂直居中绘制（与目录导航图标视觉饱满度 100% 对齐，杜绝系统外壳图标 16px 降级）、中间自适应文件名文本区（带 `ElideRight` 自动省略号）、右侧 20px 仅绘制文件夹级联展开箭头（`chevron_right`）。**严禁在分栏行内额外绘制星级评分、颜色标记圆点或重叠卡片**，彻底避免有限宽度空间内的文本裁剪挤压与视觉碰撞，选中的关联扩展元数据统一在右侧 `MetaPanel` 中全量呈现。
4. **ContentPanel 统一控制器体系融合契约 (Unified Controller Integration Contract)**：
   分列视图 (`ColumnViewWidget`) 必须 100% 深度融合进 `ContentPanel` 的全局控制与状态感知体系，严禁孤立化：
   - **右键菜单与快捷键**：分列视图内所有子视图控件必须注册 `ContentPanel` 的事件过滤器（挂载 `ContentKeyHandler`），并连接 `customContextMenuRequested` 至 `ContentContextMenu`，全面支持右键菜单、快捷键（`F2` 重命名、`Delete` 删除、`Ctrl+C/V` 复制粘贴、`Space` QuickLook 预览）；
   - **全局选择集与状态同步**：`ContentPanel::getSelectedPaths()` 必须包含分列视图活动列的选择输出，确保属性面板（`MetaPanel`）、状态栏统计与全局导航栏无缝感知当前选择集；
   - **拖拽至收藏夹与跨面板拖拽契约 (Drag & Drop to Favorite Contract)**：分列视图各列控件必须实现拖拽重写类 (`DropListView`) 并继承 `startDrag`，允许用户选择文件/文件夹后将其拖拽至左侧“收藏夹”面板（`FavoritePanel`）、导航栏（`NavPanel`）或外部文件夹，保持与网格/列表视图 100% 同等的拖拽能力；
   - **筛选与元数据感知**：分列视图所有子列必须继承 `ContentPanel` 的 `FilterState`（搜索过滤、隐藏文件显示、类型筛选），并完整配置渲染代理的色彩标记、评级与异步缩略图管线。
5. **视觉精致度与全局导航同步契约 (Visual Polish & Global Sync Contract)**：
   - **无虚线框契约**：分列视图所有列表控件项在选中与聚焦状态下，必须彻底清除虚线焦点框 (`outline: none;`，并在代理绘制时擦除 `QStyle::State_HasFocus`)，保障沉浸平滑的视觉呈现；
   - **地址栏与导航树无损同步**：分列视图展开子目录或选中文件夹时，必须同步通知全局导航服务 (`NavigationService`) 发射 `currentUrlChanged` / `directorySelected` 广播，使地址栏 (AddressBar) 与导航树 (NavPanel) 实时反映当前选中列的完整最新路径；同时 `ContentPanel` 在分列视图模式下必须阻止无意义的全列重置渲染，保障级联列堆栈的平滑展开；
   - **图标与缩略图管线加载**：分列视图每一列完成目录数据载入后，必须即时调用 `loadThumbnailsForRows` 将记录提交至全局 `ThumbnailPipelineService`，加载显示精美矢量/文件缩略图。
6. **分栏视图与筛选器数据统计实时接轨契约 (Column View FilterPanel Integration Contract)**：
   分栏视图（Miller Columns 架构）中每一列均具备独立的目录加载能力。每当分栏视图级联展开新列、最后一列加载完成或用户点击/切换当前活动列（`activePane`）时，系统必须自动捕获当前活动列的文件记录集（`ItemRecord`），驱动统计引擎（`ContentStatsWorker`）重新计算属性、标签、类型与日期分组数据，并通过广播 `directoryStatsReady` 信号与右侧筛选器面板（`FilterPanel`）实时无缝接轨。严禁出现分栏视图内容已更新但筛选器停留在旧目录统计数据的脱节现象。
7. **分栏视图级联导航选中同步与文本截断省略号契约 (Column View Selection Sync & Text Elision Contract)**：
   - **选中高亮即时同步契约**：当从外部面板（如收藏夹 `FavoritePanel`、地址栏 `AddressBar` 或快捷跳转）发起路径定位时，分栏视图除展开并加载对应路径的祖先与目标列外，必须显式在对应列中定位并高亮选中目标数据项，保障活动列与外部导航源选中的物理一致性；
   - **文本尾部省略与箭头排他区域契约**：分栏列表（`QListView`）项目名称过长时，必须统一采用 `Qt::ElideRight`（尾部 `...`）进行文本自动截断，右侧强制锁定 20px 独立画廊区域用于绘制文件夹级联指示箭头 (`chevron_right`)。禁用不必要的水平滚动条 (`ScrollBarAlwaysOff`)，杜绝横向滚动条盖住列表底部项右侧箭头的视觉缺陷。
8. **分栏视图与主视图模式切换数据模型无缝同步契约 (Column View ViewMode Switch Model Sync Contract)**：
   - 网格视图、列表视图和瀑布流视图统一共享全局主模型 `m_diskModel`，而分栏视图采用独立的级联多列模型。当用户在分栏视图模式下选中文件（`fileSelected`）时，`pathNavigated` 必须抛出对应列的文件夹路径（`pane->currentPath()`）而非文件路径，绝对禁止将文件名拼接到地址栏末尾；
   - 当用户从分栏视图切换至网格、列表或瀑布流视图时，`ContentPanel::setViewMode` 必须精准提取 `m_columnView` 最右侧列/激活列（`rightmostPane()` / `activePane()`）的最新实际文件夹路径更新至 `m_currentPath`，并触发自愈重载（`loadDirectory(m_currentPath)`），保障切换回其他视图时深层目录真实数据项 0 毫秒同步呈现，彻底消除弹回旧路径或“显示无项目”的虚假空状态缺陷。
9. **分栏视图行内编辑统一编辑器与智能选区契约 (Column View In-Place Editing & Smart Selection Sync Contract)**：
   - 分栏视图（Miller Columns 架构）的专用渲染代理 `ColumnItemDelegate` 必须彻底告别依赖 Qt 默认 `QLineEdit` 的私自实现，全面归一化接入统一的 `FileNameLineEdit` 编辑器；
   - **智能扩展名保护与按键流转**：分栏视图触发行内重命名时，获取焦点的编辑器必须具备“文件只高亮选中主文件名/自动避开扩展名，文件夹全选”的智能选区逻辑，且必须完整配备统一的按键拦截处理（阻断上下方向键导致 View 焦点漂移，优化左右方向键定位至基名末端）；
   - **应用专属右键菜单与几何对齐**：行内编辑器必须严格遵守系统专属暗色右键菜单契约（带 100% 语义匹配单色矢量图标与 10px 间距），其渲染几何区域必须精确定位在左侧 32px 留白与右侧 22px 级联指示器箭头之间，确保全视图绝对一致的重命名体验与架构纯洁性。
10. **分栏视图祖先路径级联展开、选区高亮保持与图标管线契约 (Column View Ancestor Path Cascade & Icon Pipeline Contract)**：
   - **祖先路径级联展开**：当从收藏栏、地址栏或外部导航跳转至分栏视图时，`setRootPath` 必须向上拆分完整的祖先路径栈（Path Stack），从根目录开始逐级构建多列分栏，并在每一级父列中自动定位并高亮选中指向子目录的项；
   - **祖先列高亮持续保持**：分栏视图在级联展开子列或点击父级列时，只能清除当前列右侧（深层列）的选区与列，必须 100% 保持当前列及其左侧所有父列的选择高亮状态，呈现平滑、连贯的上下文路径链；
   - **图标与缩略图管线加载**：分栏视图所有列在数据装载完成后，必须调用 `loadThumbnailsForRows` 驱动全局 `ThumbnailPipelineService`，且渲染代理 `ColumnItemDelegate` 必须统一读取并绘制 `Qt::DecorationRole` 图标/缩略图，确保与系统全视图风格绝对一致。
11. **分栏视图列分割线与边框视觉契约 (Column View Column Separator & Pane Border Contract)**：
   - 分栏视图（Miller Columns 架构）多列级联呈现时，每一列面板 (`ColumnViewPane`) 右侧必须具备物理明确的垂直分割边框线（右边框宽度 `1px`，暗色中性边框配色 `#2B2B2B` 或 `#333333`，统一声明 `border-right: 1px solid #2B2B2B;`）；
   - 通过列间右边框的物理隔离与布局间距精细化设定，确保多列级联并排时展现清晰、规整的层级视觉边界，消除列间视图粘连感与视觉漂移。
12. **分栏视图延伸留白区布局剥离与独立定位契约 (Column Blank Canvas Layout Decoupling Contract)**：
   - 最右侧延伸留白区 (`ColumnBlankCanvasWidget`) 必须彻底从 `ColumnViewWidget` 的主列布局 `m_layout` (`QHBoxLayout`) 中物理剥离，改由 `updatePaneWidths()` 统一进行绝对几何定位 (`setGeometry`) 与滚动条总边界支撑 (`m_container->setMinimumWidth(...)`)，禁止作为假拟“列”混入 `m_layout` 中；
   - 彻底取消留白区的左键单击清空选区行为，避免误触造成选择集丢失。
13. **分栏视图上下文菜单目标强绑定与视口坐标映射契约 (Column View Context Menu View-Target Binding Contract)**：
   - 上下文菜单请求必须在触发时显式强绑定对应的视图实例（`QAbstractItemView* view`），严禁依赖全局猜测（如 `activeItemView()`）；
   - 无论在列内文件下方空白处、空列内任意位置还是最右侧延伸留白区单击右键，坐标必须转换为目标视图 `viewport()` 的局部坐标系并一同传递给 `ContentPanel::onCustomContextMenuRequested(view, pos)`，彻底消除跨列误选中与弹窗错位缺陷；
   - 对 `onCustomContextMenuRequested` 重载槽函数的信号连接统一使用 Lambda 包装，物理避免 MSVC `QObject::connect` 模板推导二义性（C2665）错误。
14. **分栏视图快捷键归一化与激活列路由契约 (Column View Keyboard Shortcuts Routing Contract)**：
   - 在分栏视图模式下，`Ctrl+V` (粘贴) 与 `Ctrl+Shift+N` / 新建命令必须动态感知并路由至当前激活列的路径 `activePane()->currentPath()`，禁止误粘贴或新建在最左侧根目录；
   - `Backspace` (退格键) 与 `Left` (左方向键) 必须拦截并路由至 `columnView()->goUpColumn()`，平滑裁撤收起最右侧子列，禁止触发全局顶层路径跳转。

15. **Version-Old-6 经典列视图顶层设计理念与架构契约汇总 (Version-Old-6 Column View Master Specification)**：
   - **单层 HBox 级联展开与祖先路径栈 (Ancestor Path Stack)**：所有列面板 (`ColumnViewPane`) 挂载于单层 `QHBoxLayout` 中。传入深层路径时向上拆分完整的祖先路径栈，从根目录开始逐级构建多列分栏，并自动定位高亮选中子目录；展开子列后父列对应项通过 `IsParentExpandedRole` 保持深青灰高亮 (`#334455`)；
   - **文件夹/文件物理双分栏与折叠 (Dual-Section Panel)**：每个列面板内部由 `DualSectionPanel` 承载，拆分为上方文件夹专用列表（`m_folderProxyModel`）与下方文件专用列表（`m_fileProxyModel`），配合折叠标头 (`FolderSectionHeaderBar`) 根据项目数自适应动态计算列表高度；
   - **智能等比例宽度均分与延伸留白画布 (Smart Width Allocation & Blank Canvas)**：各列设定 230px 刚性最小宽度，视口充足时按 `viewportWidth / count` 自动等均分，最后一列吸收余数像素；最右侧剥离独立的延伸留白区 (`ColumnBlankCanvasWidget`)，支持拖拽放下与双击背景回退；
   - **严格的单击/双击语义分工**：单击文件夹/文件仅选中高亮并切换焦点，不裁撤右侧子列；双击文件夹触发 `folderExpandRequested`，裁撤当前列右侧所有子列并在右侧展开新子列；双击文件触发激活/打开；
   - **双击空白处精准回退降级**：最右侧列或留白区双击逐级关闭最右侧列；中间父列空白处双击裁撤该列右侧所有子列；仅剩最后一列时双击空白处自动降级退回 `computer://`；
   - **极简单行 Delegate 绘制 (28px Row Height)**：每行固定 28px 高度，包含左侧 8px 留白、5px 垂直色条（若有）、18x18px 图标/缩略图、文本区（`ElideRight`）、右侧星级评分及文件夹最右侧 `chevron_right` 级联指示箭头（空文件夹绘制虚线框及 `#41F2F2` 箭羽）。

---

## 10. 选择模型变更与元数据面板中介路由及两段式加载架构规范 (Selection Model & MetaPanel Routing Architecture)

全软件所有内容视图（包括网格视图 `GridView`、自适应视图 `JustifiedView`、列表视图 `ListView` 及分栏视图 `ColumnView`）在与右侧元数据属性面板（`MetaPanel`）联动时，必须无条件遵循以下**顶层中介路由与两段式（同步基础+异步深层）元数据渲染架构规范**：

1. **统一中介者去重防抖路由契约 (Mediator Debounce Routing Contract)**：
   - 所有视图内部的 `QItemSelectionModel` 在发生选择集变更时，禁止绕过控制层直接与 `MetaPanel` 进行跨模块耦合通信，必须统一通过中介协调者（`PanelMediator`）进行信号转发；
   - `PanelMediator` 在接收到选择集变更（`selectionChanged`）信号时，必须引入去重与高频防抖机制（`QTimer` 20ms~50ms 缓冲），避免键盘方向键快速连续滑动或视图切换时高频无效触发 `MetaPanel` 界面全量刷新与重绘，保持极致流畅的交互体验。
2. **基础属性与深层元数据两段式加载契约 (Two-Stage Metadata Loading Architecture)**：
   - **第一阶段（0ms 物理属性同步呈现）**：中介者与 `MetaPanel` 在接收到选中项变更的 0 毫秒内，首先从数据记录（`ItemRecord` / `QFileInfo`）中同步提取并秒级呈现基础物理属性（文件名、类型、物理大小、创建/修改/访问时间、基本评级与色标），避免界面空转或卡顿；
   - **第二阶段（按需/异步深层元数据提取管线）**：涉及 EXIF 图像宽高分辨率、音视频编码参数、色彩调色板（Palettes）提取以及深层媒体属性解析时，必须提交至后台异步管线（如 `MediaExtractorPipeline` / `ThumbnailPipelineService`）进行非阻塞处理，解析完成后异步发射事件局部更新 `MetaPanel`，保障主 UI 线程绝不阻塞。
3. **视图选择集与模型真理源（SSOT）一致性保障与分栏视图数据桥接契约 (Column View Model Metadata Bridge Contract)**：
   - 无论视图形态如何演变（单主模型或分栏多级模型），元数据面板（`MetaPanel`）所展现的星级、颜色、标签、备注与关联网址，必须统一归一化指向系统唯一权威内存真理源（`MetadataManager`）；
   - **分栏模型数据完整性保障**：分栏视图（Miller Columns 架构）所使用的轻量级级联模型（如 `DiskItemModel`）在完成磁盘文件初始扫描后，必须防抖或按需从全局真理源（`MetadataManager`）填充及更新绑定的标签、星级评级、色彩与备注扩展元数据，禁止将未绑定的空白扩展元数据直接透传给中介层与 `MetaPanel`；
   - **中介层安全熔断与数据库补全**：`PanelMediator` 在响应选择变更时，必须对视图 `ModelIndex` 携带的扩展字段完整度进行校验。当检测到 `ModelIndex` 属于未绑定扩展字段的轻量模型时，中介层必须强制向全局真理源（`MetadataManager`）请求数据补全并驱动两段式加载管线，确保任何视图模式下 `MetaPanel` 关联元数据呈现的 100% 完整与绝对实时一致；
   - 任何在 `MetaPanel` 或视图卡片上发起的元数据更新，必须经由 `CoreEngine` / `MetadataManager` 持久化后，通过事件总线（`CentralEventHub`）广播回视图，确保全视图模式下元数据状态 100% 绝对实时一致。

---

## 11. 视图代理与重命名编辑框架构归一化规范 (Delegate & Rename Framework Normalization Contract)

为彻底解决全软件各内容视图代理（Delegate）在行内重命名功能实现上的逻辑散落、重复造轮子及交互不一致等架构痛点，全软件必须遵循以下**视图代理与重命名编辑框架构归一化规范**：

1. **重命名编辑器物理剥离与高内聚自治契约 (FileNameLineEdit Independence Contract)**：
   - 专门用于行内重命名的文本编辑框 `FileNameLineEdit` 必须具备物理独立的头文件与源文件（`FileNameLineEdit.h` / `FileNameLineEdit.cpp`），彻底消除寄生于特定 Delegate 头文件的历史架构耦合；
   - **控件级交互自治**：编辑框获得焦点时的智能选区（文件夹全选、普通文件避开扩展名高亮选中主文件名）以及键盘事件拦截（吞噬上下方向键以阻断 View 行漂移、智能处理左右方向键定位至基名末端）统一下沉并高内聚于 `FileNameLineEdit` 自身的 `focusInEvent` 与 `keyPressEvent` 虚函数内部。禁止通过代理手动安装全局事件过滤器（`installEventFilter`），保障事件分发效率与架构洁净度。
2. **具备重命名能力的统一代理基类契约 (RenameCapableDelegate Base Contract)**：
   - 全软件所有需要行内重命名能力的视图渲染代理（包括树状/列表视图代理 `TreeItemDelegate`、网格卡片视图代理 `ThumbnailDelegate` 及分栏视图代理 `ColumnItemDelegate` 等），必须统一继承抽象基类 `RenameCapableDelegate`；
   - **编辑生命周期强制统一与编译器锁**：基类 `RenameCapableDelegate` 统一实现并用 `override final` 密封 `createEditor`、`setEditorData` 与 `setModelData` 虚函数。所有具体子类 Delegate 绝对禁止且无法重新覆盖这三个函数，确保全软件重命名编辑框的创建、数据填充与模型提交逻辑 100% 绝对一致；
   - **几何边界隔离**：编辑框的布局呈现与定位边界（`updateEditorGeometry`）保留为子类虚函数，由各视图代理根据各自的卡片/行数物理布局引擎进行针对性精准绘制，实现架构统一与布局灵活度的完美结合。

---

## 12. 多标签页 (Multi-Tab Bar) 与 双窗格 (Dual-Pane Split View) 顶层交互与架构规范 (Multi-Tab & Split View Contract)

为确保 QuarkMeta 在多标签页浏览与双窗格分栏操作中具备现代浏览器级的极致体验与工业级高内聚架构，全软件必须遵循以下规范：

1. **标题栏内嵌多标签页组件 (`TabBarWidget`) 架构**：
   - **标题栏植入与软硬件边距**：多标签页控件 (`TabBarWidget`) 直接内嵌于标题栏 (`TitleBarWidget`) 中间区域，替换原静态应用标题，物理高度锁定为 28px，保持与操作系统控制按钮精致对齐；
   - **标签生命周期与持久化**：全面支持新建标签（`Ctrl+T`）、关闭当前/其他/左侧/右侧标签、复制标签、恢复刚关闭标签（`Ctrl+Shift+T`）及持久化存储（`AppConfig`）；支持全局快捷键切换（`Ctrl+1~9`、`Ctrl+Tab`、`Ctrl+Shift+Tab`）；
   - **交互形态定制**：支持每个标签页单独配置专属色彩标签与矢量图标（选色器/图标网格子菜单），物理存储与状态无缝联动。

2. **双窗格分栏与工具栏按钮契约 (`ContentHeaderWidget` Split Button Contract)**：
   - **分栏按钮位置**：在主/副内容面板顶部 `ContentHeaderWidget` 工具栏右侧嵌入 `columns` 图标按钮 (`m_btnSplitView`)；
   - **单/双窗格动态流转**：在单窗格模式下点击分栏按钮，自动触发 horizontal 双窗格分栏，并优先从 `NavigationHistoryService` 读取上一次打开的文件夹路径加载至副窗格；在双窗格模式下点击分栏按钮或副窗格分栏按钮，自动收起关闭副窗格复原单窗格视角；
   - **等权路由与面板激活**：主/副内容面板处于 50/50 平等权重，通过 `panelActivated` 信号动态绑定 `m_activeContentPanel`，确保地址栏导航、快捷键与菜单命令始终精准路由至当前获得焦点的活动面板。

---

## 13. QuarkMeta 多窗格（分屏）功能设计理念 (Multi-Pane Split View Philosophy)

> 本章节整理自与用户的对话，只记录用户明确提出的设计要求/理念本身，不包含具体的 bug 诊断或代码实现细节（那些另见各自的 Implementation Plan）。
> 用途：作为验收标准与恢复基准——任何时候实现偏离了这里的描述，都视为被破坏，需要按这里的描述改回来，而不是按当时"看起来顺手"的方式改。

### 13.1 窗格数量上限与超限行为
- 一个标签页内最多同时存在 **4 个窗格**。
- 达到上限后，如果用户继续发起"分屏"操作（无论是拖拽标签页到边缘，还是其他分屏入口），**不允许再新建第 5 个窗格**，而是把这次操作携带的新路径，更新到"当前激活窗格"身上——即"用新路径替换现有激活窗格的内容"，而不是创建。

### 13.2 窗格宽度分配
- 无论当前有 2、3 还是 4 个窗格，宽度都必须**自动等分**（总宽度 ÷ 窗格数）。
- 关闭任意一个窗格后，剩余窗格必须**自动重新等分**宽度，不能出现"关掉一个,剩下的还保持原来挤在一起的窄宽度"这种情况。
- 不允许"套娃嵌套"式的实现（即不允许通过多层 `QSplitter` 互相嵌套来凑出多窗格），所有窗格必须挂在**同一层** splitter 下，这样等分和重新分配才有意义、才能统一处理。

### 13.3 关闭窗格的行为
- 关闭窗格是**按需关闭**：用户想关哪个就关哪个，只影响被关闭的那一个窗格，不牵连其余窗格。
- 不是"点一下取消分屏按钮就把所有额外窗格全部清空"，除非用户就是要关的是最后剩下唯一的额外窗格（此时才回到单窗格模式）。

### 13.4 标签页（Tab）与多窗格的关系
- **每个标签页都要有自己独立的多窗格布局**——不同标签页之间的分屏状态、窗格数量、各窗格路径互不影响，互不共享。
- 切换标签页时，应该**完整还原**这个标签页自己保存的分屏布局（路径、窗格数量、哪个窗格是激活的），而不是所有标签页共用同一套"全局唯一"的窗格状态。
- 标签页的标题文字，在分屏状态下，必须显示**所有窗格当前路径的文件夹名**，用 `|` 分隔，格式如：`文件夹A | 文件夹B`。**即使多个窗格当前打开的是同一个文件夹，也要照样重复显示**，例如 `文件夹A | 文件夹A`，不允许因为路径相同就去重合并成一个名字。
- 这个多窗格布局需要能够**跨应用重启持久化**——不是只在本次运行的内存里记着，关闭程序重新打开后，应该恢复到关闭前的分屏状态，而不是回退成单窗格。

### 13.5 视觉与焦点表现
- 分屏状态下，当前被激活/聚焦的那个窗格需要有明确的边框高亮，帮助用户分辨"我现在操作的是哪一个窗格"。
- **单窗格模式下（没有分屏）不需要、也不应该显示这条焦点边框**——只有一个窗格时，高亮它没有任何信息量，纯属多余的视觉干扰，应该完全不显示。

### 13.6 拖拽行为的边界（不同拖拽对象要区分对待）
- 拖拽**标签页**到某个窗格的边缘，触发的是"创建/替换分屏"这个语义——这是分屏功能唯一正当的触发方式。
- 拖拽**文件/文件夹**（无论拖到窗格的什么位置，包括边缘）**永远只是"把文件移动/复制到目标窗格"这个语义，绝对不应该触发创建新分屏或显示分屏专用的高亮提示**。这两种拖拽对象在交互反馈和最终行为上必须严格区分，不能用同一套"贴边判定"逻辑笼统处理。

### 13.7 功能模块必须"跟随当前激活窗格"这一总原则
以下这些功能，只要应用处于多窗格状态，全部必须作用于**当前激活/聚焦的那个窗格**，而不是永远固定作用于最初创建的那个"主窗格"：
- 从目录导航栏（左侧目录树）或收藏夹点击某个路径，应该在当前激活窗格中打开，而不是永远打开在主窗格。
- 顶部搜索框的搜索行为，应该作用于当前激活窗格，而不是永远只对主窗格生效。
- 通过右键菜单或快捷键（如 Ctrl+0~5）对选中项设置星级、颜色标记等操作后，右侧元数据面板必须**实时同步**反映这次操作的结果——不管这次操作发生在主窗格还是任意一个副窗格里。
- 一切"当前激活窗格是谁"的判定，必须有且只有一个统一的来源（不能不同功能模块各自维护一套"我以为现在激活的是哪个"的判断），后续任何新功能只要涉及"对当前操作的窗格做点什么"，都应该接入这个统一的激活窗格判定，不允许再写死指向主窗格。

### 13.8 窗格 (Pane) 与 列视图 (Column View) 概念绝对剥离防混淆铁律
全软件架构设计、类定义与代码实现中，必须严格区分并绝不混淆以下两个不同层级的概念：
1. **窗格 (Pane)**：决定标签页中**内容面板 (`ContentPanel`) 共有几个**（上限 4 个分屏窗格），是顶层窗口容器的分屏布局概念，非决定内容面板内部有几列；
2. **列视图 (Column View)**：指的是**内容面板 (`ContentPanel`) 内部的级联分栏视图模式**（与网格视图、列表视图、瀑布流视图平级），指的是面板内部的列，而非窗格数量。两者属于不同物理层级，在代码与交互定义中**绝对禁止混淆**。

---

## 14. QuarkMeta 数据库元数据与侧车文件混合架构设计理念 (File_ID Sidecar + Global Index Hybrid Architecture Philosophy)

> 本章节整理自与用户的架构研讨与共识确认，作为全软件元数据持久化、全局索引与智能文件夹（Smart Folders）的设计真理源 (SSOT)。

### 14.1 核心架构模式：侧车 JSON + 中心索引库 (Sidecar + Global Index)
QuarkMeta 采用“侧车文件持久化 + 中心数据库索引”的读写分离双引擎混合架构：
- **侧车配置文件 (`.QuarkMeta.json`)**：作为数据持久化的权威真理源 (SSOT)，存在于每个文件夹内部。解决数据便携性、跨设备同步与数据不丢问题，实现“数据随文件走”。
- **中心数据库 (`global.db`) 索引表**：`library_items` / `items_fts` 作为高速查询与全局筛选索引引擎。解决全盘搜索、智能文件夹（Smart Folders）与跨目录标签筛选的微秒级响应问题。

### 14.2 唯一标识符契约 (File_ID Unique Key Contract)
在当前混合架构中，**必须统一采用 `file_id` 作为持久化唯一标识**（在侧车 JSON 与 `global.db` 的 `library_items` 表中均以 `file_id` 作为 Primary Key，物理路径 `file_path` 降级为动态关联索引）：
1. **彻底破除“改名 / 剪切 / 盘符漂移即断链”**：
   - 传统以物理路径 `file_path` 为主键时，移动硬盘盘符变化（如 `D:` 变成 `E:`）或父目录改名会导致中心索引全部断链失明。
   - 引入固化在侧车 JSON 里的 `file_id` 后，盘符漂移或父目录改名时，系统扫描到 JSON 即可通过 `UPDATE library_items SET file_path = ? WHERE file_id = ?` 微秒级完成全量索引路径更新，100% 毫无损伤地保留所有星级、标签、历史记录与智能文件夹匹配。
2. **高效增量对账与去重 (Reconciliation & Deduplication)**：
   - `file_id` 作为 O(1) 的唯一哈希键，`global.db` 与 `.QuarkMeta.json` 之间的对账转化为纯粹的集合匹配（`UPSERT INTO library_items ... ON CONFLICT(file_id)`），以极低开销瞬间清理废弃幽灵条目并同步最新改动。
3. **异地身份识别与多机协同**：
   - 用户通过网盘（如 OneDrive / NAS）同步文件夹时，由于 `.QuarkMeta.json` 中保存了相同的 `file_id`，不同电脑上的 QuarkMeta 提炼索引时能精准识别出“同一文件在不同设备上的映射”，真正打通跨设备元数据协同。

### 14.3 工程细节与异常应对红线
1. **`file_id` 生成策略（解耦操作系统）**：
   - 绝对禁止使用依赖 Windows 特有 NTFS 物理句柄（FRN）的方式，确保跨设备与跨文件系统通用。
   - **生成机制**：当用户首次给某个文件添加星级、标签等元数据时，由系统自动生成一个 128 位 UUID / Base36 随机唯一字符串写入 `.QuarkMeta.json` 充当该文件的 `file_id`。
2. **资源管理器外部复制粘贴副本冲突处理**：
   - 当用户在 Windows 资源管理器中直接复制粘贴整个文件夹（导致副本 JSON 包含相同 `file_id`）时，QuarkMeta 扫描到新路径若发现 `file_id` 冲突且物理创建时间或物理路径不一致，自动为副本生成全新的 `file_id` 并刷新副本 JSON，保证唯一性不受破坏。

### 14.4 标签词库表纯粹性与全局多对多关联契约 (Tag Vocabulary Isolation & Global M:N Association Contract)
1. **标签词库表纯粹性红线**：
   - `global.db` 中的 `tags` 表必须且只能是**纯粹的全局词库字典表**，仅存储词条名称（`name`）、颜色（`color`）、置顶状态（`pinned`）、使用频次（`use_count`）及时间戳等词汇自身的固有属性。
   - **绝不允许**在 `tags` 词库表本体中直接外键依赖或强耦合嵌入任何业务实体（如文件、文件夹或分类卡片）的特定状态。
2. **全局多对多关联解耦通道**：
   - 数据库层若需要实现标签与物理文件、智能文件夹或分类等实体的多对多检索与过滤，**必须且只能通过独立的全局中间关联表**（如 `file_tags` / `tag_group_items` 等）进行物理解耦映射。
   - 确保词库表（`tags`）作为高内聚字典模块保持 100% 独立，关联关系交由独立的联结表（Junction Tables）承载，禁止混淆词条定义与关系绑定。

---

## 15. QuarkMeta 批量重命名与文件监视静默治理设计理念 (Batch Rename & Watch Suppression Architecture Philosophy)

> 本章节整理自与用户的架构研讨，作为批量重命名预设持久化、枚举字符串映射与文件监视防抖静默机制的设计真理源 (SSOT)。

### 15.1 重命名规则枚举与字符串映射单一真理源 (PresetManager SSOT Mapping Contract)
1. **彻底消除双重映射写两份的结构缺陷**：
   - 规则序列化（`serializeRules`）与反序列化（`deserializeRules`）绝对禁止分别手写独立的 `switch` 或 `if-else` 链条。
   - 必须在 `PresetManager` 内部建立唯一静态双向/对照映射表（`kTypeMappings`），统一涵盖 `Text` ("Text")、`Sequence` ("Sequence")、`OriginalName` ("OriginalName")、`Date` ("Date")、`Metadata` ("Metadata")、`ParentFolderName` ("ParentFolderName") 等全部 `RenameComponentType` 枚举值，新增枚举项时仅需在此单一真理源处扩展。
2. **错误处理与非静默丢弃契约**：
   - 序列化遇到未登记的枚举类型时，调试版本通过 `Q_ASSERT` 报警；
   - 反序列化遇到无法识别的类型字符串时，不得静默吞错丢弃规则，必须通过带 `unrecognizedCount` 输出参数的重载接口向调用方反馈累加计数，并在 UI 触发 `ToolTipOverlay` 友情 Toast 提示，保障用户预设数据的知情权与可靠性。

### 15.2 批量重命名文件监视静默与收口刷新契约 (Batch Rename Watch Suppression & Final Single-Refresh Contract)
1. **批量重命名期间全局监视静默 (Watch Suppression)**：
   - 在触发批量重命名（`performBatchRename`）之前，系统必须通过 RAII 管理器（`SuppressGuard`）对发起窗格及其所有分屏副窗格（`rootPane()->panes()`）调用 `beginWatchSuppress()`，开启监视静默；
   - 静默期间，`ContentChangeWatcher` 拦截所有文件系统改变回调（`onDirectoryChanged`），禁止刷新视图、启动防抖定时器或积累脏数据变动，消除批量重命名执行过程中列表的中途抖动与多次刷新。
2. **防抖队列积压丢弃与代数阻断机制 (Discard Pending & Discard Generation)**：
   - `DirectoryWatchService` 提供 `discardPending(dirPath, includeSubtree)` 接口，在清理 `m_debounceMap` 队列中积压事件的同时，引入线程安全“丢弃代数”计数器（`m_discardGenerations`）；
   - 当异步线程投递的 `directoryChanged` 信号在主线程被回调时，校验其投递代数是否匹配，若不匹配则自动熔断丢弃，确保已投递未执行的改变事件彻底失效。
3. **完成收口与尾部单次刷新**：
   - 批量重命名对话框关闭（`exec()` 结束）后，RAII Guard 自动释放所有窗格的静默状态（`endWatchSuppress()`），并在 `endSuppress()` 归零时自动调用 `discardPending` 清理监视积压；
   - 重命名成功后，对每个关联分屏窗格执行且仅执行一次 `refreshAll()` 刷平，消除任何尾部多余刷新，保证重命名完成后视图状态一致且文件监视无缝恢复。

---

## 16. QuarkMeta 标记目的地与搬运分发架构设计理念 (Marked Folders & Destination Transfer Architecture Philosophy)

> 本章节整理自与用户的架构研讨与共识确认，作为全软件标记目的地管理、文件分发搬运与复合撤销的设计真理源 (SSOT)。

### 16.1 物理搬运归一化与另起炉灶绝对禁令 (Unified Transfer & Zero-Redundancy Contract)
1. **统一物理搬运通道**：
   - 标记目的地的分发搬运（`MarkedFolderService::executeTransferToMarked`）绝对禁止自写 `copyRecursively` 等私有递归复制/删除逻辑；
   - 必须统一路由至磁盘 I/O 服务层 `DiskIoService::executeAsync`，底层调用 `ShellHelper::copyOrMoveItems`，确保侧车 `.QuarkMeta.json` 元数据与 `disk_thumbs` 缩略图缓存随物理文件漫游，与剪切、拖放、右键“移动到...”的行为 100% 物理一致。
2. **“标记”面板右键菜单完整规范与“取消其他标记”契约 (Marked Panel Context Menu Specification)**：
   - “标记”侧边栏面板（`MarkedPanel`）右键菜单必须提供完整、高内聚的桌面级操作集合，包含：
     - **打开与定位组**：`打开文件夹`（激活当前窗格加载）、`在新标签页中打开`（`TabBarWidget` 新建 Tab 打开）、`在“资源管理器”中显示`（`ShellHelper::showInGraphicalShell`）；
     - **标记状态控制组**：`标记为目标文件夹` / `取消标记`、`取消其他标记`（仅保留当前选中的文件夹为标记状态，通过 `MarkedFolderService::setExclusiveMarked(path)` 物理清空其余所有标记）、`从标记历史中移除`；
     - **辅助工具组**：`复制完整路径`（复制到剪贴板）、`刷新`（重新装载并校验标记列表）；
     - **全局清理组**：`取消所有标记`（`clearAll`）、`清空标记历史`（`clearAllFolderRecords`）；
   - **中性单色红线铁律**：右键菜单中所有的矢量图标必须 100% 统一使用 **`#EEEEEE`** 单色（绝不允许使用 `#ff551c` 或其他任何彩着色）。

2. **同盘/跨盘与回收机制**：
   - 分发时所有目标统一采用“复制”模式（`isMove = false`），确保源文件在所有目标完成前不提前消失；
   - 比较源文件路径与目标路径的物理卷序列号（`VolumePathResolver::getVolumeSerialNumber`）。若存在同盘目标且所有目标均成功复制，则调用 `DiskTrashService::moveToDiskTrash` 将源文件移入应用私有回收站；若存在任意一个失败目标或所有目标均跨盘，源文件严格保留。
3. **同源/循环嵌套跳过**：
   - 目标路径为源文件夹本身或源文件夹的子目录时（规范化路径与不区分大小写前缀判定），必须自动跳过该目标并给予明确友情提示。

### 16.2 复合分发撤销与进度条契约 (Compound Undo & Progress Contract)
1. **复合分发撤销命令 (`TransferToMarkedCommand`)**：
   - 整个标记分发搬运操作仅产生并压入一条 `TransferToMarkedCommand` 命令至 `UndoManager`；
   - 撤销时先把各目标实际生成的副本移入应用私有回收站，若源文件曾被回收，则根据 `original_path` 查询最新记录 ID 调用 `DiskTrashService::restoreFromDiskTrash` 还原原文件；若原位置已被占用，自动重命名还原并通过 `ToolTipOverlay` 友情 Toast 提示。
2. **全量进度条与监视静默**：
   - 操作前由 `TaskProgressService` 在主线程立即创建 0ms 视觉响应进度条；
   - 对涉及的内容窗格调用 `beginWatchSuppress()` / `endWatchSuppress()` 开启文件监视静默，待全部工作（复制、回收、元数据漫游、数据库刷新）彻底闭环收口后统一结束进度条与静默。

---

## 17. QuarkMeta 仓储查询模式与 Service 读写对称性架构设计理念 (Repository Pattern & Symmetric Service API Architecture Philosophy)

> 本章节整理自与用户的架构治理探讨与共识确认，作为全软件服务层（Service Layer）与数据访问层（DAO/Repository Layer）设计的真理源 (SSOT)。

### 17.1 Service/DAO 读写对称性铁律 (Symmetric Service API Contract)
1. **彻底消除“重写入/轻查询”不对称现象**：
   - 凡是具备写能力/动作能力（Write / Action）的服务与数据访问模块（如 `DiskTrashService`、`MarkedFolderService`、`PresetTagService`、`LibraryItemDao` 等），必须同步配套完整对称的读/查询能力（Read / Query）；
   - 严禁出现“只能把数据写入/移入，却无法根据 ID、路径或条件查出记录”的单向死胡同设计。
2. **标配查询接口契约**：
   - 每个持久化仓储/服务模块必须标配提供：`getById(...)`、`getByPath(...)`、`getLatestItemByOriginalPath(...)`、`exists(...)` 等标准化查询 API；
   - 确保撤销（Undo）、重做（Redo）、防抖重试、视图反查与依赖溯源等高阶交互能直接复用底层 Read 接口，严禁在 UI / Mediator 控制层手写裸 SQL 拼凑。

### 17.2 渐进式按需治理原则 (Incremental Architecture Refactoring)
1. **按需补齐，平黄渐进**：
   - 严禁进行破坏既有编译契约的大爆炸式整体重写；
   - 在后续承接具体业务需求与 Bug 修复时，随手审计并补齐相关模块缺失的 Query / Read 接口，通过多轮迭代平滑提升全软件 API 的完整性与对称度。

---

## 18. QuarkMeta 悬浮工具浮层事件路由与生命周期治理设计理念 (Overlay Window Event Routing & Dismissal Architecture Philosophy)

> 本章节整理自与用户的架构研讨与共识确认，作为全软件悬浮选择面板（如 `TagSelectorOverlay`、下拉悬浮窗等）的事件拦截、内外判定、失活离场与就地渲染设计的真理源 (SSOT)。

### 18.1 Qt 6 QPA 原生事件流水线与内外判定铁律 (QPA Native Event & Dual-Layer Hierarchy Contract)
1. **QPA 底层原生窗口事件分发事实**：
   - 在 Qt 6 / QPA 架构下，所有来自操作系统的原生鼠标输入（`WM_LBUTTONDOWN` / `WM_LBUTTONDBLCLK` 等）首先投递给顶层窗口的 `QWindow` 实例（即内部实现的 `QWidgetWindow`），随后再由 Qt 窗口系统分发给具体的子 `QWidget`。
   - 当在 `qApp`（`QApplication`）上挂载全局事件过滤器时，过滤器捕获到的第一道事件接收者必定是 `QWidgetWindow`，而绝不是具体的子控件。
2. **双层内外判定铁律（严禁单层回溯）**：
   - 悬浮面板在判定鼠标事件属于“内部”还是“外部”时，**必须且只能同时处理 `QWindow` 原生层与 `QWidget` 控件树两层**，严禁仅依靠 `QWidget::parentWidget()` 进行单层回溯：
     - **第一层（原生窗口层）**：当目标对象为 `QWindow`（`qobject_cast<QWindow*>`）时，校验其是否等于悬浮浮层自身的 `windowHandle()` 或其子窗口/瞬态附属窗口（`transientParent()`）；若匹配则 100% 判定为内部事件并直接放行，绝不可将其误判为外部点击而吞噬自毁；
     - **第二层（控件树层）**：当目标对象为 `QWidget` 时，沿 `parentWidget()` / `parent()` 链向上递归回溯，直至匹配浮层本体；
   - 唯有在两层均未匹配的情况下，方可认定为真正的“外部点击”并执行消费关闭。

### 18.2 Qt::Tool 窗口附属特性与失活判定规范 (Tool Window State & Safe Deactivation Contract)
1. **`Qt::Tool` 附属窗口失活特性**：
   - 带有 `Qt::Tool` 窗口标志且依附于主窗口的悬浮浮窗，其系统级窗口状态在 Qt/Win32 下 `isActiveWindow()` 常态为 `false`（权威激活焦点由宿主窗口维持）。
   - 严禁在 `changeEvent` 中使用简单的 `!isActiveWindow()` 或 `!isAncestorOf(focusWidget)` 作为关闭浮层的条件，否则内部焦点切换、按钮销毁或操作系统窗口刷新将瞬间误杀浮层。
2. **安全离场真理源**：
   - 浮层的失活关闭必须且只能通过以下两类安全通道触发：
     - **真正的全局外部点击**：由 `eventFilter` 捕获到确凿来自浮层外部的鼠标按下；
     - **应用程序级真正失活**：监听 `QGuiApplication::applicationStateChange`，仅在应用整体进入后台（`Qt::ApplicationInactive`）或主窗口真正失去系统焦点时关闭，杜绝内部微观焦点扰动引发的误关。

### 18.3 标签交互就地状态切换契约 (In-Place Tag State Mutation Contract)
1. **彻底废除全量销毁重建模式**：
   - 标签选择浮层在点选标签时，严禁粗暴调用清空布局、全量 `deleteLater` 所有按钮并重新创建的落后做法。
   - 该做法不仅带来内存频繁分配与界面闪烁，更会导致当前获得焦点与点击的控件瞬间物理蒸发，触发系统焦点归零与失活误杀。
2. **就地属性更新与轻量渲染**：
   - 点击标签按钮时，直接在当前 `QPushButton` 上就地切换状态（`setChecked`、`setProperty("selected", ...)` 与图标更新），保持控件树稳定，确保焦点与事件序列平稳无损。

---

## 19. 标题栏标签页组件 (TabBarWidget) 规范化架构与防溢出设计理念 (TitleBar TabBar Architecture & Layout Protection Philosophy)

> 本章节整理自与用户的架构治理探讨与共识确认（路径 B：彻底规范化架构重构），作为标题栏多标签页系统架构设计与重构实施的唯一真理源 (SSOT)。

### 19.1 标题栏安全边界与空间硬锁铁律 (TitleBar Space Protection & Anti-Overflow Contract)
1. **右侧系统工具区绝对安全边界**：
   - 标题栏右侧区域（包括视图模式切换、缩放滑块、盘符折叠按钮、窗口置顶/最小化/最大化/关闭按钮）为全软件核心系统控制区，具有最高优先级空间保护权；
   - 必须在 `TitleBarWidget` 布局中为右侧工具区设立硬性固定尺寸与物理安全边界，严禁被左侧标签栏挤压、压缩或移出可视区域。
2. **标签栏绝对紧凑左靠拢与右侧拖拽留白铁律 (Compact Left-Aligned & Drag Area Protection Contract)**：
   - **绝对紧凑左排布**：标签与标签之间的间距严格锁定为 `2px`，严禁在标签之间产生任何多余弹性间隙或均分空白（严禁散弹枪式分裂间隙）；
   - **新建按钮紧贴尾部**：`+`（新建标签页按钮）必须紧贴在最后一个标签项的右侧尾部（Follower 模式），禁止脱离孤立；
   - **右侧大面积窗口拖拽留白区**：在新建按钮右侧与右侧工具区之间，必须保留强力伸缩弹簧（`addStretch(1)`），将剩余的所有空白统一留给标题栏右侧，既作为视觉留白，也作为无边框窗口鼠标按住拖拽移动窗口的物理把手；
   - **超限方压缩（阶梯压缩）**：少标签时每个标签维持自然的紧凑最大宽度（如 `180px` ~ `200px`），绝不允许在少标签时被均分拉宽撑满全屏；仅当 `标签总数 × 默认宽度 + 新建按钮` 超过标题栏可用最大容量时，才触发等比动态阶梯压缩（平滑压缩至最小 `80px`），压缩至极限仍超出时启动横向水平滚动。
3. **标签页文字按需自然展开铁律 (Content-Driven Zero-Truncation Contract)**：
   - 当标题栏右侧空间充足、留白充裕时，标签项的宽度必须根据标题文本内容自然伸展（`QFontMetrics::horizontalAdvance(title) + 56px` 物理内边距）；
   - **空间充裕时必须 100% 完整展示标题名称，绝对严禁人为固定低宽度上限而触发 `...` 省略号截断**！
   - 唯有在所有标签自然宽度总和突破标题栏安全容量阈值时，方可自适应启动阶梯平滑压缩并在压缩触及极限下限（`80px`）后触发省略与横向水平滚动。
4. **虚拟分类与全景视图感知铁律 (Virtual Category & View State Tab Sync Contract)**：
   - 标签页作为全软件视角核心指示器，绝不仅限于展示物理磁盘路径；
   - 必须全面支持并毫秒级同步展示“库”虚拟分类（如“全部数据”、“其他”、“未分类”等）以及“智能文件夹”等非磁盘目录的真实语义名称与专属矢量图标；
   - 由 `PanelMediator` 在 `categoryPathsSelected` 等虚拟分类选择信号中统一闭环调度 `titleBar->tabBar()->updateCurrentTabTitle(name, virtualUrl)`，彻底终结标签页与内容面板状态脱节。


### 19.2 纯 View 层职责边界与依赖隔离铁律 (Strict View Layer & Dependency Isolation)
1. **彻底斩断底层私通**：
   - `TabBarWidget` 必须坚守纯 View 层定位，仅负责绘制、Tab 状态呈现与用户交互事件派发；
   - **绝对禁止**在 `TabBarWidget` 中直接包含或调用 Domain/Data 层单例与持久化服务（严禁 `#include "FavoriteDao.h"`、`"MetadataManager.h"`、`"AppConfig.h"`、`"CoreEngine.h"`）；
   - 所有右键菜单指令、元数据变更、收藏状态更新及分屏逻辑，必须由 View 抛出标准化信号（或 DTO），统一移交控制协调层（`PanelMediator`）调度处理。
2. **彻底消灭 `findChild<TabBarWidget*>()` 偷指针反模式**：
   - 严禁任何外部业务面板（如 `AddressBar`、`NavPanel`、`FavoritePanel` 等）通过 `window()->findChild<TabBarWidget*>()` 跨层下钻获取标签栏指针；
   - 外部对标签页的联动请求（如“新标签页打开”、“切换标签页”、“定位路径”）必须统一通过 `CentralEventHub` 或 `PanelMediator` 的标准化信号槽契约进行分发，维护严格的黑盒隔离。

### 19.3 生命周期差量更新契约 (Incremental Diff & Zero Full Rebuild Contract)
1. **彻底废除全量推倒重建**：
   - 严禁在标签数量或状态变动时使用清空布局、暴力 `deleteLater` 全部控件并重新循环创建的落后做法（严禁全量清空重建）；
   - 该反模式会导致频繁的堆内存碎片、高频对象抖动、瞬时焦点丢失与右键上下文断裂。
2. **基于 Tab ID 的差量增量维护**：
   - 标签栏必须建立基于唯一 Tab ID 的差量更新机制：新开标签仅执行局部 `new` 与插入，关闭标签仅针对性移除目标控件，状态变更仅就地刷新属性（`setActive`、标题与图标），确保控件树长周期稳定。


## 20. 多选批量操作一致性与反退化设计铁律 (Multi-Selection Batch Operation & Anti-Degradation Philosophy)

> 本章节整理自与用户的架构治理探讨与明确共识，作为 QuarkMeta 全局交互行为与操作执行的唯一真理源 (SSOT)。

### 20.1 核心铁律：任何操作若不支持批量操作均为失败设计
1. **天然支持批量处理铁律 (Native Batch Processing Contract)**：
   - 全软件任何交互操作（包括但不限于：右键上下文菜单、快捷键触发、拖拽移动、外部程序调用、元数据标记等），**只要用户处于多选状态（选中项数量 $\ge 2$），必须天然支持批量处理**；
   - **绝对禁止将多选操作静默退化/截断为仅针对单个项目生效**。任何在用户多选时仅提取单项路径（如右键点击行）而忽略整体选区的逻辑，均判定为严重交互反模式与设计失败。
2. **右键菜单“打开方式”批量化契约 (Batch "Open With" Menu Contract)**：
   - 当用户选中多个文件时，右键上下文菜单中的“打开方式”子菜单必须完整保留呈现，严禁在多选时隐藏或仅对单个文件生效；
   - **参数完整传递**：当用户在“打开方式”中选择指定程序或“系统默认程序打开”时，系统必须将所有选中的目标文件完整打包为参数列表（`QStringList`），一次性传递给目标外部应用程序（通过 `QProcess::startDetached(exec, filePaths)`），实现多目标一键批量载入；
   - **类型兼容策略**：
     - 若所选多个文件具有相同扩展名，精准列出该类型对应的推荐关联应用程序；
     - 若所选多个文件属于混合不同扩展名，提供多类型通用程序推荐及“选择其他应用...”接口，选定后同样对全部选中的文件执行批量打开。
3. **API 对称性与批量接口优先铁律 (Batch-First API Symmetry)**：
   - 控制器与工厂接口（如 `ContextMenuFactory`）必须优先提供接收集合类型（如 `QStringList`、`QList<QUrl>`）的批量接口；
   - 单项接口仅作为边界兼容或委托重载，其内部实现统一收敛转发至批量接口处理，杜绝单项与多项逻辑分裂与重复造轮子。


## 21. 全局悬浮反馈组件 (ToolTipOverlay) 位置呈现规范与铁律

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 全局悬浮反馈组件（ToolTipOverlay）位置呈现的唯一真理源 (SSOT)。

### 21.1 核心铁律：禁止跟随鼠标光标移动显示
1. **绝对固定锚定原则**：
   - 全软件内所有通过 `ToolTipOverlay` 触发的浮层反馈、操作提示（包括但不限于：复制名称/路径反馈、粘贴成功提示、移入回收站 Toast、快捷键操作通知等），**严禁跟随鼠标光标位置（`QCursor::pos()`）进行漂移动态渲染**；
   - 彻底杜绝提示框在屏幕中乱窜遮挡鼠标指针视线与操作焦点的交互混乱，建立全局统一、视觉稳定、符合人体工程学的反馈锚点。
2. **唯一物理位置计算公式 (SSOT)**：
   - **目标锚定控件**：自动锚定当前活跃窗口内的内容面板 `ContentPanel`（或显式传入的 `targetWidget`）；
   - **水平坐标（X）—— 面板正中水平居中**：
     $$\text{X} = \text{PanelGlobalTopLeft.x}() + \frac{\text{PanelWidth} - \text{ToastWidth}}{2}$$
   - **垂直坐标（Y）—— 内容标题栏下方 10 像素**：
     - 若内容面板内存在标题栏 `ContentHeaderWidget` 且处于可见状态：
       $$\text{Y} = \text{HeaderGlobalBottomY} + 10\text{px}$$
     - 若内容面板无标题栏或标题栏不可见：
       $$\text{Y} = \text{PanelGlobalTopLeft.y}() + 10\text{px}$$

## 22. 视图首末项导航快捷键 (Ctrl+Home / Ctrl+End) 与单行输入方向键增强规范

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 视图首尾定位、选区切换及单行文本编辑方向键交互的唯一真理源 (SSOT)。

### 22.1 全局视图 Ctrl+Home / Ctrl+End 契约 (选中并滚动 - Windows Explorer 范式)
1. **全视图统一覆盖原则**：
   - **主内容面板 (ContentPanel)**：网格视图 (GridView)、自适应瀑布流视图 (JustifiedView)、列表视图 (ListView / DropTreeView) 以及分栏视图 (ColumnView / ColumnViewPane)；
   - **左侧导航与侧边栏**：左侧目录树 (NavPanel)、收藏夹面板 (FavoritePanel) 与库面板 (LibraryPanel)。
2. **交互行为定义 (选项 B - 选中并滚动)**：
   - **`Ctrl + Home`**：清除当前其他选区，将焦点与选中状态切换至视图中**第一个真正的有效文件/文件夹项**，并驱动视图调用 `scrollTo` 滚动至视野顶部可见；
   - **`Ctrl + End`**：清除当前其他选区，将焦点与选中状态切换至视图中**最后一个真正的有效文件/文件夹项**，并驱动视图调用 `scrollTo` 滚动至视野底部可见；
   - **Section Header 与虚拟分隔行严格避让**：在查找首项或末项时，必须精准跳过 `SectionHeaderRole` 为 `true` 的分组头（如文件夹分组栏、分类分隔行等）以及非真实物理/数据项，确保选中目标永远是用户可见的真实有效项。

### 22.2 输入框焦点感知阻断铁律 (Editing Focus Protection)
- 当用户在单行输入框、多行文本框（如主搜索栏、重命名编辑框 `FileNameLineEdit`、元数据面板中的备注输入框等）处于编辑打字状态时：
  - `Ctrl + Home` / `Ctrl + End` 必须严格放行给文本编辑控件自身原生事件，触发光标跳转至文本首部/尾部；
  - 绝对禁止穿透或越权触发背景文件列表的选区切换与视图滚动。

### 22.3 单行重命名与文本输入方向键增强规范 (Single-Line LineEdit Arrow Key Enhancement)
- 在单行重命名编辑框（`FileNameLineEdit`）及相关单行输入交互场景中：
  - **按下方向键【上 (Up)】**：立即将光标定位至文本首部（索引 0）并清除文本选区，同时吞噬事件以彻底阻断底层 QAbstractItemView 的行间焦点漂移；
  - **按下方向键【下 (Down)】**：立即将光标定位至文本尾部（`text().length()`）并清除文本选区，同时吞噬事件以阻断底层 View 行切换；
  - **左右方向键**：保持对文件扩展名的智能选区与跳转支持；
  - 从而为桌面用户在单行重命名与输入时提供极致平滑、符合高阶文件管理效率的方向键光标流转体验。

## 23. 侧边栏树视图 F2 行内重命名快捷键契约 (Sidebar TreeView F2 Inline Rename Contract)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 侧边栏树节点重命名交互的唯一真理源 (SSOT)。

### 23.1 核心交互契约与桌面范式对齐
1. **F2 快捷键行内编辑闭环**：
   - 侧边栏收藏夹面板 (`FavoritePanel`) 与库面板 (`LibraryPanel`) 必须在视图事件过滤器中捕获 `Qt::Key_F2` 按键；
   - 当用户选中某个可编辑树节点并按下 `F2` 时，系统必须立即调用 `view->edit(currentIndex)` 激活行内重命名文本框，与右键菜单“重命名”保持 100% 行为对称；
   - 彻底解决由于设置 `NoEditTriggers` 导致键盘 F2 失效的设计缺陷。
2. **系统保护节点阻断铁律**：
   - 在库面板 (`LibraryPanel`) 中，“全部数据”（ID: -1）、“未分类”（ID: -2）等内置固定系统分类项以及系统保留节点，禁止响应 F2 进入编辑；
   - 普通用户自定义分类节点与收藏夹节点均畅通进入行内编辑并即时落库。

## 24. 全局选中高亮归一化契约与禁止另起炉灶铁律 (Global Selection Highlight Normalization Contract)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 全局视图控件选中高亮色彩表现的唯一真理源 (SSOT)。

### 24.1 核心铁律：禁止任何面板搞特殊或另起炉灶
1. **全局统一半透明主题蓝高亮**：
   - 全软件内所有面板，包括但不限于侧边栏（收藏夹 `FavoritePanel`、库面板 `LibraryPanel`、目录导航 `NavPanel`、标记面板 `MarkedPanel`、历史记录 `HistoryPanel`）以及右侧主内容视图（`DropTreeView`、`ThumbnailDelegate`、`ColumnItemDelegate`），其选中项的高亮背景色必须统一严格使用 **`#378ADD` 15% 半透明蓝色**（`rgba(55, 138, 221, 0.15)`）；
   - **彻底废除不透明硬色块**：严禁在个别面板（如 `MarkedTreeWidget`、`HistoryTreeWidget`）中私自引入 `#094771`、`#37373D` 等高饱和度或不透明暗色块，坚决消除视觉分裂，保障全应用设计语言的纯粹性与一致性。

## 25. 硬件指纹多条件 OR 授权通行证与移动硬盘跨设备启动规范

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 桌面应用正版授权校验与便携式移动硬盘跨设备运行的唯一真理源 (SSOT)。

### 25.1 5 选 1 硬件特征宽松准入契约 (Multi-Factor OR Validation)
1. **多条件准入原则**：
   - 为兼顾正版保护安全性与移动办公便携性，系统启动阶段对当前运行环境的 5 项核心硬件特征进行检测：
     1. **CPU ID**：`BFEBFBFF000306C3`
     2. **主板序列号**：`SGH412RF00`
     3. **BIOS 序列号**：`SGH412RF00`
     4. **程序所在硬盘物理序列号**：`NA5360WJ`
     5. **系统盘（C盘）物理序列号**：`PHYS825203NX480BGN`
   - **逻辑关系严格为 OR（或）**：只要当前环境命中上述 5 项中的**任意一项**，系统即判定当前硬件授权合法，立即放行启动主界面。
2. **移动硬盘跨设备便携运行机制**：
   - 当用户将移动硬盘（物理序列号为 `NA5360WJ`）接入到任何外部未授权计算机时，系统通过识别“程序运行路径所在物理硬盘 SN”命中 `NA5360WJ`，依然可以畅通运行主程序，实现真正的“一盘走天下”；
   - 当在授权主机上运行时，即使外接其他未知硬盘，也能通过 CPU、主板、BIOS 或系统盘特征顺利准入；
   - 只有在未经授权的电脑上运行未授权的硬盘拷贝时，才会触发安全拦截并退出。

## 26. 侧边栏历史选项卡交互规范 (Sidebar History Tab Interaction & Context Menu Contract)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 侧边栏历史记录选项卡交互与右键上下文菜单的唯一真理源 (SSOT)。

### 26.1 单击行为规范 (Click Highlight Only, Zero Unwanted Relocation Overhead)
1. **取消单击自动定位**：
   - 用户在侧边栏“历史”面板（`HistoryPanel` / `HistoryTreeWidget`）中用鼠标左键单击列表项时，**严禁触发文件定位**（不发射 `requestLocateFile`，不驱动主内容区 `ContentPanel` 重新载入与定位）；
   - **仅保留选中高亮**：单击操作仅用于在视觉上呈现当前项的选中高亮（遵守全局 `#378ADD` 15% 半透明蓝色 `rgba(55, 138, 221, 0.15)`），避免不必要的磁盘 I/O、缩略图加载与主视图切换开销。

### 26.2 右键上下文菜单完备性契约 (History Context Menu Completeness Contract)
1. **全局统一样式与中性色图标**：
   - 菜单整体采用全局统一暗色菜单样式 `UiHelper::applyMenuStyle(&menu)`；
   - 菜单内所有图标严格遵循系统规范使用统一中性色 `#EEEEEE`。
2. **上下文菜单功能结构规范**：
   右键点击历史文件项时，弹出具备以下完整能力的上下文菜单：
   - **用系统默认程序打开**：调用系统关联程序唤起打开该文件，并记录访问记录；
   - **在“资源管理器”中显示**：严格复用 `ContextMenuFactory::buildShowInExplorerAction(&menu, path, false, this)`（包含中文双引号）；
   - **在 QuarkMeta 中显示**：显式触发主视图定位（`emit requestLocateFile(path)`），让用户在需要时主动按需定位；
   - **打开方式**：严格委托 `ContextMenuFactory::buildOpenWithMenu(&menu, QStringList{path}, this)` 呈现级联子菜单；
   - 分隔符 (`addSeparator`)
   - **添加至收藏夹**：严格复用 `FavoriteService::instance().buildFavoriteAction(&menu, path, this)`；
   - **添加至“库”**：严格复用 `LibraryService::instance().buildLibraryAction(&menu, QStringList{path}, this)`；
   - 分隔符 (`addSeparator`)
   - **复制（Ctrl+C）**：将文件路径写入剪贴板（`ClipboardService::instance().copyItems({path})`）；
   - **剪切（Ctrl+X）**：将文件路径置入剪贴板剪切态（`ClipboardService::instance().cutItems({path})`）；
   - **复制名称**：严格复用 `ContextMenuFactory::buildCopyNameAction(&menu, {path}, this)`；
   - **复制完整路径**：严格复用 `ContextMenuFactory::buildCopyPathAction(&menu, {path}, this)`；
   - 分隔符 (`addSeparator`)
   - **从历史中移除**：调用 `HistoryService::instance().removeHistory(path)`；
   - **清空历史**：调用 `HistoryService::instance().clearAllHistory()`（右键空白处时亦提供此操作）。

## 27. 智能文件夹规则配置视觉规范 (Smart Folder Rule Configuration Contract)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 智能文件夹规则配置对话框 (SmartFolderDialog / SmartFolderRuleRow / SmartFolderValueWidgets) 的唯一真理源 (SSOT)。

### 27.1 星级规则类型命名与纯矢量呈现铁律 (Strict Star Rating Graphic SSOT)
1. **规则类型命名**：
   - 规则类型下拉框（SmartFolderRuleRow 中的 m_typeCombo）第一项名称统一规范命名为 **“星级”**（彻底废除原“评分”旧称）。
2. **彻底杜绝文字拼凑与多余背景**：
   - 规则编辑器中的星级下拉列表框（StarRatingValueWidget），**绝对禁止**出现“1星”、“2星”等阿拉伯数字拼凑文字；**绝对禁止**使用 Unicode 字符（如“★”）替代；
   - **完全不设“无评级”或“0星”项**，下拉选项直接从 1 星依次递增至 5 星；
   - 每一行仅绘制对应数量的浅白/亮色矢量星（1颗星、2颗星、3颗星、4颗星、5颗星，颜色使用 #C5D1DE 或 #EEEEEE），**绝对不绘制未激活的灰色背景星**；
   - 下拉选项的文本一律**清空为空字符串 ""**。
3. **消除图标压缩变形**：
   - StarRatingValueWidget 中的 m_combo 必须显式设置足够的图标尺寸 m_combo->setIconSize(QSize(100, 18))，防止 Qt 原生将整张矢量星排版图等比缩小为不可辨识的微小黑点，确保星星大而清晰、间距均匀舒展。

### 27.2 日历弹出表头舒展尺寸契约 (Calendar Popup Dimension Contract)
1. **防止表头挤压截断**：
   - 针对单日期输入框 (SingleDateValueWidget) 与双日期范围输入框 (DateRangeValueWidget) 中的 QDateEdit，其弹出的内部 QCalendarWidget 必须保证充足的宽度 (setMinimumWidth(260))；
   - 彻底解决由于宿主输入框较窄导致弹出日历控件被强制压缩、顶部星期表头（一至日）被挤截断为 ... 的视觉残缺缺陷，保障周一到周日及日期单元格完整舒展展示。

## 28. 智能文件夹图像颜色拾色器与内容面板实时联动规范 (Smart Folder Color Picker & Live Content Preview SSOT)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 智能文件夹图像颜色选择及规则编辑实时反馈的唯一真理源 (SSOT)。

### 28.1 图像颜色通用色轮/拾色器规范 (Universal Color Wheel & Tolerance Picker)
1. **废弃死板固定色块下拉框**：
   - 彻底废除原智能文件夹中仅有 8 种固定色彩的下拉列表框（`QComboBox`）；
   - 采用通用的拾色机制，以独立的颜色色块按钮（`m_colorBtn`）展示当前颜色 Hex 与悬停提示，点击弹出系统标准 `ColorPicker`（支持 HSV 色盘、预设色板、自定义 Hex 填写以及准确度/容差滑条联动调节）；
   - 规则值依然遵循 `SmartFolderRule::value` (Hex) 与 `tolerance` 的统一接口规范，确保底层数据库检索（`SmartFolderDao`）的无缝兼容。

### 28.2 智能文件夹规则配置即时联动闭环 (Live Reactive Content Panel Preview)
1. **所见即所得的交互响应**：
   - 在智能文件夹新建或编辑对话框（`SmartFolderDialog`）中，用户对任何规则条件的修改（增减规则、修改类型、切换逻辑运算符、修改值或容差）均会触发 150ms 防抖定时器；
   - 对话框对外发射 `rulesPreviewRequested(const SmartFolderRecord& record)` 信号，通过 `LibraryPanel` 中继至 `PanelMediator`；
   - `PanelMediator` 接收到实时预览请求后，即时调用 `SmartFolderDao::getMatchingPaths(record)` 检索匹配的文件路径，并调度主界面活跃内容面板 `ContentPanel::loadLibraryPaths` 加载展示，让用户在未点击“保存”前就能实时预览规则过滤结果；
   - 对话框若被“取消”关闭，内容面板自动还原至打开对话框前的历史展示状态，形成完整的交互保护闭环。

## 29. 智能文件夹元数据变化入库实时自动更新规范 (Smart Folder Metadata Live Reactive Refresh Contract)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 智能文件夹元数据（评分、颜色、标签等）变动后自动刷新左侧计数与右侧内容列表的唯一真理源 (SSOT)。

### 29.1 库表变更通知机制与源头收敛 (Library Sync Notification SSOT)
1. **单一通知出口**：
   - `LibrarySyncService` 是元数据写入 SQLite 库表 `library_items` 的唯一同步服务；
   - 新增 `void libraryItemsSynced();` 无参信号，作为元数据入库完成的唯一权威广播出口；
   - 发送时机：后台工作线程完成 `LibraryItemDao` 数据库写入后，通过 `QMetaObject::invokeMethod(this, ..., Qt::QueuedConnection)` 投递至主线程发射；
   - 覆盖写入场景：`syncPaths`（一次任务无论分多少批仅在全部完成且至少一批成功后发射一次）、`removePaths`、`ItemRenamed`、`FolderMigrated`、`ItemRemoved`；
   - 容错保护：若某一批 `upsertItems` 写入失败返回 `false`，则不为该批次发送信号，并输出 `qWarning`（包含该批首个路径与批大小）；
   - 写入路径零污染：严禁在评分/颜色/标签的触发源头（`CoreEngine`、`MetaPanel`、`MetadataManager` 等）中随意添加刷新调用。

### 29.2 智能文件夹批量只读计算 (Batch Read-Only Smart Folder Calculation)
1. **连接复用与忙等待保障**：
   - `SmartFolderDao::countMatchingItemsBatchReadOnly(const QString& dbPath, const QList<SmartFolderRecord>& records)` 建立唯一的批量只读统计通道；
   - 仅建立一个只读数据库连接 (`SQLITE_OPEN_READONLY`)，配置 `sqlite3_busy_timeout(db, 3000)` 防止锁争用超时；
   - 循环统计各智能文件夹，规则为空时正常返回计数 `0`，执行失败或数据库打开失败的条目不混入返回结果；
   - 物理清除单条旧接口 `countMatchingItemsReadOnly`，绝不保留死代码与重复轮子。

### 29.3 侧边栏计数防抖与状态隔离 (Sidebar Count Debounce & Model Guard)
1. **防抖与异步代数校验**：
   - `LibraryPanel::refreshSmartFolderCounts()` 作为公共刷新接口，内部通过 150ms 单次定时器合并高频触发；
   - 定时到期后，读取智能文件夹列表与批量计算均在后台线程（`QtConcurrent::run`）执行，跨线程回到主线程时严格通过代数序号（`m_currentCountReqId`）与 `QPointer` 守卫进行过期丢弃；
   - 仅在计数值发生物理变化时更新项的 `Qt::UserRole + 9`；返回结果中未包含的智能文件夹严格保持原计数值，禁止重置为 0；
   - 禁止调用 `loadLibrary()`，禁止重建树，禁止重置选中项与展开节点；
   - 计数更新写入 Model 期间，显式屏蔽 `m_model->itemChanged` 中的智能文件夹改名落库逻辑，消除无谓的数据库读写风暴。

### 29.4 内容面板规则快照、唯一加载入口与无感差异刷新 (Smart Folder Snapshot & Live Content Refresh)
1. **唯一加载入口与规则快照**：
   - 统一收敛为 `ContentPanel::loadSmartFolder(const SmartFolderRecord& record, const QString& displayName)`；
   - 调用 `applyViewTarget("smartfolder://<id>", "smart_folder", displayName, -20, false)`，并在 `m_activeSmartFolder` 成员中保存当前规则快照（实时预览未保存规则亦走此入口，确保后续刷新完全基于快照而非读库）；
   - 在 `applyViewTarget` 中，当 `categoryType != "smart_folder"` 时自动清空 `m_activeSmartFolder`；
   - 数据库规则匹配（`getMatchingPaths`）全部放入后台线程执行，彻底消除主线程卡顿；
   - 彻底废除 `PanelMediator`（选中与预览）及 `ContentDataLoader`（路径导航）中在主线程同步调用 `getMatchingPaths` 的三处分散实现。
2. **差异检测与无感恢复**：
   - `ContentPanel::refreshSmartFolderIfShowing()` 仅在当前处于智能文件夹视图且 `m_activeSmartFolder` 有效时运行；
   - 后台线程执行规则匹配，代数计数丢弃过期任务；
   - 回到主线程后进行路径集合等价性比对（忽略大小写、统一路径分隔符、不关心顺序）：路径完全一致时不重载、不闪烁；路径发生变动时暂存选区，通过 `loadLibraryPaths` 加载并由 `restoreSelections` 自动恢复选区；
   - `refreshAll()` 智能文件夹分支改为使用 `m_activeSmartFolder` 重新计算并整体重载。
3. **中枢单向桥接与视角隔离**：
   - `PanelMediator::setupConnections` 中将 `LibrarySyncService::libraryItemsSynced` 信号连接至 `LibraryPanel::refreshSmartFolderCounts()` 以及主窗格及所有副窗格（`rootPane()->panes()`）的 `refreshSmartFolderIfShowing()`；
   - 严格物理隔离：刷新逻辑仅在面板当前处于智能文件夹时生效，绝对不干扰磁盘目录视图、库分类视图与回收站视图。

## 30. 历史面板多选与批量操作规范 (History Panel Multi-Selection & Batch Operations Contract)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 侧边栏历史选项卡多选与批量操作的唯一真理源 (SSOT)。

### 30.1 历史面板 (HistoryPanel) 多选与批量操作一致性规范
1. **多选模式 (ExtendedSelection)**：
   - `HistoryPanel` 中的树控件 `m_treeWidget` 配置为 `QAbstractItemView::ExtendedSelection`，全面支持 Ctrl/Shift 点击多选与鼠标框选；
   - 鼠标悬停事件、拖拽事件与右键上下文菜单均支持多选条目提取（通过 `selectedItems()` 提取所有选中项的物理路径列表，去重去空）；
   - 在多选状态下进行鼠标拖拽时，提取全部选中项目的 `QUrl` 构建拖拽数据，支持批量拖出。
2. **右键上下文菜单全量批量操作映射**：
   - **用系统默认程序打开**：批量打开所有选中的有效文件，并记录访问记录；
   - **在“资源管理器”中显示**：按目录分组批量打开资源管理器窗口并高亮对应项（多于 5 个目录时弹出确认弹窗）；
   - **在 QuarkMeta 中显示**：单选时定位跳转；多选时仅当选中项为 1 个时可用或在多选时置灰，严禁产生多个无序定位冲突；
   - **打开方式**：传递多选文件路径集合调用公共级联菜单 `ContextMenuFactory::buildOpenWithMenu(parentMenu, selectedPaths, this)`；
   - **添加至收藏夹**：复用 `FavoriteService` 批量添加选中的所有路径；
   - **添加至“库”**：复用 `LibraryService::instance().buildLibraryAction(&menu, selectedPaths, this)`；
   - **复制 / 剪切**：调用 `ClipboardService::instance().copyItems(selectedPaths)` 与 `cutItems(selectedPaths)`；
   - **复制名称 / 复制完整路径**：调用 `ContextMenuFactory::buildCopyNameAction(&menu, selectedPaths, this)` 与 `buildCopyPathAction(&menu, selectedPaths, this)`；
   - **从历史中移除**：批量遍历选中的路径，调用 `HistoryService::instance().removeHistory(p)`，并从树控件中批量移除条目。


## 31. Ctrl+N 快捷键全局绑定新建 Markdown 文件规范 (Ctrl+N Create Markdown Contract)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 新建 Markdown 文件快捷键的唯一真理源 (SSOT)。

### 31.1 快捷键触发与焦点隔离
1. **全局快捷键注册**：
   - 在 `AppShortcutController` 中注册 `Ctrl+N`（`QKeySequence(Qt::CTRL | Qt::Key_N)`）；
   - 严格进行输入焦点隔离校验：只有在非文本编辑焦点（`!isEditingFocus()`）时才响应并发出 `createNewMdRequested()` 信号；
2. **中枢单向调度 (SSOT)**：
   - `PanelMediator` 监听 `createNewMdRequested`，并获取当前主窗格活跃的内容面板 `target`；
   - 调用标准既有接口 `target->createNewItem("md")`，无缝创建 Markdown 文件并在视图中进入行内重命名模式。

## 32. QuickLookWindow 无条件双击关闭与删除后回环跳转规范 (QuickLook Double Click & Cyclic Navigation Contract)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 快速预览窗口交互修复的唯一真理源 (SSOT)。

### 32.1 无条件双击关闭机制
1. **不受内容状态干预**：
   - 双击关闭 `QuickLookWindow` 是顶层交互哲学，严禁受到文件是否存在内容、是否为空白文件或当前处于何种视图（图片、文本、空文件提示标签 `m_lblEmptyPrompt`）的干预；
   - 为 `m_lblEmptyPrompt`、`m_container` 以及窗口自身安装事件过滤器（`installEventFilter(this)`）；
   - 在 `QuickLookWindow::eventFilter` 中统一捕获左键双击事件（`QEvent::MouseButtonDblClick`），只要双击落在预览窗口内容区域，无条件触发 `closePreview()` 并消费事件。

### 32.2 删除项目后回环跳转机制
1. **优先跳转下一个项目**：
   - 当在 `QuickLookWindow` 中触发删除当前项目（`deleteRequested`）时，`PanelMediator` 驱动将其移入回收站后，首先向下寻找下一个相邻有效文件并进行预览；
2. **末项回环至首项并 Toast 提示**：
   - 若被删除的项目已经是当前目录视图中的最后一个有效可预览项目，则必须回环跳转到视图中的**第一个可预览项目**；
   - 回环跳转至第一个项目时，必须通过 `ToolTipOverlay::instance()->showToast("当前处于第一个项目", 2000, QColor("#3498db"))` 给予用户明确且友好的视觉反馈；
   - 仅当整个目录下所有项目均已被删除或不存在任何可预览文件时，才关闭 QuickLook 窗口。

## 33. 从“库”中彻底移除项目 DAO 事务与元数据同步闭环规范 (Remove from Library SSOT Contract)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuarkMeta 将项目彻底从“库”中注销移除的唯一真理源 (SSOT)。

### 33.1 缺陷根因与架构修复原则
1. **废弃企图使用 `addPathsToCategory(0, paths)` 的错误实现**：
   - `LibraryDao::addPathsToCategory` 内部校验为 `if (!db || id == 0 || paths.isEmpty()) return false;`，传入 `id = 0` 会直接被守卫拦截返回 `false`，导致操作直接失败；
2. **构建独立的 `removePathsFromLibrary` SSOT 通道**：
   - 在 `LibraryDao` 中显式提供 `removePathsFromLibrary(const QStringList& paths)` 接口；
   - 内部在全局互斥锁保护下开启 SQLite 事务，执行 `DELETE FROM library_category_paths WHERE path = ?;`，彻底清除所有分类关联；
3. **两级数据与缓存同步闭环**：
   - `LibraryService::removePathsFromLibrary` 调用 `LibraryDao::removePathsFromLibrary` 成功后：
     - 驱动 `LibrarySyncService::instance().removePaths(paths)` 从 `library_items` 表彻底清除项目元数据记录；
     - 发射 `libraryChanged()` 广播全局视图变动与计数刷新；
     - 通过 `ToolTipOverlay::showText` 给予用户绿色成功提示。
