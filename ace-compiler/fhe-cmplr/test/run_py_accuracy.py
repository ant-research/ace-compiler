#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Python-path cifar accuracy driver -- the rtlib-py mirror of the C-path
``resnet_cifar.main.inc`` flow.

The C path compiles an onnx to a C executable whose ``main`` (in
``resnet_cifar.main.inc``) loops cifar images: cifar_reader.Load -> Prepare_input
-> Run_main_graph -> Handle_output -> argmax vs label -> top-1 accuracy, printing
``[RESULT] infer N images, pass X frac, fail Y frac.``

For the python path (``-CKKS2PY:rt=<rt_ref|rt_openfhe> -CKKS2PY:df=<file>``) the emitted
``.py`` only defines ``Main_graph`` (no cifar harness); this module supplies that
harness in Python: cifar images (CIFAR_READER-compatible normalization) are fed
one-by-one to the rtlib-py runtime, the output logits are argmax-compared to the
ground-truth label, and a ``[RESULT]`` line equivalent to the C path is produced.

Mirrors ``rtlib_py/common/driver.run()`` for setup (exec module -> seed _CTX ->
seed _DATA_INFO (offline weights) -> fetch encode schemes) but, unlike
``driver.run`` which compares to a golden once, it re-seeds only the input per
image and re-runs ``Main_graph`` (context + loaded weights are reused across
images -- the C path does ``Prepare_context`` once too). It does NOT call
``prov._reset`` between images (that would discard the loaded weights).

Reusable by both ``run_perf`` (1 image, [index,index]) and ``run_accuracy``
(N images, [index, index+num-1]).
"""
import os
import sys
import time

# rtlib-py must be importable for the emitted module's `from rtlib_py.<rt> import *`.
_RTLIB_PY_DIR = os.environ.get("RTLIB_PY_DIR") or os.path.abspath(
    os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", "rtlib-py"))
if os.path.isdir(os.path.join(_RTLIB_PY_DIR, "rtlib_py")) and _RTLIB_PY_DIR not in sys.path:
    sys.path.insert(0, _RTLIB_PY_DIR)


# ImageNet normalization, identical to resnet_cifar.main.inc.
CIFAR_MEAN = [0.485, 0.456, 0.406]
CIFAR_STDEV = [0.229, 0.224, 0.225]
PIXEL_SIZE = 3 * 32 * 32  # 3072


def _row_size(nclass):
    # cifar-10: 1 label byte; cifar-100: 2 label bytes (coarse + fine).
    return PIXEL_SIZE + (2 if nclass == 100 else 1)


def cifar_count(path, nclass):
    if not os.path.isfile(path):
        return 0
    return os.path.getsize(path) // _row_size(nclass)


def cifar_load(path, index, nclass):
    """Replicate CIFAR_READER::Load: CHW layout,
    ``data[ch*1024+wh] = (buf[px+label_size]/255 - mean[ch]) / stdev[ch]``.
    Returns (data[3072] floats, label). label==-1 on short read."""
    lab_size = 2 if nclass == 100 else 1
    rs = _row_size(nclass)
    with open(path, "rb") as f:
        f.seek(index * rs)
        buf = f.read(rs)
    if len(buf) < rs:
        return None, -1
    label = buf[1] if nclass == 100 else buf[0]   # cifar-100 fine label is buf[1]
    data = [0.0] * PIXEL_SIZE
    for ch in range(3):
        for wh in range(32 * 32):
            px = ch * (32 * 32) + wh
            data[px] = (buf[px + lab_size] / 255.0 - CIFAR_MEAN[ch]) / CIFAR_STDEV[ch]
    return data, label


def _setup(py_file, backend, strict=True):
    """exec the emitted module, seed ctx + offline-data descriptor + encode
    schemes. Returns (ns, provider, schemes). Raises on setup failure."""
    from rtlib_py.common.provider import get_provider
    from rtlib_py.common.driver import _seed_inputs  # noqa: F401  (kept for API parity)

    prov = get_provider(backend)
    prov._reset()
    prov.configure(strict=strict)

    with open(py_file, "r") as f:
        src = f.read()
    ns = {"__name__": "under_test", "__file__": py_file}
    exec(compile(src, py_file, "exec"), ns)

    gcp = ns.get("get_context_params")
    if gcp is None:
        raise RuntimeError("emitted module defines no get_context_params()")
    prov._seed_ctx(gcp())

    grtdi = ns.get("get_rt_data_info")
    info = grtdi() if grtdi is not None else None
    if hasattr(prov, "_seed_data_info"):
        prov._seed_data_info(info)

    schemes = None
    gges = ns.get("get_encode_scheme")
    gic = ns.get("get_input_count")
    if gges is not None and gic is not None:
        schemes = {}
        for i in range(int(gic())):
            s = gges(i)
            schemes[s["name"]] = s
    return ns, prov, schemes


def run_py_on_cifar(py_file, cifar_file, nclass, index, end_idx, backend,
                    input_name="input", output_name="output", strict=True,
                    per_image_log=False):
    """Run the emitted .py on cifar images [index, end_idx] under the rtlib-py
    provider ``backend`` and compute top-1 accuracy.

    Returns dict with: pass_cnt, total, accuracy, wall_sec, exec_sec,
    result_line (``[RESULT] ...`` mirroring the C path), errors (list of
    per-image runtime errors), setup_failed (bool) + setup_err.
    """
    from rtlib_py.common.driver import _seed_inputs

    res = dict(pass_cnt=0, total=0, accuracy=0.0, wall_sec=0.0, exec_sec=0.0,
               result_line="", errors=[], setup_failed=False, setup_err="")
    t0 = time.time()
    try:
        ns, prov, schemes = _setup(py_file, backend, strict=strict)
    except Exception as ex:  # setup failure -> whole run fails (mirrors C sys.exit on missing exec)
        import traceback
        res["setup_failed"] = True
        res["setup_err"] = f"{type(ex).__name__}: {ex}\n" + traceback.format_exc()
        res["result_line"] = f"[RESULT] setup failed: {type(ex).__name__}: {str(ex)[:120]}"
        res["wall_sec"] = time.time() - t0
        return res

    mg = ns["Main_graph"]
    n_total = end_idx - index + 1
    for i in range(index, end_idx + 1):
        data, label = cifar_load(cifar_file, i, nclass)
        if label < 0:
            continue
        res["total"] += 1
        # Per-image: refresh input/output/violations only (keep loaded weights).
        prov._INPUTS = {}
        prov._OUTPUTS = {}
        try:
            prov._VIOLATIONS = []
        except Exception:
            pass
        try:
            _seed_inputs(prov, {"inputs": {input_name: data}}, schemes)
        except Exception as ex:
            res["errors"].append(f"image {i}: input seed failed: {type(ex).__name__}: {ex}")
            continue
        ti = time.time()
        try:
            mg()
        except prov.FakeRtError as ex:
            res["errors"].append(f"image {i}: runtime constraint: {ex}")
            res["exec_sec"] += time.time() - ti
            continue
        except Exception as ex:
            res["errors"].append(f"image {i}: Main_graph failed: {type(ex).__name__}: {ex}")
            res["exec_sec"] += time.time() - ti
            continue
        res["exec_sec"] += time.time() - ti
        out = prov._OUTPUTS.get((output_name, 0))
        if out is None:
            res["errors"].append(f"image {i}: no output '{output_name}' captured; "
                                 f"keys={list(prov._OUTPUTS)}")
            continue
        slots = getattr(out, "slots", out)
        logits = [float(x) for x in list(slots)[:nclass]]
        if len(logits) < nclass:
            res["errors"].append(f"image {i}: output has {len(logits)} slots < nclass {nclass}")
            continue
        pred = max(range(nclass), key=lambda k: logits[k])
        if pred == label:
            res["pass_cnt"] += 1
        if per_image_log:
            print(f"[INFO] infer image {i} {'success' if pred == label else 'failed'}"
                  f" (pred={pred}, label={label})")
    res["accuracy"] = (res["pass_cnt"] / res["total"]) if res["total"] else 0.0
    fail = res["total"] - res["pass_cnt"]
    res["result_line"] = (
        f"[RESULT] infer {res['total']} images, pass {res['pass_cnt']} "
        f"{res['accuracy']:.3f}, fail {fail} {(1.0 - res['accuracy']):.3f}.")
    res["wall_sec"] = time.time() - t0
    # Print to stdout so the pytest driver (subprocess capture) can parse the
    # result, mirroring the C exec which prints [RESULT] to stdout.
    print(res["result_line"])
    print("PYRUN: wall=%.2fs exec=%.2fs accuracy=%.3f pass=%d/%d "
          "setup_failed=%d n_errors=%d"
          % (res["wall_sec"], res["exec_sec"], res["accuracy"], res["pass_cnt"],
             res["total"], res["setup_failed"], len(res["errors"])))
    for e in res["errors"][:10]:
        print("  ERR:", e)
    if res["setup_failed"]:
        print("  SETUP_ERR:", str(res["setup_err"])[:500])
    return res