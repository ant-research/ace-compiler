#!/bin/bash
# apply_cifar100.sh — Switch CIFAR model test driver inputs to CIFAR-100
#
# Reads one image from the CIFAR-100 binary test set, normalizes it using the
# same formula as cifar_reader.h (mean={0.485,0.456,0.406}, stdev={0.229,0.224,0.225}),
# and replaces the input1[] array in each CIFAR model test driver.
# Originals are backed up as *.c.cifar10_bak.
#
# Usage:
#   ./apply_cifar100.sh                          # download & apply
#   ./apply_cifar100.sh --data-dir /path/to/dir  # use existing dataset
#   ./apply_cifar100.sh --dry-run                # preview only
#   ./apply_cifar100.sh --restore                # revert to originals

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CIFAR100_URL="http://www.cs.toronto.edu/~kriz/cifar-100-binary.tar.gz"
TEST_DIR="/app/test"

log() { echo "[$(date '+%H:%M:%S')] $*"; }

# --- CLI ---
MODE=apply
DATA_DIR=""
IMAGE_IDX=0
AUTO_DOWNLOAD=1

while [[ $# -gt 0 ]]; do
    case "$1" in
        --data-dir) DATA_DIR="$2"; AUTO_DOWNLOAD=0; shift 2 ;;
        --image)    IMAGE_IDX="$2"; shift 2 ;;
        --dry-run)  MODE=dryrun; shift ;;
        --restore)  MODE=restore; shift ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "  Default: Download CIFAR-100 binary dataset and apply to test drivers"
            echo ""
            echo "Options:"
            echo "  --data-dir DIR   CIFAR-100 dataset directory (default: /app/cifar100_data)"
            echo "  --image N        CIFAR-100 image index to use (default: 0)"
            echo "  --dry-run        Show which files would be changed"
            echo "  --restore        Revert to original CIFAR-10 data from backups"
            echo "  -h|--help        Show this message"
            exit 0 ;;
        *) echo "Unknown option: $1" >&2; exit 1 ;;
    esac
done

# --- Restore mode ---
if [[ "$MODE" == "restore" ]]; then
    log "Reverting to CIFAR-10 originals..."
    count=0
    for bak in "$TEST_DIR"/{lenet,resnet,vgg}*.c.cifar10_bak; do
        [[ -f "$bak" ]] || continue
        orig="${bak%.cifar10_bak}"
        cp "$bak" "$orig"
        log "  RESTORED: $(basename "$orig")"
        count=$((count + 1))
    done
    if [[ $count -eq 0 ]]; then
        log "No backups found"
    else
        log "Restored $count file(s). Backups preserved."
    fi
    exit 0
fi

# --- Dry-run mode ---
if [[ "$MODE" == "dryrun" ]]; then
    shopt -s nullglob
    count=0
    for f in "$TEST_DIR"/{lenet,resnet,vgg}*.c; do
        [[ -f "$f" ]] || continue
        if grep -q 'double input1\[\]' "$f" 2>/dev/null; then
            if grep -q '// @CIFAR-100' "$f" 2>/dev/null; then
                log "  ALREADY CIFAR-100: $(basename "$f")"
            else
                log "  WOULD CHANGE: $(basename "$f")"
                count=$((count + 1))
            fi
        fi
    done
    log "$count file(s) would be changed"
    exit 0
fi

# --- Apply mode ---
if [[ -z "$DATA_DIR" ]]; then
    DATA_DIR="/app/cifar100_data"
fi

if [[ "$AUTO_DOWNLOAD" -eq 0 ]]; then
    # User provided --data-dir: look for test.bin there, fail if not found
    if [[ -f "$DATA_DIR/test.bin" ]]; then
        CIFAR100_TEST="$DATA_DIR/test.bin"
    elif [[ -f "$DATA_DIR/cifar-100-binary/test.bin" ]]; then
        CIFAR100_TEST="$DATA_DIR/cifar-100-binary/test.bin"
    else
        echo "ERROR: test.bin not found at $DATA_DIR" >&2
        echo "  Expected: $DATA_DIR/test.bin or $DATA_DIR/cifar-100-binary/test.bin" >&2
        exit 1
    fi
else
    # Auto-download mode
    mkdir -p "$DATA_DIR"
    CIFAR100_TGZ="${DATA_DIR}/cifar-100-binary.tar.gz"
    CIFAR100_TEST="${DATA_DIR}/cifar-100-binary/test.bin"

    if [[ ! -f "$CIFAR100_TEST" ]]; then
        log "CIFAR-100 test.bin not found; downloading..."

        if [[ ! -f "$CIFAR100_TGZ" ]]; then
            log "Downloading CIFAR-100 binary dataset (~161 MB)..."
            log "  URL: $CIFAR100_URL"
            wget --show-progress -O "$CIFAR100_TGZ" "$CIFAR100_URL" || {
                echo "ERROR: Download failed." >&2
                rm -f "$CIFAR100_TGZ"
                echo "  Manual: wget $CIFAR100_URL && tar -xzf cifar-100-binary.tar.gz" >&2
                echo "  Then:  $0 --data-dir ./cifar-100-binary" >&2
                exit 1
            }
        fi

        log "Extracting CIFAR-100 tarball..."
        tar -xzf "$CIFAR100_TGZ" -C "$DATA_DIR"
    fi
fi
log "CIFAR-100 dataset: $CIFAR100_TEST"

# --- Normalize first image from CIFAR-100 binary ---
log "Normalizing CIFAR-100 image ${IMAGE_IDX}..."

PYSCRIPT="${DATA_DIR}/._cifar100_normalize.py"
cat > "$PYSCRIPT" << 'PYEOF'
import sys

binary_file = sys.argv[1]
image_idx = int(sys.argv[2])

with open(binary_file, 'rb') as f:
    f.seek(image_idx * 3074)
    data = f.read(3074)

mean  = [0.485, 0.456, 0.406]
stdev = [0.229, 0.224, 0.225]

vals = []
px = 2  # skip 2-byte label (coarse + fine)
for ch in range(3):
    for _ in range(1024):
        v = (data[px] / 255.0 - mean[ch]) / stdev[ch]
        vals.append(str(v))
        px += 1

sys.stdout.write(', '.join(vals))
PYEOF

INPUT_DATA=$(python3 "$PYSCRIPT" "$CIFAR100_TEST" "$IMAGE_IDX")
rm -f "$PYSCRIPT"

if [[ -z "$INPUT_DATA" ]]; then
    echo "ERROR: Failed to generate CIFAR-100 input data" >&2
    exit 1
fi
log "Normalized 3072 pixels"

# --- Write input data to temp file ---
INPUT_FILE="${DATA_DIR}/._cifar100_input_data.txt"
echo -n "$INPUT_DATA" > "$INPUT_FILE"

# --- Build replacement script ---
REPLACE_SCRIPT="${DATA_DIR}/._cifar100_replace.py"
cat > "$REPLACE_SCRIPT" << 'PYEOF'
import re, sys

with open(sys.argv[1], 'r') as f:
    data = f.read().strip()

replacement = 'double input1[]={' + data + '};'

for target in sys.argv[2:]:
    with open(target, 'r') as f:
        content = f.read()
    new_content, n = re.subn(
        r'double input1\[\]\s*=\s*\{[^}]*\};',
        replacement, content)
    if n > 0:
        new_content = '// @CIFAR-100 input (generated by apply_cifar100.sh)\n' + new_content
        with open(target, 'w') as f:
            f.write(new_content)
        print('OK')
    else:
        print('NO_MATCH')
PYEOF

# --- Find CIFAR model test drivers ---
FILES=()
while IFS= read -r -d '' f; do
    FILES+=("$f")
done < <(grep -lZ "double input1\[\]" "$TEST_DIR"/{lenet,resnet,vgg}*.c 2>/dev/null || true)

if [[ ${#FILES[@]} -eq 0 ]]; then
    log "No CIFAR model test drivers found in $TEST_DIR"
    exit 0
fi

log "Found ${#FILES[@]} CIFAR model test driver(s)"

# --- Apply ---
CHANGED=0
for f in "${FILES[@]}"; do
    fname=$(basename "$f")
    bak="${f}.cifar10_bak"

    if grep -q '// @CIFAR-100' "$f" 2>/dev/null; then
        log "  SKIP (already CIFAR-100): $fname"
        continue
    fi

    if [[ ! -f "$bak" ]]; then
        cp "$f" "$bak"
    fi

    RESULT=$(python3 "$REPLACE_SCRIPT" "$INPUT_FILE" "$f")
    if [[ "$RESULT" == "OK" ]]; then
        log "  CHANGED: $fname"
        CHANGED=$((CHANGED + 1))
    else
        log "  SKIP: $fname"
    fi
done

rm -f "$REPLACE_SCRIPT" "$INPUT_FILE"

echo ""
echo "------------------------------------------------------------"
echo "  Done. ${CHANGED} file(s) changed."
echo "  Backups: ${TEST_DIR}/*.c.cifar10_bak"
echo "  Restore: $0 --restore"
echo "------------------------------------------------------------"