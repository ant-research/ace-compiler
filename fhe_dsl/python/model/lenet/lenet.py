#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Compact LeNet-style CNN for FHE classification.

A small CNN tuned for the FHE backends. It avoids avgpool/maxpool (the
phantom executor is known-broken on pool ops, see test_sample_smoke and the
ResNet route which uses AdaptiveAvgPool2d) and instead downsamples with
stride-2 convolutions, keeping channels and the final linear layer small so
the CKKS scale never goes out of bounds.

Structure: conv(3→8, s2) → relu → conv(8→16, s2) → relu → flatten → fc(1024→32)
→ relu → fc(32→10). Input is CIFAR-style [3, 32, 32].

Preprocessing (e.g. CIFAR normalization) is the caller's responsibility.
"""

import torch
import torch.nn as nn


class LeNet(nn.Module):
    """Compact LeNet: stride-2 conv blocks (no pooling) → flatten → 2 linear."""

    def __init__(self, num_classes: int = 10, in_channels: int = 3):
        super().__init__()
        self.conv1 = nn.Conv2d(in_channels, 8, kernel_size=3, stride=2, padding=1)
        self.relu1 = nn.ReLU(inplace=True)
        self.conv2 = nn.Conv2d(8, 16, kernel_size=3, stride=2, padding=1)
        self.relu2 = nn.ReLU(inplace=True)
        self.flatten = nn.Flatten()
        self.fc1 = nn.Linear(16 * 8 * 8, 32)
        self.relu3 = nn.ReLU(inplace=True)
        self.fc2 = nn.Linear(32, num_classes)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.relu1(self.conv1(x))    # [B, 8, 16, 16]
        x = self.relu2(self.conv2(x))    # [B, 16, 8, 8]
        x = self.flatten(x)              # [B, 1024]
        x = self.relu3(self.fc1(x))      # [B, 32]
        return self.fc2(x)               # [B, num_classes]