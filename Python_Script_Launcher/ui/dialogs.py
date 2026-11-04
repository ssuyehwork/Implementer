import os
import sys
from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QDialog, QVBoxLayout, QHBoxLayout, QLabel, QLineEdit,
    QPushButton, QTextEdit, QFileDialog, QMessageBox,
    QTableWidget, QTableWidgetItem, QHeaderView
)
from models.script import Script


class ScriptEditDialog(QDialog):
    def __init__(self, script: Script, parent=None):
        super().__init__(parent)
        self.script = script
        self.setWindowTitle("编辑脚本配置" if script.id else "新增脚本")
        self.resize(560, 480)
        self._init_ui()

    def _init_ui(self):
        layout = QVBoxLayout(self)
        layout.setSpacing(12)

        # 脚本路径
        row_path = QHBoxLayout()
        self.edit_path = QLineEdit(self.script.path)
        self.edit_path.setPlaceholderText("选择或输入 .py 文件路径...")
        btn_browse = QPushButton("浏览...")
        btn_browse.clicked.connect(self._browse_script)
        row_path.addWidget(self.edit_path)
        row_path.addWidget(btn_browse)
        layout.addWidget(QLabel("脚本文件路径 (*):"))
        layout.addLayout(row_path)

        # 脚本名称
        self.edit_name = QLineEdit(self.script.name)
        self.edit_name.setPlaceholderText("留空则默认使用文件名...")
        layout.addWidget(QLabel("显示名称:"))
        layout.addWidget(self.edit_name)

        # Python 解释器路径
        row_py = QHBoxLayout()
        self.edit_py = QLineEdit(self.script.python_path or sys.executable)
        self.edit_py.setPlaceholderText("默认使用当前 Python 解释器...")
        btn_browse_py = QPushButton("选择解释器...")
        btn_browse_py.clicked.connect(self._browse_python)
        row_py.addWidget(self.edit_py)
        row_py.addWidget(btn_browse_py)
        layout.addWidget(QLabel("Python 解释器路径:"))
        layout.addLayout(row_py)

        # 运行参数
        self.edit_args = QLineEdit(self.script.args)
        self.edit_args.setPlaceholderText("例如: --date 20260101 --debug --workers 4")
        layout.addWidget(QLabel("运行参数 (Arguments):"))
        layout.addWidget(self.edit_args)

        # 定时表达式 (Cron / Interval)
        self.edit_cron = QLineEdit(self.script.schedule_cron)
        self.edit_cron.setPlaceholderText("Cron 格式 (如: 0 8 * * *) 或 interval:60 (每60秒)")
        layout.addWidget(QLabel("定时任务表达式 (可选):"))
        layout.addWidget(self.edit_cron)

        # 环境变量
        self.edit_env = QTextEdit()
        self.edit_env.setPlainText(self.script.env_vars)
        self.edit_env.setPlaceholderText("每行一个环境变量配置，如：\nAPI_KEY=secret_xxx\nDEBUG=1")
        self.edit_env.setMaximumHeight(80)
        layout.addWidget(QLabel("自定义环境变量 (Environment Variables):"))
        layout.addWidget(self.edit_env)

        # 底部按钮
        btn_layout = QHBoxLayout()
        btn_layout.addStretch()
        btn_cancel = QPushButton("取消")
        btn_cancel.clicked.connect(self.reject)
        btn_save = QPushButton("保存")
        btn_save.setObjectName("primaryBtn")
        btn_save.clicked.connect(self._on_save)
        btn_layout.addWidget(btn_cancel)
        btn_layout.addWidget(btn_save)
        layout.addLayout(btn_layout)

    def _browse_script(self):
        file, _ = QFileDialog.getOpenFileName(
            self, "选择 Python 脚本", "", "Python Files (*.py);;All Files (*)"
        )
        if file:
            self.edit_path.setText(os.path.abspath(file))
            if not self.edit_name.text().strip():
                self.edit_name.setText(os.path.basename(file))

    def _browse_python(self):
        file, _ = QFileDialog.getOpenFileName(
            self, "选择 Python 解释器", "", "Executable Files (*);;All Files (*)"
        )
        if file:
            self.edit_py.setText(os.path.abspath(file))

    def _on_save(self):
        path = self.edit_path.text().strip()
        if not path:
            QMessageBox.warning(self, "提示", "脚本路径不能为空！")
            return
        if not os.path.exists(path):
            QMessageBox.warning(self, "提示", f"指定的脚本文件不存在:\n{path}")
            return

        name = self.edit_name.text().strip() or os.path.basename(path)
        self.script.path = os.path.abspath(path)
        self.script.name = name
        self.script.python_path = self.edit_py.text().strip()
        self.script.args = self.edit_args.text().strip()
        self.script.schedule_cron = self.edit_cron.text().strip()
        self.script.env_vars = self.edit_env.toPlainText().strip()
        self.accept()


class HistoryDialog(QDialog):
    def __init__(self, histories, script_name: str = "", parent=None):
        super().__init__(parent)
        self.setWindowTitle(f"运行历史记录 - {script_name}" if script_name else "运行历史记录")
        self.resize(780, 480)
        self.histories = histories
        self._init_ui()

    def _init_ui(self):
        layout = QVBoxLayout(self)

        table = QTableWidget()
        table.setColumnCount(6)
        table.setHorizontalHeaderLabels(["ID", "脚本名称", "开始时间", "结束时间", "耗时(秒)", "执行结果"])
        table.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeToContents)
        table.horizontalHeader().setSectionResizeMode(1, QHeaderView.Stretch)
        table.setRowCount(len(self.histories))

        for row, h in enumerate(self.histories):
            table.setItem(row, 0, QTableWidgetItem(str(h.id)))
            table.setItem(row, 1, QTableWidgetItem(h.script_name))
            table.setItem(row, 2, QTableWidgetItem(h.start_time or "-"))
            table.setItem(row, 3, QTableWidgetItem(h.end_time or "-"))
            table.setItem(row, 4, QTableWidgetItem(str(h.duration_sec)))

            res_item = QTableWidgetItem(h.result)
            if "SUCCESS" in h.result:
                res_item.setForeground(Qt.green)
            else:
                res_item.setForeground(Qt.red)
            table.setItem(row, 5, res_item)

        layout.addWidget(table)

        btn_layout = QHBoxLayout()
        btn_layout.addStretch()
        btn_close = QPushButton("关闭")
        btn_close.clicked.connect(self.accept)
        btn_layout.addWidget(btn_close)
        layout.addLayout(btn_layout)
