import os
import platform
import subprocess
import sys
from PySide6.QtCore import Qt, QUrl
from PySide6.QtGui import QAction, QDesktopServices, QGuiApplication, QIcon
from PySide6.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QSplitter,
    QLineEdit, QPushButton, QLabel, QMessageBox, QFileDialog,
    QSystemTrayIcon, QMenu, QStatusBar
)

from database.db import Database
from models.script import Script
from core.script_manager import ScriptManager
from core.scheduler import TaskScheduler
from ui.script_list import ScriptTreeWidget
from ui.log_panel import LogPanel
from ui.dialogs import ScriptEditDialog, HistoryDialog


class MainWindow(QMainWindow):
    def __init__(self, db: Database, parent=None):
        super().__init__(parent)
        self.db = db
        self.script_manager = ScriptManager(self.db, parent=self)
        self.scheduler = TaskScheduler(self.script_manager)

        self.setWindowTitle("PyLauncher Pro - Python 脚本启动器")
        self.resize(1360, 820)
        self.setAcceptDrops(True)

        self._init_ui()
        self._init_tray()
        self._bind_signals()
        self._reload_scripts()
        self._init_schedules()

    def _init_ui(self):
        central_widget = QWidget(self)
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        main_layout.setContentsMargins(12, 12, 12, 12)
        main_layout.setSpacing(10)

        # 顶部搜索与快捷操作栏
        top_bar = QHBoxLayout()
        top_bar.setSpacing(8)

        self.search_edit = QLineEdit()
        self.search_edit.setPlaceholderText("🔍 搜索脚本名称、路径或分组...")
        self.search_edit.setClearButtonEnabled(True)
        top_bar.addWidget(self.search_edit, stretch=1)

        self.btn_batch_run = QPushButton("⚡ 批量运行")
        self.btn_batch_run.setToolTip("顺序启动当前列表中的所有脚本")
        self.btn_batch_run.clicked.connect(self._batch_run_all)
        top_bar.addWidget(self.btn_batch_run)

        self.btn_global_history = QPushButton("📜 全部历史")
        self.btn_global_history.clicked.connect(self._show_global_history)
        top_bar.addWidget(self.btn_global_history)

        main_layout.addLayout(top_bar)

        # 中间 Splitter (左 30% 脚本列表，右 70% 日志输出)
        self.splitter = QSplitter(Qt.Horizontal)
        self.splitter.setChildrenCollapsible(False)

        # 左侧列表容器
        left_widget = QWidget()
        left_layout = QVBoxLayout(left_widget)
        left_layout.setContentsMargins(0, 0, 0, 0)
        left_layout.setSpacing(6)

        lbl_list_header = QLabel("📋 脚本列表 (支持直接拖拽 .py 脚本文件至此)")
        lbl_list_header.setStyleSheet("color: #888888; font-size: 11px;")
        left_layout.addWidget(lbl_list_header)

        self.script_tree = ScriptTreeWidget(self)
        left_layout.addWidget(self.script_tree)

        self.splitter.addWidget(left_widget)

        # 右侧日志面板
        self.log_panel = LogPanel(self)
        self.splitter.addWidget(self.log_panel)

        # 设置比例 3 : 7
        self.splitter.setStretchFactor(0, 3)
        self.splitter.setStretchFactor(1, 7)
        main_layout.addWidget(self.splitter, stretch=1)

        # 底部操作按钮栏
        bottom_bar = QHBoxLayout()
        bottom_bar.setSpacing(8)

        self.btn_add = QPushButton("➕ 新增脚本")
        self.btn_add.setObjectName("primaryBtn")
        self.btn_add.clicked.connect(self._add_script_dialog)
        bottom_bar.addWidget(self.btn_add)

        self.btn_run = QPushButton("▶ 运行")
        self.btn_run.setObjectName("primaryBtn")
        self.btn_run.clicked.connect(self._run_current)
        bottom_bar.addWidget(self.btn_run)

        self.btn_stop = QPushButton("⏹ 停止")
        self.btn_stop.setObjectName("dangerBtn")
        self.btn_stop.clicked.connect(self._stop_current)
        bottom_bar.addWidget(self.btn_stop)

        self.btn_edit = QPushButton("⚙ 编辑")
        self.btn_edit.clicked.connect(self._edit_current)
        bottom_bar.addWidget(self.btn_edit)

        self.btn_top = QPushButton("★ 置顶")
        self.btn_top.clicked.connect(self._toggle_top_current)
        bottom_bar.addWidget(self.btn_top)

        self.btn_copy_path = QPushButton("📋 复制路径")
        self.btn_copy_path.clicked.connect(self._copy_current_path)
        bottom_bar.addWidget(self.btn_copy_path)

        self.btn_open_dir = QPushButton("📁 打开目录")
        self.btn_open_dir.clicked.connect(self._open_current_dir)
        bottom_bar.addWidget(self.btn_open_dir)

        self.btn_delete = QPushButton("🗑 删除")
        self.btn_delete.clicked.connect(self._delete_current)
        bottom_bar.addWidget(self.btn_delete)

        bottom_bar.addStretch()

        self.lbl_stats = QLabel("脚本数: 0 | 运行中: 0")
        self.lbl_stats.setStyleSheet("color: #888888; font-size: 12px;")
        bottom_bar.addWidget(self.lbl_stats)

        main_layout.addLayout(bottom_bar)

        # 状态栏
        self.statusBar = QStatusBar()
        self.setStatusBar(self.statusBar)
        self.statusBar.showMessage("就绪")

    def _init_tray(self):
        """初始化系统托盘"""
        self.tray_icon = QSystemTrayIcon(self)
        icon_path = os.path.join(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
            "resources", "icons", "app_icon.png"
        )
        if os.path.exists(icon_path):
            icon = QIcon(icon_path)
            self.setWindowIcon(icon)
            self.tray_icon.setIcon(icon)

        self.tray_icon.setToolTip("PyLauncher Pro - Python 脚本启动器")

        tray_menu = QMenu()
        act_show = tray_menu.addAction("显示主窗口")
        act_show.triggered.connect(self.showNormal)
        tray_menu.addSeparator()
        act_quit = tray_menu.addAction("退出程序")
        act_quit.triggered.connect(self._safe_quit)

        self.tray_icon.setContextMenu(tray_menu)
        self.tray_icon.activated.connect(self._on_tray_activated)
        self.tray_icon.show()

    def _on_tray_activated(self, reason):
        if reason == QSystemTrayIcon.DoubleClick:
            if self.isVisible():
                self.hide()
            else:
                self.showNormal()
                self.activateWindow()

    def closeEvent(self, event):
        # 最小化到系统托盘
        if self.tray_icon.isVisible():
            self.hide()
            self.tray_icon.showMessage(
                "PyLauncher Pro",
                "程序已最小化到系统托盘，脚本将继续在后台执行。",
                QSystemTrayIcon.Information,
                2000
            )
            event.ignore()
        else:
            self._safe_quit()
            event.accept()

    def _safe_quit(self):
        self.scheduler.shutdown()
        self.tray_icon.hide()
        QGuiApplication.quit()

    def _bind_signals(self):
        # 搜索实时联动
        self.search_edit.textChanged.connect(self._on_search_text_changed)

        # 树视图交互信号
        self.script_tree.run_requested.connect(self._run_script)
        self.script_tree.stop_requested.connect(self._stop_script)
        self.script_tree.edit_requested.connect(self._edit_script)
        self.script_tree.delete_requested.connect(self._delete_script)
        self.script_tree.toggle_top_requested.connect(self._toggle_top)
        self.script_tree.copy_path_requested.connect(self._copy_path)
        self.script_tree.open_dir_requested.connect(self._open_dir)
        self.script_tree.history_requested.connect(self._show_script_history)
        self.script_tree.files_dropped.connect(self._on_files_dropped)

        # 业务层信号
        self.script_manager.script_log_received.connect(self._on_script_log)
        self.script_manager.script_status_changed.connect(self._on_script_status_changed)
        self.script_manager.script_finished.connect(self._on_script_finished)
        self.script_manager.scripts_reloaded.connect(self._reload_scripts)

    # 拖拽支持（整个主窗口）
    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls():
            event.acceptProposedAction()
        else:
            event.ignore()

    def dropEvent(self, event):
        urls = event.mimeData().urls()
        py_files = []
        for url in urls:
            f = url.toLocalFile()
            if f.lower().endswith(".py") and os.path.isfile(f):
                py_files.append(f)
        if py_files:
            self._on_files_dropped(py_files)
            event.acceptProposedAction()

    def _on_files_dropped(self, file_paths):
        added_count = 0
        for p in file_paths:
            res = self.script_manager.add_script(p)
            if res:
                added_count += 1
        if added_count > 0:
            self.statusBar.showMessage(f"成功导入 {added_count} 个脚本", 3000)

    def _reload_scripts(self):
        kw = self.search_edit.text().strip()
        if kw:
            scripts = self.script_manager.search_scripts(kw)
        else:
            scripts = self.script_manager.get_all_scripts()

        status_map = {s.id: self.script_manager.get_status(s.id) for s in scripts}
        self.script_tree.update_scripts(scripts, status_map)
        self.log_panel.update_script_options(self.script_manager.get_all_scripts())

        # 统计
        all_scripts = self.script_manager.get_all_scripts()
        running_cnt = sum(1 for s in all_scripts if self.script_manager.is_running(s.id))
        self.lbl_stats.setText(f"脚本数: {len(all_scripts)} | 运行中: {running_cnt}")

    def _on_search_text_changed(self, text):
        self._reload_scripts()

    def _on_script_log(self, script_id: int, level: str, message: str):
        script = self.db.get_script_by_id(script_id)
        name = script.name if script else f"ID:{script_id}"
        self.log_panel.append_log(script_id, level, message, name)

    def _on_script_status_changed(self, script_id: int, status: str):
        self._reload_scripts()
        if status == "RUNNING":
            script = self.db.get_script_by_id(script_id)
            if script:
                self.statusBar.showMessage(f"正在运行: {script.name}...", 3000)

    def _on_script_finished(self, script_id: int, return_code: int, duration: float):
        script = self.db.get_script_by_id(script_id)
        name = script.name if script else f"ID:{script_id}"
        if return_code == 0:
            self.statusBar.showMessage(f"✓ 脚本 [{name}] 执行成功 (耗时: {duration:.2f}s)", 5000)
        else:
            self.statusBar.showMessage(f"✖ 脚本 [{name}] 异常退出 (退出码: {return_code})", 5000)

    # 调度初始化
    def _init_schedules(self):
        for s in self.script_manager.get_all_scripts():
            if s.schedule_cron:
                self.scheduler.schedule_script(s.id, s.schedule_cron)

    # 快捷操作
    def _run_current(self):
        sid = self.script_tree.get_selected_script_id()
        if sid is not None:
            self._run_script(sid)

    def _stop_current(self):
        sid = self.script_tree.get_selected_script_id()
        if sid is not None:
            self._stop_script(sid)

    def _run_script(self, script_id: int):
        self.script_manager.run_script(script_id)

    def _stop_script(self, script_id: int):
        self.script_manager.stop_script(script_id)

    def _batch_run_all(self):
        scripts = self.script_manager.get_all_scripts()
        if not scripts:
            QMessageBox.information(self, "提示", "脚本列表中暂无脚本！")
            return
        idle_ids = [s.id for s in scripts if not self.script_manager.is_running(s.id)]
        if not idle_ids:
            QMessageBox.information(self, "提示", "所有脚本均已在运行中！")
            return
        self.script_manager.run_batch(idle_ids)
        self.statusBar.showMessage(f"已启动批量运行: {len(idle_ids)} 个脚本", 3000)

    def _add_script_dialog(self):
        new_script = Script()
        dlg = ScriptEditDialog(new_script, self)
        if dlg.exec():
            # 保存到数据库
            sid = self.db.add_script(new_script)
            new_script.id = sid
            if new_script.schedule_cron:
                self.scheduler.schedule_script(sid, new_script.schedule_cron)
            self._reload_scripts()
            self.statusBar.showMessage(f"新增脚本: {new_script.name}", 3000)

    def _edit_current(self):
        sid = self.script_tree.get_selected_script_id()
        if sid is not None:
            self._edit_script(sid)

    def _edit_script(self, script_id: int):
        script = self.db.get_script_by_id(script_id)
        if not script:
            return
        dlg = ScriptEditDialog(script, self)
        if dlg.exec():
            self.db.update_script(script)
            if script.schedule_cron:
                self.scheduler.schedule_script(script.id, script.schedule_cron)
            else:
                self.scheduler.remove_job(script.id)
            self._reload_scripts()
            self.statusBar.showMessage(f"已更新脚本配置: {script.name}", 3000)

    def _delete_current(self):
        sid = self.script_tree.get_selected_script_id()
        if sid is not None:
            self._delete_script(sid)

    def _delete_script(self, script_id: int):
        script = self.db.get_script_by_id(script_id)
        if not script:
            return
        ret = QMessageBox.question(
            self,
            "确认删除",
            f"确定要从启动器中移除脚本【{script.name}】吗？\n（注：磁盘上的实体脚本文件不会被删除）",
            QMessageBox.Yes | QMessageBox.No,
            QMessageBox.No
        )
        if ret == QMessageBox.Yes:
            self.scheduler.remove_job(script_id)
            self.script_manager.delete_script(script_id)
            self.statusBar.showMessage(f"已删除脚本: {script.name}", 3000)

    def _toggle_top_current(self):
        sid = self.script_tree.get_selected_script_id()
        if sid is not None:
            self._toggle_top(sid)

    def _toggle_top(self, script_id: int):
        self.script_manager.toggle_top(script_id)

    def _copy_current_path(self):
        p = self.script_tree.get_selected_script_path()
        if p:
            self._copy_path(p)

    def _copy_path(self, path: str):
        QGuiApplication.clipboard().setText(path)
        self.statusBar.showMessage(f"已复制路径到剪贴板: {path}", 2000)

    def _open_current_dir(self):
        p = self.script_tree.get_selected_script_path()
        if p:
            self._open_dir(p)

    def _open_dir(self, file_path: str):
        if not file_path or not os.path.exists(file_path):
            QMessageBox.warning(self, "提示", "文件或目录不存在！")
            return
        dir_path = os.path.dirname(os.path.abspath(file_path))

        # 跨平台打开目录
        try:
            if platform.system() == "Windows":
                os.startfile(dir_path)
            elif platform.system() == "Darwin":
                subprocess.Popen(["open", dir_path])
            else:
                subprocess.Popen(["xdg-open", dir_path])
        except Exception:
            # Qt 通用回退
            QDesktopServices.openUrl(QUrl.fromLocalFile(dir_path))

    def _show_script_history(self, script_id: int):
        script = self.db.get_script_by_id(script_id)
        histories = self.db.get_histories(script_id=script_id)
        dlg = HistoryDialog(histories, script_name=script.name if script else "", parent=self)
        dlg.exec()

    def _show_global_history(self):
        histories = self.db.get_histories(limit=300)
        dlg = HistoryDialog(histories, script_name="全部脚本", parent=self)
        dlg.exec()
