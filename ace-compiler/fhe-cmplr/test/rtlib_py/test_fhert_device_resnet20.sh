#!/bin/bash
# rt_fhert device resnet20 e2e gate (default OFF; F4.9 SKIP-77 ctest).
#
# Drives run_resnet20_vs_onnx.py --backend rt_fhert --device to run a full
# resnet20_cifar10_pre inference on the fhert device. Default OFF: this
# ctest self-SKIPs (exit 77, ctest SKIP_RETURN_CODE) unless
# FHE_RUN_RESNET20_DEVICE=1 is set, so a standard `ctest` run does NOT
# exercise the device path. resnet20 device is a known-limited feature
# (per-cycle HBM accumulation; fix B1 future); this refactor records the
# device suite as NOT-RUN-in-this-env (see F4.9 / F4.12).
#
# Args: $1 = fhe-cmplr binary dir.  $2 = python interpreter (default python3).

set -u

# Default OFF -- must be explicitly enabled via FHE_RUN_RESNET20_DEVICE=1.
if [ "${FHE_RUN_RESNET20_DEVICE:-0}" != "1" ]; then
  echo "SKIP: resnet20-device ctest default-off (FHE_RUN_RESNET20_DEVICE != 1)"
  exit 77
fi

BIN="$1/driver/fhe_cmplr"
PY="${2:-${FHE_PY:-python3}}"

if [ ! -x "$BIN" ]; then
  echo "FAIL: fhe_cmplr not found at $BIN" >&2
  exit 1
fi
if ! command -v "$PY" >/dev/null 2>&1; then
  echo "FAIL: python interpreter '$PY' not found" >&2
  exit 1
fi

FHR="${FHERT_ROOT:-/workspace/fhert}"
export FHERT_ROOT="$FHR"
if [ -d "$FHR" ]; then
  export PYTHONPATH="$FHR${PYTHONPATH:+:$PYTHONPATH}"
fi
if ! "$PY" -c "import fhert.openfhe, onnx, onnxruntime" >/dev/null 2>&1; then
  echo "SKIP: $PY lacks fhert.openfhe/onnx/onnxruntime (resnet20 device gate needs them)"
  exit 77
fi

SRC="$(cd "$(dirname "$0")" && pwd)"
PY_VERIFY="$SRC/py_verify"

exec "$PY" "$PY_VERIFY/run_resnet20_vs_onnx.py" --backend rt_fhert --device \
  --cmplr "$BIN"
