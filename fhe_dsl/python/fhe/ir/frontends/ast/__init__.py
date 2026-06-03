#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AST Frontend Module

Python AST conversion to FHEProgram.

Usage:
    from ace.fhe.ir.frontends.ast import ASTToIRConverter

    converter = ASTToIRConverter()
    fhe_program = converter.convert_function(my_function)
    fhe_program.export_ir("output.B")
"""

from .ast_converter import ASTToIRConverter

__all__ = [
    "ASTToIRConverter",
]