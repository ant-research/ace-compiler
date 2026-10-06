#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
import sys
import argparse
import subprocess
import datetime
import signal
import re

#onnx model name
MODELS = ['ResNet-20',
          'ResNet-32',
          'ResNet-32*',
          'ResNet-44',
          'ResNet-56',
          'ResNet-110']
INDEXES = ['resnet20_cifar10',
           'resnet32_cifar10',
           'resnet32_cifar100',
           'resnet44_cifar10',
           'resnet56_cifar10',
           'resnet110_cifar10']
ONNX_FILES = ['resnet20_cifar10_pre.onnx',
              'resnet32_cifar10_pre.onnx',
              'resnet32_cifar100_pre.onnx',
              'resnet44_cifar10_pre.onnx',
              'resnet56_cifar10_pre.onnx',
              'resnet110_cifar10_pre.onnx']

def find_index_from_onnx(onnx_file, indexes):
    potential_index = onnx_file.split('_pre.onnx')[0].split('_train.onnx')[0]

    if potential_index in indexes:
        return potential_index
    else:
        return None

def write_log(info, log):
    print(info[:-1])
    log.write(info)
    log.flush()
    return

def time_and_memory(outputs):
    matches = re.findall(r'"([^"]*)"', outputs)
    if matches:
        result = matches[-1].split()
    return result[0], result[1]

def check_required_memory(rq_mem):
    mem_info_file = '/proc/meminfo'
    # If we don't know, run anyway
    if not os.path.exists(mem_info_file):
        print(mem_info_file, 'does not exist!')
        return True
    mem_info = open(mem_info_file, 'r')
    total_mem = 0
    free_mem = 0
    for line in mem_info:
        if line.find('MemTotal') != -1:
            total_mem = int(line.split(':')[1].strip().split(' ')[0].strip())
            total_mem /= 1000000
        elif line.find('MemFree') != -1:
            free_mem = int(line.split(':')[1].strip().split(' ')[0].strip())
            free_mem /= 1000000
    mem_info.close()
    if total_mem > rq_mem:
        if free_mem < rq_mem:
            print('Warning: Not enough free memory,', rq_mem, 'GB is required')
            return False
        return True
    print('Warning: Not enough memory,', rq_mem, 'GB is required')
    return False

def test_perf_fhe_mp_cnn(kpath, log, debug):
    os.chdir(kpath)
    info = '-------- Expert --------\n'
    write_log(info, log)
    for rn in [20, 32, 44, 56, 110]:
        for cfar in [10, 100]:
            if cfar == 100 and rn != 32:
                continue
            cmds = ['time', '-f', '\"%e %M\"', './cnn_depth_30', str(rn), str(cfar), '0', '0']
            # import pdb; pdb.set_trace()
            if debug:
                print(' '.join(cmds))
            ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            case = 'resnet' + str(rn) + '_cifar' + str(cfar) + '_expert'
            info = case + ':'
            write_log(info, log)
            if ret.returncode == 0:
                time, memory = time_and_memory(ret.stderr.decode().splitlines()[0])
                msg = ret.stdout.decode().splitlines()
                in_conv = False
                in_bts = False
                in_relu = False
                CONV='Tensor::conv'
                BTS='FHE::bootstrap'
                RELU='FHE::relu'
                op_time = {}
                op_cnt = {}
                key_mem = 0
                for line in msg:
                    if line.find('multiplexed parallel convolution...') != -1 or\
                       line.find('multiplexed parallel downsampling...') != -1:
                        in_conv = True
                        in_relu = False
                        in_bts = False
                    elif line.find('approximate ReLU...') != -1:
                        in_conv = False
                        in_relu = True
                        in_bts = False
                    elif line.find('bootstrapping...') != -1:
                        in_conv = False
                        in_relu = False
                        in_bts = True
                    elif line.find('KeyMemory') != -1:
                        key_mem = line.split('=')[1].split('(')[0].strip()
                    elif line.find('time') != -1:
                        time_val = line.split(':')[1].split(' ')[1].strip()
                        if in_conv:
                            if CONV in op_time:
                                op_time[CONV] += float(time_val)
                                op_cnt[CONV] += 1
                            else:
                                op_time[CONV] = float(time_val)
                                op_cnt[CONV] = 1
                            in_conv = False
                        elif in_relu:
                            if RELU in op_time:
                                op_time[RELU] += float(time_val)
                                op_cnt[RELU] += 1
                            else:
                                op_time[RELU] = float(time_val)
                                op_cnt[RELU] = 1
                            in_relu = False
                        elif in_bts:
                            if BTS in op_time:
                                op_time[BTS] += float(time_val)
                                op_cnt[BTS] += 1
                            else:
                                op_time[BTS] = float(time_val)
                                op_cnt[BTS] = 1
                            in_bts = False
                        elif line.find('all threads time :') != -1:
                            parts = line.split(':')
                            if len(parts) > 1:
                                time_str = parts[1].strip().split(' ')[0]
                                inf_time = float(time_str)
                                info = ' ' * (len(case) + 6) + 'Inference Time = '  + str("%.4f" % (inf_time/1000)) + '(s)\n'
                                write_log(info, log)
            else:
                info = case + ': failed'
                if ret.returncode > 128:
                    info += ' due to ' + signal.Signals(ret.returncode - 128).name
                info += '\n'
                write_log(info, log)
    return

# DEAD CODE — kept for reference; do NOT call.
# ace_compile_and_run_onnx (below) was the -f/--file path but references
# process_model/build_target/inference that are never imported (they live
# in ace_compile_gpu.py as compile_model/link_target with mismatched paths
# /app/fhe-cmplr vs /app/ace-compiler/fhe-cmplr). Running it raises NameError.
# The -f branch in main() now rejects cleanly. This body is intentionally
# retained (not deleted) so the historical -f intent is visible; remove it
# only if you also drop argparse -f entirely.
def ace_compile_and_run_onnx(cwd, cmplr_path, onnx_path, onnx_model, log, debug):
    script_dir = os.path.dirname(__file__)
    src_dir = '/app/fhe-cmplr/rtlib/phantom/example/'
    target_dir = '/app/ace-compiler/release_gpu/rtlib/build/example/'
    #onnx2c = os.path.join(script_dir, 'onnx2c.py')
    #if not os.path.exists(onnx2c):
    #    print(onnx2c, 'does not exist')
    #    sys.exit(-1)
    model_file = os.path.join(onnx_path, onnx_model)
    if not os.path.exists(model_file):
        print(model_file, 'does not exist')
        return
    info = onnx_model + ':\n'
    write_log(info, log)
    model_base = onnx_model.split('.')[0]
    onnx_c = os.path.join(src_dir, onnx_model + '.inc')
    exec_file = os.path.join(target_dir, 'resnet20_cifar10_gpu')
    tfile = os.path.join(cwd, model_base + '.t')
    jfile = os.path.join(cwd, model_base + '.json')
    # compile onnx
    matching_index = find_index_from_onnx(onnx_model, INDEXES)
    ret = process_model(cmplr_path, matching_index, onnx_path + '/' + onnx_model, onnx_c)
    if os.path.exists(tfile):
        os.remove(tfile)
    if os.path.exists(jfile):
        os.remove(jfile)
    if ret.returncode == 0:
        time, memory = time_and_memory(ret.stderr.decode('utf-8'))
        info = ' '*(len(onnx_model)+2) + 'Compile : Time = ' + str(time) \
            + '(s) Memory = ' + str("%.2f" % (int(memory)/1000000)) + '(Gb)\n'
        write_log(info, log)
    else:
        info = 'ACE: failed\n'
        if debug:
            info = ' '.join(cmds) + '\n'
        write_log(info, log)
        return
    # link executable
    matching_index = find_index_from_onnx(onnx_model, INDEXES)
    ret = build_target(f'model/{matching_index}_gpu.cu', onnx_c, src_dir)
    if ret.returncode == 0:
        time, memory = time_and_memory(ret.stderr.decode('utf-8'))
        # info += 'GCC: ' + str(time) + 's ' + str(memory) + 'Kb '
        info = ' '*(len(onnx_model)+2) + 'Build : Time = ' + str(time) \
            + '(s) Memory = ' + str("%.2f" % (int(memory)/1000000)) + '(Gb)\n'
        write_log(info, log)
    else:
        info = 'GCC: failed\n'
        if debug:
            info = ' '.join(cmds) + '\n'
        write_log(info, log)
        return
    # run
    cmds = ['time', '-f', '\"%e %M\"', exec_file]
    if debug:
        print(' '.join(cmds))
    ret = inference(exec_file, '/app/cifar/cifar-10-batches-bin/test_batch.bin', 0, 0)
    if ret.returncode == 0:
        time, memory = time_and_memory(ret.stderr.decode('utf-8'))
        info = ' '*(len(onnx_model)+2) + 'Inference : Time = ' + str(time) \
            + '(s) Memory = ' + str("%.2f" % (int(memory)/1000000)) + '(Gb)\n'
        write_log(info, log)
    else:
        info = 'Exec: failed'
        if ret.returncode > 128:
            info += ' due to ' + signal.Signals(ret.returncode - 128).name
        info += '\n'
        write_log(info, log)
    return

def test_perf_ace(cwd, cmplr_path, onnx_path, log, debug):
    info = '-------- ACE --------\n'
    write_log(info, log)

    exec_dir = os.path.join(cwd, 'ace-compiler/release_gpu/rtlib/build/example')
    for model in INDEXES:
        case = model + '_gpu'
        if model == 'resnet32_cifar100':
            dataset = os.path.join(cwd, 'cifar/cifar-100-binary/test.bin')
        else:
            dataset = os.path.join(cwd, 'cifar/cifar-10-batches-bin/test_batch.bin')

        cmds = ['time', '-f', '\"%e %M\"', f'./{case}']
        cmds.extend([dataset, '0', '0'])

        ret = subprocess.run(cmds, 
                             cwd=exec_dir,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if ret.returncode == 0:
            info = model + '_ace: '
            write_log(info, log)
            msg = ret.stdout.decode().splitlines()
            for item in msg:
                if '[RESULT] infer' in item:
                    info = ' '*(len(model)+12) + item
                    write_log(info, log)
                elif 'Total Execution Time (all operations)' in item:
                    parts = item.split(':')
                    if len(parts) > 1:
                        time_str = parts[1].strip().split(' ')[0]
                        total_execution_time = float(time_str)
                        info = ' '*(len(model)+12) + 'Inference Time = ' + str("%.4f" % (total_execution_time/1000)) + '(s)\n'
                        write_log(info, log)
        else:
            info = case + ': failed'
            if ret.returncode > 128:
                info += ' due to ' + signal.Signals(ret.returncode - 128).name
            info += '\n'
            write_log(info, log)
    return

def main():
    parser = argparse.ArgumentParser(description='Run performance data for ACE Framework')
    parser.add_argument('-k', '--kpath', metavar='PATH', help='Path to build directory of FHE-MP-CNN')
    parser.add_argument('-c', '--cmplr', metavar='PATH', help='Path to the ACE compiler')
    parser.add_argument('-m', '--model', metavar='PATH', help='Path to the onnx models')
    parser.add_argument('-a', '--all', action='store_true', default=False, help='Run all performance suites')
    parser.add_argument('-e', '--exp', action='store_true', default=False, help='Run expert performance suites only')
    parser.add_argument('-f', '--file', metavar='PATH', help='Run single ONNX model only')
    parser.add_argument('-d', '--debug', action='store_true', default=False, help='Print out debug info')
    args = parser.parse_args()
    debug = args.debug
    # check memory requirement
    mem_enough = False
    if args.exp or args.all:
        mem_enough = check_required_memory(50)
    else:
        mem_enough = check_required_memory(60)
    if not mem_enough:
        sys.exit(-1)
    # FHE-MP-CNN path
    kpath = '/app/baselines/gpu/build/bin'
    cwd = os.getcwd()
    if args.kpath is not None:
        kpath = os.path.abspath(args.kpath)
    if not os.path.exists(kpath):
        print(kpath, 'does not exist! Please build EXPERT executables first!')
        sys.exit(-1)
    # ACE compiler path
    cmplr_path = '/app/ace_cmplr_gpu'
    if args.cmplr is not None:
        cmplr_path = os.path.abspath(args.cmplr)
    if not os.path.exists(cmplr_path):
        print(cmplr_path, 'does not exist! Please ACE compiler first!')
        sys.exit(-1)
    # ONNX model path
    onnx_path = '/app/model'
    if args.model is not None:
        onnx_path = os.path.abspath(args.model)
    if not os.path.exists(onnx_path):
        print(onnx_path, 'does not exist! Pre-trained ONNX model files are missing!')
        sys.exit(-1)
    date_time = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M")
    log_file_name = '7.4_' + date_time + '.log'
    log = open(os.path.join(cwd, log_file_name), 'w')
    # run tests
    if args.file is not None:
        # NOTE: -f/--file is not supported in perf_gpu.py. The original
        # ace_compile_and_run_onnx path called process_model/build_target
        # which were never imported (they live in ace_compile_gpu.py under
        # different names compile_model/link_target and mismatched paths),
        # so -f crashed with NameError. README 7.4 uses only -a/-e; -f is
        # rejected cleanly here. The flag stays in argparse for syntax compat.
        print('Error: -f/--file is not supported in perf_gpu.py. Use -e or -a.',
              file=sys.stderr)
        sys.exit(1)
    elif args.exp:
        test_perf_fhe_mp_cnn(kpath, log, debug)
    elif args.all:
        test_perf_fhe_mp_cnn(kpath, log, debug)
        test_perf_ace(cwd, cmplr_path, onnx_path, log, debug)
    else:
        test_perf_ace(cwd, cmplr_path, onnx_path, log, debug)
    info = '-------- Done --------\n'
    write_log(info, log)
    log.close()
    return

if __name__ == "__main__":
    main()
