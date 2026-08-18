#!/bin/bash
# run_micro.sh — Run micro benchmarks across configs
#
# Usage:
#   ./run_micro.sh                          # all 7 configs
#   ./run_micro.sh -c base                 # single config
#   ./run_micro.sh -c base,lmr             # multiple configs
#   ./run_micro.sh -c all -m i512_o10      # single model
#   ./run_micro.sh --model-first           # iterate by model then config
#   ./run_micro.sh --compile-only          # compile only
#   ./run_micro.sh --list                  # list configs

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "${SCRIPT_DIR}/micro_common.sh"

# Config: "compiler_options fast_dotprod"
declare -A CFGS=(
    [base]="VEC:conv_fast 0"
    [modup]="POLY:muh VEC:conv_fast 0"
    [moddown]="POLY:mdh VEC:conv_fast 0"
    [fm_cte]="POLY:mef P2C:cte:ctp:df_limit=819200 VEC:conv_fast 0"
    [lmr]="VEC:conv_fast 1"
    [all]="POLY:muh:mdh:mef P2C:cte:ctp:df_limit=819200 VEC:conv_fast 1"
)

ALL_CONFIGS="base modup moddown fm_cte lmr all"

COMPILE_ONLY=0
MODEL_OPT=""
MODEL_FIRST=0

# --- CLI ---

CONFIG=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -c) CONFIG="$2"; shift 2 ;;
        -m) MODEL_OPT="$2"; shift 2 ;;
        --model-first) MODEL_FIRST=1; shift ;;
        --compile-only) COMPILE_ONLY=1; shift ;;
        --list)
            echo "Configs: ${ALL_CONFIGS}"
            for c in $ALL_CONFIGS; do echo "  $c = ${CFGS[$c]}"; done
            exit 0 ;;
        -h|--help)
            echo "Usage: $0 [-c CONFIG] [-m MODEL] [--model-first] [--compile-only] [--list]"
            echo ""
            echo "Options:"
            echo "  -c CONFIG       Config name or comma-separated (default: all)"
            echo "  -m MODEL        Single micro model name (e.g. i512_o10)"
            echo "  --model-first   Iterate by model then config (default: by config then model)"
            echo "  --compile-only  Skip link and run"
            echo "  --list          List configs and options"
            echo ""
            echo "Configs:"
            echo "  base              -VEC:conv_fast"
            echo "  modup             -POLY:muh -VEC:conv_fast"
            echo "  moddown           -POLY:mdh -VEC:conv_fast"
            echo "  fm_cte            -POLY:mef -P2C:cte:ctp:df_limit=819200 -VEC:conv_fast"
            echo "  lmr               -VEC:conv_fast  (FAST_DOT_PROD=1)"
            echo "  all               -POLY:muh:mdh:mef -P2C:cte:ctp:df_limit=819200 -VEC:conv_fast  (FAST_DOT_PROD=1)"
            echo ""
            echo "Models: ${MICRO_MODELS[*]}"
            exit 0 ;;
        *) echo "Unknown option: $1" >&2; exit 1 ;;
    esac
done

# --- Resolve configs ---

if [[ -n "$CONFIG" ]]; then
    IFS=',' read -ra SELECTED <<< "$CONFIG"
    for c in "${SELECTED[@]}"; do
        [[ -z "${CFGS[$c]+_}" ]] && { echo "ERROR: invalid config '$c'" >&2; exit 1; }
    done
else
    SELECTED=($ALL_CONFIGS)
fi

# --- Resolve models ---

if [[ -n "$MODEL_OPT" ]]; then
    MODELS=("$MODEL_OPT")
else
    MODELS=("${MICRO_MODELS[@]}")
fi

TOTAL=$((${#MODELS[@]} * ${#SELECTED[@]}))

# --- Run ---

[[ ! -f "$FHE_CMPLR" ]] && { echo "ERROR: fhe_cmplr not found at $FHE_CMPLR" >&2; exit 1; }

PASS=0; FAIL=0
declare -A RESULTS

run_with_config() {
    local c="$1"
    local cfg_line="${CFGS[$c]}"
    CFG_NAME="$c"
    FAST_DP="${cfg_line##* }"
    local_opts="${cfg_line% *}"
    # Convert "OPT1 OPT2" to "-OPT1 -OPT2"
    CFG_OPTS=""
    for tok in $local_opts; do
        CFG_OPTS="${CFG_OPTS} -${tok}"
    done
    CFG_OPTS="${CFG_OPTS# }"
    [[ "$FAST_DP" != "1" ]] && FAST_DP=""
}

if [[ $MODEL_FIRST -eq 1 ]]; then
    for m in "${MODELS[@]}"; do
        for c in "${SELECTED[@]}"; do
            run_with_config "$c"
            log ">>> ${m} [${c}]"
            if run_one "$m"; then
                RESULTS["${m}.${c}"]="PASS"; PASS=$((PASS+1))
            else
                RESULTS["${m}.${c}"]="FAIL"; FAIL=$((FAIL+1))
            fi
        done
    done
else
    for c in "${SELECTED[@]}"; do
        run_with_config "$c"
        for m in "${MODELS[@]}"; do
            log ">>> ${m} [${c}]"
            if run_one "$m"; then
                RESULTS["${m}.${c}"]="PASS"; PASS=$((PASS+1))
            else
                RESULTS["${m}.${c}"]="FAIL"; FAIL=$((FAIL+1))
            fi
        done
    done
fi

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
