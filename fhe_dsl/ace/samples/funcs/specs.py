#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
CompileSpec instances for sample functions using three-layer architecture.

Layer 1: FuncEntity - describes the Python function
Layer 2: CompileConfig - describes how to compile
Layer 3: RuntimeConfig - describes how to run/validate
"""
import torch

from ace.fhe.spec import (
    CompileSpec, FuncEntity, CompileConfig, InputSpec
)
from ace.samples.funcs import (
    add_func, sub_func, mul_func, div_func,
    abs_func, neg_func, square_func, sqrt_func, clamp_func, log_func, exp_func,
    relu_func, sigmoid_func, tanh_func, softmax_func,
    conditional_add_func, conditional_relu_func,
    loop_multiply_func, loop_add_func, nested_loop_func,
    while_loop_func, conditional_chain_func, branch_execution_func,
)


def _make_func_spec(name, func, input_shapes, ir_ops=None):
    """Helper to build a function CompileSpec."""
    entity = FuncEntity(
        name=name,
        func=func,
        ir_ops=ir_ops,
    )

    input_spec = [InputSpec(shape=shape, dtype=torch.float32) for shape in input_shapes]

    compile_config = CompileConfig(
        input_spec=input_spec,
        encrypt_inputs=["x", "y"] if len(input_shapes) > 1 else ["x"],
    )

    spec = CompileSpec(entity=entity, compile=compile_config)
    spec.create = lambda: func
    return spec


# =============================================================================
# Arithmetic Functions
# =============================================================================

ADD_FUNC = _make_func_spec("add_func", add_func, [(1, 10), (1, 10)], ir_ops=["Add"])
SUB_FUNC = _make_func_spec("sub_func", sub_func, [(1, 10), (1, 10)], ir_ops=["Sub"])
MUL_FUNC = _make_func_spec("mul_func", mul_func, [(1, 10), (1, 10)], ir_ops=["Mul"])
DIV_FUNC = _make_func_spec("div_func", div_func, [(1, 10), (1, 10)], ir_ops=["Div"])
ABS_FUNC = _make_func_spec("abs_func", abs_func, [(1, 10)], ir_ops=["Abs"])
NEG_FUNC = _make_func_spec("neg_func", neg_func, [(1, 10)], ir_ops=["Neg"])
SQUARE_FUNC = _make_func_spec("square_func", square_func, [(1, 10)], ir_ops=["Sqrt"])  # square = x*x
SQRT_FUNC = _make_func_spec("sqrt_func", sqrt_func, [(1, 10)], ir_ops=["Sqrt"])
CLAMP_FUNC = _make_func_spec("clamp_func", clamp_func, [(1, 10)], ir_ops=["Clip"])
LOG_FUNC = _make_func_spec("log_func", log_func, [(1, 10)], ir_ops=["Log"])
EXP_FUNC = _make_func_spec("exp_func", exp_func, [(1, 10)], ir_ops=["Exp"])

# =============================================================================
# Activation Functions
# =============================================================================

RELU_FUNC = _make_func_spec("relu_func", relu_func, [(1, 10)], ir_ops=["Relu"])
SIGMOID_FUNC = _make_func_spec("sigmoid_func", sigmoid_func, [(1, 10)], ir_ops=["Sigmoid"])
TANH_FUNC = _make_func_spec("tanh_func", tanh_func, [(1, 10)], ir_ops=["Tanh"])
SOFTMAX_FUNC = _make_func_spec("softmax_func", softmax_func, [(1, 10)], ir_ops=["Softmax"])

# =============================================================================
# Control Flow Functions (simplified - just basic shapes)
# =============================================================================

CONDITIONAL_ADD_FUNC = _make_func_spec("conditional_add_func", conditional_add_func, [(1, 10), (1, 10)], ir_ops=["Add", "Greater", "Where"])
CONDITIONAL_RELU_FUNC = _make_func_spec("conditional_relu_func", conditional_relu_func, [(1, 10)], ir_ops=["Relu", "Greater"])
LOOP_MULTIPLY_FUNC = _make_func_spec("loop_multiply_func", loop_multiply_func, [(1, 10)], ir_ops=["Mul"])
LOOP_ADD_FUNC = _make_func_spec("loop_add_func", loop_add_func, [(1, 10)], ir_ops=["Add"])
NESTED_LOOP_FUNC = _make_func_spec("nested_loop_func", nested_loop_func, [(1, 10)], ir_ops=["Add"])
WHILE_LOOP_FUNC = _make_func_spec("while_loop_func", while_loop_func, [(1, 10)], ir_ops=["Add"])
CONDITIONAL_CHAIN_FUNC = _make_func_spec("conditional_chain_func", conditional_chain_func, [(1, 10), (1, 10)], ir_ops=["Add", "Greater", "Where"])
BRANCH_EXECUTION_FUNC = _make_func_spec("branch_execution_func", branch_execution_func, [(1, 10)], ir_ops=["Add"])

# =============================================================================
# All Specs
# =============================================================================

ALL_FUNCS_SPECS = [
    ADD_FUNC, SUB_FUNC, MUL_FUNC, DIV_FUNC, ABS_FUNC, NEG_FUNC,
    SQUARE_FUNC, SQRT_FUNC, CLAMP_FUNC, LOG_FUNC, EXP_FUNC,
    RELU_FUNC, SIGMOID_FUNC, TANH_FUNC, SOFTMAX_FUNC,
    CONDITIONAL_ADD_FUNC, CONDITIONAL_RELU_FUNC,
    LOOP_MULTIPLY_FUNC, LOOP_ADD_FUNC, NESTED_LOOP_FUNC,
    WHILE_LOOP_FUNC, CONDITIONAL_CHAIN_FUNC, BRANCH_EXECUTION_FUNC,
]

__all__ = [
    "ADD_FUNC", "SUB_FUNC", "MUL_FUNC", "DIV_FUNC", "ABS_FUNC", "NEG_FUNC",
    "SQUARE_FUNC", "SQRT_FUNC", "CLAMP_FUNC", "LOG_FUNC", "EXP_FUNC",
    "RELU_FUNC", "SIGMOID_FUNC", "TANH_FUNC", "SOFTMAX_FUNC",
    "CONDITIONAL_ADD_FUNC", "CONDITIONAL_RELU_FUNC",
    "LOOP_MULTIPLY_FUNC", "LOOP_ADD_FUNC", "NESTED_LOOP_FUNC",
    "WHILE_LOOP_FUNC", "CONDITIONAL_CHAIN_FUNC", "BRANCH_EXECUTION_FUNC",
    "ALL_FUNCS_SPECS",
]