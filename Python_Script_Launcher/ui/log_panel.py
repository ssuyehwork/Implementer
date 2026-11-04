from datetime import datetime
from PySide6.QtCore import Qt
from PySide6.QtGui import QColor, QTextCharFormat, QTextCursor
from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel, QPushButton,
    QPlainTextEdit, QCheckBox, QComboBox
)


class LogPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._auto_scroll = True
        self._current_filter_script_id = None
        self._init_ui()

    def _init_ui(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(6)

        # 顶部工具栏
        toolbar = QHBoxLayout()
        toolbar.setSpacing(8)

        self.lbl_title = QLabel("📄 运行日志")
        self.lbl_title.setStyleSheet("font-weight: bold; font-size: 14px; color: #569cd6;")
        toolbar.addWidget(self.lbl_title)

        toolbar.addStretch()

        self.cb_filter = QComboBox()
        self.cb_filter.addItem("全部输出", None)
        self.cb_filter.currentIndexChanged.connect(self._on_filter_changed)
        toolbar.addWidget(QLabel("过滤:"))
        toolbar.addWidget(self.cb_filter)

        self.chk_autoscroll = QCheckBox("自动滚动")
        self.chk_autoscroll.setChecked(True)
        self.chk_autoscroll.toggled.connect(lambda v: setattr(self, "_auto_scroll", v))
        toolbar.addWidget(self.chk_autoscroll)

        self.btn_copy = QPushButton("复制")
        self.btn_copy.setToolTip("复制所有日志")
        self.btn_copy.clicked.connect(self.copy_logs)
        toolbar.addWidget(self.btn_copy)

        self.btn_clear = QPushButton("清空")
        self.btn_clear.clicked.connect(self.clear_logs)
        toolbar.addWidget(self.btn_clear)

        layout.addLayout(toolbar)

        # 文本输出框
        self.text_edit = QPlainTextEdit()
        self.text_edit.setReadOnly(True)
        self.text_edit.setMaximumBlockCount(10000)  # 限制最大行数防止内存占用过高
        layout.addWidget(self.text_edit)

        # 颜色预设
        self._format_info = QTextCharFormat()
        self._format_info.setForeground(QColor("#4ec9b0"))  # 绿色偏青

        self._format_warn = QTextCharFormat()
        self._format_warn.setForeground(QColor("#dcdcaa"))  # 黄色

        self._format_error = QTextCharFormat()
        self._format_error.setForeground(QColor("#f44747"))  # 红色

        self._format_default = QTextCharFormat()
        self._format_default.setForeground(QColor("#d4d4d4"))  # 浅灰白

        self._format_header = QTextCharFormat()
        self._format_header.setForeground(QColor("#569cd6"))  # 蓝色
        self._format_header.setFontWeight(600)

        # 日志缓冲区，用于按脚本切换过滤 (script_id, level, time_str, message)
        self._log_records = []

    def append_log(self, script_id: int, level: str, message: str, script_name: str = ""):
        now_str = datetime.now().strftime("%H:%M:%S")
        record = (script_id, level, now_str, message, script_name)
        self._log_records.append(record)
        if len(self._log_records) > 10000:
            self._log_records.pop(0)

        # 检查是否满足当前筛选
        if self._current_filter_script_id is not None and self._current_filter_script_id != script_id:
            return

        self._render_log_line(record)

    def _render_log_line(self, record):
        script_id, level, now_str, message, script_name = record
        tag = f"[{script_name}]" if script_name else ""
        header_text = f"[{now_str}] [{level}] {tag} "

        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.End)

        if level == "ERROR":
            cursor.setCharFormat(self._format_error)
        elif level == "WARNING":
            cursor.setCharFormat(self._format_warn)
        elif "======" in message:
            cursor.setCharFormat(self._format_header)
        else:
            cursor.setCharFormat(self._format_info)

        cursor.insertText(header_text + message + "\n")

        if self._auto_scroll:
            self.text_edit.setTextCursor(cursor)
            self.text_edit.ensureCursorVisible()

    def set_script_filter(self, script_id: int = None, script_name: str = ""):
        self._current_filter_script_id = script_id
        self.text_edit.clear()
        for rec in self._log_records:
            if script_id is None or rec[0] == script_id:
                self._render_log_line(rec)

    def update_script_options(self, scripts):
        current_data = self.cb_filter.currentData()
        self.cb_filter.blockSignals(True)
        self.cb_filter.clear()
        self.cb_filter.addItem("全部输出", None)
        for s in scripts:
            self.cb_filter.addItem(f"{s.name} (ID: {s.id})", s.id)

        # 恢复先前选中
        idx = self.cb_filter.findData(current_data)
        if idx >= 0:
            self.cb_filter.setCurrentIndex(idx)
        self.cb_filter.blockSignals(False)

    def _on_filter_changed(self, index):
        data = self.cb_filter.currentData()
        self.set_script_filter(data)

    def clear_logs(self):
        self.text_edit.clear()
        if self._current_filter_script_id is None:
            self._log_records.clear()
        else:
            self._log_records = [r for r in self._log_records if r[0] != self._current_filter_script_id]

    def copy_logs(self):
        self.text_edit.selectAll()
        self.text_edit.copy()
        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.End)
        self.text_edit.setTextCursor(cursor)
