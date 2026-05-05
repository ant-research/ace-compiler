#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Compiled FHE program that can be called directly.

Usage:
    program = add.compile([x, y])
    result = program(x, y)  # Run inference
    is_valid = program.validate()  # Validate using compile-time inputs
"""

from typing import Any, Optional
import torch

from .fhe_runtime import FHERuntime


class CompiledProgram:
    """
    Compiled FHE program that can be called directly.
    """

    def __init__(self, package: dict, func=None, example_inputs=None, model=None):
        self.package = package
        self._runtime = None
        self._func = func  # Original function for plaintext computation
        self._example_inputs = example_inputs  # Saved compile-time inputs
        self._model = model  # Pre-created model instance for validation

    def __call__(self, *args, **kwargs):
        """Run FHE inference with the given inputs."""
        if self._runtime is None:
            self._runtime = FHERuntime(self.package)
        return self._runtime.inference(*args, **kwargs)

    def runtime(self):
        """Get the FHERuntime instance."""
        if self._runtime is None:
            self._runtime = FHERuntime(self.package)
        return self._runtime

    def validate(self) -> bool:
        """
        Validate FHE inference by comparing with plaintext computation.

        Uses the example inputs from compile time to compute expected result
        and compares with FHE result.

        Returns:
            bool: True if FHE result matches plaintext result
        """
        if self._func is None:
            raise ValueError("Original function not available for validation")

        if self._example_inputs is None:
            raise ValueError("Example inputs not available for validation")

        # Compute expected result using plaintext computation
        # Use pre-created model instance if available, otherwise create new one
        if self._model is not None:
            # Use pre-created model instance (has the same weights as at compile time)
            expected = self._model(*self._example_inputs)
        elif isinstance(self._func, type) and issubclass(self._func, torch.nn.Module):
            # It's a nn.Module class, create instance
            model = self._func()
            expected = model(*self._example_inputs)
        else:
            # It's a function
            expected = self._func(*self._example_inputs)

        # Create a fresh runtime for validation
        runtime = FHERuntime(self.package)
        fhe_result = runtime.inference(*self._example_inputs)

        # Compare results - handle shape mismatch between FHE (4D) and plaintext
        fhe_result = fhe_result.float()
        expected = expected.float()

        # Try multiple strategies to handle shape differences
        # Strategy 1: squeeze both
        try:
            if torch.allclose(fhe_result.squeeze(), expected.squeeze(), atol=1e-3):
                return True
        except RuntimeError:
            pass

        # Strategy 2: flatten both
        try:
            if torch.allclose(fhe_result.flatten(), expected.flatten(), atol=1e-3):
                return True
        except RuntimeError:
            pass

        # Strategy 3: squeeze FHE result only
        try:
            if torch.allclose(fhe_result.squeeze(), expected, atol=1e-3):
                return True
        except RuntimeError:
            pass

        # Strategy 4: direct comparison
        try:
            if torch.allclose(fhe_result, expected, atol=1e-3):
                return True
        except RuntimeError:
            pass

        return False