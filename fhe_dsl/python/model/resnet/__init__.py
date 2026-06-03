#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet models for CIFAR-10/CIFAR-100 classification.

This module provides high-level abstractions for creating and using
ResNet models with FHE compilation.

Main interface:
- create_pretrained_resnet(): Unified entry point for creating models

For ModelSpec instances (with compile options), use ace.model.spec_resnet:
    from ace.model.spec_resnet import RESNET20_CIFAR10
"""

# Import model creators
from .resnet20 import resnet20_cifar10
from .resnet32 import resnet32_cifar10, resnet32_cifar100
from .resnet44 import resnet44_cifar10
from .resnet56 import resnet56_cifar10
from .resnet110 import resnet110_cifar10

__all__ = [
    # Unified entry point
    "create_pretrained_resnet",
]


def create_pretrained_resnet(n_layers: int = 20, num_classes: int = 10):
    """Create a pretrained ResNet model for CIFAR-10/CIFAR-100.

    Args:
        n_layers: ResNet depth (20, 32, 44, 56, 110)
        num_classes: Number of output classes (10 for CIFAR-10, 100 for CIFAR-100)

    Returns:
        Pretrained model in eval mode.
    """
    # Delay import to avoid circular dependency
    from ace.model.spec_resnet import load_resnet_pretrained

    creators = {
        20: resnet20_cifar10,
        32: resnet32_cifar10,
        44: resnet44_cifar10,
        56: resnet56_cifar10,
        110: resnet110_cifar10,
    }
    if n_layers not in creators:
        raise ValueError(f"Unsupported ResNet depth: {n_layers}. Choose from {list(creators.keys())}")
    create_fn = creators[n_layers]
    model = create_fn(num_classes=num_classes)
    load_resnet_pretrained(model, n_layers=n_layers, num_classes=num_classes)
    return model