import os
import sqlite3
import threading
from datetime import datetime
from typing import List, Optional
from models.script import Script, RunHistory

DEFAULT_DB_FILE = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "data", "launcher.db")


class Database:
    _instance = None
    _lock = threading.Lock()

    def __init__(self, db_path: Optional[str] = None):
        self.db_path = db_path or DEFAULT_DB_FILE
        os.makedirs(os.path.dirname(os.path.abspath(self.db_path)), exist_ok=True)
        self._local = threading.local()
        self.create_tables()

    @classmethod
    def get_instance(cls, db_path: Optional[str] = None):
        with cls._lock:
            if cls._instance is None:
                cls._instance = Database(db_path)
            return cls._instance

    @property
    def conn(self) -> sqlite3.Connection:
        if not hasattr(self._local, "conn") or self._local.conn is None:
            conn = sqlite3.connect(self.db_path)
            conn.row_factory = sqlite3.Row
            self._local.conn = conn
        return self._local.conn

    def create_tables(self):
        with self.conn:
            self.conn.execute("""
            CREATE TABLE IF NOT EXISTS scripts (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                path TEXT UNIQUE NOT NULL,
                python_path TEXT DEFAULT '',
                args TEXT DEFAULT '',
                env_vars TEXT DEFAULT '',
                is_top INTEGER DEFAULT 0,
                run_count INTEGER DEFAULT 0,
                create_time TEXT DEFAULT '',
                last_run_time TEXT DEFAULT '',
                group_name TEXT DEFAULT '默认',
                schedule_cron TEXT DEFAULT ''
            );
            """)

            self.conn.execute("""
            CREATE TABLE IF NOT EXISTS logs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                script_id INTEGER,
                level TEXT,
                content TEXT,
                create_time TEXT
            );
            """)

            self.conn.execute("""
            CREATE TABLE IF NOT EXISTS history (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                script_id INTEGER,
                script_name TEXT,
                script_path TEXT,
                start_time TEXT,
                end_time TEXT,
                duration_sec REAL DEFAULT 0.0,
                exit_code INTEGER DEFAULT 0,
                result TEXT
            );
            """)

            # 确保列存在的迁移处理
            cur = self.conn.execute("PRAGMA table_info(scripts)")
            existing_cols = {row["name"] for row in cur.fetchall()}
            if "args" not in existing_cols:
                self.conn.execute("ALTER TABLE scripts ADD COLUMN args TEXT DEFAULT ''")
            if "env_vars" not in existing_cols:
                self.conn.execute("ALTER TABLE scripts ADD COLUMN env_vars TEXT DEFAULT ''")
            if "group_name" not in existing_cols:
                self.conn.execute("ALTER TABLE scripts ADD COLUMN group_name TEXT DEFAULT '默认'")
            if "schedule_cron" not in existing_cols:
                self.conn.execute("ALTER TABLE scripts ADD COLUMN schedule_cron TEXT DEFAULT ''")

    # ---------------- 脚本 CRUD 操作 ----------------

    def add_script(self, script: Script) -> int:
        now_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        create_time = script.create_time or now_str
        with self.conn:
            cur = self.conn.execute("""
                INSERT INTO scripts (name, path, python_path, args, env_vars, is_top, run_count, create_time, last_run_time, group_name, schedule_cron)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            """, (
                script.name,
                script.path,
                script.python_path,
                script.args,
                script.env_vars,
                script.is_top,
                script.run_count,
                create_time,
                script.last_run_time,
                script.group_name or '默认',
                script.schedule_cron
            ))
            return cur.lastrowid

    def update_script(self, script: Script):
        with self.conn:
            self.conn.execute("""
                UPDATE scripts
                SET name = ?, path = ?, python_path = ?, args = ?, env_vars = ?, is_top = ?,
                    run_count = ?, last_run_time = ?, group_name = ?, schedule_cron = ?
                WHERE id = ?
            """, (
                script.name,
                script.path,
                script.python_path,
                script.args,
                script.env_vars,
                script.is_top,
                script.run_count,
                script.last_run_time,
                script.group_name or '默认',
                script.schedule_cron,
                script.id
            ))

    def delete_script(self, script_id: int):
        with self.conn:
            self.conn.execute("DELETE FROM scripts WHERE id = ?", (script_id,))
            self.conn.execute("DELETE FROM logs WHERE script_id = ?", (script_id,))
            self.conn.execute("DELETE FROM history WHERE script_id = ?", (script_id,))

    def get_script_by_id(self, script_id: int) -> Optional[Script]:
        cur = self.conn.execute("SELECT * FROM scripts WHERE id = ?", (script_id,))
        row = cur.fetchone()
        if row:
            return self._row_to_script(row)
        return None

    def get_script_by_path(self, path: str) -> Optional[Script]:
        cur = self.conn.execute("SELECT * FROM scripts WHERE path = ?", (path,))
        row = cur.fetchone()
        if row:
            return self._row_to_script(row)
        return None

    def get_all_scripts(self) -> List[Script]:
        cur = self.conn.execute("""
            SELECT * FROM scripts
            ORDER BY is_top DESC, last_run_time DESC, id DESC
        """)
        return [self._row_to_script(row) for row in cur.fetchall()]

    def search_scripts(self, keyword: str) -> List[Script]:
        kw = f"%{keyword}%"
        cur = self.conn.execute("""
            SELECT * FROM scripts
            WHERE name LIKE ? OR path LIKE ? OR group_name LIKE ?
            ORDER BY is_top DESC, last_run_time DESC, id DESC
        """, (kw, kw, kw))
        return [self._row_to_script(row) for row in cur.fetchall()]

    def toggle_top(self, script_id: int) -> int:
        cur = self.conn.execute("SELECT is_top FROM scripts WHERE id = ?", (script_id,))
        row = cur.fetchone()
        if not row:
            return 0
        new_top = 0 if row["is_top"] else 1
        with self.conn:
            self.conn.execute("UPDATE scripts SET is_top = ? WHERE id = ?", (new_top, script_id))
        return new_top

    def record_run(self, script_id: int):
        now_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with self.conn:
            self.conn.execute("""
                UPDATE scripts
                SET run_count = run_count + 1, last_run_time = ?
                WHERE id = ?
            """, (now_str, script_id))

    # ---------------- 历史记录操作 ----------------

    def add_history(self, history: RunHistory) -> int:
        with self.conn:
            cur = self.conn.execute("""
                INSERT INTO history (script_id, script_name, script_path, start_time, end_time, duration_sec, exit_code, result)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            """, (
                history.script_id,
                history.script_name,
                history.script_path,
                history.start_time,
                history.end_time,
                history.duration_sec,
                history.exit_code,
                history.result
            ))
            return cur.lastrowid

    def get_histories(self, limit: int = 200, script_id: Optional[int] = None) -> List[RunHistory]:
        if script_id:
            cur = self.conn.execute("""
                SELECT * FROM history
                WHERE script_id = ?
                ORDER BY id DESC LIMIT ?
            """, (script_id, limit))
        else:
            cur = self.conn.execute("""
                SELECT * FROM history
                ORDER BY id DESC LIMIT ?
            """, (limit,))
        return [self._row_to_history(row) for row in cur.fetchall()]

    def clear_history(self, script_id: Optional[int] = None):
        with self.conn:
            if script_id:
                self.conn.execute("DELETE FROM history WHERE script_id = ?", (script_id,))
            else:
                self.conn.execute("DELETE FROM history")

    # ---------------- 辅助方法 ----------------

    def _row_to_script(self, row: sqlite3.Row) -> Script:
        return Script(
            id=row["id"],
            name=row["name"],
            path=row["path"],
            python_path=row["python_path"] if "python_path" in row.keys() else "",
            args=row["args"] if "args" in row.keys() else "",
            env_vars=row["env_vars"] if "env_vars" in row.keys() else "",
            is_top=row["is_top"] if "is_top" in row.keys() else 0,
            run_count=row["run_count"] if "run_count" in row.keys() else 0,
            create_time=row["create_time"] if "create_time" in row.keys() else "",
            last_run_time=row["last_run_time"] if "last_run_time" in row.keys() else "",
            group_name=row["group_name"] if "group_name" in row.keys() else "默认",
            schedule_cron=row["schedule_cron"] if "schedule_cron" in row.keys() else ""
        )

    def _row_to_history(self, row: sqlite3.Row) -> RunHistory:
        return RunHistory(
            id=row["id"],
            script_id=row["script_id"],
            script_name=row["script_name"],
            script_path=row["script_path"],
            start_time=row["start_time"],
            end_time=row["end_time"],
            duration_sec=row["duration_sec"],
            exit_code=row["exit_code"],
            result=row["result"]
        )
