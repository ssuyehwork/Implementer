# RapidNotes 架构唯一真理源 (Single Source of Truth, SSOT) 规范文档

## 1. 概述与核心原则
本文档为 `RapidNotes` 项目架构重构与维护的唯一权威规范基准（SSOT）。
- **零补丁原则**：严禁采用局部修补、中间垫片（forwarding shim）或壳接口。修改或重构必须贯通所有调用方。
- **无冗余原则**：禁止出现多处重复判定、重复计算或分散的硬编码枚举字面量。
- **专职模块原则**：所有通用纯算法、词表和数据转换逻辑必须收拢于 `src/core/` 专职类中，严禁在 UI 层或数据库层各自发明重复逻辑。

---

## 2. 条目类型词表 (ItemType SSOT)
**命名空间与类**：`RapidNotes::ItemType`
**头文件**：`src/core/ItemType.h`
**实现文件**：`src/core/ItemType.cpp`

### 2.1 枚举定义
全工程所有涉及笔记/剪贴板条目类型的识别、流转与存储，统一使用枚举 `RapidNotes::ItemType::Type`：
- `Text` (`"text"`)
- `Image` (`"image"`)
- `Code` (`"code"`)
- `Link` (`"link"`)
- `Color` (`"color"`)
- `Palette` (`"palette"`)
- `OcrText` (`"ocr_text"`)
- `CapturedMessage` (`"captured_message"`)
- `PixelRuler` (`"pixel_ruler"`)
- `DeletedCategory` (`"deleted_category"`)
- `File` (`"file"`)
- `Files` (`"files"`)
- `Folder` (`"folder"`)
- `Folders` (`"folders"`)
- `LocalFile` (`"local_file"`)
- `LocalFolder` (`"local_folder"`)
- `LocalBatch` (`"local_batch"`)
- `Unknown`

### 2.2 转换与断言接口
- `static QString toString(Type type)`：枚举转小写字符串（写入数据库或序列化）。
- `static Type fromString(const QString& str)`：字符串解析为枚举（不区分大小写，自动 trim）。
- `static bool isFileOrFolder(Type type)`：判定是否属于 7 种文件/目录/路径相关类型（`File`, `Files`, `Folder`, `Folders`, `LocalFile`, `LocalFolder`, `LocalBatch`）。
- `static bool isLegacyFiveFileTypes(Type type)`：严格匹配历史 5 种物理路径/文件类型（`File`, `LocalFile`, `LocalBatch`, `Folder`, `LocalFolder`），专门供给 `DatabaseManager::extractFileExtensions` 与 `StringUtils::copyNotesToClipboard`。
- `static bool isPureText(Type type)`：判定是否属于纯文本正文类（`Text`, `Code`, `OcrText`, `CapturedMessage`）。

---

## 3. 标签编解码与格式规范 (TagCodec SSOT)
**命名空间与类**：`RapidNotes::TagCodec`
**头文件**：`src/core/TagCodec.h`
**实现文件**：`src/core/TagCodec.cpp`

### 3.1 格式契约
- **分隔符解析**：同时支持半角逗号 `,` 与全角逗号 `，`。
- **存储与纯净性**：存储时严格使用半角逗号 `,` 连接；展示时使用半角逗号加空格 `, ` 连接。
- **自动清洗**：拆分、合并与规范化时，自动对各项执行 `trimmed()` 清理空白，过滤空字符串，并保序去重。

### 3.2 接口矩阵
- `static QStringList split(const QString& tagsStr)`：拆分字符串为清洗后的无重复标签列表。
- `static QStringList normalize(const QStringList& tags)`：对外部传入的标签列表进行清洗、剔除空项和保序去重。
- `static void merge(QStringList& target, const QStringList& incoming, bool* changed = nullptr)`：将 `incoming` 合并入 `target`，执行清洗与保序去重，可返回是否发生了实际内容变动。
- `static QString join(const QStringList& tags)`：紧凑半角逗号拼接（底层/内部交换）。
- `static QString joinDisplay(const QStringList& tags)`：带空格半角逗号拼接（数据库冗余列展示/UI 显示）。

---

## 4. 字数阶梯计算 (WordBucket SSOT)
**命名空间与类**：`RapidNotes::WordBucket`
**头文件**：`src/core/WordBucket.h`
**实现文件**：`src/core/WordBucket.cpp`

### 4.1 算法规则
输入条目正文与类型，执行 HTML/脚本剥离与空白压缩，并按阶梯映射为整数分桶：
- 非正文类型（非纯文本族）：返回 `0`
- `len == 0`：返回 `0`
- `len <= 100`：以 10 为阶梯分桶（`10, 20, ..., 100`）
- `len <= 1000`：以 100 为阶梯分桶（`200, 300, ..., 1000`）
- `len > 1000`：返回 `1001`（表示 1000 字以上）

### 4.2 接口矩阵
- `static int calculate(const QString& content, const QString& itemType)`

---

## 5. 类型探测与吸纳 (ItemClassifier SSOT)
**命名空间与类**：`RapidNotes::ItemClassifier`
**头文件**：`src/core/ItemClassifier.h`
**实现文件**：`src/core/ItemClassifier.cpp`
负责剪贴板或导入流的类型侦测，彻底替代旧 `StringUtils::detectItemType`。

---

## 6. 路径概括与缩略图 (PathSummary & ThumbnailMaker SSOT)
**路径概括**：`RapidNotes::PathSummary` (`src/core/PathSummary.h`)
负责分号分隔路径的中文语义折叠（如“1个文件”、“多个文件”等）。
**缩略图生成**：`RapidNotes::ThumbnailMaker` (`src/core/ThumbnailMaker.h`)
负责安全内存缩略图生成，严禁主线程进行同步重型磁盘 I/O。
