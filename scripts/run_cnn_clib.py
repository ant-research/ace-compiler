#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Drive the FHE-MP-CNN `cnn` executable (built by scripts/build_cnn_clib.py)
# through the clib SDK's real-backend application flow:
#
#   1. export    - the CNN graph runs in export mode (clib::Run_application):
#                  the driver opens the context and every SEAL-level
#                  encode/encrypt request is recorded as a declaration
#                  (clib::Encrypt_input / clib::Encode_plain), together with
#                  the op/rotation requirements;
#   2. context   - clib-ant-tool prepares the ANT parameters, keys,
#                  evaluation keys and bootstrap material for the collected
#                  N/Q/rotation requirements;
#   3. encode    - ANT encodes the declared plaintexts and encrypts the
#                  declared inputs offline;
#   4. run       - the CNN runs in compute mode on the real backend: the
#                  driver opens the context with the ANT material, the
#                  declarations load the encrypted inputs, and every
#                  seal::Evaluator operation executes the real clib kernel;
#                  the final per-image logits are declared as verifiable
#                  outputs (seal::Ciphertext::save_output -> clib::Save_output);
#   5. verify    - ANT decrypts the declared outputs offline and checks each
#                  against the plaintext reference with the declared
#                  tolerance.
#
# The same cnn binary keeps the direct golden-model invocation
# (./cnn LAYERS DATASET START END) when CLIB_MODE is unset.
#
# Level scheduling on the clib backend (see scripts/build_cnn_clib.py, the
# per-cycle second bootstrap step, and clib-seal/Bootstrapper.cpp):
#
#   * clib's Bootstrap refreshes Q=2 to at most Max_bootstrap_q_limbs =
#     Q-30 = 32 limbs on a Q=62 context, i.e. 15 logical levels, while the
#     FHE-MP-CNN cycle consumes 16 between consecutive refreshes
#     (approx-ReLU 14 + multiplexed convolution 2). The staged source now
#     bootstraps a second time right after each in-loop ReLU, splitting
#     every cycle into two pockets (14 | 2) that both fit; the pre-refresh
#     mod-switch ladder is value-exact, so the CNN's numerics are unchanged
#     (the golden model produces bit-identical logits).
#   * the clib-backed Bootstrapper resets the SEAL-style scale ledger to
#     the bootstrapper's final scale at every refresh (as the original SEAL
#     bootstrapper does) and the message-path bootstrap books 32 limbs, so
#     the ledger stays balanced/positive on both the golden and the real
#     paths (without the reset it decays ~logp bits per level and
#     underflows to a zero scale within a few stages).
#   * bootstrap entry discipline: the bootstrap input must come fresh from
#     the rescale ladder's canonical scale. The CNN's batch-norm bias
#     position (a cipher subtract right before the main refresh) degrades
#     that refresh from ~1e-7 to ~6e-3 slot error - the degradation the
#     SDK's own apps/resnet20 masks with its 1e-3 bootstrap tolerance; the
#     final logits are verified with a 2.0 tolerance.
#
# Stages can be selected individually with --stage for inspection (the tool
# reuses a prepared context across runs).

import argparse
import os
import subprocess
import sys

CWD = '/app'

FHE_ROOT = os.path.join(CWD, 'baselines', 'fpga')
BUILD_DIR = os.path.join(FHE_ROOT, 'build', 'cnn-ckks')
CNN_EXE = os.path.join(BUILD_DIR, 'cnn')
ANT_TOOL = os.path.join(FHE_ROOT, 'clib-sdk', 'tools', 'clib-ant-tool')

# clib material profile for the CNN: N=65536 (32768 slots, logN=16) and the
# maximum supported even Q of 62 (the staged infer_seal.cpp trims the chain
# accordingly; a chain of 64 limbs is rejected by the driver).
RING_DEGREE = 65536
Q_LIMBS = 62

STAGES = ('export', 'context', 'encode', 'run', 'verify')


def run(cmd, cwd=None, env=None):
    print(f"\n$ {' '.join(cmd)}")
    ret = subprocess.run(cmd, cwd=cwd, env=env)
    assert ret.returncode == 0, f"command failed: {' '.join(cmd)}"


def cnn_env(mode, work, output, layers, cifar, start, end):
    """Environment for the staged cnn driver (see run/run_cnn.cpp)."""
    env = dict(os.environ)
    env.update({
        'CLIB_MODE': mode,
        'CLIB_N': str(RING_DEGREE),
        'CLIB_Q': str(Q_LIMBS),
        'CLIB_WORK': work,
        'CLIB_OUTPUT': output,
        'CNN_LAYERS': str(layers),
        'CNN_DATASET': str(cifar),
        'CNN_START': str(start),
        'CNN_END': str(end),
        'OMP_WAIT_POLICY': 'PASSIVE',
    })
    return env


def main():
    parser = argparse.ArgumentParser(
        description='Run FHE-MP-CNN on the real clib backend '
                    '(export -> ANT context -> ANT encode -> compute -> verify)')
    parser.add_argument('-n', '--layers', type=int, default=20,
                        help='ResNet depth: 20/32/44/56/110')
    parser.add_argument('-c', '--cifar', type=int, default=10,
                        help='Dataset: 10 or 100')
    parser.add_argument('-s', '--start', type=int, default=0,
                        help='start image id (0-9999)')
    parser.add_argument('-e', '--end', type=int, default=0,
                        help='end image id (0-9999)')
    parser.add_argument('--stage', choices=STAGES, default='verify',
                        help='run stages cumulatively up to and including this one')
    args = parser.parse_args()
    if not (args.cifar == 100 and args.layers == 32) and args.cifar not in (10, 100):
        raise SystemExit('dataset must be 10 or 100')
    if args.cifar == 100 and args.layers != 32:
        raise SystemExit('CIFAR-100 is supported for ResNet-32 only')

    if not os.path.exists(CNN_EXE):
        raise SystemExit(f'{CNN_EXE} does not exist! Please run scripts/build_cnn_clib.py first.')

    case = f'resnet{args.layers}_cifar{args.cifar}'
    base = os.path.join(FHE_ROOT, 'build', 'cnn-ckks-clib', case)
    work = os.path.join(base, 'work')
    output = os.path.join(base, 'result')
    os.makedirs(base, exist_ok=True)

    for stage in STAGES[:STAGES.index(args.stage) + 1]:
        if stage in ('export', 'run'):
            mode = stage if stage == 'export' else 'run'
            run([CNN_EXE], cwd=BUILD_DIR,
                env=cnn_env(mode, work, output, args.layers, args.cifar, args.start, args.end))
        elif stage == 'context':
            run([ANT_TOOL, 'application-context', '--work', work])
        elif stage == 'encode':
            run([ANT_TOOL, 'application-encode', '--work', work])
        elif stage == 'verify':
            run([ANT_TOOL, 'application-verify', '--work', work, '--output', output])
        print(f"[{stage}] done")
    print(f"\nwork dir: {work}\noutput dir: {output}")


if __name__ == '__main__':
    main()