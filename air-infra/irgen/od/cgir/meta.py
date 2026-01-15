#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Defines metadata for CGIR.
'''

from typing import Optional, List
from od.cgir.enum import OpndKind, RegClass  # noqa: T484

REG_UNKNOWN = 255


class OpndMeta:
    '''
    OPND_META in CGIR.
    '''

    def __init__(self, opnd_kind: OpndKind,
                 reg_class: RegClass = RegClass.UNKNOWN,
                 reg_num: int = REG_UNKNOWN, size_mask: str = "") -> None:
        ''' init OPND_META. '''
        self._opnd_kind = opnd_kind
        self._reg_class = reg_class
        self._reg_num = reg_num
        self._size_mask = size_mask
        self._domain = ""

    @property
    def opnd_kind(self) -> OpndKind:
        ''' Get operand kind. '''
        return self._opnd_kind

    def set_opnd_kind(self, opnd_kind: OpndKind) -> 'OpndMeta':
        ''' Set operand kind. '''
        self._opnd_kind = opnd_kind
        return self

    @property
    def reg_class(self) -> RegClass:
        ''' Get register class. '''
        return self._reg_class

    def set_reg_class(self, reg_class: RegClass) -> 'OpndMeta':
        ''' Set register class. '''
        self._reg_class = reg_class
        return self

    @property
    def reg_num(self) -> int:
        ''' Get register number. '''
        return self._reg_num

    def set_reg_num(self, reg_num: int) -> 'OpndMeta':
        ''' Set register number. '''
        self._reg_num = reg_num
        return self

    @property
    def size_mask(self) -> str:
        ''' Get size mask. '''
        return self._size_mask

    def set_size_mask(self, size_mask: str) -> 'OpndMeta':
        ''' Set size mask. '''
        self._size_mask = size_mask
        return self

    @property
    def domain(self) -> str:
        ''' Get domain. '''
        return self._domain

    def set_domain(self, domain: str) -> 'OpndMeta':
        ''' Set domain. '''
        self._domain = domain
        return self

    def __str__(self) -> str:
        ''' Convert to string. '''
        assert self._domain != ""
        return "{" + f"{self.opnd_kind}, {self.reg_class}, " + \
            f"{self.reg_num}, {self.domain}_{self.size_mask}" + "}"


class InstMeta:
    '''
    INST_META in CGIR.
    '''

    def __init__(self, name: str, code: int, res_cnt: int) -> None:
        ''' init INST_META. '''
        self._name = name
        self._code = code
        self._res_cnt = res_cnt
        self._res: Optional[OpndMeta] = None
        self._opnds: List[OpndMeta] = []
        self._domain = ""

    @property
    def name(self) -> str:
        ''' Get instruction name. '''
        return self._name

    def set_name(self, name: str) -> 'InstMeta':
        ''' Set instruction name. '''
        self._name = name
        return self

    @property
    def code(self) -> int:
        ''' Get instruction code. '''
        return self._code

    def set_code(self, code: int) -> 'InstMeta':
        ''' Set instruction code. '''
        self._code = code
        return self

    @property
    def res_cnt(self) -> int:
        ''' Get result count. '''
        return self._res_cnt

    def set_res_cnt(self, res_cnt: int) -> 'InstMeta':
        ''' Set result count. '''
        self._res_cnt = res_cnt
        return self

    @property
    def opnds(self) -> list:
        ''' Get result operands. '''
        return self._opnds

    def add_opnd(self, opnd: OpndMeta) -> 'InstMeta':
        ''' Add operand. '''
        self._opnds.append(opnd)
        return self

    @property
    def res(self) -> Optional[OpndMeta]:
        ''' Get result operand. '''
        return self._res

    def set_res(self, res: Optional['OpndMeta']) -> Optional['InstMeta']:
        ''' Set result operand. '''
        self._res = res
        return self

    @property
    def domain(self) -> str:
        ''' Get domain. '''
        return self._domain

    def set_domain(self, domain: str) -> 'InstMeta':
        ''' Set domain. '''
        self._domain = domain
        return self

    def __str__(self) -> str:
        ''' Convert to string. '''
        assert self._domain != ""
        assert self._res is not None
        opnds = ", ".join([str(opnd.set_domain(self.domain))
                          for opnd in self._opnds])
        prefix = f"static constexpr struct INST_META {self.domain}_INST_META_"
        prefix += self._name.upper() + " = "
        value = '{' + f'"{self.name}", {self.code}, {self.res_cnt}, ' + \
            f"{len(self.opnds)}, " + "{" + \
            f"{self._res.set_domain(self.domain)}, {opnds}" + \
            "}" + "};"
        return prefix + value
