import logging
from typing import Dict, Optional
from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.triggers.cron import CronTrigger
from apscheduler.triggers.interval import IntervalTrigger


class TaskScheduler:
    _instance = None

    def __init__(self, script_manager=None):
        self.script_manager = script_manager
        self.scheduler = BackgroundScheduler(daemon=True)
        self.scheduler.start()
        self._jobs: Dict[int, str] = {}  # script_id -> job_id

    def set_script_manager(self, script_manager):
        self.script_manager = script_manager

    def schedule_script(self, script_id: int, cron_expr: str):
        """支持 cron 格式如 '0 8 * * *' 或 'interval:60' (每60秒)"""
        self.remove_job(script_id)
        if not cron_expr.strip():
            return

        def _job_func():
            if self.script_manager:
                self.script_manager.run_script(script_id)

        job_id = f"script_{script_id}"
        cron_expr = cron_expr.strip()

        try:
            if cron_expr.startswith("interval:"):
                secs = int(cron_expr.split(":")[1])
                self.scheduler.add_job(_job_func, IntervalTrigger(seconds=secs), id=job_id, replace_existing=True)
            else:
                parts = cron_expr.split()
                if len(parts) == 5:
                    minute, hour, dom, month, dow = parts
                    self.scheduler.add_job(
                        _job_func,
                        CronTrigger(minute=minute, hour=hour, day=dom, month=month, day_of_week=dow),
                        id=job_id,
                        replace_existing=True
                    )
            self._jobs[script_id] = job_id
        except Exception as e:
            logging.error(f"添加调度任务失败 ({cron_expr}): {e}")

    def remove_job(self, script_id: int):
        job_id = self._jobs.pop(script_id, None)
        if job_id:
            try:
                self.scheduler.remove_job(job_id)
            except Exception:
                pass

    def shutdown(self):
        if self.scheduler.running:
            self.scheduler.shutdown(wait=False)
