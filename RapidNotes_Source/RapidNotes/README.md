# 备份方案: QuarkMeta

**备份时间**：2026-11-06 13:27:07
**备份目录**：Buk_QuarkMeta_20261106_132705

---

修复回收站视图下按快捷键 Ctrl+Space 没有弹出提示及接口未按场景分流的问题：
1. 深入分析并定位 Windows 底层键盘钩子将 Ctrl+Space 派发至 AppShortcutController 与 PanelMediator 的调用链路；
2. 在 PanelMediator::setupConnections 的 moveToTrashRequested 响应槽中，补充回收站视图判定（isTrashView）；
3. 在回收站视图下：若有选中项，调用 PermanentDeleteService::execute 执行永久删除；若无选中项，调用 ToolTipOverlay::showToast 弹出统一防护提示（“请先选择要永久删除的项目”），彻底杜绝误触；
4. 普通物理目录下保持原有逻辑不变（非空调用 moveToTrash）；
5. 同步更新实施方案文档 RecycleBin_Delete_Shortcuts_Plan.md 至 v2.1。
