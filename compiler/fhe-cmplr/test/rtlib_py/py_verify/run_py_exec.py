"""Reusable executor for emitted `.py` modules (Phase 2.0 execution entry point).

The ckks IR -> Python backend emits a module that begins with a REAL runtime
import

    from rtlib_py.rt_ref import *          # default provider (-CKKS2PY:rt=...)

and defines `Main_graph`, `get_context_params`, `get_input_count`,
`get_output_count`. Phase 2.0 retires the Phase-1 hack (inject fake_rt names
into a namespace then exec): the runtime is now a real rtlib-py provider, so
this file is a thin shell over `rtlib_py.common.driver.run`, which loads the
module, seeds ctx/inputs, runs `Main_graph()`, reads outputs, and checks them
against the expected plaintext reference + asserts no CKKS level/scale
violations. See rtlib_py/common/driver.py for the runner (mirrors the C
`Run_main_graph`/`Prepare_input`/`Handle_output`).

Shared by:
  * `run_py_verify.py` (e2e: onnx -> fhe_cmplr -CKKS2PY -> py_compile -> run)
  * `test_fhepoly_py_allops.cxx` (`std::system("python3 run_py_exec.py ...")`)

Usage (CLI):  python3 run_py_exec.py <out.py> <spec.json|@workload> [--rt <module>]
  spec.json = {"inputs": {"<name>": [<float>...]},
               "expected": {"<name>": [<float>...]},
               "strict": true}                 # strict defaults to true

A workload name may be passed instead of a spec file as `<spec.json>` == the
literal `@<workload>` -- resolved via `workloads.py` (used by the e2e driver
when invoking the CLI form).

Phase 3 --rt: the rtlib-py provider to run under (`rt_ref` default,
`rt_openfhe` for real HE). When `--rt` is omitted the provider is AUTO-DERIVED
from the emitted module's first ``from rtlib_py.<name> import *`` line, so the
exec runs under the SAME provider the compiler bound. The C++ allops/
validate/bootstrap tests (which emit rt_ref) keep working with no change.
"""

import json
import os
import re
import sys

_HERE = os.path.dirname(os.path.abspath(__file__))
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

# Bootstrap rtlib-py (the -CKKS2PY pure-Python runtime home) onto sys.path. It
# lives at the repo root, three levels up from test/rtlib_py/py_verify/; override with
# RTLIB_PY_DIR. Required so `import rtlib_py` (and the emitted module's
# `from rtlib_py.rt_ref import *`) resolve when this entry point is run
# directly / via std::system (the harness sets no PYTHONPATH).
_RTLIB_PY_DIR = os.environ.get("RTLIB_PY_DIR") or os.path.abspath(
    os.path.join(_HERE, "..", "..", "..", "rtlib-py"))
if os.path.isdir(os.path.join(_RTLIB_PY_DIR, "rtlib_py")) and \
        _RTLIB_PY_DIR not in sys.path:
    sys.path.insert(0, _RTLIB_PY_DIR)

# Phase 3: locate the upstream openfhe-python binding (built .so) and put its
# dir on sys.path so `import openfhe` resolves. Override with
# OPENFHE_PY_BUILD_DIR; default /workspace/openfhe-python/build. Injected here
# (module level) so any importer -- run_py_verify, the C++ std::system
# allops/validate/bootstrap tests, the Phase-3 unit/bootstrap tests -- gets it
# before it `import openfhe` for a skip-77 guard. rt_openfhe._build_context
# ALSO self-injects, so the provider works when driven directly. No-op on a
# host without the binding (those tests skip via `import openfhe` failing).
_OPENFHE_BUILD_DIR = os.environ.get("OPENFHE_PY_BUILD_DIR") or \
    "/workspace/openfhe-python/build"
if os.path.isdir(_OPENFHE_BUILD_DIR) and _OPENFHE_BUILD_DIR not in sys.path:
    sys.path.insert(0, _OPENFHE_BUILD_DIR)

# Plan (rt_fhert): fhert is a PATH-based bazel source tree (NOT pip-installed),
# so `import fhert.openfhe` needs the fhert checkout on sys.path. Inject the
# fhert root here (override with FHERT_ROOT; default /workspace/fhert), mirroring
# the openfhe-python injection above. rt_fhert._build_context ALSO self-injects.
# No-op on a host without fhert (the rt_fhert tests skip via `import
# fhert.openfhe` failing). NB: rt_fhert runs under the FHERT VENV
# (/workspace/fhert/.venv/bin/python), which lacks openfhe-python -- so the two
# bindings never coexist in one process (a deliberate environment separation).
_FHERT_ROOT = os.environ.get("FHERT_ROOT") or "/workspace/fhert"
if os.path.isdir(_FHERT_ROOT) and _FHERT_ROOT not in sys.path:
    sys.path.insert(0, _FHERT_ROOT)

from rtlib_py.common.driver import run  # noqa: E402

__all__ = ["run"]

_RT_IMPORT_RE = re.compile(r"^\s*from rtlib_py\.(\w+) import \*", re.MULTILINE)


def _derive_provider(py_file):
    """Phase 3: auto-derive the rtlib-py provider from the emitted module's
    first ``from rtlib_py.<name> import *`` line, so an exec without `--rt`
    runs under the SAME provider the compiler bound (rt_ref default;
    rt_openfhe under `-CKKS2PY:rt=rt_openfhe`). Returns None if no such line (the
    caller falls back to the rt_ref default)."""
    try:
        with open(py_file, "r") as f:
            m = _RT_IMPORT_RE.search(f.read())
        return m.group(1) if m else None
    except OSError:
        return None


def _load_spec(spec_arg):
    """Resolve a spec argument: a JSON file path, or `@<workload>`."""
    if spec_arg.startswith("@"):
        name = spec_arg[1:]
        import workloads
        wl = workloads.WORKLOADS[name]
        return workloads.spec_from_workload(wl)
    with open(spec_arg, "r") as f:
        return json.load(f)


def _probe_binding(provider):
    """Pre-flight: confirm a real-HE provider's upstream binding is importable
    so a missing binding SKIPs (exit 77) instead of crashing the executor (the
    driver.run() path raises an uncaught ImportError from the lazy
    `_build_context` import). Mirrors run_py_verify.py's probe and the .sh
    `python -c "import openfhe"` SKIP gate. rt_ref (the default) has no upstream
    binding and needs no probe. Returns (True, "") when importable or not
    applicable, (False, msg) when the binding is absent -> caller emits 77."""
    if provider == "rt_openfhe":
        try:
            import openfhe  # noqa: F401  (OPENFHE_PY_BUILD_DIR injected at module top)
        except ImportError as ex:
            return False, (f"openfhe-python binding unavailable (import openfhe "
                           f"failed: {ex}; OPENFHE_PY_BUILD_DIR points at the "
                           f"built .so)")
    elif provider == "rt_fhert":
        try:
            import fhert.openfhe  # noqa: F401  (FHERT_ROOT injected at module top)
        except ImportError as ex:
            return False, (f"fhert.openfhe binding unavailable (import failed: "
                           f"{ex}; FHERT_ROOT points at the fhert checkout + use "
                           f"its venv)")
    return True, ""


def main(argv):
    import argparse
    ap = argparse.ArgumentParser(
        prog="run_py_exec.py",
        description="Execute an emitted -CKKS2PY module under an rtlib-py provider "
                    "and verify its outputs.")
    ap.add_argument("py_file", help="emitted .py module to exec")
    ap.add_argument("spec", help="spec JSON file or @<workload>")
    ap.add_argument("--rt", default=None,
                    help="rtlib-py provider (rt_ref | rt_openfhe | rt_fhert); "
                         "default auto-derives from the emitted import line, "
                         "else rt_ref. Phase 3.")
    args = ap.parse_args(argv)
    py_file = args.py_file
    if not os.path.isfile(py_file):
        sys.stderr.write(f"FAIL: no such file: {py_file}\n")
        return 2
    try:
        spec = _load_spec(args.spec)
    except Exception as ex:
        sys.stderr.write(f"FAIL: could not load spec {args.spec!r}: {ex}\n")
        return 2
    provider = args.rt or _derive_provider(py_file) or "rt_ref"
    # SKIP gate: a real-HE provider whose upstream binding is not importable
    # exits 77 (ctest SKIP_RETURN_CODE), NOT a hard ImportError crash. Per F5.3;
    # mirrors run_py_verify.py and the .sh-driven test wrappers.
    ok_probe, probe_msg = _probe_binding(provider)
    if not ok_probe:
        sys.stderr.write(f"SKIP: {py_file} -- {probe_msg}\n")
        return 77
    ok, detail = run(py_file, spec, provider=provider)
    (sys.stdout if ok else sys.stderr).write(
        f"{'PASS' if ok else 'FAIL'}: {py_file} -- {detail}\n")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))