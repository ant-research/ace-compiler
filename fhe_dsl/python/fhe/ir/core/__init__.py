#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Core IR Builder Module

Provides the Python IRBuilder wrapper for AIR IR generation.
"""

from .ir_builder import IRBuilder, TensorInfo

__all__ = [
    "IRBuilder",
    "TensorInfo",
]