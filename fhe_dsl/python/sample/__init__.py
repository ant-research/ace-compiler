#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Sample operators and functions for ACE FHE.

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
    from ace.sample.funcs import add_func, relu_func

    # Import neural network modules
    from ace.sample.ops import LinearOp, Conv2dOp, AddOp, ReluOp, MLP

    # Import custom C++ operator models (for torch frontend testing)
    from ace.sample.tensor_ops import AddTensorOp, ReLUTensorOp, ConvTensorOp
"""

# Import subpackages for re-export
from . import tensor_ops

__all__ = [
    "tensor_ops",
]