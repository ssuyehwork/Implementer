import os
import sys
from datetime import datetime
from typing import Dict, List, Optional
from PySide6.QtCore import QObject, Signal

from database.db import Database
from models.script import Script, RunHistory
from core.runner import ScriptRunner


class ScriptManager(QObject):
    # 集中事件信号
    script_log_received = Signal(int, str, str)       # script_id, level, message
    script_status_changed = Signal(int, str)         # script_id, status
    script_finished = Signal(int, int, float)        # script_id, returncode, duration
    scripts_reloaded = Signal()

    def __init__(self, db: Database, parent=None):
        super().__init__(parent)
        self.db = db
        # 活跃运行线程映射: script_id -> ScriptRunner
        self._runners: Dict[int, ScriptRunner] = {}
        # 脚本运行状态字典: script_id -> "IDLE" | "RUNNING" | "STOPPED" | "FINISHED" | "ERROR"
        self._statuses: Dict[int, str] = {}

    def get_status(self, script_id: int) -> str:
        return self._statuses.get(script_id, "IDLE")

    def is_running(self, script_id: int) -> bool:
        return self.get_status(script_id) == "RUNNING"

    def get_all_scripts(self) -> List[Script]:
        return self.db.get_all_scripts()

    def search_scripts(self, keyword: str) -> List[Script]:
        return self.db.search_scripts(keyword)

    def add_script(self, file_path: str, name: str = "", group_name: str = "默认") -> Optional[Script]:
        abs_path = os.path.abspath(file_path)
        if not os.path.exists(abs_path) or not abs_path.endswith(".py"):
            return None

        # 检查是否已存在
        existing = self.db.get_script_by_path(abs_path)
        if existing:
            return existing

        script_name = name or os.path.basename(abs_path)
        script = Script(
            name=script_name,
            path=abs_path,
            python_path=sys.executable,
            args="",
            env_vars="",
            is_top=0,
            run_count=0,
            create_time=datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            last_run_time="",
            group_name=group_name
        )
        sid = self.db.add_script(script)
        script.id = sid
        self.scripts_reloaded.emit()
        return script

    def update_script(self, script: Script):
        self.db.update_script(script)
        self.scripts_reloaded.emit()

    def delete_script(self, script_id: int):
        # 若正在运行先终止
        if self.is_running(script_id):
            self.stop_script(script_id)
        self.db.delete_script(script_id)
        self._statuses.pop(script_id, None)
        self.scripts_reloaded.emit()

    def toggle_top(self, script_id: int) -> int:
        res = self.db.toggle_top(script_id)
        self.scripts_reloaded.emit()
        return res

    def run_script(self, script_id: int) -> bool:
        if self.is_running(script_id):
            return False

        script = self.db.get_script_by_id(script_id)
        if not script:
            return False

        runner = ScriptRunner(
            script_id=script.id,
            script_path=script.path,
            python_path=script.python_path or sys.executable,
            args=script.args,
            env_vars=script.env_vars,
            parent=self
        )

        runner.log_signal.connect(self._on_runner_log)
        runner.status_signal.connect(self._on_runner_status)
        runner.finished_signal.connect(self._on_runner_finished)

        self._runners[script_id] = runner
        self._statuses[script_id] = "RUNNING"
        self.script_status_changed.emit(script_id, "RUNNING")

        runner.start()
        return True

    def stop_script(self, script_id: int) -> bool:
        runner = self._runners.get(script_id)
        if runner and runner.isRunning():
            runner.stop()
            return True
        return False

    def run_batch(self, script_ids: List[int]):
        for sid in script_ids:
            self.run_script(sid)

    def _on_runner_log(self, script_id: int, level: str, msg: str):
        self.script_log_received.emit(script_id, level, msg)

    def _on_runner_status(self, script_id: int, status: str):
        self._statuses[script_id] = status
        self.script_status_changed.emit(script_id, status)

    def _on_runner_finished(self, script_id: int, return_code: int, duration: float):
        script = self.db.get_script_by_id(script_id)
        if script:
            # 记录运行次数与最后运行时间
            self.db.record_run(script_id)
            # 记录历史
            end_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            res_str = "SUCCESS" if return_code == 0 else f"FAILED({return_code})"
            if self.get_status(script_id) == "STOPPED":
                res_str = "STOPPED"
            self.db.add_history(RunHistory(
                script_id=script.id,
                script_name=script.name,
                script_path=script.path,
                start_time=script.last_run_time or end_time,
                end_time=end_time,
                duration_sec=round(duration, 2),
                exit_code=return_code,
                result=res_str
            ))

        if script_id in self._runners:
            del self._runners[script_id]

        self.script_finished.emit(script_id, return_code, duration)
        self.scripts_reloaded.emit()
