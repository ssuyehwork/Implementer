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
   - **地址栏与标签页**：不再显示任何物理路径，仅严格显示当前选中的数据库分类名称；
2. **面板架构收敛与解耦**：
   - 彻底剔除盘符栏 (`DriveBarWidget`)、物理收藏夹 (`FavoritePanel` 选项卡及相关逻辑代码彻底根除)；
   - 彻底移除物理标记分发 (`MarkedPanel` / `MarkedFolderService`)；
   - 侧边栏仅保留面向分类数据库表（`categories`）的 `LibraryPanel`；
   - “目录导航”栏 (`NavPanel`) 暂不物理删除，标记为 **TODO**，待后续专门重构；
3. **交互实体与快捷键转变**：
   - 彻底移除磁盘扫描、物理文件碰撞提示 (`FileCollisionDialog`)、文件系统拖拽分发逻辑；
   - **创建快捷键语义转换**：
     - `Ctrl+Shift+N`：由原 QuarkMeta 的“创建物理文件夹”转变为“**创建分类**”；
     - `Ctrl+N`：由原 QuarkMeta 的“创建 Markdown 实体文件”转变为“**创建笔记**”；
   - **行内重命名**：重命名交互严格采用行内就地编辑（Inline Rename），禁止使用弹窗对话框；
   - 视图中所有卡片展示、星级评分、颜色标记、重命名、拖拽移动与回收站软删除，其实体**严格且仅为数据库中的笔记记录 (`notes`)**。

### 14.3 视图控制与界面功能精简 (View Controls & Feature Pruning)
1. **视图模式收敛**：
   - MainWindow 界面**仅保留“列表视图 (ListView)”和“网格视图 (GridView)”**；
   - 状态栏上的“自适应”、“列视图 (ColumnView)”按钮彻底移除；
2. **根除物理文件系统特性**：
   - “显示/隐藏隐藏项目”功能彻底根除；
   - “显示子文件夹中的项目”（递归查看物理目录）功能彻底根除；
3. **内容面板标题联动**：
   - 内容面板（ContentPanel）标题栏不再固定显示“内容”，而是动态联动显示为**当前选中的分类名称**；
4. **视觉与操作资产保留**：
   - QuarkMeta 原带的标准色系（包含高对比度暗色调与颜色标签调色盘）必须完整保留，不得丢失；
   - 标准快捷键映射体系完整保留，无缝平移至笔记数据模型上。

### 14.4 唯一存储真理源：`inspiration.db` (Storage SSOT)
1. **唯一数据库**：所管理的数据库**绝非 QuarkMeta 原有的 `QuarkMeta.store` 数据库**，而是 RapidNotes 自身的 **`inspiration.db`** 数据库（主要包括 `notes` 表与 `categories` 表）；
2. **彻底作废侧车文件**：
   - 原 QuarkMeta 的 `.QuarkMeta.json` 侧车机制彻底作废与删除，所有颜色、星级、标签等元数据直接存入 `inspiration.db` 的字段中；
3. **零数据冗余与严禁缓存副本**：
   - 严禁在界面层自建第二份缓存副本或重复持久化层，所有数据读取与变更必须单向穿透至 RapidNotes 现有的 `DatabaseManager` 与领域服务中。
### 14.5 卡片视图呈现与 Notes 虚拟后缀真理源规范 (Card View & Notes Extension SSOT)
1. **卡片中央图标呈现规范**：
   - 卡片中央图标完全对齐 QuickWindow 列表左侧图标（`NoteModel::data(DecorationRole)`、`ItemClassifier` 与 `ThumbnailMaker`）：
     - **图片 (`image`)**：直接显示图片本身的缩略图；
     - **纯文本 (`text`)**：显示 `text` 文本图标；
     - **网址链接 (`link`)**：显示 `link` 链接图标；
     - **代码片段 (`code`)**：显示 `code` 代码图标；
     - **颜色/调色板 (`color`/`palette`)**：显示 `palette` 调色盘图标；
     - **OCR 识别文本 (`screenshot_ocr`)**：显示 `screenshot_ocr` 图标；
     - **剪贴板消息 (`captured_message`)**：显示 `message` 图标；
     - **历史文件/文件夹类**：按单文件（`file`）、单文件夹（`folder`）、多文件（`files_multiple`）、多文件夹（`folders_multiple`）分别显示对应图标。
2. **纯笔记特化虚拟后缀 "Notes"**：
   - 为了便于统一管理与后期多维精准筛选，特地为纯笔记设定后缀名为 **`Notes`**；
   - **卡片左上角徽章 (Badge)**：纯笔记的徽章一律固定显示为 **`Notes`**；
   - **筛选器 (FilterPanel) 与统计引擎联动**：在数据契约层（`ModelContract` / `NoteModel`）中，纯文本笔记返回的扩展名统一为 `Notes`，使得筛选面板的文件类型分组可以直接一键筛选出纯笔记。
3. **卡片底部名称显示与截断规范**：
   - 卡片名称部分显示笔记的标题（`title`）；
   - **截断规则**：若标题过长，**仅显示前 10 个字**（超出部分予以截断），保持卡片布局清爽整洁。

---

## 15. 数据库结构向 Ditto 看齐：废除大文件本体持久化与物理路径引用规范 (Ditto-Style Path Storage & Zero-Blob SSOT)

> 本章节记录 RapidNotes 数据库结构重构的核心设计理念与事实规范，面向剪贴板文件/文件夹条目彻底终结 `data_blob` 写入模式，向 Ditto 架构看齐。

### 15.1 核心设计理念 (Core Design Philosophy)
1. **彻底废除文件本体存库（零 BLOB 铁律）**：
   - 严禁向 SQLite 数据库 `notes.data_blob` 写入任何来自文件系统的大文件、小文件、文件夹的二进制字节流；
   - 彻底解决大文件入库引发的 SQLite 文件膨胀、锁库重写延迟与 UI 主线程掉帧问题；
   - **向 Ditto 架构对齐**：凡是文件类剪贴板对象（`file`, `local_file`, `folder`, `local_folder`, `local_batch`, `files`, `folders` 等），数据库**仅记录文件的物理绝对路径**，不存实体。

2. **字段职责与物理路径规范**：
   - **`content` 字段**：唯一存储文件的绝对物理路径。单文件/单目录直接存绝对路径字符串；多文件/多目录统一以分号 `;` 拼接绝对路径；
   - **`data_blob` 字段**：对于所有文件/路径类条目，此字段恒为 `NULL` / 空 `QByteArray()`。**全软件仅允许纯内存截图/剪贴板位图（`image` 类型且无物理落地）写入位图二进制字节流**。

3. **极速哈希查重计算 (Content Hash SSOT)**：
   - 对于所有文件/路径类条目，计算 `content_hash` 时**严禁读取物理文件内容进行 Hash**；
   - 统一直接对物理路径字符串（`content.toUtf8()`）计算 SHA-256 哈希，实现毫秒级快速查重与去重。

4. **剪贴板系统级还原与回写 (`QMimeData::setUrls`)**：
   - 无论是通过 `StringUtils::copyNotesToClipboard`、`QuickWindow` 还是 `RecentNotesWindow` 向剪贴板回写文件条目：
     - 一律将 `content` 中的绝对物理路径解析并校验本地是否存在；
     - 存在的物理路径转为 `QUrl::fromLocalFile(path)` 并调用 `QMimeData::setUrls(urls)` 写入系统剪贴板（Windows 原生 `CF_HDROP` 格式）；
     - 若物理文件已被外部删除/移动导致有效路径为空，则降级为复制路径纯文本，并给出友好提示，严禁再通过临时目录写入临时文件导出。

5. **存量数据平滑迁移与空间回收 (Schema Migration)**：
   - 在 `DatabaseManager::createTables` 中建立自动化迁移检查项 `migration_20260510_ditto_file_blob_purge`；
   - 执行单事务清空历史文件 BLOB：
     ```sql
     UPDATE notes SET data_blob = NULL 
     WHERE (item_type IN ('file', 'files', 'local_file', 'folder', 'folders', 'local_folder', 'local_batch') 
            OR file_extensions != '') 
       AND data_blob IS NOT NULL;
     ```
   - 在空闲或关闭时通过检查点与 `VACUUM` 回收物理磁盘空间。

---

## 16. 彻底纯文本化数据库与外部媒体资产托管库解耦架构规范 (Pure-Text Database & Media Storage Architecture SSOT)

> 本章节记录 RapidNotes 数据库彻底纯文本化、零 BLOB、建立“Ditto 式物理引用 + 外部图片托管库”解耦架构的核心设计理念与事实规范。

### 16.1 核心设计理念 (Core Design Philosophy)
1. **原则一（数据库彻底纯文本化与零 BLOB 铁律）**：
   - `inspiration.db` 的 `notes` 表彻底停用 `data_blob` 字段；
   - 数据库内**坚决不保存任何文件或图片的二进制本体字节流**，杜绝 SQLite 文件体积膨胀、大字段 I/O 阻塞、WAL 刷盘压力与高频备份卡顿。

2. **原则二（外部文件走绝对物理路径引用）**：
   - 凡是来自文件系统的文件/文件夹条目（`file`, `local_file`, `folder`, `local_folder`, `local_batch`, `files`, `folders` 等），`content` 字段仅记录其绝对物理路径（多路径以分号 `;` 分隔），`data_blob` 恒为 `NULL`。

3. **原则三（纯内存位图走主程序同根目录媒体托管库落地）**：
   - 凡是无物理落地的纯剪贴板截屏/位图（`image`），必须在入库前立即持久化落地保存到磁盘“托管媒体库”中；
   - **托管库路径规范**：严格与主程序同根目录（绿色便携模式），位于 `QCoreApplication::applicationDirPath() + "/media/{yyyyMM}/"`；
   - **命名与去重规范**：对图片字节流计算 SHA-256，命名为 `{hash}.png`。若同名文件已存在则复用，杜绝重复写盘；
   - **入库内容**：将生成的绝对物理路径写入 `notes.content`，`item_type` 记为 `image`，`data_blob` 传空（`NULL`）；
   - **极速查重**：所有条目的 `content_hash` 一律基于 `content.toUtf8()`（即路径或文本内容字符串）计算 SHA-256，实现毫秒级快速查重，严禁对大文件做耗时内容哈希。

### 16.2 存量历史旧数据安全热迁移 (Migration & Zero Data Loss SSOT)
1. **历史图片导出与保全**：
   - 在 `DatabaseManager::createTables` 中部署单事务热迁移 `migration_20260515_media_purge_all_blobs`；
   - 迁移脚本首先检索所有 `item_type == 'image'` 且 `data_blob IS NOT NULL` 的存量记录；
   - 逐一将二进制数据写入托管媒体库 `media/{yyyyMM}/{hash}.png`，并将其生成的绝对路径回填到 `content`；
   - 导出回填完成后，执行全量清空：
     ```sql
     UPDATE notes SET data_blob = NULL WHERE data_blob IS NOT NULL;
     ```
   - 确保历史截图一张不丢，且数据库物理字段彻底纯文本化。

### 16.3 剪贴板系统级双模回写与 UI 渲染规范 (Clipboard & View SSOT)
1. **剪贴板双模回写（全场景兼容粘贴）**：
   - **文件/目录条目**：将 `content` 中的绝对物理路径解析为 `QUrl` 列表，通过 `QMimeData::setUrls()` 写入剪贴板（Windows 原生 `CF_HDROP` 格式）；
   - **图片条目 (`image`)**：校验 `content` 路径有效性并加载 `QImage`，同时调用：
     - `mimeData->setImageData(img)`：确保微信、QQ、浏览器、Word 等聊天与富文本应用能直接粘贴图片本体；
     - `mimeData->setUrls({QUrl::fromLocalFile(path)})`：确保 Windows 资源管理器、桌面能直接粘贴生成该图片文件。
   - 若物理文件失效或已被删除，降级为复制路径文本，并弹出轻量提示“文件已移动或丢失”。
2. **UI 渲染与预览解耦**：
   - `NoteModel`（缩略图、ToolTip 预览）与 `QuickPreview` 不再依赖 `data_blob`，统一根据 `content` 的物理文件路径加载图片；
   - 失效图片统一展示优雅的缺失占位图标，杜绝程序崩溃与白屏。

---

## 17. 独立看门狗守护进程架构规范 (Standalone Watchdog Daemon SSOT)

> 本章节记录 RapidNotes 与看门狗守护程序 `RapidNotesWatchdog` 彻底解耦与独立构建的核心架构设计规范。

### 17.1 核心设计理念 (Core Design Philosophy)
1. **单一可执行目标与进程隔离铁律**：
   - 严禁将 `src/watchdog/main.cpp` 与主程序 `src/main.cpp` 打包编译到同一个 `RapidNotes` 可执行目标中（杜绝多重 main 函数符号冲突）；
   - `CMakeLists.txt` 中必须独立定义看门狗子目标 `RapidNotesWatchdog`：
     ```cmake
     add_executable(RapidNotesWatchdog src/watchdog/main.cpp)
     target_link_libraries(RapidNotesWatchdog PRIVATE Qt6::Core)
     ```
   - 独立生成轻量无 UI 守护可执行文件 `RapidNotesWatchdog.exe`。

2. **生命周期与崩溃重启调度契约**：
   - **守护启动**：主程序 `RapidNotes.exe` 启动并完成单实例监听后，以非阻塞分离进程方式启动看门狗：
     ```cpp
     QString watchdogExe = QCoreApplication::applicationDirPath() + "/RapidNotesWatchdog.exe";
     if (QFile::exists(watchdogExe)) {
         QProcess::startDetached(watchdogExe, {"--pid", QString::number(QCoreApplication::applicationPid())});
     }
     ```
   - **合法退出握手 (`exit_gracefully.lock`)**：主程序在用户主动退出（如托盘退出、`doSafeExit`）时，在应用目录创建锁文件 `exit_gracefully.lock`。看门狗在监测到主进程 PID 终止后，若发现该锁文件则清理锁并静默退出；若无锁文件则判定为主程序异常崩溃，自动延迟拉起 `RapidNotes.exe` 恢复现场。

---

## 18. 基础编译契约与头文件包含闭合规范 (Compilation Contract & Header Closure SSOT)

> 本章节记录 RapidNotes 命名空间作用域闭合、基类显式暴露以及跨模块头文件包含契约。

### 18.1 FramelessDialog 命名空间全局导出
- `FramelessDialogBase.h` 在 `namespace RapidNotes` 中定义了 `class FramelessDialog : public QDialog`；
- `FramelessDialog.h` 作为中枢头文件，必须显式 `using RapidNotes::FramelessDialog;`，保证派生类如 `SettingsWindow`、`TagManagerWindow`、`HelpWindow`、`ActivationDialog` 能正确识别基类，避免出现 `C2504 未定义基类` 导致的 `QWidget` 成员缺失连锁报错。

### 18.2 ProgressDialog 统一收敛与类型闭合
- 全局使用 `BatchProgressDialog.h`（派生自 `FramelessDialog`）作为无边框进度对话框统一真理源；
- 为保持既有接口兼容，提供 `using FramelessProgressDialog = RapidNotes::BatchProgressDialog;` 类型别名，彻底清除 `使用了未定义类型 FramelessProgressDialog` 错误。

### 18.3 JSON 元数据头文件映射契约
- 废弃历史未同步的 `RapidNotesJson.h` 与 `RapidNotesJsonStore.h` 散落别名引用，直接重命名或头文件对齐包含 `QuarkMetaJson.h` 与 `QuarkMetaJsonStore.h`。

### 18.4 KeyboardHook 信号完整性
- 维护 `KeyboardHook` 的信号完整性，显式声明 `enterPressedInOtherApp()` 信号供消息捕获处理器绑定。

---

## 19. 架构演变与重构里程碑时间轴 (Architecture Evolution & Refactoring Timeline SSOT)

> 本章节作为 RapidNotes 全局演变的时间轴与工作接力台，记录核心决策、剥离模块、已完成与进行中的重构里程碑，确保多轮会话之间无缝衔接与长久持续续航。

### 19.1 里程碑记录 (Timeline Milestones)

#### 📅 里程碑 1：QuarkMeta 融合与纯数据库化转变 (QuarkMeta Merge into RapidNotes)
- **核心定位**：彻底放弃 QuarkMeta 面向物理磁盘文件系统的传统理念，全面转变为由 `inspiration.db`（`notes` + `categories` 表）驱动的统一管理中枢；
- **纯化剥离**：彻底剔除盘符栏 (`DriveBarWidget`)、物理收藏夹 (`FavoritePanel`)、物理标记分发 (`MarkedPanel`) 与文件碰撞对话框；
- **虚拟路由**：导航协议统一为 `library://<categoryId>`，地址栏仅显示数据库分类名，按键转换为 `Ctrl+N`（新建笔记）与 `Ctrl+Shift+N`（新建分类）。

#### 📅 里程碑 2：列视图与自适应视图彻底剥离与网格视图纯化 (ColumnView & JustifiedView Pruning)
- **背景与目标**：RapidNotes 版本聚焦专业集中管控，界面模式收敛，不再需要“列视图（ColumnView / 分栏）”与“自适应视图（JustifiedViewMode）”，仅保留“列表视图 (ListView)”和“网格视图 (GridView)”。
- **物理删除的列视图源码文件（共 8 个文件，并在 `CMakeLists.txt` 中同步移除）**：
  1. `src/ui/ColumnViewWidget.h` / `src/ui/ColumnViewWidget.cpp`
  2. `src/ui/ColumnViewPane.h` / `src/ui/ColumnViewPane.cpp`
  3. `src/ui/ColumnBlankCanvasWidget.h` / `src/ui/ColumnBlankCanvasWidget.cpp`
  4. `src/ui/ColumnItemDelegate.h` / `src/ui/ColumnItemDelegate.cpp`
- **自适应视图彻底剥离与网格视图纯粹化**：
  - `JustifiedView.h` / `.cpp`：作为 `DropJustifiedView` 的基类与 `GridView` 的载体，彻底剔除了自适应布局（`LayoutMode::JustifiedMode`）计算分支与切换逻辑，网格视图纯化为等比网格布局；
  - `TitleBarWidget` 与 `MainWindow`：彻底清除了状态栏上的自适应按钮（`m_btnToggleJustified` / `actAdaptive`）与分栏按钮（`m_btnToggleColumn`），剔除了 `ColumnViewMode` 与 `JustifiedViewMode` 枚举项、菜单项及切换信号；
  - `ContentPanel`：彻底移除 `ColumnView`、`JustifiedViewMode` 枚举项、`m_columnView` 成员变量、初始化与所有视图分支逻辑；
  - **全链路控制器解耦清理**：覆盖 `PanelMediator`、`ContentViewCoordinator`、`ContentKeyHandler`、`ContentChangeWatcher`、`ContentDataLoader`、`ContentFileOpsHandler`、`ContentContextMenu`，彻底清除了所有涉及列视图与自适应模式的业务分支与死代码引用。

#### 📅 里程碑 3：看门狗独立进程解耦 (Standalone Watchdog Daemon)
- **根因消除**：解决了 `src/main.cpp` 与 `src/watchdog/main.cpp` 在同一个 CMake 目标中导致的 main 符号重定义冲突；
- **构建独立**：将看门狗独立构建为轻量守护程序 `RapidNotesWatchdog.exe`，主程序通过 `QProcess::startDetached` 启动守护，退出流通过 `exit_gracefully.lock` 握手。

#### 📅 里程碑 4：数据库纯文本化与外部图片托管库架构 (Ditto-Style Path Storage & Media Storage)
- **零 BLOB 铁律**：`notes.data_blob` 彻底废除，所有文件条目仅在 `content` 存绝对物理路径；
- **托管媒体库**：纯截屏图片落盘至主程序同级根目录 `applicationDirPath() + "/media/{yyyyMM}/{hash}.png"`，去重后将路径写入 `content`；
- **剪贴板双模回写**：图片复制时同时调用 `QMimeData::setImageData(img)`（兼容聊天工具）与 `QMimeData::setUrls()`（兼容系统资源管理器）。

#### 📅 里程碑 5：MSVC 编译契约与头文件包含闭合 (Header Closure & Namespace Export)
- **命名空间导出**：在 `FramelessDialog.h` 中导出 `using RapidNotes::FramelessDialog;`，消除派生窗口 C2504 未定义基类引发的大量成员缺失报错；
- **类型收敛与包含闭合**：统一进度条别名 `FramelessProgressDialog = RapidNotes::BatchProgressDialog;`，补齐 `ToolTipOverlay.h`、`FramelessMessageBox.h`，纠正 `QuarkMetaJson.h` 包含，恢复 `KeyboardHook::enterPressedInOtherApp` 信号。

#### 📅 里程碑 6：彻底剥离物理文件批量创建与批量重命名链条 (BatchCreate & BatchRename Pruning)
- **决策根因**：RapidNotes 全面转向由 `inspiration.db` 驱动的纯笔记与分类体系，所有视图实体皆为数据库条目，彻底脱离了本地磁盘物理文件系统映射；
- **废弃判定**：
  - `BatchCreateDialog`（物理磁盘批量生成 `.md` / 目录）与当前 `Ctrl+N`（新建笔记）/ `Ctrl+Shift+N`（新建分类）的数据库体系相违背；
  - `BatchRenameDialog`（物理磁盘 `MoveFileEx` 文件重命名）与既有的卡片/列表 **行内就地重命名 (F2 Inline Rename)** 规则相违背；
- **清理与剥离范围（整条调用链彻底清除）**：
  1. **物理删除的源码文件（共 10 个，并在 `CMakeLists.txt` 中同步移除）**：
     - `src/ui/BatchCreateDialog.h` / `src/ui/BatchCreateDialog.cpp`
     - `src/ui/CreateRuleRow.h` / `src/ui/CreateRuleRow.cpp`
     - `src/ui/BatchRenameDialog.h` / `src/ui/BatchRenameDialog.cpp`
     - `src/meta/BatchRenameEngine.h` / `src/meta/BatchRenameEngine.cpp`
     - `src/core/BatchRenameService.h` / `src/core/BatchRenameService.cpp`
     - `src/core/commands/BatchRenameCommand.h`
  2. **控制器与右键菜单清理**：
     - `ContentContextMenu`：移除 `ActionBatchCreate` 与批量重命名菜单动作；
     - `ContentFileOpsHandler`：移除 `batchRename(...)` 槽函数及 `WatchSuppressor` 物理文件监听抑制器。

---

## 20. Win32 原生全局热键与交互式设置弹窗架构规范 (Win32 Native Hotkey & Settings Dialog SSOT)

> 本章节记录全软件快捷键与系统热键的唯一合法实现机制。凡是不采用此机制、擅自引入低级键盘钩子或轮询者，一律一票否决不予通过。

### 20.1 顶层架构铁律 (Hard Architectural Locks)
1. **彻底摒弃低级键盘钩子 (`WH_KEYBOARD_LL`)**：
   - 严禁使用 Windows 低级键盘钩子 `SetWindowsHookEx(WH_KEYBOARD_LL, ...)` 处理全局热键；
   - 低级键盘钩子不仅在高负载、全屏独占或游戏场景下容易引发系统级卡顿、输入丢键甚至被杀软判定为恶意键盘记录器（Keylogger），且具有极高的系统轮询损耗；
   - **唯一合法架构通道**：必须使用 Win32 原生全局热键机制：
     - `RegisterHotKey(nullptr, id, modifiers, vk)` / `UnregisterHotKey(nullptr, id)`；
     - 配合 Qt 跨平台原生事件过滤器 `QAbstractNativeEventFilter` 的 `nativeEventFilter` 监听 `WM_HOTKEY` 消息；
     - 实现零轮询、零卡顿、零 CPU 损耗的纯事件驱动中枢 `HotkeyManager`。

2. **交互式快捷键设置弹窗 (`HotkeySettingsDialog`) 架构规范**：
   - 必须提供专职、独立的无边框交互式快捷键配置弹窗 `HotkeySettingsDialog`（继承自 `FramelessDialog`）；
   - **交互设计**：内嵌专属快捷键捕获控件（`HotkeyKeyEdit` / `HotkeyEdit`），在获得焦点时精准捕获用户的按键组合（修饰键 Modifiers + 虚拟键码 VK），实时格式化显示按键文本；
   - **自动化持久化**：点击保存或应用后，自动将修饰键与键码写入持久化配置（`QSettings hotkeys("RapidNotes", "Hotkeys")`），并立即驱动 `HotkeyManager::reapplyHotkeys()` 动态热重载生效，无需重启软件；
   - **冲突与恢复**：支持一键“恢复默认设置”，在热键被外部系统抢占时输出友好告警，杜绝静默失败。

---

## 21. 彻底剔除侧车 JSON、废除物理收藏夹与“标记”面板底层纯分类化规范 (Purge Sidecar JSON & Favorites, Transform Marked Panel to Database Categories SSOT)

> 本章节记录 RapidNotes 从物理文件系统彻底纯化为统一数据库笔记管理体系的最高架构决策与剥离重构事实标准。

### 21.1 顶层架构铁律 (Hard Architectural Locks)
1. **彻底拔除侧车 JSON (`QuarkMetaJson` / `QuarkMetaJsonStore`)**：
   - 在纯数据库笔记模式下，所有笔记元数据（标题、颜色、星级、标签、备注、路径等）均统一持久化在 `inspiration.db` 的 `notes`、`tags`、`categories` 等结构化表中；
   - 物理磁盘目录中的 `.RapidNotes.json` / `.QuarkMeta.json` 隐藏侧车文件机制彻底失去存在意义；
   - **全面删除与解耦**：物理删除 `QuarkMetaJson.h`、`QuarkMetaJson.cpp`、`QuarkMetaJsonStore.h`、`QuarkMetaJsonStore.cpp`，消除所有磁盘元数据读写、脏缓冲区刷新及与 `DiskItemModel`、`DiskMediaExtractor`、`ShellHelper`、`LibrarySyncService` 的深层耦合，元数据读写唯一穿透至 `DatabaseManager`。

2. **彻底删除物理收藏夹 (`FavoritePanel` / `FavoriteDao` / `FavoriteService`)**：
   - 原 QuarkMeta 的“收藏夹”是面向物理磁盘路径（如驱动器盘符、文件绝对路径）构建的多级虚拟树；
   - 在 RapidNotes 纯笔记体系下，收藏功能已原生收敛为 `notes.is_favorite` 数据库字段以及系统内置分类“收藏”视图，物理收藏夹树已完全冗余；
   - **全面删除**：物理删除 `FavoritePanel.h/.cpp`、`FavoriteDao.h/.cpp`、`FavoriteService.h/.cpp`，从 `MainWindow`、`PanelMediator`、`SidebarContainerWidget` 中彻底移除收藏夹选项卡与右键菜单项。

3. **“标记”选项卡零篡改，底层纯分类化 (Keep "Marked" Tab Intact, Internal Category Redirection)**：
   - **UI 零篡改铁律**：侧边栏顶部的“标记”选项卡名称（`"标记"`）与既有图标（`Destination.svg`）**保持 100% 稳定不变，严禁改名或擅自修改 UI 选项卡**；
   - **底层数据源与行为转换**：“标记”面板内部的数据源由原先的物理磁盘目录映射，转换为加载与呈现数据库中的**分类（`categories` 表）**；双击或拖拽不再分发至本地磁盘物理文件夹，而是定位或归档至数据库对应的 `categoryId`。

---

## 22. 窗口职责分工与双击行为解耦规范 (Window Responsibilities & Double-Click Semantics SSOT)

> 本章节记录 RapidNotes 中主窗口（`MainWindow`）与快捷悬浮窗口（`QuickWindow`）的核心职责解耦与双击交互行为的唯一真理源 (SSOT)。坚决杜绝职责混淆与跨窗口行为篡改。

### 22.1 窗口核心定位与职责分工
1. **`QuickWindow`（快捷悬浮窗口）—— 数据发送与第三方投递终端**：
   - **核心定位**：专职面向第三方目标应用（如微信、浏览器、IDE、文本编辑器等）的即时投递工具；
   - **核心交互契约**：用户在 `QuickWindow` 中的操作（包括列表项双击、按回车发送等），其核心业务意图是将选中的笔记/数据原子化**发送/粘贴至当前激活的第三方应用程序**中，用后即隐，追求极致的高效投递节奏。
2. **`MainWindow`（主管理窗口）—— 数据资产管理与沉浸浏览中心**：
   - **核心定位**：专职负责笔记数据的全局组织、多维分类、标签关联、批量整理、全量检索与内容编辑；
   - **核心交互契约**：用户在 `MainWindow` 内容列表中的双击操作，**必须严格保持原生纯粹的“预览功能”**（即唤起 `QuickLookWindow` 就地快速预览笔记图文与元数据），绝对禁止将 `QuickWindow` 的发送粘贴逻辑套用至主窗口；
3. **语义严格隔离铁律**：
   - 两个窗口职责边界分明，交互上下文完全独立：
     - **`MainWindow` 双击条目** $\equiv$ **就地预览 (QuickLook Preview)**，保持稳定不变；
     - **`QuickWindow` 双击条目** $\equiv$ **发送数据到第三方应用 (Paste/Send to Third-Party App)**；
   - 严禁任何形式的交互倒错与逻辑混淆。

---

## 23. Quick 界面中央大脑架构接入与上帝类职责解耦规范 (QuickWindow Brain Decoupling & Batch SSOT)

> 本章节记录用户的核心设计理念指导与明确共识，作为 QuickWindow 彻底废除直连 DatabaseManager、接入中央大脑 (CoreEngine)、上帝类职责拆分与全链路收敛至批量 SSOT 的唯一真理源 (SSOT)。

### 23.1 核心设计理念与架构铁律 (Core Philosophy & Hard Locks)
1. **彻底拔除 UI 直连存储层（零直连铁律）**：
   - `QuickWindow` 严禁直接调用 `DatabaseManager::instance()` 执行业务状态变更、数据写盘或直接订阅数据库信号；
   - 全面纯化为 **Dumb View 容器**，所有业务意图统一包装为标准命令结构体 `AppCommand`，通过 `CoreEngine::instance().executeCommand(cmd)` 调度；
   - 所有数据变更监听统一走全局事件中枢 `CentralEventHub`，严格遵守单向依赖流动：
     $$\text{QuickWindow (View)} \longrightarrow \text{QuickMediator / Controller} \longrightarrow \text{CoreEngine (Domain)} \longrightarrow \text{Repository / SQLite}$$

2. **QuickWindow 职责过载拆分与模块化规范**：
   - 彻底拆解超过 4000 行的上帝类 `QuickWindow.cpp`，按职责单一原则（SRP）正交拆解为专职模块：
     - **`QuickMediator`**：中枢协调器，专职负责将 UI 操作映射为 `AppCommand` 投递给大脑，并监听 `CentralEventHub` 驱动列表刷新；
     - **`QuickContextMenuBuilder`**：专职菜单构建器，承接上千行右键菜单、分类/标签子菜单的动态构建；
     - **`QuickPasteDeliveryService`**：专职第三方投递服务，负责前台目标窗口句柄探测、系统级双模回写与模拟粘贴投递；
     - **`QuickShortcutHandler`**：专职快捷键拦截器，统一对齐 `LocalShortcutInterceptor` 架构，强制执行打字态输入焦点保护（`isEditingFocus()`）。

3. **违背 SSOT 行为全面归一化与强制批量优先契约 (Batch SSOT Contract)**：
   - **状态变更必须原生支持批量接口**：
     - 严禁在 UI 层使用 `for` 循环单条串行调用 `updateNoteState` 伪装批量操作；
     - 必须统一接入 `CoreEngine` 批量操作通道，底层使用 `WHERE id IN (...)` 集合式 SQL 完成单事务批处理与单次聚合广播；
   - **标签编解码统一真理源**：所有标签操作严格收敛至 `RapidNotes::TagCodec`（拆分 `split`、合并 `merge`、规范化 `normalize`、紧凑拼接 `join` 与展示拼接 `joinDisplay`），严禁手写字符串正则；
   - **路径条目解析统一真理源**：所有剪贴板物理路径、多路径拆分统一委托给 `RapidNotes::PathSummary`，主线程零磁盘探测；
   - **软删除与永久删除统一真理源**：删除行为统一接入 `TrashService` / `PermanentDeleteService`，杜绝分散的零星删除逻辑。

4. **品牌与 Logo 资产主次规范 (Logo & Brand Assets SSOT)**：
   - 整体工程以 **RapidNotes 为主，QuarkMeta 为次**；
   - Windows 资源清单、可执行程序图标、系统托盘图标、窗口标题栏图标统一以 RapidNotes 官方资产为主视觉真理源；
   - 清理重定义冲突的 Manifest 与 RC 资源文件，保持命名与品牌规范严谨闭合。

---

## 25. 品牌与 Logo 资产归一化与历史包袱彻底清理规范 (Brand & Logo Assets Consolidation SSOT)

> 本章节记录用户的核心设计决策：全工程品牌资产与图标引用严格遵循“以 RapidNotes 为主，QuarkMeta 为次”，彻底消除命名割裂与孤儿资源，保持应用主品牌的统一性与纯粹性。

### 25.1 核心设计决策与规范 (Core Philosophy & Brand SSOT)
1. **主次定位与品牌纯粹性铁律**：
   - **主品牌 (Primary Brand)**：**RapidNotes** —— 作为对外呈现、可执行程序名称、系统托盘、任务栏及窗口标题栏的唯一法定主品牌；
   - **次要底座 (Secondary Engine)**：**QuarkMeta** —— 仅作为多窗格、文件管理与多维视图内核的技术来源；在用户界面的所有常规视觉呈现中，不作为主品牌露出，仅允许在“关于”或“技术内核信息”中次要标注；
   - 彻底清除主界面中容易引起品牌认知混淆的遗留标识。

2. **Windows 资源文件与清单彻底归一化**：
   - **RC 资源文件归一化**：废弃并物理重命名/收敛根目录下残留的 `QuarkMeta.rc`，统一归入 `resources/app_icon.rc`（或标准 `RapidNotes.rc`），确保 PE 资源表中的 `IDI_ICON1` 严格指向 `resources/app_icon.ico`（RapidNotes 官方图标）；
   - **Manifest 应用程序清单归一化**：彻底修正清单描述，将 `<description>QuarkMeta File Manager</description>` 修正为 `<description>RapidNotes Desktop Application</description>`，装配件标识 `<assemblyIdentity name="QuarkMeta" .../>` 修正为 `name="RapidNotes"`；
   - 避免在 Windows 任务管理器、UAC 提权提示及系统属性中出现错误的 "QuarkMeta File Manager" 旧程序描述。

3. **编译构建与代码级残留清除 (Dead Code Pruning)**：
   - **物理注销与删除未使用的 `QuarkApplication`**：`src/ui/QuarkApplication.cpp` / `.h` 为原 QuarkMeta 单实例与自愈包装类，在 RapidNotes 纯笔记体系中从未被 `main.cpp` 调用，属于典型的孤儿死代码；必须在 `CMakeLists.txt` 构建列表中物理注销并从项目中彻底剔除，杜绝符号冗余与多入口混淆；
   - **资源清单收敛**：`resources/resources.qrc` 仅保留全工程实际引用的 SVG 与图片资产，彻底断绝任何误引用废弃资源的路径。

---

## 24. MSVC 动态链接与三权分立构建规范 (MSVC Dynamic Linking & Build SSOT)

> 本章节记录用户的核心架构决策：RapidNotes 正式锁定官方标准动态链接版 Qt (`msvc2022_64`)，构建体系全面归一化为动态运行时库 (`/MD`)，并依托“三权分立”与 `windeployqt` 进行固化运行层部署。

### 24.1 核心设计决策与硬性规范 (Core Philosophy & Build Locks)
1. **彻底终结 `/MT` 与 `/MD` 混用冲突 (LNK2038 / LNK2005 根因消除)**：
   - 正式废除 RapidNotes 构建系统中强行设置的静态运行时库（`MultiThreaded` / `/MT`）；
   - 全面切换为动态运行时库 **`/MD` (Release) 与 `/MDd` (Debug)**，即 `CMAKE_MSVC_RUNTIME_LIBRARY` 统一设为 `MultiThreadedDLL$<$<CONFIG:Debug>:Debug>`；
   - 与官方 Qt SDK `C:/Qt/6.10.2/msvc2022_64`（其内部为 `/MD` 构建）实现 100% 运行时库契约对齐，彻底消除 `libcpmt.lib` 与 `msvcprt.lib` 的多重符号定义冲突。

2. **静态专属插件导入彻底清除 (Clean Dynamic Linking)**：
   - 物理移除 `CMakeLists.txt` 中的静态插件导入宏 `qt_import_plugins(...)`，动态版由 Qt 官方机制自动加载动态插件（如 `platforms/qwindows.dll`、`imageformats/qjpeg.dll` 等）；
   - 彻底解决静态插件对象初始化文件（如 `QGifPlugin_init.cpp.obj` 等）引发的 LNK2038 报错。

3. **三权分立与运行层部署标准 (Three-Tier Architecture & windeployqt)**：
   - 严格遵循根目录 `T 统一编译路径.md` 约定的工程闭环：
     - **源码层**：`RapidNotes_Source/RapidNotes` 纯净代码圣殿，零二进制污染；
     - **中间构建层**：`RapidNotes_Build/x64-release/` 作为临时编译消耗品目录；
     - **固化运行层**：`RapidNotes_Build/RapidNotes/` 免安装绿色沙盒，由 CMake 自动将 `RapidNotes.exe` 输出至该目录，并使用 `windeployqt.exe RapidNotes.exe` 部署一次依赖，日常编译仅覆盖单一 exe，实现即编即测。

## 25. 无边框拖拽缩放归一化 (Frameless Resize Normalization)

> 本章节记录用户的核心架构决策：手动拖拽调整窗口大小的逻辑必须归一化到 `FramelessWindowHelper` 单一真理源，任何窗口禁止另起炉灶手写 WM_* 拷贝。

### 25.1 核心设计决策 (Core Philosophy)
1. **WS_THICKFRAME 是拖拽缩放的物理开关**：Win32 窗口样式（GWL_STYLE）缺少 `WS_THICKFRAME` 时，系统会直接无视 `WM_NCHITTEST` 返回的 HTLEFT/HTRIGHT 等缩放指令——"样式相关"的根因即指此原生窗口样式，而非 QSS。
2. **三层契约架构**：①样式地基（WS_THICKFRAME 注入，基础设施层）→ ②命中测试（WM_NCHITTEST 边缘/交互控件/标题栏判定，基础设施层）→ ③拖拽执行（系统模态尺寸循环，OS 零代码）。应用代码严禁在 mouseMoveEvent 里手工计算缩放。
3. **生命周期钩子取代 static 一次性开关**：`FramelessWindowHelper` 以 `QEvent::WinIdChange` 事件过滤器在句柄重建（如 setWindowFlags）后自动补注 WS_THICKFRAME；`QuickWindow::showEvent` 的样式注入改为每次幂等复检，物理消灭 `static bool` 单次开关。
4. **归一化落地**：QuickWindow 删除自有 WM_NCHITTEST/WM_NCCALCSIZE 拷贝，`nativeEvent` 仅一行转发 `m_framelessHelper->handleNativeEvent()`；`FramelessWindowHelper::apply(this, nullptr)` 为无标题栏窗口接入范式（TagSelectorOverlay 先例）。

### 25.5 全窗口缩放物理统一归一 (Unified Resize Physics)
- 所有顶层窗口/弹窗的 8 向拖拽缩放物理逻辑（WM_NCHITTEST/WM_NCCALCSIZE/WM_GETMINMAXINFO/WM_SETCURSOR）**只能**存在于 `FramelessWindowHelper` 一处；
- `FramelessDialog` 基类统一 `apply()` + `nativeEvent` 转发（覆盖 40+ 弹窗），子类通过 `setTitleBar()` 按需启用原生拖拽/双击最大化；
- `ResizeHandle` 私有拖角小部件已物理删除（含 CMakeLists 条目），其 600x400 缩放下限改由 `setMinimumSize` + Helper WM_GETMINMAXINFO 承载；
- NoteEditWindow(15px)/QuickPreview(12px)/QuickLookWindow(8px) 按各自可见边框内缩量传入热区宽度。
