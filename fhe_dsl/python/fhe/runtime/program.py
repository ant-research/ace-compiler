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

from typing import List, Optional
import torch

from .runtime import FHERuntime
from .results import DatasetResult
from ..profiler import FHEProfiler, ProfileResult


class CompiledProgram:
    """Compiled FHE program that can be called directly."""

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
        """Validate FHE inference by comparing with plaintext computation.

        Uses the example inputs from compile time to compute expected result
        and compares with FHE result using C++ Validate_result (absolute and
        relative error thresholds).

        Returns:
            bool: True if FHE result matches plaintext result

        Raises:
            ValueError: If no plaintext reference is available for comparison.
        """
        if self._func is None and self._model is None:
            raise ValueError(
                "No plaintext reference available for validation. "
                "Provide func or model to compile() to enable validation."
            )

        if self._example_inputs is None:
            raise ValueError("Example inputs not available for validation")

        # Compute expected result using plaintext computation
        expected = self._compute_plaintext()

        # Reuse existing runtime or create one
        if self._runtime is None:
            self._runtime = FHERuntime(self.package)
        fhe_result = self._runtime.inference(*self._example_inputs)

        # Use C++ Validate_result for error-threshold comparison
        return self._runtime.validate(fhe_result, expected)

    def _compute_plaintext(self):
        """Compute plaintext result using the original model/function."""
        if self._model is not None:
            return self._model(*self._example_inputs)
        elif isinstance(self._func, type) and issubclass(self._func, torch.nn.Module):
            model = self._func()
            return model(*self._example_inputs)
        else:
            return self._func(*self._example_inputs)

    def run_dataset(
        self,
        images: torch.Tensor,
        labels: List[int],
        top_k: int = 1,
        parallel: bool = False,
        num_threads: int = 0,
        verbose: bool = True,
        plaintext_predictions: Optional[List[int]] = None,
    ) -> DatasetResult:
        """Run dataset inference with accuracy metrics.

        Convenience method that delegates to FHERuntime.run_dataset().

        Args:
            images: Tensor of shape (N, C, H, W) with all images.
            labels: Ground truth labels for each image.
            top_k: Number of top predictions (1 for top-1, 5 for top-1+top-5).
            parallel: Use parallel batch execution (CPU only, experimental).
            num_threads: Threads for parallel mode (0 = auto).
            verbose: If True, print per-image progress and results via C++ spdlog.
            plaintext_predictions: Optional plaintext predictions for FHE match rate.

        Returns:
            DatasetResult with accuracy metrics and timing.
        """
        if self._runtime is None:
            self._runtime = FHERuntime(self.package)
        return self._runtime.run_dataset(
            images, labels, top_k=top_k, parallel=parallel,
            num_threads=num_threads, verbose=verbose,
            plaintext_predictions=plaintext_predictions,
        )

    def profile(
        self,
        images: torch.Tensor,
        labels: Optional[List[int]] = None,
        device: str = "cpu",
        trace_dir: Optional[str] = None,
        **kwargs,
    ) -> ProfileResult:
        """Profile FHE inference with torch.profiler.

        Convenience method that wraps inference in an FHEProfiler context
        and returns structured profiling results.

        Args:
            images: Input tensor(s).
            labels: Ground truth labels (uses run_dataset if provided).
            device: "cpu" or "cuda" for profiler activity selection.
            trace_dir: If set, auto-export Chrome Trace to this directory.
            **kwargs: Additional args passed to run_dataset or inference.

        Returns:
            ProfileResult with FHE events, memory snapshots, and dataset result.
        """
        with FHEProfiler(device=device, trace_dir=trace_dir) as prof:
            if labels is not None:
                dataset_result = self.run_dataset(images, labels, **kwargs)
            else:
                dataset_result = self(*images) if isinstance(images, (list, tuple)) else self(images)
        prof._result.dataset_result = dataset_result
        return prof._result