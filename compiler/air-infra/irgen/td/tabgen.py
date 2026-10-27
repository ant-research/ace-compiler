#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
The driver of TD
'''

import argparse
from target.cg import TDCodeGen
from target.parser import TDParser


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='TD driver')
    parser.add_argument('-i', '--input', nargs='+',
                        help='input yaml files', required=True)
    parser.add_argument(
        '-o', '--output', help='output directory', required=True)
    parser.add_argument('-t', '--target', action='store_true',
                        help='generate source code for target info')
    args = parser.parse_args()

    if args.target:
        parser = TDParser(args.input)
        targets = parser.parse()
        TDCodeGen(targets, args.output).generate()
