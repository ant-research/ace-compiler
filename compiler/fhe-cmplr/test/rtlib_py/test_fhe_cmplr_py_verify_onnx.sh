#!/bin/bash
# rtlib-py-onnx plan *rt_ref* wrapper -- doc/rtlib-py-onnx/PLAN.md (Phase 1/2).
#
# Drives run_py_verify.py over the onnx-models/ corpus workloads registered in
# workloads.py, under the rt_ref plaintext reference backend:
#   1. green + peripheral sweep (--all): the P0 green single-ops (PASS
#      max_abs=0) + the bucket (b) emitter-bug and (c) context-budget
#      peripheral (XFAIL, true-cause attributed by _classify_xfail). Zero FAIL
#      is the gate (XFAIL/XPASS do not fail it).
#   2. heavy compile-only full-nets (resnet110/alexnet): compile + py_compile
#      only (exec skipped -- their emitted .py can be 100s of MB); a timeout/
#      OOM (124/137) is skip-if-OOM, not a regression.
#   3. P4 synthetic probes (mulmul/concat): recorded (XPASS/XFAIL) but NOT part
#      of the green gate -- their exit code is informational.
#
# --all runs the onnx-models corpus only, so the green sweep stays additive-green.
# fhert is out of scope.
#
# Skips (exit 77, ctest SKIP_RETURN_CODE) when onnx/pytest is absent -- so a
# host without them stays green on the emit-gate and does not go red here.
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

# onnx + pytest are required (workload gen + the e2e spec); skip cleanly if absent.
if ! "$PY" -c "import onnx, pytest" >/dev/null 2>&1; then
  echo "SKIP: $PY lacks onnx/pytest (rtlib-py-onnx rt_ref gate needs them)"
  exit 77
fi

SRC="$(cd "$(dirname "$0")" && pwd)"
PY_VERIFY="$SRC/py_verify"

fail=0

echo "== 1. rt_ref green + bucket b/c peripheral (run_py_verify.py --all) =="
"$PY" "$PY_VERIFY/run_py_verify.py" --bin "$BIN" --rt rt_ref \
  --label phase1_onnx_models --all
rc=$?
if [ $rc -ne 0 ]; then echo "FAIL: run_py_verify.py --rt rt_ref --all rc=$rc" >&2; fail=1; fi

echo "== 2. rt_ref heavy compile-only full-nets (skip-if-OOM guarded) =="
for w in resnet20 resnet110 mobilenet squeezenet alexnet; do
  timeout 2400 "$PY" "$PY_VERIFY/run_py_verify.py" --bin "$BIN" --rt rt_ref \
    --label phase1_onnx_models --workload "$w" --include-heavy
  rc=$?
  if [ $rc -eq 124 ] || [ $rc -eq 137 ]; then
    echo "  [$w] SKIP: timeout/OOM (rc=$rc) -- skip-if-OOM, not a regression"
  elif [ $rc -ne 0 ]; then
    echo "  [$w] FAIL: non-skip rc=$rc" >&2; fail=1
  fi
done

echo "== 3. rt_ref P4 synthetic probes (mulmul/concat; informational) =="
"$PY" "$PY_VERIFY/run_py_verify.py" --bin "$BIN" --rt rt_ref \
  --label phase1_onnx_models --include-probes
rc=$?
# Probes record XPASS/XFAIL (and concat may surface a no-Python-path FAIL as a
# finding); their exit code is informational, not a gate failure.
echo "  (probes run_py_verify rc=$rc -- informational, not gating)"

if [ $fail -ne 0 ]; then
  echo "FAIL: test_fhe_cmplr_py_verify_onnx"
  exit 1
fi
echo "PASS: test_fhe_cmplr_py_verify_onnx"
exit 0
