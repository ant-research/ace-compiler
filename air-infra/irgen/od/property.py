#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Defines an enumeration, `OprProp`, for operator properties.
'''

from enum import Enum

class OprProp(Enum):
    '''
    Enumeration of operator properties.
    '''
    NONE = 0           # No property, for INVALID operator
    EX_CHILD = 1       # Has extra child
    ENTRY = 2          # Operator has entry
    RET_VAR = 3        # Has return value to PREG
    SCF = 4            # Structured Control Flow
    NON_SCF = 5        # Non-Structured Control Flow
    END_BB = 6         # Operator ends current bb
    LEAF = 7           # Operator is leaf node
    STMT = 8           # Operator is statement
    EXPR = 9           # Operator is expression
    STORE = 10         # Operator is store
    LOAD = 11          # Operator is load
    CALL = 12          # Operator is call
    COMPARE = 13       # Operator is compare
    BOOLEAN = 14       # Operator generates boolean result
    NOT_EXEC = 15      # Operator isn't executable
    SYM = 16           # Operator has symbol
    TYPE = 17          # Operator has type
    LABEL = 18         # Operator has label
    OFFSET = 19        # Operator has offset
    VALUE = 20         # Operator has value
    FLAGS = 21         # Operator has flag
    FIELD_ID = 22      # Operator has field id
    CONST_ID = 23      # Operator has const id
    BARRIER = 24       # Operator is barrier
    PREFETCH = 25      # Operator is prefetch
    ATTR = 26          # Operator has attribute
    ACC_TYPE = 27      # Operator has access type
    PREG = 28          # Operator has preg
    LIB_CALL = 29      # Operator is mapped to library call
    LAST_PROP = 30     # Last operator property, should never be used

    def __str__(self) -> str:
        if self == OprProp.NONE:
            return "0"
        return "PROP_" + self.name
