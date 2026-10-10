#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Run the FHE-MP-CNN `cnn` executable built against the clib SDK
# (scripts/build_cnn_clib.py -> baselines/fpga/build/cnn-ckks/cnn).
#
# This mirrors the "Expert" suite of scripts/perf_gpu.py: it drives the CNN
# program over the supported ResNet/CIFAR configurations and logs per-op and
# total inference times, parsed from the cnn program's stdout. Each run's
# exit status is checked, so a crashed configuration is reported as e.g.
# "failed due to SIGABRT" instead of skewing the numbers.

import argparse
import datetime
import os
import signal
import subprocess
import sys

CWD = '/app'

FHE_ROOT = os.path.join(CWD, 'baselines', 'fpga')
BUILD_DIR = os.path.join(FHE_ROOT, 'build', 'cnn-ckks')

# Supported configurations, mirroring the FHE-MP-CNN README:
# CIFAR-10 -> ResNet-20/32/44/56/110 ; CIFAR-100 -> ResNet-32.
CASES = [(rn, cfar) for rn in (20, 32, 44, 56, 110) for cfar in (10, 100)
         if not (cfar == 100 and rn != 32)]

# stdout section markers, as printed by infer_seal.cpp.
CONV, BTS, RELU = 'Tensor::conv', 'FHE::bootstrap', 'FHE::relu'


def write_log(info, log):
    """Print a line to the console and append it to the log file."""
    print(info, end='')
    log.write(info)
    log.flush()


def parse_output(msg):
    """Extract per-op timings from the cnn stdout, mirroring perf_gpu.py.

    The stdout is a sequence of sections ("...convolution...", "...ReLU...",
    "...bootstrapping..."), each closed by a "time : <val> ms" line. Sum
    every such line into the op class of the section it closes.
    """
    op_time = {}
    op_cnt = {}
    in_conv = in_bts = in_relu = False
    for line in msg:
        if 'multiplexed parallel convolution...' in line or \
           'multiplexed parallel downsampling...' in line:
            in_conv, in_relu, in_bts = True, False, False
        elif 'approximate ReLU...' in line:
            in_conv, in_relu, in_bts = False, True, False
        elif 'bootstrapping...' in line:
            in_conv, in_relu, in_bts = False, False, True
        elif line.find('time') != -1 and ':' in line:
            try:
                time_val = float(line.split(':')[1].strip().split(' ')[0])
            except (ValueError, IndexError):
                continue
            key = CONV if in_conv else RELU if in_relu else BTS if in_bts else None
            if key is not None:
                op_time[key] = op_time.get(key, 0.0) + time_val
                op_cnt[key] = op_cnt.get(key, 0) + 1
                in_conv = in_relu = in_bts = False
    return op_time, op_cnt


def run_case(kpath, rn, cfar, log, debug):
    """Run one ResNet/CIFAR configuration; log timings on success."""
    # The /usr/bin/time wrapper mirrors perf_gpu.py; its report goes to
    # stderr, which is discarded — the timings come from stdout instead.
    cmds = ['time', '-f', '%e %M', './cnn', str(rn), str(cfar), '0', '0']
    case = f'resnet{rn}_cifar{cfar}'
    if debug:
        print(' '.join(cmds))
    ret = subprocess.run(cmds, cwd=kpath, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if ret.returncode == 0:
        msg = ret.stdout.decode(errors='replace').splitlines()
        op_time, op_cnt = parse_output(msg)
        write_log(f'{case}:\n', log)
        for key in (CONV, RELU, BTS):
            if key in op_time:
                write_log(f'  {key:16s}: {op_time[key]:10.3f} ms over {op_cnt[key]} ops\n', log)
        return True
    info = case + ': failed'
    if ret.returncode > 128:
        info += ' due to ' + signal.Signals(ret.returncode - 128).name
    info += '\n'
    write_log(info, log)
    return False


def main():
    parser = argparse.ArgumentParser(description='Run FHE-MP-CNN (clib-sdk backend) performance data')
    parser.add_argument('-k', '--kpath', metavar='PATH',
                        help='Path to the build directory containing the cnn executable')
    parser.add_argument('-d', '--debug', action='store_true', default=False,
                        help='Print out debug info')
    args = parser.parse_args()

    kpath = BUILD_DIR
    if args.kpath is not None:
        kpath = os.path.abspath(args.kpath)
    if not os.path.exists(os.path.join(kpath, 'cnn')):
        print(f'{kpath}/cnn does not exist! Please run scripts/build_cnn_clib.py first.')
        sys.exit(-1)

    date_time = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M")
    log = open(os.path.join(CWD, date_time + '.log'), 'w')

    write_log('-------- FHE-MP-CNN (clib-sdk) --------\n', log)
    for rn, cfar in CASES:
        run_case(kpath, rn, cfar, log, args.debug)
    write_log('-------- Done --------\n', log)
    log.close()


if __name__ == '__main__':
    main()