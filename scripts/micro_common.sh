#!/bin/bash
# micro_common.sh — Shared pipeline for micro-benchmark scripts
#
# Sourced by run_micro_<config>.sh scripts.
# Caller must set before sourcing:
#   CFG_NAME  — config name for log filename (base, modup, moddown, fm_cte, lmr, all)
#   CFG_OPTS  — compiler options for this config
#   FAST_DP   — "1" to enable FAST_DOT_PROD, empty otherwise

SCRIPT_DIR=/app/scripts
PROJECT_DIR=/app/ace-compiler
FHE_CMPLR=/app/hpao_cmplr/bin/fhe_cmplr
ONNX_DIR=/app/model
TEST_DIR=/app/test
CMPLR_DIR=/app/hpao_cmplr
RESULT_DIR="${AE_RESULT:-/app/ae_result/hpao}"
TMP_DIR="${AE_TMP:-/app/ae_tmp}"

COMMON_OPTS="-CKKS:sk_hw=192 -O2A:ts -FHE_SCHEME:ts -VEC:ts -SIHE:ts -FHE_SCHEME:ts -POLY:ts -P2C:ts -CKKS:ts:q0=60:sf=56 -POLY:ts -P2C:ts -P2C:fp -SIHE:relu_vr_def=10"

MICRO_MODELS=(i16x32x32_ci16_co16_k3p1s1 i32x16x16_ci32_co32_k3p1s1 i64x8x8_ci64_co64_k3p1s1
              i16x32x32_ci16_co32_k3p1s2 i16x32x32_ci16_co32_k1p0s2 i32x16x16_ci32_co64_k3p1s2 i32x16x16_ci32_co64_k1p0s2
              i128x16x16_k2p0s2 i256x8x8_k2p0s2 i512x4x4_k2p0s2
              i64x16x16_ci64_co128_k3p1s1 i512_o10 i64_o10 i4096_o10 i1024_o4096)

log() { echo "[$(date '+%H:%M:%S')] $*"; }

run_one() {
    local model="$1"
    local logfile="${RESULT_DIR}/${model}.${CFG_NAME}.log"

    local opts="${CFG_OPTS} -VEC:ms=32768 -CKKS:N=65536 -P2C:df=${TMP_DIR}/${model}.onnx.weight ${COMMON_OPTS}"

    local onnx="${ONNX_DIR}/${model}.onnx"
    local cfile="${TMP_DIR}/${model}.onnx.c"
    local wfile="${TMP_DIR}/${model}.onnx.weight"
    local exe="${TMP_DIR}/${model}.onnx.out"
    local driver="${TEST_DIR}/${model}.c"

    mkdir -p "${RESULT_DIR}" "${TMP_DIR}"
    > "$logfile"

    log ">>> ${model} [${CFG_NAME}]"

    # --- Compile ---
    local t0=$(date +%s%N) rc=0
    echo "FAST_DOT_PROD=${FAST_DP:-0}" >> "$logfile"
    echo "$FHE_CMPLR $onnx -o $cfile $opts" >> "$logfile"
    (cd "${TMP_DIR}" && env ${FAST_DP:+FAST_DOT_PROD=1} "$FHE_CMPLR" "$onnx" -o "$cfile" $opts) \
        >> "$logfile" 2>&1 || rc=$?
    local t1=$(date +%s%N)
    local elapsed=$(awk "BEGIN{printf \"%.6f\", ($t1 - $t0) / 1000000000}")
    echo "COMPILE RETV:${rc}" >> "$logfile"
    echo "COMPILE CMD TIME:${elapsed}s" >> "$logfile"
    [[ $rc -ne 0 ]] && { rm -f "$wfile" "${TMP_DIR}/${model}.t" "${TMP_DIR}/${model}.json"; log "COMPILE FAIL"; return 1; }
    log "COMPILE OK (${elapsed}s)"

    if [[ $COMPILE_ONLY -eq 1 ]]; then
        rm -f "$wfile" "${TMP_DIR}/${model}.t" "${TMP_DIR}/${model}.json"; log "Skip link/run (--compile-only)"; return 0
    fi

    # --- Link + Run ---
    t0=$(date +%s%N); rc=0
    echo "$CMPLR_DIR $model.onnx $driver ${model}.onnx.c" >> "$logfile"
    cc "$driver" "$cfile" \
        -I"${CMPLR_DIR}/rtlib/include" -I"${CMPLR_DIR}/rtlib/include/rt_ant" \
        "${CMPLR_DIR}/rtlib/lib/libFHErt_ant.a" "${CMPLR_DIR}/rtlib/lib/libFHErt_common.a" \
        /usr/lib/x86_64-linux-gnu/libgmp.so /usr/lib/x86_64-linux-gnu/libm.so \
        -O3 -g -fopenmp -o "$exe" >> "$logfile" 2>&1 || rc=$?
    if [[ $rc -ne 0 ]]; then
        t1=$(date +%s%N)
        elapsed=$(awk "BEGIN{printf \"%.6f\", ($t1 - $t0) / 1000000000}")
        echo "LINK_EXEC RETV:${rc}" >> "$logfile"
        echo "LINK_EXEC CMD TIME:${elapsed}s" >> "$logfile"
        [[ -f "$cfile" ]] && cp "$cfile" "${RESULT_DIR}/${model}.onnx.c"
        rm -f "$wfile" "${TMP_DIR}/${model}.t" "${TMP_DIR}/${model}.json"; log "LINK FAIL"; return 1
    fi

    export RTLIB_BTS_EVEN_POLY=1
    export RTLIB_TIMING_OUTPUT=stdout
    export RT_DATA_SYNC_READ=1
    export PT_ENTRY_COUNT=16
    export PT_PREFETCH_COUNT=8
    if [[ -n "$FAST_DP" ]]; then
        export FAST_DOT_PROD=1
    else
        unset FAST_DOT_PROD
    fi
    /usr/bin/time -f "%es %MKB" "$exe" >> "$logfile" 2>&1
    local run_rc=$?
    unset FAST_DOT_PROD

    t1=$(date +%s%N)
    elapsed=$(awk "BEGIN{printf \"%.6f\", ($t1 - $t0) / 1000000000}")
    echo "Exit $run_rc - Check log: $logfile" >> "$logfile"
    echo "LINK_EXEC RETV:${rc}" >> "$logfile"
    echo "LINK_EXEC CMD TIME:${elapsed}s" >> "$logfile"

    rm -f "$wfile" "${TMP_DIR}/${model}.t" "${TMP_DIR}/${model}.json"
    if [[ $rc -ne 0 ]]; then
        log "LINK FAIL"; return 1
    fi
    if [[ $run_rc -ne 0 ]]; then
        log "RUN FAIL"; return 1
    fi
    log "LINK/RUN OK (${elapsed}s)"
    return 0
}

run_all() {
    COMPILE_ONLY=0
    LOCAL_MODEL=""

    while [[ $# -gt 0 ]]; do
        case "$1" in
            --compile-only) COMPILE_ONLY=1; shift ;;
            -m) LOCAL_MODEL="$2"; shift 2 ;;
            *) echo "Unknown option: $1" >&2; exit 1 ;;
        esac
    done

    [[ ! -f "$FHE_CMPLR" ]] && { echo "ERROR: fhe_cmplr not found at $FHE_CMPLR" >&2; exit 1; }

    if [[ -n "$LOCAL_MODEL" ]]; then
        RUN_MODELS=("$LOCAL_MODEL")
    else
        RUN_MODELS=("${MICRO_MODELS[@]}")
    fi

    declare -A RESULTS
    PASS=0; FAIL=0
    TOTAL=${#RUN_MODELS[@]}

    for m in "${RUN_MODELS[@]}"; do
        if run_one "$m"; then
            RESULTS["${m}.${CFG_NAME}"]="PASS"
            PASS=$((PASS+1))
        else
            RESULTS["${m}.${CFG_NAME}"]="FAIL"
            FAIL=$((FAIL+1))
        fi
    done

    echo ""
    echo "------------------------------------------------------------"
    printf "  %-45s %s\n" "MODEL.CONFIG" "RESULT"
    echo "------------------------------------------------------------"
    for key in $(printf '%s\n' "${!RESULTS[@]}" | sort); do
        printf "  %-45s %s\n" "$key" "${RESULTS[$key]}"
    done
    echo "------------------------------------------------------------"
    echo "  PASS: ${PASS}  FAIL: ${FAIL}  TOTAL: ${TOTAL}"
    echo "  Logs: ${RESULT_DIR}/"
    echo "------------------------------------------------------------"

    [[ $FAIL -eq 0 ]]
}
