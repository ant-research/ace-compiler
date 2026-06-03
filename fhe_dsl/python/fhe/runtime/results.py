# -*- coding: utf-8 -*-
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#"""Batch inference result types."""

from dataclasses import dataclass
from typing import List, Optional

import torch


@dataclass
class BatchTiming:
    """Timing statistics for a batch inference run."""

    total_ms: float
    avg_per_image_ms: float
    min_image_ms: float
    max_image_ms: float
    num_images: int

    @classmethod
    def from_cxx(cls, cxx_timing) -> "BatchTiming":
        return cls(
            total_ms=cxx_timing.total_ms,
            avg_per_image_ms=cxx_timing.avg_per_image_ms,
            min_image_ms=cxx_timing.min_image_ms,
            max_image_ms=cxx_timing.max_image_ms,
            num_images=cxx_timing.num_images,
        )

    def __str__(self) -> str:
        return (
            f"BatchTiming(total={self.total_ms:.1f}ms, "
            f"avg={self.avg_per_image_ms:.1f}ms/img, "
            f"min={self.min_image_ms:.1f}ms, max={self.max_image_ms:.1f}ms, "
            f"n={self.num_images})"
        )


@dataclass
class BatchResult:
    """Result of a batch FHE inference run."""

    outputs: List[torch.Tensor]
    timing: BatchTiming
    num_success: int
    num_failure: int

    @classmethod
    def from_cxx(cls, cxx_result) -> "BatchResult":
        return cls(
            outputs=list(cxx_result.outputs),
            timing=BatchTiming.from_cxx(cxx_result.timing),
            num_success=cxx_result.num_success,
            num_failure=cxx_result.num_failure,
        )

    def __str__(self) -> str:
        return (
            f"BatchResult(success={self.num_success}, failure={self.num_failure}, "
            f"{self.timing})"
        )


@dataclass
class DatasetResult:
    """Result of dataset inference with accuracy metrics.

    Accuracy is measured as FHE-vs-plaintext match rate: how many FHE
    predictions match the corresponding plaintext predictions. This is
    the standard metric for FHE benchmarking since FHE's job is to
    faithfully approximate plaintext computation.

    If plaintext_predictions is not provided, falls back to comparing
    FHE predictions against ground truth labels.
    """

    predictions: List[int]
    labels: List[int]
    top1_accuracy: float
    top5_accuracy: Optional[float]
    timing: BatchTiming
    num_correct_top1: int
    num_correct_top5: int
    total: int
    # FHE-vs-plaintext match metrics
    plaintext_predictions: Optional[List[int]] = None
    fhe_match_count: int = 0
    fhe_match_rate: float = 0.0

    def __str__(self) -> str:
        lines = [
            f"DatasetResult(total={self.total}, "
            f"top1={self.top1_accuracy:.1%} ({self.num_correct_top1}/{self.total}))",
        ]
        if self.top5_accuracy is not None:
            lines.append(
                f"  top5={self.top5_accuracy:.1%} ({self.num_correct_top5}/{self.total})"
            )
        if self.plaintext_predictions is not None:
            lines.append(
                f"  fhe_match={self.fhe_match_rate:.1%} ({self.fhe_match_count}/{self.total})"
            )
        lines.append(f"  {self.timing}")
        return "\n".join(lines)