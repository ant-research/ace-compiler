#!/usr/bin/env python3
#
# Build the FPGA baseline (= cpu baseline's copy with fhe_ops interface
# adaptation; CPU verification path) under baselines/fpga/. Assumes the
# source tree is already laid out at baselines/fpga/cnn_ckks/ (the fpga
# adaptation is git tracked there; pretrained_parameters / testFile /
# testFile_cifar100 are gitignored data, cloned alongside as needed).
#
# This script does NOT clone or patch — bootstrap (copy cpu baseline's
# cnn_ckks source + apply the fhe_ops adaptation, which lives in git)
# is documented in baselines/README.md ("FPGA baseline source"). The
# CPU backend macro FHE_BACKEND_CPU is the default for this build (the
# CPU verification path); switch to FHE_BACKEND_FPGA in
# baselines/fpga/cnn_ckks/CMakeLists.txt once the FPGA SDK is wired
# (see .claude/plans/fpga-adaptation/FPGA-INTEGRATION.md).
#
# Outputs:
#   baselines/fpga/cnn_ckks/build_fpga/cnn  — the executable, run as
#      cd baselines/fpga/cnn_ckks/build_fpga && ./cnn <layer> <dataset> <start> <end>

import os

cnn_ckks = '/app/baselines/fpga/cnn_ckks'

# cmake configure + build (CNN=ON picks the cnn target; build_fpga/ inner
# dir name uses fpga, not the historical iie).
build_fpga = (
    f'cmake -S {cnn_ckks} -B {cnn_ckks}/build_fpga '
    f'-DCNN=ON -DCMAKE_BUILD_TYPE=Release; '
    f'cmake --build {cnn_ckks}/build_fpga -- -j'
)

print('\nbuild fpga cnn_ckks ...')
ret = os.system(build_fpga)
assert ret == 0, f'build_cnn_fpga failed (rc={ret})'

print('\nDONE — cnn binary at baselines/fpga/cnn_ckks/build_fpga/cnn')
