# RapidNotes 核心 SSOT 接口与唯一真理源登记表 (RapidNotes_SSOT.md)

> 本文档为 RapidNotes 软件当前版本所有对外公开接口、类、函数及核心真理源通道的单一事实来源（SSOT, Single Source of Truth）统一注册登记表。
> 严格遵循《Standard Specifications for Implementation Plan.md》与《RapidNotes_Prompts-2.md》中 SSOT 架构治理规范建立。

---

## 一、 SSOT 架构治理原则

在 RapidNotes 系统中，**SSOT（单一事实来源）** 代表：
1. **单一事实对应唯一来源**：对于软件中的任何核心规则、类型体系、数据定义与通用行为，必须且只能存在一个公认的、权威的真理模块；
2. **禁止散落与重复造轮子**：严禁在业务层、控制器层或 UI 层各自手写解析逻辑、私自定义类型字面量或重复实现判定规则；
3. **接口与文档 100% 对齐**：头文件中的 Doxygen 规范注释与本 SSOT 登记表严格保持一一对应与字面一致。

---

## 二、 核心词表与纯函数真理源 (Domain Vocabularies & Pure Functions)

### 1. 条目类型词表 (`RapidNotes::ItemType`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/ItemType.h` 与 `ItemType.cpp`
- **所属命名空间**：`RapidNotes`
- **它是哪一件事实的唯一来源**：全工程条目类型词表（全工程唯一允许出现 `item_type` 字符串字面量的地方）。
- **用途与适用场景**：提供类型强枚举 `ItemType::Type` 与数据库/协议字面量字符串之间的双向强类型映射，以及类型谓词判定。
- **禁止在别处重复实现的内容**：严禁在任何其他文件中硬编码 `"text"`、`"image"`、`"code"`、`"link"`、`"color"`、`"file"`、`"folder"` 等类型字符串字面量或做字符串比对。
- **主要调用方**：`ItemClassifier`、`NoteModel`、`NoteEditWindow`、`main.cpp`、`QuickWindow`、`DatabaseManager`。
- **公开接口列表**：
  1. `QString RapidNotes::ItemType::toString(Type type)`
     - 签名：`static QString toString(Type type);`
     - 用途：将条目类型强类型枚举转换为数据库和网络通信的标准小写字符串标识。
  2. `RapidNotes::ItemType::Type RapidNotes::ItemType::fromString(const QString& str)`
     - 签名：`static Type fromString(const QString& str);`
     - 用途：将外部传入的字符串安全解析为强类型枚举，未识别字符串回退为 `Type::Unknown`。
  3. `bool RapidNotes::ItemType::isFileOrFolder(Type type)`
     - 签名：`static bool isFileOrFolder(Type type);`
     - 用途：断言给定类型是否属于文件或文件夹类（含中间态与最终态）。
  4. `bool RapidNotes::ItemType::isPureText(Type type)`
     - 签名：`static bool isPureText(Type type);`
     - 用途：断言给定类型是否属于纯文本正文类（Text、Code、OcrText、CapturedMessage）。

---

### 2. 文本特征与类型判定规则 (`RapidNotes::ItemClassifier`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/ItemClassifier.h` 与 `ItemClassifier.cpp`
- **所属命名空间**：`RapidNotes`
- **它是哪一件事实的唯一来源**：文本的特征与类型判定规则（网址、HEX 色码、RGB 三元组、代码特征、路径模式）。
- **用途与适用场景**：提供入库时根据文本特征的自动类型推导，以及视图层根据类型与内容计算图标与样式的纯内存显示分类计算。
- **禁止在别处重复实现的内容**：严禁在 `main.cpp`、`NoteModel`、工具类或各个编辑窗口中各自维护正则或手写前缀/后缀判定，视图层判定严格禁止做磁盘 I/O。
- **主要调用方**：`main.cpp`（剪贴板入库）、`NoteEditWindow`（内容变更后自动类型识别）、`NoteModel`（列表图标与颜色计算）。
- **公开接口列表**：
  1. `bool RapidNotes::ItemClassifier::isUrl(const QString& text)`
     - 签名：`static bool isUrl(const QString& text);`
     - 用途：基于严格前缀规则断言文本是否为有效网址（http://, https://, www.）。
  2. `bool RapidNotes::ItemClassifier::isHexColor(const QString& text)`
     - 签名：`static bool isHexColor(const QString& text);`
     - 用途：断言文本是否为标准 HEX 颜色码（#RRGGBB 或 #RGB）。
  3. `bool RapidNotes::ItemClassifier::isRgbTriplet(const QString& text, int* r = nullptr, int* g = nullptr, int* b = nullptr)`
     - 签名：`static bool isRgbTriplet(const QString& text, int* r = nullptr, int* g = nullptr, int* b = nullptr);`
     - 用途：断言并解析 rgb(r,g,b) 颜色文本格式。
  4. `bool RapidNotes::ItemClassifier::isCodeLike(const QString& text)`
     - 签名：`static bool isCodeLike(const QString& text);`
     - 用途：基于语法关键词与 HTML 结构特征断言文本是否属于代码或脚本。
  5. `bool RapidNotes::ItemClassifier::isAbsolutePathText(const QString& text)`
     - 签名：`static bool isAbsolutePathText(const QString& text);`
     - 用途：断言去除引号后的文本是否具有 Windows 绝对路径格式（盘符或 UNC 路径），严禁访问物理磁盘。
  6. `RapidNotes::ItemType::Type RapidNotes::ItemClassifier::detectIngestType(const QString& text)`
     - 签名：`static ItemType::Type detectIngestType(const QString& text);`
     - 用途：入库时的核心类型推导入口，输出标准 ItemType 枚举。
  7. `RapidNotes::DisplayCategory RapidNotes::ItemClassifier::detectDisplayCategory(const QString& itemTypeStr, const QString& content)`
     - 签名：`static DisplayCategory detectDisplayCategory(const QString& itemTypeStr, const QString& content);`
     - 用途：视图层图标与颜色唯一判定入口，纯内存计算，严格禁止磁盘 I/O。

---

### 3. 路径种类与图标归并 (`RapidNotes::PathSummary`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/PathSummary.h` 与 `PathSummary.cpp`
- **所属命名空间**：`RapidNotes`
- **它是哪一件事实的唯一来源**：文件/文件夹条目的路径种类归纳与对应视图图标判定。
- **用途与适用场景**：由路径列表或分号连接的分号串计算得出单文件、单文件夹、多文件、多文件夹的归一化种类与视觉样式。
- **禁止在别处重复实现的内容**：禁止在各窗口或 Delegate 里自行遍历路径并判断文件夹与文件组合，严禁私自修改图标名称与对应 Hex 颜色。
- **主要调用方**：`NoteModel::data`、`QuickNoteDelegate`、`RecentNotesDelegate`。
- **公开接口列表**：
  1. `RapidNotes::PathSummary::Kind RapidNotes::PathSummary::summarize(const QStringList& paths)`
     - 签名：`static Kind summarize(const QStringList& paths);`
     - 用途：由路径列表推导出种类（SingleFile, SingleFolder, MultipleFilesOrMixed, MultipleFolders）。
  2. `RapidNotes::PathSummary::Kind RapidNotes::PathSummary::summarizeFromSemicolonString(const QString& content)`
     - 签名：`static Kind summarizeFromSemicolonString(const QString& content);`
     - 用途：便捷入口，由分号连接的路径字符串推导出种类。
  3. `QString RapidNotes::PathSummary::getIconName(Kind kind)`
     - 签名：`static QString getIconName(Kind kind);`
     - 用途：根据路径种类返回统一矢量图标名称（"file"、"folder"、"files_multiple"、"folders_multiple"）。
  4. `QString RapidNotes::PathSummary::getIconColor(Kind kind)`
     - 签名：`static QString getIconColor(Kind kind);`
     - 用途：根据路径种类返回统一图标颜色 Hex（`#FFFF00`、`#FF8C00`、`#FF0000`、`#483D8B`）。

---

### 4. 字数阶梯计算 (`RapidNotes::WordBucket`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/WordBucket.h` 与 `WordBucket.cpp`
- **所属命名空间**：`RapidNotes`
- **它是哪一件事实的唯一来源**：正文字数阶梯与字数统计清洗逻辑。
- **用途与适用场景**：正文 HTML 标签剥离、实体字符清洗，并按 0、10、50、100、200、500、1000、1001 阶梯输出字数分桶。
- **禁止在别处重复实现的内容**：禁止在入库、修改或统计时单独计算正文字数或写清洗逻辑。
- **主要调用方**：`DatabaseManager::addNote`、`DatabaseManager::updateNote`。
- **公开接口列表**：
  1. `int RapidNotes::WordBucket::calculate(const QString& content, const QString& itemType)`
     - 签名：`static int calculate(const QString& content, const QString& itemType);`
     - 用途：输入内容与类型，返回清洗后的统计字数。非正文类型直接返回 0。

---

### 5. 标签编解码与格式归一化 (`RapidNotes::TagCodec`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/TagCodec.h` 与 `TagCodec.cpp`
- **所属命名空间**：`RapidNotes`
- **它是哪一件事实的唯一来源**：标签集合与字符串的双向流转（拆分与拼接）。
- **用途与适用场景**：全工程所有标签字符串与标签列表的双向转换，拆分兼容中英逗号，拼接严格半角。
- **禁止在别处重复实现的内容**：严禁在任何业务层、数据层或 UI 层直接调用 `.split(",")`、`.split(QRegularExpression(...))` 或 `.join(",")`。
- **主要调用方**：`DatabaseManager`、`QuickWindow`、`NoteEditWindow`、`TagEditDialog`、`AdvancedTagSelector`、`FramelessDialog`、`FileStorageHelper`。
- **公开接口列表**：
  1. `QStringList RapidNotes::TagCodec::split(const QString& tagsStr)`
     - 签名：`static QStringList split(const QString& tagsStr);`
     - 用途：拆分标签字符串，同时支持半角逗号 `,` 与全角逗号 `，`，自动清洗首尾空白、去重与过滤空项。
  2. `QString RapidNotes::TagCodec::join(const QStringList& tags)`
     - 签名：`static QString join(const QStringList& tags);`
     - 用途：持久化与数据存储用拼接，严格使用纯半角逗号 `,` 无空格连接。
  3. `QString RapidNotes::TagCodec::joinDisplay(const QStringList& tags)`
     - 签名：`static QString joinDisplay(const QStringList& tags);`
     - 用途：界面展示与文本框回填专用，使用半角逗号加空格 `, ` 连接。

---

### 6. 图片缩略图生成算法 (`RapidNotes::ThumbnailMaker`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/ThumbnailMaker.h` 与 `ThumbnailMaker.cpp`
- **所属命名空间**：`RapidNotes`
- **它是哪一件事实的唯一来源**：图片 64x64 平滑缩略图与图标生成。
- **用途与适用场景**：入库预处理与视图缓存渲染时的高性能平滑缩略图生成。
- **禁止在别处重复实现的内容**：禁止在 UI 模型中内嵌手写缩放参数或重新解码整图。
- **主要调用方**：`NoteModel`、后台入库计算流水线。
- **公开接口列表**：
  1. `QImage RapidNotes::ThumbnailMaker::makeThumbnail(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64))`
     - 签名：`static QImage makeThumbnail(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64));`
     - 用途：按保持纵横比与平滑缩放算法（`Qt::SmoothTransformation`）生成缩略图 QImage。
  2. `QIcon RapidNotes::ThumbnailMaker::makeThumbnailIcon(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64))`
     - 签名：`static QIcon makeThumbnailIcon(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64));`
     - 用途：将缩略图转换为可直接用于 Model DecorationRole 的 QIcon。

---

## 三、 视觉渲染与资源管理真理源 (Visual Rendering & Resources)

### 7. 矢量图标渲染与缓存中心 (`IconHelper`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/ui/IconHelper.h`
- **它是哪一件事实的唯一来源**：基于 SVG 矢量代码生成指定颜色与尺寸的 `QIcon`，并由全局缓存池统一管理。
- **用途与适用场景**：全应用统一图标生成与复用，自动拦截重复渲染，保证 UI 渲染性能。
- **禁止在别处重复实现的内容**：禁止直接解析 SVG 字符串或在控件内私自实现 `QSvgRenderer` 绘制。
- **主要调用方**：`NoteModel`、`CategoryModel`、`QuickWindow`、`QuickNoteDelegate`、`RecentNotesDelegate`、各对话框。
- **公开接口列表**：
  1. `QIcon IconHelper::getIcon(const QString& name, const QString& color = "#cccccc", int size = 64)`
     - 签名：`static QIcon getIcon(const QString& name, const QString& color = "#cccccc", int size = 64);`
     - 用途：获取着色后的矢量图标，内置线程安全缓存。

### 8. 矢量图标字典真理源 (`SvgIcons`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/ui/SvgIcons.h`
- **它是哪一件事实的唯一来源**：全应用矢量图标 SVG 源代码定义。
- **用途与适用场景**：为 `IconHelper` 提供唯一的原始矢量数据。
- **禁止在别处重复实现的内容**：禁止在别处内联定义任何业务图标的 SVG 字符串。
- **公开数据定义**：
  - `inline const QMap<QString, QString> SvgIcons::icons;`

---

## 四、 系统服务与设备交互真理源 (System Services & Hardware)

### 9. 硬件指纹采集与设备授权真理源 (`HardwareInfoHelper`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/HardwareInfoHelper.h` 与 `HardwareInfoHelper.cpp`
- **它是哪一件事实的唯一来源**：Windows 物理硬件信息（CPUID、BIOS SN、主板 SN、物理磁盘 SN、MAC 地址）采集与解析。
- **用途与适用场景**：软件激活授权校验、本地数据复合密钥生成。
- **禁止在别处重复实现的内容**：禁止在各窗口或模块各自通过 WMI/CLI 查询机器指纹。
- **公开接口列表**：
  1. `QString HardwareInfoHelper::getCDiskPhysicalSerialNumber()`：获取 C 盘所在物理磁盘序列号。
  2. `QString HardwareInfoHelper::getAppDrivePhysicalSerialNumber()`：获取当前程序运行所在盘物理序列号。
  3. `QString HardwareInfoHelper::getDiskPhysicalSerialNumberByDrive(const QString& drivePath)`：根据驱动器盘符获取物理磁盘序列号。
  4. `QString HardwareInfoHelper::getBoardSerialNumber()`：获取主板 UUID / 序列号。
  5. `QString HardwareInfoHelper::getCpuId()`：获取 CPUID 序列号。
  6. `QString HardwareInfoHelper::getBiosSerialNumber()`：获取 BIOS 序列号。
  7. `QStringList HardwareInfoHelper::getAllDiskSerialNumbers()`：获取全部物理磁盘序列号。
  8. `QStringList HardwareInfoHelper::getAllMacAddresses()`：获取全部网卡 MAC 地址列表。

### 10. 全局热键中枢 (`HotkeyManager`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/HotkeyManager.h` 与 `HotkeyManager.cpp`
- **它是哪一件事实的唯一来源**：系统全局热键的注册、监听与派发。
- **用途与适用场景**：托盘唤醒、快速抓取、全局贴图等全局快捷键监听。
- **公开接口列表**：
  1. `static HotkeyManager& HotkeyManager::instance()`
  2. `bool HotkeyManager::registerHotkey(int id, uint modifiers, uint vk)`
  3. `void HotkeyManager::unregisterHotkey(int id)`
  4. `void HotkeyManager::reapplyHotkeys()`
  5. 信号：`void hotkeyPressed(int id)`

### 11. 快捷键映射管理 (`ShortcutManager`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/ShortcutManager.h` 与 `ShortcutManager.cpp`
- **它是哪一件事实的唯一来源**：应用内快捷键键位配置与持久化。
- **用途与适用场景**：全应用快捷键统一查询、重定义与配置管理。
- **公开接口列表**：
  1. `static ShortcutManager& ShortcutManager::instance()`
  2. `QKeySequence ShortcutManager::getShortcut(const QString& id) const`
  3. `void ShortcutManager::setShortcut(const QString& id, const QKeySequence& key)`
  4. `QList<ShortcutInfo> ShortcutManager::getAllShortcuts() const`
  5. `void ShortcutManager::save()` / `load()` / `resetToDefaults()`

### 12. 剪贴板监听真理源 (`ClipboardMonitor`)
- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/ClipboardMonitor.h` 与 `ClipboardMonitor.cpp`
- **它是哪一件事实的唯一来源**：系统剪贴板监听、去重与捕获信号发射。
- **用途与适用场景**：自动采集剪贴板新内容并通知主控流水线入库。
- **公开接口列表**：
  1. `static ClipboardMonitor& ClipboardMonitor::instance()`
  2. `void ClipboardMonitor::skipNext()`
  3. `void ClipboardMonitor::setIgnore(bool ignore)`
  4. `void ClipboardMonitor::forceNext(const QString& type = "")`
  5. `void ClipboardMonitor::clearLastHash()`
  6. 信号：`void newContentDetected(...)` / `void requestFeedback(...)`

---

## 五、 数据管理与访问当前唯一真理源 (`DatabaseManager`)

> **注**：在当前步骤（步骤 1）中，`DatabaseManager` 仍是数据存储的唯一门面；按照重构规划，后续步骤 2 将按职责拆分为独立的 Repository、QueryService 与 IngestService。

- **模块位置**：`RapidNotes_Source/RapidNotes/src/core/DatabaseManager.h` 与 `DatabaseManager.cpp`
- **它是哪一件事实的唯一来源**：笔记记录、分类树、标签映射、待办事项的底层 SQLite CRUD 与事务管理。
- **公开接口关键列表**：
  1. `static DatabaseManager& DatabaseManager::instance()`
  2. `int addNote(...)`：笔记入库
  3. `bool updateNote(...)`：笔记更新
  4. `bool deleteNotesBatch(const QList<int>& ids)`：笔记物理删除
  5. `bool softDeleteNotes(const QList<int>& ids)`：笔记软删除（入回收站）
  6. `bool updateNoteState(int id, const QString& column, const QVariant& value)`：状态字段更新
  7. `int addCategory(...)` / `getOrCreateCategoryByName(...)`：分类维护
  8. `QList<QVariantMap> searchNotes(...)`：笔记检索

---

## 六、 SSOT 接口与事实对照汇总表

| 序号 | 事实 | 唯一真理源 (SSOT) | 关键公开方法 | 归一化约束说明 |
|:---:|:---|:---|:---|:---|
| 1 | 条目类型词表 | `RapidNotes::ItemType` | `toString`, `fromString`, `isFileOrFolder`, `isPureText` | 唯一允许出现 `item_type` 字符串字面量的地方 |
| 2 | 文本类型判定规则 | `RapidNotes::ItemClassifier` | `detectIngestType`, `detectDisplayCategory`, `isUrl`, `isHexColor` | 纯内存判定，彻底与物理磁盘解耦 |
| 3 | 路径种类与图标样式 | `RapidNotes::PathSummary` | `summarize`, `summarizeFromSemicolonString`, `getIconName`, `getIconColor` | 单文件/单文件夹/多文件/多文件夹统一图标与色值 |
| 4 | 字数阶梯与统计清洗 | `RapidNotes::WordBucket` | `calculate` | 阶梯清洗阈值 0..1001 严格统一 |
| 5 | 标签编解码与双向流转 | `RapidNotes::TagCodec` | `split`, `join`, `joinDisplay` | 唯一标签拆分与拼接实现，杜绝手工造轮子 |
| 6 | 图片 64x64 缩略图 | `RapidNotes::ThumbnailMaker` | `makeThumbnail`, `makeThumbnailIcon` | 平滑纵横比缩略图唯一实现 |
| 7 | 矢量图标渲染与缓存 | `IconHelper` | `getIcon` | 全局线程安全矢量渲染与缓存 |
| 8 | 矢量图标 SVG 字典 | `SvgIcons` | `icons` | 原始矢量数据字典 |
| 9 | 硬件指纹与底层设备 | `HardwareInfoHelper` | `getCpuId`, `getBoardSerialNumber`, `getAllDiskSerialNumbers` | 设备序列号采集唯一通道 |
| 10 | 系统全局热键 | `HotkeyManager` | `instance`, `registerHotkey` | 系统原生热键监听唯一门面 |
| 11 | 应用快捷键配置 | `ShortcutManager` | `instance`, `getShortcut`, `setShortcut` | 快捷键管理与持久化唯一入口 |
| 12 | 剪贴板事件监听 | `ClipboardMonitor` | `instance`, `skipNext`, `clearLastHash` | 系统剪贴板捕获与去重唯一入口 |
| 13 | 数据库持久化门面 | `DatabaseManager` | `instance`, `addNote`, `updateNote`, `searchNotes` | 数据访问与持久化唯一门面（待步骤2拆分） |
