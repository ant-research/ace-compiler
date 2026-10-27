#!/usr/bin/env python3
"""ResNet20 (``resnet20_cifar10_pre``) on a cifar image: compile with
``fhe_cmplr`` under an rtlib-py backend, run, and compare the 10 output logits
against onnxruntime.

Self-contained: compiles the model into a temp dir (using the SAME option the
``ace_test.py`` harness uses, via ``ace_util.get_ace_option`` /
``ace_compile._ace_option_to_py``), loads a cifar image, runs the emitted ``.py``
under the backend, and prints onnxruntime vs backend logits + max_abs + preds.

Reuses the existing harness machinery (no bespoke numerics). Host and device
modes are selected by the same env knobs the ``rt_fhert`` provider reads
(``FHE_DEVICE``, ``FHE_DEVICE_RING``, ``FHE_DEVICE_HW``, ``FHE_BTS_SF``,
``FHE_BTS_CORRECTION``); pass ``--device`` to set ``FHE_DEVICE=1``.

Usage (run from anywhere; paths default to the repo layout):

  # fast plaintext reference (validates the script end-to-end, ~90 s):
  FHERT_ROOT=/workspace/fhert python3 run_resnet20_vs_onnx.py --backend rt_ref

  # fhert host (real CKKS bootstrap, ~20 min):
  FHERT_ROOT=/workspace/fhert FHE_BTS_SF=59 FHE_BTS_CORRECTION=13 \
      python3 run_resnet20_vs_onnx.py --backend rt_fhert

  # fhert device (currently fails in device setup -- see the handoff doc):
  FHERT_ROOT=/workspace/fhert FHE_DEVICE=1 FHE_DEVICE_RING=65536 \
      FHE_DEVICE_HW=256,256,1,8 FHE_BTS_SF=59 FHE_BTS_CORRECTION=13 \
      XDG_CACHE_HOME=/tmp/fhert_dev_cache \
      python3 run_resnet20_vs_onnx.py --backend rt_fhert --device
"""
import argparse
import os
import sys
import subprocess
import tempfile
import time
import shutil

_HERE = os.path.dirname(os.path.abspath(__file__))
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)
_TEST_DIR = os.path.abspath(os.path.join(_HERE, "..", ".."))  # test/ holds ace_util/ace_compile/run_py_accuracy
if _TEST_DIR not in sys.path:
    sys.path.insert(0, _TEST_DIR)
_RTLIB_PY_DIR = os.environ.get("RTLIB_PY_DIR") or os.path.abspath(
    os.path.join(_HERE, "..", "..", "..", "rtlib-py"))
if os.path.isdir(os.path.join(_RTLIB_PY_DIR, "rtlib_py")) and _RTLIB_PY_DIR not in sys.path:
    sys.path.insert(0, _RTLIB_PY_DIR)

import ace_util as au          # get_ace_option / get_test_name (the ace_test option source)
import ace_compile as ac       # _ace_option_to_py (-P2C -> -CKKS2PY mapping)
import run_py_accuracy as rpa  # _setup / cifar_load (the python-path cifar harness)
import onnxruntime as ort
import numpy as np

DEFAULT_ONNX = os.environ.get("FHE_ONNX",
    "/workspace/fhe-cmplr/onnx-models/ace/resnet20_cifar10_pre.onnx")
DEFAULT_CMPLR = os.environ.get("FHE_CMPLR", "")  # FHE_CMPLR env or --cmplr flag; no host-fixed default
DEFAULT_CIFAR = os.environ.get("FHE_CIFAR",
    "/workspace/datasets/cifar-10-batches-bin/data_batch_1.bin")


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--backend", default="rt_fhert",
                    help="rtlib-py provider: rt_ref | rt_openfhe | rt_fhert")
    ap.add_argument("--device", action="store_true",
                    help="set FHE_DEVICE=1 (rt_fhert device mode; ring via "
                         "FHE_DEVICE_RING/FHE_DEVICE_HW env)")
    ap.add_argument("--image", type=int, default=0, help="cifar image index")
    ap.add_argument("--onnx", default=DEFAULT_ONNX)
    ap.add_argument("--cmplr", default=DEFAULT_CMPLR)
    ap.add_argument("--cifar", default=DEFAULT_CIFAR)
    ap.add_argument("--ace-extra", default="",
                    help="extra ACE compile flags (space-separated) appended after "
                         "the default resnet20 option, to sweep the post-BS budget "
                         "(mxbl) and per-relu depth, e.g. "
                         "'-CKKS:mxbl=9 -SIHE:relu_depth=9:relu_basis=1'")
    args = ap.parse_args()

    if not args.cmplr or not os.access(args.cmplr, os.X_OK):
        sys.stderr.write(
            "FAIL: fhe_cmplr not found (pass --cmplr or set FHE_CMPLR env).\n")
        return 2

    if args.device:
        os.environ["FHE_DEVICE"] = "1"

    # 1. compile the model for the backend (same option as ace_test.py)
    test = au.get_test_name("resnet20_cifar10_pre")
    _extra = args.ace_extra.split() if args.ace_extra.strip() else None
    ace_opt = au.get_ace_option(test, None, "openfhe", _extra, False, False)
    wd = tempfile.mkdtemp(prefix="r20_vs_onnx_%s_" % args.backend)
    try:
        py_path = os.path.join(wd, "resnet20_cifar10_pre.py")
        wfile = os.path.join(wd, "weights.dat")
        py_opt = ac._ace_option_to_py(ace_opt, args.backend, wfile)
        cmd = [args.cmplr, args.onnx] + py_opt + ["-o", py_path]
        print("[compile] " + " ".join(cmd), flush=True)
        r = subprocess.run(cmd, cwd=wd, capture_output=True, text=True, timeout=600)
        if r.returncode != 0 or not os.path.isfile(py_path):
            sys.stderr.write("COMPILE FAIL rc=%d\n%s\n%s\n"
                             % (r.returncode, (r.stdout or "")[-800:], (r.stderr or "")[-800:]))
            return 2
        print("[compile] OK -> %s" % py_path, flush=True)

        # 2. onnxruntime reference logits for the cifar image
        data, label = rpa.cifar_load(args.cifar, args.image, 10)
        if label < 0:
            sys.stderr.write("could not load cifar image %d\n" % args.image)
            return 2
        x = np.array(data, dtype=np.float32).reshape(1, 3, 32, 32)
        sess = ort.InferenceSession(args.onnx, providers=["CPUExecutionProvider"])
        onnx_logits = [float(v) for v in sess.run(None, {"input": x})[0][0]]
        onnx_pred = int(np.argmax(onnx_logits))

        # 3. run the emitted .py under the backend, capture output logits
        from rtlib_py.common.driver import _seed_inputs
        t0 = time.perf_counter()
        ns, prov, schemes = rpa._setup(py_path, args.backend, strict=True)  # may crash in device setup
        prov._INPUTS = {}
        prov._OUTPUTS = {}
        try:
            prov._VIOLATIONS = []
        except Exception:
            pass
        _seed_inputs(prov, {"inputs": {"input": data}}, schemes)
        ns["Main_graph"]()
        dt = time.perf_counter() - t0
        out = prov._OUTPUTS.get(("output", 0))
        if out is None:
            sys.stderr.write("no 'output' captured; keys=%s\n" % list(prov._OUTPUTS))
            return 2
        be_logits = [float(v) for v in list(out.slots)[:10]]
        be_pred = int(max(range(10), key=lambda k: be_logits[k]))
        max_abs = max(abs(a - b) for a, b in zip(be_logits, onnx_logits))

        # 4. report
        print("image=%d label=%d backend=%s%s" %
              (args.image, label, args.backend, " [device]" if args.device else ""))
        print("onnx      pred=%d logits=%s" % (onnx_pred, [round(v, 4) for v in onnx_logits]))
        print("%-9s pred=%d logits=%s" % (args.backend, be_pred, [round(v, 4) for v in be_logits]))
        print("time=%ds  max_abs(vs onnx)=%.4f  pred_match=%s"
              % (int(dt), max_abs, be_pred == onnx_pred == label))
        return 0 if be_pred == label else 1
    finally:
        shutil.rmtree(wd, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())
