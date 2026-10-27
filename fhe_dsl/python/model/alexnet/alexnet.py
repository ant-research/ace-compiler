#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AlexNet CNN for FHE classification (CIFAR-adapted).

CIFAR-adapted AlexNet, matching the reference torch model
(cti/verify/onnx2c/cnn/alexnet_cifar10_ax2.py) and the classic IIE ONNX
(model/alexnet_cifar10.onnx):

    conv 3→64 (s2, p1) → ReLU → AvgPool2d(2)     # 32→16→8
    conv 64→192 (p1)   → ReLU → AvgPool2d(2)     # 8→4
    conv 192→384 (p1)  → ReLU                    # 4
    conv 384→256 (p1)  → ReLU                    # 4
    conv 256→256 (p1)  → ReLU → AvgPool2d(2)     # 4→2
    flatten → Linear(1024→4096) → ReLU
            → Linear(4096→4096) → ReLU
            → Linear(4096→num_classes)

Input is CIFAR-shaped [3, 32, 32]; the stride-2 first conv and 3 avg-pools
take 32×32 → 2×2, so the first linear sees 256·2·2 = 1024 features. No
BatchNorm and no Dropout (Dropout is identity in eval and the FHE backend
ignores it; the reference model has a Dropout layer before each of the first
two linears, which is a no-op at inference).

Note: 2D AveragePool is only supported by the ant (CPU) backend — the
phantom / ace GPU executors segfault on pool ops (see
tests/regression/sample/test_sample_smoke_*.py). AlexNet is therefore an
ant-CPU-only model, exactly like VGG16.

Preprocessing (e.g. CIFAR normalization) is the caller's responsibility.
"""

import torch
import torch.nn as nn


class AlexNet(nn.Module):
    """CIFAR-adapted AlexNet: 5 conv + 3 avg-pool → flatten → 3 linear."""

    def __init__(self, num_classes: int = 10, in_channels: int = 3):
        super().__init__()
        self.features = nn.Sequential(
            nn.Conv2d(in_channels, 64, kernel_size=3, stride=2, padding=1),  # 32→16
            nn.ReLU(inplace=True),
            nn.AvgPool2d(kernel_size=2, stride=2),                           # 16→8
            nn.Conv2d(64, 192, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.AvgPool2d(kernel_size=2, stride=2),                           # 8→4
            nn.Conv2d(192, 384, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(384, 256, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(256, 256, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.AvgPool2d(kernel_size=2, stride=2),                           # 4→2
        )
        self.classifier = nn.Sequential(
            nn.Linear(256 * 2 * 2, 4096),
            nn.ReLU(inplace=True),
            nn.Linear(4096, 4096),
            nn.ReLU(inplace=True),
            nn.Linear(4096, num_classes),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.features(x)          # [B, 256, 2, 2]
        x = torch.flatten(x, 1)       # [B, 1024]
        return self.classifier(x)     # [B, num_classes]