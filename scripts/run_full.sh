#!/bin/bash
  
set -e  # terminiate when error occurs
ORIG_DIR="$(pwd)"

err() {
    echo "[ERROR] ($1): $2"
    cd "$ORIG_DIR"
    exit 1
}

cd /app/scripts || err ${LINENO} "Failed to cd to /app/scripts"
./build_cmplr.sh Release || err ${LINENO} "build_cmplr.sh failed"
./hpao.sh || err ${LINENO} "hpao.sh failed"
python3 generate_figures.py || err ${LINENO} "generate_figures.py failed"

cd "$ORIG_DIR" || exit 1
echo "All steps completed successfully"
