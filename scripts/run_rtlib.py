#!/usr/bin/env python3
import os
import sys
import argparse
import datetime
import subprocess


def _discover_executables(example_dir):
    out = []
    for entry in sorted(os.listdir(example_dir)):
        p = os.path.join(example_dir, entry)
        if os.path.isfile(p) and os.access(p, os.X_OK):
            out.append(entry)
    return out


def run_one(example_dir, name, timeout, debug):
    path = os.path.join(example_dir, name)
    if not os.path.isfile(path):
        return -1, 'not found: %s' % path
    try:
        ret = subprocess.run([path], stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT, timeout=timeout)
        out = ret.stdout.decode(errors='replace')
        if debug:
            print(out)
        return ret.returncode, out.splitlines()[-1] if out.splitlines() else ''
    except subprocess.TimeoutExpired:
        return 124, 'timeout'
    except Exception as e:
        return -2, str(e)


def main():
    parser = argparse.ArgumentParser(
        description='Run 7.5 runtime testcases (eg_fhertlib_* + models)')
    parser.add_argument('-e', '--example_dir', required=True,
                        help='rtlib example build dir (all executables in '
                             'this dir are run)')
    parser.add_argument('-o', '--output_dir', default='.',
                        help='where to write the .rtlib.log')
    parser.add_argument('-d', '--debug', action='store_true', default=False,
                        help='print each case output to stdout')
    parser.add_argument('--timeout', type=int, default=300,
                        help='timeout per case in seconds (default 300)')
    args = parser.parse_args()

    if not os.path.isdir(args.example_dir):
        print('Error: example dir not found: %s' % args.example_dir,
              file=sys.stderr)
        sys.exit(-1)

    cases = _discover_executables(args.example_dir)
    if not cases:
        print('Error: no executables in %s' % args.example_dir, file=sys.stderr)
        sys.exit(-1)

    cwd = os.path.abspath(args.output_dir)
    date_time = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M")
    log_file_name = '7.5_' + date_time + '.rtlib.log'
    log = open(os.path.join(cwd, log_file_name), 'w')
    log.write('#### log for: %s\n' % ' '.join(sys.argv))
    log.write('## 7.5 Runtime testcase\n\n')
    log.write('| 测试用例 | 测试结果 | 备注 |\n')
    log.write('|---|---|---|\n')

    total = len(cases)
    passed = 0
    for name in cases:
        rc, tail = run_one(args.example_dir, name, args.timeout, args.debug)
        ok = (rc == 0)
        if ok:
            passed += 1
        log.write('| %s | %s | rc=%d %s |\n' %
                  (name, 'PASS' if ok else 'FAIL', rc, tail))
        log.flush()
        print('%s: %s' % (name, 'PASS' if ok else 'FAIL (rc=%d %s)' % (rc, tail)))

    log.write('\nSummary: %d/%d PASS\n' % (passed, total))
    log.close()
    if passed != total:
        sys.exit(1)


if __name__ == "__main__":
    main()
