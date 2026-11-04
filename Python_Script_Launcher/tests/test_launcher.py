import os
import sys
import tempfile
import time
import unittest
from PySide6.QtCore import QCoreApplication
from PySide6.QtWidgets import QApplication

# 确保 Python_Script_Launcher 在 sys.path
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

from database.db import Database
from models.script import Script, RunHistory
from core.runner import ScriptRunner
from core.script_manager import ScriptManager
from core.scheduler import TaskScheduler
from ui.main_window import MainWindow


class TestPythonScriptLauncher(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        os.environ["QT_QPA_PLATFORM"] = "offscreen"
        cls.app = QApplication.instance()
        if not cls.app:
            cls.app = QApplication([])

    def setUp(self):
        self.temp_db = tempfile.NamedTemporaryFile(suffix=".db", delete=False)
        self.temp_db.close()
        self.db = Database(self.temp_db.name)

    def tearDown(self):
        if os.path.exists(self.temp_db.name):
            os.unlink(self.temp_db.name)

    def test_database_crud_and_sorting(self):
        # 1. 增
        s1 = Script(name="Script B", path="/tmp/b.py", is_top=0)
        s2 = Script(name="Script A", path="/tmp/a.py", is_top=1)
        id1 = self.db.add_script(s1)
        id2 = self.db.add_script(s2)
        self.assertTrue(id1 > 0)
        self.assertTrue(id2 > 0)

        # 2. 查与排序 (is_top=1 应该排在最前面)
        all_scripts = self.db.get_all_scripts()
        self.assertEqual(len(all_scripts), 2)
        self.assertEqual(all_scripts[0].name, "Script A")
        self.assertEqual(all_scripts[0].is_top, 1)

        # 3. 搜索
        searched = self.db.search_scripts("Script B")
        self.assertEqual(len(searched), 1)
        self.assertEqual(searched[0].id, id1)

        # 4. 改与置顶切换
        new_top = self.db.toggle_top(id1)
        self.assertEqual(new_top, 1)
        s1_updated = self.db.get_script_by_id(id1)
        self.assertEqual(s1_updated.is_top, 1)

        # 5. 运行计数
        self.db.record_run(id1)
        s1_ran = self.db.get_script_by_id(id1)
        self.assertEqual(s1_ran.run_count, 1)
        self.assertTrue(len(s1_ran.last_run_time) > 0)

        # 6. 删
        self.db.delete_script(id1)
        self.assertIsNone(self.db.get_script_by_id(id1))
        self.assertEqual(len(self.db.get_all_scripts()), 1)

    def test_history_records(self):
        h = RunHistory(
            script_id=10,
            script_name="Demo.py",
            script_path="/tmp/demo.py",
            start_time="2026-01-01 10:00:00",
            end_time="2026-01-01 10:00:02",
            duration_sec=2.1,
            exit_code=0,
            result="SUCCESS"
        )
        hid = self.db.add_history(h)
        self.assertTrue(hid > 0)

        histories = self.db.get_histories(script_id=10)
        self.assertEqual(len(histories), 1)
        self.assertEqual(histories[0].result, "SUCCESS")
        self.assertEqual(histories[0].duration_sec, 2.1)

    def test_runner_and_manager_execution(self):
        # 创建临时可执行脚本
        with tempfile.NamedTemporaryFile("w", suffix=".py", delete=False) as f:
            f.write("import sys\nprint('Output message line 1')\nprint('Error message line 2', file=sys.stderr)\n")
            script_path = f.name

        try:
            mgr = ScriptManager(self.db)
            script = mgr.add_script(script_path, name="TestRunnerScript")
            self.assertIsNotNone(script)

            logs = []
            statuses = []
            mgr.script_log_received.connect(lambda sid, lvl, msg: logs.append((lvl, msg)))
            mgr.script_status_changed.connect(lambda sid, st: statuses.append(st))

            # 启动脚本
            started = mgr.run_script(script.id)
            self.assertTrue(started)

            # 等待执行完毕
            for _ in range(60):
                self.app.processEvents()
                if not mgr.is_running(script.id):
                    break
                time.sleep(0.05)

            self.assertFalse(mgr.is_running(script.id))
            self.assertIn("RUNNING", statuses)
            self.assertIn("FINISHED", statuses)

            # 验证日志包含输出
            log_texts = [msg for lvl, msg in logs]
            self.assertTrue(any("Output message line 1" in t for t in log_texts))
            self.assertTrue(any("Error message line 2" in t for t in log_texts))

            # 验证历史记录自动生成
            histories = self.db.get_histories(script_id=script.id)
            self.assertEqual(len(histories), 1)
            self.assertEqual(histories[0].result, "SUCCESS")

        finally:
            if os.path.exists(script_path):
                os.unlink(script_path)

    def test_main_window_integration(self):
        win = MainWindow(self.db)
        win.show()

        # 拖拽导入模拟
        with tempfile.NamedTemporaryFile("w", suffix=".py", delete=False) as f:
            f.write("print('Hello from UI')\n")
            ui_script_path = f.name

        try:
            win._on_files_dropped([ui_script_path])
            all_scripts = win.script_manager.get_all_scripts()
            self.assertTrue(any(s.path == os.path.abspath(ui_script_path) for s in all_scripts))

            # 搜索测试
            win.search_edit.setText("Hello")
            win.search_edit.clear()

            # 运行测试
            s_obj = [s for s in all_scripts if s.path == os.path.abspath(ui_script_path)][0]
            win._run_script(s_obj.id)

            for _ in range(60):
                self.app.processEvents()
                if not win.script_manager.is_running(s_obj.id):
                    break
                time.sleep(0.05)

            self.assertFalse(win.script_manager.is_running(s_obj.id))
        finally:
            win._safe_quit()
            if os.path.exists(ui_script_path):
                os.unlink(ui_script_path)


if __name__ == "__main__":
    unittest.main()
