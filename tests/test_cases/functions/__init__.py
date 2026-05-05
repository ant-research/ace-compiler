# test_cases/functions/__init__.py
"""
Function test cases organized by category.
"""
from .basic import FUNCTION_BASIC_TEST_CASES
from .control_flow import FUNCTION_CONTROL_FLOW_TEST_CASES

# All function test cases combined
FUNCTION_TEST_CASES = (
    FUNCTION_BASIC_TEST_CASES
    + FUNCTION_CONTROL_FLOW_TEST_CASES
)

__all__ = [
    "FUNCTION_TEST_CASES",
    "FUNCTION_BASIC_TEST_CASES",
    "FUNCTION_CONTROL_FLOW_TEST_CASES",
]