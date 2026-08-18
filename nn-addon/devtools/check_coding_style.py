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

def find_all_files(cwd, exclude_dirs):
    '''
    Find all C/C++ & Python files under cwd
    '''
    res = []
    for root, _, files in os.walk(cwd):
        res.extend(find_valid_files(root, exclude_dirs, files))
    return res

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

def find_last_changed_files(cwd, exclude_dirs, time_out):
    '''
    Find latest changed files,
    If there's no diff files in cwd,
    Return diff files in last commit
    '''
    res = []
    # look for diff files of current branch first, ignore deleted & unmerged files
    cmds = ['git', 'diff', 'origin/HEAD', '--name-only', '--diff-filter=du']
    ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=time_out)
    if ret.returncode == 0:
        files = ret.stdout.decode().splitlines()
        res.extend(find_valid_files(cwd, exclude_dirs, files))
    # look for cached diff files, ignore deleted files
    cmds = ['git', 'diff', '--cached', '--name-only', '--diff-filter=d']
    ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=time_out)
    if ret.returncode == 0:
        files = ret.stdout.decode().splitlines()
        for file in find_valid_files(cwd, exclude_dirs, files):
            if file not in res:
                res.append(file)
    # if no diff file, look for files in last commit/merge, ignore deleted files
    if len(res) == 0:
        cmds = ['git', 'log', '-m', '-1', '--name-only', '--diff-filter=d', '--pretty=']
        ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=time_out)
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

def pylint_check(files, dry_run, debug, time_out):
    '''
    Check python coding style via Pylint
    '''
    ret_val = 0
    for file in files:
        cmds = ['pylint', file]
        # ignore checks
        cmds.extend(['-d', 'W1510'])
        if debug:
            print(' '.join(cmds))
        ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=time_out)
        if ret.returncode != 0:
            ret_val = 1
            if dry_run:
                print('Coding style issues in', file)
            else:
                print(ret.stdout.decode())
    return ret_val

def check_naming(files, build, dry_run, debug, time_out):
    '''
    Check naming convention
    If there're issues, returns 1, else returns 0
    '''
    ret_val = 0
    config_file = os.path.abspath('./devtools/.clang-tidy')
    for file in files:
        cmds = ['clang-tidy', '-p', build, '--config-file='+config_file, file]
        if debug:
            print(' '.join(cmds))
        ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=time_out)
        errors = ret.stdout.decode().splitlines()
        has_issue = False
        for item in errors:
            if item.find('warning: invalid case style') != -1:
                has_issue = True
                break
        if has_issue:
            ret_val = 1
            if dry_run:
                print('Naming convention issue in', file)
            else:
                for item in errors:
                    print(item)
    return ret_val

def force_format(files, dry_run, debug, time_out):
    '''
    Correct formatting automatically
    '''
    config_file = os.path.abspath('./devtools/.clang-format')
    for file in files:
        cmds = ['clang-format', '-style=file:'+config_file, file]
        if dry_run:
            cmds.extend(['-n', '-Werror'])
        else:
            cmds.append('-i')
        if debug:
            print(' '.join(cmds))
        ret = subprocess.run(cmds, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=time_out)
        if ret.returncode != 0:
            if dry_run:
                print('Format issue in', file)
            else:
                print('Failed to correct format issue in', file)

def main():
    '''
    Main function
    '''
    parser = argparse.ArgumentParser(description='Check Coding Style of C/C++ & Python files',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-a', '--all', action='store_true', default=False,
                        help='if set, check all C/C++ & Python files')
    parser.add_argument('-b', '--build', metavar='PATH', default='./build',
                        help='build path that contains \'compile_commands.json\'')
    parser.add_argument('-d', '--debug', action='store_true', default=False,
                        help='if set, print out debug info')
    parser.add_argument('-f', '--file', metavar='FILE',
                        help='single C/C++ or Python file to check')
    parser.add_argument('-n', '--dry', action='store_true', default=False,
                        help='dry run, print out if a file has issues only')
    parser.add_argument('-x', '--exclude', metavar='DIR', nargs='*',
                        help='exclude dir(s) from checking')
    parser.add_argument('-t', '--timeout', type=int, default=60,
                        help='set subprocess timeout period when checking')
    args = parser.parse_args()
    cwd = os.getcwd()
    # check .clang-tidy & .clang-format files
    format_config = os.path.join(cwd, './devtools/.clang-format')
    if not os.path.exists(format_config):
        print(format_config, 'does not exists!')
        sys.exit(-1)
    tidy_config = os.path.join(cwd, './devtools/.clang-tidy')
    if not os.path.exists(tidy_config):
        print(tidy_config, 'does not exists!')
        sys.exit(-1)
    # build option
    build_path = os.path.join(cwd, args.build)
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
        files = find_a_file(cwd, args.file, exclude_dirs)
    elif args.all:
        files = find_all_files(cwd, exclude_dirs)
    else:
        files = find_last_changed_files(cwd, exclude_dirs, args.timeout)
    pfiles, cfiles = separate_files(files, args.debug)
    # Python file check
    ret = pylint_check(pfiles, args.dry, args.debug, args.timeout)
    if ret != 0:
        sys.exit(ret)
    # C/C++ file check
    force_format(cfiles, args.dry, args.debug, args.timeout)
    ret = check_naming(cfiles, build_path, args.dry, args.debug, args.timeout)
    sys.exit(ret)

if __name__ == "__main__":
    main()
