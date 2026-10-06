#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
import sys
import signal
import argparse
import datetime
from ace_util import *  # noqa F403


def run_single(exec_cmd, indent_size, log, trace):
    '''
    Run single encrypted computation

    Args:
        exec_cmd(list[str]): command to run
        indent_size(int): size of indent to write in log
        log(file): log file
        trace(bool): if runtime trace is enabled
    '''
    indent = ' ' * indent_size
    os.environ["RTLIB_BTS_EVEN_POLY"] = "1"
    if trace:
        os.environ["RTLIB_TIMING_OUTPUT"] = "stdout"
    ret = run_cmd(exec_cmd)
    msg = ret.stdout.decode().splitlines()
    err = ret.stderr.decode().splitlines()
    if ret.returncode == 0:
        time, memory = time_and_memory(err[-1])
        accuracy = 0.0
        for line in msg:
            if line.find('[RESULT]') == -1:
                continue
            item = line.split(' ')
            accuracy = float(item[6].strip().split(',')[0].strip()) * 100
        info = '%sExec: Time = %.1f(s), Memory = %.1f(GB), Accuracy = %.1f%%\n' % (
            indent, time, memory, accuracy)
        write_log(info, log)
        if trace:
            indent = ' ' * (indent_size + 6)
            for item in msg:
                write_log(indent + item + '\n', log)
    else:
        info = 'Exec: failed'
        if ret.returncode > 128:
            info += ' due to ' + signal.Signals(ret.returncode - 128).name
        info += '\n'
        write_log(info, log)
        info = ' '.join(exec_cmd) + '\n'
        write_log(info, log)
        for item in msg:
            write_log(item + '\n', log)
        for item in err:
            write_log(item + '\n', log)
    return


def run_perf(exec_files, cifar10_dir, cifar100_dir, index, debug, trace, log):
    '''
    Main function to run encrypted computations in serial

    Args:
        exec_files(list[str]): encrypted executables to run
        cifar10_dir(str): path of the dir that holds the cifar10 data file
        cifar100_dir(str): path of the dir that holds the cifar100 data file
        index(int): starting index in the cifar data set
        debug(bool): print out debug info
        trace(bool): if runtime trace is enabled
        log(file): log file
    '''
    info = '-------- ACE Performance --------\n'
    write_log(info, log)
    start_time = datetime.datetime.now()
    for exec_file in exec_files:
        exec_file = os.path.abspath(exec_file)
        if not os.path.exists(exec_file):
            print(exec_file, 'does not exist!')
            sys.exit(-1)
        cmds = ['time', '-f', '\"%e %M\"', exec_file]
        test = get_test_name(exec_file)
        cifar_file = get_cifar_option(test, cifar10_dir, cifar100_dir)
        if cifar_file is not None:
            cmds.extend([cifar_file, str(index), str(index)])
        rq_mem = get_exec_mem(test)
        if not check_required_memory(rq_mem):
            print('Not enough memory, %s requires %s GB' % (test, rq_mem))
            sys.exit(-1)
        info = test + ':\n'
        write_log(info, log)
        if debug:
            print(' '.join(cmds))
        # run tests in serial
        run_single(cmds, len(test) + 2, log, trace)
    end_time = datetime.datetime.now()
    spend_time = (end_time - start_time).total_seconds()
    info = 'Spent time = %.2f(s)\n' % spend_time
    info += '-------- Done --------\n'
    write_log(info, log)
    return


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Run encrypted computations in serial for performance data',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-cf10', '--cifar10', metavar='PATH', default='/app/cifar/cifar-10-batches-bin',
                        help='direcotry where the cifar10 data are placed')
    parser.add_argument('-cf100', '--cifar100', metavar='PATH', default='/app/cifar/cifar-100-binary',
                        help='direcotry where the cifar100 data are placed')
    parser.add_argument('-d', '--debug', action='store_true', default=False,
                        help='print out debug info')
    parser.add_argument('-e', '--exec', metavar='PATH', nargs='+',
                        help='path of executables of encrypted computations')
    parser.add_argument('-i', '--index', type=int, default=0,
                        help='index of the starting image to inference, range: [0, 9999]')
    parser.add_argument('-t', '--trace', action='store_false', default=True,
                        help='print out trace info into log file')
    args = parser.parse_args()
    cwd = os.getcwd()
    date_time = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M")
    index = str(args.index)
    log_file_name = date_time + '.perf.' + index + '.log'
    log = open(os.path.join(cwd, log_file_name), 'w')
    info = '#### log for: %s\n' % (' '.join(sys.argv))
    write_log(info, log)
    run_perf(args.exec, args.cifar10, args.cifar100, args.index, args.debug, args.trace, log)
    log.close()