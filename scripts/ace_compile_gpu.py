#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
import subprocess
import datetime

CONFIG = {
    "resnet20_cifar10": {
        "vec": '-VEC:conv_parl:ssf',
        "relu_vr_def": 3,
        "relu_vr": (
            "/relu/Relu=4;"
            "/layer1/layer1.0/relu_1/Relu=4;/layer1/layer1.1/relu/Relu=4;/layer1/layer1.1/relu_1/Relu=5;/layer1/layer1.2/relu_1/Relu=5;"
            "/layer2/layer2.0/relu_1/Relu=5;/layer2/layer2.1/relu_1/Relu=5;/layer2/layer2.2/relu_1/Relu=7;/layer3/layer3.0/relu_1/Relu=4;"
            "/layer3/layer3.1/relu_1/Relu=6;/layer3/layer3.2/relu/Relu=4;/layer3/layer3.2/relu_1/Relu=20"
        ),
        "ckks": "-CKKS:q0=60:sf=56:N=65536:icl=17",
        "p2c": "-P2C:lib=phantom"
    },
    "resnet32_cifar10": {
        "vec": '-VEC:conv_parl:ssf',
        "relu_vr_def": 2,
        "relu_vr": (
            "/relu/Relu=4;"
            "/layer1/layer1.0/relu/Relu=3;/layer1/layer1.0/relu_1/Relu=5;/layer1/layer1.1/relu/Relu=3;/layer1/layer1.1/relu_1/Relu=5;"
            "/layer1/layer1.2/relu/Relu=3;/layer1/layer1.2/relu_1/Relu=5;/layer1/layer1.3/relu/Relu=3;/layer1/layer1.3/relu_1/Relu=5;/layer1/layer1.4/relu/Relu=3;/layer1/layer1.4/relu_1/Relu=5;"
            "/layer2/layer2.0/relu/Relu=3;/layer2/layer2.0/relu_1/Relu=5;/layer2/layer2.1/relu_1/Relu=5;/layer2/layer2.2/relu_1/Relu=5;/layer2/layer2.3/relu_1/Relu=6;/layer2/layer2.4/relu_1/Relu=6;"
            "/layer3/layer3.0/relu/Relu=3;/layer3/layer3.0/relu_1/Relu=5;/layer3/layer3.1/relu_1/Relu=4;/layer3/layer3.2/relu/Relu=3;/layer3/layer3.2/relu_1/Relu=6;/layer3/layer3.3/relu/Relu=4;/layer3/layer3.3/relu_1/Relu=10;/layer3/layer3.4/relu_1/Relu=11"
        ),
        "ckks": "-CKKS:q0=60:sf=56:N=65536:icl=17",
        "p2c": "-P2C:lib=phantom"
    },
    "resnet32_cifar100": {
        "vec": '-VEC:conv_parl:ssf',
        "relu_vr_def": 3,
        "relu_vr": (
            "/relu/Relu=5;"
            "/layer1/layer1.0/relu_1/Relu=6;/layer1/layer1.1/relu_1/Relu=7;/layer1/layer1.2/relu_1/Relu=8;/layer1/layer1.3/relu_1/Relu=10;/layer1/layer1.4/relu/Relu=4;/layer1/layer1.4/relu_1/Relu=7;"
            "/layer2/layer2.0/relu/Relu=4;/layer2/layer2.0/relu_1/Relu=6;/layer2/layer2.1/relu_1/Relu=8;/layer2/layer2.2/relu/Relu=4;/layer2/layer2.2/relu_1/Relu=8;/layer2/layer2.3/relu_1/Relu=9;/layer2/layer2.4/relu_1/Relu=11;"
            "/layer3/layer3.0/relu/Relu=4;/layer3/layer3.0/relu_1/Relu=8;/layer3/layer3.1/relu_1/Relu=9;/layer3/layer3.2/relu/Relu=4;/layer3/layer3.2/relu_1/Relu=11;/layer3/layer3.3/relu/Relu=4;/layer3/layer3.3/relu_1/Relu=26;/layer3/layer3.4/relu/Relu=5;/layer3/layer3.4/relu_1/Relu=46"
        ),
        "ckks": "-CKKS:q0=60:sf=56:N=65536:icl=17",
        "p2c": "-P2C:lib=phantom"
    },
    "resnet44_cifar10": {
        "vec": '-VEC:conv_parl:ssf',
        "relu_vr_def": 2,
        "relu_vr": (
            "/relu/Relu=4;"
            "/layer1/layer1.0/relu_1/Relu=5;/layer1/layer1.1/relu_1/Relu=5;/layer1/layer1.2/relu_1/Relu=5;/layer1/layer1.3/relu_1/Relu=6;/layer1/layer1.4/relu_1/Relu=6;/layer1/layer1.5/relu/Relu=2;/layer1/layer1.5/relu_1/Relu=7;/layer1/layer1.6/relu_1/Relu=6;"
            "/layer2/layer2.0/relu/Relu=2;/layer2/layer2.0/relu_1/Relu=5;/layer2/layer2.1/relu/Relu=2;/layer2/layer2.1/relu_1/Relu=5;/layer2/layer2.2/relu/Relu=2;"
            "/layer2/layer2.2/relu_1/Relu=5;/layer2/layer2.3/relu/Relu=2;/layer2/layer2.3/relu_1/Relu=5;/layer2/layer2.4/relu/Relu=2;/layer2/layer2.4/relu_1/Relu=5;/layer2/layer2.5/relu/Relu=2;/layer2/layer2.5/relu_1/Relu=6;/layer2/layer2.6/relu/Relu=2;/layer2/layer2.6/relu_1/Relu=7;"
            "/layer3/layer3.0/relu_1/Relu=5;/layer3/layer3.1/relu/Relu=2;/layer3/layer3.1/relu_1/Relu=5;/layer3/layer3.2/relu/Relu=2;/layer3/layer3.2/relu_1/Relu=6;/layer3/layer3.3/relu_1/Relu=7;/layer3/layer3.4/relu_1/Relu=9;/layer3/layer3.5/relu_1/Relu=15;/layer3/layer3.6/relu_1/Relu=16"
        ),
        "ckks": "-CKKS:q0=60:sf=56:N=65536:icl=17",
        "p2c": "-P2C:lib=phantom"
    },
    "resnet56_cifar10": {
        "vec": '-VEC:conv_parl:ssf',
        "relu_vr_def": 2,
        "relu_vr": (
            "/relu/Relu=4;"
            "/layer1/layer1.0/relu_1/Relu=6;/layer1/layer1.1/relu_1/Relu=5;/layer1/layer1.2/relu/Relu=3;/layer1/layer1.2/relu_1/Relu=6;/layer1/layer1.3/relu/Relu=3;/layer1/layer1.3/relu_1/Relu=7;/layer1/layer1.4/relu_1/Relu=6;/layer1/layer1.5/relu_1/Relu=6;/layer1/layer1.6/relu_1/Relu=6;/layer1/layer1.7/relu_1/Relu=6;/layer1/layer1.8/relu_1/Relu=5;"
            "/layer2/layer2.0/relu_1/Relu=4;/layer2/layer2.1/relu_1/Relu=4;/layer2/layer2.2/relu_1/Relu=5;/layer2/layer2.3/relu_1/Relu=5;/layer2/layer2.4/relu_1/Relu=6;/layer2/layer2.5/relu_1/Relu=8;/layer2/layer2.6/relu_1/Relu=11;/layer2/layer2.7/relu_1/Relu=11;/layer2/layer2.8/relu_1/Relu=12;"
            "/layer3/layer3.0/relu/Relu=3;/layer3/layer3.0/relu_1/Relu=5;/layer3/layer3.1/relu_1/Relu=5;/layer3/layer3.2/relu_1/Relu=5;/layer3/layer3.3/relu_1/Relu=5;/layer3/layer3.4/relu_1/Relu=5;/layer3/layer3.5/relu_1/Relu=6;/layer3/layer3.6/relu_1/Relu=8;/layer3/layer3.7/relu/Relu=3;/layer3/layer3.7/relu_1/Relu=10;/layer3/layer3.8/relu_1/Relu=12"
        ),
        "ckks": "-CKKS:q0=60:sf=56:N=65536:icl=17",
        "p2c": "-P2C:lib=phantom"
    },
    "resnet110_cifar10": {
        "vec": '-VEC:conv_parl:ssf',
        "relu_vr_def": 3,
        "relu_vr": (
            "/relu/Relu=14;"
            "/layer1/layer1.0/relu/Relu=4;/layer1/layer1.0/relu_1/Relu=14;/layer1/layer1.1/relu/Relu=4;/layer1/layer1.1/relu_1/Relu=15;/layer1/layer1.10/relu/Relu=5;/layer1/layer1.10/relu_1/Relu=18;/layer1/layer1.11/relu/Relu=5;/layer1/layer1.11/relu_1/Relu=17;"
            "/layer1/layer1.12/relu/Relu=4;/layer1/layer1.12/relu_1/Relu=21;/layer1/layer1.13/relu/Relu=7;/layer1/layer1.13/relu_1/Relu=22;/layer1/layer1.14/relu/Relu=7;/layer1/layer1.14/relu_1/Relu=23;/layer1/layer1.15/relu/Relu=7;/layer1/layer1.15/relu_1/Relu=25;"
            "/layer1/layer1.16/relu/Relu=6;/layer1/layer1.16/relu_1/Relu=22;/layer1/layer1.17/relu/Relu=6;/layer1/layer1.17/relu_1/Relu=22;/layer1/layer1.2/relu/Relu=3;/layer1/layer1.2/relu_1/Relu=15;/layer1/layer1.3/relu/Relu=6;/layer1/layer1.3/relu_1/Relu=20;"
            "/layer1/layer1.4/relu/Relu=5;/layer1/layer1.4/relu_1/Relu=17;/layer1/layer1.5/relu/Relu=5;"
            "/layer1/layer1.5/relu_1/Relu=17;/layer1/layer1.6/relu/Relu=5;/layer1/layer1.6/relu_1/Relu=17;/layer1/layer1.7/relu/Relu=4;/layer1/layer1.7/relu_1/Relu=17;/layer1/layer1.8/relu/Relu=4;/layer1/layer1.8/relu_1/Relu=17;/layer1/layer1.9/relu/Relu=5;/layer1/layer1.9/relu_1/Relu=18;"
            "/layer2/layer2.0/relu/Relu=6;/layer2/layer2.0/relu_1/Relu=14;/layer2/layer2.1/relu/Relu=4;/layer2/layer2.1/relu_1/Relu=14;/layer2/layer2.10/relu/Relu=3;/layer2/layer2.10/relu_1/Relu=19;/layer2/layer2.11/relu/Relu=3;/layer2/layer2.11/relu_1/Relu=19;/layer2/layer2.12/relu/Relu=3;"
            "/layer2/layer2.12/relu_1/Relu=22;/layer2/layer2.13/relu/Relu=3;/layer2/layer2.13/relu_1/Relu=21;/layer2/layer2.14/relu/Relu=3;/layer2/layer2.14/relu_1/Relu=23;/layer2/layer2.15/relu/Relu=3;/layer2/layer2.15/relu_1/Relu=22;/layer2/layer2.16/relu/Relu=4;/layer2/layer2.16/relu_1/Relu=23;"
            "/layer2/layer2.17/relu/Relu=3;/layer2/layer2.17/relu_1/Relu=22;/layer2/layer2.2/relu/Relu=3;/layer2/layer2.2/relu_1/Relu=15;/layer2/layer2.3/relu/Relu=4;/layer2/layer2.3/relu_1/Relu=16;/layer2/layer2.4/relu/Relu=4;/layer2/layer2.4/relu_1/Relu=16;/layer2/layer2.5/relu/Relu=3;"
            "/layer2/layer2.5/relu_1/Relu=17;/layer2/layer2.6/relu/Relu=5;/layer2/layer2.6/relu_1/Relu=17;/layer2/layer2.7/relu/Relu=3;/layer2/layer2.7/relu_1/Relu=17;/layer2/layer2.8/relu/Relu=3;/layer2/layer2.8/relu_1/Relu=18;/layer2/layer2.9/relu/Relu=4;/layer2/layer2.9/relu_1/Relu=19;"
            "/layer3/layer3.0/relu/Relu=5;/layer3/layer3.0/relu_1/Relu=14;/layer3/layer3.1/relu/Relu=4;/layer3/layer3.1/relu_1/Relu=15;/layer3/layer3.10/relu/Relu=3;/layer3/layer3.10/relu_1/Relu=20;/layer3/layer3.11/relu/Relu=4;/layer3/layer3.11/relu_1/Relu=20;"
            "/layer3/layer3.12/relu/Relu=4;/layer3/layer3.12/relu_1/Relu=20;/layer3/layer3.13/relu/Relu=4;/layer3/layer3.13/relu_1/Relu=24;/layer3/layer3.14/relu/Relu=4;/layer3/layer3.14/relu_1/Relu=27;/layer3/layer3.15/relu/Relu=4;/layer3/layer3.15/relu_1/Relu=30;"
            "/layer3/layer3.16/relu/Relu=4;/layer3/layer3.16/relu_1/Relu=27;/layer3/layer3.17/relu/Relu=9;/layer3/layer3.17/relu_1/Relu=33;/layer3/layer3.2/relu/Relu=3;/layer3/layer3.2/relu_1/Relu=15;/layer3/layer3.3/relu/Relu=4;/layer3/layer3.3/relu_1/Relu=15;/layer3/layer3.4/relu/Relu=3;/layer3/layer3.4/relu_1/Relu=15;/layer3/layer3.5/relu/Relu=3;"
            "/layer3/layer3.5/relu_1/Relu=16;/layer3/layer3.6/relu/Relu=3;/layer3/layer3.6/relu_1/Relu=16;/layer3/layer3.7/relu/Relu=3;/layer3/layer3.7/relu_1/Relu=16;/layer3/layer3.8/relu/Relu=3;/layer3/layer3.8/relu_1/Relu=17;/layer3/layer3.9/relu/Relu=3;/layer3/layer3.9/relu_1/Relu=18"
        ),
        "ckks": "-CKKS:q0=60:sf=56:N=65536:icl=17",
        "p2c": "-P2C:lib=phantom"
    },
}

def parser_option(model):
    """
    Get the compilation option based on the model name.
    """

    config = CONFIG.get(model)
    if config is None:
        print(f"Configuration for model '{model}' not found.")
        return
    
    vec_option = config['vec']
    sihe_option = f"-SIHE:relu_vr_def={config['relu_vr_def']}:relu_vr={config['relu_vr']}"
    ckks_option = config['ckks']
    p2c_option = config['p2c']
    
    return [vec_option, sihe_option, ckks_option, p2c_option]

def compile_model(cmplr, model, input, output, keep=None, work_dir=None):
    """
    Compile the model with the given commands and options.

    work_dir: the directory fhe_cmplr runs in. Intermediate files (.conf/.t/.json
    that fhe_cmplr writes to its CWD) land there instead of polluting /app root.
    Paths for the onnx input, the .inc output, and the .msg weight side-file are
    absolute, so changing the CWD only affects the side-channel intermediates.
    """
    options = parser_option(model)

    tfile = os.path.splitext(os.path.basename(input))[0] + '.t'
    jfile = os.path.splitext(os.path.basename(input))[0] + '.json'

    # Externalize weight data to a side file (same -P2C:df=<file>:fp form as
    # the CPU build_<model>.sh scripts) instead of embedding it in the .inc.
    # The new fhe-cmplr supports -P2C:lib=phantom + data_file (df) per the
    # fhe_cmplr --help (lib=ant/seal/openfhe/phantom/ace; df=data_file). The df
    # file lives next to the .inc (the rtlib example dir).
    df_file = os.path.splitext(output)[0] + '.msg'
    # Append df=<file>:fp to the existing -P2C:lib=phantom option (find it).
    for i, opt in enumerate(options):
        if opt.startswith('-P2C:lib=phantom'):
            options[i] = opt + f':df={df_file}'
            break

    # Cmplr command
    cmds = ['time', '-f', '\"%e %M\"', cmplr]
    cmds.extend(options)
    cmds.extend([input, '-o', output])

    print(f"Compile command: {' '.join(cmds)}")
    run_kwargs = dict(stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if work_dir is not None:
        run_kwargs['cwd'] = work_dir
    ret = subprocess.run(cmds, **run_kwargs)
    if ret.returncode == 0:
        print(f"Compile {model} : SUCCESS!")
    else:
        print(f"Compile {model} : FAILED!")
        print("Error output:", ret.stderr)
    if not keep:
        if os.path.exists(tfile):
            os.remove(tfile)
        if os.path.exists(jfile):
            os.remove(jfile)

    return ret

def link_target(cwd, model, inc_file):
    """
    Compile the model inference program
    """

    script = os.path.join(cwd, "scripts/build_target_gpu.sh")
    cu_file = os.path.join(cwd, f'model/{model}_gpu.cu')

    # Linker command
    cmds = ['time', '-f', '\"%e %M\"', script]
    cmds.extend([cu_file, inc_file])

    print(f"Build Target command: {' '.join(cmds)}")
    ret = subprocess.run(cmds, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if ret.returncode == 0:
        print(f"Compile {model} : SUCCESS!")
    else:
        print(f"Compile {model} : FAILED!")
        print("Error output:", ret.stderr)

    return ret

def inference(prog, input, idx, num):
    """
    Compile the model inference program
    """
    cmds = ['time', '-f', '\"%e %M\"', prog]
    cmds.extend([input, str(idx), str(num)])

    print(f"Inference command: {' '.join(cmds)}")
    ret = subprocess.run(cmds, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if ret.returncode == 0:
        print("inference executed successfully.")
    else:
        print("inference execution failed.")
        print("Error output:", ret.stderr)

    return ret

def main():
    cwd = '/app'
    # cwd = os.getcwd()
    cmplr = os.path.join(cwd, 'ace_cmplr_gpu/bin/fhe_cmplr')
    model_dir = os.path.join(cwd, 'model')
    link_dir = os.path.join(cwd, 'ace-compiler/fhe-cmplr/rtlib/phantom/example')

    # Side-channel intermediates (.conf/.t/.json fhe_cmplr writes to its CWD) live
    # under /tmp/ace_cmplr_gpu, never polluting /app root or the install dir.
    work_dir = '/tmp/ace_cmplr_gpu'
    os.makedirs(work_dir, exist_ok=True)

    # Cmplr & Build TestCase
    for model, config in CONFIG.items():
        input_file = os.path.join(model_dir, f'{model}_pre.onnx')
        output_file = os.path.join(link_dir, f'{model}_gpu.onnx.inc')

        compile_model(cmplr, model, input_file, output_file, work_dir=work_dir)
        link_target(cwd, model, output_file)

    # Inference
    # for model_name, config in CONFIG.items():
    #    target_dir = os.path.join(cwd, 'release/rtlib/build/example')
    #    program = f"{target_dir}/{model}_gpu"
    #    if model_name == 'resnet32_cifar100':
    #         dataset = f"{cwd}/dataset/cifar-100-binary/test.bin"
    #    else:
    #         dataset = f"{cwd}/dataset/cifar-10-batches-bin/test_batch.bin"

    #    inference(program, dataset, 0, 0)

if __name__ == "__main__":
    main()
