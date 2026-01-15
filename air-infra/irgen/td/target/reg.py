#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Register related data structure.
'''

from enum import IntFlag


class RegFlag(IntFlag):
    ''' Register flags. '''
    REG_ALLOCATABLE = 0x0001  # allocatable
    REG_PARAMETER = 0x0002  # parameter passing
    REG_RET_VALUE = 0x0004  # return value
    REG_SCRATCH = 0x0008  # temporary register
    REG_PRESERVED = 0x0010  # callee-saved register
    REG_READONLY = 0x0020  # readonly, writes are ignored
    REG_RET_ADDR = 0x0040  # return address
    REG_GLOBAL_PTR = 0x0080  # global pointer
    REG_STACK_PTR = 0x0100  # stack pointer
    REG_THREAD_PTR = 0x0200  # thread pointer
    REG_ZERO = 0x0400  # fixed value of zero

    def __str__(self) -> str:
        l = len(self.__class__.__name__) + 1
        return super().__str__()[l:]


def to_flags(flag_list: list) -> RegFlag:
    """
    Convert a list of flag names to their corresponding RegFlag value.
    """
    # Map of flag names to RegFlag values
    flag_map = {
        "allocatable": RegFlag.REG_ALLOCATABLE,
        "parameter": RegFlag.REG_PARAMETER,
        "ret_value": RegFlag.REG_RET_VALUE,
        "scratch": RegFlag.REG_SCRATCH,
        "preserved": RegFlag.REG_PRESERVED,
        "readonly": RegFlag.REG_READONLY,
        "ret_addr": RegFlag.REG_RET_ADDR,
        "global_ptr": RegFlag.REG_GLOBAL_PTR,
        "stack_ptr": RegFlag.REG_STACK_PTR,
        "thread_ptr": RegFlag.REG_THREAD_PTR,
        "zero": RegFlag.REG_ZERO,
    }

    # Combine all matching flags
    flags = RegFlag(0)
    for flag_name in flag_list:
        if flag_name.lower() in flag_map:
            flags |= flag_map[flag_name.lower()]
        else:
            raise ValueError(f"Unknown flag: {flag_name}")

    return flags


class RegMeta:
    ''' Represents REG_META in air-infra. '''

    def __init__(self, name: str, code: str, size: str) -> None:
        self._name = name
        self._code = code
        self._size = size
        self._flags = RegFlag(0)

    @property
    def name(self) -> str:
        ''' Getter for name. '''
        return self._name

    @property
    def code(self) -> str:
        ''' Getter for code. '''
        return self._code

    @property
    def size(self) -> str:
        ''' Getter for size. '''
        return self._size

    @property
    def flags(self) -> RegFlag:
        ''' Getter for flags. '''
        return self._flags

    def set_flags(self, flags: RegFlag) -> 'RegMeta':
        ''' Set flags. '''
        self._flags = flags
        return self

    def __str__(self) -> str:
        return f"RegMeta(name={self.name}, code={self.code}, " \
            f"size={self.size}, flags={str(self.flags)})"


class RegCls:
    ''' Represents REG_INFO_META in air-infra.'''

    def __init__(self, name: str, cid: str) -> None:
        self._name = name
        self._id = cid
        self._size = "0"
        self._regs: list = []

    def __str__(self) -> str:
        return f"RegCls(name={self.name}, id={self.id}, size={self.size})"

    @property
    def name(self) -> str:
        ''' Getter for name. '''
        return self._name

    @property
    def id(self) -> str:
        ''' Getter for id. '''
        return self._id

    @property
    def regs(self) -> list:
        ''' Getter for registers. '''
        return self._regs

    @property
    def length(self) -> str:
        ''' Getter for length. '''
        return str(len(self._regs))

    @property
    def size(self) -> str:
        ''' Getter for size. '''
        return self._size

    def add_reg(self, reg: RegMeta) -> 'RegCls':
        ''' Add register. '''
        self._regs.append(reg)
        return self

    def set_size(self, size: str) -> 'RegCls':
        ''' Set size. '''
        self._size = size
        return self


UNKNOWN = 255


class RegConv:
    ''' Represents REG_CONV_META in air-infra. '''

    def __init__(self, name: str, cls: int) -> None:
        self._name = name
        self._cls = cls
        self._zero = UNKNOWN
        self._param: list = []
        self._ret: list = []

    @property
    def name(self) -> str:
        ''' Getter for name. '''
        return self._name

    @property
    def cls(self) -> int:
        ''' Getter for cls. '''
        return self._cls

    @property
    def zero(self) -> int:
        ''' Getter for reg_zero. '''
        return self._zero

    @property
    def param(self) -> list:
        ''' Getter for param. '''
        return self._param

    @property
    def ret(self) -> list:
        ''' Getter for ret. '''
        return self._ret

    def set_zero(self, zero: int) -> 'RegConv':
        ''' Set zero. '''
        self._zero = zero
        return self

    def set_param(self, param: list) -> 'RegConv':
        ''' Add param. '''
        self._param = param
        return self

    def set_ret(self, ret: list) -> 'RegConv':
        ''' Add ret. '''
        self._ret = ret
        return self

    def __str__(self) -> str:
        res = "{\n  " + f"{self.cls}, {self.zero}, {len(self.param)}, {len(self.ret)},\n"
        res += "  {\n    " + ", ".join(str(i) for i in self.param) + ",\n    "
        res += ", ".join(str(i) for i in self.ret) + "\n  }\n}"
        return res


class ABIMeta:
    ''' Calling ABI_INFO_META for registers. '''

    def __init__(self, name: str, id: str) -> None:
        self._name = name
        self._id = id
        self._gpr_cls = UNKNOWN
        self._fp = UNKNOWN
        self._sp = UNKNOWN
        self._gp = UNKNOWN
        self._tp = UNKNOWN
        self._ra = UNKNOWN
        self._convs: list = []

    @property
    def name(self) -> str:
        ''' Getter for name. '''
        return self._name

    @property
    def id(self) -> str:
        ''' Getter for id. '''
        return self._id

    @property
    def gpr_cls(self) -> int:
        ''' Getter for gpr_cls. '''
        return self._gpr_cls

    @property
    def fp(self) -> int:
        ''' Getter for fp. '''
        return self._fp

    @property
    def sp(self) -> int:
        ''' Getter for sp. '''
        return self._sp

    @property
    def gp(self) -> int:
        ''' Getter for gp. '''
        return self._gp

    @property
    def tp(self) -> int:
        ''' Getter for tp. '''
        return self._tp

    @property
    def ra(self) -> int:
        ''' Getter for ra. '''
        return self._ra

    @property
    def convs(self) -> list:
        ''' Getter for convs. '''
        return self._convs

    def set_gpr_cls(self, gpr_cls: int) -> 'ABIMeta':
        ''' Set gpr_cls. '''
        self._gpr_cls = gpr_cls
        return self

    def set_fp(self, fp: int) -> 'ABIMeta':
        ''' Set fp. '''
        self._fp = fp
        return self

    def set_sp(self, sp: int) -> 'ABIMeta':
        ''' Set sp. '''
        self._sp = sp
        return self

    def set_gp(self, gp: int) -> 'ABIMeta':
        ''' Set gp. '''
        self._gp = gp
        return self

    def set_tp(self, tp: int) -> 'ABIMeta':
        ''' Set tp. '''
        self._tp = tp
        return self

    def set_ra(self, ra: int) -> 'ABIMeta':
        ''' Set ra. '''
        self._ra = ra
        return self

    def add_conv(self, conv: dict) -> 'ABIMeta':
        ''' Add conv. '''
        self._convs.append(conv)
        return self

    def dump(self) -> str:
        ''' Dump the content of the object. '''
        s = f"RegConv(name={self.name}, id={self.id}, gpr_cls={self.gpr_cls}, " \
            f"fp={self.fp}, sp={self.sp}, gp={self.gp}, tp={self.tp}, ra={self.ra}, " \
            f"convs={self.convs})"
        return s

    def check_entries(self, entries: dict) -> bool:
        ''' Check if the entries are valid. '''
        fields = ["gpr_cls", "fpr_cls", "fp", "sp", "gp", "tp",
                  "ra", "int_zero", "param_retv"]
        for field in fields:
            if field not in entries:
                return False
        return True


class OPFlags:
    ''' Represents OPND_FLAG_META in air-infra. '''

    def __init__(self, name: str, id: str, desc: str) -> None:
        self._name = name
        self._id = id
        self._desc = desc

    @property
    def name(self) -> str:
        ''' Getter for name. '''
        return self._name

    @property
    def id(self) -> str:
        ''' Getter for id. '''
        return self._id

    @property
    def desc(self) -> str:
        ''' Getter for desc. '''
        return self._desc

    def __str__(self) -> str:
        return f"OPFlags(name={self.name}, id={self.id}, desc={self.desc})"
