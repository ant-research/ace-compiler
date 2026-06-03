#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Operator examples - basic operations and functions.
"""
from .arithmetic import (
    # Arithmetic functions
    add_func,
    sub_func,
    mul_func,
    div_func,
    abs_func,
    neg_func,
    square_func,
    sqrt_func,
    clamp_func,
    log_func,
    exp_func,
    # Activation functions
    relu_func,
    sigmoid_func,
    tanh_func,
    softmax_func,
)

from .control_flow import (
    # Control flow functions
    conditional_add_func,
    conditional_relu_func,
    loop_multiply_func,
    loop_add_func,
    nested_loop_func,
    while_loop_func,
    conditional_chain_func,
    branch_execution_func,
)

from . import specs

__all__ = [
    # Arithmetic functions
    "add_func",
    "sub_func",
    "mul_func",
    "div_func",
    "abs_func",
    "neg_func",
    "square_func",
    "sqrt_func",
    "clamp_func",
    "log_func",
    "exp_func",
    # Activation functions
    "relu_func",
    "sigmoid_func",
    "tanh_func",
    "softmax_func",
    # Control flow functions
    "conditional_add_func",
    "conditional_relu_func",
    "loop_multiply_func",
    "loop_add_func",
    "nested_loop_func",
    "while_loop_func",
    "conditional_chain_func",
    "branch_execution_func",
]