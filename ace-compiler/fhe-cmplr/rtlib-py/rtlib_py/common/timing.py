"""rtlib-py timing -- mirror of rtlib-c `rtlib/include/common/rtlib_timing.h`.

The `-POLY:rtt` / `:rtt` (runtime timing) option makes fhe_cmplr emit
`CMPLR_TM_START(id, s)` / `CMPLR_TM_END(id, s)` calls around timed regions.
rtlib-c defines these as `Mark_rtm_start(id, s)` / `Mark_rtm_end(id, s)`
(rtlib_timing.h:184-185) which push/pop a `Rtm_stack` of `RTM_INFO` with
clock_gettime ns timestamps, `Append_rtlib_timing` records elapsed, and
`Report_rtlib_timing` dumps the report. This module mirrors that for the
Python path.

Phase 1 (no-op): CMPLR_TM_START/END are no-ops so `:rtt`-emitted modules
run past the NameError and the underlying computation can be verified for
correctness (timing does not affect computation).

Phase 2 (full mirror): Mark_rtm_start/end via time.perf_counter_ns +
Rtm_stack (nested) + Append_rtlib_timing + Report_rtlib_timing, mirroring
rtlib-c so Python-path timing is recorded and reportable.
"""

# ---- CMPLR_TM_START/END: mirror rtlib-c Mark_rtm_start/end (phase 2) ----
# Phase 1 was no-op (pass); phase 2 activates the full mirror via Mark_rtm_*.

def CMPLR_TM_START(id, s):  # noqa: N802 (mirror C macro name)
    """Mirror rtlib-c CMPLR_TM_START(id, s) -> Mark_rtm_start(id, s).
    Pushes a timing start mark onto the Rtm_stack (perf_counter_ns)."""
    Mark_rtm_start(id, s)


def CMPLR_TM_END(id, s):  # noqa: N802 (mirror C macro name)
    """Mirror rtlib-c CMPLR_TM_END(id, s) -> Mark_rtm_end(id, s).
    Pops the matching start, appends elapsed ns to the timing record."""
    Mark_rtm_end(id, s)


# ---- Phase 2 implementation (full mirror of rtlib_timing.h) ----

import time as _time

# Mirrors rtlib-c Rtm_stack (rtlib_timing.h:123): a stack of (event, id, t0).
_MAX_NEST = 64
_Rtm_stack = []      # list of [event, id, t0_ns]
_Rtm_record = []     # appended (event, id, elapsed_ns) by Mark_rtm_end


def Init_rtlib_timing():
    """Mirror rtlib-c Init_rtlib_timing (rtlib_timing.h:130). Reset state."""
    _Rtm_stack.clear()
    _Rtm_record.clear()


def Mark_rtm_start(event, id):
    """Mirror rtlib-c Mark_rtm_start (rtlib_timing.h:141): push start ns.

    Phase 2 activates this (CMPLR_TM_START -> Mark_rtm_start). Records the
    region start; nested regions push onto the stack.
    """
    _Rtm_stack.append([event, id, _time.perf_counter_ns()])


def Mark_rtm_end(event, id):
    """Mirror rtlib-c Mark_rtm_end (rtlib_timing.h:157): pop, append elapsed ns.

    Phase 2 activates this (CMPLR_TM_END -> Mark_rtm_end). Computes elapsed
    since the matching Mark_rtm_start and appends to _Rtm_record.
    """
    if not _Rtm_stack:
        return
    _event, _id, t0 = _Rtm_stack.pop()
    _Rtm_record.append((_event, _id, _time.perf_counter_ns() - t0))


def Append_rtlib_timing(event, id, nsec, cnt=0):
    """Mirror rtlib-c Append_rtlib_timing (rtlib_timing.h:136)."""
    _Rtm_record.append((event, id, nsec))


def Report_rtlib_timing():
    """Mirror rtlib-c Report_rtlib_timing (rtlib_timing.h:133). Return the
    accumulated timing records as a list of (event, id, elapsed_ns)."""
    return list(_Rtm_record)


__all__ = [
    "CMPLR_TM_START", "CMPLR_TM_END",
    "Init_rtlib_timing", "Mark_rtm_start", "Mark_rtm_end",
    "Append_rtlib_timing", "Report_rtlib_timing",
]
