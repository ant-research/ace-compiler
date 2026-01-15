#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
import sys
import argparse
import datetime
from ace_util import write_log
from ace_compile import run_ace_compile
from run_perf import run_perf
from run_accuracy import run_accuracy


def main(cwd, args, log):
    '''
    Main function to run ACE compiler E2E testing

    Args:
        cwd(str): current working directory
        args(Namespace): arguments needed to run
        log(file): log file
    '''
    exec_files = run_ace_compile(cwd, args.file, args.model, args.build, args.cmplr, args.src,
                                 args.paper, args.lib, args.extra, args.omp, args.keep,
                                 args.debug, args.trace, log)
    if args.acc:
        run_accuracy(exec_files, args.cifar10, args.cifar100,
                     args.index, args.num, args.debug, args.trace, log)
    else:
        run_perf(exec_files, args.cifar10, args.cifar100, args.index, args.debug, args.trace, log)
    return


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Run performance and/or accuracy tests for ACE Compiler',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-a', '--acc', action='store_true', default=False,
                        help='run accuracy tests with specified ACE compiler')
    parser.add_argument('-b', '--build', metavar='PATH',
                        help='directory where the ACE compiler is built')
    parser.add_argument('-c', '--cmplr', metavar='PATH',
                        help='installation directory of the ACE compiler')
    parser.add_argument('-cf10', '--cifar10', metavar='PATH', default='./cifar-10-batches-bin',
                        help='direcotry where the cifar10 data are placed')
    parser.add_argument('-cf100', '--cifar100', metavar='PATH', default='./cifar-100-binary',
                        help='direcotry where the cifar100 data are placed')
    parser.add_argument('-d', '--debug', action='store_true', default=False,
                        help='print out debug info')
    parser.add_argument('-e', '--extra', metavar='OPTIONS', nargs='*',
                        help='extra ACE compilation options')
    parser.add_argument('-f', '--file', metavar='PATH',
                        help='run single ONNX model test')
    parser.add_argument('-i', '--index', type=int, default=0,
                        help='index of the starting image to infer, range: [0, 9999]')
    parser.add_argument('-k', '--keep', action='store_true', default=False,
                        help='keep files generated in the test')
    parser.add_argument('-l', '--lib', choices=['ant', 'seal', 'openfhe'], default='ant',
                        help='target library to link')
    parser.add_argument('-m', '--model', metavar='PATH', default='./model',
                        help='directory holds the onnx models to run')
    parser.add_argument('-n', '--num', type=int, default=10,
                        help='number of images to run for each model, ranges: [1, 10000]')
    parser.add_argument('-o', '--omp', action='store_true', default=False,
                        help='if it is an OpenMP version of ACE compiler')
    parser.add_argument('-p', '--paper', choices=['cgo25', 'asplos25'],
                        help='configuration to reproduce paper results')
    parser.add_argument('-s', '--src', metavar='PATH', default='./',
                        help='directory that holds repos of air-infra, fhe-cmplr & nn-addon')
    parser.add_argument('-t', '--trace', action='store_false', default=True,
                        help='print out trace info into log file')
    args = parser.parse_args()
    cwd = os.getcwd()
    date_time = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M")
    log_file_name = date_time + '.ace.log'
    log = open(os.path.join(cwd, log_file_name), 'w')
    info = '#### log for: %s\n' % (' '.join(sys.argv))
    write_log(info, log)
    main(cwd, args, log)
    log.close()
