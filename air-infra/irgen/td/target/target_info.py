#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Target info.
'''

from .reg import RegCls  # noqa: T484


class TargetInfo:
    '''
    Represents TARG_INFO_META in air-infra.
    '''

    def __init__(self, name: str, isa_id: str) -> None:
        self._name = name
        self._isa_id = isa_id
        self._guard = ""
        self._headers = ""
        self._ns: list = []
        self._global_vars: list = []
        self._reg_cls: list = []
        self._abi = None
        self._inst_meta: list = []
        self._op_flags: list = []
        self._includes: list = []
        self._used_macros: list = []

    def __str__(self) -> str:
        return f"Target {self._name}, {self._isa_id}\n"

    def set_guard(self, guard: str) -> 'TargetInfo':
        ''' Set guard. '''
        self._guard = guard
        return self

    def set_headers(self, headers: str) -> 'TargetInfo':
        ''' Set headers. '''
        self._headers = headers
        return self

    def set_namespace(self, decl: list, use: list) -> 'TargetInfo':
        ''' Set namespace. '''
        self._ns.append(decl)
        self._ns.append(use)
        return self

    def set_global_vars(self, global_vars: list) -> 'TargetInfo':
        ''' Set global variables. '''
        self._global_vars = global_vars
        return self

    def add_reg_cls(self, reg_cls: RegCls) -> 'TargetInfo':
        ''' Add register class. '''
        self._reg_cls.append(reg_cls)
        return self

    def set_abi(self, abi) -> 'TargetInfo':
        ''' Set ABI. '''
        self._abi = abi
        return self

    def set_inst_meta(self, inst_meta: list) -> 'TargetInfo':
        ''' Set instruction meta. '''
        self._inst_meta = inst_meta
        return self

    def set_op_flags(self, op_flags: list) -> 'TargetInfo':
        ''' Set op flags. '''
        self._op_flags = op_flags
        return self

    def set_includes(self, includes: list) -> 'TargetInfo':
        ''' Set includes. '''
        self._includes = includes
        return self

    def set_used_macros(self, used_macros: list) -> 'TargetInfo':
        ''' Set used macros. '''
        self._used_macros = used_macros
        return self

    @property
    def guard(self) -> str:
        ''' Getter for guard. '''
        return self._guard

    @property
    def headers(self) -> str:
        ''' Getter for headers. '''
        return self._headers

    @property
    def ns(self) -> list:
        ''' Getter for namespace. '''
        return self._ns

    @property
    def global_vars(self) -> list:
        ''' Getter for global variables. '''
        return self._global_vars

    @property
    def reg_cls(self) -> list:
        ''' Getter for register classes. '''
        return self._reg_cls

    @property
    def name(self) -> str:
        ''' Getter for name. '''
        return self._name

    @property
    def isa_id(self) -> str:
        ''' Getter for ISA ID. '''
        return self._isa_id

    @property
    def abi(self):
        ''' Getter for ABI. '''
        return self._abi

    @property
    def inst_meta(self) -> list:
        ''' Getter for instruction meta. '''
        return self._inst_meta

    @property
    def op_flags(self) -> list:
        ''' Getter for op flags. '''
        return self._op_flags

    @property
    def includes(self) -> list:
        ''' Getter for includes. '''
        return self._includes

    @property
    def used_macros(self) -> list:
        ''' Getter for used macros. '''
        return self._used_macros

    def is_none(self) -> bool:
        ''' Check if the target is None. '''
        return self._name == "None"
