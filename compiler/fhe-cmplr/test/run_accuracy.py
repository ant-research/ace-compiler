#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
import sys
import argparse
import datetime
from multiprocessing import Pool
from ace_util import *  # noqa F403


def run_parallel(all_cmds, image_num, log, trace):
    '''
    Run encrypted computations in parallel with given image numbers

    Args:
        all_cmds(list[list[str]]): all commands to run
        image_num(int): number of images to test
        log(file): log file
        trace(bool): if runtime trace is enabled
    '''
    test_set = [i[3] for i in all_cmds]
    num_cpus, num_process = calculate_process_num(image_num, test_set, log)
    total_time = 0.0
    os.environ["RTLIB_BTS_EVEN_POLY"] = "1"
    os.environ["RT_BTS_CLEAR_IMAG"] = "1"
    os.environ["OMP_NUM_THREADS"] = str(num_cpus)
    start_time = datetime.datetime.now()
    with Pool(num_process) as p:
        results = p.map(run_cmd, all_cmds)
        p.close()
        p.join()
    end_time = datetime.datetime.now()
    spend_time = (end_time - start_time).total_seconds()
    for ret in results:
        accuracy = 0.0
        test = get_test_name(ret.args[3])
        indent = ' ' * (len(test) + 2)
        msg = ret.stdout.decode().splitlines()
        err = ret.stderr.decode().splitlines()
        if ret.returncode == 0:
            time, memory = time_and_memory(err[-1])
            total_time += time
            for line in msg:
                if line.find('[RESULT]') == -1:
                    continue
                item = line.split(' ')
                accuracy = float(item[6].strip().split(',')[0].strip()) * 100
            info = '%s: Accuracy = %.1f%%, Time = %.2f(s), Memory = %.1f(GB)\n' % (
                test, accuracy, time, memory)
            write_log(info, log)
            if trace:
                for item in msg:
                    write_log(indent + item + '\n', log)
        else:
            info = '%s: Exec failed\n' % test
            write_log(info, log)
            info = ' '.join(ret.args) + '\n'
            write_log(info, log)
            for item in msg:
                write_log(item + '\n', log)
            for item in err:
                write_log(item + '\n', log)
    info = 'Spent time = %.2f(s), Accumulated time = %.2f(s)\n' % (spend_time, total_time)
    write_log(info, log)
    return


def run_accuracy(exec_files, cifar10_dir, cifar100_dir, index, image_num, debug, trace, log):
    '''
    Main function to run encrypted computations in parallel

    Args:
        exec_files(list[str]): encrypted executables to run
        cifar10_dir(str): path of the dir that holds the cifar10 data file
        cifar100_dir(str): path of the dir that holds the cifar100 data file
        index(int): starting index in the cifar data set
        image_num(int): number of images to test
        debug(bool): print out debug info
        trace(bool): if runtime trace is enabled
        log(file): log file
    '''
    info = '-------- ACE Accuracy --------\n'
    write_log(info, log)
    all_cmds = []
    for exec_file in exec_files:
        exec_file = os.path.abspath(exec_file)
        if not os.path.exists(exec_file):
            print(exec_file, 'does not exist!')
            sys.exit(-1)
        cmds = ['time', '-f', '\"%e %M\"', exec_file]
        test = get_test_name(exec_file)
        cifar_file = get_cifar_option(test, cifar10_dir, cifar100_dir)
        if cifar_file is not None:
            start_idx = str(index)
            end_idx = str(index + image_num - 1)
            cmds.extend([cifar_file, start_idx, end_idx])
        all_cmds.append(cmds)
    if debug:
        print('Commands to run in parallel:')
        for cmd in all_cmds:
            print(' '.join(cmd))
    # run tests in parallel
    run_parallel(all_cmds, image_num, log, trace)
    info = '-------- Done --------\n'
    write_log(info, log)
    return


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Run encrypted computations in parallel for accuracy data',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-cf10', '--cifar10', metavar='PATH', default='./cifar-10-batches-bin',
                        help='direcotry where the cifar10 data are placed')
    parser.add_argument('-cf100', '--cifar100', metavar='PATH', default='./cifar-100-binary',
                        help='direcotry where the cifar100 data are placed')
    parser.add_argument('-d', '--debug', action='store_true', default=False,
                        help='print out debug info')
    parser.add_argument('-e', '--exec', metavar='PATH', nargs='+',
                        help='path of executables of encrypted computations')
    parser.add_argument('-i', '--index', type=int, default=0,
                        help='index of the starting image to inference, range: [0, 9999]')
    parser.add_argument('-n', '--num', type=int, default=10,
                        help='number of images to run for each model, ranges: [1, 10000]')
    parser.add_argument('-t', '--trace', action='store_false', default=True,
                        help='print out trace info into log file')
    args = parser.parse_args()
    cwd = os.getcwd()
    date_time = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M")
    start_idx = str(args.index)
    end_idx = str(args.index + args.num - 1)
    log_file_name = date_time + '.acc.' + start_idx + '.' + end_idx + '.log'
    log = open(os.path.join(cwd, log_file_name), 'w')
    info = '#### log for: %s\n' % (' '.join(sys.argv))
    write_log(info, log)
    run_accuracy(args.exec, args.cifar10, args.cifar100,
                 args.index, args.num, args.debug, args.trace, log)
    log.close()
