#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-32 model for CIFAR-10/CIFAR-100 classification.
"""
import os
import torch
import torch.nn as nn


def conv3x3(in_planes, out_planes, stride=1):
    """3x3 convolution with padding."""
    return nn.Conv2d(in_planes, out_planes, kernel_size=3, stride=stride, padding=1, bias=False)


def conv1x1(in_planes, out_planes, stride=1):
    """1x1 convolution."""
    return nn.Conv2d(in_planes, out_planes, kernel_size=1, stride=stride, bias=False)


class BasicBlock(nn.Module):
    """Basic residual block for ResNet."""
    expansion = 1

    def __init__(self, inplanes, planes, stride=1, downsample=None):
        super(BasicBlock, self).__init__()
        self.conv1 = conv3x3(inplanes, planes, stride)
        self.bn1 = nn.BatchNorm2d(planes)
        self.relu = nn.ReLU(inplace=True)
        self.conv2 = conv3x3(planes, planes)
        self.bn2 = nn.BatchNorm2d(planes)
        self.downsample = downsample
        self.stride = stride

    def forward(self, x):
        identity = x

        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)

        out = self.conv2(out)
        out = self.bn2(out)

        if self.downsample is not None:
            identity = self.downsample(x)

        out += identity
        out = self.relu(out)

        return out


class ResNet_CIFAR(nn.Module):
    """ResNet for CIFAR-10/CIFAR-100 classification."""

    def __init__(self, block, layers, num_classes=10):
        super(ResNet_CIFAR, self).__init__()
        self.inplanes = 16
        self.conv1 = conv3x3(3, 16)
        self.bn1 = nn.BatchNorm2d(16)
        self.relu = nn.ReLU(inplace=True)

        self.layer1 = self._make_layer(block, 16, layers[0])
        self.layer2 = self._make_layer(block, 32, layers[1], stride=2)
        self.layer3 = self._make_layer(block, 64, layers[2], stride=2)

        self.avgpool = nn.AdaptiveAvgPool2d((1, 1))
        self.fc = nn.Linear(64 * block.expansion, num_classes)

        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
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
        x = self.relu(x)

        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)

        x = self.avgpool(x)
        x = x.view(x.size(0), -1)
        x = self.fc(x)

        return x


def resnet32_cifar10(num_classes=10):
    """Create ResNet-32 model for CIFAR-10."""
    return ResNet_CIFAR(BasicBlock, [5, 5, 5], num_classes=num_classes)


# Alias: same architecture, just different default
resnet32_cifar100 = resnet32_cifar10


# ============================================================================
# Pre-trained weights loader
# ============================================================================

# Path to pre-trained weights (in weights/ subdirectory)
_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "weights")

_PRETRAINED_WEIGHTS = {
    10: os.path.join(_WEIGHTS_DIR, "cifar10_resnet32-ef93fc4d.pt"),
    100: os.path.join(_WEIGHTS_DIR, "cifar100_resnet32-84213ce6.pt"),
}


def get_pretrained_weights_path(num_classes: int = 10) -> str:
    """Return the path to the pre-trained weights file."""
    if num_classes not in _PRETRAINED_WEIGHTS:
        raise ValueError(f"No pretrained weights for num_classes={num_classes}. "
                         f"Available: {list(_PRETRAINED_WEIGHTS.keys())}")
    return _PRETRAINED_WEIGHTS[num_classes]


def load_resnet32_pretrained(model: nn.Module, num_classes: int = 10):
    """Load pre-trained weights into ResNet-32 model."""
    weights_path = _PRETRAINED_WEIGHTS.get(num_classes)
    if weights_path and os.path.exists(weights_path):
        state_dict = torch.load(weights_path, map_location='cpu')
        model.load_state_dict(state_dict)
    model.eval()