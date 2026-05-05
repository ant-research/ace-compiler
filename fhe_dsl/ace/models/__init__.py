#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Complete models for ACE FHE.

This package provides complete, ready-to-use models that can be compiled
with FHE. These models are more complex than the basic modules in
ace.samples.ops and represent end-to-end architectures.

Structure:
    - resnet/: ResNet family models
        - resnet20.py: ResNet-20 for CIFAR-10

Usage:
    # Import ResNet models
    from ace.models.resnet import ResNet_CIFAR, resnet20_cifar10
    from ace.models.resnet import load_resnet20_pretrained

    # Create a ResNet-20 model
    model = resnet20_cifar10(num_classes=10)
"""

__all__ = []

# Lazy imports for CIFAR-10 utilities
def __getattr__(name):
    if name in ("load_cifar10_images", "CIFAR10_CLASSES", "IMAGENET_MEAN", "IMAGENET_STD"):
        from ace.models import cifar10
        return getattr(cifar10, name)
    if name in ("create_pretrained_resnet", "RESNET20_COMPILE_OPTIONS", "RESNET32_CIFAR100_COMPILE_OPTIONS"):
        from ace.models import resnet
        return getattr(resnet, name)
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")