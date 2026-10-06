#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
import sys
import argparse
from ace_util import REQUIRED_MEMORY

CMPLR = 'Compile'
PERF = 'Perf'
ACC = 'Accuracy'
DATA_SEC_MAP = {
    '-------- ACE Compilation --------' : CMPLR,
    '-------- ACE Performance --------' : PERF,
    '-------- ACE Accuracy --------' : ACC,
}
DATA_SEC_END = '-------- Done --------'
CMPLR_DATA='ACE:'
PERF_DATA='Exec:'
KEY_DATA='Total memory size for keys:'
CONV_DATA='NN_CONV'
RELU_DATA='NN_RELU'
BTS_DATA='BOOTSTRAP'
ACC_DATA='[RESULT]'
INFER_DATA='[INFO]'

TIME='time'
MEMORY='memory'
KEYMEMORYCNT='rot_key_cnt'
KEYMEMORY='rot_key_mem'
CONV_CNT='conv_cnt'
CONV_TIME='conv_time'
RELU_CNT='relu_cnt'
RELU_TIME='relu_time'
BTS_CNT='bts_cnt'
BTS_TIME='bts_time'
ACC_RATE='acc_rate'
ACC_CNT='image_num'
INFER_FAIL='fail_list'
INFER_SUCC='succ_list'

TIME_DIFF_THOLD=5.0
MEMORY_DIFF_THOLD=5.0

def start_data_section(line):
    '''
    Return if the line indicate the start of a data section for
    compilation, performance or accuracy
    '''
    section = None
    for sec in DATA_SEC_MAP:
        if line.find(sec) != -1:
            section = DATA_SEC_MAP[sec]
            break
    return section


def end_data_section(line):
    '''
    Return if the line indicate the end of a data section
    '''
    if line.find(DATA_SEC_END) != -1:
        return True
    return False


def start_test_data(line):
    '''
    Return if the line indicate the start of the data for a specific test
    '''
    test = None
    items = line.strip().split(':')
    if items[0] in REQUIRED_MEMORY:
        test = items[0]
    return test


def process_perf_data(line, test_result):
    '''
    Process performance data like exection time & memory, rotation key count & memory,
    convolution count & time, relu count & time, bootstrapping count & time.
    The performance section is organized as:
    -------- ACE Performance --------
    resnet20_cifar10:
                      Exec: Time = 1457.0(s), Memory = 42.5(GB)
                            ...
                            Total memory size for keys: rot_key_cnt = 227, rot_key_size = ...
                            ...
                            RTLib functions         	       Count	      Elapse
                            --------------------    	    --------	    --------
                            ...
                             NN_CONV                 	          21	  158.957066 sec
                            ...
                             NN_RELU                 	          19	 1049.186360 sec
                            ...
                             BOOTSTRAP               	          17	  823.326756 sec
                            ...
    resnet32_cifar10:
                      ...
    -------- Done --------
    '''
    if line.find(PERF_DATA) != -1:
        items = line.strip().split('=')
        exec_time = items[1].strip().split('(')[0]
        exec_mem = items[2].strip().split('(')[0]
        test_result[TIME] = float(exec_time)
        test_result[MEMORY] = float(exec_mem)
    elif line.find(KEY_DATA) != -1:
        items = line.strip().split('=')
        rot_key_cnt = items[1].strip().split(',')[0]
        test_result[KEYMEMORYCNT] = int(rot_key_cnt)
        rot_key = items[2].strip().split()[0]
        test_result[KEYMEMORY] = round(float(rot_key) / 1000000000, 1)
    elif line.find(CONV_DATA) != -1:
        items = line.strip().split()
        conv_cnt = items[1].strip()
        conv_time = items[2].strip()
        test_result[CONV_CNT] = int(conv_cnt)
        test_result[CONV_TIME] = round(float(conv_time), 2)
    elif line.find(RELU_DATA) != -1:
        items = line.strip().split()
        relu_cnt = items[1].strip()
        relu_time = items[2].strip()
        test_result[RELU_CNT] = int(relu_cnt)
        test_result[RELU_TIME] = round(float(relu_time), 2)
    elif line.find(BTS_DATA) != -1:
        items = line.strip().split()
        bts_cnt = items[1].strip()
        if bts_cnt.isdigit():
            bts_time = items[2].strip()
            test_result[BTS_CNT] = int(bts_cnt)
            test_result[BTS_TIME] = round(float(bts_time), 2)
    return


def process_acc_data(line, test_result):
    '''
    Process accuracy data, maintain inference success/fail results 
    if there are detailed info.
    The accuracy section is organized as:
    -------- ACE Accuracy --------
    resnet20_cifar10: Accuracy = 100.0%, ...
                      ...
                      [INFO] infer image 0 success
                      ...
                      [RESULT] infer 10 images, ...
    resnet32_cifar10: ...
    -------- Done --------
    '''
    if line.find(INFER_DATA) != -1:
        items = line.split()
        image_num = int(items[3].strip())
        if items[4] != 'success':
            if image_num not in test_result[INFER_FAIL]:
                test_result[INFER_FAIL].append(image_num)
            if image_num in test_result[INFER_SUCC]:
                print('Conflict infer result for image:', image_num)
                sys.exit(-1)
        else:
            if image_num not in test_result[INFER_SUCC]:
                test_result[INFER_SUCC].append(image_num)
            if image_num in test_result[INFER_FAIL]:
                print('Conflict infer result for image:', image_num)
                sys.exit(-1)
        succ_list = test_result[INFER_SUCC]
        fail_list = test_result[INFER_FAIL]
        succ_list.sort()
        fail_list.sort()
        image_cnt = len(succ_list) + len(fail_list)
        acc = round(float(len(succ_list)) * 100 / image_cnt, 1)
        test_result[ACC_RATE] = acc
        test_result[ACC_CNT] = image_cnt
    return


def process_ace_log(log, res):
    '''
    Process ace compilation/performance/accuracy info from log file
    The compilation section is organized as:
    -------- ACE Compilation --------
    resnet20_cifar10:
                      ACE: Time = 0.62(s), Memory = 0.46(GB)
                      GCC: Time = 11.77(s), Memory = 0.60(GB)
    resnet32_cifar10:
                      ACE: Time = 0.89(s), Memory = 0.69(GB)
                      ...
    -------- Done --------
    '''
    section = None
    test = None
    for line in log:
        if section is None:
            section = start_data_section(line)
            if section is not None:
                if res.get(section) is None:
                    res[section] = {}
        elif end_data_section(line):
            section = None
            test = None
        elif section == CMPLR:
            new_test = start_test_data(line)
            if new_test is not None:
                test = new_test
                if res[section].get(test) is not None:
                    print('Multiple compilation results for', test)
                    sys.exit(-1)
                res[section][test] = {}
            elif line.find(CMPLR_DATA) != -1:
                cmplr_time = line.strip().split('=')[1].strip().split('(')[0]
                res[section][test][TIME] = float(cmplr_time)
        elif section == PERF:
            new_test = start_test_data(line)
            if new_test is not None:
                test = new_test
                if res[section].get(test) is not None:
                    print('Multiple performance results for', test)
                    sys.exit(-1)
                res[section][test] = {}
            elif test is not None:
                process_perf_data(line, res[section][test])
        elif section == ACC:
            new_test = start_test_data(line)
            if new_test is not None:
                test = new_test
                if res[section].get(test) is None:
                    res[section][test] = {}
                    res[section][test][INFER_SUCC] = []
                    res[section][test][INFER_FAIL] = []
                items = line.strip().split('=')
                acc = items[1].split('%')[0]
                res[section][test][ACC_RATE] = float(acc)
            elif test is not None:
                process_acc_data(line, res[section][test])
    return


def compare_cmplr(test, base, cur):
    '''
    Compare the compilation data of a test to a baseline
    '''
    has_diff = False
    base_time = base[TIME]
    cur_time = cur[TIME]
    time_diff = (cur_time - base_time) * 100 / base_time
    if abs(time_diff) > TIME_DIFF_THOLD:
        has_diff = True
        print('%s: cmplr_time(%+.1f%%)' % (test, time_diff))
    return has_diff


def compare_perf(test, base, cur):
    '''
    Compare the performance data of a test to a baseline
    '''
    has_diff = False

    base_time = base[TIME]
    cur_time = cur[TIME]
    time_diff = (cur_time - base_time) * 100 / base_time
    if abs(time_diff) > TIME_DIFF_THOLD:
        has_diff = True

    base_mem = base[MEMORY]
    cur_mem = cur[MEMORY]
    mem_diff = (cur_mem - base_mem) * 100 / base_mem
    if abs(mem_diff) > MEMORY_DIFF_THOLD:
        has_diff = True

    base_rot_cnt = base[KEYMEMORYCNT]
    cur_rot_cnt = cur[KEYMEMORYCNT]
    rot_cnt_diff = cur_rot_cnt - base_rot_cnt
    if rot_cnt_diff != 0:
        has_diff = True

    base_rot_mem = base[KEYMEMORY]
    cur_rot_mem = cur[KEYMEMORY]
    rot_mem_diff = (cur_rot_mem - base_rot_mem) * 100 / base_rot_mem
    if abs(rot_mem_diff) > MEMORY_DIFF_THOLD:
        has_diff = True

    base_conv_cnt = base[CONV_CNT]
    cur_conv_cnt = cur[CONV_CNT]
    conv_cnt_diff = cur_conv_cnt - base_conv_cnt
    if conv_cnt_diff != 0:
        has_diff = True

    base_conv_time = base[CONV_TIME]
    cur_conv_time = cur[CONV_TIME]
    conv_time_diff = (cur_conv_time - base_conv_time) * 100 / base_conv_time
    if abs(conv_time_diff) > TIME_DIFF_THOLD:
        has_diff = True

    base_relu_cnt = base[RELU_CNT]
    cur_relu_cnt = cur[RELU_CNT]
    relu_cnt_diff = cur_relu_cnt - base_relu_cnt
    if relu_cnt_diff != 0:
        has_diff = True

    base_relu_time = base[RELU_TIME]
    cur_relu_time = cur[RELU_TIME]
    relu_time_diff = (cur_relu_time - base_relu_time) * 100 / base_relu_time
    if abs(relu_time_diff) > TIME_DIFF_THOLD:
        has_diff = True

    base_bts_cnt = base[BTS_CNT]
    cur_bts_cnt = cur[BTS_CNT]
    bts_cnt_diff = cur_bts_cnt - base_bts_cnt
    if bts_cnt_diff != 0:
        has_diff = True

    base_bts_time = base[BTS_TIME]
    cur_bts_time = cur[BTS_TIME]
    bts_time_diff = (cur_bts_time - base_bts_time) * 100 / base_bts_time
    if abs(bts_time_diff) > TIME_DIFF_THOLD:
        has_diff = True

    if has_diff:
        info = '%s:' % test
        if abs(time_diff) > TIME_DIFF_THOLD:
            info += ' exec_time(%+.1f%%)' % time_diff
        if abs(mem_diff) > MEMORY_DIFF_THOLD:
            info += ' exec_mem(%+.1f%%)' % mem_diff
        if rot_cnt_diff != 0:
            info += ' rot_key_cnt(%+d)' % rot_cnt_diff
        if abs(rot_mem_diff) > MEMORY_DIFF_THOLD:
            info += ' rot_key_mem(%+.1f%%)' % rot_mem_diff
        if conv_cnt_diff != 0:
            info += ' conv_cnt(%+d)' % conv_cnt_diff
        if abs(conv_time_diff) > TIME_DIFF_THOLD:
            info += ' conv_time(%+.1f%%)' % conv_time_diff
        if relu_cnt_diff != 0:
            info += ' relu_cnt(%+d)' % relu_cnt_diff
        if abs(relu_time_diff) > TIME_DIFF_THOLD:
            info += ' relu_time(%+.1f%%)' % relu_time_diff
        if bts_cnt_diff != 0:
            info += ' bts_cnt(%+d)' % bts_cnt_diff
        if abs(bts_time_diff) > TIME_DIFF_THOLD:
            info += ' bts_time(%+.1f%%)' % bts_time_diff
        print(info)
    return has_diff


def compare_accuracy(test, base_res, cur_res):
    '''
    Compare the accuracy data of a test to a baseline
    '''
    has_diff = False
    cur_acc = cur_res[ACC_RATE]
    base_acc = base_res[ACC_RATE]
    succ_info = 'success+('
    has_succ = False
    for i in cur_res[INFER_SUCC]:
        if i not in base_res[INFER_SUCC]:
            if not has_succ:
                has_succ = True
            else:
                succ_info += ','
            succ_info += '%d' % i
    if has_succ:
        succ_info += ')'
    fail_info = 'fail+('
    has_fail = False
    for i in cur_res[INFER_FAIL]:
        if i not in base_res[INFER_FAIL]:
            if not has_fail:
                has_fail = True
            else:
                fail_info += ','
            fail_info += '%d' % i
    if has_fail:
        fail_info += ')'
    info = None
    if cur_acc != base_acc or has_succ or has_fail:
        info = '%s: accuracy(%+.1f%%)' % (test, cur_acc - base_acc)
        has_diff = True
    if has_succ:
        info += ', %s' % succ_info
    if has_fail:
        info += ', %s' % fail_info
    if info is not None:
        print(info)
    return has_diff


def compare_result(base, cur):
    '''
    Compare current results to baseline, return True if there's difference
    '''
    has_diff = False
    for phase in cur:
        if base.get(phase) is None:
            print('No \'%s\' data in baseline' % phase)
            has_diff = True
            continue
        for test in cur[phase]:
            if base[phase].get(test) is None:
                print('No \'%s::%s\' data in baseline' % (phase, test))
                has_diff = True
                continue
            cur_res = cur[phase][test]
            base_res = base[phase][test]
            if phase == CMPLR:
                has_diff = compare_cmplr(test, base_res, cur_res)
            elif phase == PERF:
                has_diff = compare_perf(test, base_res, cur_res)
            elif phase == ACC:
                has_diff = compare_accuracy(test, base_res, cur_res)
    return has_diff


def result_diff(base, cur):
    base_res = {}
    for log_file in base:
        if not os.path.exists(log_file):
            print(log_file, 'does not exist')
            sys.exit(-1)
        log = open(log_file, 'r')
        process_ace_log(log, base_res)
        log.close()
    cur_res = {}
    for log_file in cur:
        if not os.path.exists(log_file):
            print(log_file, 'does not exist')
            sys.exit(-1)
        log = open(log_file, 'r')
        process_ace_log(log, cur_res)
        log.close()
    return compare_result(base_res, cur_res)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Compare ACE performance / accuracy test results',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-b', '--base', metavar='PATH', nargs='+',
                        help='test logs of baseline results')
    parser.add_argument('-c', '--cur', metavar='PATH', nargs='+',
                        help='test logs of current results')
    args = parser.parse_args()
    if result_diff(args.base, args.cur):
        sys.exit(-1)
    else:
        sys.exit(0)
