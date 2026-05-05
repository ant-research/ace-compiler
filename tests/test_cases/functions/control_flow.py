# test_cases/functions/control_flow.py
"""
Function test cases with control flow.

Note: Function definitions are imported from ace.samples.funcs.control_flow
"""
import torch

from ..base import FunctionTestCase
from ace.samples.funcs import (
    conditional_add_func,
    conditional_relu_func,
    loop_multiply_func,
    loop_add_func,
    nested_loop_func,
    while_loop_func,
    conditional_chain_func,
    branch_execution_func,
)


# ============================================================================
# Test Cases
# ============================================================================

FUNCTION_CONTROL_FLOW_TEST_CASES = [
    FunctionTestCase("conditional_add", conditional_add_func, (torch.randn(1, 10), torch.randn(1, 10))),
    FunctionTestCase("conditional_relu", conditional_relu_func, (torch.randn(1, 10),)),
    FunctionTestCase("loop_multiply", loop_multiply_func, (torch.randn(1, 10),)),
    FunctionTestCase("loop_add", loop_add_func, (torch.randn(1, 10),)),
    FunctionTestCase("nested_loop", nested_loop_func, (torch.randn(1, 10),)),
    FunctionTestCase("while_loop", while_loop_func, (torch.randn(1, 10),)),
    FunctionTestCase("conditional_chain", conditional_chain_func, (torch.randn(1, 10), torch.randn(1, 10))),
    FunctionTestCase("branch_execution", branch_execution_func, (torch.randn(1, 10), torch.randn(1, 10))),
]