#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Defines enumerations for CGIR metadata.
'''

from enum import Enum


class OpndKind(Enum):
    '''
    Enumeration of operand kinds.
    '''
    REGISTER = 0  # Operand is a pseudo or physical register
    IMMEDIATE = 1  # Operand is an integer immediate
    CONSTANT = 2  # Operand references an entry in constant table
    SYMBOL = 3    # Operand references an entry in symbol table
    LABEL = 4     # Operand is an label

    def __str__(self) -> str:
        return "OPND_KIND::" + self.name


class RegClass(Enum):
    '''
    Enumeration of register classes.
    '''
    UNKNOWN = 0         # Unknown register class
    CSR = 1             # Control and status register
    GPR = 2             # General purpose register
    FPR = 3             # Floating point register
    VER = 4             # Vector extension register
    TARG_SPEC = VER + 1  # Target specific register starts from VER + 1

    def __str__(self) -> str:
        return "REG_CLASS::" + self.name
