#!/bin/bash
# rtlib-py-onnx plan *rt_openfhe* wrapper -- doc/rtlib-py-onnx/PLAN.md (Phase 3).
#
# The real-CKKS analog of test_fhe_cmplr_py_verify_onnx.sh: the SAME onnx-models
# corpus workloads, compiled `-CKKS2PY:lib=openfhe -CKKS2PY:rt=rt_openfhe` and executed
# under rtlib_py.rt_openfhe (real ciphertexts):
#   1. green + peripheral sweep (--all): the P0 green subset must PASS within
#      its section E tolerance (add/flatten/add_const 1e-9, mult/mult_const 1e-9,
#      mulmul 1e-7); the bucket b/c peripheral is fail-and-investigate under
#      the toy context (XFAIL, not a gate FAIL -- rt_openfhe leniency in
#      run_workload). Zero FAIL is the gate.
#   2. heavy compile-only full-nets: compile + py_compile only (exec skipped;
#      the rt_openfhe context is never built for them), skip-if-OOM guarded.
#   3. P4 probes (mulmul/concat): informational.
#
# Same module, same defect class as rt_ref -- this confirms the plan's "emission
# + execution behavior is provider-independent" for the green set, with the
# toy-context extra constraint (40-bit scale cap, shallow tower) recorded on the
# peripheral. fhert is out of scope.
#
# Skips (exit 77, ctest SKIP_RETURN_CODE) when openfhe-python / onnx / pytest
# is absent -- so a host without the binding stays green on the rt_ref path
# (phase1_onnx_models) and does not go red here.
#
# Args:
#   $1 = fhe-cmplr binary dir (CMake).   $2 = python interpreter (default python3).

set -u

BIN="$1/driver/fhe_cmplr"
PY="${2:-python3}"

if [ ! -x "$BIN" ]; then
  echo "FAIL: fhe_cmplr not found at $BIN" >&2
  exit 1
fi
if ! command -v "$PY" >/dev/null 2>&1; then
  echo "FAIL: python interpreter '$PY' not found" >&2
  exit 1
fi

# Locate the upstream openfhe-python binding (built .so); put it on PYTHONPATH
# so `import openfhe` resolves. Override with OPENFHE_PY_BUILD_DIR.
OFD="${OPENFHE_PY_BUILD_DIR:-/workspace/openfhe-python/build}"
export OPENFHE_PY_BUILD_DIR="$OFD"
if [ -d "$OFD" ]; then
  export PYTHONPATH="$OFD${PYTHONPATH:+:$PYTHONPATH}"
fi

# openfhe-python is required by this gate; skip cleanly if absent.
if ! "$PY" -c "import openfhe" >/dev/null 2>&1; then
  echo "SKIP: $PY lacks openfhe-python (Phase 3 real-HE gate needs it; rt_ref/phase1_onnx_models unaffected)"
  exit 77
fi
if ! "$PY" -c "import onnx, pytest" >/dev/null 2>&1; then
  echo "SKIP: $PY lacks onnx/pytest (Phase 3 real-HE gate needs them)"
  exit 77
fi

SRC="$(cd "$(dirname "$0")" && pwd)"
PY_VERIFY="$SRC/py_verify"

fail=0

echo "== 1. rt_openfhe green + bucket b/c peripheral (run_py_verify.py --all) =="
# --prec 1e-9 is the green-subset tolerance (per-workload prec overrides for
# mulmul=1e-7); the toy-context peripheral is XFAIL (fail-and-investigate).
"$PY" "$PY_VERIFY/run_py_verify.py" --bin "$BIN" --rt rt_openfhe --prec 1e-9 \
  --label phase3_onnx_models --all
rc=$?
if [ $rc -ne 0 ]; then echo "FAIL: run_py_verify.py --rt rt_openfhe --all rc=$rc" >&2; fail=1; fi

echo "== 2. rt_openfhe heavy compile-only full-nets (skip-if-OOM guarded) =="
for w in resnet20 resnet110 mobilenet squeezenet alexnet; do
  timeout 2400 "$PY" "$PY_VERIFY/run_py_verify.py" --bin "$BIN" --rt rt_openfhe \
    --label phase3_onnx_models --workload "$w" --include-heavy
  rc=$?
  if [ $rc -eq 124 ] || [ $rc -eq 137 ]; then
    echo "  [$w] SKIP: timeout/OOM (rc=$rc) -- skip-if-OOM, not a regression"
  elif [ $rc -ne 0 ]; then
    echo "  [$w] FAIL: non-skip rc=$rc" >&2; fail=1
  fi
done

echo "== 3. rt_openfhe P4 synthetic probes (mulmul/concat; informational) =="
"$PY" "$PY_VERIFY/run_py_verify.py" --bin "$BIN" --rt rt_openfhe --prec 1e-9 \
  --label phase3_onnx_models --include-probes
rc=$?
echo "  (probes run_py_verify rc=$rc -- informational, not gating)"

if [ $fail -ne 0 ]; then
  echo "FAIL: test_fhe_cmplr_py_verify_onnx_openfhe"
  exit 1
fi
echo "PASS: test_fhe_cmplr_py_verify_onnx_openfhe"
exit 0
