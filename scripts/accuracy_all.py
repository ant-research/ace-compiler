#!/usr/bin/python3

import os
import sys
import argparse
import matplotlib.pyplot as plt
import subprocess
import datetime
import shutil

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
              'resnet110_cifar10_train.onnx']

def write_log(info, log):
    print(info[:-1])
    log.write(info)
    log.flush()
    return

def time_and_memory(outputs):
    result = outputs.strip('"').split(' ')
    return result[0], result[1]

def generate_accuracy(raw_acc, ace_acc, table10_name, log):
    '''
    Generate accuracy table (Table 10 in paper)
    '''
    acc_gain = []
    for idx in range(len(ace_acc)):
        gain = (raw_acc[idx]- ace_acc[idx]) * 100 / raw_acc[idx]
        gain = "%.1f%%" % round(gain, 1)
        acc_gain.append(gain)
    rows = []
    for idx in range(len(ace_acc)):
        rows.append([MODELS[idx], "%.1f%%" % raw_acc[idx], "%.1f%%" % ace_acc[idx], acc_gain[idx]])

    fig, ax = plt.subplots()
    ax.axis('off')
    ax.table(cellText=rows, \
             colLabels=['Model', 'Unencrypted', 'Encrypted', 'Accuracy Loss'], \
             cellLoc='center', loc='center')
    fig.tight_layout()
    plt.savefig(table10_name)
    plt.close()
    info = '%s generated!\n' % table10_name
    write_log(info, log)
    return

def run_raw_accuracy(image_num, log):
    '''
    Run unencrypted accuracy test for all models with given image numbers
    '''
    info = '-------- Unencrypted Accuracy for %d Images --------\n' % image_num
    write_log(info, log)
    acc_res = []
    cifar_path = '/app/cifar'
    if not os.path.exists(cifar_path):
        write_log('%s does not exist!\n' % cifar_path, log)
        sys.exit(-1)
    cifar10_script = '/app/scripts/infer_model.py'
    if not os.path.exists(cifar10_script):
        write_log('%s does not exist!\n' % cifar10_script, log)
        sys.exit(-1)
    cifar100_script = '/app/scripts/infer_model_cifar100.py'
    if not os.path.exists(cifar100_script):
        write_log('%s does not exist!\n' % cifar100_script, log)
        sys.exit(-1)
    onnx_path = '/app/model'
    if not os.path.exists(onnx_path):
        write_log('%s does not exist!\n' % onnx_path, log)
        sys.exit(-1)
    total_time = 0.0
    max_memory = 0.0
    for i in range(len(INDEXES)):
        rn = INDEXES[i]
        onnx_file = os.path.join(onnx_path, ONNX_FILES[i])
        if not os.path.exists(onnx_file):
            write_log('%s does not exist!\n' % onnx_file, log)
            sys.exit(-1)
        if onnx_file.find(rn + '_') == -1:
            write_log('Model file %s does not match %s test!\n' % (onnx_file, rn), log)
            sys.exit(-1)
        script = cifar10_script
        if rn.find('cifar100') != -1:
            script = cifar100_script
        cmds = ['time', '-f', '\"%e %M\"', 'python3', script, '--loader_onnx', onnx_file, '--datasets', cifar_path, '--images', str(image_num)]
        ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        accuracy = 1.0
        info = '%s: Inference Failed!\n' % rn
        info += ' ' * (len(rn)+2) + ' '.join(cmds) + '\n'
        if ret.returncode == 0:
            rtime, rmemory = time_and_memory(ret.stderr.decode().splitlines()[0])
            time = float(rtime)
            memory = float(rmemory) / 1000000
            total_time += time
            if max_memory < memory:
                max_memory = memory
            accuracy = float(ret.stdout.decode().splitlines()[-1].split(':')[1].strip()) * 100
            info = '%s: Unencrypted Accuracy = %.1f%%\n' % (rn, accuracy)
        acc_res.append(accuracy)
        write_log(info, log)
    info = 'Total time = %.2f(s), Max memory usage = %.1f(Gb)\n' % (total_time, max_memory)
    write_log(info, log)
    return acc_res

def run_ace_accuracy(image_num, log):
    '''
    Run ACE accuracy test for all models with given image numbers
    '''
    info = '-------- Encrypted Accuracy via ACE for %d Images --------\n' % image_num
    write_log(info, log)
    acc_res = []
    script = '/app/scripts/accuracy.sh'
    if not os.path.exists(script):
        write_log('%s does not exist!\n' % script, log)
        sys.exit(-1)
    total_time = 0.0
    max_memory = 0.0
    for rn in INDEXES:
        cmds = ['time', '-f', '\"%e %M\"', script, rn, '0', str(image_num)]
        ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        accuracy = 0.0
        err_log = '/app/release_openmp/' + rn + '.acc.0.' + str(image_num-1) + '.log'
        info = '%s: Inference Failed! Refer to \'%s\' for details.\n' % (rn, err_log)
        info += ' ' * (len(rn)+2) + ' '.join(cmds) + '\n'
        if not os.path.exists(err_log):
            write_log('Warning: %s does not exist!\n' % err_log, log)
        if ret.returncode == 0:
            rtime, rmemory = time_and_memory(ret.stderr.decode().splitlines()[0])
            time = float(rtime)
            memory = float(rmemory) / 1000000
            total_time += time
            if max_memory < memory:
                max_memory = memory
            msg = ret.stdout.decode().splitlines()
            for line in msg:
                if line.find('[RESULT]') == -1:
                    continue
                item = line.split(' ')
                accuracy = float(item[6].strip().split(',')[0].strip()) * 100
            info = '%s: Accuracy = %.1f%%, Time = %.2f(s), Memory = %.1f(GB)\n' % (rn, accuracy, time, memory)
        acc_res.append(accuracy)
        write_log(info, log)
    info = 'Total time = %.2f(s), Max memory usage = %.1f(Gb)\n' % (total_time, max_memory)
    write_log(info, log)
    return acc_res

def main():
    parser = argparse.ArgumentParser(description='Run accuracy data for ACE Framework')
    parser.add_argument('-n', '--num', type=int, default=10, help='Number of images to run for each model, ranges: [0, 10000]')
    args = parser.parse_args()
    image_num = args.num
    table10_name = 'Table10'
    if image_num != 1000:
        table10_name += '-' + str(image_num) + '-ImagesOnly'
    table10_name += '.pdf'
    ace_cmplr = '/app/release_openmp/driver/fhe_cmplr'
    if not os.path.exists(ace_cmplr):
        print('ACE compiler %s does not exist! Please build OpenMP version of the ACE compiler first!' % ace_cmplr)
        sys.exit(-1)
    date_time = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M")
    log_file_name = date_time + '_acc_' + str(image_num) + '.log'
    log = open(os.path.join(os.getcwd(), log_file_name), 'w')
    raw_acc = run_raw_accuracy(image_num, log)
    ace_acc = run_ace_accuracy(image_num, log)
    info = '-------- Accuracy Test Done --------\n'
    write_log(info, log)
    generate_accuracy(raw_acc, ace_acc, table10_name, log)
    log.close()
    res_dir = '/app/ace_ae_result'
    if os.path.exists(res_dir):
        shutil.copyfile(table10_name, os.path.join(res_dir, table10_name))
    return

if __name__ == "__main__":
    main()
