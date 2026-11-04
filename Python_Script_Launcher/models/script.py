from dataclasses import dataclass
from typing import Optional


@dataclass
class Script:
    id: Optional[int] = None
    name: str = ""
    path: str = ""
    python_path: str = ""
    args: str = ""
    env_vars: str = ""
    is_top: int = 0
    run_count: int = 0
    create_time: str = ""
    last_run_time: str = ""
    group_name: str = "默认"
    schedule_cron: str = ""


@dataclass
class RunHistory:
    id: Optional[int] = None
    script_id: Optional[int] = None
    script_name: str = ""
    script_path: str = ""
    start_time: str = ""
    end_time: str = ""
    duration_sec: float = 0.0
    exit_code: int = 0
    result: str = ""
