#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Frontends Module

Provides converters from various input formats to IR:
- Torch: PyTorch FX tracing path
- ONNX: ONNX model conversion
- AST: Python AST conversion
"""

from .ast.ast_converter import ASTToIRConverter
from .onnx.onnx_converter import convert_onnx_to_air_binary, convert_onnx_to_fhe_program
from .torch.torch_trace import FXTracedModel, TorchTracedModel


__all__ = [
    # AST path
    "ASTToIRConverter",
    "FXTracedModel",
    # Torch path
    "TorchTracedModel",
    "convert_onnx_to_air_binary",
    # ONNX path
    "convert_onnx_to_fhe_program",
]
