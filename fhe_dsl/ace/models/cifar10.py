# -*- coding: utf-8 -*-
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""CIFAR-10 dataset utilities for FHE model evaluation.

Provides image loading and normalization matching the C++ CIFAR_READER
used in the FHE runtime (ImageNet mean/std normalization).

Usage:
    from ace.models.cifar10 import load_cifar10_images, CIFAR10_CLASSES

    images, labels = load_cifar10_images(10)  # (10, 3, 32, 32) float32 tensor
"""

import pickle
from pathlib import Path
from typing import List, Tuple

import numpy as np
import torch


# CIFAR-10 test data search paths (in order of priority)
_CIFAR10_SEARCH_PATHS = [
    "/app/cifar/cifar-10-batches-py/test_batch",
    str(Path(__file__).parent.parent.parent.parent / "benchmark" / "data" / "cifar10" / "cifar-10-batches-py" / "test_batch"),
]

# ImageNet normalization (same as C++ CIFAR_READER in resnet_cifar.main.inc)
IMAGENET_MEAN = np.array([0.485, 0.456, 0.406], dtype=np.float32).reshape(1, 3, 1, 1)
IMAGENET_STD = np.array([0.229, 0.224, 0.225], dtype=np.float32).reshape(1, 3, 1, 1)

# CIFAR-10 class names
CIFAR10_CLASSES = [
    "airplane", "automobile", "bird", "cat", "deer",
    "dog", "frog", "horse", "ship", "truck",
]


def _find_test_batch() -> str:
    """Find CIFAR-10 test_batch file path."""
    for path in _CIFAR10_SEARCH_PATHS:
        if Path(path).exists():
            return path
    raise FileNotFoundError(
        f"CIFAR-10 test_batch not found. Searched: {_CIFAR10_SEARCH_PATHS}"
    )


def load_cifar10_images(n: int, offset: int = 0) -> Tuple[torch.Tensor, List[int]]:
    """Load and normalize CIFAR-10 test images.

    Args:
        n: Number of images to load.
        offset: Start index (default 0).

    Returns:
        Tuple of (images_tensor, labels) where images_tensor is
        (N, 3, 32, 32) float32 tensor with ImageNet normalization,
        and labels is a list of ints.
    """
    test_file = _find_test_batch()

    with open(test_file, "rb") as f:
        data = pickle.load(f, encoding="bytes")

    images = data[b"data"][offset:offset + n].reshape(-1, 3, 32, 32).astype(np.float32) / 255.0
    labels = list(data[b"labels"][offset:offset + n])

    images = (images - IMAGENET_MEAN) / IMAGENET_STD

    return torch.from_numpy(images), labels