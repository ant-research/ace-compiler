#!/bin/bash
# Phase 3b *real EvalBootstrap* smoke test (rt_fhert host) --
# doc/pr676-refactor/ (F4.9 host bootstrap gate).
#
# Mirrors test_openfhe_bootstrap.sh but targets the fhert provider. Builds
# the toy bootstrap context (ring 2^14, FIXEDMANUAL, SPARSE_TERNARY +
# correction factor, level_budget [4,4]), encodes a small message near the
# bottom of the chain, and asserts rt_fhert.bootstrap_ciph (the real
# EvalBootstrap behind the single Phase-2.4 name) recovers it within the
# FHE_BTS_TOL default 1e-1 (lossy refresh; the test prints the measured
# max_abs). Set FHE_RUN_BOOTSTRAP=0 to opt out (CI-flakiness escape hatch;
# the test runs by default so F4.9 is a real gate).
#
# Skips (exit 77, ctest SKIP_RETURN_CODE) when the fhert openfhe binding or
# pytest is absent.
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

# fhert source root (the provider's _ensure_of also reads FHERT_ROOT).
FHR="${FHERT_ROOT:-/workspace/fhert}"
export FHERT_ROOT="$FHR"
if [ -d "$FHR" ]; then
  export PYTHONPATH="$FHR${PYTHONPATH:+:$PYTHONPATH}"
fi

if ! "$PY" -c "import fhert.openfhe, pytest" >/dev/null 2>&1; then
  echo "SKIP: $PY lacks fhert.openfhe/pytest (F4.9 host bootstrap gate needs them)"
  exit 77
fi

SRC="$(cd "$(dirname "$0")" && pwd)"
PY_VERIFY="$SRC/py_verify"

exec "$PY" -m pytest "$PY_VERIFY/test_fhert_bootstrap_host.py" -q
