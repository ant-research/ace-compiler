#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Custom Operators Module

This module contains code related to PyTorch custom operators (torch.ops.tensor.xxx)
that generate AIR IR instead of computing results.

NOTE: This is a SEPARATE path from TensorLevelHandler. The two paths are:

Path 1 (Custom Ops - this module):
  Python: torch.ops.tensor.relu(x)
      ↓
  C++: tensor_relu_impl() in torch_ops.cxx
      ↓
  C++: AIR_GEN::GetGlobalInstance()->AddOperation()
      ↓
  AIR IR generation

Path 2 (TensorLevelHandler - current main path):
  Python: IRBuilder.add_op('relu', ...)
      ↓
  C++: IR_BUILDER::AddOperation()
      ↓
  C++: TensorLevelHandler::ProcessOp()
      ↓
  AIR IR generation

Currently, Path 2 is the main path used by torch_trace.py.
Path 1 is available but not actively used in the default workflow.
"""

from .registry import (
    CUSTOM_OPERATORS,
    HARDWARE_MAP,
    OP_TO_AIR_GEN,
    STANDARD_OP_MAPPING,
    get_cpp_function_name,
)
from .tracer import CustomTracer, trace_with_metadata


__all__ = [
    "CUSTOM_OPERATORS",
    "HARDWARE_MAP",
    "OP_TO_AIR_GEN",
    "STANDARD_OP_MAPPING",
    "CustomTracer",
    "get_cpp_function_name",
    "trace_with_metadata",
]
