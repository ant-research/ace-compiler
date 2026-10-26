#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
VGG16 (CIFAR) with ReLU replaced by the square activation x^2.

1:1 mirror of the reference implementation
(cti verify/onnx2c/cnn/vgg_cifar_x2_model.py, VGG('VGG16')):
- cfg [64, 64, "M", 128, 128, "M", 256, 256, 256, "M",
  512, 512, 512, "M", 512, 512, 512, "M"] with BatchNorm after
  every conv — identical channels/pools to the standard VGG16
  here,
- the square replaces every ReLU — 13 squares total,
- untrained weights.

Like resnet20_x2, untrained BN running stats (mean=0, var=1) do
NOT tame the square-compounding magnitude: the 13-square float32
forward blows up. model_post_init therefore runs the same
deterministic seeded BN calibration used by the resnet20_x2 spec,
after which eval logits are small and finite.

The purpose is precision measurement: x -> x^2 is a single
cipher*cipher MUL — exact in plaintext arithmetic, no ReLU
polynomial in the way — so the ciphertext-vs-plaintext logit
difference is pure CKKS noise. With untrained weights both paths
evaluate the SAME function, so no training is needed.

Separate file from the standard VGG16 (vgg16.py) — the original
model, weights, VR profiles and specs are untouched.

The Square forward uses ``x * x`` (NOT torch.pow) so the
exporter emits a plain element-wise MUL — cipher*cipher is
relin'd automatically.
"""

import torch
import torch.nn as nn


# Channel plan: an int is Conv2d(c_in, int, 3, padding=1)+BN+Square;
# "M" is an AvgPool2d(2, 2). Same as the standard VGG16 here and the
# reference vgg16_cifar10 cfg.
_CFG = [64, 64, "M", 128, 128, "M", 256, 256, 256, "M", 512, 512, 512, "M", 512, 512, 512, "M"]


class Square(nn.Module):
    """x^2 activation (reference: MyReLU)."""

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return x * x


class VGG16X2(nn.Module):
    """VGG16 (CIFAR-adapted) with square activation: conv+BN blocks
    with avg-pool → flatten → linear."""

    def __init__(self, num_classes: int = 10, in_channels: int = 3):
        super().__init__()
        self.features = self._make_features(in_channels)
        self.classifier = nn.Linear(512, num_classes)

    @staticmethod
    def _make_features(in_channels: int) -> nn.Sequential:
        layers: list[nn.Module] = []
        cin = in_channels
        for v in _CFG:
            if v == "M":
                layers.append(nn.AvgPool2d(kernel_size=2, stride=2))
            else:
                layers.append(nn.Conv2d(cin, v, kernel_size=3, padding=1))
                layers.append(nn.BatchNorm2d(v))
                layers.append(Square())
                cin = v
        return nn.Sequential(*layers)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.features(x)          # [B, 512, 1, 1]
        x = torch.flatten(x, 1)       # [B, 512]
        return self.classifier(x)     # [B, num_classes]
