#!/usr/bin/env python3
"""round-3 sweep driver: read catalog CSV, run -P2P e2e per
(relpath, option, provider, df_state) tuple.

round-3 changes (vs old):
  * sys.path fix: test/py_verify -> test/rtlib_py/py_verify (old path removed).
  * df on/off: --df-states off,on (default runs both). df crashes recorded
    honestly (not silently skipped; KNOWN_GAPS section 5).
  * df_state dimension: done set + OUT_CSV schema include df_state column
    (old 4 rows without df_state treated as off).
  * argparse CLI: --csv/--out/--bin/--onnx-root/--providers/--df-states/--filter,
    enabling phase-1 regress-58 catalog and phase-2 140-table.
  * Legacy positional filter retained (python run_cti_params_sweep.py op/add smoke).

Tiered resources: rt_ref runs all (tol 1e-6); rt_openfhe uses tol 1e-5 +
a .py>2.1GB size-guard (RESOURCE_FAIL_UNROLL). NO_CTI_PARAM rows run bare.
Writes OUT_CSV incrementally (resumable, key=(relpath,option,provider,df_state))."""
import os, sys, csv, re, time, tempfile, shutil, argparse, signal


class _ExecTimeout(Exception):
    """rt_openfhe/rt_ref exec timeout (signal.alarm triggered)."""


def _alarm_handler(signum, frame):
    raise _ExecTimeout
# All paths env-overridable (no hardcoded machine paths). PY_VERIFY_DIR and
# RTLIB_PY_DIR default repo-relative; bin/onnx-root/csv/out require an env var
# or CLI flag (this is a parameterized methodology runner, not a one-machine
# script). OPENFHE_PY_BUILD_DIR is read from env and added to sys.path when set.
_HERE = os.path.dirname(os.path.abspath(__file__))
_REPO = os.path.abspath(os.path.join(_HERE, "..", ".."))
_PY_VERIFY_DIR = os.environ.get(
    "PY_VERIFY_DIR", os.path.join(_REPO, "test", "rtlib_py", "py_verify"))
_RTLIB_PY_DIR = os.environ.get("RTLIB_PY_DIR", os.path.join(_REPO, "rtlib-py"))
if _PY_VERIFY_DIR not in sys.path:
    sys.path.insert(0, _PY_VERIFY_DIR)
if _RTLIB_PY_DIR not in sys.path:
    sys.path.insert(0, _RTLIB_PY_DIR)
_OF_BUILD = os.environ.get("OPENFHE_PY_BUILD_DIR", "")
if _OF_BUILD and _OF_BUILD not in sys.path:
    sys.path.insert(0, _OF_BUILD)
from run_py_verify import _compile_one, _golden_from_onnx
from rtlib_py.common.driver import run

DEFAULT_BIN = os.environ.get("FHE_CMPLR", "")
DEFAULT_ONNX_ROOT = os.environ.get("ONNX_ROOT", "")
DEFAULT_PARAM_CSV = os.environ.get("CTI_PARAM_CSV", "")
DEFAULT_OUT_CSV = os.environ.get("E2E_OUT_CSV", "")
GIANT_PY = 2.1 * (1 << 30)
# rt_fhert is real CKKS (OpenFHE 1.4.2) over the SAME green-gate corpus as
# rt_openfhe, so it uses the SAME real-CKKS tolerance (1e-5) -- NOT a loosened
# threshold to hide errors, but parity with the established real-CKKS gate
# (rt_ref's 1e-6 is plaintext-exact and does not apply to real HE). A real-CKKS
# max_abs in (1e-6, 1e-5) is legitimate CKKS noise, not a correctness bug.
TOL = {"rt_ref": 1e-6, "rt_openfhe": 1e-5, "rt_fhert": 1e-5}

FIELDNAMES = ["method", "relpath", "option", "provider", "df_state", "param_type",
              "status", "max_abs", "pylines", "seconds", "detail"]

# Inlined from the OUT'd onnx-sweep helper in test/rtlib_py/py_verify (OUT per
# PR676 section 5; this runner is KEEP and must be self-contained in v2).
# Byte-verbatim feat bodies, location-only move -- section 6.8
# dangling-ref-to-OUT-file rewrite (the sweep helper is not maintained in v2).
def _graph_info(onnx_path):
    """Return (op_types:[str], in_shapes:{name:[int]}, out_shapes:{name:[int]},
    real_inputs:[name], n_nodes). Uses load_external_data=False so even the
    536MB giant parses light (weights skipped)."""
    import onnx
    m = onnx.load(onnx_path, load_external_data=False)
    g = m.graph
    init_names = {i.name for i in g.initializer}
    op_types = [n.op_type for n in g.node]

    def _shape(vi):
        tt = vi.type.tensor_type
        dims = []
        for d in tt.shape.dim:
            dv = d.dim_value if d.dim_value > 0 else 1  # symbolic -> 1 (batch)
            dims.append(int(dv))
        return dims

    in_shapes = {vi.name: _shape(vi) for vi in g.input}
    out_shapes = {vi.name: _shape(vi) for vi in g.output}
    real_inputs = [vi.name for vi in g.input if vi.name not in init_names]
    return op_types, in_shapes, out_shapes, real_inputs, len(g.node)


# ---- deterministic small-magnitude input fill (mirrors workloads._pat) -------
def _fill(n):
    """Deterministic values in ~[-0.5,0.5] (poly-approx safe, matches the
    existing golden_onnx PASS models)."""
    return [round(((i * 7 + 3) % 11 - 5) * 0.1, 4) for i in range(n)]


def _gen_inputs(real_inputs, in_shapes):
    """{name: [floats]} sized to each real input's total element count."""
    import math
    out = {}
    for name in real_inputs:
        shp = in_shapes.get(name) or [1]
        n = int(math.prod(shp)) if shp else 1
        out[name] = _fill(n)
    return out


def maxabs(d):
    m = re.search(r"max_abs=([0-9.eE+-]+)", d or "")
    return float(m.group(1)) if m else None


def classify(detail, ma, tol):
    if "[hard]" in (detail or ""):
        return "RUNTIME_CONSTRAINT"
    if ma is None:
        return "RESOURCE_FAIL_MEM" if "MemoryError" in (detail or "") else "EXEC_ERROR"
    if ma < tol:
        return "PASS"
    if ma < 1e-2:
        return "NOISY"
    return "MISMATCH"


def _parse_df_states(s):
    s = (s or "").lower()
    if "both" in s or s in ("", "all"):
        return ["off", "on"]
    return [x.strip() for x in s.split(",") if x.strip() in ("off", "on")] or ["off"]


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--csv", default=DEFAULT_PARAM_CSV,
                    help="catalog CSV (phase-1 regress-58 or phase-2 140-table)")
    ap.add_argument("--out", default=DEFAULT_OUT_CSV, help="results CSV (append/resume)")
    ap.add_argument("--bin", default=DEFAULT_BIN)
    ap.add_argument("--onnx-root", default=DEFAULT_ONNX_ROOT)
    ap.add_argument("--providers", default="rt_ref,rt_openfhe",
                    help="comma-separated rtlib-py provider (rt_ref|rt_openfhe|rt_fhert)")
    ap.add_argument("--df-states", default="off,on",
                    help="comma-separated df states (off|on|both); round-3 default off,on")
    ap.add_argument("--compile-timeout", type=int, default=180,
                    help="fhe_cmplr compile timeout in seconds (default 180)")
    ap.add_argument("--exec-timeout", type=int, default=300,
                    help="run_py_exec timeout in seconds (signal.alarm; default 300)")
    ap.add_argument("filter", nargs="?", default=None,
                    help="relpath substring filter (for smoke, e.g. op/add)")
    args = ap.parse_args(argv)

    providers = [p.strip() for p in args.providers.split(",") if p.strip()]
    df_states = _parse_df_states(args.df_states)
    signal.signal(signal.SIGALRM, _alarm_handler)  # exec timeout

    done = set()
    if os.path.exists(args.out) and os.path.getsize(args.out) > 0:
        for r in csv.DictReader(open(args.out)):
            # old rows without df_state/method column -> treat as off/""
            done.add((r.get("method", ""), r["relpath"], r["option"], r["provider"],
                      r.get("df_state", "off")))

    rows = list(csv.DictReader(open(args.csv)))
    if args.filter:
        rows = [r for r in rows if args.filter in r["relpath"]]
    # sort: if catalog has onnx_size column, sort by size ascending
    # (small/fast first); else old rule (cti-param first, then relpath)
    def _row_size(r):
        try:
            return int(r.get("onnx_size") or 0)
        except (TypeError, ValueError):
            return 0
    if any(r.get("onnx_size") for r in rows):
        rows.sort(key=_row_size)
    else:
        rows.sort(key=lambda r: (0 if r.get("param_type") == "cti-param" else 1, r["relpath"]))

    new_header = not os.path.exists(args.out) or os.path.getsize(args.out) == 0
    fout = open(args.out, "a", newline="")
    w = csv.DictWriter(fout, fieldnames=FIELDNAMES)
    if new_header:
        w.writeheader()

    try:
        for i, row in enumerate(rows):
            method = row.get("method", "")
            rel = row["relpath"]
            opt = row["option"]
            ptype = row.get("param_type", "")
            tokens = opt.split() if opt.strip() else []
            onnx = f"{args.onnx_root}/{rel}"
            if not os.path.exists(onnx):
                print(f"[SKIP] missing {rel}", flush=True)
                continue
            for prov in providers:
                for df_state in df_states:
                    df_bool = (df_state == "on")
                    key = (method, rel, opt, prov, df_state)
                    if key in done:
                        print(f"[resume-skip] {method} {rel} | {prov} | df={df_state}", flush=True)
                        continue
                    t0 = time.time(); status = "?"; ma = None; pylines = 0; detail = ""; wd = None
                    try:
                        wd = tempfile.mkdtemp(prefix=f"e2e_{prov}_df{df_state}_")
                        cok, py_path, cdetail = _compile_one(
                            args.bin, onnx, wd, df=df_bool, rt=prov,
                            extra_flags=tokens or None, timeout=args.compile_timeout)
                        if not cok:
                            status = "COMPILE_FAIL"; detail = str(cdetail)[:200]
                        else:
                            pylines = sum(1 for _ in open(py_path))
                            pysize = os.path.getsize(py_path)
                            if prov == "rt_openfhe" and pysize > GIANT_PY:
                                status = "RESOURCE_FAIL_UNROLL"
                                detail = f".py {pysize >> 20}MB > 2.1GB size-guard"
                            else:
                                signal.alarm(args.exec_timeout)
                                try:
                                    ops, in_sh, out_sh, real_in, n_nodes = _graph_info(onnx)
                                    inputs = _gen_inputs(real_in, in_sh)
                                    expected = _golden_from_onnx(onnx, inputs)
                                    spec = {"inputs": inputs, "expected": expected,
                                            "strict": True, "epsilon": TOL.get(prov, 1e-6)}
                                    rok, rdetail = run(py_path, spec, provider=prov)
                                finally:
                                    signal.alarm(0)
                                ma = maxabs(rdetail)
                                status = classify(rdetail, ma, TOL.get(prov, 1e-6))
                                detail = str(rdetail)[:200]
                    except _ExecTimeout:
                        signal.alarm(0)
                        status = "RESOURCE_FAIL_TIMEOUT"
                        detail = f"exec timeout after {args.exec_timeout}s"
                    except Exception as e:
                        # record df crashes (NameError etc.) as EXC, do not silently skip
                        status = "EXC"; detail = f"{type(e).__name__}: {str(e)[:150]}"
                    finally:
                        shutil.rmtree(wd, ignore_errors=True)  # bounded disk: clean after each model
                    dt = time.time() - t0
                    w.writerow(dict(method=method, relpath=rel, option=opt, provider=prov,
                                    df_state=df_state, param_type=ptype, status=status,
                                    max_abs="" if ma is None else f"{ma:.3e}",
                                    pylines=pylines, seconds=f"{dt:.1f}",
                                    detail=detail.replace("\n", " ")[:200]))
                    fout.flush()
                    print(f"[{i + 1}/{len(rows)}][{status}] {method} {rel} | {prov} | df={df_state} | "
                          f"opt={' '.join(tokens)[:32]} | ma={ma} | {dt:.1f}s", flush=True)
    finally:
        fout.close()
    print("DONE", flush=True)


if __name__ == "__main__":
    main()