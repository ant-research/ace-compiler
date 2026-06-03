#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
import sys
import argparse
import datetime
from ace_util import *  # noqa F403


def ace_compile_and_link(cwd, onnx_file, cmplr_dir, installed, src_dir,
                         paper, lib, extra, acc, keep, debug, trace, log):
    '''
    Compile and link executable for onnx_file

    Args:
        cwd(str): current working directory
        onnx_file(str): path of the onnx file
        cmplr_dir(str): path of the dir that the ACE compiler is built or installed
        installed(bool): if the ACE compiler is installed or located in build dir
        src_dir(str): path that holds air-infra, nn-addon & fhe-cmplr
        paper(str): conference paper configuration
        lib(str): target library to link
        extra(list[str]): extra ACE compilation options
        acc(bool): compile for accuracy test
        keep(bool): if intermediate (.t, .json) files are kept
        debug(bool): print out debug info
        trace(bool): if ACE trace is enabled
        log(file): log file

    Returns:
        exec_file(str): encrypted executable of the input onnx_file
    '''
    # onnx_file : xxx/resnet20_cifar10_pre.onnx
    # onnx_name : resnet20_cifar10_pre
    onnx_name, _ = os.path.splitext(os.path.basename(onnx_file))
    # test : resnet20_cifar10
    test = get_test_name(onnx_name)
    info = test + ':\n'
    write_log(info, log)
    indent = ' ' * (len(test) + 2)
    exec_file = os.path.join(cwd, onnx_name + '.ace')
    onnx_c = os.path.join(cwd, onnx_name + '.c')
    tfile = os.path.join(cwd, onnx_name + '.t')
    jfile = os.path.join(cwd, onnx_name + '.json')
    wfile = onnx_name + '.weight'
    # ACE compile
    ace_cmplr = get_ace_cmplr(cmplr_dir)
    cmds = ['time', '-f', '\"%e %M\"', ace_cmplr, onnx_file]
    ace_option = get_ace_option(test, paper, lib, extra, acc, trace)
    cmds.extend(ace_option)
    cmds.extend(['-P2C:df=' + wfile, '-o', onnx_c])
    if debug:
        print(' '.join(cmds))
    if os.path.exists(wfile):
        os.remove(wfile)
    ret = run_cmd(cmds)
    if not keep:
        if os.path.exists(tfile):
            os.remove(tfile)
        if os.path.exists(jfile):
            os.remove(jfile)
    msg = ret.stdout.decode().splitlines()
    err = ret.stderr.decode().splitlines()
    if ret.returncode == 0:
        time, memory = time_and_memory(err[-1])
        info = '%sACE: Time = %.2f(s), Memory = %.2f(GB)\n' % (indent, time, memory)
        write_log(info, log)
        if trace:
            for item in msg:
                # ignore message that has nothing to do with performance statistics
                if item.find('Assertion Failure') != -1 or item.find('width_block_data') != -1:
                    continue
                write_log(indent + item + '\n', log)
    else:
        info = 'ACE: failed\n'
        write_log(info, log)
        info = ' '.join(cmds) + '\n'
        write_log(info, log)
        for item in msg:
            write_log(item + '\n', log)
        for item in err:
            write_log(item + '\n', log)
        return None
    # replace onnx.inc with onnx.c
    onnx_inc = get_onnx_inc_file(test, src_dir)
    if onnx_inc is None:
        print('Failed to find source to test %s' % onnx_name)
        sys.exit(-1)
    cmds = ['cp', onnx_c, onnx_inc]
    if debug:
        print(' '.join(cmds))
    ret = subprocess.run(cmds, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if ret.returncode != 0:
        print('Failed to replace onnx.c to onnx.inc for %s' % onnx_name)
        print(' '.join(cmds))
        sys.exit(-1)
    # link executable
    test_src = os.path.join(os.path.dirname(onnx_inc), test + '.cxx')
    if not os.path.exists(test_src):
        print(test_src, 'does not exist')
        sys.exit(-1)
    cmds = ['time', '-f', '\"%e %M\"', 'c++', test_src]
    cpp_option = get_cpp_option(cmplr_dir, installed, src_dir, acc)
    cmds.extend(cpp_option)
    cmds.extend(['-lgmp', '-lm', '-o', exec_file])
    if acc:
        cmds.append('-lgomp')
    if debug:
        print(' '.join(cmds))
    ret = subprocess.run(cmds, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if ret.returncode == 0:
        time, memory = time_and_memory(ret.stderr.decode().splitlines()[-1])
        info = '%sGCC: Time = %.2f(s), Memory = %.2f(GB)\n' % (indent, time, memory)
        write_log(info, log)
    else:
        info = 'GCC: failed\n'
        write_log(info, log)
        info = ' '.join(cmds) + '\n'
        write_log(info, log)
        for item in ret.stdout.decode().splitlines():
            write_log(item + '\n', log)
        for item in ret.stderr.decode().splitlines():
            write_log(item + '\n', log)
        return None
    return exec_file


def run_ace_compile(cwd, file_path, model_dir, build_dir, install_dir, src_dir,
                    paper, lib, extra, acc, keep, debug, trace, log):
    '''
    Main function to compile ONNX models via ACE compiler to encrypted executables

    Args:
        cwd(str): current working directory
        file_path(str): path of the onnx file
        model_dir(str): path of dir that holds onnx files to test
        build_dir(str): path of the build dir of the ACE compiler
        install_dir(str): path of the install dir of the ACE compiler
        src_dir(str): path that holds air-infra, nn-addon & fhe-cmplr
        paper(str): conference paper configuration
        lib(str): target library to link
        extra(list[str]): extra ACE compilation options
        acc(bool): compile for accuracy test
        keep(bool): if intermediate (.t, .json) files are kept
        debug(bool): print out debug info
        trace(bool): if ACE trace is enabled
        log(file): log file

    Returns:
        res(list[str]): encrypted executables of the input onnx_files
    '''
    info = '-------- ACE'
    if acc:
        info += '(OpenMP)'
    info += ' Compilation --------\n'
    write_log(info, log)
    onnx_files = get_test_onnx_files(file_path, model_dir, paper)
    cmplr_dir, installed = check_compile_env(build_dir, install_dir, src_dir)
    res = []
    for onnx_file in onnx_files:
        # run compilations
        exec_file = ace_compile_and_link(cwd, onnx_file, cmplr_dir, installed, src_dir,
                                         paper, lib, extra, acc, keep, debug, trace, log)
        if exec_file is not None:
            res.append(exec_file)
    info = '-------- Done --------\n'
    write_log(info, log)
    return res


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Use ACE to compile ONNX models to executables of encrypted computations',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-a', '--acc', action='store_true', default=False,
                        help='compile for accuracy tests (enable OpenMP)')
    parser.add_argument('-b', '--build', metavar='PATH',
                        help='directory where the ACE compiler is built')
    parser.add_argument('-c', '--cmplr', metavar='PATH',
                        help='installation directory of the ACE compiler')
    parser.add_argument('-d', '--debug', action='store_true', default=False,
                        help='print out debug info')
    parser.add_argument('-e', '--extra', metavar='OPTIONS', nargs='*',
                        help='extra ACE compilation options')
    parser.add_argument('-f', '--file', metavar='PATH',
                        help='compile single ONNX model')
    parser.add_argument('-k', '--keep', action='store_true', default=False,
                        help='keep files generated in the test')
    parser.add_argument('-l', '--lib', choices=['ant', 'seal', 'openfhe'], default='ant',
                        help='target library to link')
    parser.add_argument('-m', '--model', metavar='PATH', default='./model',
                        help='directory holds the onnx models to be compiled')
    parser.add_argument('-p', '--paper', choices=['cgo25', 'asplos25'],
                        help='configuration to reproduce paper results')
    parser.add_argument('-s', '--src', metavar='PATH', default='./',
                        help='directory that holds repos of air-infra, fhe-cmplr & nn-addon')
    parser.add_argument('-t', '--trace', action='store_false', default=True,
                        help='print out trace info into log file')
    args = parser.parse_args()
    cwd = os.getcwd()
    date_time = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M")
    log_file_name = date_time + '.cmp.log'
    log = open(os.path.join(cwd, log_file_name), 'w')
    info = '#### log for: %s\n' % (' '.join(sys.argv))
    write_log(info, log)
    run_ace_compile(cwd, args.file, args.model, args.build, args.cmplr, args.src, args.paper,
                    args.lib, args.extra, args.acc, args.keep, args.debug, args.trace, log)
    log.close()
