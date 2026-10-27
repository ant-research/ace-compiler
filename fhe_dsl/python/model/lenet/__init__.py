#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
LeNet CNN model for FHE classification.

This package provides the LeNet nn.Module. ModelSpec instances live at
ace.model.spec_lenet (in the model package root, mirroring spec_resnet.py
and spec_lr.py).

Usage:
    from ace.model.spec_lenet import LENET_CIFAR10
    model = LENET_CIFAR10.create_model()

    x^2 (square-activation) variant (untrained, precision testing):
    from ace.model.spec_lenet_x2 import LENET_X2
    model = LENET_X2.create_model()
"""

from .lenet import LeNet
from .lenet_x2 import LeNetX2

__all__ = [
    "LeNet",
    "LeNetX2",
]