import os
import shlex
import subprocess
import sys
import time
from datetime import datetime
from typing import Dict, Optional
from PySide6.QtCore import QThread, Signal


class ScriptRunner(QThread):
    # 信号定义
    log_signal = Signal(int, str, str)          # script_id, level, message
    status_signal = Signal(int, str)            # script_id, status ("RUNNING", "FINISHED", "ERROR", "STOPPED")
    finished_signal = Signal(int, int, float)   # script_id, returncode, duration_sec

    def __init__(self, script_id: int, script_path: str, python_path: str = "",
                 args: str = "", env_vars: str = "", parent=None):
        super().__init__(parent)
        self.script_id = script_id
        self.script_path = os.path.abspath(script_path)
        self.python_path = python_path.strip() or sys.executable
        self.args = args.strip()
        self.env_vars = env_vars.strip()
        self.process: Optional[subprocess.Popen] = None
        self._is_stopped = False

    def run(self):
        start_time = time.time()
        start_dt_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.status_signal.emit(self.script_id, "RUNNING")
        self.log_signal.emit(
            self.script_id,
            "INFO",
            f"====== 启动任务 [{os.path.basename(self.script_path)}] ({start_dt_str}) ======"
        )
        self.log_signal.emit(
            self.script_id,
            "INFO",
            f"解释器: {self.python_path} | 路径: {self.script_path} | 参数: {self.args or '(无)'}"
        )

        if not os.path.exists(self.script_path):
            self.log_signal.emit(self.script_id, "ERROR", f"错误: 脚本文件不存在: {self.script_path}")
            self.status_signal.emit(self.script_id, "ERROR")
            self.finished_signal.emit(self.script_id, -1, 0.0)
            return

        cmd = [self.python_path, "-u", self.script_path]
        if self.args:
            try:
                # 在 Windows/Linux 上安全切分参数
                cmd.extend(shlex.split(self.args, posix=(os.name != "nt")))
            except Exception as e:
                self.log_signal.emit(self.script_id, "WARNING", f"参数解析异常，按空格直接分割: {e}")
                cmd.extend(self.args.split())

        # 环境变量构建
        env = os.environ.copy()
        # 强制无缓冲
        env["PYTHONUNBUFFERED"] = "1"
        if self.env_vars:
            for line in self.env_vars.splitlines():
                line = line.strip()
                if line and "=" in line:
                    k, v = line.split("=", 1)
                    env[k.strip()] = v.strip()

        # 工作目录设置为脚本所在目录
        work_dir = os.path.dirname(self.script_path)

        try:
            self.process = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                encoding="utf-8",
                errors="replace",
                cwd=work_dir,
                env=env,
                bufsize=1
            )
        except Exception as e:
            self.log_signal.emit(self.script_id, "ERROR", f"进程创建失败: {e}")
            self.status_signal.emit(self.script_id, "ERROR")
            self.finished_signal.emit(self.script_id, -1, time.time() - start_time)
            return

        # 实时逐行读取输出
        try:
            if self.process.stdout:
                for line in iter(self.process.stdout.readline, ""):
                    if self._is_stopped:
                        break
                    line_clean = line.rstrip("\r\n")
                    if line_clean:
                        # 识别级别
                        lvl = "INFO"
                        upper = line_clean.upper()
                        if "ERROR" in upper or "TRACEBACK" in upper or "EXCEPTION" in upper:
                            lvl = "ERROR"
                        elif "WARN" in upper:
                            lvl = "WARNING"
                        self.log_signal.emit(self.script_id, lvl, line_clean)
                self.process.stdout.close()
        except Exception as e:
            self.log_signal.emit(self.script_id, "ERROR", f"读取输出异常: {e}")

        # 等待进程退出
        try:
            return_code = self.process.wait()
        except Exception:
            return_code = -1

        duration = time.time() - start_time
        end_dt_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        if self._is_stopped:
            self.log_signal.emit(
                self.script_id,
                "WARNING",
                f"====== 任务已被用户强制终止 (耗时 {duration:.2f}s) ======"
            )
            self.status_signal.emit(self.script_id, "STOPPED")
        elif return_code == 0:
            self.log_signal.emit(
                self.script_id,
                "INFO",
                f"====== 任务执行完成 [退出码: 0] (耗时 {duration:.2f}s, {end_dt_str}) ======"
            )
            self.status_signal.emit(self.script_id, "FINISHED")
        else:
            self.log_signal.emit(
                self.script_id,
                "ERROR",
                f"====== 任务异常退出 [退出码: {return_code}] (耗时 {duration:.2f}s, {end_dt_str}) ======"
            )
            self.status_signal.emit(self.script_id, "ERROR")

        self.finished_signal.emit(self.script_id, return_code, duration)

    def stop(self):
        """强制终止进程"""
        self._is_stopped = True
        if self.process and self.process.poll() is None:
            try:
                self.process.terminate()
                # 稍等片刻如果还在运行则 kill
                try:
                    self.process.wait(timeout=1.0)
                except subprocess.TimeoutExpired:
                    self.process.kill()
            except Exception:
                pass
