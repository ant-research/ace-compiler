#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ONNX Frontend Module

ONNX model conversion to FHEProgram or AIR binary.

Usage:
    from ace.fhe.ir.frontends.onnx import convert_onnx_to_fhe_program

    fhe_program = convert_onnx_to_fhe_program("model.onnx")
    fhe_program.export_ir("output.B")
"""

from .onnx_converter import convert_onnx_to_fhe_program, convert_onnx_to_air_binary

__all__ = [
    "convert_onnx_to_fhe_program",
    "convert_onnx_to_air_binary",
]