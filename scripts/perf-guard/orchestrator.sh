#!/bin/bash
# orchestrator.sh
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# In-container driver for the nightly ResNet performance guard.
#
# Runs benchmark/resnet/ under pytest. The committed baselines
# (test_latency/*.csv, test_memory/*.csv, test_accuracy/*.yml) are compared by
# pytest-regressions; this script just runs pytest read-only and aggregates the
# verdict. NEVER pass --regen-all here — that overwrites the committed baselines
# and defeats the guard.
#
# Environment (injected by the host run.sh wrapper):
#   PERF_RUN_ID     e.g. 20260708_0317           (required)
#   CI_TOKEN        for dev-build internal deps   (required if building)
#   CUDA_VISIBLE_DEVICES  pinned GPU             (default 0)
#   ACE_DATA_DIR    CIFAR dataset root            (default /work/dataset)
#
# Options:
#   PERF_SKIP_BUILD=1   skip dev-build (assume already built)
#   PERF_DRY=1          print the pytest command, do not execute
#   PERF_FAST=1         subset: latency resnet20+110 phantom + memory only
#
# Output:
#   $PERF_HISTORY/<run_id>/{junit.xml,pytest.log}
#   $PERF_HISTORY/history.jsonl   (one line appended by summarize.py)
set -euo pipefail

REPO_ROOT="/work/ace-fhe/ace-compiler"
cd "$REPO_ROOT"

# ----------------------------------------------------------------------------
# Config
# ----------------------------------------------------------------------------
PERF_RUN_ID="${PERF_RUN_ID:-$(date -u +%Y%m%d_%H%M)}"
PERF_HISTORY_ROOT="${PERF_HISTORY_ROOT:-/mnt/nvme0/perf_history/resnet}"
PERF_OUT_DIR="$PERF_HISTORY_ROOT/runs/$PERF_RUN_ID"
PERF_HISTORY_JSONL="$PERF_HISTORY_ROOT/history.jsonl"
ACE_DATA_DIR="${ACE_DATA_DIR:-/work/dataset}"
CUDA_VISIBLE_DEVICES="${CUDA_VISIBLE_DEVICES:-0}"

export ACE_DATA_DIR CUDA_VISIBLE_DEVICES PERF_RUN_ID

mkdir -p "$PERF_OUT_DIR"

# ----------------------------------------------------------------------------
# Preflight
# ----------------------------------------------------------------------------
# Kill any zombie pytest/fhe processes from a prior aborted run.
pkill -9 -f 'pytest benchmark' 2>/dev/null || true

# Locate the installed runtime libs (develop.md "Runtime Library Issues").
ACE_LIB="$(python3 -c 'import sysconfig; print(sysconfig.get_path("purelib"))')/ace/lib"
export LD_LIBRARY_PATH="${ACE_LIB}:${LD_LIBRARY_PATH:-}"

GIT_SHA="$(git rev-parse --short HEAD 2>/dev/null || echo unknown)"

echo "=== nightly perf-guard  run_id=$PERF_RUN_ID  git=$GIT_SHA ==="
echo "=== gpu=CUDA_VISIBLE_DEVICES=$CUDA_VISIBLE_DEVICES  out=$PERF_OUT_DIR ==="

# ----------------------------------------------------------------------------
# Build (optional)
# ----------------------------------------------------------------------------
if [[ "${PERF_SKIP_BUILD:-0}" != "1" ]]; then
    echo "=== build (dev-build.sh) ==="
    CI_TOKEN="${CI_TOKEN:-}" ./scripts/dev-build.sh
else
    echo "=== PERF_SKIP_BUILD=1, skipping build ==="
fi

# Sanity: ace import must work before we pay for pytest collection.
python3 -c 'import ace; print("ace import OK")'

# ----------------------------------------------------------------------------
# Select test subset
# ----------------------------------------------------------------------------
if [[ "${PERF_FAST:-0}" == "1" ]]; then
    # Quick guard: depth endpoints on phantom only + the full memory file.
    PYTEST_TARGETS=(
        'benchmark/resnet/test_latency.py'
        'benchmark/resnet/test_memory.py'
    )
    PYTEST_K='-k (resnet20 or resnet110) and phantom'
else
    PYTEST_TARGETS=('benchmark/resnet/')
    PYTEST_K=''
fi

# -o addopts="" clears pytest.ini defaults so --junitxml is the only reporter.
PYTEST_CMD=(
    pytest
    "${PYTEST_TARGETS[@]}"
    -o addopts=""
    -p no:cacheprovider
    --tb=short
    --junitxml="$PERF_OUT_DIR/junit.xml"
)
[[ -n "$PYTEST_K" ]] && PYTEST_CMD+=("$PYTEST_K")

echo "=== pytest ==="
echo "    ${PYTEST_CMD[*]}"

if [[ "${PERF_DRY:-0}" == "1" ]]; then
    echo "=== PERF_DRY=1, not executing ==="
    exit 0
fi

# Run pytest; capture exit code (1 = tests failed, which is a valid guard result).
set +e
"${PYTEST_CMD[@]}" 2>&1 | tee "$PERF_OUT_DIR/pytest.log"
PYTEST_RC=${PIPESTATUS[0]}
set -e

echo "=== pytest exit code: $PYTEST_RC ==="

# ----------------------------------------------------------------------------
# Aggregate -> history.jsonl
# ----------------------------------------------------------------------------
python3 "$REPO_ROOT/scripts/perf-guard/summarize.py" \
    --junit "$PERF_OUT_DIR/junit.xml" \
    --history "$PERF_HISTORY_JSONL" \
    --run-id "$PERF_RUN_ID" \
    --git "$GIT_SHA"
SUMMARY_RC=$?

# summarize.py returns 0 on pass / 1 on fail; prefer its verdict, but if
# summarize itself failed (e.g. junit missing), surface the pytest/rc error.
if [[ $SUMMARY_RC -ne 0 && $SUMMARY_RC -ne 1 ]]; then
    echo "=== summarize.py failed (rc=$SUMMARY_RC), surfacing pytest rc=$PYTEST_RC ==="
    exit "$PYTEST_RC"
fi

exit "$SUMMARY_RC"
