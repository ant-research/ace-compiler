#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 (CIFAR) with ReLU replaced by the square activation x^2.

1:1 mirror of the reference implementation
(cti verify/onnx2c/cnn/resnet_cifar10_x2_model.py):
- conv3x3/conv1x1 carry a bias (uniform(-0.1, 0.1) init),
- the square sits at BOTH ReLU sites of the standard BasicBlock,
  including after the residual add — 19 squares total,
- untrained weights, validated FHE-vs-plaintext on random inputs.

The purpose is precision measurement: x -> x^2 is a single cipher*cipher
MUL — exact in plaintext arithmetic, no ReLU polynomial/bootstrap in the
way — so the ciphertext-vs-plaintext logit difference is pure CKKS
noise. With untrained weights both paths evaluate the SAME function, so
no training is needed (nor is the structure trainable in float32: the
post-add square compounds magnitude as M^(2^9) across the stack).

Separate file/weight path from the standard ResNet-20
(fhe_dsl/python/model/resnet/resnet20.py) — the original model, its
weights and profiles are untouched.

The Square forward uses ``x * x`` (NOT torch.pow) so the FX tracer emits
a plain element-wise MUL — supported end-to-end by the torch frontend
(custom op ``mul``), the AIR NN layer, and every backend (phantom/ace/
ant; cipher*cipher is relin'd automatically).
"""

import torch
import torch.nn as nn


class Square(nn.Module):
    """x^2 activation (reference: MyReLU)."""

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return x * x


def conv3x3(in_planes, out_planes, stride=1):
    """3x3 convolution with padding (bias=True, as in the reference)."""
    return nn.Conv2d(in_planes, out_planes, kernel_size=3, stride=stride, padding=1)


def conv1x1(in_planes, out_planes, stride=1):
    """1x1 convolution (bias=True, as in the reference)."""
    return nn.Conv2d(in_planes, out_planes, kernel_size=1, stride=stride)


class BasicBlock(nn.Module):
    """Basic residual block, square activation replacing ReLU.

    Reference structure, including the square AFTER the residual add.
    """

    expansion = 1

    def __init__(self, inplanes, planes, stride=1, downsample=None):
        super().__init__()
        self.conv1 = conv3x3(inplanes, planes, stride)
        self.bn1 = nn.BatchNorm2d(planes)
        self.act = Square()
        self.conv2 = conv3x3(planes, planes)
        self.bn2 = nn.BatchNorm2d(planes)
        self.downsample = downsample
        self.stride = stride

    def forward(self, x):
        identity = x

        out = self.conv1(x)
        out = self.bn1(out)
        out = self.act(out)

        out = self.conv2(out)
        out = self.bn2(out)

        if self.downsample is not None:
            identity = self.downsample(x)

        out += identity
        out = self.act(out)

        return out


class ResNetX2(nn.Module):
    """ResNet-20 square-activation model, CIFAR-shaped input,
    square activation throughout.

    Reference structure (conv biases included)."""

    def __init__(self, block, layers, num_classes=10):
        super().__init__()
        self.inplanes = 16
        self.conv1 = conv3x3(3, 16)
        self.bn1 = nn.BatchNorm2d(16)
        self.act = Square()

        self.layer1 = self._make_layer(block, 16, layers[0])
        self.layer2 = self._make_layer(block, 32, layers[1], stride=2)
        self.layer3 = self._make_layer(block, 64, layers[2], stride=2)

        self.avgpool = nn.AdaptiveAvgPool2d((1, 1))
        self.fc = nn.Linear(64 * block.expansion, num_classes)

        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode="fan_out", nonlinearity="relu")
                nn.init.uniform_(m.bias, a=-0.1, b=0.1)
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

    def _make_layer(self, block, planes, blocks, stride=1):
        downsample = None
        if stride != 1 or self.inplanes != planes * block.expansion:
            downsample = nn.Sequential(
                conv1x1(self.inplanes, planes * block.expansion, stride),
                nn.BatchNorm2d(planes * block.expansion),
            )

        layers = []
        layers.append(block(self.inplanes, planes, stride, downsample))
        self.inplanes = planes * block.expansion
        for _ in range(1, blocks):
            layers.append(block(self.inplanes, planes))

        return nn.Sequential(*layers)

    def forward(self, x):
        x = self.conv1(x)
        x = self.bn1(x)
        x = self.act(x)

        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)

        x = self.avgpool(x)
        x = x.view(x.size(0), -1)
        x = self.fc(x)

        return x


def resnet20_x2(num_classes=10):
    """Create ResNet-20 square-activation model (reference structure)."""
    return ResNetX2(BasicBlock, [3, 3, 3], num_classes=num_classes)
