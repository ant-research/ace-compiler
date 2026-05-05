#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Tensor-level PyTorch op to custom op mapping.

Custom ops registered in C++ extension via TORCH_LIBRARY(tensor, m).
Available: add, sub, mul, div, matmul, concat, relu, softmax, max_pool,
           average_pool, global_average_pool, flatten, sqrt, silu, conv, gemm
"""

import operator
import torch
import torch.nn as nn
import torch.nn.functional as F
from typing import Any, Dict, Optional, Callable


# ============================================================================
# Thread-local context for op name (for ONNX-style comment generation)
# ============================================================================

import threading

# Thread-local storage for current op name (hardcode sync with ONNX)
_tls = threading.local()

def set_current_op_name(name: str):
    """Set current op name for comment generation (hardcode sync with ONNX)."""
    _tls.current_op_name = name

def get_current_op_name() -> Optional[str]:
    """Get current op name for comment generation."""
    return getattr(_tls, 'current_op_name', None)

# ============================================================================
# Tensor-level Op Mapping
# ============================================================================

TORCH_OP_TO_CUSTOM_OP: Dict[Any, str] = {
    # Binary operators
    operator.add: "add",
    operator.sub: "sub",
    operator.mul: "mul",
    operator.truediv: "div",
    torch.add: "add",
    torch.sub: "sub",
    torch.mul: "mul",
    torch.div: "div",
    torch.matmul: "matmul",
    torch.cat: "concat",

    # Unary operators
    torch.relu: "relu",
    torch.sqrt: "sqrt",
    torch.flatten: "flatten",
    torch.reshape: "reshape",
    F.relu: "relu",
    F.softmax: "softmax",
    F.max_pool2d: "max_pool",
    F.avg_pool2d: "average_pool",
    F.adaptive_avg_pool2d: "global_average_pool",
    F.silu: "silu",

    # Ternary operators
    F.conv2d: "conv",
    F.linear: "gemm",
}

# nn.Module class to custom op mapping
NN_MODULE_TO_CUSTOM_OP: Dict[Any, str] = {
    nn.ReLU: "relu",
    nn.MaxPool2d: "max_pool",
    nn.AvgPool2d: "average_pool",
    nn.AdaptiveAvgPool2d: "global_average_pool",
    nn.Flatten: "flatten",
    nn.Conv2d: "conv",
    nn.Linear: "gemm",
}


def get_custom_op(torch_op: Any) -> Optional[Callable]:
    """
    Get custom op callable: torch.ops.tensor.<op_name>

    Args:
        torch_op: Original torch operator

    Returns:
        Custom op callable or None if not found
    """
    op_name = TORCH_OP_TO_CUSTOM_OP.get(torch_op)
    if op_name is None:
        return None
    try:
        return getattr(torch.ops.tensor, op_name)
    except AttributeError:
        return None


def get_op_name(torch_op: Any) -> Optional[str]:
    """
    Get custom op name for a torch operator.

    Args:
        torch_op: Original torch operator

    Returns:
        Custom op name or None if not found
    """
    return TORCH_OP_TO_CUSTOM_OP.get(torch_op)


def get_custom_op_for_module(module: nn.Module) -> Optional[Callable]:
    """
    Get custom op callable for an nn.Module instance.

    Args:
        module: PyTorch nn.Module instance

    Returns:
        Custom op callable or None if not found
    """
    module_type = type(module)
    op_name = NN_MODULE_TO_CUSTOM_OP.get(module_type)
    if op_name is None:
        return None
    try:
        return getattr(torch.ops.tensor, op_name)
    except AttributeError:
        return None


def get_module_op_name(module: nn.Module) -> Optional[str]:
    """
    Get custom op name for an nn.Module instance.

    Args:
        module: PyTorch nn.Module instance

    Returns:
        Custom op name or None if not found
    """
    module_type = type(module)
    return NN_MODULE_TO_CUSTOM_OP.get(module_type)