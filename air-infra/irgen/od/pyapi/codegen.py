#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Generate API code for DSL operators of different domains
"""

import shutil
import subprocess
import logging
from pathlib import Path
from functools import reduce
from domain import DomainDescEntry  # noqa: T484
from op import OpDescEntry          # noqa: T484
from cg.config import Config        # noqa: T484

IDT = "\t"
ARG_PLACEHOLDER = "ArgRep.UNCOMPILED"


class PyAPICodeGen:
    '''
    Generate API code for DSL operators of different domains
    '''

    def __init__(self, domain: DomainDescEntry):
        self._domain = domain
        self._macro_name = ""

    @property
    def domain(self) -> DomainDescEntry:
        ''' Get the domain. '''
        return self._domain

    @property
    def macro_name(self) -> str:
        ''' Get the macro name. '''
        return self._macro_name

    def gen_call_macro(self):
        '''
        Generate the CallMacro class for DSL the domain
        '''
        self._macro_name = f"{self.domain.name}CallMacro"
        res = f"class {self.macro_name}(CallMacro):\n"

        # generate the _on_Call method
        res += IDT + "@classmethod\n"
        res += IDT + "def _on_Call(cls, visitor: ast.NodeVisitor, args):  # noqa: N802\n"
        res += IDT * 2 + \
            f"return cls._{self.domain.name.lower()}_on_Call(visitor, args)\n\n"

        # generate the argreps function
        res += IDT + "@abstractmethod\n"
        res += IDT + "def argreps(cls, args):\n"
        res += IDT * 2 + "pass\n\n"

        # generate the _{domain}_on_Call method
        res += IDT + "@abstractmethod\n"
        res += IDT + \
            f"def _{self.domain.name.lower()}" + \
            "_on_Call(visitor, args):  # noqa N802\n"
        res += IDT * 2 + "pass\n\n"

        return res + "\n"

    def gen_import(self):
        '''
        Generate the import statement for the domain
        '''
        res = f'''import ast
from abc import abstractmethod
from pydsl.macro import ArgRep, CallMacro  # noqa T400
from air_dsl import PrimTypeEnum, {self.domain.name}API\n\n'''
        return res

    def gen_header(self):
        '''
        Generate the header for the domain
        '''
        res = f'''#!/usr/bin/env python3 # noqa T499
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
"""
Define the {self.domain.name} API in Python dsl.
"""
'''
        return res + "\n"

    def gen_op(self, op: OpDescEntry):
        ''' Generate the API code for the operator '''

        res = f"class {op.name.title().replace('_', '')}({self.macro_name}):\n"
        res += f'{IDT}"""\n{IDT}Handle the function {op.name.capitalize()}.\n{IDT}"""\n'''

        # generate the argreps function
        res += IDT + "def argreps():\n"
        res += IDT * 2 + \
            "return [" + ", ".join([ARG_PLACEHOLDER] * op.arg_num) + "]\n\n"

        # generate the _{domain}_on_Call method
        res += IDT + f"def _{self.domain.name.lower()}_on_Call(visitor, args):  # noqa N802\n"

        # generate spos
        res += IDT * 2 + "spos = visitor._scope_manager.exprs[0].Spos()\n"

        # generate get dsl
        res += IDT * 2 + "dsl = visitor._air\n"

        # generate domain api object
        res += IDT * 2 + f"api = {self.domain.name}API(dsl)\n"

        # visit the arguments
        for i in range(op.arg_num):
            res += IDT * 2 + f"res_{i} = visitor.visit(args[{i}])\n"

            # argument is not declared
            if len(op.args) == 0:
                continue

            # TODO: refine this hardcode
            if op.args[i].type == "int":
                res += IDT * 2 + \
                    f"res_{i} = dsl.intconst(PrimTypeEnum.INT_S32, res_{i}, spos)\n"

        # generate return statement
        args = ", ".join([f"res_{i}" for i in range(op.arg_num)] + ["spos"])
        res += IDT * 2 + f"return api.{op.name}({args})\n\n"

        return res + "\n"

    def gen_ops(self):
        ''' Generate the API code for all operators '''
        res = ""
        for op in self.domain.ops:
            res += self.gen_op(op)
        return res

    def generate(self, output_dir: str):
        '''
        Generate the API code for the domain
        '''
        res = self.gen_header()
        res += self.gen_import()
        res += self.gen_call_macro()
        res += self.gen_ops()

        with open(output_dir + f"/{self.domain.name}.py", 'w', encoding='utf-8') as f:
            f.write(res)
        return res


HEADER_FILE_INCLUDE = ["py_airgen.h"]
NAMESPACE = ["air", "dsl"]
API_GEN_DIRVER = "PY_AIRGEN"


class CXXAPIConfig(Config):
    ''' Config class for the code generator '''

    def __init__(self):
        super().__init__()
        self._clang_format_cfg = Path("")
        for h in HEADER_FILE_INCLUDE:
            self.add_include(h, False)

    def set_clang_format_cfg(self, cfg: str) -> 'CXXAPIConfig':
        ''' Set clang-format config file '''
        p = Path(cfg)
        if not p.exists():
            logging.info("clang-format config file %s does not exist", cfg)
            return self
        self._clang_format_cfg = p
        return self

    @property
    def clang_format_cfg(self) -> Path:
        ''' Get clang-format config file '''
        return self._clang_format_cfg


class CXXAPICodeGen:
    '''
    Generate CXX API code for DSL operators of different domains
    '''

    def __init__(self, domain: DomainDescEntry, config: CXXAPIConfig):
        self._domain = domain
        self._config = config

    @property
    def domain(self) -> DomainDescEntry:
        ''' Get the domain. '''
        return self._domain

    @property
    def config(self) -> CXXAPIConfig:
        ''' Get the config. '''
        return self._config

    def run_clang_format(self, file_path: str, cfg_file: Path):
        ''' Run clang-format to format the file '''
        clang_fmt = shutil.which("clang-format")
        if clang_fmt is None:
            logging.info("clang-format is not found")
            return
        try:
            if cfg_file.exists():
                subprocess.run([clang_fmt, "-i", "-style=file",
                               "-assume-filename", cfg_file, file_path], check=True)
            else:
                subprocess.run([clang_fmt, "-i", file_path], check=True)
        except subprocess.CalledProcessError as e:
            logging.error("Failed to run clang-format %s:", e)

    def gen_header(self):
        '''Generate the header for the domain'''
        res = '''//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================\n
'''
        return res

    def gen_guard_begin(self):
        ''' Generate the guard for the domain '''
        res = f"#ifndef {self.domain.name.upper()}_API_H\n"
        res += f"#define {self.domain.name.upper()}_API_H\n\n"
        return res

    def gen_guard_end(self):
        ''' Generate the guard for the domain '''
        return "#endif\n"

    def gen_header_include(self):
        ''' Generate the include statement for the domain '''
        res = "\n".join(
            [f"#include <{header}>" for header in self.config.cpp_lib_include])
        res += "\n".join(
            [f"#include \"{header}\"" for header in self.config.cpp_user_include])
        return res + "\n\n"

    def gen_cxx_include(self):
        ''' Generate the include statement for the domain '''
        res = f"#include \"{self.domain.name.lower()}_ops.h\"\n"
        return res + "\n\n"

    def gen_namespace_begin(self):
        ''' Generate the namespace for the domain '''
        res = "::".join(NAMESPACE)
        return f"namespace {res} {{\n"

    def gen_constructor(self):
        ''' Generate the constructor for the API class '''
        cls_name = f"{self.domain.name.upper()}_API"
        res = f"{IDT}{cls_name}::{cls_name}({API_GEN_DIRVER}& dsl) : _dsl(dsl) {{}}\n\n"
        return res

    def gen_namespace_end(self):
        ''' Generate the namespace for the domain '''
        return "}" + f"  //namesapce {'::'.join(NAMESPACE)}\n\n"

    def gen_op_sig(self, res: str, op: OpDescEntry):
        ''' Generate the API code for the operator '''
        res += f"{IDT}NODE_PTR {op.name.capitalize()}("
        if op.arg_num == 0:
            res += "const SPOS& spos);\n"
            return res
        if len(op.args) > 0:
            res += ", ".join([f"NODE_PTR {arg.name}" for arg in op.args])
        else:
            res += ", ".join([f"NODE_PTR op{i}" for i in range(op.arg_num)])
        res += ", const SPOS& spos);\n"
        return res

    def gen_op_impl(self, res: str, op: OpDescEntry):
        ''' Generate the API code for the operator '''
        res += f"NODE_PTR {self.domain.name.upper()}_API::{op.name.capitalize()}("
        res += ", ".join([f"NODE_PTR op{i}" for i in range(op.arg_num)])
        res += (", " if op.arg_num > 0 else "") + "const SPOS& spos) {\n"

        # Special case for encode operation - it should return PLAINTEXT type
        if op.name == 'encode' and self.domain.name.lower() == 'ckks':
            res += f"{IDT}CONTAINER& cntr = _dsl.Get_cur_func_scope()->Container();\n"
            res += f"{IDT}// Get PLAINTEXT type using PY_AIRGEN::Get_plain_type() method\n"
            res += f"{IDT}TYPE_PTR plain_type = _dsl.Get_plain_type(spos);\n"
            res += f"{IDT}// Use New_cust_node with PLAINTEXT type (correct approach)\n"
            ns = "::".join(self.config.get_namespaces())
            opcode = f"air::base::OPCODE({ns}::{self.domain.name.upper()}," \
                f" {ns}::OPCODE::{op.name.upper()})"
            res += f"{IDT}OPCODE encode_op({opcode});\n"
            res += f"{IDT}NODE_PTR encode_node = cntr.New_cust_node(encode_op, plain_type, spos);\n"
            for i in range(op.arg_num):
                res += f"{IDT}encode_node->Set_child({i}, op{i});\n"
            res += f"{IDT}return encode_node;\n"
            res += "}\n\n"
            return res

        # call New_xxx_arith function to create the node
        ret_exp = f"{IDT}return _dsl.Get_cur_func_scope()->Container()."
        api_map = {
            1: "una",
            2: "bin",
            3: "tern",
            4: "quad",
            5: "quint",
            6: "sext",
        }
        def is_invalid(op): return op.name == 'invalid'
        if is_invalid(op):
            # get glob_scope
            res += f"{IDT}GLOB_SCOPE& glob_scope = _dsl.Get_cur_func_scope()->Glob_scope();\n"

            # hard code rtype of op without child as void type now
            res += ret_exp + "New_invalid("
        else:
            res += ret_exp + f"New_{api_map[op.arg_num]}_arith("

        # generate OPCODE
        ns = "::".join(self.config.get_namespaces())
        logging.debug("namespaces: %s", ns)

        opcode = f"air::base::OPCODE({ns}::{self.domain.name.upper()}," \
            f" {ns}::OPCODE::{op.name.upper()}), "
        rtype = "op0->Rtype()," if not is_invalid(op) else ''
        ops = ", ".join([f"op{i}" for i in range(op.arg_num)])
        spos = f"{', ' if not is_invalid(op) else ''} spos"
        res += f"{opcode} {rtype} {ops} {spos});\n"
        res += "}\n\n"
        return res

    def gen_api_class(self):
        ''' Generate the API class for the domain '''
        cls_name = f"{self.domain.name.upper()}_API"
        res = f"class {cls_name} {{\n"
        res += "public:\n"

        # generate the constructor
        res += f"{IDT}{cls_name}({API_GEN_DIRVER}& dsl);\n\n"

        # generate the API functions
        res = reduce(self.gen_op_sig, self.domain.ops, res)

        # generate the private members
        res += "private:\n"
        res += f"{IDT}{API_GEN_DIRVER}& _dsl;\n"

        res += "};\n\n"
        return res

    def gen_binding(self):
        ''' Generate the binding code for the domain '''
        cls_name = f"{self.domain.name.upper()}_API"
        def cap(s): return s if s[0].isupper() else s.capitalize()
        res = f"py::class_<{cls_name}>(m, \"{cap(self.domain.name)}API\")\n"
        res += f"{IDT}.def(py::init<{API_GEN_DIRVER}&>())\n"
        ret_policy = "py::return_value_policy::reference"
        for op in self.domain.ops:
            op_name = op.name.capitalize()
            res += f"{IDT}.def(\"{op.name}\", &{cls_name}::{op_name}, {ret_policy})\n"
        return res[:-1] + ";\n"

    def gen_header_file(self):
        ''' Generate the header file for the domain '''
        res = self.gen_header()
        res += self.gen_guard_begin()
        res += self.gen_header_include()
        res += self.gen_namespace_begin()
        res += self.gen_api_class()
        res += self.gen_namespace_end()
        res += self.gen_guard_end()
        return res

    def gen_cxx_file(self):
        ''' Generate the cxx file for the domain '''
        res = self.gen_header()
        res += self.gen_cxx_include()
        res += self.gen_namespace_begin()
        res += self.gen_constructor()
        res = reduce(self.gen_op_impl, self.domain.ops, res)
        res += self.gen_namespace_end()

        return res

    def generate(self, output_dir: str):
        ''' Generate the API code for the domain '''
        res = self.gen_header_file()
        header_path = output_dir + f"/{self.domain.name.lower()}_ops.h"
        with open(header_path, 'w', encoding='utf-8') as f:
            f.write(res)
        # print(res)
        self.run_clang_format(
            header_path, cfg_file=self.config.clang_format_cfg)

        res = self.gen_cxx_file()
        cxx_path = output_dir + f"/{self.domain.name.lower()}_ops.cxx"
        with open(cxx_path, 'w', encoding='utf-8') as f:
            f.write(res)
        self.run_clang_format(cxx_path, cfg_file=self.config.clang_format_cfg)

        res = self.gen_binding()
        binding_path = output_dir + f"/{self.domain.name.lower()}_binding.h"
        with open(binding_path, 'w', encoding='utf-8') as f:
            f.write(res)
        # print(res)


class APICodeGen:
    '''
    Driver of pyapi codegen and cxxapi codegen
    '''

    def __init__(self, domain: DomainDescEntry, config: CXXAPIConfig):
        self._domain = domain
        self._config = config

    @property
    def domain(self) -> DomainDescEntry:
        ''' Get the domain. '''
        return self._domain

    @property
    def config(self) -> CXXAPIConfig:
        ''' Get the config. '''
        return self._config

    def generate(self, output_dir: str):
        ''' Generate the API code for the domain '''
        PyAPICodeGen(self.domain).generate(output_dir)
        CXXAPICodeGen(self.domain, self.config).generate(output_dir)
