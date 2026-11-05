# RapidNotes SSOT 接口索引文档

> 本文档为 RapidNotes 全软件对外公开接口（类、公开函数、信号）的唯一真理源登记索引。
> 严格遵循《RapidNotes 数据库与底层逻辑重构提示词》第二部分 2.1 SSOT 表规范进行维护。
> 每个接口在头文件中的注释与本文档严格保持 100% 一致。

---

## 1. 条目类型词表 (ItemType)

### `RapidNotes::ItemType` (`src/core/ItemType.h`)
- **它是哪一件事实的唯一来源**：条目类型词表（全工程唯一允许出现 `item_type` 字符串字面量的地方）。
- **用途与适用场景**：提供类型枚举 `Type` 与数据库/界面类型字符串的双向强类型转换，以及类型分类纯谓词断言。
- **禁止在别处重复实现的内容**：严禁在任何其他文件中硬编码 `"text"`、`"image"`、`"code"`、`"link"`、`"color"`、`"file"`、`"folder"` 等类型字符串字面量。
- **主要调用方**：`ItemClassifier`、`NoteModel`、`NoteEditWindow`、`main.cpp`、`QuickWindow`、`DatabaseManager`。

#### 公开接口列表：
1. `QString RapidNotes::ItemType::toString(Type type)`
   - 用途：将条目类型强类型枚举转换为数据库和网络通信的标准小写字符串标识。
2. `RapidNotes::ItemType::Type RapidNotes::ItemType::fromString(const QString& str)`
   - 用途：将外部传入的字符串安全解析为强类型枚举，未识别字符串回退为 `Type::Unknown`。
3. `bool RapidNotes::ItemType::isFileOrFolder(Type type)`
   - 用途：断言给定类型是否属于文件或文件夹类（含中间态与最终态）。
4. `bool RapidNotes::ItemType::isLegacyFiveFileTypes(Type type)`
   - 用途：严格匹配历史 5 种物理路径/文件类型（File, LocalFile, LocalBatch, Folder, LocalFolder），不含 files/folders，专供 DatabaseManager::extractFileExtensions 与 StringUtils::copyNotesToClipboard。
5. `bool RapidNotes::ItemType::isPureText(Type type)`
   - 用途：断言给定类型是否属于纯文本正文类（Text、Code、OcrText、CapturedMessage）。

---

## 2. 文本类型判定规则 (ItemClassifier)

### `RapidNotes::ItemClassifier` (`src/core/ItemClassifier.h`)
- **它是哪一件事实的唯一来源**：文本的特征与类型判定规则（网址、色码、代码特征、路径）。
- **用途与适用场景**：提供入库时的自动类型识别，以及界面显示时的纯内存图标与颜色匹配。
- **禁止在别处重复实现的内容**：严禁在 `main.cpp`、`NoteModel`、工具类或各个编辑窗口中各自维护正则或手写前缀判定。
- **主要调用方**：`main.cpp`（剪贴板入库）、`NoteEditWindow`（内容变更后自动类型识别）、`NoteModel`（列表图标与颜色计算）。

#### 公开接口列表：
1. `bool RapidNotes::ItemClassifier::isUrl(const QString& text)`
   - 用途：基于严格前缀规则断言文本是否为有效网址（http://, https://, www.）。
2. `bool RapidNotes::ItemClassifier::isHexColor(const QString& text)`
   - 用途：断言文本是否为标准 HEX 颜色码（#RRGGBB 或 #RGB）。
3. `bool RapidNotes::ItemClassifier::isRgbTriplet(const QString& text, int* r, int* g, int* b)`
   - 用途：断言并解析 rgb(r,g,b) 颜色文本格式。
4. `bool RapidNotes::ItemClassifier::isCodeLike(const QString& text)`
   - 用途：基于语法关键词与 HTML 结构特征断言文本是否属于代码或脚本。
5. `bool RapidNotes::ItemClassifier::isAbsolutePathText(const QString& text)`
   - 用途：断言去除引号后的文本是否具有 Windows 绝对路径格式（盘符或 UNC 路径），严禁访问物理磁盘。
6. `RapidNotes::ItemType::Type RapidNotes::ItemClassifier::detectIngestType(const QString& text)`
   - 用途：入库时的核心类型推导入口，输出标准 ItemType 枚举。
7. `RapidNotes::DisplayCategory RapidNotes::ItemClassifier::detectDisplayCategory(const QString& itemTypeStr, const QString& content)`
   - 用途：视图层图标与颜色唯一判定入口，纯内存计算，严格禁止磁盘 I/O。

---

## 3. 路径种类与图标归并 (PathSummary)

### `RapidNotes::PathSummary` (`src/core/PathSummary.h`)
- **它是哪一件事实的唯一来源**：文件/文件夹条目的路径种类归纳与对应视图图标判定。
- **用途与适用场景**：由路径列表或分号连接的分号串计算得出单文件、单文件夹、多文件、多文件夹的归一化种类与视觉样式。
- **禁止在别处重复实现的内容**：禁止在各窗口或 Delegate 里自行遍历路径并判断文件夹与文件组合。
- **主要调用方**：`NoteModel::data`、`QuickNoteDelegate`、`RecentNotesDelegate`。

#### 公开接口列表：
1. `RapidNotes::PathSummary::Kind RapidNotes::PathSummary::summarize(const QStringList& paths)`
   - 用途：由路径列表推导出种类（SingleFile, SingleFolder, MultipleFilesOrMixed, MultipleFolders）。
2. `RapidNotes::PathSummary::Kind RapidNotes::PathSummary::summarizeFromSemicolonString(const QString& content)`
   - 用途：便捷入口，由分号连接的路径字符串推导出种类。
3. `QString RapidNotes::PathSummary::getIconName(Kind kind)`
   - 用途：根据路径种类返回统一矢量图标名称（"file"、"folder"、"files_multiple"、"folders_multiple"）。
4. `QString RapidNotes::PathSummary::getIconColor(Kind kind)`
   - 用途：根据路径种类返回统一图标颜色 Hex（"#FFFF00"、"#FF8C00"、"#FF0000"、"#483D8B"）。

---

## 4. 字数阶梯计算 (WordBucket)

### `RapidNotes::WordBucket` (`src/core/WordBucket.h`)
- **它是哪一件事实的唯一来源**：字数阶梯与字数统计清洗逻辑。
- **用途与适用场景**：正文 HTML 标签剥离、实体字符清洗，并按 0、10..100、200..1000、1001 阶梯输出字数分桶。
- **禁止在别处重复实现的内容**：禁止在入库、修改或统计时单独计算正文字数或写清洗逻辑。
- **主要调用方**：`DatabaseManager::addNote`、`DatabaseManager::updateNote`。

#### 公开接口列表：
1. `int RapidNotes::WordBucket::calculate(const QString& content, const QString& itemType)`
   - 用途：输入内容与类型，返回清洗后的统计字数。非正文类型直接返回 0。

---

## 5. 标签编解码与格式归一化 (TagCodec)

### `RapidNotes::TagCodec` (`src/core/TagCodec.h`)
- **它是哪一件事实的唯一来源**：标签的拆分与拼接。
- **用途与适用场景**：全工程所有标签字符串与标签列表的双向转换，拆分兼容中英逗号，拼接严格半角。
- **禁止在别处重复实现的内容**：严禁在任何业务层、数据层或 UI 层直接调用 `.split(",")`、`.split(QRegularExpression(...))` 或 `.join(",")`。
- **主要调用方**：`DatabaseManager`、`QuickWindow`、`NoteEditWindow`、`TagEditDialog`、`AdvancedTagSelector`、`FramelessDialog`、`FileStorageHelper`。

#### 公开接口列表：
1. `QStringList RapidNotes::TagCodec::split(const QString& tagsStr)`
   - 用途：拆分标签字符串，同时支持半角逗号 `,` 与全角逗号 `，`，自动清洗首尾空白、空项并保序去重。
2. `QStringList RapidNotes::TagCodec::normalize(const QStringList& tags)`
   - 用途：对已有标签列表执行逐项 trim、剔除空项并保序去重。
3. `void RapidNotes::TagCodec::merge(QStringList& target, const QStringList& incoming, bool* changed = nullptr)`
   - 用途：将 incoming 标签列表合并到 target 中，自动 trim、剔除空项并保序去重，可选输出是否产生变更。
4. `QString RapidNotes::TagCodec::join(const QStringList& tags)`
   - 用途：持久化与数据存储用拼接，严格使用纯半角逗号 `,` 无空格连接。
5. `QString RapidNotes::TagCodec::joinDisplay(const QStringList& tags)`
   - 用途：界面展示与文本框回填专用，使用半角逗号加空格 `, ` 连接。

---

## 6. 图片缩略图生成算法 (ThumbnailMaker)

### `RapidNotes::ThumbnailMaker` (`src/core/ThumbnailMaker.h`)
- **它是哪一件事实的唯一来源**：图片 64x64 平滑缩略图与图标生成。
- **用途与适用场景**：入库预处理与视图缓存渲染时的高性能平滑缩略图生成。
- **禁止在别处重复实现的内容**：禁止在 UI 模型中内嵌手写缩放参数或重新解码整图。
- **主要调用方**：`NoteModel`、后台入库计算流水线。

#### 公开接口列表：
1. `QImage RapidNotes::ThumbnailMaker::makeThumbnail(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64))`
   - 用途：按保持纵横比与平滑缩放算法（`Qt::SmoothTransformation`）生成缩略图 QImage。
2. `QIcon RapidNotes::ThumbnailMaker::makeThumbnailIcon(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64))`
   - 用途：将缩略图转换为可直接用于 Model DecorationRole 的 QIcon。
