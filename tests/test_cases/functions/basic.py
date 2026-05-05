# test_cases/functions/basic.py
"""
Basic function test cases.

Note: Function definitions are imported from ace.samples.funcs.arithmetic
"""
import torch

from ..base import FunctionTestCase
from ace.samples.funcs import (
    add_func,
    sub_func,
    mul_func,
    div_func,
    relu_func,
    sigmoid_func,
    tanh_func,
    abs_func,
    neg_func,
    square_func,
    sqrt_func,
    clamp_func,
    softmax_func,
    log_func,
    exp_func,
)


# ============================================================================
# Test Cases
# ============================================================================

FUNCTION_BASIC_TEST_CASES = [
    FunctionTestCase("add", add_func, (torch.randn(1, 10), torch.randn(1, 10))),
    FunctionTestCase("sub", sub_func, (torch.randn(1, 10), torch.randn(1, 10))),
    FunctionTestCase("mul", mul_func, (torch.randn(1, 10), torch.randn(1, 10))),
    FunctionTestCase("div", div_func, (torch.ones(1, 10), torch.ones(1, 10) * 2)),
    FunctionTestCase("relu", relu_func, (torch.randn(1, 10),)),
    FunctionTestCase("sigmoid", sigmoid_func, (torch.randn(1, 10),)),
    FunctionTestCase("tanh", tanh_func, (torch.randn(1, 10),)),
    FunctionTestCase("abs", abs_func, (torch.randn(1, 10),)),
    FunctionTestCase("neg", neg_func, (torch.randn(1, 10),)),
    FunctionTestCase("square", square_func, (torch.randn(1, 10),)),
    FunctionTestCase("sqrt", sqrt_func, (torch.randn(1, 10),)),
    FunctionTestCase("clamp", clamp_func, (torch.randn(1, 10),)),
    FunctionTestCase("softmax", softmax_func, (torch.randn(1, 10),)),
    FunctionTestCase("log", log_func, (torch.randn(1, 10),)),
    FunctionTestCase("exp", exp_func, (torch.randn(1, 10),)),
]