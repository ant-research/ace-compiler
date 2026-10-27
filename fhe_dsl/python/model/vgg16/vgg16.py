#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
VGG16 CNN for FHE classification (CIFAR-adapted).

Faithful VGG16 architecture: 13 × (Conv3x3 + ReLU) grouped into 5 blocks,
each block terminated by a 2×2 AveragePool2d, followed by Flatten → Linear.
CIFAR-shaped input [3, 32, 32] (5 pools take 32×32 → 1×1) with a single
linear classifier 512 → num_classes.

Each conv is followed by BatchNorm2d + ReLU (standard VGG; matches the
reference torch model). The compiler folds BN into the preceding conv at
build time (ModelPreparePass), exactly like the ResNet route, so the emitted
FHE IR is Conv/Relu/AveragePool/Gemm — BN never appears in the compiled
program. No Dropout.

Note: 2D AveragePool is only supported by the ant (CPU) backend — the
phantom / ace GPU executors segfault on pool ops (see
tests/regression/sample/test_sample_smoke_*.py). VGG16 is therefore an
ant-CPU-only model; LeNet used stride-2 convs to avoid pooling and run on GPU.

Preprocessing (e.g. CIFAR normalization) is the caller's responsibility.
"""

import torch
import torch.nn as nn

# Channel plan: an int is a Conv2d(c_in, int, 3, padding=1)+BN+ReLU step;
# "M" is an AvgPool2d(2, 2). Mirrors the reference vgg16_cifar10.onnx
# (cfg['VGG16'] in the upstream model definition).
_CFG = [64, 64, "M", 128, 128, "M", 256, 256, 256, "M", 512, 512, 512, "M", 512, 512, 512, "M"]


class VGG16(nn.Module):
    """VGG16 (CIFAR-adapted): conv blocks with avg-pool → flatten → linear."""

    def __init__(self, num_classes: int = 10, in_channels: int = 3, batchnorm: bool = True):
        super().__init__()
        self.features = self._make_features(in_channels, batchnorm)
        self.classifier = nn.Linear(512, num_classes)

    @staticmethod
    def _make_features(in_channels: int, batchnorm: bool) -> nn.Sequential:
        layers: list[nn.Module] = []
        cin = in_channels
        for v in _CFG:
            if v == "M":
                layers.append(nn.AvgPool2d(kernel_size=2, stride=2))
            else:
                layers.append(nn.Conv2d(cin, v, kernel_size=3, padding=1))
                if batchnorm:
                    layers.append(nn.BatchNorm2d(v))
                layers.append(nn.ReLU(inplace=True))
                cin = v
        return nn.Sequential(*layers)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.features(x)          # [B, 512, 1, 1]
        x = torch.flatten(x, 1)       # [B, 512]
        return self.classifier(x)     # [B, num_classes]