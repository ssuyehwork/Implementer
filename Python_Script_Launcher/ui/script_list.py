import os
from typing import List, Optional
from PySide6.QtCore import Qt, Signal, QMimeData
from PySide6.QtGui import QColor, QFont, QIcon, QAction
from PySide6.QtWidgets import (
    QTreeWidget, QTreeWidgetItem, QHeaderView, QMenu, QMessageBox
)
from models.script import Script


class ScriptTreeWidget(QTreeWidget):
    # 交互信号
    run_requested = Signal(int)             # script_id
    stop_requested = Signal(int)            # script_id
    edit_requested = Signal(int)            # script_id
    delete_requested = Signal(int)          # script_id
    toggle_top_requested = Signal(int)      # script_id
    copy_path_requested = Signal(str)       # path
    open_dir_requested = Signal(str)        # path
    history_requested = Signal(int)         # script_id
    files_dropped = Signal(list)            # list of file paths

    COL_STAR = 0
    COL_NAME = 1
    COL_STATUS = 2
    COL_RUN_COUNT = 3
    COL_LAST_RUN = 4
    COL_PATH = 5

    ROLE_SCRIPT_ID = Qt.UserRole + 1

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setAcceptDrops(True)
        self.setDragDropMode(QTreeWidget.DropOnly)
        self._init_tree()

    def _init_tree(self):
        self.setHeaderLabels(["★", "脚本名称", "状态", "运行次数", "最后运行时间", "路径"])
        self.setColumnCount(6)
        self.setRootIsDecorated(False)
        self.setAlternatingRowColors(True)
        self.setContextMenuPolicy(Qt.CustomContextMenu)
        self.customContextMenuRequested.connect(self._show_context_menu)
        self.itemDoubleClicked.connect(self._on_item_double_clicked)

        header = self.header()
        header.setSectionResizeMode(self.COL_STAR, QHeaderView.ResizeToContents)
        header.setSectionResizeMode(self.COL_NAME, QHeaderView.Interactive)
        header.setSectionResizeMode(self.COL_STATUS, QHeaderView.ResizeToContents)
        header.setSectionResizeMode(self.COL_RUN_COUNT, QHeaderView.ResizeToContents)
        header.setSectionResizeMode(self.COL_LAST_RUN, QHeaderView.Interactive)
        header.setSectionResizeMode(self.COL_PATH, QHeaderView.Stretch)
        self.setColumnWidth(self.COL_NAME, 200)
        self.setColumnWidth(self.COL_LAST_RUN, 150)

    # 拖拽支持
    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls():
            event.acceptProposedAction()
        else:
            event.ignore()

    def dragMoveEvent(self, event):
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
            self.files_dropped.emit(py_files)
            event.acceptProposedAction()
        else:
            event.ignore()

    def update_scripts(self, scripts: List[Script], status_map: dict):
        selected_id = self.get_selected_script_id()
        self.clear()

        for s in scripts:
            item = QTreeWidgetItem()
            item.setData(0, self.ROLE_SCRIPT_ID, s.id)

            # 星标/置顶
            if s.is_top:
                item.setText(self.COL_STAR, "★")
                item.setForeground(self.COL_STAR, QColor("#ffd700"))
            else:
                item.setText(self.COL_STAR, "☆")
                item.setForeground(self.COL_STAR, QColor("#666666"))
            item.setTextAlignment(self.COL_STAR, Qt.AlignCenter)

            # 脚本名称
            item.setText(self.COL_NAME, s.name)
            font = item.font(self.COL_NAME)
            if s.is_top:
                font.setBold(True)
            item.setFont(self.COL_NAME, font)

            # 状态
            st = status_map.get(s.id, "IDLE")
            item.setText(self.COL_STATUS, self._format_status_text(st))
            item.setForeground(self.COL_STATUS, self._format_status_color(st))
            item.setTextAlignment(self.COL_STATUS, Qt.AlignCenter)

            # 运行次数
            item.setText(self.COL_RUN_COUNT, str(s.run_count))
            item.setTextAlignment(self.COL_RUN_COUNT, Qt.AlignCenter)

            # 最后运行时间
            item.setText(self.COL_LAST_RUN, s.last_run_time or "-")

            # 脚本路径
            item.setText(self.COL_PATH, s.path)
            item.setToolTip(self.COL_PATH, f"完整路径: {s.path}\n参数: {s.args or '(无)'}\n解释器: {s.python_path or '默认'}")

            self.addTopLevelItem(item)

            if selected_id is not None and s.id == selected_id:
                self.setCurrentItem(item)

    def _format_status_text(self, status: str) -> str:
        if status == "RUNNING":
            return "● 运行中"
        elif status == "FINISHED":
            return "✓ 已完成"
        elif status == "ERROR":
            return "✖ 异常"
        elif status == "STOPPED":
            return "⏹ 已停止"
        return "就绪"

    def _format_status_color(self, status: str) -> QColor:
        if status == "RUNNING":
            return QColor("#4ec9b0")  # 青绿
        elif status == "FINISHED":
            return QColor("#89d185")  # 浅绿
        elif status == "ERROR":
            return QColor("#f44747")  # 红色
        elif status == "STOPPED":
            return QColor("#cca700")  # 橙黄
        return QColor("#888888")

    def get_selected_script_id(self) -> Optional[int]:
        item = self.currentItem()
        if item:
            return item.data(0, self.ROLE_SCRIPT_ID)
        return None

    def get_selected_script_path(self) -> Optional[str]:
        item = self.currentItem()
        if item:
            return item.text(self.COL_PATH)
        return None

    def _on_item_double_clicked(self, item, column):
        script_id = item.data(0, self.ROLE_SCRIPT_ID)
        if column == self.COL_STAR:
            # 双击星标列快速切换置顶
            self.toggle_top_requested.emit(script_id)
        else:
            # 双击运行脚本
            self.run_requested.emit(script_id)

    def _show_context_menu(self, pos):
        item = self.itemAt(pos)
        if not item:
            return

        script_id = item.data(0, self.ROLE_SCRIPT_ID)
        script_path = item.text(self.COL_PATH)
        is_top = (item.text(self.COL_STAR) == "★")
        is_running = ("运行中" in item.text(self.COL_STATUS))

        menu = QMenu(self)

        act_run = menu.addAction("▶ 运行脚本")
        act_run.setEnabled(not is_running)
        act_run.triggered.connect(lambda: self.run_requested.emit(script_id))

        act_stop = menu.addAction("⏹ 停止运行")
        act_stop.setEnabled(is_running)
        act_stop.triggered.connect(lambda: self.stop_requested.emit(script_id))

        menu.addSeparator()

        act_top = menu.addAction("★ 取消置顶" if is_top else "★ 设为置顶")
        act_top.triggered.connect(lambda: self.toggle_top_requested.emit(script_id))

        act_edit = menu.addAction("⚙ 编辑配置...")
        act_edit.triggered.connect(lambda: self.edit_requested.emit(script_id))

        act_history = menu.addAction("📜 运行历史...")
        act_history.triggered.connect(lambda: self.history_requested.emit(script_id))

        menu.addSeparator()

        act_copy = menu.addAction("📋 复制路径")
        act_copy.triggered.connect(lambda: self.copy_path_requested.emit(script_path))

        act_open_dir = menu.addAction("📁 打开所在目录")
        act_open_dir.triggered.connect(lambda: self.open_dir_requested.emit(script_path))

        menu.addSeparator()

        act_del = menu.addAction("🗑 删除脚本")
        act_del.triggered.connect(lambda: self.delete_requested.emit(script_id))

        menu.exec(self.mapToGlobal(pos))
