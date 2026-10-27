#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AlexNet (CIFAR) with ReLU replaced by the square activation x^2.

1:1 mirror of the reference implementation
(cti verify/onnx2c/cnn/alexnet_cifar10_x2.py):
- same conv/pool layout as the standard AlexNet here (5 conv, 3
  2x2 avg-pools, 1024 → 4096 → 4096 → 10 classifier),
- the square sits at every ReLU site — 7 squares total,
- untrained weights (default torch init, kept uniform ±0.1 conv
  biases so magnitudes stay tame).

The purpose is precision measurement: x -> x^2 is a single
cipher*cipher MUL — exact in plaintext arithmetic, no ReLU
polynomial/bootstrap in the way — so the ciphertext-vs-plaintext
logit difference is pure CKKS noise, measured like the resnet20_x2
experiment. With untrained weights both paths evaluate the SAME
function, so no training is needed.

Separate file from the standard AlexNet (alexnet.py) — the original
model, its weights, its VR profiles and specs are untouched.

The Square forward uses ``x * x`` (NOT torch.pow) so the FX
tracer / ONNX exporter emits a plain element-wise MUL — supported
end-to-end by the frontend, the AIR NN layer, and every backend;
cipher*cipher is relin'd automatically.
"""

import torch
import torch.nn as nn


class Square(nn.Module):
    """x^2 activation (reference: MyReLU)."""

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return x * x


class AlexNetX2(nn.Module):
    """CIFAR-adapted AlexNet, square activation throughout.

    Reference structure (Dropout omitted — identity at inference and
    ignored by the FHE backend, same as the standard AlexNet here).
    """

    def __init__(self, num_classes: int = 10, in_channels: int = 3):
        super().__init__()
        self.features = nn.Sequential(
            nn.Conv2d(in_channels, 64, kernel_size=3, stride=2, padding=1),  # 32→16
            Square(),
            nn.AvgPool2d(kernel_size=2, stride=2),                           # 16→8
            nn.Conv2d(64, 192, kernel_size=3, padding=1),
            Square(),
            nn.AvgPool2d(kernel_size=2, stride=2),                           # 8→4
            nn.Conv2d(192, 384, kernel_size=3, padding=1),
            Square(),
            nn.Conv2d(384, 256, kernel_size=3, padding=1),
            Square(),
            nn.Conv2d(256, 256, kernel_size=3, padding=1),
            Square(),
            nn.AvgPool2d(kernel_size=2, stride=2),                           # 4→2
        )
        self.classifier = nn.Sequential(
            nn.Linear(256 * 2 * 2, 4096),
            Square(),
            nn.Linear(4096, 4096),
            Square(),
            nn.Linear(4096, num_classes),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.features(x)          # [B, 256, 2, 2]
        x = torch.flatten(x, 1)       # [B, 1024]
        return self.classifier(x)     # [B, num_classes]
