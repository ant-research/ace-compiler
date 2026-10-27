"""Minimal psutil stub. ace_util.py only touches virtual_memory() (for .total)
and cpu_count(); the cpu_percent()/Process().memory_info() stubs are kept as a
no-op superset."""
def virtual_memory():
    class M: pass
    m = M(); m.total = m.available = m.percent = m.used = m.free = 0
    return m
def cpu_count(logical=True): return 1
def cpu_percent(): return 0.0
class Process:
    def __init__(self, *a, **k): pass
    def memory_info(self):
        class M: pass
        m = M(); m.rss = m.vms = 0
        return m
    def cpu_percent(self): return 0.0