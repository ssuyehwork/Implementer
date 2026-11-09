# 可执行文件与独立图标格式真机专属 Logo 提取与隔离缓存实施方案

**文档语言**：中文（接口定义与 Git Merge Diff 标记除外）
**基准 Commit**：`d13c068`
**关联规范**：`Memories_QuarkMeta.md` 第 48 章节、`Standard Specifications for Implementation Plan.md`

---

## 物理阅读证据清单 (Gate 1: Pre-Draft Physical Evidence)

1. **涉及修改与阅读的物理文件列表**：
   - `QuarkMeta_Source/src/ui/WindowsShellThumbnailProvider.h`（已物理读取第 1~70 行，确认公开接口签名与静态成员声明）
   - `QuarkMeta_Source/src/ui/WindowsShellThumbnailProvider.cpp`（已物理读取第 1~25 行确认头文件包含、第 55~120 行与第 150~215 行确认 `iconKey` 计算入口、`getFileIconFast` 缓存检查与 `handleIconLoad` 异步加载管线）
   - `QuarkMeta_Source/src/ui/models/ItemModelBase.h`（已物理读取第 60~110 行，确认 `onSystemIconLoaded` 与 `flushIconRefresh` 行级局部刷新机制）
   - `QuarkMeta_Source/src/ui/models/DiskItemModel.cpp`（已物理读取第 800~830 行，确认 `cleanKey` 二级缓存与 `ShellIconManager::getFileIconFast` 调用时序）
   - `QuarkMeta_Source/src/ui/FavoritePanel.cpp`（已物理读取第 430 行与第 855~890 行，确认 `IconLoadNotifier::iconLoaded` 刷新机制）
   - `QuarkMeta_Source/src/ui/TreeItemDelegate.h`（已物理读取第 110~220 行，确认微型卡片绘制逻辑与尺寸约束）

2. **外部成员函数原样签名核验**：
   - `WindowsShellThumbnailProvider::iconKey(const QString& filePath, bool isDir, const QString& suffix)`：`QuarkMeta_Source/src/ui/WindowsShellThumbnailProvider.h:42`
   - `WindowsShellThumbnailProvider::getFileIconFast(const QString& filePath, bool isDir, const QString& suffix)`：`QuarkMeta_Source/src/ui/WindowsShellThumbnailProvider.h:45`
   - `WindowsShellThumbnailProvider::isIconCached(const QString& filePath, bool isDir, const QString& suffix)`：`QuarkMeta_Source/src/ui/WindowsShellThumbnailProvider.h:46`
   - `IconLoadNotifier::iconLoadedForKey(const QString& key)`：`QuarkMeta_Source/src/ui/WindowsShellThumbnailProvider.h:23`

---

## Chapter 1: Overview（概述与根因分析）

### 1.1 缺陷现象与根本原因
1. **缺陷现象**：
   - 在主界面目录导航（树形/列表/网格视图）及侧边栏收藏夹中，所有 `.exe`（如 `OCR.exe`、`RapidNotes.exe`）均显示完全相同的 Windows 通用应用程序默认占位符图标，无法展现软件自身内嵌的专属品牌 Logo。
2. **根本原因深度定位**：
   - **原因 1：虚拟文件名导致未读取真实 PE 资源**：
     传入虚拟文件名 `dummy.exe`，只向操作系统询问通用扩展名关联图标，从未读取可执行文件真实的磁盘物理二进制内容。
   - **原因 2：缓存键过度合并导致全局同化**：
     `iconKey` 对所有非目录文件仅使用其后缀名（`"exe"`），导致所有 exe 共享单一槽位，互相覆盖。
   - **原因 3：模型异步通知粒度失配**：
     无法针对单个 exe 文件进行行级重绘。

### 1.2 架构重构契约与解决方案
1. 建立独立图标扩展名白名单（`.exe`、`.lnk`、`.ico`、`.scr`），采用规范化物理路径 `QDir::cleanPath(filePath)` 作为专属缓存键；普通文件继续复用扩展名键。
2. 在异步子线程中以真实 `QFileInfo` 解析 PE 资源提取真机专属 Logo，失败平滑降级至通用占位符，处于 COM 与熔断保护之下。
