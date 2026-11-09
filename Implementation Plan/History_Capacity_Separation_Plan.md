# 历史记录容量边界隔离与上限恢复实施方案

**方案版本**：v1.0
**日期**：2026-11-09
**关联 Memories_QuarkMeta.md 条目**：`## 41. 侧边栏目录导航树“回收站”置顶与“最近访问”记录扩容设计理念` 中 `41.1.2 “最近访问”与历史记录容量边界严格分离契约`

---

## 物理阅读证据清单 (Gate 1: Pre-Draft Physical Evidence)

1. `QuarkMeta_Source/src/core/NavigationHistoryService.h`：底层历史池为 30 项；
2. `QuarkMeta_Source/src/core/NavigationHistoryService.cpp`：`recordRecentVisitedFolder` 上限恢复为 15（`> 14`）；
3. `QuarkMeta_Source/src/ui/AddressHistoryPanel.cpp`：地址栏下拉历史面板循环渲染时截断最多 15 项；
4. `QuarkMeta_Source/src/ui/controllers/ContentContextMenu.cpp`：“移动到”子菜单最多取 15 项；
5. `QuarkMeta_Source/src/ui/NavPanel.cpp`：侧边栏“最近访问”保持 30 项深度展示。
