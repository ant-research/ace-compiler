#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Compact LeNet-style CNN with ReLU replaced by the square
activation x^2.

Structurally the SAME as the standard LeNet here (lenet.py,
itself the compact stride-2-conv LeNet tuned for the FHE
backends):
- conv(3→8, s2) → Square → conv(8→16, s2) → Square →
  flatten → fc(1024→32) → Square → fc(32→10) — 3 squares
  total (every ReLU site),
- no pooling layers, so unlike the reference
  (cti verify/onnx2c/cnn/lenet_x2.py, which pools with MaxPool2d)
  this variant also runs on the GPU executors
  (phantom/ace) that segfault on pool ops — ant-cpu works either way,
- untrained weights (default torch init; the shallow + narrow
  stack keeps float32 magnitudes finite — no BN calibration
  needed, there is no BN).

The purpose is precision measurement: x -> x^2 is a single
cipher*cipher MUL — exact in plaintext arithmetic, no ReLU
polynomial/bootstrap in the way — so the ciphertext-vs-plaintext
logit difference is pure CKKS noise, measured like the
resnet20_x2 / alexnet_x2 / vgg16_x2 experiments. With untrained
weights both paths evaluate the SAME function, so no training is
needed.

Separate file from the standard LeNet (lenet.py) — the original
model, its weights, its VR profiles and specs are untouched.

The Square forward uses ``x * x`` (NOT torch.pow) so the tracer
emits a plain element-wise MUL — supported end-to-end by the
frontend, the AIR NN layer, and every backend (phantom/ace/ant;
cipher*cipher is relin'd automatically).
"""

import torch
import torch.nn as nn


class Square(nn.Module):
    """x^2 activation (reference: MyReLU)."""

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return x * x


class LeNetX2(nn.Module):
    """Compact LeNet: stride-2 conv blocks (no pooling) → flatten →
    2 linear, square activation at every ReLU site."""

    def __init__(self, num_classes: int = 10, in_channels: int = 3):
        super().__init__()
        self.conv1 = nn.Conv2d(in_channels, 8, kernel_size=3, stride=2, padding=1)
        self.act1 = Square()
        self.conv2 = nn.Conv2d(8, 16, kernel_size=3, stride=2, padding=1)
        self.act2 = Square()
        self.fc1 = nn.Linear(16 * 8 * 8, 32)
        self.act3 = Square()
        self.fc2 = nn.Linear(32, num_classes)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.act1(self.conv1(x))    # [B, 8, 16, 16]
        x = self.act2(self.conv2(x))    # [B, 16, 8, 8]
        x = torch.flatten(x, 1)         # [B, 1024]
        x = self.act3(self.fc1(x))      # [B, 32]
        return self.fc2(x)              # [B, num_classes]