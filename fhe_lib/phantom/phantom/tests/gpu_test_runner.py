# -*- coding: utf-8 -*-
import subprocess
import time
import psutil

def is_gpu_idle(threshold=15):
    try:
        result = subprocess.run(['nvidia-smi', '--query-gpu=utilization.gpu', '--format=csv,noheader,nounits'], stdout=subprocess.PIPE)
        gpu_usages = result.stdout.decode('utf-8').strip().split('\n')
        
        if len(gpu_usages) > 0:
            usage = float(gpu_usages[0]) 
            if usage < threshold:
                print(f"GPU 0 is idle with usage {usage}%.")
                return True
        return False
    except Exception as e:
        print(f"Failed to get GPU usage: {e}")
        return False 

def run_tests(test_commands, output_file):
    for command in test_commands:
        try:
            result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            
            with open(output_file, 'a') as f:
                f.write(f"Test run for command {command} at {time.ctime()}\n")
                f.write(result.stdout.decode('utf-8'))
                f.write(result.stderr.decode('utf-8'))
                f.write('\n\n')
        except Exception as e:
            print(f"Failed to run test {command}: {e}")

def main():
    test_groups = [
        (['./add_inplace', './add_plain', './add_plain_inplace', './add', './add_many'], 'add_op.txt'),
        (['./sub_inplace', './sub_plain', './sub_plain_inplace', './sub'], 'sub_op.txt'),
        (['./multi_inplace', './multi_plain', './multi_plain_inplace', './multi'], 'multi_op.txt'),
        (['./encryption', './decryption'], 'encryption_op.txt'),
        (['./apply_galois_inplace', './apply_galois'], 'galois_op.txt'),
        (['./negation_inplace', './negation_inplace'], 'negation_op.txt'),
        (['./relin_inplace', './relin'], 'relin_op.txt'),
        (['./rescale_to_next_inplace', './rescale_to_next'], 'rescale_op.txt'),
        (['./rotation'], 'rotation_op.txt'),
        (['./conjugate', './conjugate_inplace'], 'conjugate_op.txt'),
        (['./encode', './decode'], 'encode_op.txt'),
        (['./bootstrapping_text'], 'bootstrapping_op.txt')
    ]

    execution_count = 0
    max_executions = 5

    while execution_count < max_executions:
        if is_gpu_idle():
            print("GPU is idle, running tests...")
            for test_commands, output_file in test_groups:
                run_tests(test_commands, output_file)
            execution_count += 1
            print(f"Completed execution {execution_count}/{max_executions}")
        else:
            print("GPU is busy, waiting...")

        time.sleep(60)

if __name__ == '__main__':
    main()