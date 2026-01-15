#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Check Coding Style, correct formatting automatically
and warn naming convention issue
'''

import argparse
import os
import sys
import subprocess


def is_excluded(path, exclude_dirs):
    '''
    If path contains excluded dirs
    '''
    for item in exclude_dirs:
        if path.find(item) != -1:
            return True
    return False


def find_a_file(cwd, src, exclude_dirs):
    '''
    Find a C/C++ or Python file under cwd
    '''
    for root, _, files in os.walk(cwd):
        for file in files:
            if file == os.path.basename(src):
                item = os.path.abspath(os.path.join(root, file))
                if is_excluded(item, exclude_dirs):
                    continue
                if os.path.dirname(src) != '':
                    tgt = os.path.abspath(src)
                    if tgt == item:
                        return [item]
                else:
                    return [item]
    return []


def find_valid_files(cwd, exclude_dirs, files):
    '''
    Find files not under exclude_dirs
    '''
    res = []
    for file in files:
        item = os.path.abspath(os.path.join(cwd, file))
        if is_excluded(item, exclude_dirs):
            continue
        if file.endswith('.cpp'):
            print('Do not use .cpp postfix, use .cxx for file:', os.path.basename(file))
        if file.endswith('.c') or file.endswith('.h') or file.endswith('.cxx') \
           or file.endswith('.py'):
            res.append(item)
    return res


def find_all_files(cwd, exclude_dirs):
    '''
    Find all C/C++ & Python files under cwd
    '''
    res = []
    for root, _, files in os.walk(cwd):
        res.extend(find_valid_files(root, exclude_dirs, files))
    return res


def print_except_info(info, exc: subprocess.TimeoutExpired):
    '''
    Print out exeception information
    '''
    print(info)
    if exc.stdout is not None:
        print('-------- stdout --------')
        print(exc.stdout.decode())
    if exc.stderr is not None:
        print('-------- stderr --------')
        print(exc.stderr.decode())
    print('-------- end of time out info --------')
    return


def find_last_changed_files(cwd, exclude_dirs, time_out):
    '''
    Find latest changed files,
    If there's no diff files in cwd,
    Return diff files in last commit
    '''
    res = []
    # look for diff files of current branch first, ignore deleted & unmerged files
    cmds = ['git', 'diff', 'origin/HEAD', '--name-only', '--diff-filter=du']
    try:
        ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             timeout=time_out, check=False)
    except subprocess.TimeoutExpired as exc:
        print_except_info('Git diff time out: ' + ' '.join(cmds), exc)
        sys.exit(-1)
    if ret.returncode == 0:
        files = ret.stdout.decode().splitlines()
        res.extend(find_valid_files(cwd, exclude_dirs, files))
    # look for cached diff files, ignore deleted files
    cmds = ['git', 'diff', '--cached', '--name-only', '--diff-filter=d']
    try:
        ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             timeout=time_out, check=False)
    except subprocess.TimeoutExpired as exc:
        print_except_info('Git diff cached time out: ' + ' '.join(cmds), exc)
        sys.exit(-1)
    if ret.returncode == 0:
        files = ret.stdout.decode().splitlines()
        for file in find_valid_files(cwd, exclude_dirs, files):
            if file not in res:
                res.append(file)
    # if no diff file, look for files in last commit/merge, ignore deleted files
    if len(res) == 0:
        cmds = ['git', 'log', '-m', '-1', '--name-only', '--diff-filter=d', '--pretty=']
        try:
            ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                 timeout=time_out, check=False)
        except subprocess.TimeoutExpired as exc:
            print_except_info('Git log time out: ' + ' '.join(cmds), exc)
            sys.exit(-1)
        if ret.returncode == 0:
            files = ret.stdout.decode().splitlines()
            res.extend(find_valid_files(cwd, exclude_dirs, files))
    return res


def separate_files(files, debug):
    '''
    Classify files into Python & C/C++ sets
    '''
    pfiles = []
    cfiles = []
    if debug:
        print(len(files), 'File(s) to check:', files)
    for file in files:
        if file.endswith('.py'):
            pfiles.append(file)
        else:
            cfiles.append(file)
    if debug:
        print(len(cfiles), 'C/C++ File(s) to check:', cfiles)
        print(len(pfiles), 'Python File(s) to check:', pfiles)
    return pfiles, cfiles


def pep8_format(files, dry_run, debug, time_out):
    '''
    Format python files in pep8 coding style
    '''
    ret_val = 0
    for file in files:
        cmds = ['autopep8', '--max-line-length', '100', file]
        if dry_run:
            cmds.append('-d')
        else:
            cmds.append('-i')
        if debug:
            print(' '.join(cmds))
        try:
            ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                 timeout=time_out, check=False)
        except subprocess.TimeoutExpired as exc:
            print_except_info('Pep8 time out: ' + ' '.join(cmds), exc)
            sys.exit(-1)
        if dry_run:
            if len(ret.stdout.decode().splitlines()) != 0:
                ret_val = 1
                print(file, 'has format issues!')
        else:
            if ret.returncode != 0:
                ret_val = 1
                print(file, 'failed pep8 formatting!')
    return ret_val


def flake8_check(config_file, mypy_config, files, dry_run, debug, time_out):
    '''
    Check python coding style via flake8
    '''
    ret_val = 0
    for file in files:
        cmds = ['flake8', '--config', config_file, '--mypy-config', mypy_config, file]
        if debug:
            print(' '.join(cmds))
        try:
            ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                 timeout=time_out, check=False)
        except subprocess.TimeoutExpired as exc:
            print_except_info('Flake8 time out: ' + ' '.join(cmds), exc)
            sys.exit(-1)
        if ret.returncode != 0:
            ret_val = 1
            if dry_run:
                print(file, 'has coding style issues!')
            else:
                print(ret.stdout.decode())
    return ret_val


def check_naming(config_file, files, build, dry_run, debug, time_out):
    '''
    Check C/C++ source code naming convention
    If there're issues, returns 1, else returns 0
    '''
    ret_val = 0
    for file in files:
        cmds = ['clang-tidy', '-p', build, '--config-file=' + config_file, file]
        if debug:
            print(' '.join(cmds))
        try:
            ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                 timeout=time_out, check=False)
        except subprocess.TimeoutExpired as exc:
            print_except_info('Clang-tidy time out: ' + ' '.join(cmds), exc)
            sys.exit(-1)
        errors = ret.stdout.decode().splitlines()
        has_issue = False
        for item in errors:
            if item.find('warning: invalid case style') != -1:
                has_issue = True
                break
        if has_issue:
            ret_val = 1
            if dry_run:
                print(file, 'has naming convention issues!')
            else:
                for item in errors:
                    print(item)
    return ret_val


def force_format(config_file, files, dry_run, debug, time_out):
    '''
    Correct C/C++ formatting automatically
    '''
    ret_val = 0
    for file in files:
        cmds = ['clang-format', '-style=file:' + config_file, file]
        if dry_run:
            cmds.extend(['-n', '-Werror'])
        else:
            cmds.append('-i')
        if debug:
            print(' '.join(cmds))
        try:
            ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                 timeout=time_out, check=False)
        except subprocess.TimeoutExpired as exc:
            print_except_info('Clang-format time out: ' + ' '.join(cmds), exc)
            sys.exit(-1)
        if ret.returncode != 0:
            ret_val = 1
            if dry_run:
                print(file, 'has format issues!')
            else:
                print(file, 'failed to correct format issues!')
    return ret_val


def check_configure():
    '''
    Get configuration information
    '''
    cfg_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
    format_config = os.path.join(cfg_dir, './devtools/.clang-format')
    if not os.path.exists(format_config):
        print(format_config, 'does not exist!')
        sys.exit(-1)
    tidy_config = os.path.join(cfg_dir, './devtools/.clang-tidy')
    if not os.path.exists(tidy_config):
        print(tidy_config, f'{cfg_dir} does not exist!')
        sys.exit(-1)
    flake8_config = os.path.join(cfg_dir, './devtools/.flake8')
    if not os.path.exists(flake8_config):
        print(flake8_config, 'does not exist!')
        sys.exit(-1)
    mypy_config = os.path.join(cfg_dir, './devtools/.mypy.ini')
    if not os.path.exists(mypy_config):
        print(mypy_config, 'does not exist!')
        sys.exit(-1)
    return format_config, tidy_config, flake8_config, mypy_config


def main():
    '''
    Main function for check coding style
    '''
    parser = argparse.ArgumentParser(description='Check Coding Style of C/C++ & Python files',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-a', '--all', action='store_true', default=False,
                        help='if set, check all C/C++ & Python files')
    parser.add_argument('-b', '--build', metavar='PATH', default='./build',
                        help='build path that contains \'compile_commands.json\'')
    parser.add_argument('-c', '--cwd', metavar='PATH', default='./',
                        help='set the path of sourcecode when checking')
    parser.add_argument('-d', '--debug', action='store_true', default=False,
                        help='if set, print out debug info')
    parser.add_argument('-f', '--file', metavar='FILE',
                        help='single C/C++ or Python file to check')
    parser.add_argument('-n', '--dry', action='store_true', default=False,
                        help='dry run, print out if a file has issues only')
    parser.add_argument('-t', '--timeout', type=int, default=60,
                        help='set subprocess timeout period when checking')
    parser.add_argument('-x', '--exclude', metavar='DIR', nargs='*',
                        help='exclude dir(s) from checking')
    args = parser.parse_args()
    # check .flake8, .clang-tidy & .clang-format files
    format_config, tidy_config, flake8_config, mypy_config = check_configure()
    src_dir = ''
    if args.cwd:
        src_dir = args.cwd
    else:
        src_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
    # build option
    build_path = os.path.join(os.getcwd(), args.build)
    if not os.path.exists(build_path):
        print(build_path, 'does not exists!')
        sys.exit(-1)
    # check compile_commands.json file required by clang-tidy
    json_file = os.path.join(build_path, 'compile_commands.json')
    if not os.path.exists(json_file):
        print(json_file, 'does not exists! Config cmake with -DCMAKE_EXPORT_COMPILE_COMMANDS=on')
        sys.exit(-1)
    # exclude option
    exclude_dirs = [os.path.abspath(build_path)]
    if args.exclude is not None:
        for item in args.exclude:
            # exclude_dirs.append(os.path.abspath(os.path.join(cwd, item)))
            exclude_dirs.append(item)
    if args.debug:
        print('Dir(s) to exclude:', exclude_dirs)
    # process files to check
    files = None
    if args.file is not None:
        files = find_a_file(src_dir, args.file, exclude_dirs)
    elif args.all:
        files = find_all_files(src_dir, exclude_dirs)
    else:
        files = find_last_changed_files(src_dir, exclude_dirs, args.timeout)
    pfiles, cfiles = separate_files(files, args.debug)
    # Python file check
    ret = pep8_format(pfiles, args.dry, args.debug, args.timeout)
    if ret != 0:
        sys.exit(ret)
    ret = flake8_check(flake8_config, mypy_config, pfiles, args.dry, args.debug, args.timeout)
    if ret != 0:
        sys.exit(ret)
    # C/C++ file check
    ret = force_format(format_config, cfiles, args.dry, args.debug, args.timeout)
    if ret != 0:
        sys.exit(ret)
    ret = check_naming(tidy_config, cfiles, build_path, args.dry, args.debug, args.timeout)
    sys.exit(ret)


if __name__ == "__main__":
    main()
