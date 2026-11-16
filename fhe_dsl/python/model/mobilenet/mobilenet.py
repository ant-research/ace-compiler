#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
MobileNet CNN for FHE classification (CIFAR-adapted).

Depthwise-separable MobileNet following the classic pytorch-cifar reference
(kuangliu/pytorch-cifar ``models/mobilenet.py``): a stride-1 3×3 stem
(3 → 32), 13 depthwise-separable blocks (3×3 depthwise conv with
``groups=in_planes`` + 1×1 pointwise conv, each Conv+BN+ReLU), a final
2×2 average pool and a 1024 → num_classes linear classifier.

    cfg = [64, (128,2), 128, (256,2), 256, (512,2), 512, 512, 512,
           512, 512, (1024,2), 1024]

Every Conv is followed by BatchNorm2d + ReLU. The compiler folds BN into the
preceding conv at build time (ModelPreparePass), exactly like the
ResNet/VGG16 route, so the emitted FHE IR is Conv/Relu/AveragePool/Gemm —
BN never appears in the compiled program. No Dropout.

Note: 2D AveragePool is only supported by the ant (CPU) backend — the
phantom / ace GPU executors segfault on pool ops. MobileNet is therefore an
ant-CPU-only model, same as AlexNet/VGG16/SqueezeNet.

The depthwise convs (``groups=in_planes``) are a new op shape for the FHE
compiler relative to AlexNet/VGG16/SqueezeNet — verified to compile through
the torch-via-onnx frontend on ant-cpu (see spec_mobilenet.py).

Preprocessing (e.g. CIFAR normalization) is the caller's responsibility.
"""

from typing import ClassVar

import torch
import torch.nn as nn
import torch.nn.functional as F


class Block(nn.Module):
    """Depthwise conv + Pointwise conv (each Conv+BN+ReLU)."""

    def __init__(self, in_planes: int, out_planes: int, stride: int = 1):
        super().__init__()
        self.conv1 = nn.Conv2d(in_planes, in_planes, kernel_size=3, stride=stride, padding=1,
                                groups=in_planes, bias=False)
        self.bn1 = nn.BatchNorm2d(in_planes)
        self.conv2 = nn.Conv2d(in_planes, out_planes, kernel_size=1, stride=1, padding=0, bias=False)
        self.bn2 = nn.BatchNorm2d(out_planes)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        out = F.relu(self.bn1(self.conv1(x)))
        out = F.relu(self.bn2(self.conv2(out)))
        return out


class MobileNet(nn.Module):
    """MobileNet (CIFAR-adapted): depthwise-separable blocks → avg-pool → linear.

    (128, 2) in ``cfg`` means conv planes=128, conv stride=2; a plain int
    means conv planes=N, conv stride=1.
    """

    # (128,2) means conv planes=128, conv stride=2, by default conv stride=1
    cfg: ClassVar[list] = [64, (128, 2), 128, (256, 2), 256, (512, 2), 512, 512, 512, 512, 512, (1024, 2), 1024]

    def __init__(self, num_classes: int = 10, in_channels: int = 3):
        super().__init__()
        self.conv1 = nn.Conv2d(in_channels, 32, kernel_size=3, stride=1, padding=1, bias=False)
        self.bn1 = nn.BatchNorm2d(32)
        self.layers = self._make_layers(in_planes=32)
        self.linear = nn.Linear(1024, num_classes)

    def _make_layers(self, in_planes: int) -> nn.Sequential:
        layers: list[nn.Module] = []
        for x in self.cfg:
            out_planes = x if isinstance(x, int) else x[0]
            stride = 1 if isinstance(x, int) else x[1]
            layers.append(Block(in_planes, out_planes, stride))
            in_planes = out_planes
        return nn.Sequential(*layers)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        out = F.relu(self.bn1(self.conv1(x)))
        out = self.layers(out)
        out = F.avg_pool2d(out, 2)   # [B, 1024, 1, 1]
        out = out.view(out.size(0), -1)
        return self.linear(out)      # [B, num_classes]
