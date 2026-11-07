# Memories_RapidNotes.md - RapidNotes 顶层架构设计理念与历史记忆真理源 (SSOT)

> 本文档由 Jules（资深 C++/Qt 软件架构师与协同工程师）根据用户的最高设计意图、严格共识与《RapidNotes-1.md》排查报告权威整理编写，作为 RapidNotes 全局架构重构、代码规范、五大红线与所有实施方案的唯一真理源 (SSOT)。

---

## 1. 顶层架构治理五大一票否决红线 (Five Hard Architectural Locks)

全软件任何代码修改、功能新增、重构或 Bug 修复，必须无条件遵守以下五大红线，违者直接一票否决判定失败：

1. **算力与资源利用红线（严禁“笨拙且极度浪费计算资源”的手法）**：
   - 严禁采用全表全量重新查询、整个控件树粗暴清空重建、高频全量文件重新扫描、或主线程死循环/Sleep 等笨拙手法勉强实现功能；
   - 必须建立基于增量（Diff / Delta）的高性能更新体系，局部变动仅局部渲染与局部持久化。

2. **接口归一化与批量优先红线（不支持批量操作则绝对不予通过）**：
   - 全软件任何操作功能必须归一化，创建批量写入、读取、查询接口（`QList<int>` / `QStringList` / 批量 DTO）；
   - 严禁在调用方使用 `for` 循环单条串行调用伪装成批量处理，严禁逐条发射广播事件；
   - 凡是不支持批量操作的功能与接口，一律判定为不合格，绝不通过。

3. **打补丁零容忍红线（凡采用打补丁方式则是失败的，绝不可以通过）**：
   - 严禁采用 `singleShot(0)` 延迟调用压制时序竞态、通过临时全局标志位强行闭合逻辑漏洞、通过 Win32 `SetForegroundWindow` / `AttachThreadInput` 强抢焦点等治标不治本手段；
   - 必须深挖并解决根本原因，重构状态机与所有权模型，杜绝一切“碰运气”补丁。

4. **对象生命周期与指针安全红线（不可发生“偷指针”事件）**：
   - 严禁“偷指针”行为：严禁绕过父子层级通过私有成员强行获取裸指针、严禁跨模块窥探或私自持有其他 Widget 的内部子控件裸指针、严禁通过多级 `parent()->parentWidget()->findChild<T>()` 偷取依赖、严禁直接跨线程传递捕获裸指针；
   - 跨模块协作必须基于公开纯粹的 DTO 契约或信号槽解耦；异步回调与跨线程通信必须使用 `QPointer` 弱引用守卫或值语义拷贝。

5. **快捷键架构规范化红线（快捷键必须遵循 QuarkMeta 方式去架构）**：
   - 废除割裂散落的热键实现，采用两级防护与中枢调度架构：顶层 `AppShortcutController` + `LocalShortcutInterceptor`；
   - 强制执行输入态焦点断言 `isEditingFocus()`：在用户打字时不越权截断，原生放行，绝不抢占。

---

## 2. 中央大脑与单向依赖分层架构 (Central Brain & Layering)

1. **中央调度中心 (`CoreEngine`)**：
   - 所有业务逻辑的唯一决策中心，作为单一事实来源 (SSOT) 通道；
   - 引入标准命令结构体 `AppCommand` 与事件广播总线 `CentralEventHub`。
2. **UI 纯化为 Dumb View 容器**：
   - UI 面板（如 `QuickWindow`、`NoteEditWindow` 等）只负责渲染与收集用户交互意图，产生意图后组装 `AppCommand` 投递给 `CoreEngine`，严禁直接直连 SQLite 或修改 Windows 注册表。
3. **分层依赖流向**：
   $$\text{View (UI)} \longrightarrow \text{Mediator / Controller} \longrightarrow \text{Domain / CoreEngine} \longrightarrow \text{Dao / Repository} \longrightarrow \text{SQLite / Disk Storage}$$
   严禁逆向包含与跨层下钻。

---

## 3. 存储底层革新：托管库存储模式 (Managed Library Pattern)

1. **彻底物理移除 SQLite `data_blob` 字段**：
   - 数据库仅存轻量元数据（标题、标签、时间、文件相对路径、SHA-256、大小等）；
   - 物理文件（截图、大图、附件）落入托管库物理目录（`storage/files/{year}/{month}/{hash}.ext`）。
2. **内容寻址去重与异步加载**：
   - 基于 SHA-256 去重，数据库记录指向同一物理存储；
   - 界面列表 0ms 秒开，图片/文件由后台工作线程流水线异步加载，彻底杜绝数据库体积膨胀与 UI 掉帧。

---

## 4. UI 样式架构：外联 QSS 与矢量化契约 (External QSS & Vector Standard)

1. **彻底拔除 C++ 内联样式**：
   - 严禁在 C++ 中调用 `setStyleSheet`；
   - 全面收敛至 `resources/style.qss` 与 `ThemeManager`；通过 `setObjectName` 与 `setProperty` 驱动状态。
2. **矢量图标契约**：
   - 严禁使用 Unicode 文本符号（如 `"★"`、`"×"`）；
   - 统一由 `SvgIcons.h` 引用标准 SVG 矢量路径，保持动态着色与现代桌面端暗色一致性。

---

## 5. 智能文件夹与数据库索引架构规范 (Smart Folder & Database Indexing)

1. **智能文件夹 (`smart_folders`)**：
   - 支持多维动态规则（关键字、星级、颜色、标签、日期区间）；
   - 规则匹配与统计计算必须走后台工作线程（`QtConcurrent::run`）与只读数据库句柄 (`SQLITE_OPEN_READONLY`)，主线程零卡顿。
2. **复合索引构建**：
   - 强制建立覆盖 `is_deleted`、`updated_at`、`category_id`、`rating`、`color` 的高性能复合索引，杜绝全表扫描。

---

## 6. 底层逻辑重构步骤 1：词表与纯函数真理源规范 (Step 1: Vocabularies & Pure Functions SSOT)

根据《RapidNotes_Prompts.md》第一部分与第二部分要求，底层重构第一步必须把散落在各处的判定与数据加工规则归一化为唯一纯函数模块，表结构和 SQL 在此步骤保持不变：

1. **条目类型词表唯一真理源 (`ItemType`)**：
   - `ItemType` 为全工程唯一允许出现 `item_type` 字符串字面量的地方；
   - 包含枚举 `Type`（`Text`, `Image`, `Code`, `Link`, `Color`, `Palette`, `OcrText`, `CapturedMessage`, `PixelRuler`, `DeletedCategory`, 以及旧文件类型中间态 `File`, `Files`, `Folder`, `Folders`, `LocalFile`, `LocalFolder`, `LocalBatch`，后续在步骤 3 合并为 `Path`）；
   - 提供双向无损转换：`QString toString(Type)` 与 `Type fromString(const QString&)`，以及类型分类断言（`isFileOrFolder(Type)`、`isPureText(Type)` 等）。

2. **类型与特征判定唯一真理源 (`ItemClassifier`)**：
   - 消除 `main.cpp`、`NoteModel::data`、`StringUtils::detectItemType` 和 `DatabaseManager` 中分散手写的判定规则；
   - 统一提供高内聚纯谓词：`isUrl(text)`、`isHexColor(text)`、`isRgbTriplet(text)`、`isCodeLike(text)`、`isAbsolutePathText(text)`；
   - 提供两个明确的入口：
     - 入库类型判定：`detectIngestType(text)`；
     - 视图显示图标类别（不得访问磁盘）：`detectDisplayCategory(type, content)`。

3. **路径归并种类唯一真理源 (`PathSummary`)**：
   - 输入路径列表，根据路径类型与数量得出归一化种类：`SingleFile`（单文件）、`SingleFolder`（单文件夹）、`MultipleFilesOrMixed`（多文件或混合）、`MultipleFolders`（多文件夹）；
   - `NoteModel` 界面图标判定全部委托给 `PathSummary`，彻底杜绝分散的路径分割与判断。

4. **辅助纯函数归一化 (`WordBucket` / `TagCodec` / `ThumbnailMaker`)**：
   - `WordBucket::calculate(content, itemType)`：字数阶梯计算唯一来源（0, 10..100, 200..1000, 1001），原样承接清洗与阶梯逻辑；
   - `TagCodec`：全工程唯一的标签编码/解码器；拆分统一支持半角逗号 `,` 与全角逗号 `，`（`split(tagsStr)`），拼接严格只用半角逗号加空格 `, ` 或 `,`（`join(tagList)`）；
   - `ThumbnailMaker::makeThumbnail(img, size = QSize(64, 64))`：原样封装 `NoteModel` 现有的 64×64 平滑保持纵横比缩略图生成算法。

---

5. **TagCodec 全工程标签编解码唯一真理源与防孤儿死代码契约**：
   - 全工程所有涉及标签的拆分（`split`）与拼接（`join`），必须统一收敛至 `TagCodec`。严禁在任何业务层、数据层或 UI 层直接调用 `.split(",")`、`.split(QRegularExpression("[,，]"))` 或 `.join(",")`；
   - `TagCodec::split` 统一支持半角逗号 `,` 与全角逗号 `，`，并自动去空与去重；
   - `TagCodec::join` 严格使用纯半角逗号 `,` 连接；`TagCodec::joinDisplay` 专供界面显示，使用半角逗号加空格 `, ` 连接；
   - 任何实施方案中设计的新模块，必须在方案内提供全量既有调用点的精准替换 Diff，彻底清除孤儿死代码与平行实现。

6. **ItemType 与 ItemClassifier 剪贴板与全工程收敛契约**：
   - 剪贴板监控与入库流程（`main.cpp`）中的类型判定（颜色 HEX/RGB、网址 Link、本地绝对路径文件/文件夹检测等）必须统一收敛至 `ItemClassifier` 与 `ItemType`；
   - 界面 ToolTip 与复制展示统一由 `ItemType::isFileOrFolder` 与 `PathSummary` 驱动，杜绝零散的类型字符串比对。

---

## 7. 底层逻辑重构步骤 2：模块拆分与单一职责架构规范 (Step 2: Module Decomposition & SSOT Architecture)

根据《RapidNotes_Prompts.md》步骤 2 要求，彻底拆解并物理删除职责过载的上帝类 `DatabaseManager`，严格在保证“不改动表结构、不改动 SQL 语义”的前提下，将所有能力解耦归入独立专职的模块中：

### 7.1 核心模块与职责矩阵（唯一归属）
1. **`ConnectionManager`**：
   - 唯一职责：打开与关闭数据库连接、配置连接 PRAGMA、管理数据库物理文件；
   - 事务作用域 `TransactionScope`：支持 RAII 事务管理与任意层级安全嵌套，只在最外层执行真实的 `commit` 或 `rollback`。对外写接口遵循“一个写操作恰好一个事务”原则，彻底消灭循环刷盘与无事务裸写。
2. **`NoteRepository`**：
   - 笔记及其关联实体的底层原子读写：增、删、改、状态更新（收藏/置顶/评分/颜色）、排序写入、`recordAccess`、`getNoteById`、`getLastCreatedNoteId`、回收站还原/物理删除；
   - **纯粹数据仓储**：`NoteRepository` 绝对不包含任何业务规则判定，仅执行纯粹 SQL 操作。
3. **`NoteIngestService`**：
   - 所有创建与采集笔记的业务指挥中心：按内容哈希（`content_hash`）去重合并、已有分类优先保留、标签集合并集计算、原标题为通用标题时才替换、`link` 类型自动归入 "Link" 分类、分类预设标签自动追加；
   - 由它调用 `NoteRepository` 的原子写接口完成落库。
4. **`TagRepository`**：
   - 标签增删改查、note_tags 关联维护、全局重命名与批量删除、标签使用频次统计。
5. **`CategoryRepository`**：
   - 分类树的增删改、树形节点迁移、排序更新、预设标签管理。
6. **`CategoryLockService`**：
   - 分类密码设置、密码验证、解锁状态维护，以及锁定分类集合（`lockedCategoryIds`）的内存缓存，分类加锁状态变化时自动失效与重新计算，查询时不再高频读表。
7. **`TodoRepository`**：
   - 待办事项（`todos` 表）的 CRUD 与状态流转。
8. **`NoteQueryService`**：
   - 笔记检索与统计服务：`searchNotes`、`getNotesCount`、`getFilterStats`、`getCounts`、`applyCommonFilters`、`applySecurityFilter`；
   - 封装分页常量（`DEFAULT_PAGE_SIZE = 500`），本步骤保持既有 SQL 语义 100% 相同。
9. **`AppSettings`**：
   - 业务配置唯一真理源：自动归档开关、自动归档目标分类、隐藏加锁分类等，沿用现有 QSettings 的组织名、应用名与键名。
10. **`TagClipboard`**：
    - 标签剪贴板（原 DatabaseManager 中 `s_tagClipboard`）独立管理，互斥锁安全保护。
11. **`DataEvents`**：
    - 全局领域数据变更总线，沿用现有信号名与参数（如 `noteCreated`、`noteUpdated`、`notesDeleted`、`categoryChanged` 等）；
    - 铁律：必须在事务提交（Commit）成功后发射，一个业务操作最多发射一次。
12. **`LicenseService` 与 `LegacyDatabaseRescue`**：
    - 原样搬迁旧版授权判定与旧数据库抢救逻辑，不改动任何核心逻辑与数值。

### 7.2 依赖单向流动与反向依赖解耦铁律
- **拔除核心层对 UI 的逆向依赖**：`src/core` 下任何文件绝对禁止包含 `src/ui` 的头文件（如 `StringUtils.h`、`FramelessDialog.h` 等）；
- **删除操作与剪贴板解耦**：`NoteRepository` 执行删除后发射 `DataEvents::notesDeleted` 信号，`ClipboardMonitor` 通过应用装配层订阅该信号清理内部最后哈希，严禁仓储层反向调用 UI 或剪贴板单例；
- **哈希互斥保护**：`ClipboardMonitor::m_lastHash` 的所有读写与重置操作必须受 `m_hashMutex` 互斥锁保护。

---

## 8. 底层逻辑重构步骤 3：表结构终态与热迁移真理源规范 (Step 3: Database Schema & Migration SSOT)

根据《RapidNotes_Prompts.md》步骤 3 要求，彻底落地数据实体终态，废除临时字符串列，实现标签唯一真理源、路径条目归一化与高效索引体系：

### 8.1 终态数据表结构与字段语义契约
1. **`notes` 表（轻量元数据中心）**：
   - 包含列：`id`, `title`, `content`, `item_type`, `content_hash`, `category_id`, `color`, `rating`, `is_pinned`, `is_favorite`, `is_deleted`, `source_app`, `source_title`, `remark`, `sort_order`, `word_count`, `created_at`, `updated_at`, `last_accessed_at`, `data_blob`, `thumb_blob`；
   - **彻底物理移除列**：`tags`（收敛至 `note_tags` 表）、`file_extensions`（收敛至 `note_paths` 表）、以及未被工程实际读取的 `is_locked`；
   - **路径条目 content 置空规范**：`item_type == 'path'` 的条目，`notes.content` 物理存储为空；读取向界面暴露时，由 `note_paths` 路径记录按 `;` 拼接返回，保持既有上层读取契约 100% 稳定。
2. **`note_paths` 表（路径条目真理源）**：
   - 包含列：`note_id INTEGER`, `ord INTEGER`, `path TEXT`, `is_dir INTEGER`, `ext TEXT`；复合主键 `(note_id, ord)`；
   - `ext` 为纯小写后缀（不带点），文件夹或无扩展名为空。
3. **`tags` 与 `note_tags` 表（标签实体与多对多关联真理源）**：
   - `tags` 表：`id INTEGER PRIMARY KEY AUTOINCREMENT`, `name TEXT UNIQUE`；
   - `note_tags` 表：`note_id INTEGER`, `tag_id INTEGER`, `ord INTEGER`；复合主键 `(note_id, tag_id)`。
4. **终态高性能索引矩阵**：
   - `idx_notes_main_filter ON notes(is_deleted, category_id, is_pinned, updated_at)`；
   - `idx_notes_rating ON notes(rating) WHERE rating > 0`；
   - `idx_notes_content_hash ON notes(content_hash)`；
   - `idx_notes_created_date ON notes(date(created_at))`；
   - `idx_notes_updated_date ON notes(date(updated_at))`；
   - `idx_notes_accessed ON notes(last_accessed_at)`；
   - `idx_note_tags_lookup ON note_tags(tag_id, note_id)`；
   - `idx_note_paths_ext ON note_paths(ext)`。

### 8.2 安全迁移与自动化版本控制 (`SchemaMigrator`)
1. **`PRAGMA user_version` 架构版本治理**：
   - 彻底废止 `system_config` 散乱标志位；
   - 启动时若 `user_version == 0` 且检测到 `notes.tags` 列存在，则判定触发全量数据架构热迁移；迁移成功后提升 `user_version = 1`。
2. **灾难容灾镜像备份（一票否决防线）**：
   - 迁移执行前，必须首先调用 SQLite 原生 `VACUUM INTO` 命令在 `backups/` 目录导出镜像：`pre_migration_<yyyyMMdd_HHmmss>.db`；若备份失败则立即中断。
3. **单事务迁移与完整性自检**：
   - 全量数据热迁移必须且只能在**单一原子事务**内完成；
   - 迁移数据处理：旧标签按 `TagCodec::split` 导入 `tags` 与 `note_tags`；旧文件类型（`file`, `files`, `folder`, `folders`, `local_*`）统一归并并转储至 `note_paths`；
   - **严格数据一致性校验**：迁移后校验笔记总行数与标签集合数量是否 100% 对齐；若不一致则必须回滚并保留原库，通过现有机制报错退出，严禁静默吞食异常。

### 8.3 批量集合式 API 与缩略图后台异步流水线
1. **彻底杜绝循环逐条读写与循环 prepare**：
   - 批量状态更新采用 `UPDATE notes SET ... WHERE id IN (...)`；
   - 分类移动与预设标签追加采用 `INSERT OR IGNORE ... SELECT` 集合式 SQL；
   - 手动排序采用单次 prepare 循环参数绑定。
2. **缩略图后台异步流水线**：
   - 存量图片在迁移完毕后由后台工作线程（独立 SQLite 连接）分批加载原图并通过 `ThumbnailMaker` 生成 64×64 缩略图写入 `thumb_blob`；
   - 每批次一个事务，全部生成完成后单次发射 `DataEvents::noteUpdated` 信号；生成完成前默认显示图片占位图标。

---

## 9. 底层逻辑重构步骤 4：读取路径与模型解耦真理源规范 (Step 4: Query Path & Model SSOT)

根据《RapidNotes_Prompts.md》步骤 4 要求，彻底收敛分散在搜索、统计、计数各处的过滤与排序逻辑，实现统一真理源，并将视图层模型彻底与磁盘 I/O 解耦：

### 9.1 查询服务统一过滤与排序真理源 (`NoteQueryService` & `SortPolicy`)
1. **单一过滤构造器铁律**：
   - `fetchPage`（分页列表）、`count`（条件计数）、`stats`（多维统计）以及手动排序时的当前视图顺序检索，**必须且只能由 `NoteQueryService` 内部唯一的过滤构造器生成 WHERE 子句与绑定参数**；
   - 关键词检索规则集中：按空白拆分关键词，每个词独立且必须命中标题、备注、标签、物理后缀之一；
   - 保持稳定且性能经过验证的标准 `LIKE` 模糊匹配，严禁擅自引入未经验证的复杂全文检索引擎，坚决清除工程中所有声称“已启用 FTS5 触发器”的虚假过期注释。
2. **排序规则集中化 (`SortPolicy`)**：
   - 全软件查询与手动排序共用统一的 `SortPolicy`，严格保留现有各视图（所有分类、今日、昨日、最近访问、回收站等）的排序规则与结果展示。
3. **极速紧凑统计聚合（4 条 SQL 约束）**：
   - `stats` 对过滤结果集执行统计时，严禁使用低效的字符串拼接替换，且 SQL 执行总数**严格限制在 4 条以内**：
     1. 一次紧凑列扫描（覆盖收藏、评分、字数阶梯、颜色、类型、创建日期、更新日期），由 C++ 纯内存单次遍历聚合；
     2. 标签聚合统计一条（JOIN 关联表）；
     3. 物理后缀聚合统计一条（JOIN 路径表）；
     4. 特殊无标签 `__untagged__` 兜底统计一条。
4. **系统分类极速计数 (`systemCounts`)**：
   - 采用 1 条条件聚合语句 + 1 条分类分组语句，一次性获取全部系统计数键（`all`、`today`、`yesterday`、`recently_visited`、`uncategorized`、`untagged`、`bookmark`、`trash`、`cat_<id>`）；
   - 加锁分类集合直接读取 `CategoryLockService` 缓存，严禁每次计数反复 SELECT categories 表。

### 9.2 视图模型纯化与主线程零磁盘访问铁律 (`NoteModel` & `CategoryModel`)
1. **主线程绝对禁止磁盘访问与全量解码**：
   - `NoteModel::data()` 内严禁调用 `QFileInfo`，严禁探测物理磁盘文件是否存在，严禁在渲染列表时解码原始大图 `data_blob`；
   - 文件夹与文件图标完全由 `PathSummary` 与数据库结构化存储的 `is_dir` 决定；
   - 图片缩略图直接读取轻量列 `thumb_blob`（沿用 64×64 平滑缩放与 500 项缓存上限）；
   - 大图数据（`BlobRole` 与 ToolTip 原图）通过 `NoteRepository::getNoteById` 按需懒加载。
2. **清除无用死代码与缓存优化**：
   - 彻底物理删除只写不读、从未生效的冗余成员 `m_tooltipCache`。
3. **分类模型轻量刷新**：
   - `CategoryModel::refresh` 仅查询一次 `systemCounts()` 与一次分类列表；分类加锁状态取自 `CategoryLockService` 内存缓存，不再逐个分类高频下钻读表。
4. **大数据量流式导出 (`streamNotes`)**：
   - 导出大文件与多笔记时，统一使用带回调函数的 `streamNotes` 流式逐行处理，按需加载大字段，杜绝内存雪崩。

---

## 10. SSOT 接口索引文档与全局普查反向核对真理源规范 (SSOT Index & Census Protocol)

根据最新《RapidNotes_Prompts-2.md》通用规则第 6 条、第 7 条与第 8 条要求，重构每一阶段必须严格践行全景索引与控制变量普查：

1. **唯一 SSOT 接口索引文档 (`docs/SSOT接口索引.md`)**：
   - 登记全软件全部对外公开的接口（类、公开函数、信号）；
   - 严格按“事实”分类归纳（以第二部分 2.1 的 SSOT 表为目录结构），绝对不按物理文件分类；
   - 每一条接口必须包含五大要素：接口名与所属模块、它是哪一件事实的唯一来源、用途与适用场景、禁止在别处重复实现的内容、主要调用方；
   - 头文件注释与索引内容严格保持 100% 绝对一致，杜绝任何两处分歧。
2. **动手前全工程普查与完成后反向核对铁律**：
   - 任何步骤动手修改代码前，必须执行覆盖 `src/core`、`src/models`、`src/ui`、`src/main.cpp` 的全量命令检索，统计所有读写点、调用方（文件数与处数），并如实记录于汇报开头；
   - 完成后必须用完全相同的检索命令执行反向核对，证明旧用法彻底归零，杜绝一切盲目推测与部分替换。
3. **消除写盘过多、多份事实与职责过载三大根因**：
   - 每件事实只有一个来源；
   - 一个模块只做一件事；
   - 不增加写盘次数和计算量；
   - 严禁任何特判、兜底分支与兼容壳，凡出现此类倾向必须停下从根上重新设计。

---

---

## 11. 步骤 1 返工规范：词表与纯函数彻底收敛契约 (Step 1 Remediation & Pure SSOT Contract)

根据《RapidNotes 步骤 1 返工清单》，在词表与纯函数阶段必须彻底落实无死角的收敛与零冗余：

1. **`ItemType` 全工程类型词表唯一 SSOT 铁律**：
   - `DatabaseManager.cpp` 必须显式包含 `ItemType.h`，彻底消灭所有手写的类型字符串字面量（如 `"link"`、`"file"`、`"local_file"`、`"local_batch"`、`"folder"`、`"local_folder"`、`"image"`、`"code"`、`"text"` 等）。
   - `extractFileExtensions` 内部使用 `ItemType::fromString` 或 `ItemType::toString` 进行比较；
   - `addNote` 中对 Link 类型的自动分类判断改用 `ItemType::Type::Link`；
   - 类型统计分支（`image`、`code`、`text`、`link`、`file`、`local_file`、`local_folder`、`folder`、`local_batch`）全部改用 `ItemType`；
   - 过滤函数 `applyCommonFilters` 中 SQL 字符串里的类型值（`'image'` 等）必须由 `ItemType::toString(...)` 统一拼入，不得手写；
   - 列名 `"item_type"` 允许保留作为数据库物理字段名，不受类型值限制；
   - `NoteModel.cpp`（`ToolTipRole`、`DisplayRole`、`mimeData`）与 `StringUtils.h`（`copyNotesToClipboard`、`generateNotePreviewHtml`）中所有类型字面量全面改用 `ItemType`，判定范围严格保持逐处一致。

2. **标签规范化与编解码单一真理源 (`TagCodec`)**：
   - 全工程唯一允许对标签执行 `split`、`join`、`joinDisplay` 和去重的地方只有 `TagCodec`；
   - `TagCodec` 新增 `normalize(const QStringList& tags)` 统一实现 trim、去空与保序去重；
   - `DatabaseManager::updateNote` 与 `addTagsToNote` 移除手写 trim + 去重循环，分别经由 `TagCodec::normalize` 并调用 `TagCodec::joinDisplay`（保留现有的逗号+空格格式）和 `TagCodec::join`（保留现有的纯逗号格式）；
   - 全工程严禁出现任何非 `TagCodec` 的标签逗号分割与拼接。

3. **彻底物理拔除 `calculateWordBucket` 转发壳**：
   - 物理删除 `DatabaseManager.cpp` 匿名命名空间中无实质意义的包装壳函数 `calculateWordBucket`；
   - 所有调用点统一直接调用 `RapidNotes::WordBucket::calculate`，消除间接调用与调用栈损耗。

4. **数据迁移零丢失硬性红线（步骤 3 提前锚定）**：
   - 一条文本数据都不能丢，文件本体不迁移；
   - 迁移前必须在 `backups/` 目录使用 `VACUUM INTO` 生成 `pre_migration_<yyyyMMdd_HHmmss>.db` 物理快照备份；
   - 单一数据库事务内完成全部数据复制、字段映射与索引重建，失败则完整回滚；
   - 迁移完成后必须通过 `ATTACH` 机制逐字段自动化校验，比对所有非文件类笔记关键字段，不一致行数必须为 0。


---

---

## 12. UI 样式内联转外联彻底重构契约 (Inline QSS to External QSS Migration Contract)

根据架构治理与用户明确指令（“由内联转成外联时，不可擅自修改样式参数，只做搬迁，不做重写、不做优化、不做重新设计”）：

1. **零篡改搬迁铁律 (Strict Zero-Value-Alteration Migration)**：
   - 彻底拔除 C++ 代码中所有散落的 `setStyleSheet(...)` 内联硬编码；
   - 严禁在搬迁过程中修改任何视觉数值参数（包括但不限于：颜色 Hex/RGB/RGBA、边框粗细与线型、圆角 border-radius、内边距 padding、外边距 margin、字体大小 font-size、最小/最大宽高 min-width/max-width/height 等）；
   - 不做重写、不做所谓“优化”、不做重新设计，只做纯粹的物理搬迁。

2. **外联 QSS 架构规范**：
   - 所有样式统一集中搬迁至 `RapidNotes/resources/qss/dark_style.qss`；
   - C++ 控件统一通过规范的 `setObjectName(...)` 赋予唯一语义化 ID，或通过 `setProperty("state", ...)` / `setProperty("active", ...)` 动态切换状态，并在 QSS 中使用选择器精准匹配（如 `QPushButton#myBtn[state="active"]`）；
   - 在动态状态变化时，通过 `style()->unpolish(widget); style()->polish(widget);` 触发样式重算，替代在 C++ 中动态拼接 `setStyleSheet` 字符串；
   - 若个别动态数值完全依赖运行时坐标或绝对动态计算且无法提取为 QSS（如取色器色相渐变、透明度滑块动态背景），需严格经过架构审计，其余所有固定视觉样式一律外联化。
---

## 13. QuickWindow 侧边栏折叠组件解耦与一体化架构设计理念 (QuickWindow Collapsible Sidebar Architecture Philosophy)

> 本章节整理自用户的核心设计理念指导与明确共识，作为 QuickWindow 侧边栏“我的分类”等折叠组件架构重构的唯一真理源 (SSOT)。

### 13.1 核心设计理念 (Core Design Philosophy)
针对 QuickWindow 侧边栏分类视图组织，彻底告别原有的“普通父子树节点小箭头”或“多控件物理割裂堆叠”模式，全面对齐主程序侧边栏“库”选项卡成熟的折叠架构规范：

1. **表面视觉一体化 (Accordion / Collapsible Box 视觉一体化)**：
   - 侧边栏分类区域呈现为连续、统一的视图流，整体视觉浑然一体；
   - “我的分类 (N)”与“快速访问”作为一级分组标头（Section Header），具备整洁统一的文字排版、清晰的节点计数徽标（如 `(113)`）以及现代矢量折叠箭头（如 `chevron_down` / `chevron_right`）；
   - 折叠组件与下属的具体分类条目在层级与外观上高度契合，呈现出标准的折叠面板容器效果。

2. **实际交互彻底独立解耦 (Hit-Test 区域精确隔离与事件消费)**：
   - **折叠交互绝对独立**：在视图代理（Delegate）的 `editorEvent` 中严格收窄 Hit-Test 命中判定。只有当鼠标点击精确命中折叠箭头有效区域（如 18×18 px 矩形）时，才发出折叠/展开信号，驱动该组展开或收起；
   - **标头空白区域防误触**：点击标头文字、数字或空白处时，事件被框架直接消费（`return true`），绝对不得像普通分类那样触发数据选中、高亮或内容列表刷新；
   - **分类子项自由交互**：展开后的各个具体分类子项（如 RapidNotes、Link 等）保持纯粹独立的业务行为（单选/多选高亮、数据筛选、右键上下文菜单、拖拽移动与状态联动）；
   - **职责单一与扩展性**：标头仅负责视觉分组与折叠状态组织，不挂载任何实体业务数据，彻底杜绝数据与容器的耦合混淆。
---

---

## 14. QuarkMeta 迁移合并至 RapidNotes 核心设计理念与数据真理源 (QuarkMeta Merge Architecture & Data SSOT)

> 本章节记录将 QuarkMeta 的界面与架构能力迁移合并至 RapidNotes 的根本目的与最高设计理念，作为后续一切界面适配、业务解耦与数据重构的唯一真理源 (SSOT)。

### 14.1 根本目的与业务定位 (Core Purpose & Dual-Mode Roles)
将 QuarkMeta 合并到 RapidNotes 里的根本目的：**通过 QuarkMeta 成熟的 MainWindow 界面与多维视图管理能力，更高效、更专业地集中管控 QuickWindow 日常记录与沉淀的笔记数据**。
1. **轻量日常速记 (QuickWindow)**：作为常驻桌面的轻量悬浮入口，专注高效便携的灵感捕获、随手记事与全文快速检索；
2. **专业集中管控 (MainWindow)**：由 QuickWindow 工具栏窗口按钮唤起，提供多标签页、多窗格分屏、分类层级树、元数据检查与多维即时筛选等专业级管理大盘；
3. **数据一体双模共治**：两者外在交互形态虽异，但内在完全对齐、管理且共享同一份笔记数据。

### 14.2 彻底放弃“物理路径”理念 (Abolishment of Physical Path Concept)
QuarkMeta 原版面向本地磁盘物理文件系统（磁盘驱动器、物理路径树、文件冲突、目录监听等），而在合并至 RapidNotes 后，**所有物理路径相关理念必须彻底放弃，全面转变为纯数据库管理**：
1. **路由与导航纯化**：
   - 彻底废除物理磁盘路径（如 `C:\...`、`D:\...`）；
   - 导航协议与面包屑层级严格统一为虚拟数据库路由协议 `library://<categoryId>`（如 `library://-1` 对应全部笔记，`library://-2` 对应未分类，`library://<id>` 对应各级分类）；
2. **面板架构收敛与解耦**：
   - 彻底剔除盘符栏 (`DriveBarWidget`)、物理收藏夹 (`FavoritePanel`)、标记分发 (`MarkedPanel` / `MarkedFolderService`) 以及物理目录树 (`NavPanel`)；
   - 侧边栏仅保留面向分类数据库表（`categories`）的 `LibraryPanel`；
3. **交互实体转变**：
   - 彻底移除磁盘扫描、物理文件碰撞提示 (`FileCollisionDialog`)、文件系统拖拽分发逻辑；
   - 视图中所有卡片展示、星级评分、颜色标记、重命名、拖拽移动与回收站软删除，其实体**严格且仅为数据库中的笔记记录 (`notes`)**。

### 14.3 唯一存储真理源：`inspiration.db` (Storage SSOT)
1. **唯一数据库**：所管理的数据库**绝非 QuarkMeta 原有的 `QuarkMeta.store` 数据库**，而是 RapidNotes 自身的 **`inspiration.db`** 数据库（主要包括 `notes` 表与 `categories` 表）；
2. **零数据冗余与严禁侧车**：
   - 严禁引入或残留 `QuarkMeta.store`、`.QuarkMeta.json` 侧车文件；
   - 严禁在界面层自建第二份缓存副本或重复持久化层，所有数据读取与变更必须单向穿透至 RapidNotes 现有的 `DatabaseManager` 与领域服务中。
