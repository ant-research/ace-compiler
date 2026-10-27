#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IR Representations Module

Provides in-memory IR representations:
- FHEProgram: Top-level IR container
- FHEGraph: Computation graph with basic blocks
- IRNode, BasicBlock: Graph elements
"""

from .fhe_program import FHEProgram
from .graph import BasicBlock, FHEGraph, IRNode


__all__ = [
    "BasicBlock",
    "FHEGraph",
    "FHEProgram",
    "IRNode",
]
