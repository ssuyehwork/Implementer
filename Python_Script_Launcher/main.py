import os
import sys

# 保证当前根目录在 sys.path 中
APP_DIR = os.path.dirname(os.path.abspath(__file__))
if APP_DIR not in sys.path:
    sys.path.insert(0, APP_DIR)

from PySide6.QtCore import Qt
from PySide6.QtWidgets import QApplication

from database.db import Database
from ui.dark_theme import apply_dark_theme
from ui.main_window import MainWindow


def main():
    # 启用高 DPI 支持属性
    QApplication.setHighDpiScaleFactorRoundingPolicy(
        Qt.HighDpiScaleFactorRoundingPolicy.PassThrough
    )

    app = QApplication(sys.argv)
    app.setApplicationName("PyLauncher Pro")
    app.setApplicationDisplayName("PyLauncher Pro - Python 脚本启动器")

    # 应用现代化深色主题
    apply_dark_theme(app)

    # 数据库初始化
    db = Database()

    # 主窗口构建与展示
    window = MainWindow(db)
    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
