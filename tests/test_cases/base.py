# test_cases/base.py
"""
Base test case classes for ANT-ACE testing framework.
"""
from typing import Callable, Optional, List, Any, Dict
import torch
import torch.nn as nn


class TestCase:
    """Base class for all test cases."""

    def __init__(self, name: str):
        self.name = name

    def __str__(self):
        return f"{self.__class__.__name__}({self.name})"

    def __repr__(self):
        return self.__str__()


class ModelTestCase(TestCase):
    """
    Test case for PyTorch models.

    Args:
        name: Test case name
        model_class: PyTorch model class
        example_inputs: Tuple of example input tensors
        encrypt_inputs: List of input names to encrypt
        model_init_args: Positional args for model constructor
        model_init_kwargs: Keyword args for model constructor
        expected_ops: List of expected operator types in IR
        model_post_init: Optional callback for custom initialization
        compile_options: Optional dict of compiler options (vec, ckks, sihe, p2c)
    """

    def __init__(
        self,
        name: str,
        model_class: type,
        example_inputs: tuple,
        encrypt_inputs: List[str],
        model_init_args: tuple = (),
        model_init_kwargs: dict = None,
        expected_ops: Optional[List[str]] = None,
        model_post_init: Optional[Callable[[nn.Module], None]] = None,
        compile_options: Optional[Dict[str, Any]] = None
    ):
        super().__init__(name)
        self.model_class = model_class
        self.example_inputs = example_inputs
        self.encrypt_inputs = encrypt_inputs
        self.expected_ops = expected_ops or []
        self.model_init_args = model_init_args
        self.model_init_kwargs = model_init_kwargs or {}
        self.model_post_init = model_post_init
        self.compile_options = compile_options or {}

    def create_model(self) -> nn.Module:
        """Create and return a model instance."""
        model = self.model_class(*self.model_init_args, **self.model_init_kwargs)
        if self.model_post_init is not None:
            self.model_post_init(model)
        return model


class FunctionTestCase(TestCase):
    """
    Test case for Python functions.

    Args:
        name: Test case name
        func: Python function to test
        example_inputs: Tuple of example input tensors
        compile_options: Optional dict of compiler options (vec, ckks, sihe, p2c)
    """

    def __init__(
        self,
        name: str,
        func: Callable,
        example_inputs: tuple,
        compile_options: Optional[Dict[str, Any]] = None
    ):
        super().__init__(name)
        self.func = func
        self.example_inputs = example_inputs
        self.compile_options = compile_options or {}

    def run(self, *inputs):
        """Run the function with given inputs."""
        return self.func(*inputs)