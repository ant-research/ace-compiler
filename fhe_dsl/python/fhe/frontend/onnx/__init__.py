#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ONNX Frontend Module

ONNX file path for AIR IR generation.

Pipeline:
1. prepare()  - Load ONNX file → ONNXFileIR
2. compile()  - ONNX → AIR IR (memory) - NOT IMPLEMENTED
3. export()   - Export ONNX file (copy) or .B file
"""

from .onnx_frontend import Onnx

__all__ = [
    "Onnx",
]