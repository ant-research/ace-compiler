#!/bin/bash
# Phase 3b *real EvalBootstrap* smoke test (rt_openfhe) --
# doc/rtlib-py-integrate/rt_openfhe_python_plan.md, 3b.
#
# Builds the toy Set-A bootstrap context (ring 2^12, FIXEDMANUAL, level_budget
# [4,4]), encodes a small message near the top of the chain, and asserts
# rt_openfhe.bootstrap_ciph (the real EvalBootstrap behind the single Phase-2.4
# name) recovers it within 1e-2 with a fresh scale_deg=1. Slow (~6s); the ctest
# gives a wide timeout. Set OPENFHE_RUN_BOOTSTRAP=0 to opt out (CI-flakiness
# escape hatch; the test runs by default so 3b is a real gate).
#
# Skips (exit 77, ctest SKIP_RETURN_CODE) when openfhe-python / pytest absent.
#
# Args: $1 = fhe-cmplr binary dir (ignored -- the provider builds its own
#            context, no compile needed); $2 = python interpreter (default
#            python3).

set -u

PY="${2:-${FHE_PY:-python3}}"

if ! command -v "$PY" >/dev/null 2>&1; then
  echo "FAIL: python interpreter '$PY' not found" >&2
  exit 1
fi

OFD="${OPENFHE_PY_BUILD_DIR:-/workspace/openfhe-python/build}"
export OPENFHE_PY_BUILD_DIR="$OFD"
if [ -d "$OFD" ]; then
  export PYTHONPATH="$OFD${PYTHONPATH:+:$PYTHONPATH}"
fi

if ! "$PY" -c "import openfhe, pytest" >/dev/null 2>&1; then
  echo "SKIP: $PY lacks openfhe-python/pytest (Phase 3b bootstrap gate needs them)"
  exit 77
fi

SRC="$(cd "$(dirname "$0")" && pwd)"
PY_VERIFY="$SRC/py_verify"

exec "$PY" -m pytest "$PY_VERIFY/test_openfhe_bootstrap.py" -q