#!/usr/bin/python3

import os
import sys
import argparse
import matplotlib.pyplot as plt
import numpy as np
import math
import shutil

ACE='ACE'
EXPERT='Expert'
COMP='Compile'
EXEC='Exec'
TIME='Time'
MEMORY='Memory'
KEYMEMORY='KeyMemory'
LOGN='LogN'
LOGQ0='LogQ0'
DELTA='Delta'
MODELS = ['ResNet-20', 'ResNet-32', 'ResNet-32*', 'ResNet-44', 'ResNet-56', 'ResNet-110']
INDEXES = ['resnet20_cifar10', 'resnet32_cifar10', 'resnet32_cifar100', 'resnet44_cifar10', 'resnet56_cifar10', 'resnet110_cifar10']
FRONTEND='ONNX2AIR'
VECTOR='VECTOR'
SIHE='SIHE'
CKKS='CKKS'
POLY='POLY'
POLY2C='POLY2C'
CONV='Tensor::conv'
BTS='FHE::bootstrap'
RELU='FHE::relu'

MYBLUE='#6C8EBF'
MYORANGE='#FFCC99'
MYGREEN='#CDEB8B'
MYOTHER='#D79B00'
MYYELLOW='#FFFF88'
MYPINK='#FFCCCC'
MYPURPLE='#9673A6'

FIGURE5='Figure5.pdf'
FIGURE6='Figure6.pdf'
FIGURE7='Figure7.pdf'
TABLE9='Table9.pdf'

BARWIDTH = 0.45
INDEXACE = np.arange(len(MODELS))
INDEXEXP = INDEXACE + BARWIDTH

def generate_comp_time(result):
    '''
    Generate ACE compilation time figure
    '''
    # ACE raw data
    ace_compile_time = []
    ace_fe_time = []
    ace_vec_time = []
    ace_sihe_time = []
    ace_ckks_time = []
    ace_poly_time = []
    ace_2c_time = []
    # Accumulated bar height
    ace_acc_vec_time = []
    ace_acc_sihe_time = []
    ace_acc_ckks_time = []
    ace_acc_poly_time = []
    ace_acc_2c_time = []
    ace_other_time = []
    for onnx_model in INDEXES:
        comp_info = result[ACE][onnx_model][COMP]
        # ACE raw data
        ace_compile_time.append(comp_info[TIME])
        ace_fe_time.append(comp_info[FRONTEND])
        ace_vec_time.append(comp_info[VECTOR])
        ace_sihe_time.append(comp_info[SIHE])
        ace_ckks_time.append(comp_info[CKKS])
        ace_poly_time.append(comp_info[POLY])
        ace_2c_time.append(comp_info[POLY2C])
        # Accumulated bar height
        ace_acc_vec_time.append(comp_info[FRONTEND] + comp_info[VECTOR])
        ace_acc_sihe_time.append(ace_acc_vec_time[-1] + comp_info[SIHE])
        ace_acc_ckks_time.append(ace_acc_sihe_time[-1] + comp_info[CKKS])
        ace_acc_poly_time.append(ace_acc_ckks_time[-1] + comp_info[POLY])
        ace_acc_2c_time.append(ace_acc_poly_time[-1] + comp_info[POLY2C])
        ace_other_time.append(ace_compile_time[-1] - ace_acc_2c_time[-1])

    plt.figure(figsize=(10, 4))

    plt.bar(INDEXACE, height=ace_fe_time, width=BARWIDTH, label='Front-end', color=MYORANGE)
    plt.bar(INDEXACE, height=ace_vec_time, width=BARWIDTH, bottom=ace_fe_time, label='VECTOR', color=MYPURPLE)
    plt.bar(INDEXACE, height=ace_sihe_time, width=BARWIDTH, bottom=ace_acc_vec_time, label='SIHE', color=MYYELLOW)
    plt.bar(INDEXACE, height=ace_ckks_time, width=BARWIDTH, bottom=ace_acc_sihe_time, label='CKKS', color=MYBLUE)
    plt.bar(INDEXACE, height=ace_poly_time, width=BARWIDTH, bottom=ace_acc_ckks_time, label='POLY', color=MYPINK)
    plt.bar(INDEXACE, height=ace_2c_time, width=BARWIDTH, bottom=ace_acc_poly_time, label='POLY2C', color=MYGREEN)
    plt.bar(INDEXACE, height=ace_other_time, width=BARWIDTH, bottom=ace_acc_2c_time, label='Others', color=MYOTHER)

    plt.legend(prop={'size':'large', 'weight':'bold'})
    plt.ylabel('Seconds(s)', fontsize='xx-large')
    plt.xticks(INDEXACE, MODELS, rotation=7, fontsize='x-large', weight='bold')

    idx = np.arange(len(ace_compile_time))
    for a, b in zip(idx, ace_compile_time):
        plt.text(a, b, b, ha='center', va='bottom', weight='bold', fontsize='large')

    plt.savefig(FIGURE5)
    plt.close()
    print('%s generated!' % FIGURE5)
    return

def generate_exec_time(result):
    '''
    Generate per-image inference time comparison figure
    '''
    # ACE raw data
    ace_exec_time = []
    ace_conv_time = []
    ace_bts_time = []
    ace_relu_time = []
    # Accumulated bar height
    ace_acc_bts_time = []
    ace_acc_relu_time = []
    acc_other_time = []

    # Expert raw data
    exp_exec_time = []
    exp_conv_time = []
    exp_bts_time = []
    exp_relu_time = []
    # Accumulated bar height
    exp_acc_bts_time = []
    exp_acc_relu_time = []
    exp_other_time = []

    for onnx_model in INDEXES:
        exec_info = result[ACE][onnx_model][EXEC]
        total_time = exec_info[TIME]
        conv_time = exec_info[CONV]
        bts_time = exec_info[BTS]
        relu_time = exec_info[RELU]
        # ACE raw data
        ace_exec_time.append(total_time)
        ace_conv_time.append(conv_time)
        ace_bts_time.append(bts_time)
        ace_relu_time.append(relu_time)
        # Accumulated bar height
        ace_acc_bts_time.append(conv_time + bts_time)
        ace_acc_relu_time.append(ace_acc_bts_time[-1] + relu_time)
        acc_other_time.append(ace_exec_time[-1] - ace_acc_relu_time[-1])
        # Expert data
        exec_info = result[EXPERT][onnx_model][EXEC]
        total_time = exec_info[TIME]
        conv_time = exec_info[CONV]
        bts_time = exec_info[BTS]
        relu_time = exec_info[RELU]
        exp_exec_time.append(total_time)
        exp_conv_time.append(conv_time)
        exp_bts_time.append(bts_time)
        exp_relu_time.append(relu_time)
        # Accumulated bar height
        exp_acc_bts_time.append(conv_time + bts_time)
        exp_acc_relu_time.append(exp_acc_bts_time[-1] + relu_time)
        exp_other_time.append(exp_exec_time[-1] - exp_acc_relu_time[-1])

    plt.figure(figsize=(10, 4))

    plt.bar(INDEXACE, height=ace_conv_time, width=BARWIDTH, label='Conv', color=MYBLUE)
    plt.bar(INDEXACE, height=ace_bts_time, width=BARWIDTH, bottom=ace_conv_time, label='Bootstrap', color=MYPINK)
    plt.bar(INDEXACE, height=ace_relu_time, width=BARWIDTH, bottom=ace_acc_bts_time, label='ReLU', color=MYGREEN)
    plt.bar(INDEXACE, height=acc_other_time, width=BARWIDTH, bottom=ace_acc_relu_time, label='Others', color=MYOTHER)

    plt.bar(INDEXEXP, height=exp_conv_time, width=BARWIDTH, color=MYBLUE)
    plt.bar(INDEXEXP, height=exp_bts_time, width=BARWIDTH, bottom=exp_conv_time, color=MYPINK)
    plt.bar(INDEXEXP, height=exp_relu_time, width=BARWIDTH, bottom=exp_acc_bts_time, color=MYGREEN)
    plt.bar(INDEXEXP, height=exp_other_time, width=BARWIDTH, bottom=exp_acc_relu_time, color=MYOTHER)

    plt.legend(prop={'size':'x-large', 'weight':'bold'})
    plt.ylabel('Seconds(s)', fontsize='xx-large')
    plt.xticks(INDEXACE + BARWIDTH/2, MODELS, rotation=7, fontsize='x-large', weight='bold')

    idx = np.arange(len(ace_exec_time))
    for a, b in zip(idx, ace_exec_time):
        plt.text(a, b, b, ha='center', va='bottom', weight='bold', fontsize='large')
    for a, b in zip(idx+BARWIDTH, exp_exec_time):
        plt.text(a, b, b, ha='center', va='bottom', weight='bold', fontsize='large')

    plt.savefig(FIGURE6, pad_inches=0)
    plt.close()
    print('%s generated!' % FIGURE6)
    return

    # Print out analysis info
    ace_exec_total_time = 0
    ace_conv_total_time = 0
    ace_bts_total_time = 0
    ace_relu_total_time = 0
    exp_exec_total_time = 0
    exp_conv_total_time = 0
    exp_bts_total_time = 0
    exp_relu_total_time = 0
    for i in range(len(ace_exec_time)):
        if exp_exec_time[i] == 0:
            continue
        ace_exec_total_time += ace_exec_time[i]
        exp_exec_total_time += exp_exec_time[i]
        ace_conv_total_time += ace_conv_time[i]
        ace_bts_total_time += ace_bts_time[i]
        ace_relu_total_time += ace_relu_time[i]
        exp_conv_total_time += exp_conv_time[i]
        exp_bts_total_time += exp_bts_time[i]
        exp_relu_total_time += exp_relu_time[i]
    print('ACE speed up: %.2f,' % (exp_exec_total_time/ace_exec_total_time),\
          'Conv reduced: %.1f%%,' % (100 * (1 - ace_conv_total_time/exp_conv_total_time)),\
          'Bootstrap reduced: %.1f%%,' % (100 * (1 - ace_bts_total_time/exp_bts_total_time)),\
          'ReLU reduced: %.1f%%' % (100 * (1 - ace_relu_total_time/exp_relu_total_time)))
    return

def generate_exec_mem(result):
    '''
    Generate memory usage comparison figure
    '''
    # ACE data
    ace_mem = []
    ace_key = []
    ace_other = []

    # Expert data
    exp_mem = []
    exp_key = []
    exp_other = []

    for onnx_model in INDEXES:
        exec_info = result[ACE][onnx_model][EXEC]
        # ACE data
        ace_mem.append(exec_info[MEMORY])
        ace_key.append(exec_info[KEYMEMORY])
        ace_other.append(ace_mem[-1] - ace_key[-1])
        # Expert data
        exec_info = result[EXPERT][onnx_model][EXEC]
        exp_mem.append(exec_info[MEMORY])
        exp_key.append(exec_info[KEYMEMORY])
        exp_other.append(exp_mem[-1] - exp_key[-1])

    plt.figure(figsize=(10, 4))

    plt.bar(INDEXACE, height=ace_key, width=BARWIDTH, label='CKKS-Keys', color=MYBLUE)
    plt.bar(INDEXACE, height=ace_other, width=BARWIDTH, bottom=ace_key, label='Others', color=MYOTHER)

    plt.bar(INDEXEXP, height=exp_key, width=BARWIDTH, color=MYBLUE)
    plt.bar(INDEXEXP, height=exp_other, width=BARWIDTH, bottom=exp_key, color=MYOTHER)

    plt.legend(bbox_to_anchor=(0.5, 1.16), ncol=2, loc='upper center', prop={'size':'x-large', 'weight':'bold'})
    plt.ylabel('Gigabyte(GB)', fontsize='xx-large')
    plt.xticks(INDEXACE + BARWIDTH/2, MODELS, rotation=7, fontsize='x-large', weight='bold')

    idx = np.arange(len(ace_mem))
    for a, b in zip(idx, ace_mem):
        plt.text(a, b, b, ha='center', va='bottom', weight='bold', fontsize='large')
    for a, b in zip(idx+BARWIDTH, exp_mem):
        plt.text(a, b, b, ha='center', va='bottom', weight='bold', fontsize='large')

    plt.savefig(FIGURE7)
    plt.close()
    print('%s generated!' % FIGURE7)
    return

    # Print out analysis info
    ace_total_mem = 0
    ace_key_total_mem = 0
    exp_total_mem = 0
    exp_key_total_mem = 0
    for i in range(len(ace_mem)):
        if exp_mem[i] == 0:
            continue
        ace_total_mem += ace_mem[i]
        ace_key_total_mem += ace_key[i]
        exp_total_mem += exp_mem[i]
        exp_key_total_mem += exp_key[i]
    print('TotalMemory reduced: %.1f%%' % (100 * (1 - ace_total_mem/exp_total_mem)),\
          'KeyMemory reduced: %.1f%%' % (100 * (1 - ace_key_total_mem/exp_key_total_mem)))
    return

def generate_sec_param(result):
    '''
    Generate security parameters selected for CKKS table
    '''
    rows = []
    for idx in range(len(MODELS)):
        onnx_model = INDEXES[idx]
        exec_info = result[ACE][onnx_model][EXEC]
        log2_n = int(math.log2(exec_info[LOGN]))
        log2_q0 = exec_info[LOGQ0]
        delta = exec_info[DELTA]
        rows.append([MODELS[idx], log2_n, log2_q0, delta])

    fig, ax = plt.subplots()
    ax.axis('off')
    ax.table(cellText=rows, \
             colLabels=['Model', 'log\u2082(N)', 'log\u2082(Q\u2080)', 'log\u2082(\u0394)'], \
             cellLoc='center', loc='center')
    fig.tight_layout()
    plt.savefig(TABLE9, pad_inches=0)
    plt.close()
    print('%s generated!' % TABLE9)
    return

def generate_figures(result):
    generate_comp_time(result)
    generate_exec_time(result)
    generate_exec_mem(result)
    generate_sec_param(result)
    return

def read_ace_log(log_file, result):
    # read logs
    in_ace = False
    in_comp = False
    in_exec = False
    onnx_model = ''
    for line in log_file:
        if line.find('-------- ACE --------') != -1:
            in_ace = True
            result[ACE] = {}
        elif line.find('-------- Expert --------') != -1:
            in_ace = False
        elif in_ace:
            if line.find('.onnx') != -1:
                info = line.split('_')
                onnx_model = info[0] + '_' + info[1]
                result[ACE][onnx_model] = {}
                in_comp = False
                in_exec = False
            elif line.find('ACE:') != -1:
                comp_time = line.split('=')[1].strip().split('(')[0]
                result[ACE][onnx_model][COMP] = {}
                result[ACE][onnx_model][COMP][TIME] = float(comp_time)
                in_comp = True
                in_exec = False
            elif line.find('Exec:') != -1:
                info = line.split('=')
                exec_time = info[1].strip().split('(')[0]
                result[ACE][onnx_model][EXEC] = {}
                result[ACE][onnx_model][EXEC][TIME] = float(exec_time)
                all_mem = info[2].strip().split('(')[0]
                result[ACE][onnx_model][EXEC][MEMORY] = float(all_mem)
                key_mem = info[3].strip().split('(')[0]
                result[ACE][onnx_model][EXEC][KEYMEMORY] = float(key_mem)
                log2_n = info[8].strip().split(' ')[0]
                result[ACE][onnx_model][EXEC][LOGN] = int(log2_n)
                log2_q0 = info[9].strip().split(' ')[0]
                result[ACE][onnx_model][EXEC][LOGQ0] = int(log2_q0)
                delta = info[10].strip().split(' ')[0]
                result[ACE][onnx_model][EXEC][DELTA] = int(delta)
                in_comp = False
                in_exec = True
            elif line.find('-------- Done --------') != -1:
                in_ace = False
                in_comp = False
                in_exec = False
            else:
                info = line.split('=')
                var = info[0].strip()
                val = info[1].split('/')[0].strip()
                if in_comp:
                    key = COMP
                elif in_exec:
                    key = EXEC
                else:
                    sys.exit(-1)
                result[ACE][onnx_model][key][var] = float(val)
    return

def read_exp_log(log_file, result):
    # read logs
    in_exp = False
    onnx_model = ''
    for line in log_file:
        if line.find('-------- Expert --------') != -1:
            in_exp = True
            result[EXPERT] = {}
        elif line.find('-------- ACE --------') != -1:
            in_exp = False
        elif in_exp:
            if line.find('expert') != -1:
                info = line.split('_')
                onnx_model = info[-3] + '_' + info[-2]
                result[EXPERT][onnx_model] = {}
                result[EXPERT][onnx_model][EXEC] = {}
                info = info[-1].split('=')
                exec_time = info[1].split('(')[0].strip()
                result[EXPERT][onnx_model][EXEC][TIME] = float(exec_time)
                all_mem = info[2].split('(')[0].strip()
                result[EXPERT][onnx_model][EXEC][MEMORY] = float(all_mem)
                key_mem = info[3].split('(')[0].strip()
                result[EXPERT][onnx_model][EXEC][KEYMEMORY] = float(key_mem)
            elif line.find('-------- Done --------') != -1:
                in_exp = False
            else:
                info = line.split('=')
                var = info[0].strip()
                val = info[1].split('/')[0].strip()
                result[EXPERT][onnx_model][EXEC][var] = float(val)
    return

def main():
    parser = argparse.ArgumentParser(description='Generate ACE performance figures from log file')
    parser.add_argument('-f', '--file', metavar='PATH', help='Log file with both ACE and EXPERT performance data')
    parser.add_argument('-af', '--ace_file', metavar='PATH', help='Log file for ACE performance data')
    parser.add_argument('-ef', '--exp_file', metavar='PATH', help='Log file for EXPERT performance data')

    args = parser.parse_args()
    script_dir = os.path.dirname(__file__)
    ace_log_file_name = os.path.join(script_dir, 'ace_pre.log')
    exp_log_file_name = os.path.join(script_dir, 'expert_pre.log')
    if args.file is not None:
        ace_log_file_name = args.file
        exp_log_file_name = args.file
    if args.ace_file is not None:
        ace_log_file_name = args.ace_file
    if args.exp_file is not None:
        exp_log_file_name = args.exp_file
    if not os.path.exists(ace_log_file_name):
        print(ace_log_file_name, 'does not exist, an ACE performance data log is required!')
        sys.exit(-1)
    if not os.path.exists(exp_log_file_name):
        print(exp_log_file_name, 'does not exist, an EXPERT performance data log is required!')
        sys.exit(-1)

    result = {}
    # read ACE performance data
    ace_log_file = open(ace_log_file_name, 'r')
    read_ace_log(ace_log_file, result)
    ace_log_file.close()
    # read EXPERT performance data
    exp_log_file = open(exp_log_file_name, 'r')
    read_exp_log(exp_log_file, result)
    exp_log_file.close()
    # generate figures
    generate_figures(result)
    res_dir = '/app/ace_ae_result'
    if os.path.exists(res_dir):
        shutil.copyfile(FIGURE5, os.path.join(res_dir, FIGURE5))
        shutil.copyfile(FIGURE6, os.path.join(res_dir, FIGURE6))
        shutil.copyfile(FIGURE7, os.path.join(res_dir, FIGURE7))
        shutil.copyfile(TABLE9, os.path.join(res_dir, TABLE9))
    return

if __name__ == "__main__":
    main()
