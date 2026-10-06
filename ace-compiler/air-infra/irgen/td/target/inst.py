#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Instruction Meta
'''

from enum import Enum


class OpndKind(Enum):
    """
    Operand kind for CGIR instructions.
    """
    REGISTER = 1   # Operand is a pseudo or physical register
    IMMEDIATE = 2  # Operand is an integer immediate
    CONSTANT = 3   # Operand references an entry in constant table
    SYMBOL = 4     # Operand references an entry in symbol table
    BB = 5         # Operand is a BB for local branch
    LAST = 6       # Last unused operand kind

    def __str__(self):
        return f"OPND_KIND::{self.name}"


OpndMap = {
    "reg": OpndKind.REGISTER,
    "imm": OpndKind.IMMEDIATE,
    "const": OpndKind.CONSTANT,
    "sym": OpndKind.SYMBOL,
    "bb": OpndKind.BB
}


class OpndMeta:
    ''' Operand meta. '''

    def __init__(self, kind: str, reg_cls: str, reg_num: str, size: str) -> None:
        self._kind = OpndMap[kind]
        self._reg_cls = reg_cls
        self._reg_num = reg_num
        self._size = size

    @property
    def kind(self) -> OpndKind:
        ''' Getter for kind. '''
        return self._kind

    @property
    def reg_cls(self) -> str:
        ''' Getter for reg_cls. '''
        return self._reg_cls

    @property
    def reg_num(self) -> str:
        ''' Getter for reg_num. '''
        return self._reg_num

    @property
    def size(self) -> str:
        ''' Getter for size. '''
        return self._size

    def __str__(self):
        s = "{ "
        s += f"{self.kind}, {self.reg_cls}, {self.reg_num}, {self.size}"
        s += " }"
        return s


class InstMeta:
    '''
    Represents INST_META in air-infra.
    '''

    def __init__(self, name: str, code: str, res_cnt: str, opnd_cnt: str, flag: str) -> None:
        self._name = name
        self._code = code
        self._res_cnt = res_cnt
        self._opnd_cnt = opnd_cnt
        self._flag = flag
        self._opnds: list = []

    @property
    def name(self) -> str:
        ''' Getter for name. '''
        return self._name

    @property
    def code(self) -> str:
        ''' Getter for code. '''
        return self._code

    @property
    def res_cnt(self) -> str:
        ''' Getter for res_cnt. '''
        return self._res_cnt

    @property
    def opnd_cnt(self) -> str:
        ''' Getter for opnd_cnt. '''
        return self._opnd_cnt

    @property
    def flag(self) -> str:
        ''' Getter for operator flag'''
        return self._flag

    @property
    def opnds(self) -> list:
        ''' Getter for opnds. '''
        return self._opnds

    def add_opnd(self, opnd: OpndMeta) -> 'InstMeta':
        ''' Add operand. '''
        self._opnds.append(opnd)
        return self

    def __str__(self):
        s = "{\n"
        s += f"  \"{self.name}\",\n"
        s += f"  {self.code},\n"
        s += f"  {self.res_cnt},\n"
        s += f"  {self.opnd_cnt},\n"
        s += f"  {self.flag},\n"
        s += "  {\n" + \
            ",\n".join([f"    {str(opnd)}" for opnd in self.opnds]) + "\n  }"
        s += "\n}"
        return s
