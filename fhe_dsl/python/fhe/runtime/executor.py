#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import torch
import logging

from ace import runtime as _C
from .results import BatchResult

logger = logging.getLogger(__name__)


class KernelExecutor:
    """Manages the lifecycle of a single FHE kernel (.so).

    Wraps the C++ ProviderManager for kernel loading, context initialization,
    single-image and batch inference, and validation.
    """

    def __init__(self, name: str, lib_path: str, use_cuda_graph: bool = False):
        """
        Args:
            name: Identifier for this kernel (e.g. "kernel").
            lib_path: Path to the compiled FHE shared library.
            use_cuda_graph: If True, capture CUDA graph on first Execute()
                and replay on subsequent calls to reduce launch overhead.
                Only effective for GPU backends. Falls back to normal
                execution if graph capture fails.
        """
        self.name = name
        self.lib_path = lib_path

        self._manager = _C.ProviderManager()
        self._manager.register_kernel(name, lib_path, use_cuda_graph)

        self._initialized = False

    def init(self):
        """Initialize FHE context (key generation, etc.).

        Call once before multiple execute() calls to avoid per-inference context setup.
        execute() will auto-call this if not already done (backward compatible).
        """
        if not self._initialized:
            self._manager.init(self.name)
            self._initialized = True

    def finalize(self):
        """Finalize FHE context and release resources.

        Call once after all execute() calls are done.
        After this, init() must be called again before the next execute().
        """
        if self._initialized:
            self._manager.finalize(self.name)
            self._initialized = False

    def capture_graph(self) -> bool:
        """Attempt CUDA Graph capture for the Execute() phase.

        Must be called after init(). Runs Main_graph() once under stream
        capture. If successful, subsequent phase-split Execute() calls
        will replay the captured graph, reducing kernel launch overhead.

        WARNING: Only call this if the kernel's Main_graph() does NOT call
        cudaMalloc/cudaFree internally (illegal during stream capture).
        Phantom backend is NOT compatible; use only with ACE's own kernels.

        Returns:
            True if capture succeeded, False otherwise.
        """
        return self._manager.capture_graph(self.name)

    def execute(self, *input_tensors: torch.Tensor) -> torch.Tensor:
        """Run single FHE inference.

        Auto-initializes FHE context if not already done.

        Args:
            *input_tensors: Input tensors (will be converted to 4D if needed).

        Returns:
            Output tensor from FHE computation.
        """
        self.init()
        return self._manager.execute(self.name, list(input_tensors))

    def execute_batch(
        self,
        batch_inputs: list,
        parallel: bool = False,
        num_threads: int = 0,
        verbose: bool = False,
    ) -> BatchResult:
        """Run batch FHE inference on multiple inputs.

        Args:
            batch_inputs: List of input tensor lists, one per image.
                Each element is a list/tuple of input tensors for a single
                inference call, e.g. [(tensor1,), (tensor2,), ...].
            parallel: If True, use OpenMP parallel batch execution.
            num_threads: Number of threads for parallel mode (0 = auto).
            verbose: If True, print per-image progress and results.

        Returns:
            BatchResult with outputs, timing, and success/failure counts.
        """
        self.init()

        # Ensure each item is a list of tensors
        prepared = []
        for item in batch_inputs:
            if isinstance(item, torch.Tensor):
                item = [item]
            prepared.append(list(item))

        cxx_result = self._manager.execute_batch(
            self.name, prepared,
            parallel=parallel, num_threads=num_threads, verbose=verbose
        )

        result = BatchResult.from_cxx(cxx_result)
        return result

    def validate(self, result: torch.Tensor, expected: torch.Tensor) -> bool:
        """Validate FHE result against expected tensor.

        Compares the FHE inference result with the expected plaintext result
        using absolute and relative error thresholds.

        Args:
            result: FHE inference result tensor
            expected: Expected (plaintext) tensor

        Returns:
            True if validation passes (within error thresholds)
        """
        return _C.validate_result(result.to(torch.float64), expected.to(torch.float64))