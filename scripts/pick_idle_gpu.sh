#!/bin/bash
#=============================================================================
#
# Select an idle GPU on a shared multi-GPU host, then export
# CUDA_VISIBLE_DEVICES so the calling shell's child processes (pytest, etc.)
# run on exactly that one card.
#
# Why: this server has 8 GPUs but they are shared — any card may be occupied
# by other developers or jobs. CI used to pin a hardcoded card
# (--gpus device=N); if that card was busy the job ran on a contended card
# (the source of latency max-image spikes) or failed outright. Instead the
# job container now sees all cards and this script enumerates every GPU,
# picks ONE idle card at RANDOM (so concurrent CI jobs spread across the box
# instead of all racing for the same index), and exports
# CUDA_VISIBLE_DEVICES=<id> for the child pytest run.
#
# Idle definition (both must hold; thresholds overridable via env):
#   memory.used        < GPU_IDLE_MEM_MIB   (default 200 MiB)
#   utilization.gpu    < GPU_IDLE_UTIL_PCT  (default 5 %)
# Memory is the reliable signal — an occupied card always has HBM resident
# even when the util sample reads 0 at the polling instant. The util gate
# guards against a card mid-alloc-but-not-yet-using.
#
# If no card is idle, wait GPU_IDLE_SLEEP_S (default 120s = 2 min) and retry
# up to GPU_IDLE_RETRIES (default 5 → ~10 min) before giving up with a
# non-zero exit so CI fails loudly rather than running on a busy card.
#
# Usage (must be sourced so the export propagates):
#   source scripts/pick_idle_gpu.sh && CUDA_VISIBLE_DEVICES=$CUDA_VISIBLE_DEVICES pytest ...
#
# Override thresholds (env vars, all optional):
#   GPU_IDLE_MEM_MIB=200 GPU_IDLE_UTIL_PCT=5 \
#   GPU_IDLE_RETRIES=5 GPU_IDLE_SLEEP_S=120 \
#   GPU_SCAN_ORDER="7 6 5 4 3 2 1 0" source scripts/pick_idle_gpu.sh
#
#=============================================================================

# --- Config (env-overridable) ------------------------------------------------
GPU_IDLE_MEM_MIB="${GPU_IDLE_MEM_MIB:-200}"
GPU_IDLE_UTIL_PCT="${GPU_IDLE_UTIL_PCT:-5}"
GPU_IDLE_RETRIES="${GPU_IDLE_RETRIES:-5}"
GPU_IDLE_SLEEP_S="${GPU_IDLE_SLEEP_S:-120}"
# Optional fixed scan order. When unset the script scans ALL GPUs in RANDOM
# order (so concurrent CI jobs spread across cards instead of all racing for
# the same low-index one). Accepts space- or comma-separated indices.
GPU_SCAN_ORDER="${GPU_SCAN_ORDER:-}"

# --- Colors ------------------------------------------------------------------
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Only emit color when stdout is a terminal; CI logs are not, and coloring
# there just produces literal escape noise.
if [ -t 1 ]; then
    C_OK="$GREEN"; C_WARN="$YELLOW"; C_ERR="$RED"
else
    C_OK=""; C_WARN=""; C_ERR=""
fi

# Probe a single GPU index. Return 0 if idle, 1 otherwise.
# Prints the index to stdout on success so the caller can capture it.
_is_gpu_idle() {
    local idx="$1"
    # One nvidia-smi call per card. Slightly more calls than a single sweep,
    # but the loop runs at most once per retry tick and keeps the code dead
    # simple (rare cards stay independently judgeable if a query hiccups).
    local line
    if ! line=$(nvidia-smi --query-gpu=memory.used,utilization.gpu \
        --format=csv,noheader,nounits -i "$idx" 2>/dev/null); then
        return 1
    fi
    # line is "mem_mib, util_pct"
    local mem util
    mem="${line%%,*}"
    util="${line#*,}"
    mem="${mem// /}"
    util="${util// /}"
    # Empty / non-numeric → treat as busy (don't trust unknown state).
    [[ "$mem" =~ ^[0-9]+$ ]] || return 1
    [[ "$util" =~ ^[0-9]+$ ]] || return 1
    if [ "$mem" -lt "$GPU_IDLE_MEM_MIB" ] && [ "$util" -lt "$GPU_IDLE_UTIL_PCT" ]; then
        return 0
    fi
    return 1
}

_pick() {
    local idx
    # Scan order: explicit GPU_SCAN_ORDER if given, else ALL GPUs in RANDOM
    # order (shuf) so concurrent CI jobs land on different cards. Enumerate
    # indices from nvidia-smi -L so cards are discovered, not hardcoded.
    local order="$GPU_SCAN_ORDER"
    if [ -z "$order" ]; then
        order=$(nvidia-smi -L 2>/dev/null | sed -n 's/^GPU \([0-9]*\):.*/\1/p' | shuf | tr '\n' ' ')
    fi
    for idx in $order; do
        if _is_gpu_idle "$idx"; then
            echo "$idx"
            return 0
        fi
    done
    return 1
}

# Print a one-shot snapshot of every GPU's index / mem / util. Replaces the
# bare `nvidia-smi` the YAMLs used to call — now visibility lives here so the
# YAMLs only need `source scripts/pick_idle_gpu.sh`. Printed up front (host
# snapshot before selection); the chosen card is then stated explicitly in the
# summary line, so no in-table marker is needed.
_print_gpu_table() {
    printf "[pick_idle_gpu] GPU state (idx, mem_used_MiB, util_pct):\n" >&2
    nvidia-smi --query-gpu=index,memory.used,utilization.gpu \
        --format=csv,noheader,nounits 2>/dev/null | while IFS=, read -r idx mem util; do
        idx="${idx// /}"; mem="${mem// /}"; util="${util// /}"
        printf "  GPU %-2s  %7s MiB  %3s %%\n" "$idx" "$mem" "$util" >&2
    done
}

# Always print the host snapshot up front so the run log shows what was
# available before selection — mirrors the old standalone `nvidia-smi`.
_print_gpu_table

attempt=0
while [ "$attempt" -lt "$GPU_IDLE_RETRIES" ]; do
    attempt=$((attempt + 1))
    chosen=$(_pick)
    if [ -n "$chosen" ]; then
        export CUDA_VISIBLE_DEVICES="$chosen"
        printf "${C_OK}[pick_idle_gpu] GPU %s is idle (mem<%sMiB util<%s%%) — CUDA_VISIBLE_DEVICES=%s${NC}\n" \
            "$chosen" "$GPU_IDLE_MEM_MIB" "$GPU_IDLE_UTIL_PCT" "$CUDA_VISIBLE_DEVICES" >&2
        # Sourced usage: nothing else to do; env is exported.
        return 0 2>/dev/null || exit 0
    fi
    if [ "$attempt" -lt "$GPU_IDLE_RETRIES" ]; then
        printf "${C_WARN}[pick_idle_gpu] no idle GPU (attempt %s/%s), retrying in %ss…${NC}\n" \
            "$attempt" "$GPU_IDLE_RETRIES" "$GPU_IDLE_SLEEP_S" >&2
        sleep "$GPU_IDLE_SLEEP_S"
    fi
done

printf "${C_ERR}[pick_idle_gpu] no idle GPU after %s attempts over ~%s min — giving up.${NC}\n" \
    "$GPU_IDLE_RETRIES" "$((GPU_IDLE_RETRIES * GPU_IDLE_SLEEP_S / 60))" >&2
# Sourced script that hits an error: can't `exit` (would close the caller's
# shell), so set a flag and return non-zero. Caller's `source ... && pytest`
# already gates on our success — but to be safe also set GPU_PICK_FAILED.
GPU_PICK_FAILED=1
return 1 2>/dev/null || exit 1