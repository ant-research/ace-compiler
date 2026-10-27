#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AlexNet CNN model for FHE classification.

This package provides the AlexNet nn.Module. ModelSpec instances live at
ace.model.spec_alexnet (in the model package root, mirroring spec_resnet.py,
spec_lenet.py and spec_vgg16.py).

Usage:
    from ace.model.spec_alexnet import ALEXNET_CIFAR10
    model = ALEXNET_CIFAR10.create_model()

    x^2 (square-activation) variant (untrained, precision testing):
    from ace.model.spec_alexnet_x2 import ALEXNET_X2
    model = ALEXNET_X2.create_model()
"""

from .alexnet import AlexNet
from .alexnet_x2 import AlexNetX2

__all__ = [
    "AlexNet",
    "AlexNetX2",
]