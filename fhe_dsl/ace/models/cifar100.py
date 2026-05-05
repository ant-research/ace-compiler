# -*- coding: utf-8 -*-
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""CIFAR-100 dataset utilities for FHE model evaluation.

Provides image loading and normalization matching the C++ CIFAR_READER
used in the FHE runtime (ImageNet mean/std normalization).

Usage:
    from ace.models.cifar100 import load_cifar100_images, CIFAR100_CLASSES

    images, labels = load_cifar100_images(10)  # (10, 3, 32, 32) float32 tensor
"""

import pickle
from pathlib import Path
from typing import List, Tuple

import numpy as np
import torch


# CIFAR-100 test data search paths (in order of priority)
_CIFAR100_SEARCH_PATHS = [
    "/app/cifar/cifar-100-python/test",
]

# ImageNet normalization (same as C++ CIFAR_READER in resnet_cifar.main.inc)
IMAGENET_MEAN = np.array([0.485, 0.456, 0.406], dtype=np.float32).reshape(1, 3, 1, 1)
IMAGENET_STD = np.array([0.229, 0.224, 0.225], dtype=np.float32).reshape(1, 3, 1, 1)


def _find_test_file() -> str:
    """Find CIFAR-100 test file path."""
    for path in _CIFAR100_SEARCH_PATHS:
        if Path(path).exists():
            return path
    raise FileNotFoundError(
        f"CIFAR-100 test file not found. Searched: {_CIFAR100_SEARCH_PATHS}"
    )


def _load_class_names() -> List[str]:
    """Load CIFAR-100 fine label names from meta file."""
    search_paths = [
        str(Path(p).parent / "meta") for p in _CIFAR100_SEARCH_PATHS
    ]
    for path in search_paths:
        if Path(path).exists():
            with open(path, "rb") as f:
                meta = pickle.load(f, encoding="bytes")
            return [name.decode("utf-8") for name in meta[b"fine_label_names"]]
    raise FileNotFoundError(
        f"CIFAR-100 meta file not found. Searched: {search_paths}"
    )


# CIFAR-100 class names (100 fine-grained classes)
CIFAR100_CLASSES = _load_class_names()


def load_cifar100_images(n: int, offset: int = 0) -> Tuple[torch.Tensor, List[int]]:
    """Load and normalize CIFAR-100 test images.

    Args:
        n: Number of images to load.
        offset: Start index (default 0).

    Returns:
        Tuple of (images_tensor, labels) where images_tensor is
        (N, 3, 32, 32) float32 tensor with ImageNet normalization,
        and labels is a list of ints (fine labels).
    """
    test_file = _find_test_file()

    with open(test_file, "rb") as f:
        data = pickle.load(f, encoding="bytes")

    images = data[b"data"][offset:offset + n].reshape(-1, 3, 32, 32).astype(np.float32) / 255.0
    labels = list(data[b"fine_labels"][offset:offset + n])

    images = (images - IMAGENET_MEAN) / IMAGENET_STD

    return torch.from_numpy(images), labels