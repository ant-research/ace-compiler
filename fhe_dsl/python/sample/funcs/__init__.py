#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Operator examples - basic operations and functions.
"""

from . import specs
from .arithmetic import (
    abs_func,
    # Arithmetic functions
    add_func,
    clamp_func,
    div_func,
    exp_func,
    log_func,
    mul_func,
    neg_func,
    # Activation functions
    relu_func,
    sigmoid_func,
    softmax_func,
    sqrt_func,
    square_func,
    sub_func,
    tanh_func,
)
from .control_flow import (
    branch_execution_func,
    # Control flow functions
    conditional_add_func,
    conditional_chain_func,
    conditional_relu_func,
    loop_add_func,
    loop_multiply_func,
    nested_loop_func,
    while_loop_func,
)


__all__ = [
    "abs_func",
    # Arithmetic functions
    "add_func",
    "branch_execution_func",
    "clamp_func",
    # Control flow functions
    "conditional_add_func",
    "conditional_chain_func",
    "conditional_relu_func",
    "div_func",
    "exp_func",
    "log_func",
    "loop_add_func",
    "loop_multiply_func",
    "mul_func",
    "neg_func",
    "nested_loop_func",
    # Activation functions
    "relu_func",
    "sigmoid_func",
    "softmax_func",
    "sqrt_func",
    "square_func",
    "sub_func",
    "tanh_func",
    "while_loop_func",
]
