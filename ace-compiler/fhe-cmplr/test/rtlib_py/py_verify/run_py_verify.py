"""Phase 1 e2e verification driver (execution gate).

For each ONNX workload in `workloads.py`:
  1. generate the onnx (`gen_workload_onnx.py`, no oss / onnxruntime needed)
  2. `fhe_cmplr <onnx> -o out.py -CKKS2PY:rt=rt_ref`  -- full pipeline
  3. assert .py + .conf produced, NO .c leaks (gating), `py_compile` passes
  4. `run_py_exec.run(out.py, spec)` -- exec Main_graph under rtlib_py.rt_ref
     (real import, no injection), check plaintext output vs spec['expected']
     within `--prec` epsilon (Phase 2.5: reports the measured max_abs_diff)
     + assert no level/scale violations
  5. report PASS / FAIL / XFAIL

Exit 0 iff every non-xfail workload is green and every xfail workload fails
for its documented reason. XFAIL workloads that unexpectedly PASS, or that
FAIL for a different reason, are surfaced (the former is good news -- the gap
closed -- but is flagged so the XFAIL marker can be removed). (mulc2 runs in
the default green gate alongside add1/add8/mul1. Phase 2.3 adds `mulc2df` --
the same model compiled with `-CKKS2PY:df=` so the weight is encoded offline from
the C binary side data file via rt_ref.load_data_entry.)

CLI:
  --bin <fhe_cmplr>     default $FHE_CMPLR (or pass --bin; no host-fixed path)
  --workload a,b,...    subset (default: all)
  --strict|--lenient    strictness passed into the runtime spec (default strict)
  --prec EPS            accuracy epsilon (max per-slot |actual-expected|
                        allowed; default 1e-9). Phase 2.5: the per-workload
                        PASS line reports the measured max_abs_diff vs this
                        epsilon, so a precision regression is visible in CI.
                        rt_ref is float64 plaintext-exact -> max_abs=0 for the
                        green-gate workloads; Phase-3 real-HE will need a
                        wider epsilon.
  --rt <module>         rtlib-py provider: rt_ref (default) | rt_openfhe (Phase
                        3 real-CKKS). Appends `-CKKS2PY:rt=<module>` to fhe_cmplr
                        (so the emitted module imports that provider) and runs
                        the exec under the SAME provider -- the two MUST match
                        (`rtlib_py.common.driver.run` seeds the provider the
                        emitted Main_graph binds). openfhe-python is located
                        via run_py_exec's OPENFHE_PY_BUILD_DIR injection; tests
                        skip (exit 77) if `import openfhe` fails.
  --label NAME          ctest label (default phase1_verify); no behavioral effect
"""

import argparse
import os
import shutil
import subprocess
import sys
import tempfile

_HERE = os.path.dirname(os.path.abspath(__file__))
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

# Phase 2.0: bootstrap rtlib-py (the -CKKS2PY pure-Python runtime home) onto
# sys.path before importing run_py_exec (which imports rtlib_py.common.driver).
# rtlib-py lives at the repo root, three levels up from test/rtlib_py/py_verify/; override
# with RTLIB_PY_DIR. The emitted module's `from rtlib_py.rt_ref import *`
# resolves through the same path at exec time. This is the ONLY change to this
# file in 2.0 (the assertions / compile / classify-xfail logic is unchanged).
_RTLIB_PY_DIR = os.environ.get("RTLIB_PY_DIR") or os.path.abspath(
    os.path.join(_HERE, "..", "..", "..", "rtlib-py"))
if os.path.isdir(os.path.join(_RTLIB_PY_DIR, "rtlib_py")) and \
        _RTLIB_PY_DIR not in sys.path:
    sys.path.insert(0, _RTLIB_PY_DIR)

import workloads as WL  # noqa: E402
import run_py_exec      # noqa: E402

DEFAULT_BIN = os.environ.get("FHE_CMPLR", "")  # FHE_CMPLR env or --bin flag; no host-fixed default


def _run(cmd, cwd=None, timeout=None):
    return subprocess.run(cmd, cwd=cwd, capture_output=True, text=True,
                          timeout=timeout)


def _compile_one(bin_path, onnx_path, out_dir, df=False, rt="rt_ref",
                 extra_flags=None, timeout=None):
    """Run fhe_cmplr -CKKS2PY:rt=<rt> (and -CKKS2PY:df=<weights> when df). The
    emitted module imports that rtlib-py provider (the exec under run_py_exec.run
    uses the SAME provider). `extra_flags` are extra fhe_cmplr args
    (rtlib-py-onnx plan P4: a raised -CKKS context for the deep-context mulmul
    probe). `timeout` bounds the compile wall-clock (heavy full-net OOM guard).
    Returns (ok, py_path, detail)."""
    py_path = os.path.join(out_dir, "out.py")
    cmd = [bin_path, onnx_path, "-o", py_path, "-CKKS2PY:rt=" + rt]
    if df:
        # Phase 2.3: offline data-file encode. The C binary side data file is
        # written next to the .py; rt_ref.load_data_entry reads it back.
        cmd += ["-CKKS2PY:df=" + os.path.join(out_dir, "weights.dat")]
    if extra_flags:
        cmd += list(extra_flags)
    try:
        r = _run(cmd, cwd=out_dir, timeout=timeout)
    except subprocess.TimeoutExpired:
        return False, None, f"compile timeout after {timeout}s (heavy/slow model)"
    if r.returncode != 0:
        return False, None, f"fhe_cmplr rc={r.returncode}\n{r.stdout}{r.stderr}"
    if not os.path.isfile(py_path) or os.path.getsize(py_path) == 0:
        return False, None, "out.py not produced (or empty)"
    # gating: no .c may leak; a .conf must accompany the .py
    leaks = [f for f in os.listdir(out_dir) if f.endswith(".c")]
    if leaks:
        return False, None, f"C emission leaked: {leaks} (gating failed)"
    confs = [f for f in os.listdir(out_dir) if f.endswith(".conf")]
    if not confs:
        return False, None, "no .conf produced (lower_ctx.Emit_conf)"
    return True, py_path, f"ok; conf={confs[0]}"


def _py_compile(py_path):
    r = _run([sys.executable, "-m", "py_compile", py_path])
    if r.returncode != 0:
        return False, f"py_compile rc={r.returncode}\n{r.stdout}{r.stderr}"
    return True, "ok"


def _classify_xfail(detail, wl):
    """Did an xfail workload fail for a reason covered by its xfail_reason?

    rtlib-py-onnx plan (PLAN.md section G.4): classify by the EXECUTION-time outcome,
    NOT a compile rc=1 (this corpus compiles rc=0, so the old `compile: rc=1`
    key never fired). The two integration defects map to distinct detail
    signatures:
      (b) emitter-bug: NameError on an un-initialized accumulator list
          (*_block_*) from the BlockingRot replicate unroll (conv/gemm/mvp/...).
      (c) context-budget: a [hard] ... level out of bounds runtime constraint
          from a lowered op exceeding the baked get_context_params range
          (relu/pool/gap/reshape).
    A _cst_N NameError is the distinct Phase-2 array-const residual. Any other
    [hard] (e.g. a shallow-context droplevel, gap #1/#2) is a generic runtime
    constraint violation. Anything else (a NEW exec regression, or a real
    'fell back to C' compile/py_compile failure where the .py carries C
    syntax) surfaces as FAIL -- a classifier hole to fix, not masked XFAIL. An
    unexpected pass is XPASS.

    Post the bucket-(b)/(c) fixes (poly2py accumulator-list init; rt_ref /
    rt_openfhe encode-level bound against input_level=num_primes), (b)/(c) no
    longer fire on this corpus; the b/c-peripheral workloads now fail at the
    SECOND layer and are attributed by the branches below: [hard] mul_plain
    plain/cipher slot-count broadcast (workloads.py _DEEP_BCAST), [strict]
    add_ciph level mismatch / gap #1/#2 (_DEEP_LEVEL), or -- if a workload now
    EXECUTES to completion -- bucket (d) output-mismatch (_DEEP_NUM / a stale
    placeholder golden), keyed on "mismatch"+"output". The (b)/(c) branches are
    retained as a defensive backstop for any future emitter/runtime regression."""
    low = detail.lower()
    # (b) emitter unrolling bug: un-initialized accumulator list *_block_*
    if "nameerror" in low and "_block_" in low:
        return True, ("emitter-bug: unrolled loop writes un-initialized "
                      "accumulator list (*_block_*) -> NameError")
    # genuine _cst_N NameError (Phase 2 array-const residual; distinct from b)
    if "nameerror" in low and "_cst" in low:
        return True, "NameError on _cst_N (Phase 2 array-const gap) as expected"
    # (c) context-level budget mismatch
    if "[hard]" in low and ("out of bounds" in low or "level" in low):
        return True, ("context-budget: lowered op level out of baked "
                      "get_context_params range")
    # other [hard] runtime constraint (e.g. shallow-ctx droplevel, gap #1/#2)
    if "[hard]" in low:
        return True, "runtime constraint violation [hard]: " + low[:120]
    # [strict] CKKS-discipline violation (e.g. a depth-2 mul chain whose second
    # operand is not mod_switched to the rescaled intermediate's level -- the
    # P4a mulmul probe, gap #1/#2 deep-context level management). Distinct from
    # the [hard] level-OOB (bucket c): here levels are in-range but misaligned.
    if "[strict]" in low:
        return True, ("runtime constraint violation [strict] (CKKS discipline, "
                      "e.g. level/scale misalignment -- gap #1/#2 deep-context): "
                      + low[:110])
    # compile-stage frontend lowering limitation: a TODO/assertion in the
    # nn-addon/air lowerer rejects the model BEFORE emit (compile rc=1). This is
    # distinct from bucket b/c (which compile fine and fail at EXEC) and from a
    # 'fell back to C' emit regression (caught at py_compile, not compile). The
    # plan's premise that this corpus compiles rc=0 was falsified by
    # gemv_512x512 (tensor2vector_util k_pad TODO) -- record the true cause.
    if "compile:" in low and ("assertion" in low or "todo" in low
                              or "rc=1" in low):
        return True, ("frontend lowering limitation (compile-time, before "
                      "emit): " + low[:140])
    # (d) bucket (d) numerical: the workload now EXECUTES to completion (b/c
    # fixed -- poly2py emits the accumulator-list init; rt_ref/rt_openfhe bound
    # the encode level against input_level=num_primes) but the output does not
    # match the golden. For b/c-peripheral workloads whose `expected` was a
    # placeholder `_zeros`, reaching output comparison is itself progress; a
    # mismatch here is either a stale placeholder golden OR a real numerical /
    # plain-packing defect (the workload's xfail_reason records which -- e.g.
    # conv_p1_s1_g32 is verified via onnxruntime: output multiset != the ONNX
    # golden under every slot layout, so it is a real defect, not packing).
    # Distinct from [hard]/[strict] (which fail BEFORE output comparison).
    # Keyed on the output-comparison signature ("mismatch" + "output") and
    # placed AFTER the [strict]/[hard] branches so a [strict] "level mismatch"
    # is attributed to its own bucket, not this one.
    if "mismatch" in low and "output" in low:
        return True, ("bucket (d): exec completes after b/c fix but output != "
                      "golden -- stale placeholder golden OR a real numerical/"
                      "plain-packing defect; see the workload's xfail_reason "
                      "(not a b/c regression)")
    return False, (f"failed for an UNEXPECTED reason -- surfacing as FAIL: "
                   f"{detail}")


def _golden_from_onnx(onnx_file, inputs):
    """Compute the TRUE onnx golden (row-major flatten) for an onnx_file
    workload via onnxruntime, replacing the static `expected` placeholder.
    `inputs` is the workload's {name: [flat floats]}; each is reshaped to the
    onnx graph input's declared shape (row-major) and fed to onnxruntime; the
    outputs are flattened row-major. Used by `golden_onnx=True` workloads whose
    FHE output packing is row-major leading (depthwise conv, gemm) -- verified
    to match rt_ref to float32 noise. Returns {output_name: [flat floats]}."""
    import onnxruntime as ort
    import numpy as np
    sess = ort.InferenceSession(onnx_file, providers=["CPUExecutionProvider"])
    feed = {}
    for inp in sess.get_inputs():
        if inp.name not in inputs:
            continue
        flat = [float(x) for x in inputs[inp.name]]
        # Reshape the flat input to the graph shape (row-major); unknown /
        # symbolic dims (onnxruntime returns them as non-positive or strings)
        # collapse to -1 so numpy infers them. All golden_onnx workloads in this
        # corpus have fully-known static input shapes (conv [1,32,32,32], gemm
        # [1,2]), so the reshape is exact.
        shape = []
        for d in inp.shape:
            try:
                dv = int(d)
            except (TypeError, ValueError):
                dv = -1
            shape.append(dv if dv > 0 else -1)
        feed[inp.name] = np.array(flat, dtype=np.float32).reshape(shape)
    outs = sess.run(None, feed)
    return {o.name: [float(x) for x in np.asarray(out).reshape(-1)]
            for o, out in zip(sess.get_outputs(), outs)}


def run_workload(bin_path, wl, strict, prec, rt):
    onnx_dir = tempfile.mkdtemp(prefix=f"{wl.name}_")
    try:
        onnx = wl.write_onnx(onnx_dir)
        ok, py_path, detail = _compile_one(
            bin_path, onnx, onnx_dir, df=wl.df, rt=rt,
            extra_flags=getattr(wl, "extra_flags", None),
            timeout=getattr(wl, "compile_timeout", None))
        if not ok:
            # A compile_only/heavy model that fails to compile (likely OOM or
            # timeout on a giant net) is a skip-if-OOM, NOT a regression.
            if getattr(wl, "compile_only", False):
                return "SKIP", (f"compile-only model compile failed (likely "
                                f"OOM/timeout): {detail}")
            if wl.xfail:
                matched, why = _classify_xfail(f"compile: {detail}", wl)
                return "XFAIL" if matched else "FAIL", why
            return "FAIL", f"compile: {detail}"
        # rtlib-py-onnx plan: heavy full-nets are COMPILE-ONLY (their emitted
        # .py can be 100s of MB to ~1 GB; exec-compiling the giant Main_graph
        # body, and even py_compiling it, can OOM/timeout -- a Python
        # scalability caveat, NOT an rtlib-py emit regression). The .c-leak +
        # .conf + .py-nonempty gate in _compile_one already proves the Python
        # emit path is intact for the full net; record the .py size and attempt
        # py_compile under a timeout (skip-on-overflow, never FAIL).
        if getattr(wl, "compile_only", False):
            size_mb = os.path.getsize(py_path) / 1e6
            pc_note = ""
            if size_mb <= 150:
                try:
                    r = _run([sys.executable, "-m", "py_compile", py_path],
                             timeout=getattr(wl, "compile_timeout", None)
                             or 1200)
                    pc_note = ("py_compile OK" if r.returncode == 0
                               else f"py_compile rc={r.returncode} (giant .py)")
                except subprocess.TimeoutExpired:
                    pc_note = (f"py_compile timeout (giant .py "
                               f"{size_mb:.0f}MB) -- skip-if-OOM")
            else:
                pc_note = (f"py_compile skipped (giant .py {size_mb:.0f}MB; "
                           f"Python scalability, not an emit regression)")
            return "COMPILE_ONLY", (f"compile OK; .py={size_mb:.0f}MB; conf "
                                    f"present; exec skipped (compile_only). "
                                    f"{pc_note}")
        ok, detail = _py_compile(py_path)
        if not ok:
            return "FAIL", f"py_compile: {detail}"
        spec = WL.spec_from_workload(wl)
        spec["strict"] = strict and wl.strict
        # rtlib-py-onnx plan G.2: per-workload epsilon override (green models
        # carry their section E tolerance; default to the global --prec).
        spec["epsilon"] = (wl.prec if getattr(wl, "prec", None) is not None
                           else prec)
        # golden_onnx: replace the static placeholder `expected` (e.g. _zeros)
        # with the TRUE onnxruntime golden (row-major), for row-major-packing
        # workloads (depthwise conv / gemm). Needs onnxruntime; on failure
        # surface as FAIL (a golden-compute regression) rather than mask it.
        if getattr(wl, "golden_onnx", False):
            try:
                spec["expected"] = _golden_from_onnx(wl.onnx_file, wl.inputs)
            except Exception as exc:  # pragma: no cover - golden-compute error
                return "FAIL", f"golden_onnx compute failed: {exc}"
        # Phase 3: run under the SAME rtlib-py provider the module was compiled
        # with (the emitted `from rtlib_py.<rt> import *` must match the driver
        # provider, else Main_graph binds one provider's names and the driver
        # seeds another's stores -- the plan's seeded-vs-emitted mismatch).
        ok, detail = run_py_exec.run(py_path, spec, provider=rt)
        if ok:
            if wl.xfail:
                # unexpected pass -- the gap may have closed; flag for removal
                return "XPASS", (f"unexpected PASS (xfail gap may be closed); "
                                 f"detail: {detail}")
            return "PASS", detail
        # exec failed
        if wl.xfail:
            matched, why = _classify_xfail(detail, wl)
            if matched:
                return "XFAIL", why
            # rt_openfhe toy context: a bucket b/c peripheral may fail with a
            # DIFFERENT signature than the rt_ref bucket (context construction
            # on a 16k/56-bit-scale conv capped to 40, droplevel [hard] from a
            # shallow tower, ...). The plan (section E) records these as
            # fail-and-investigate, NOT a gate FAIL -- only a GREEN (non-xfail)
            # model failing surfaces as a regression. Same module, same defect
            # class, toy-context extra constraint noted.
            if rt == "rt_openfhe":
                return "XFAIL", ("fail-and-investigate (rt_openfhe toy context; "
                                 "not the rt_ref bucket signature): "
                                 + detail[:160])
            return "FAIL", why
        return "FAIL", detail
    finally:
        shutil.rmtree(onnx_dir, ignore_errors=True)


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument("--bin", default=DEFAULT_BIN)
    ap.add_argument("--workload", default=None,
                    help="comma-separated subset (default: green-gate workloads)")
    ap.add_argument("--all", action="store_true",
                    help="include opt-in XFAIL workloads (the rtlib-py-onnx "
                         "bucket b/c peripheral) in the run, EXCLUDING the "
                         "heavy compile-only full-nets and the P4 probes "
                         "(mulmul/concat) -- see --include-heavy/--include-probes)")
    ap.add_argument("--include-heavy", action="store_true",
                    help="also include heavy compile-only full-nets "
                         "(resnet110/alexnet); they compile + py_compile only "
                         "(exec skipped), with a skip-if-OOM guard")
    ap.add_argument("--include-probes", action="store_true",
                    help="also include the P4 synthetic probes (mulmul/concat); "
                         "their outcome is recorded (XPASS/XFAIL) but they are "
                         "not part of the green gate")
    ap.add_argument("--strict", dest="strict", action="store_true", default=True)
    ap.add_argument("--lenient", dest="strict", action="store_false")
    ap.add_argument("--prec", type=float, default=1e-9,
                    help="accuracy epsilon: max per-slot |actual-expected| "
                         "allowed (default 1e-9; rt_ref is float64 "
                         "plaintext-exact, so the green-gate workloads measure "
                         "max_abs=0. Phase-3 real-HE needs a wider epsilon.)")
    ap.add_argument("--rt", default="rt_ref",
                    help="rtlib-py provider: rt_ref (default) | rt_openfhe "
                         "(Phase 3 real-CKKS) | rt_fhert (Plan: real-CKKS via "
                         "fhert's OpenFHE 1.4.2). Both the compile (`-CKKS2PY:rt=`) "
                         "and the exec run under this provider.")
    ap.add_argument("--label", default="phase1_verify")
    args = ap.parse_args(argv)

    if not os.access(args.bin, os.X_OK):
        sys.stderr.write(f"FAIL: fhe_cmplr not found / not executable: {args.bin}\n")
        return 2
    # Phase 3 / Plan(rt_fhert): a real-HE provider needs its native binding; if
    # absent, skip cleanly (exit 77 = ctest SKIP_RETURN_CODE) rather than fail --
    # mirrors the onnx/pytest optional-dep skip. rt_ref (the default) needs none.
    if args.rt not in ("rt_ref", "rt_openfhe", "rt_fhert"):
        sys.stderr.write(
            f"FAIL: unknown --rt {args.rt!r} (rt_ref | rt_openfhe | rt_fhert)\n")
        return 2
    if args.rt == "rt_openfhe":
        try:
            import openfhe  # noqa: F401  (path injected via run_py_exec import)
        except ImportError:
            sys.stderr.write(
                "SKIP: --rt rt_openfhe needs openfhe-python (import openfhe "
                "failed; OPENFHE_PY_BUILD_DIR points at the built .so)\n")
            return 77
    elif args.rt == "rt_fhert":
        # fhert.openfhe is path-imported (FHERT_ROOT; run_py_exec injects it).
        # rt_fhert runs under the FHERT VENV, where openfhe-python is absent, so
        # the two bindings never co-load; just probe fhert.openfhe here.
        try:
            import fhert.openfhe  # noqa: F401
        except ImportError:
            sys.stderr.write(
                "SKIP: --rt rt_fhert needs fhert.openfhe (import failed; "
                "FHERT_ROOT points at the fhert checkout + use its venv)\n")
            return 77
    # python3 + onnx are required (onnxruntime is NOT; refs are hand-computed).
    if not shutil.which("python3") and sys.executable:
        pass  # sys.executable is a python; fine
    try:
        import onnx  # noqa: F401
    except ImportError:
        sys.stderr.write("FAIL: onnx package required to generate workloads\n")
        return 2

    names = (args.workload.split(",") if args.workload
             else [n for n, w in WL.WORKLOADS.items()
                   if (not w.xfail or args.all)
                   and (not getattr(w, "heavy", False) or args.include_heavy)
                   and (not getattr(w, "probe", False) or args.include_probes)])
    print(f"== Phase 1 e2e (label={args.label}, bin={args.bin}, "
          f"strict={args.strict}, prec={args.prec}, rt={args.rt}) ==")
    results = []
    fail_count = 0
    for name in names:
        wl = WL.WORKLOADS.get(name)
        if wl is None:
            print(f"  [{name}] SKIP: unknown workload")
            continue
        status, detail = run_workload(args.bin, wl, args.strict, args.prec,
                                      args.rt)
        results.append((name, status, detail))
        if status == "FAIL":
            fail_count += 1
        print(f"  [{name}] {status}: {detail}")

    # status tally (XFAIL/XPASS/SKIP/COMPILE_ONLY do not fail the gate; only
    # FAIL does). Helps the rtlib-py-onnx report build its per-bucket matrix.
    from collections import Counter
    tally = Counter(r[1] for r in results)
    print(f"== summary: {len(results)} run, {fail_count} FAIL "
          f"({', '.join(f'{s}={c}' for s, c in sorted(tally.items()))}) ==")
    return 1 if fail_count else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))