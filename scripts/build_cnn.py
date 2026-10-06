#!/usr/bin/env python3
#
# Build the CPU baseline (= FHE-MP-CNN, SEAL 3.6.6 OOP API) under
# baselines/cpu/. Assumes the source tree is already laid out flat at
# baselines/cpu/cnn_ckks/, plus the modified SEAL 3.6.6 source under
# baselines/cpu/cnn_ckks/cpu-ckks/single-key/seal-modified-3.6.6/.
#
# This script does NOT clone or patch — bootstrap (clone upstream +
# apply 0001-capture-keymem-support-resnet32_cifar10.patch) is documented in
# baselines/README.md ("CPU baseline source"). Run that bootstrap once on a
# clean checkout to populate baselines/cpu/cnn_ckks; thereafter this script
# only builds.
#
# Outputs:
#   baselines/cpu/build_seal  — modified SEAL 3.6.6 cmake build (then installed)
#   baselines/cpu/cnn_ckks/build_cnn/cnn  — the executable, run as
#      cd baselines/cpu/cnn_ckks/build_cnn && ./cnn <layer> <dataset> <start> <end>

import os

dir = '/app/baselines/cpu'
cnn_ckks = f'{dir}/cnn_ckks'
seal_src = f'{cnn_ckks}/cpu-ckks/single-key/seal-modified-3.6.6'

# 1) build + install modified SEAL 3.6.6 (lives in baselines/cpu/cnn_ckks/,
#    installs to /usr/local so the cnn link picks it up via find_package(SEAL)).
build_seal = (
    f'cmake -S {seal_src} -B {dir}/build_seal '
    f'-DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5; '
    f'cmake --build {dir}/build_seal -- -j; '
    f'cmake --install {dir}/build_seal'
)

# 2) build the cnn executable (CNN=ON picks the cnn target).
build_cnn = (
    f'cmake -S {cnn_ckks} -B {cnn_ckks}/build_cnn '
    f'-DCNN=ON -DCMAKE_BUILD_TYPE=Release; '
    f'cmake --build {cnn_ckks}/build_cnn -- -j'
)

print('\nbuild seal-3.6.6 ...')
ret = os.system(build_seal)
assert ret == 0, f'build_seal failed (rc={ret})'

print('\nbuild cnn_ckks ...')
ret = os.system(build_cnn)
assert ret == 0, f'build_cnn failed (rc={ret})'

print('\nDONE — cnn binary at baselines/cpu/cnn_ckks/build_cnn/cnn')
