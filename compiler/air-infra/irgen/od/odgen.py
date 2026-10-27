#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
The driver of OD
'''

import sys
import argparse
from pathlib import Path
try:
    sys.path.append(str(Path(__file__).resolve().parents[1]))
    from cg.opcode import (OpcodeHeaderConfig, OpcodeHeaderFileCG,
                           OpcodeCXXConfig, OpcodeCXXFileCG, OpcodeWrite)
    from parser import ODParser
    from pyapi.codegen import APICodeGen, CXXAPIConfig
except Exception as e:
    print(f"Failed to import OD: {e}")
    sys.exit(1)


def codegen(p, input, output_dir):
    ''' Generate CXX/H files for OD. '''
    p.parse()
    header_config = (OpcodeHeaderConfig()
                     .set_macro_guard(p.guard))
    cxx_config = OpcodeCXXConfig()
    for ns in p.ns_decl:
        header_config.add_namespace(ns)
        cxx_config.add_namespace(ns)
    header_cg = OpcodeHeaderFileCG(header_config, p.domain)
    cxx_cg = OpcodeCXXFileCG(cxx_config, p.domain)

    (header, _, _) = (OpcodeWrite(output_dir)
                      .set_header_cg(header_cg)
                      .set_cxx_cg(cxx_cg)
                      .set_od_path(input)
                      .write_file())

    cxx_api_config = CXXAPIConfig()
    # TODO: read clang-format config from the yml file  # noqa: W0511
    cxx_api_config.set_clang_format_cfg("/code/air-infra/.clang-format")
    for ns in p.ns_decl:
        cxx_api_config.add_namespace(ns)
    cxx_api_config.add_include(header.absolute(), False)
    APICodeGen(p.domain, cxx_api_config).generate(output_dir)


if __name__ == "__main__":
    args = argparse.ArgumentParser(description='OD driver')
    args.add_argument('-i', '--input',
                      help='input yaml files', required=True)
    args.add_argument('-o', '--output',
                      help='output directory', required=True)
    args = args.parse_args()

    parser = ODParser(args.input)
    codegen(parser, args.input, args.output)
