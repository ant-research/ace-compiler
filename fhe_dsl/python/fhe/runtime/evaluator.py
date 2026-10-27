#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""Dataset-level FHE evaluation.

`DatasetEvaluator` orchestrates running an FHE program over a whole dataset
and computing accuracy / FHE-vs-plaintext match metrics. It is an evaluation
*orchestrator* layered on top of `FHERuntime`'s inference primitives — it
deliberately does **not** manage the FHE context lifecycle. The caller is
responsible for keeping the runtime alive (and finalizing it), typically via
``with FHERuntime(pkg) as rt: DatasetEvaluator(rt).evaluate(...)``.
"""

import torch

from .results import DatasetResult


class DatasetEvaluator:
    """Run a dataset through an FHE runtime and compute accuracy metrics.

    This extracts the evaluation concern out of `FHERuntime`: the runtime only
    owns inference primitives (`run` / `run_batch`) and lifecycle; metrics
    (argmax, top-k, FHE-vs-plaintext match rate) live here.

    The evaluator does NOT call ``init()`` / ``finalize()`` on the runtime —
    context lifecycle is the caller's responsibility. ``run_batch`` (which the
    evaluator routes through) auto-initializes lazily, so an explicit ``init()``
    is not required, but the caller must release the context with
    ``finalize()`` or a ``with`` block.
    """

    def __init__(self, runtime):
        """
        Args:
            runtime: A live ``FHERuntime`` (or anything exposing a compatible
                ``run_batch``). It must outlive the `evaluate` call.
        """
        self._runtime = runtime

    def evaluate(
        self,
        images: torch.Tensor,
        labels: list[int],
        *,
        top_k: int = 1,
        parallel: bool = False,
        num_threads: int = 0,
        verbose: bool = True,
        plaintext_predictions: list[int] | None = None,
    ) -> DatasetResult:
        """Run FHE inference over a dataset and compute accuracy metrics.

        Args:
            images: Tensor of shape (N, C, H, W) with all images. Each
                ``images[i:i+1]`` becomes one inference input; ``run_batch``
                performs the 4D cast.
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
        rt = self._runtime
        with torch.profiler.record_function("fhe::run_dataset"):
            N = images.shape[0]

            # Build batch_inputs: one raw 4D-castable slice per image.
            # run_batch handles the _to_4d conversion (accepts a bare Tensor).
            batch_inputs = [images[i : i + 1] for i in range(N)]

            batch_result = rt.run_batch(batch_inputs, parallel=parallel, num_threads=num_threads, verbose=verbose)

            predictions = []
            for output in batch_result.outputs:
                pred = output.flatten().argmax().item()
                predictions.append(pred)

            # Compute FHE-vs-plaintext match rate
            fhe_match_count = 0
            if plaintext_predictions is not None:
                fhe_match_count = sum(1 for f, p in zip(predictions, plaintext_predictions) if f == p)
            fhe_match_rate = fhe_match_count / N if N > 0 else 0.0

            # Compute FHE-vs-labels accuracy
            num_correct_top1 = sum(1 for p, lbl in zip(predictions, labels) if p == lbl)
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
