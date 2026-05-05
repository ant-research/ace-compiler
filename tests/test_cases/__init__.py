# test_cases/__init__.py
"""
Test cases for ANT-ACE testing framework.

This package contains test cases for both models and functions,
organized by category for easy access and maintenance.

Note: Model and function definitions are now in:
    - ace.samples.funcs: Basic operations (add, mul, relu, etc.)
    - ace.samples.ops: Neural network modules (Linear, Conv, Pool, etc.)
    - ace.models: Complete models (ResNet, etc.)

Structure:
    - models/: PyTorch model test cases
    - functions/: Python function test cases
    - base.py: Base classes (TestCase, ModelTestCase, FunctionTestCase)

Usage:
    # Import test cases for parametrized tests
    from test_cases import MODEL_TEST_CASES, FUNCTION_TEST_CASES

    # Import model classes for direct use (from new locations)
    from ace.samples.ops import LinearOp, Conv2dOp
    from ace.samples.funcs import relu_func, add_func
    from ace.models.resnet import create_pretrained_resnet

    # Use in tests
    @pytest.mark.parametrize("test_case", MODEL_TEST_CASES, ids=lambda tc: tc.name)
    def test_model(test_case):
        model = test_case.create_model()
        ...
"""
from .base import TestCase, ModelTestCase, FunctionTestCase
from .models import MODEL_TEST_CASES
from .functions import FUNCTION_TEST_CASES
from .default_options import get_compile_options, set_env_options, clear_env_options

__all__ = [
    # Base classes
    "TestCase",
    "ModelTestCase",
    "FunctionTestCase",
    # Test cases
    "MODEL_TEST_CASES",
    "FUNCTION_TEST_CASES",
    # Compile options
    "get_compile_options",
    "set_env_options",
    "clear_env_options",
]