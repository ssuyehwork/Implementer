DARK_STYLE_SHEET = """
/* 全局深色基础样式 */
QWidget {
    background-color: #1e1e1e;
    color: #e0e0e0;
    font-family: "Segoe UI", "PingFang SC", "Microsoft YaHei", sans-serif;
    font-size: 13px;
    selection-background-color: #094771;
    selection-color: #ffffff;
}

/* 窗口与分割器 */
QMainWindow {
    background-color: #181818;
}

QSplitter::handle {
    background-color: #2b2b2b;
}

QSplitter::handle:horizontal {
    width: 3px;
}

QSplitter::handle:vertical {
    height: 3px;
}

QSplitter::handle:hover {
    background-color: #007acc;
}

/* 输入框 */
QLineEdit {
    background-color: #252526;
    border: 1px solid #3c3c3c;
    border-radius: 4px;
    padding: 6px 10px;
    color: #ffffff;
}

QLineEdit:focus {
    border: 1px solid #007acc;
    background-color: #2a2d2e;
}

/* 按钮 */
QPushButton {
    background-color: #2d2d2d;
    border: 1px solid #3e3e42;
    border-radius: 4px;
    padding: 6px 14px;
    color: #cccccc;
    font-weight: 500;
}

QPushButton:hover {
    background-color: #3e3e42;
    border-color: #555555;
    color: #ffffff;
}

QPushButton:pressed {
    background-color: #007acc;
    border-color: #007acc;
    color: #ffffff;
}

QPushButton:disabled {
    background-color: #252526;
    border-color: #333333;
    color: #656565;
}

/* 主操作按钮（强调色） */
QPushButton#primaryBtn {
    background-color: #0e639c;
    border-color: #1177bb;
    color: #ffffff;
}

QPushButton#primaryBtn:hover {
    background-color: #1177bb;
}

QPushButton#dangerBtn {
    background-color: #a1260d;
    border-color: #c72e10;
    color: #ffffff;
}

QPushButton#dangerBtn:hover {
    background-color: #c72e10;
}

/* 树形列表 */
QTreeWidget {
    background-color: #252526;
    border: 1px solid #2d2d2d;
    border-radius: 4px;
    outline: none;
    color: #cccccc;
}

QTreeWidget::item {
    height: 32px;
    padding: 2px 4px;
    border-bottom: 1px solid #2a2a2a;
}

QTreeWidget::item:hover {
    background-color: #2a2d2e;
}

QTreeWidget::item:selected {
    background-color: #094771;
    color: #ffffff;
}

QHeaderView::section {
    background-color: #2d2d30;
    color: #aaaaaa;
    padding: 6px;
    border: none;
    border-right: 1px solid #3c3c3c;
    border-bottom: 1px solid #3c3c3c;
    font-weight: bold;
}

/* 文本/日志输出区域 */
QPlainTextEdit {
    background-color: #1e1e1e;
    border: 1px solid #2d2d2d;
    border-radius: 4px;
    color: #d4d4d4;
    font-family: "Consolas", "Courier New", "Fira Code", monospace;
    font-size: 12px;
    padding: 6px;
}

/* 滚动条 */
QScrollBar:vertical {
    border: none;
    background: #1e1e1e;
    width: 10px;
    margin: 0px 0px 0px 0px;
}

QScrollBar::handle:vertical {
    background: #3e3e42;
    min-height: 20px;
    border-radius: 5px;
}

QScrollBar::handle:vertical:hover {
    background: #505054;
}

QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0px;
}

QScrollBar:horizontal {
    border: none;
    background: #1e1e1e;
    height: 10px;
    margin: 0px 0px 0px 0px;
}

QScrollBar::handle:horizontal {
    background: #3e3e42;
    min-width: 20px;
    border-radius: 5px;
}

QScrollBar::handle:horizontal:hover {
    background: #505054;
}

QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
    width: 0px;
}

/* 右键菜单 */
QMenu {
    background-color: #252526;
    border: 1px solid #3c3c3c;
    padding: 4px;
}

QMenu::item {
    padding: 6px 24px 6px 12px;
    border-radius: 2px;
}

QMenu::item:selected {
    background-color: #094771;
    color: #ffffff;
}

QMenu::separator {
    height: 1px;
    background: #3c3c3c;
    margin: 4px 6px;
}

/* 对话框与标签 */
QDialog {
    background-color: #1e1e1e;
}

QLabel {
    color: #cccccc;
}

QComboBox {
    background-color: #252526;
    border: 1px solid #3c3c3c;
    border-radius: 4px;
    padding: 4px 10px;
    color: #ffffff;
}

QComboBox::drop-down {
    border: none;
}

QComboBox QAbstractItemView {
    background-color: #252526;
    border: 1px solid #3c3c3c;
    selection-background-color: #094771;
    color: #ffffff;
}

QTabWidget::pane {
    border: 1px solid #2d2d2d;
    background: #1e1e1e;
}

QTabBar::tab {
    background: #252526;
    color: #969696;
    padding: 8px 16px;
    border-top-left-radius: 4px;
    border-top-right-left-radius: 4px;
}

QTabBar::tab:selected {
    background: #1e1e1e;
    color: #ffffff;
    border-bottom: 2px solid #007acc;
}

QStatusBar {
    background-color: #007acc;
    color: #ffffff;
    font-size: 12px;
}
"""


def apply_dark_theme(app):
    app.setStyleSheet(DARK_STYLE_SHEET)
