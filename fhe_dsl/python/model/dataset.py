# -*- coding: utf-8 -*-
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""Dataset utilities for FHE model evaluation.

Provides image loading and normalization matching training configs.

Usage:
    from ace.model.dataset import load_cifar10_images, CIFAR10_CLASSES
    from ace.model.dataset import load_cifar100_images, CIFAR100_CLASSES

    images, labels = load_cifar10_images(10)   # (10, 3, 32, 32) float32
    images, labels = load_cifar100_images(10)  # (10, 3, 32, 32) float32

Environment:
    ACE_DATA_DIR: Override dataset root directory (default: auto-detect)

Dataset Layout:
    <data_root>/
        cifar10/cifar-10-batches-py/test_batch
        cifar100/cifar-100-python/test
        cifar100/cifar-100-python/meta
"""
import os
import pickle
import subprocess
import tarfile
from pathlib import Path
from typing import List, Tuple

import numpy as np
import torch


# =============================================================================
# Dataset root detection
# =============================================================================

# Project root: ace/models/datasets.py -> 4 levels up
_PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent.parent

# Default system-wide dataset location (container image)
_SYSTEM_DATA_DIR = Path("/opt/dataset")

# CIFAR download URLs
_CIFAR10_URL = "https://www.cs.toronto.edu/~kriz/cifar-10-python.tar.gz"
_CIFAR100_URL = "https://www.cs.toronto.edu/~kriz/cifar-100-python.tar.gz"


def _get_data_root() -> Path:
    """Get dataset root directory.

    Priority:
        1. ACE_DATA_DIR environment variable
        2. /opt/dataset (system-wide, for container images)
        3. <project_root>/data (development, auto-download)
    """
    env = os.environ.get("ACE_DATA_DIR")
    if env:
        return Path(env)
    if _SYSTEM_DATA_DIR.exists():
        return _SYSTEM_DATA_DIR
    return _PROJECT_ROOT / "data"


def _ensure_cifar10_downloaded(data_root: Path) -> Path:
    """Download CIFAR-10 if not present."""
    cifar10_dir = data_root / "cifar10" / "cifar-10-batches-py"
    if cifar10_dir.exists():
        return cifar10_dir

    # Download to data_root
    data_root.mkdir(parents=True, exist_ok=True)
    tar_path = data_root / "cifar-10-python.tar.gz"

    print(f"CIFAR-10 not found. Downloading to {data_root}...")
    try:
        subprocess.run(
            ["wget", "-q", "-O", str(tar_path), _CIFAR10_URL],
            check=True,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        # Fallback to curl if wget not available
        subprocess.run(
            ["curl", "-sL", "-o", str(tar_path), _CIFAR10_URL],
            check=True,
        )

    print(f"Extracting {tar_path}...")
    with tarfile.open(tar_path, "r:gz") as tar:
        tar.extractall(data_root)

    # Move cifar-10-batches-py to cifar10/cifar-10-batches-py
    extracted_dir = data_root / "cifar-10-batches-py"
    if extracted_dir.exists():
        cifar10_parent = data_root / "cifar10"
        cifar10_parent.mkdir(exist_ok=True)
        extracted_dir.rename(cifar10_dir)

    tar_path.unlink(missing_ok=True)
    print(f"CIFAR-10 ready at {cifar10_dir}")
    return cifar10_dir


def _ensure_cifar100_downloaded(data_root: Path) -> Path:
    """Download CIFAR-100 if not present."""
    cifar100_dir = data_root / "cifar100" / "cifar-100-python"
    if cifar100_dir.exists():
        return cifar100_dir

    # Download to data_root
    data_root.mkdir(parents=True, exist_ok=True)
    tar_path = data_root / "cifar-100-python.tar.gz"

    print(f"CIFAR-100 not found. Downloading to {data_root}...")
    try:
        subprocess.run(
            ["wget", "-q", "-O", str(tar_path), _CIFAR100_URL],
            check=True,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        # Fallback to curl if wget not available
        subprocess.run(
            ["curl", "-sL", "-o", str(tar_path), _CIFAR100_URL],
            check=True,
        )

    print(f"Extracting {tar_path}...")
    with tarfile.open(tar_path, "r:gz") as tar:
        tar.extractall(data_root)

    # Move cifar-100-python to cifar100/cifar-100-python
    extracted_dir = data_root / "cifar-100-python"
    if extracted_dir.exists():
        cifar100_parent = data_root / "cifar100"
        cifar100_parent.mkdir(exist_ok=True)
        extracted_dir.rename(cifar100_dir)

    tar_path.unlink(missing_ok=True)
    print(f"CIFAR-100 ready at {cifar100_dir}")
    return cifar100_dir


# =============================================================================
# CIFAR-10
# =============================================================================

# CIFAR-10 normalization (mean from training config, std tuned for FHE)
# Training std: [0.2023, 0.1994, 0.2010], but larger std compresses activation
# ranges and improves FHE polynomial approximation accuracy.
CIFAR10_MEAN = np.array([0.4914, 0.4822, 0.4465], dtype=np.float32).reshape(1, 3, 1, 1)
CIFAR10_STD = np.array([0.229, 0.224, 0.225], dtype=np.float32).reshape(1, 3, 1, 1)

# CIFAR-10 class names
CIFAR10_CLASSES = [
    "airplane", "automobile", "bird", "cat", "deer",
    "dog", "frog", "horse", "ship", "truck",
]


def load_cifar10_images(n: int, offset: int = 0) -> Tuple[torch.Tensor, List[int]]:
    """Load and normalize CIFAR-10 test images.

    Args:
        n: Number of images to load.
        offset: Start index (default 0).

    Returns:
        Tuple of (images_tensor, labels) where images_tensor is
        (N, 3, 32, 32) float32 tensor with CIFAR-10 normalization,
        and labels is a list of ints.
    """
    test_file = _find_cifar10_test_batch()

    with open(test_file, "rb") as f:
        data = pickle.load(f, encoding="bytes")

    images = data[b"data"][offset:offset + n].reshape(-1, 3, 32, 32).astype(np.float32) / 255.0
    labels = list(data[b"labels"][offset:offset + n])

    images = (images - CIFAR10_MEAN) / CIFAR10_STD

    return torch.from_numpy(images), labels


def _find_cifar10_test_batch() -> str:
    """Find CIFAR-10 test_batch file path."""
    data_root = _get_data_root()
    test_file = data_root / "cifar10" / "cifar-10-batches-py" / "test_batch"

    if test_file.exists():
        return str(test_file)

    # Auto-download if not found
    cifar10_dir = _ensure_cifar10_downloaded(data_root)
    test_file = cifar10_dir / "test_batch"
    return str(test_file)


# =============================================================================
# CIFAR-100
# =============================================================================

# CIFAR-100 normalization (matching training config from chenyaofo/pytorch-cifar-models)
CIFAR100_MEAN = np.array([0.5070, 0.4865, 0.4409], dtype=np.float32).reshape(1, 3, 1, 1)
CIFAR100_STD = np.array([0.2673, 0.2564, 0.2761], dtype=np.float32).reshape(1, 3, 1, 1)


_CIFAR100_CLASSES: List[str] = []


def _load_cifar100_class_names() -> List[str]:
    """Load CIFAR-100 fine label names from meta file."""
    global _CIFAR100_CLASSES
    if _CIFAR100_CLASSES:
        return _CIFAR100_CLASSES

    data_root = _get_data_root()
    meta_file = data_root / "cifar100" / "cifar-100-python" / "meta"

    if not meta_file.exists():
        # Auto-download if not found
        _ensure_cifar100_downloaded(data_root)

    if meta_file.exists():
        with open(meta_file, "rb") as f:
            meta = pickle.load(f, encoding="bytes")
        _CIFAR100_CLASSES = [name.decode("utf-8") for name in meta[b"fine_label_names"]]
        return _CIFAR100_CLASSES

    raise FileNotFoundError(
        f"CIFAR-100 meta file not found at {meta_file}\n"
        "Set ACE_DATA_DIR to your dataset root directory."
    )


# CIFAR-100 class names (100 fine-grained classes) - lazy loaded on first access
def __getattr__(name: str):
    if name == "CIFAR100_CLASSES":
        return _load_cifar100_class_names()
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")


def load_cifar100_images(n: int, offset: int = 0) -> Tuple[torch.Tensor, List[int]]:
    """Load and normalize CIFAR-100 test images.

    Args:
        n: Number of images to load.
        offset: Start index (default 0).

    Returns:
        Tuple of (images_tensor, labels) where images_tensor is
        (N, 3, 32, 32) float32 tensor with CIFAR-100 normalization,
        and labels is a list of ints (fine labels).
    """
    test_file = _find_cifar100_test_file()

    with open(test_file, "rb") as f:
        data = pickle.load(f, encoding="bytes")

    images = data[b"data"][offset:offset + n].reshape(-1, 3, 32, 32).astype(np.float32) / 255.0
    labels = list(data[b"fine_labels"][offset:offset + n])

    images = (images - CIFAR100_MEAN) / CIFAR100_STD

    return torch.from_numpy(images), labels


def _find_cifar100_test_file() -> str:
    """Find CIFAR-100 test file path."""
    data_root = _get_data_root()
    test_file = data_root / "cifar100" / "cifar-100-python" / "test"

    if test_file.exists():
        return str(test_file)

    # Auto-download if not found
    cifar100_dir = _ensure_cifar100_downloaded(data_root)
    test_file = cifar100_dir / "test"
    return str(test_file)