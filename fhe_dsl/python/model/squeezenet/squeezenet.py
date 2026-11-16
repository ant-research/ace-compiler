#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
SqueezeNet (CIFAR-10 variant) for FHE classification.

Reimplementation of the reference ``squeezenet_cifar10.onnx``
(ace-compiler-iiecas ``model/squeezenet_cifar10.onnx``) as a plain torch
module. The reference graph was produced from the upstream
``cti/verify/onnx2c/cnn/squeezenet_cifar10_x2.py`` model *after* BN folding:
every Conv carries a bias and there are no BatchNorm initializers, so this
module folds BN into the Conv weights at construction time (bias=True) and
has no BN layers at all.

Architecture (input [1, 3, 32, 32], output [1, num_classes]):

    conv1  1x1  3→64                       → ReLU
    fire2  squeeze 64→16, expand 16→32/16→32 → Concat → ReLU
    fire3  squeeze 64→16, expand 16→32/16→32 → Concat → ReLU
    fire4  squeeze 64→32, expand 32→64/32→64 → AvgPool(2) on both branches
           → Concat → ReLU                                    32×32 → 16×16
    fire5  squeeze 128→32, expand 32→128/32→128 → Concat → ReLU
    fire6  squeeze 256→32, expand 32→128/32→128 → Concat → ReLU
    fire7  squeeze 256→32, expand 32→128/32→128 → Concat → ReLU
    fire8  squeeze 256→64, expand 64→256/64→256 → AvgPool(2) on both branches
           → Concat → ReLU                                    16×16 → 8×8
    fire9  squeeze 512→64, expand 64→256/64→256 → Concat → ReLU
    conv10 1x1 512→512                     → ReLU
    GlobalAveragePool → Flatten → Linear(512 → num_classes)

18 ReLUs, 26 Convs, 8 Concats, 4 AveragePool2d and 1 global average pool —
matching the reference ONNX op counts exactly.

The compiled IR is Conv/Relu/AveragePool/Concat/Gemm. Backend scope is
**ant (CPU) only**: pooling (and the concat) is supported by the antlib CPU
runtime but not by the phantom / ace GPU executors, same as AlexNet/VGG16.

Preprocessing (e.g. CIFAR normalization) is the caller's responsibility; the
shipped weights were trained with ImageNet mean/std, see
``import_reference_weights.py``.
"""

import torch
import torch.nn as nn


class FireBlock(nn.Module):
    """SqueezeNet fire module: 1x1 squeeze → two parallel expands → concat → ReLU."""

    def __init__(self, in_ch: int, squeeze_ch: int, expand1x1_ch: int, expand3x3_ch: int):
        super().__init__()
        self.squeeze = nn.Conv2d(in_ch, squeeze_ch, kernel_size=1, bias=True)
        self.expand1x1 = nn.Conv2d(squeeze_ch, expand1x1_ch, kernel_size=1, bias=True)
        self.expand3x3 = nn.Conv2d(squeeze_ch, expand3x3_ch, kernel_size=3, padding=1, bias=True)
        self.relu = nn.ReLU(inplace=False)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        s = self.relu(self.squeeze(x))
        return self.relu(torch.cat([self.expand1x1(s), self.expand3x3(s)], dim=1))


class FireBlockPool(FireBlock):
    """Fire module with a 2×2 average pool on each expand branch (downsampling)."""

    def __init__(self, in_ch: int, squeeze_ch: int, expand1x1_ch: int, expand3x3_ch: int):
        super().__init__(in_ch, squeeze_ch, expand1x1_ch, expand3x3_ch)
        self.pool = nn.AvgPool2d(kernel_size=2, stride=2)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        s = self.relu(self.squeeze(x))
        e1 = self.pool(self.expand1x1(s))
        e3 = self.pool(self.expand3x3(s))
        return self.relu(torch.cat([e1, e3], dim=1))


class SqueezeNet(nn.Module):
    """CIFAR-10 SqueezeNet: 10 fire stages, global average pool, one linear."""

    def __init__(self, num_classes: int = 10, in_channels: int = 3):
        super().__init__()
        self.conv1 = nn.Conv2d(in_channels, 64, kernel_size=1, bias=True)
        self.fire2 = FireBlock(64, 16, 32, 32)
        self.fire3 = FireBlock(64, 16, 32, 32)
        self.fire4 = FireBlockPool(64, 32, 64, 64)
        self.fire5 = FireBlock(128, 32, 128, 128)
        self.fire6 = FireBlock(256, 32, 128, 128)
        self.fire7 = FireBlock(256, 32, 128, 128)
        self.fire8 = FireBlockPool(256, 64, 256, 256)
        self.fire9 = FireBlock(512, 64, 256, 256)
        self.conv10 = nn.Conv2d(512, 512, kernel_size=1, bias=True)
        self.relu = nn.ReLU(inplace=False)
        self.avg_pool = nn.AdaptiveAvgPool2d((1, 1))
        self.fc = nn.Linear(512, num_classes)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.relu(self.conv1(x))
        x = self.fire2(x)
        x = self.fire3(x)
        x = self.fire4(x)          # 32×32 → 16×16
        x = self.fire5(x)
        x = self.fire6(x)
        x = self.fire7(x)
        x = self.fire8(x)          # 16×16 → 8×8
        x = self.fire9(x)
        x = self.relu(self.conv10(x))
        x = self.avg_pool(x)       # [B, 512, 1, 1]
        x = torch.flatten(x, 1)
        return self.fc(x)
