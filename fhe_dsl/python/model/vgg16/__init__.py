#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
VGG16 CNN model for FHE classification.

This package provides the VGG16 nn.Module. ModelSpec instances live at
ace.model.spec_vgg16 (in the model package root, mirroring spec_resnet.py,
spec_lenet.py and spec_lr.py).

Usage:
    from ace.model.spec_vgg16 import VGG16_CIFAR10
    model = VGG16_CIFAR10.create_model()

    x^2 (square-activation) variant (untrained, precision testing):
    from ace.model.spec_vgg16_x2 import VGG16_X2
    model = VGG16_X2.create_model()
"""

from .vgg16 import VGG16
from .vgg16_x2 import VGG16X2

__all__ = [
    "VGG16",
    "VGG16X2",
]