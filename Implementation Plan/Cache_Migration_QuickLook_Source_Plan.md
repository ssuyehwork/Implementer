# 缓存根目录整体搬家迁移与 QuickLook 预览图缓存源双轨选择实施方案

**文档语言**：中文（接口定义与 Git Merge Diff 标记除外）
**基准 Commit**：`d13c068`
**关联规范**：`Memories_QuarkMeta.md` 第 47 章节、`Standard Specifications for Implementation Plan.md`

---

## 物理阅读证据清单 (Gate 1: Pre-Draft Physical Evidence)

1. **涉及修改与阅读的物理文件列表**：
   - `QuarkMeta_Source/src/ui/dialogs/CacheLocationDialog.h`（已物理读取第 1~35 行，确认对话框结构与成员变量声明）
   - `QuarkMeta_Source/src/ui/dialogs/CacheLocationDialog.cpp`（已物理读取第 20~135 行，确认 `onBrowsePath`、`onRestoreDefault`、`onSaveAndAccept` 以及搬家迁移逻辑，发现旧代码使用 `QFile::copy` 且未清理原路径的伪迁移缺陷）
   - `QuarkMeta_Source/src/core/AppConfig.h`（已物理读取第 40~55 行，确认 `getGlobalCacheDir` 读取 `Cache/CustomLocation` 拼接 `/cache` 的单一事实来源）
   - `QuarkMeta_Source/src/ui/QuickLookWindow.h`（已物理读取第 20~60 行，确认 `loadOrExtractQuickLookImage` 原样声明）
   - `QuarkMeta_Source/src/ui/QuickLookWindow.cpp`（已物理读取第 1~35 行包含列表与第 325~390 行，确认双轨取图与降级逻辑）
   - `QuarkMeta_Source/src/util/DiskMediaExtractor.h` 与 `DiskMediaExtractor.cpp`（已物理读取第 185~210 行，确认 `DiskMediaExtractor::getDiskThumbCachePath(filePath)` 签名与实现）

2. **外部成员函数原样签名核验**：
   - `DiskMediaExtractor::getDiskThumbCachePath(const QString& filePath)`：`QuarkMeta_Source/src/util/DiskMediaExtractor.h:37`
   - `AppConfig::instance().getGlobalCacheDir()`：`QuarkMeta_Source/src/core/AppConfig.h:42`
   - `AppConfig::instance().getValue(const QString& key, const QVariant& defaultValue)`：`QuarkMeta_Source/src/core/AppConfig.h`
   - `AppConfig::instance().setValue(const QString& key, const QVariant& value)`：`QuarkMeta_Source/src/core/AppConfig.h`
   - `AppConfig::instance().remove(const QString& key)`：`QuarkMeta_Source/src/core/AppConfig.h`
   - `AppConfig::instance().sync()`：`QuarkMeta_Source/src/core/AppConfig.h`

---

## Chapter 1: Overview（概述与根因分析）

### 1.1 缺陷现象与架构纠偏
1. **纠偏一：假“迁移”变成真“搬家”（原路径数据必须彻底清理消除）**
   - 彻底解决旧代码只复制不删除的伪迁移行为，复制成功后从原路径彻底删除文件，并在完成后递归清理旧空目录，真正释放原磁盘空间。
2. **纠偏二：整棵 `cache` 根目录的整体搬迁**
   - 全局缓存统一以整棵 `cache` 树（含 `disk_thumbs` 与 `quicklook_previews`）为最小搬迁单位，杜绝分拆零碎处理，路径输入框标准化对齐 `.../cache`。
3. **纠偏三：QuickLook 预览图缓存源双轨互斥单选与平滑降级**
   - 界面提供互斥二选一复选框组（`disk_thumbs` vs `quicklook_previews`），配备思量说明注释；运行时若 `disk_thumbs` 不存在或未生成，自动平滑向下穿透实时解码，彻底杜绝黑屏。
