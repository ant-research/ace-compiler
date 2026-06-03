#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Complete models for ACE FHE.

This package provides complete, ready-to-use models that can be compiled
with FHE. These models are more complex than the basic modules in
ace.sample.ops and represent end-to-end architectures.

Structure:
    - dataset.py:          Dataset utilities (CIFAR-10, CIFAR-100, etc.)
    - spec_resnet.py:      ResNet ModelSpec instances (FHE compilation specs)
    - train_resnet.py:     ResNet training script (CLI: python -m ace.model.train_resnet)
    - relu_profile.py:     ReLU VR profiling tool (CLI: python -m ace.model.relu_profile)
    - resnet/:             ResNet model definitions, weights, profiles

Usage:
    # Import ResNet specs
    from ace.model.spec_resnet import RESNET20_CIFAR10

    # Import dataset utilities
    from ace.model.dataset import load_cifar10_images, CIFAR10_CLASSES

    # Create a pretrained ResNet model
    from ace.model.resnet import create_pretrained_resnet
    model = create_pretrained_resnet(n_layers=20, num_classes=10)
"""

__all__ = []


# Lazy imports
def __getattr__(name):
    # Dataset utilities
    if name in ("load_cifar10_images", "CIFAR10_CLASSES", "CIFAR10_MEAN", "CIFAR10_STD",
                "load_cifar100_images", "CIFAR100_CLASSES", "CIFAR100_MEAN", "CIFAR100_STD"):
        from ace.model import dataset
        return getattr(dataset, name)
    # ResNet model creation
    if name == "create_pretrained_resnet":
        from ace.model import resnet
        return getattr(resnet, name)
    # ResNet specs
    if name in (
        "RESNET20_CIFAR10", "RESNET32_CIFAR10", "RESNET32_CIFAR100",
        "RESNET44_CIFAR10", "RESNET56_CIFAR10", "RESNET110_CIFAR10",
        "RESNET_CIFAR10_SPECS", "RESNET_CIFAR100_SPECS", "ALL_RESNET_SPECS",
    ):
        from ace.model import spec_resnet
        return getattr(spec_resnet, name)
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")