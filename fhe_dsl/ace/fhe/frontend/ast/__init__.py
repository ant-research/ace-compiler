#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AST Frontend Module

Python AST path for AIR IR generation.

Pipeline:
1. prepare()  - Python → AST Analysis → FHEProgram
2. compile()  - Returns FHEProgram directly
3. export()   - Export .B file
"""

from .ast_frontend import AST

__all__ = [
    "AST",
]