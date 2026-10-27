"""Runner for emitted `-CKKS2PY` modules under an rtlib-py provider.

Mirrors the C rtlib driver face `Run_main_graph()` / `Prepare_input()` /
`Handle_output()` (`rtlib/include/common/rt_api.h:31-44,97`): load the emitted
module, seed context + inputs, call `Main_graph()`, read outputs, check them
against the expected plaintext reference, and fail on any [hard]/[strict]
CKKS constraint violation.

Phase 2.0 change vs the Phase-1 `run_py_exec.py` hack: the emitted module now
starts with a **real** `from rtlib_py.<provider> import *`, so the runtime
names are bound by the import itself -- the driver no longer injects
`__all__` names into the exec namespace. The driver still seeds the provider's
internal stores (`_CTX`/`_INPUTS`) and reads its `_OUTPUTS`/`_VIOLATIONS`
directly off the SAME provider module instance the emitted module bound (they
are one cached module, so state is shared).

`run(py_file, spec, provider="rt_ref")` returns `(ok: bool, detail: str)`,
preserving the Phase-1 contract so `run_py_verify.py` / `run_py_exec.py`'s CLI
XFAIL classifier (which greps the detail) keep working unchanged.
"""

import math
import os
import traceback

from .provider import get_provider


def _slots_close(actual, expected, epsilon=1e-9):
    """Compare a cipher's plaintext slots to the expected reference.

    Returns ``(ok, why, max_abs)`` where ``max_abs`` is the largest per-slot
    ``|actual - expected|`` (0.0 when the outputs match exactly, ``inf`` when
    no per-slot diff can be formed -- None output, or actual shorter than the
    golden). ``epsilon`` is the pass threshold. Phase 2.5 makes it configurable
    via ``spec['epsilon']`` (default ``1e-9``); rt_ref is float64 plaintext-
    exact, so the green-gate workloads measure ``max_abs=0``.

    B3: the cipher carries the FULL ring (poly_degree/2 slots, the input zero-
    padded to the ring -- mirroring ant); the meaningful output is the leading
    ``len(expected)`` slots (the golden length M; ant's output's first M slots
    are the result, trailing slots are zero padding). So truncate ``actual`` to
    ``M`` before comparing (a no-op when the cipher already carries exactly M
    slots, e.g. rt_openfhe's _n_logical-truncated output, or a model whose input
    length == poly_degree/2). An actual SHORTER than the golden is a real
    shortfall (fail), not truncation."""
    if actual is None:
        return False, "no output captured", float("inf")
    a = [float(x) for x in actual]
    e = [float(x) for x in expected]
    if len(a) > len(e):
        a = a[:len(e)]
    elif len(a) < len(e):
        return False, f"slot count {len(a)} < expected {len(e)}", float("inf")
    max_abs = 0.0
    first_bad = -1
    for i, (x, y) in enumerate(zip(a, e)):
        d = abs(x - y)
        # A non-finite diff (a NaN/inf actual from a numerical blow-up) must NOT
        # silently pass: abs(nan - expected) is nan, and (nan > max_abs) /
        # (nan > epsilon) are both False (NaN-comparison semantics), so without
        # this guard first_bad stays -1 and max_abs stays 0.0 -- the blown-up
        # output would report PASS. Reject any non-finite per-slot diff as a
        # hard fail (returning max_abs=inf so the runner's `ma < tol` -> MISMATCH,
        # not PASS).
        if not math.isfinite(d):
            return (False, f"slot {i}: non-finite diff {d!r} (actual {x!r}, expected "
                    f"{y!r}; output blew up to NaN/inf)", float("inf"))
        if d > max_abs:
            max_abs = d
        if d > epsilon and first_bad < 0:
            first_bad = i
    ok = first_bad < 0
    why = "" if ok else f"slot {first_bad}: {a[first_bad]} != expected {e[first_bad]}"
    return ok, why, max_abs


def _seed_inputs(prov, spec, schemes=None):
    """Seed prov._INPUTS from spec['inputs']. Each input is packed into one or
    more ciphers at (input_level, scale_deg=1, is_cipher3=False).

    B3: zero-pad the input values to the provider's full ring slot count
    (poly_degree/2) before building the cipher -- mirroring ant, whose cipher
    ALWAYS carries N/2 slots (input in the leading slots, rest zero). Without
    this, a lowered BlockingRot `add_ciph(input, rotate_ciph(input, -k))` rotates
    on a short cipher and collapses to identity (k mod M == 0) -> e.g. gemm's 2x
    output. rt_openfhe already pads in its Cipher ctor (_N_SLOTS), so re-padding
    here is a no-op for it; this fixes rt_ref.

    E (2026-08-12, scheme-based): multi-cipher input packing uses the
    compiler-emitted DATA_SCHEME (get_encode_scheme: per-cipher desc with
    start/count/stride/pad), mirroring ant's Prepare_input / Pre_encode_scheme
    (rtlib/ant/ckks/src/rtlib.c:67-110, encoder.c:125-146). Each cipher k is
    packed as flat[start : start+count : stride] (+pre/post neighbor-pad when
    pad>0), then zero-padded to n_slots. The layout (e.g. 4 ciphers x 3136
    channel-aligned elems for resnet18-conv2d) is NOT a contiguous n_slots chunk.
    Fallback (no scheme, e.g. legacy -CKKS2PY modules): contiguous ceil(n/n_slots)
    flat chunk -> K=1 (n<=n_slots) is a no-op vs the original single-cipher path
    -> no regression on passing single-cipher cases."""

    input_level = prov._ctx_input_level()
    n_slots = prov._default_slot_count()

    def _pack_cipher(flat, d, hw):
        start, count = d.get("start", 0), d.get("count", 0)
        stride = d.get("stride", 1) or 1
        pad = d.get("pad", 0)
        slots = []
        if pad > 0 and hw:
            begin_channel = (start % hw) == 0
            for i in range(pad):
                idx = start - (pad - i) * stride
                slots.append(0.0 if (begin_channel or idx < 0 or idx >= len(flat))
                             else flat[idx])
        for j in range(count):
            idx = start + j * stride
            slots.append(flat[idx] if 0 <= idx < len(flat) else 0.0)
        if pad > 0 and hw:
            end_channel = ((start + count * stride) % hw) == 0
            for i in range(pad):
                idx = start + count * stride + i * stride
                slots.append(0.0 if (end_channel or idx < 0 or idx >= len(flat))
                             else flat[idx])
        if n_slots and len(slots) < n_slots:
            slots = slots + [0.0] * (n_slots - len(slots))
        return slots

    for name, vals in spec.get("inputs", {}).items():
        v = [float(x) for x in vals]
        sch = (schemes or {}).get(name)
        desc = sch.get("desc") if sch else None
        # Use the scheme only when it is non-degenerate (first desc count>0).
        # The compiler emits a placeholder scheme (count=0, stride=0, shape all
        # 0) for single-cipher / non-tiled inputs; packing per such a degenerate
        # desc would yield an all-zero input -> MISMATCH. Fall back to the
        # contiguous chunk path for those (K=1 no-op vs the original).
        if desc and desc[0].get("count", 0) > 0:
            shape = sch.get("shape") or []
            hw = shape[2] * shape[3] if len(shape) >= 4 else None
            for k, d in enumerate(desc):
                cv = _pack_cipher(v, d, hw)
                cipher = prov.Cipher(cv, input_level, 1, False)
                prov._seed_input(name, k, cipher)
        else:
            if not n_slots:
                chunks = [v]
            else:
                K = (len(v) + n_slots - 1) // n_slots  # ceil(n / n_slots)
                chunks = [v[k * n_slots:(k + 1) * n_slots] for k in range(max(K, 1))]
            for k, chunk in enumerate(chunks):
                cv = chunk
                if n_slots and len(cv) < n_slots:
                    cv = cv + [0.0] * (n_slots - len(cv))
                cipher = prov.Cipher(cv, input_level, 1, False)
                prov._seed_input(name, k, cipher)


def run(py_file, spec, provider="rt_ref"):
    """Execute the emitted module at `py_file` under the named rtlib-py
    provider and check its outputs against `spec['expected']`. Returns
    `(ok: bool, detail: str)`."""
    prov = get_provider(provider)

    prov._reset()
    prov.configure(strict=bool(spec.get("strict", True)))

    with open(py_file, "r") as f:
        src = f.read()

    # 1. exec the module. Its top-level `from rtlib_py.<provider> import *`
    #    binds the frozen public runtime API into the namespace; the module
    #    then defines its OWN Main_graph / get_context_params / ....
    ns = {"__name__": "under_test", "__file__": py_file}
    try:
        exec(compile(src, py_file, "exec"), ns)
    except Exception:
        return False, "module exec failed:\n" + traceback.format_exc()

    # 2. seed _CTX from the module's own get_context_params()
    gcp = ns.get("get_context_params")
    if gcp is None:
        return False, "module defines no get_context_params()"
    try:
        ctx = gcp()
    except Exception:
        return False, "get_context_params() failed:\n" + traceback.format_exc()
    prov._seed_ctx(ctx)

    # 2b. offline data-file descriptor (Phase 2.3): only emitted when -CKKS2PY:df=
    # was passed. load_data_entry reads the C binary side data file it names so
    # an offline encode pulls the weights from the file instead of an embedded
    # _cst_N list. None when the module defines no get_rt_data_info (no -df=).
    grtdi = ns.get("get_rt_data_info")
    try:
        info = grtdi() if grtdi is not None else None
    except Exception:
        return False, "get_rt_data_info() failed:\n" + traceback.format_exc()
    if hasattr(prov, "_seed_data_info"):
        prov._seed_data_info(info)

    # 2c. encode scheme (E 2026-08-12): compiler-emitted per-cipher input packing
    # layout (DATA_SCHEME). _seed_inputs uses desc[k] (start/count/stride/pad) to
    # pack each cipher mirroring ant Prepare_input; absent -> contiguous fallback.
    schemes = None
    gges = ns.get("get_encode_scheme")
    gic = ns.get("get_input_count")
    if gges is not None and gic is not None:
        try:
            schemes = {}
            for i in range(int(gic())):
                s = gges(i)
                schemes[s["name"]] = s
        except Exception:
            return False, "get_encode_scheme() failed:\n" + traceback.format_exc()

    # 3. seed inputs (needs _CTX.input_level)
    try:
        _seed_inputs(prov, spec, schemes)
    except Exception:
        return False, "input seeding failed:\n" + traceback.format_exc()

    # 4. run Main_graph() -- runtime calls resolve to the provider module
    mg = ns.get("Main_graph")
    if mg is None:
        return False, "module defines no Main_graph()"
    try:
        mg()
    except prov.FakeRtError as ex:
        return False, f"runtime constraint violation: {ex}"
    except Exception:
        return False, "Main_graph() failed:\n" + traceback.format_exc()

    # 5. compare outputs (lenient mode may have recorded [strict] warnings;
    #    those still fail the gate -- they are real findings to report).
    epsilon = float(spec.get("epsilon", 1e-9))  # Phase 2.5: configurable prec
    details = []
    expected = spec.get("expected", {})
    all_ok = True
    if not expected:
        # A spec without expected outputs cannot verify anything -- refuse the
        # vacuous pass (do not let an empty 'expected' report "outputs match").
        all_ok = False
        details.append("spec has no 'expected' outputs -- cannot verify "
                       "(vacuous pass refused)")
    max_abs = 0.0  # worst per-slot |actual - expected| across all outputs
    for name, exp_vals in expected.items():
        out = prov._OUTPUTS.get((name, 0))
        if out is None:
            # some emitters use a different output key; report what was captured
            all_ok = False
            details.append(f"expected output {name!r} not captured; "
                           f"got keys={list(prov._OUTPUTS)}")
            continue
        ok, why, ma = _slots_close(out.slots, exp_vals, epsilon)
        if ma > max_abs:
            max_abs = ma
        if not ok:
            all_ok = False
            details.append(f"output {name!r} mismatch: {why} "
                           f"(max_abs={ma:.1e}, eps={epsilon:.1e}, "
                           f"actual={out.slots})")

    if prov._VIOLATIONS:
        all_ok = False
        details.append(f"{len(prov._VIOLATIONS)} constraint violation(s):")
        details.extend(f"  {v}" for v in prov._VIOLATIONS)

    if all_ok:
        return True, (f"OK: outputs match (max_abs={max_abs:.1e}, "
                      f"eps={epsilon:.1e}) + no constraint violations"
                      + (f"; {'; '.join(details)}" if details else ""))
    return False, "; ".join(details) if details else "unknown failure"


__all__ = ["run", "_slots_close", "_seed_inputs"]