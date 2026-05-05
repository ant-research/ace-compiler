#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Samples for ACE FHE.

This package provides sample operators and neural network modules that can be used as:
- Reference implementations for FHE compilation
- Building blocks for larger models
- Test cases for validation
- Starting points for custom models

Structure:
    - funcs/: Basic operations (arithmetic, activation, control flow)
        - arithmetic.py: add, sub, mul, div, relu, sigmoid, etc.
        - control_flow.py: loops, conditionals
    - ops/: Neural network modules
        - linear.py: Linear, MLP
        - conv.py: Conv2d variants
        - pool.py: Pooling operations
        - mlp.py: Multi-layer perceptrons
    - tensor_ops/: Custom C++ operator models (torch.ops.tensor.*)
        - binary.py: add, sub, mul, div, matmul, concat
        - unary.py: relu, softmax, pool, flatten, sqrt, silu
        - ternary.py: conv, gemm
        - composite.py: Composite models

Usage:
    # Import operator functions
    from ace.samples.funcs import add_func, relu_func

    # Import neural network modules
    from ace.samples.ops import LinearOp, Conv2dOp, AddOp, ReluOp, MLP

    # Import custom C++ operator models (for torch frontend testing)
    from ace.samples.tensor_ops import AddTensorOp, ReLUTensorOp, ConvTensorOp

    # Import utility functions
    from ace.samples import generate_inputs_by_mode, InputMode
"""

from ._utils import (
    InputMode,
    generate_inputs_by_mode,
    REGRESSION_MODES,
)

# Import tensor_ops package for re-export
from . import tensor_ops

__all__ = [
    # Utilities
    "InputMode",
    "generate_inputs_by_mode",
    "REGRESSION_MODES",
    # Packages
    "tensor_ops",
]