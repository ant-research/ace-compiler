#!/usr/bin/python3

import os
import sys
import argparse
import subprocess
import datetime
import signal

def write_log(info, log):
    print(info[:-1])
    log.write(info)
    log.flush()
    return

def time_and_memory(outputs):
    result = outputs.strip('"').split(' ')
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
            cmds = ['time', '-f', '\"%e %M\"', './cnn', str(rn), str(cfar), '5', '5']
            if debug:
                print(' '.join(cmds))
            ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            case = 'resnet' + str(rn) + '_cifar' + str(cfar) + '_expert'
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
                info = case + ': Time = ' + str(time) + '(s) TotalMemory = ' \
                    + str("%.1f" % (int(memory)/1000000)) + '(Gb) KeyMemory = '\
                    + str("%.1f" % (float(key_mem))) + '(GB)\n'
                write_log(info, log)
                if op_time is not None:
                    sum_time = 0
                    for key in op_time:
                        sum_time += op_time[key]
                        info = ' ' * (len(case) + 6) + key + ' = ' \
                                + str("%.2f" % (op_time[key]/1000)) + ' / ' \
                                + str("%.2f" % (sum_time/1000)) + '(s) Count = ' + str(op_cnt[key]) + '\n'
                        write_log(info, log)
            else:
                info = case + ': failed'
                if ret.returncode > 128:
                    info += ' due to ' + signal.Signals(ret.returncode - 128).name
                info += '\n'
                write_log(info, log)
    return

def ace_compile_and_run_onnx(cwd, cmplr_path, onnx_path, onnx_model, log, debug):
    script_dir = os.path.dirname(__file__)
    onnx2c = os.path.join(script_dir, 'onnx2c.py')
    if not os.path.exists(onnx2c):
        print(onnx2c, 'does not exist')
        sys.exit(-1)
    model_file = os.path.join(onnx_path, onnx_model)
    if not os.path.exists(model_file):
        print(model_file, 'does not exist')
        return
    info = onnx_model + ':\n'
    write_log(info, log)
    model_base = onnx_model.split('.')[0]
    main_c = os.path.join(cwd, model_base + '.main.c')
    exec_file = os.path.join(cwd, model_base + '.ace')
    onnx_c = os.path.join(cwd, onnx_model + '.c')
    tfile = os.path.join(cwd, model_base + '.t')
    jfile = os.path.join(cwd, model_base + '.json')
    wfile = model_base + '.weight'
    # compile onnx
    cmds = ['python3', onnx2c, '-mp', model_file, '-op', main_c]
    if debug:
        print(' '.join(cmds))
    ret = subprocess.run(cmds, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if ret.returncode != 0:
        info += 'Failed to generate main.c\n'
        if debug:
            info = ' '.join(cmds) + '\n'
        write_log(info, log)
        return
    # ACE compile
    os.environ["RTLIB_BTS_EVEN_POLY"] = "1"
    os.environ["RTLIB_TIMING_OUTPUT"] = "stdout"
    cmds = ['time', '-f', '\"%e %M\"', os.path.join(cmplr_path, 'bin', 'fhe_cmplr')]
    cmds.extend([model_file, '-P2C:fp:df=' + wfile + ':lib=ant'])
    cmds.extend(['-SIHE:relu_vr_def=2:relu_mul_depth=13', '-CKKS:sk_hw=192', '-o', onnx_c])
    # options for detailed time info
    cmds.extend(['-O2A:ts', '-FHE_SCHEME:ts', '-VEC:ts:rtt:conv_fast', '-SIHE:ts:rtt'])
    cmds.extend(['-CKKS:ts:q0=60:sf=56', '-POLY:ts:rtt', '-P2C:ts'])
    if debug:
        print(' '.join(cmds))
    if os.path.exists(wfile):
        os.remove(wfile)
    ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if os.path.exists(tfile):
        os.remove(tfile)
    if os.path.exists(jfile):
        os.remove(jfile)
    if ret.returncode == 0:
        time, memory = time_and_memory(ret.stderr.decode().splitlines()[0])
        info = ' '*(len(onnx_model)+2) + 'ACE: Time = ' + str(time) \
            + '(s) Memory = ' + str("%.2f" % (int(memory)/1000000)) + '(Gb)\n'
        write_log(info, log)
        msg = ret.stdout.decode().splitlines()
        for item in msg:
            if item.find('phase_time') == -1:
                continue
            phase = item.split('[')[2].split(']')[0]
            time = item.split(':')[1].split('=')[1].split('/')[0].strip()
            acc_time = item.split(':')[1].split('=')[1].split('/')[1].split('(')[0].strip()
            info = ' '*(len(onnx_model)+7) + phase + ' = ' + str("%.3f" % float(time)) \
                    + ' / ' + str("%.3f" % float(acc_time)) + '(s)\n'
            write_log(info, log)
    else:
        info = 'ACE: failed\n'
        if debug:
            info = ' '.join(cmds) + '\n'
        write_log(info, log)
        return
    # link executable
    cmds = ['time', '-f', '\"%e %M\"', 'cc', main_c, onnx_c]
    cmds.extend(['-I', os.path.join(cmplr_path, 'rtlib/include')])
    cmds.extend(['-I', os.path.join(cmplr_path, 'rtlib/include/rt_ant')])
    cmds.append(os.path.join(cmplr_path, 'rtlib/lib/libFHErt_ant.a'))
    cmds.append(os.path.join(cmplr_path, 'rtlib/lib/libFHErt_common.a'))
    cmds.extend(['-lgmp', '-lm', '-o', exec_file])
    if debug:
        print(' '.join(cmds))
    ret = subprocess.run(cmds, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if ret.returncode == 0:
        time, memory = time_and_memory(ret.stderr.decode().splitlines()[0])
        # info += 'GCC: ' + str(time) + 's ' + str(memory) + 'Kb '
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
    ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    res = ret.stdout.decode().splitlines()
    if ret.returncode == 0:
        time, memory = time_and_memory(ret.stderr.decode().splitlines()[0])
        op_time = {}
        op_count = {}
        lib_info = []
        in_lib = False
        skip = True
        for msg in res:
            if msg.find('[RT_STAT]') != -1:
                items = msg.split(' ')
                if items[1] in op_time:
                    op_time[items[1]] += float(items[3])
                    op_count[items[1]] += 1
                else:
                    op_time[items[1]] = float(items[3])
                    op_count[items[1]] = 1
            elif msg.find('Total memory size for keys') != -1:
                key_info = msg.split(':')[1].split(',')
                rkey_cnt = key_info[0].split('=')[1].split(' ')[1].strip()
                rkey_mem = key_info[1].split('=')[1].split(' ')[1].strip()
                key_mem = key_info[2].split('=')[1].split(' ')[1].strip()
            elif msg.find('Total memory size for weight plain') != -1:
                weight_info = msg.split(':')[1].split(',')
                weight_cnt = weight_info[0].split('=')[1].strip()
                weight_mem = weight_info[1].split('=')[1].split(' ')[1].strip()
            elif msg.find('RTLib functions') != -1:
                in_lib = False
            elif in_lib:
                if skip:
                    skip = False
                elif msg.find('MAIN_GRAPH') != -1 and msg.find('sub total') != -1:
                    in_lib = False
                    lib_info.append(msg)
                else:
                    lib_info.append(msg)
        info = ' '*(len(onnx_model)+2) + 'Exec: Time = ' + str(time) + '(s) TotalMemory = ' \
                + str("%.1f" % (int(memory)/1000000)) + '(Gb) KeyMemory = ' \
                + str("%.1f" % (int(key_mem)/1000000000)) + '(Gb) RotKeyMem = ' \
                + str("%.1f" % (int(rkey_mem)/1000000000)) + '(Gb) RotKeyCnt = ' + rkey_cnt + ' WeightMem = ' \
                + str("%.1f" % (int(weight_mem)/1000000000)) + '(Gb) WeightCnt = ' + weight_cnt
        params = res[0].split(',')
        for item in params:
            equals = item.split('=')
            par = equals[0].strip()
            val = equals[1].strip()
            if par.find('_poly_degree') != -1:
                info += ' N = ' + val
            elif par.find('_first_mod_size') != -1:
                info += ' q0 = ' + val
            elif par.find('_scaling_mod_size') != -1:
                info += ' Delta = ' + val
        info += '\n'
        write_log(info, log)
        if op_time is not None:
            sum_time = 0
            for key in op_time:
                sum_time += op_time[key]
                info = ' ' * (len(onnx_model) + 8) + key + ' = ' \
                        + str("%.2f" % op_time[key]) + ' / ' \
                        + str("%.2f" % sum_time) + '(s) Count = ' + str(op_count[key]) + '\n'
                write_log(info, log)
        if lib_info is not None:
            for item in lib_info:
                write_log(' ' * (len(onnx_model) + 8) + item + '\n', log)
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
    os.chdir(cwd)
    model_files = [f for f in os.listdir(onnx_path) if os.path.isfile(os.path.join(onnx_path, f))]
    model_files.sort()
    for onnx_model in model_files:
        ace_compile_and_run_onnx(cwd, cmplr_path, onnx_path, onnx_model, log, debug)
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
        mem_enough = check_required_memory(320)
    else:
        mem_enough = check_required_memory(60)
    if not mem_enough:
        sys.exit(-1)
    # FHE-MP-CNN path
    kpath = '/app/FHE-MP-CNN/FHE-MP-CNN/cnn_ckks/build_cnn'
    cwd = os.getcwd()
    if args.kpath is not None:
        kpath = os.path.abspath(args.kpath)
    if not os.path.exists(kpath):
        print(kpath, 'does not exist! Please build EXPERT executables first!')
        sys.exit(-1)
    # ACE compiler path
    cmplr_path = '/app/ace_cmplr'
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
    log_file_name = date_time + '.log'
    log = open(os.path.join(cwd, log_file_name), 'w')
    # run tests
    if args.file is not None:
        info = '-------- ACE --------\n'
        write_log(info, log)
        test = os.path.basename(args.file)
        onnx_path = os.path.abspath(os.path.dirname(args.file))
        ace_compile_and_run_onnx(cwd, cmplr_path, onnx_path, test, log, debug)
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
