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
from .graph import IRNode, BasicBlock, FHEGraph

__all__ = [
    "FHEProgram",
    "FHEGraph",
    "BasicBlock",
    "IRNode",
]