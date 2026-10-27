#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Frontend Module

Frontend strategies for FHE data representation.

Five conversion paths:
1. torch: PyTorch → FX Trace → TorchTracedModel → AIR
2. torch-via-onnx: PyTorch → ONNX → AIR
3. onnx: ONNX File → AIR
4. ast: Python AST → FHEProgram → AIR
5. ast-via-onnx: Python → ONNX → AIR
"""

from ..driver.registry import get_frontend, list_frontends, register_frontend


# Register built-in frontends
register_frontend("torch", "ace.fhe.frontend.torch.torch_frontend.Torch")
register_frontend("torch-via-onnx", "ace.fhe.frontend.torch.torch_via_onnx.TorchViaOnnx")
register_frontend("onnx", "ace.fhe.frontend.onnx.onnx_frontend.Onnx")
register_frontend("ast", "ace.fhe.frontend.ast.ast_frontend.AST")
register_frontend("ast-via-onnx", "ace.fhe.frontend.ast.ast_via_onnx.ASTViaOnnx")

# Public API - Frontend classes
try:
    from .torch.torch_frontend import Torch
except (ImportError, ModuleNotFoundError):
    Torch = None

try:
    from .onnx.onnx_frontend import Onnx
except (ImportError, ModuleNotFoundError):
    Onnx = None

try:
    from .ast.ast_frontend import AST
except (ImportError, ModuleNotFoundError):
    AST = None

try:
    from .torch.torch_via_onnx import TorchViaOnnx
except (ImportError, ModuleNotFoundError):
    TorchViaOnnx = None

try:
    from .ast.ast_via_onnx import ASTViaOnnx
except (ImportError, ModuleNotFoundError):
    ASTViaOnnx = None

# Utility functions for quick access (from ir.io.onnx_tools)
try:
    from ..ir.io.onnx_tools import (
        convert_onnx_to_air,
        export_function_to_onnx,
        export_model_to_onnx,
        inspect_onnx_model,
        validate_onnx_model,
    )
except (ImportError, ModuleNotFoundError):
    validate_onnx_model = None
    inspect_onnx_model = None
    export_model_to_onnx = None
    export_function_to_onnx = None
    convert_onnx_to_air = None

# Re-export at ace.fhe level for convenience
# This allows: from ace.fhe import frontend
# And: from ace import frontend

# Public API
__all__ = [
    "AST",
    "ASTViaOnnx",
    "Onnx",
    # Frontend classes
    "Torch",
    "TorchViaOnnx",
    "convert_onnx_to_air",
    "export_function_to_onnx",
    "export_model_to_onnx",
    "get_frontend",
    "inspect_onnx_model",
    "list_frontends",
    # Registry
    "register_frontend",
    # Utility functions
    "validate_onnx_model",
]
