#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

from typing import Dict, List, Any, Optional
import logging
import torch

logger = logging.getLogger(__name__)

from .loader import ConfigLoader
from .executor import KernelExecutor
from .results import BatchResult, BatchTiming, DatasetResult
from ace import runtime as _C

# Map Python logging levels to C++ spdlog level strings
_PY_TO_SPDLOG_LEVEL = {
    logging.DEBUG: "DEBUG",
    logging.INFO: "INFO",
    logging.WARNING: "WARN",
    logging.ERROR: "ERROR",
    logging.CRITICAL: "CRITICAL",
}


def _sync_log_level():
    """Sync Python logger level to C++ spdlog."""
    rt_logger = logging.getLogger("ace.runtime")
    level = rt_logger.getEffectiveLevel()
    spdlog_level = _PY_TO_SPDLOG_LEVEL.get(level, "INFO")
    try:
        _C.set_log_level(spdlog_level)
    except Exception:
        pass


class FHERuntime:
    """FHE inference runtime.

    Manages configuration, kernel lifecycle, and provides inference APIs
    for single-image, batch, and dataset execution.

    Note: The C++ FHE runtime uses process-wide state. Only one FHERuntime
    instance should be active at a time. Creating a new FHERuntime will
    automatically finalize the previous one.
    """

    _active_instance = None

    def __init__(self, package):
        """
        Args:
            package: Model package — either a dict with keys:
                - model: Model name (used to derive config path)
                - kernel: Path to compiled FHE shared library
                - config_path: (optional) Explicit path to config file
              or a CompiledProgram instance (its .package dict is used).
        """
        # Finalize previous instance to avoid process-wide state conflicts
        self._cleanup_previous()

        # Accept CompiledProgram objects directly
        if hasattr(package, 'package') and isinstance(package.package, dict):
            package = package.package
        self.package = package

        # Load FHE configuration
        config_path = package.get('config_path', f"{package['model']}.conf")
        self.config_loader = ConfigLoader(config_path)

        # Register and hold the single kernel executor
        self._executor = KernelExecutor("kernel", package['kernel'])

        # Register as active instance
        FHERuntime._active_instance = self

        _sync_log_level()

    @classmethod
    def _cleanup_previous(cls):
        """Finalize and release the previous active runtime instance."""
        prev = cls._active_instance
        if prev is not None:
            try:
                prev._executor.finalize()
            except Exception:
                pass
            prev._executor = None
            cls._active_instance = None

    def init(self):
        """Initialize FHE context (key generation, etc.).

        Call once before multiple inference() calls to avoid per-inference
        context setup. inference() will auto-call this if not already done.
        """
        self._executor.init()

    def finalize(self):
        """Finalize FHE context and release resources.

        Call once after all inference calls are done.
        """
        self._executor.finalize()

    def inference(self, *input_tensors: torch.Tensor) -> torch.Tensor:
        """Run single FHE inference.

        Args:
            *input_tensors: Input tensors (auto-converted to 4D).

        Returns:
            Output tensor from FHE computation.
        """
        with torch.profiler.record_function("fhe::inference"):
            input_tensors_4d = [self._to_4d(t) for t in input_tensors]
            return self._executor.execute(*input_tensors_4d)

    def run_batch(
        self,
        batch_inputs: list,
        parallel: bool = False,
        num_threads: int = 0,
        verbose: bool = False,
    ) -> BatchResult:
        """Run FHE inference on a batch of inputs.

        Args:
            batch_inputs: List of input tensor lists, one per image.
                Each element can be a single tensor or a list/tuple of tensors.
            parallel: If True, use OpenMP parallel batch execution (CPU only).
            num_threads: Number of threads for parallel mode (0 = auto).
            verbose: If True, print per-image progress and results.

        Returns:
            BatchResult with outputs, timing, and success/failure counts.
        """
        with torch.profiler.record_function("fhe::run_batch"):
            # Ensure each item's tensors are 4D
            prepared = []
            for item in batch_inputs:
                if isinstance(item, torch.Tensor):
                    item = [self._to_4d(item)]
                else:
                    item = [self._to_4d(t) for t in item]
                prepared.append(item)

            return self._executor.execute_batch(
                prepared, parallel=parallel, num_threads=num_threads, verbose=verbose
            )

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
        """Run FHE inference on a dataset with accuracy metrics.

        Args:
            images: Tensor of shape (N, C, H, W) with all images.
            labels: Ground truth labels for each image.
            top_k: Number of top predictions for accuracy (1 for top-1 only,
                   5 for top-1 + top-5).
            parallel: Use parallel batch execution (CPU only, experimental).
            num_threads: Threads for parallel mode (0 = auto).
            verbose: If True, print per-image progress and results via C++ spdlog.
            plaintext_predictions: Optional list of plaintext (model) predictions.
                When provided, FHE match rate (FHE-vs-plaintext) is computed as
                the primary accuracy metric. When absent, top1_accuracy compares
                FHE predictions against ground truth labels.

        Returns:
            DatasetResult with accuracy metrics and timing.
        """
        with torch.profiler.record_function("fhe::inference"):
            self._executor.init()

            N = images.shape[0]

            # Build batch_inputs: each item is a single 4D tensor
            batch_inputs = []
            for i in range(N):
                img = self._to_4d(images[i:i+1])
                batch_inputs.append([img])

            try:
                batch_result = self._executor.execute_batch(
                    batch_inputs, parallel=parallel,
                    num_threads=num_threads, verbose=verbose
                )

                predictions = []
                for output in batch_result.outputs:
                    pred = output.flatten().argmax().item()
                    predictions.append(pred)

            finally:
                self._executor.finalize()

            # Compute FHE-vs-plaintext match rate
            fhe_match_count = 0
            if plaintext_predictions is not None:
                fhe_match_count = sum(
                    1 for f, p in zip(predictions, plaintext_predictions) if f == p
                )
            fhe_match_rate = fhe_match_count / N if N > 0 else 0.0

            # Compute FHE-vs-labels accuracy
            num_correct_top1 = sum(1 for p, l in zip(predictions, labels) if p == l)
            top1_accuracy = num_correct_top1 / N if N > 0 else 0.0

            top5_accuracy = None
            num_correct_top5 = num_correct_top1
            if top_k >= 5 and N > 0:
                num_correct_top5 = 0
                for i, output in enumerate(batch_result.outputs):
                    flat = output.flatten()
                    k = min(5, flat.numel())
                    _, top5_indices = flat.topk(k)
                    if labels[i] in top5_indices.tolist():
                        num_correct_top5 += 1
                top5_accuracy = num_correct_top5 / N

            return DatasetResult(
                predictions=predictions,
                labels=labels,
                top1_accuracy=top1_accuracy,
                top5_accuracy=top5_accuracy,
                timing=batch_result.timing,
                num_correct_top1=num_correct_top1,
                num_correct_top5=num_correct_top5,
                total=N,
                plaintext_predictions=plaintext_predictions,
                fhe_match_count=fhe_match_count,
                fhe_match_rate=fhe_match_rate,
            )

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
        return self._executor.validate(result, expected)

    @staticmethod
    def _to_4d(tensor: torch.Tensor) -> torch.Tensor:
        """Ensure tensor is 4D (N, C, H, W)."""
        return tensor if tensor.dim() == 4 else tensor.unsqueeze(1).unsqueeze(2)