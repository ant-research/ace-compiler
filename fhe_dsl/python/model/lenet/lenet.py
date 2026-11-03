#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Reference LeNet (CIFAR-10), ported from the original demo
``cti/verify/onnx2c/cnn/lenet.py``.

Two deliberate changes vs. the reference:

1. **MaxPool2d -> AvgPool2d.** The phantom / ace GPU executors do not
   support MaxPool, while ``nn.AvgPool2d`` lowers to ``OPC_AVERAGE_POOL``
   and is supported end-to-end.
2. **``F.relu(x)`` -> ``self.reluN(x)`` modules.** The ReLU VR profiler
   (ace.fhe.config.ReLUProfiler) discovers ``nn.ReLU`` modules and
   ``torch.relu``, but not ``F.relu``; functional ReLUs would silently
   fall back to ``relu_vr_def`` for every node. Using modules keeps the
   per-node value ranges accurate. This is numerically identical to the
   reference (ReLU has no parameters).

Structure (input [1, 3, 32, 32]):
    conv1(3->6,  k5) -> relu1 -> pool1(2,2)   # [1, 6, 14, 14]
    conv2(6->16, k5) -> relu2 -> pool2(2,2)   # [1, 16, 5, 5]
    flatten                                   # [1, 400]
    fc1(400->120) -> relu3
    fc2(120->84)  -> relu4
    fc3(84->10)

The reference demo has no trained weights; the weights here come from
``train_lenet.py`` (trained on the repo's CIFAR-10 split).
"""

import torch
import torch.nn as nn


class LeNet(nn.Module):
    """Original demo LeNet with AvgPool2d in place of MaxPool2d."""

    def __init__(self, num_classes: int = 10, in_channels: int = 3):
        super().__init__()
        self.conv1 = nn.Conv2d(in_channels, 6, 5)
        self.relu1 = nn.ReLU()
        self.pool1 = nn.AvgPool2d(2, 2)
        self.conv2 = nn.Conv2d(6, 16, 5)
        self.relu2 = nn.ReLU()
        self.pool2 = nn.AvgPool2d(2, 2)
        self.fc1 = nn.Linear(16 * 5 * 5, 120)
        self.relu3 = nn.ReLU()
        self.fc2 = nn.Linear(120, 84)
        self.relu4 = nn.ReLU()
        self.fc3 = nn.Linear(84, num_classes)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.pool1(self.relu1(self.conv1(x)))   # [B, 6, 14, 14]
        x = self.pool2(self.relu2(self.conv2(x)))   # [B, 16, 5, 5]
        x = torch.flatten(x, 1)                     # [B, 400]
        x = self.relu3(self.fc1(x))                 # [B, 120]
        x = self.relu4(self.fc2(x))                 # [B, 84]
        return self.fc3(x)                          # [B, num_classes]