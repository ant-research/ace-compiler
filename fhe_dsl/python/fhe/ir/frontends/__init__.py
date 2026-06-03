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

from .torch.torch_trace import TorchTracedModel, FXTracedModel
from .onnx.onnx_converter import convert_onnx_to_fhe_program, convert_onnx_to_air_binary
from .ast.ast_converter import ASTToIRConverter

__all__ = [
    # Torch path
    "TorchTracedModel",
    "FXTracedModel",
    # ONNX path
    "convert_onnx_to_fhe_program",
    "convert_onnx_to_air_binary",
    # AST path
    "ASTToIRConverter",
]