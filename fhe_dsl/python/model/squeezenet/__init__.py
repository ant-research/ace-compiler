#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
SqueezeNet CNN model for FHE classification.

This package provides the SqueezeNet nn.Module. ModelSpec instances live at
ace.model.spec_squeezenet (in the model package root, mirroring
spec_resnet.py, spec_lenet.py, spec_alexnet.py and spec_vgg16.py).

Usage:
    from ace.model.spec_squeezenet import SQUEEZENET_CIFAR10
    model = SQUEEZENET_CIFAR10.create_model()
"""

from .squeezenet import FireBlock, FireBlockPool, SqueezeNet


__all__ = [
    "FireBlock",
    "FireBlockPool",
    "SqueezeNet",
]
