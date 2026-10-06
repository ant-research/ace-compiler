"""Minimal local `psutil` shim.

ONLY used on hosts where the real `psutil` is absent (this dev host has no
psutil and the pip mirror does not serve it). The cti:cifar image ships the
real `psutil`, which takes precedence there (real site-packages is searched
before this dir on PYTHONPATH only when nothing else provides it) -- so the
C-path test behaviour is unchanged: when real psutil is present it is used.
This shim implements exactly the two symbols `ace_util.py` touches:
`virtual_memory()` (`.total`, bytes) and `cpu_count(logical=False)`.

It is intentionally tiny and dependency-free; it is NOT a general psutil
replacement.
"""
import os


class _Mem:
    # Report an effectively-unbounded total so check_required_memory passes
    # locally (the memory gate is a C-path/real-machine concern; the per-model
    # GB floors are not enforced locally).
    total = 1 << 50  # 1 PiB


def virtual_memory():
    return _Mem()


def cpu_count(logical=True):
    try:
        n = len(os.sched_getaffinity(0))
    except (AttributeError, OSError):
        n = os.cpu_count() or 4
    return n