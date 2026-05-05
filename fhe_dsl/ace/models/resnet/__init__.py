#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet models for CIFAR-10/CIFAR-100 classification.

This module provides high-level abstractions for creating and using
ResNet models with FHE compilation.

Main interfaces:
- create_pretrained_resnet(): Unified entry point for creating models
- RESNET*_COMPILE_OPTIONS: Tuned FHE compile options
- CompileSpec instances: Unified model descriptions (in specs submodule)
"""
from .config import (
    RESNET20_COMPILE_OPTIONS,
    RESNET32_COMPILE_OPTIONS,
    RESNET32_CIFAR100_COMPILE_OPTIONS,
    RESNET44_COMPILE_OPTIONS,
    RESNET56_COMPILE_OPTIONS,
    RESNET110_COMPILE_OPTIONS,
)

from . import specs

# Import model creators and loaders internally for create_pretrained_resnet
from .resnet20 import (
    resnet20_cifar10,
    load_resnet20_pretrained,
)

from .resnet32 import (
    resnet32_cifar10,
    resnet32_cifar100,
    load_resnet32_pretrained,
)

from .resnet44 import (
    resnet44_cifar10,
    load_resnet44_pretrained,
)

from .resnet56 import (
    resnet56_cifar10,
    load_resnet56_pretrained,
)

from .resnet110 import (
    resnet110_cifar10,
    load_resnet110_pretrained,
)

__all__ = [
    # Unified entry point
    "create_pretrained_resnet",
    # Compile options
    "RESNET20_COMPILE_OPTIONS",
    "RESNET32_COMPILE_OPTIONS",
    "RESNET32_CIFAR100_COMPILE_OPTIONS",
    "RESNET44_COMPILE_OPTIONS",
    "RESNET56_COMPILE_OPTIONS",
    "RESNET110_COMPILE_OPTIONS",
    # Specs
    "specs",
]


def create_pretrained_resnet(n_layers: int = 20, num_classes: int = 10):
    """Create a pretrained ResNet model for CIFAR-10/CIFAR-100.

    Args:
        n_layers: ResNet depth (20, 32, 44, 56, 110)
        num_classes: Number of output classes (10 for CIFAR-10, 100 for CIFAR-100)

    Returns:
        Pretrained model in eval mode.
    """
    creators = {
        20: (resnet20_cifar10, load_resnet20_pretrained),
        32: (resnet32_cifar10, load_resnet32_pretrained),
        44: (resnet44_cifar10, load_resnet44_pretrained),
        56: (resnet56_cifar10, load_resnet56_pretrained),
        110: (resnet110_cifar10, load_resnet110_pretrained),
    }
    if n_layers not in creators:
        raise ValueError(f"Unsupported ResNet depth: {n_layers}. Choose from {list(creators.keys())}")
    create_fn, load_fn = creators[n_layers]
    model = create_fn(num_classes=num_classes)
    # resnet32's load function accepts num_classes to select weights
    if n_layers == 32:
        load_fn(model, num_classes=num_classes)
    else:
        load_fn(model)
    return model