#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
import sys
import psutil
import math
import subprocess

# Minimal memory required to run a model in Gigabytes
REQUIRED_MEMORY = {
    'resnet20_cifar10': 50,
    'resnet32_cifar10': 50,
    'resnet32_cifar100': 60,
    'resnet44_cifar10': 50,
    'resnet56_cifar10': 50,
    'resnet110_cifar10': 50,
    'alexnet_cifar10' : 400,
    'squeezenet_cifar10' : 210,
    'vgg16_cifar10' : 730,
    'conv_3x16x32x3': 10,
    'conv_16x16x32x3': 10,
    'conv_32x32x16x3': 10,
    'conv_64x64x8x3': 10,
    'conv_64x64x56x3': 10,
    'conv_128x128x28x3': 10,
    'conv_256x256x14x3': 10,
    'conv_512x512x7x3': 15,
}
# Default options, supposed to be the fastest options
DEFAULT_OPTION = [
    '-CKKS:hw=192:q0=60:sf=56:sbm',
    '-P2C:fp'
]
# CGO25 test configurations
# Test models
CGO25_MODEL = [
    'resnet20_cifar10',
    'resnet32_cifar10',
    'resnet32_cifar100',
    'resnet44_cifar10',
    'resnet56_cifar10',
    'resnet110_cifar10'
]
# ACE compilation options for performance tests
CGO25_PERF_OPTION = [
    '-SIHE:relu_vr_def=2:relu_mul_depth=13',
    '-CKKS:hw=192:q0=60:sf=56',
    '-P2C:fp'
]
# Relu accuracy tuning result options
CGO25_RESNET20_CIFAR10_RELU_OPT = [
    '-SIHE:relu_vr_def=3:relu_vr=' +
    '/relu/Relu=4' +  # 1
    ';/layer1/layer1.0/relu_1/Relu=4' +  # 3
    ';/layer1/layer1.1/relu/Relu=4' +  # 4
    ';/layer1/layer1.1/relu_1/Relu=5' +  # 5
    ';/layer1/layer1.2/relu_1/Relu=5' +  # 7
    ';/layer2/layer2.0/relu_1/Relu=5' +  # 9
    ';/layer2/layer2.1/relu_1/Relu=5' +  # 11
    ';/layer2/layer2.2/relu_1/Relu=7' +  # 13
    ';/layer3/layer3.0/relu_1/Relu=4' +  # 15
    ';/layer3/layer3.1/relu_1/Relu=6' +  # 17
    ';/layer3/layer3.2/relu/Relu=4' +  # 18
    ';/layer3/layer3.2/relu_1/Relu=20'  # 19
]
CGO25_RESNET32_CIFAR10_RELU_OPT = [
    '-SIHE:relu_vr_def=2:relu_vr=' +
    '/relu/Relu=4' +  # 1
    ';/layer1/layer1.0/relu/Relu=3' +  # 2
    ';/layer1/layer1.0/relu_1/Relu=5' +  # 3
    ';/layer1/layer1.1/relu/Relu=3' +  # 4
    ';/layer1/layer1.1/relu_1/Relu=5' +  # 5
    ';/layer1/layer1.2/relu/Relu=3' +  # 6
    ';/layer1/layer1.2/relu_1/Relu=5' +  # 7
    ';/layer1/layer1.3/relu/Relu=3' +  # 8
    ';/layer1/layer1.3/relu_1/Relu=5' +  # 9
    ';/layer1/layer1.4/relu/Relu=3' +  # 10
    ';/layer1/layer1.4/relu_1/Relu=5' +  # 11
    ';/layer2/layer2.0/relu/Relu=3' +  # 12
    ';/layer2/layer2.0/relu_1/Relu=5' +  # 13
    ';/layer2/layer2.1/relu_1/Relu=5' +  # 15
    ';/layer2/layer2.2/relu_1/Relu=5' +  # 17
    ';/layer2/layer2.3/relu_1/Relu=6' +  # 19
    ';/layer2/layer2.4/relu_1/Relu=6' +  # 21
    ';/layer3/layer3.0/relu/Relu=3' +  # 22
    ';/layer3/layer3.0/relu_1/Relu=5' +  # 23
    ';/layer3/layer3.1/relu_1/Relu=4' +  # 25
    ';/layer3/layer3.2/relu/Relu=3' +  # 26
    ';/layer3/layer3.2/relu_1/Relu=6' +  # 27
    ';/layer3/layer3.3/relu/Relu=4' +  # 28
    ';/layer3/layer3.3/relu_1/Relu=10' +  # 29
    ';/layer3/layer3.4/relu_1/Relu=11'  # 31
]
CGO25_RESNET32_CIFAR100_RELU_OPT = [
    '-SIHE:relu_vr_def=3:relu_vr=' +
    '/relu/Relu=5' +  # 1
    ';/layer1/layer1.0/relu_1/Relu=6' +  # 3
    ';/layer1/layer1.1/relu_1/Relu=7' +  # 5
    ';/layer1/layer1.2/relu_1/Relu=8' +  # 7
    ';/layer1/layer1.3/relu_1/Relu=10' +  # 9
    ';/layer1/layer1.4/relu/Relu=4' +  # 10
    ';/layer1/layer1.4/relu_1/Relu=7' +  # 11
    ';/layer2/layer2.0/relu/Relu=4' +  # 12
    ';/layer2/layer2.0/relu_1/Relu=6' +  # 13
    ';/layer2/layer2.1/relu_1/Relu=8' +  # 15
    ';/layer2/layer2.2/relu/Relu=4' +  # 16
    ';/layer2/layer2.2/relu_1/Relu=8' +  # 17
    ';/layer2/layer2.3/relu_1/Relu=9' +  # 19
    ';/layer2/layer2.4/relu_1/Relu=11' +  # 21
    ';/layer3/layer3.0/relu/Relu=4' +  # 22
    ';/layer3/layer3.0/relu_1/Relu=8' +  # 23
    ';/layer3/layer3.1/relu_1/Relu=9' +  # 25
    ';/layer3/layer3.2/relu/Relu=4' +  # 26
    ';/layer3/layer3.2/relu_1/Relu=11' +  # 27
    ';/layer3/layer3.3/relu/Relu=4' +  # 28
    ';/layer3/layer3.3/relu_1/Relu=26' +  # 29
    ';/layer3/layer3.4/relu/Relu=5' +  # 30
    ';/layer3/layer3.4/relu_1/Relu=46'  # 31
]
CGO25_RESNET44_CIFAR10_RELU_OPT = [
    '-SIHE:relu_vr_def=2:relu_vr=' +
    '/relu/Relu=4' +  # 1
    ';/layer1/layer1.0/relu_1/Relu=5' +  # 3
    ';/layer1/layer1.1/relu_1/Relu=5' +  # 5
    ';/layer1/layer1.2/relu_1/Relu=5' +  # 7
    ';/layer1/layer1.3/relu_1/Relu=6' +  # 9
    ';/layer1/layer1.4/relu_1/Relu=6' +  # 11
    ';/layer1/layer1.5/relu/Relu=2' +  # 12
    ';/layer1/layer1.5/relu_1/Relu=7' +  # 13
    ';/layer1/layer1.6/relu_1/Relu=6' +  # 15
    ';/layer2/layer2.0/relu/Relu=2' +  # 16
    ';/layer2/layer2.0/relu_1/Relu=5' +  # 17
    ';/layer2/layer2.1/relu/Relu=2' +  # 18
    ';/layer2/layer2.1/relu_1/Relu=5' +  # 19
    ';/layer2/layer2.2/relu/Relu=2' +  # 20
    ';/layer2/layer2.2/relu_1/Relu=5' +  # 21
    ';/layer2/layer2.3/relu/Relu=2' +  # 22
    ';/layer2/layer2.3/relu_1/Relu=5' +  # 23
    ';/layer2/layer2.4/relu/Relu=2' +  # 24
    ';/layer2/layer2.4/relu_1/Relu=5' +  # 25
    ';/layer2/layer2.5/relu/Relu=2' +  # 26
    ';/layer2/layer2.5/relu_1/Relu=6' +  # 27
    ';/layer2/layer2.6/relu/Relu=2' +  # 28
    ';/layer2/layer2.6/relu_1/Relu=7' +  # 29
    ';/layer3/layer3.0/relu_1/Relu=5' +  # 31
    ';/layer3/layer3.1/relu/Relu=2' +  # 32
    ';/layer3/layer3.1/relu_1/Relu=5' +  # 33
    ';/layer3/layer3.2/relu/Relu=2' +  # 34
    ';/layer3/layer3.2/relu_1/Relu=6' +  # 35
    ';/layer3/layer3.3/relu_1/Relu=7' +  # 37
    ';/layer3/layer3.4/relu_1/Relu=9' +  # 39
    ';/layer3/layer3.5/relu_1/Relu=15' +  # 41
    ';/layer3/layer3.6/relu_1/Relu=16'  # 43
]
CGO25_RESNET56_CIFAR10_RELU_OPT = [
    '-SIHE:relu_vr_def=2:relu_vr=' +
    '/relu/Relu=4' +  # 1
    ';/layer1/layer1.0/relu_1/Relu=6' +  # 3
    ';/layer1/layer1.1/relu_1/Relu=5' +  # 5
    ';/layer1/layer1.2/relu/Relu=3' +  # 6
    ';/layer1/layer1.2/relu_1/Relu=6' +  # 7
    ';/layer1/layer1.3/relu/Relu=3' +  # 8
    ';/layer1/layer1.3/relu_1/Relu=7' +  # 9
    ';/layer1/layer1.4/relu_1/Relu=6' +  # 11
    ';/layer1/layer1.5/relu_1/Relu=6' +  # 13
    ';/layer1/layer1.6/relu_1/Relu=6' +  # 15
    ';/layer1/layer1.7/relu_1/Relu=6' +  # 17
    ';/layer1/layer1.8/relu_1/Relu=5' +  # 19
    ';/layer2/layer2.0/relu_1/Relu=4' +  # 21
    ';/layer2/layer2.1/relu_1/Relu=4' +  # 23
    ';/layer2/layer2.2/relu_1/Relu=5' +  # 25
    ';/layer2/layer2.3/relu_1/Relu=5' +  # 27
    ';/layer2/layer2.4/relu_1/Relu=6' +  # 29
    ';/layer2/layer2.5/relu_1/Relu=8' +  # 31
    ';/layer2/layer2.6/relu_1/Relu=11' +  # 33
    ';/layer2/layer2.7/relu_1/Relu=11' +  # 35
    ';/layer2/layer2.8/relu_1/Relu=12' +  # 37
    ';/layer3/layer3.0/relu/Relu=3' +  # 38
    ';/layer3/layer3.0/relu_1/Relu=5' +  # 39
    ';/layer3/layer3.1/relu_1/Relu=5' +  # 41
    ';/layer3/layer3.2/relu_1/Relu=5' +  # 43
    ';/layer3/layer3.3/relu_1/Relu=5' +  # 45
    ';/layer3/layer3.4/relu_1/Relu=5' +  # 47
    ';/layer3/layer3.5/relu_1/Relu=6' +  # 49
    ';/layer3/layer3.6/relu_1/Relu=8' +  # 51
    ';/layer3/layer3.7/relu/Relu=3' +  # 52
    ';/layer3/layer3.7/relu_1/Relu=10' +  # 53
    ';/layer3/layer3.8/relu_1/Relu=12'  # 55
]
CGO25_RESNET110_CIFAR10_RELU_OPT = [
    '-SIHE:relu_vr_def=3:relu_vr=' +
    '/relu/Relu=14' +  # 109
    ';/layer1/layer1.0/relu/Relu=4' +  # 1
    ';/layer1/layer1.0/relu_1/Relu=14' +  # 2
    ';/layer1/layer1.1/relu/Relu=4' +  # 3
    ';/layer1/layer1.1/relu_1/Relu=15' +  # 4
    ';/layer1/layer1.10/relu/Relu=5' +  # 5
    ';/layer1/layer1.10/relu_1/Relu=18' +  # 6
    ';/layer1/layer1.11/relu/Relu=5' +  # 7
    ';/layer1/layer1.11/relu_1/Relu=17' +  # 8
    ';/layer1/layer1.12/relu/Relu=4' +  # 9
    ';/layer1/layer1.12/relu_1/Relu=21' +  # 10
    ';/layer1/layer1.13/relu/Relu=7' +  # 11
    ';/layer1/layer1.13/relu_1/Relu=22' +  # 12
    ';/layer1/layer1.14/relu/Relu=7' +  # 13
    ';/layer1/layer1.14/relu_1/Relu=23' +  # 14
    ';/layer1/layer1.15/relu/Relu=7' +  # 15
    ';/layer1/layer1.15/relu_1/Relu=25' +  # 16
    ';/layer1/layer1.16/relu/Relu=6' +  # 17
    ';/layer1/layer1.16/relu_1/Relu=22' +  # 18
    ';/layer1/layer1.17/relu/Relu=6' +  # 19
    ';/layer1/layer1.17/relu_1/Relu=22' +  # 20
    ';/layer1/layer1.2/relu/Relu=3' +  # 21
    ';/layer1/layer1.2/relu_1/Relu=15' +  # 22
    ';/layer1/layer1.3/relu/Relu=6' +  # 23
    ';/layer1/layer1.3/relu_1/Relu=20' +  # 24
    ';/layer1/layer1.4/relu/Relu=5' +  # 25
    ';/layer1/layer1.4/relu_1/Relu=17' +  # 26
    ';/layer1/layer1.5/relu/Relu=5' +  # 27
    ';/layer1/layer1.5/relu_1/Relu=17' +  # 28
    ';/layer1/layer1.6/relu/Relu=5' +  # 29
    ';/layer1/layer1.6/relu_1/Relu=17' +  # 30
    ';/layer1/layer1.7/relu/Relu=4' +  # 31
    ';/layer1/layer1.7/relu_1/Relu=17' +  # 32
    ';/layer1/layer1.8/relu/Relu=4' +  # 33
    ';/layer1/layer1.8/relu_1/Relu=17' +  # 34
    ';/layer1/layer1.9/relu/Relu=5' +  # 35
    ';/layer1/layer1.9/relu_1/Relu=18' +  # 36
    ';/layer2/layer2.0/relu/Relu=6' +  # 37
    ';/layer2/layer2.0/relu_1/Relu=14' +  # 38
    ';/layer2/layer2.1/relu/Relu=4' +  # 39
    ';/layer2/layer2.1/relu_1/Relu=14' +  # 40
    ';/layer2/layer2.10/relu/Relu=3' +  # 41
    ';/layer2/layer2.10/relu_1/Relu=19' +  # 42
    ';/layer2/layer2.11/relu/Relu=3' +  # 43
    ';/layer2/layer2.11/relu_1/Relu=19' +  # 44
    ';/layer2/layer2.12/relu/Relu=3' +  # 45
    ';/layer2/layer2.12/relu_1/Relu=22' +  # 46
    ';/layer2/layer2.13/relu/Relu=3' +  # 47
    ';/layer2/layer2.13/relu_1/Relu=21' +  # 48
    ';/layer2/layer2.14/relu/Relu=3' +  # 49
    ';/layer2/layer2.14/relu_1/Relu=23' +  # 50
    ';/layer2/layer2.15/relu/Relu=3' +  # 51
    ';/layer2/layer2.15/relu_1/Relu=22' +  # 52
    ';/layer2/layer2.16/relu/Relu=4' +  # 53
    ';/layer2/layer2.16/relu_1/Relu=23' +  # 54
    ';/layer2/layer2.17/relu/Relu=3' +  # 55
    ';/layer2/layer2.17/relu_1/Relu=22' +  # 56
    ';/layer2/layer2.2/relu/Relu=3' +  # 57
    ';/layer2/layer2.2/relu_1/Relu=15' +  # 58
    ';/layer2/layer2.3/relu/Relu=4' +  # 59
    ';/layer2/layer2.3/relu_1/Relu=16' +  # 60
    ';/layer2/layer2.4/relu/Relu=4' +  # 61
    ';/layer2/layer2.4/relu_1/Relu=16' +  # 62
    ';/layer2/layer2.5/relu/Relu=3' +  # 63
    ';/layer2/layer2.5/relu_1/Relu=17' +  # 64
    ';/layer2/layer2.6/relu/Relu=5' +  # 65
    ';/layer2/layer2.6/relu_1/Relu=17' +  # 66
    ';/layer2/layer2.7/relu/Relu=3' +  # 67
    ';/layer2/layer2.7/relu_1/Relu=17' +  # 68
    ';/layer2/layer2.8/relu/Relu=3' +  # 69
    ';/layer2/layer2.8/relu_1/Relu=18' +  # 70
    ';/layer2/layer2.9/relu/Relu=4' +  # 71
    ';/layer2/layer2.9/relu_1/Relu=19' +  # 72
    ';/layer3/layer3.0/relu/Relu=5' +  # 73
    ';/layer3/layer3.0/relu_1/Relu=14' +  # 74
    ';/layer3/layer3.1/relu/Relu=4' +  # 75
    ';/layer3/layer3.1/relu_1/Relu=15' +  # 76
    ';/layer3/layer3.10/relu/Relu=3' +  # 77
    ';/layer3/layer3.10/relu_1/Relu=20' +  # 78
    ';/layer3/layer3.11/relu/Relu=4' +  # 79
    ';/layer3/layer3.11/relu_1/Relu=20' +  # 80
    ';/layer3/layer3.12/relu/Relu=4' +  # 81
    ';/layer3/layer3.12/relu_1/Relu=20' +  # 82
    ';/layer3/layer3.13/relu/Relu=4' +  # 83
    ';/layer3/layer3.13/relu_1/Relu=24' +  # 84
    ';/layer3/layer3.14/relu/Relu=4' +  # 85
    ';/layer3/layer3.14/relu_1/Relu=27' +  # 86
    ';/layer3/layer3.15/relu/Relu=4' +  # 87
    ';/layer3/layer3.15/relu_1/Relu=30' +  # 88
    ';/layer3/layer3.16/relu/Relu=4' +  # 89
    ';/layer3/layer3.16/relu_1/Relu=27' +  # 90
    ';/layer3/layer3.17/relu/Relu=9' +  # 91
    ';/layer3/layer3.17/relu_1/Relu=33' +  # 92
    ';/layer3/layer3.2/relu/Relu=3' +  # 93
    ';/layer3/layer3.2/relu_1/Relu=15' +  # 94
    ';/layer3/layer3.3/relu/Relu=4' +  # 95
    ';/layer3/layer3.3/relu_1/Relu=15' +  # 96
    ';/layer3/layer3.4/relu/Relu=3' +  # 97
    ';/layer3/layer3.4/relu_1/Relu=15' +  # 98
    ';/layer3/layer3.5/relu/Relu=3' +  # 99
    ';/layer3/layer3.5/relu_1/Relu=16' +  # 100
    ';/layer3/layer3.6/relu/Relu=3' +  # 101
    ';/layer3/layer3.6/relu_1/Relu=16' +  # 102
    ';/layer3/layer3.7/relu/Relu=3' +  # 103
    ';/layer3/layer3.7/relu_1/Relu=16' +  # 104
    ';/layer3/layer3.8/relu/Relu=3' +  # 105
    ';/layer3/layer3.8/relu_1/Relu=17' +  # 106
    ';/layer3/layer3.9/relu/Relu=3' +  # 107
    ';/layer3/layer3.9/relu_1/Relu=18'  # 108
]

ASPLOS25_ALEXNET_CIFAR10_RELU_OPT = [
    '-SIHE:relu_vr_def=2:relu_vr=' +
    '/classifier/classifier.2/Relu=9' +  # 1
    ';/classifier/classifier.5/Relu=16' +  # 2
    ';/features/features.10/Relu=6' +  # 3
    ';/features/features.13/Relu=6' +  # 4
    ';/features/features.16/Relu=6' +  # 5
    ';/features/features.2/Relu=9' +  # 6
    ';/features/features.6/Relu=6' # 7
]

ASPLOS25_MOBILENET_CIFAR10_RELU_OPT = [
    '-SIHE:relu_vr_def=6:relu_vr=' +
    '/Relu=7' + # 1
    ';/layers/layers.0/Relu=6' + # 2
    ';/layers/layers.0/Relu_1=6' + # 3
    ';/layers/layers.1/Relu=6' +  # 4
    ';/layers/layers.1/Relu_1=6' + # 5
    ';/layers/layers.10/Relu=6' + # 6
    ';/layers/layers.10/Relu_1=6' + # 7
    ';/layers/layers.11/Relu=6' + # 8
    ';/layers/layers.11/Relu_1=6' + # 9
    ';/layers/layers.12/Relu=3' + # 10
    ';/layers/layers.12/Relu_1=3' + # 11
    ';/layers/layers.2/Relu=7' +  # 12
    ';/layers/layers.2/Relu_1=3' +  # 13
    ';/layers/layers.3/Relu=6' + # 14
    ';/layers/layers.3/Relu_1=3' + # 15
    ';/layers/layers.4/Relu=6' + # 16
    ';/layers/layers.4/Relu_1=3"' + # 17
    ';/layers/layers.5/Relu=6' + # 18
    ';/layers/layers.5/Relu_1=6' + # 19
    ';/layers/layers.6/Relu=3' + # 20
    ';/layers/layers.6/Relu_1=6' + # 21
    ';/layers/layers.7/Relu=6' + # 22
    ';/layers/layers.7/Relu_1=6' + # 23
    ';/layers/layers.8/Relu=3' + # 24
    ';/layers/layers.8/Relu_1=6' + # 25
    ';/layers/layers.9/Relu=6' + # 26
    ';/layers/layers.9/Relu_1=6' # 27
]

ASPLOS25_SQUEEZENET_CIFAR10_RELU_OPT = [
    '-SIHE:relu_vr_def=2:relu_vr=' +
    '/conv1/conv1.2/Relu=2' +  # 1
    ';/conv10/conv10.2/Relu=11' +  # 2
    ';/fire2/Relu=3' +  # 3
    ';/fire2/squeeze/squeeze.2/Relu=2' +  # 4
    ';/fire3/Relu=4' +  # 5
    ';/fire3/squeeze/squeeze.2/Relu=3' +  # 6
    ';/fire4/Relu=3' + # 7
    ';/fire4/squeeze/squeeze.2/Relu=4' + # 8
    ';/fire5/Relu=3' + # 9
    ';/fire5/squeeze/squeeze.2/Relu=4' + # 10
    ';/fire6/Relu=3' + # 11
    ';/fire6/squeeze/squeeze.2/Relu=4' + # 12
    ';/fire7/Relu=3' + # 13
    ';/fire7/squeeze/squeeze.2/Relu=4' + # 14
    ';/fire8/Relu=3' + # 15
    ';/fire8/squeeze/squeeze.2/Relu=5' + # 16
    ';/fire9/Relu=13' + # 17
    ';/fire9/squeeze/squeeze.2/Relu=5' # 18
]

ASPLOS25_VGG16_CIFAR10_RELU_OPT = [
    '-SIHE:relu_vr_def=4:relu_vr=' +
    '/features/features.12/Relu=4' +  # 1
    ';/features/features.16/Relu=4' + # 2
    ';/features/features.19/Relu=4' + # 3
    ';/features/features.2/Relu=5' +  # 4
    ';/features/features.22/Relu=4' + # 5
    ';/features/features.26/Relu=4' + # 6
    ';/features/features.29/Relu=4' + # 7
    ';/features/features.32/Relu=4' + # 8
    ';/features/features.36/Relu=4' + # 9
    ';/features/features.39/Relu=4' + # 10
    ';/features/features.42/Relu=4' + # 11
    ';/features/features.5/Relu=5' +  # 12
    ';/features/features.9/Relu=4'    # 13
]

# Default configurations
DEFAULT_CONFIG = {
    'test': CGO25_MODEL,
    'perf': DEFAULT_OPTION,
    'accuracy': DEFAULT_OPTION,
    'resnet20_cifar10_relu': CGO25_RESNET20_CIFAR10_RELU_OPT,
    'resnet32_cifar10_relu': CGO25_RESNET32_CIFAR10_RELU_OPT,
    'resnet32_cifar100_relu': CGO25_RESNET32_CIFAR100_RELU_OPT,
    'resnet44_cifar10_relu': CGO25_RESNET44_CIFAR10_RELU_OPT,
    'resnet56_cifar10_relu': CGO25_RESNET56_CIFAR10_RELU_OPT,
    'resnet110_cifar10_relu': CGO25_RESNET110_CIFAR10_RELU_OPT
}
# CGO25 test configurations
CGO25_CONFIG = {
    'test': CGO25_MODEL,
    'perf': CGO25_PERF_OPTION,
    'accuracy': DEFAULT_OPTION,
    'resnet20_cifar10_relu': CGO25_RESNET20_CIFAR10_RELU_OPT,
    'resnet32_cifar10_relu': CGO25_RESNET32_CIFAR10_RELU_OPT,
    'resnet32_cifar100_relu': CGO25_RESNET32_CIFAR100_RELU_OPT,
    'resnet44_cifar10_relu': CGO25_RESNET44_CIFAR10_RELU_OPT,
    'resnet56_cifar10_relu': CGO25_RESNET56_CIFAR10_RELU_OPT,
    'resnet110_cifar10_relu': CGO25_RESNET110_CIFAR10_RELU_OPT
}

# ASPLOS25 test configurations
ASPLOS25_MODEL = [
    'alexnet_cifar10',
    'resnet20_cifar10',
    'squeezenet_cifar10',
    'vgg16_cifar10'
]

ASPLOS25_ACC_OPTION = [
    '-CKKS:hw=192:q0=60:sf=57:sbm',
    '-P2C:fp'
]

ASPLOS25_PERF_OPTION = [
    '-SIHE:relu_vr_def=2:relu_mul_depth=13',
    '-CKKS:hw=192:q0=60:sf=56',
    '-P2C:fp'
]

ASPLOS25_CONFIG = {
    'test': ASPLOS25_MODEL,
    'perf': ASPLOS25_PERF_OPTION,
    'accuracy': ASPLOS25_ACC_OPTION,
    'alexnet_cifar10_relu': ASPLOS25_ALEXNET_CIFAR10_RELU_OPT,
    'resnet20_cifar10_relu': CGO25_RESNET20_CIFAR10_RELU_OPT,
    'squeezenet_cifar10_relu': ASPLOS25_SQUEEZENET_CIFAR10_RELU_OPT,
    'vgg16_cifar10_relu': ASPLOS25_VGG16_CIFAR10_RELU_OPT
}

# OPSLA25 test configurations
OOPSLA25_MODEL = [
    'conv_3x16x32x3',
    'conv_16x16x32x3',
    'conv_32x32x16x3',
    'conv_64x64x8x3',
    'conv_64x64x56x3',
    'conv_128x128x28x3',
    'conv_256x256x14x3',
    'conv_512x512x7x3',
    'gemv_4096x4096',   # 64M size onnx file
    'gemv_4096x25088'   # 392M size onnx file
]

# ACE compilation options for performance tests
OOPSLA25_PERF_OPTION = [
    '-VEC:ms=32768:ssf:conv_parl:sharding:rtt',
    '-SIHE:relu_vr_def=100:rtt',
    '-CKKS:hw=192:q0=60:sf=56:N=65536:pots:rtt',
    '-POLY:rtt',
    '-P2C:fp'           # df option is enabled in the ace_compile_and_link method
]

OOPSLA25_CONFIG = {
    'test': OOPSLA25_MODEL,
    'perf': OOPSLA25_PERF_OPTION
}

def write_log(info, log):
    '''
    Print out info and write to log file if exists

    Args:
        info(str): message to print out
        log(file): log file
    '''
    print(info[:-1])
    if log is not None:
        log.write(info)
        log.flush()
    return


def time_and_memory(outputs):
    '''
    Identify time and memory info from time output

    Args:
        outputs(str): 'time -f "%e %M"' outputs

    Returns:
        (time, memory)(float, float): execution time & memory
    '''
    result = outputs.strip('"').split(' ')
    return float(result[0]), float(result[1]) / 1000000


def get_test_name(onnx_file):
    '''
    Return test name according to onnx_file name
    e.g. return resnet20_cifar10 for resnet20_cifar10_pre.onnx

    Args:
        onnx_file(str): path of the onnx file

    Returns:
        test(str): name of the test
    '''
    for idx in REQUIRED_MEMORY:
        idx_start = onnx_file.find(idx)
        if idx_start != -1:
            idx_end = idx_start + len(idx)
            onnx_name_size = len(onnx_file)
            if idx_end == onnx_name_size:
                return idx
            elif idx_end < onnx_name_size:
                if onnx_file[idx_end] == '_':
                    return idx
    test, _ = os.path.splitext(os.path.basename(onnx_file))
    return test


def get_exec_mem(onnx_file):
    '''
    Return memory requirement for the test in Gigabytes

    Args:
        onnx_file(str): path of the onnx file

    Returns:
        mem(int): memory required in Gigabytes to run the test
    '''
    idx = get_test_name(onnx_file)
    if idx is not None:
        return REQUIRED_MEMORY[idx]
    # Unknown one, allow to run anyway
    return 0


def get_ace_cmplr(cmplr_dir):
    '''
    Find out the executable of ACE compiler

    Args:
        cmplr_dir(str): path of the dir that the ACE compiler is built or installed

    Returns:
        cmplr(str): path of the 'fhe_cmplr' executable
    '''
    for root, _, files in os.walk(cmplr_dir):
        for f in files:
            if f == 'fhe_cmplr':
                return os.path.abspath(os.path.join(root, f))
    return None


def get_cifar_option(test, cifar10_dir, cifar100_dir):
    '''
    Return the cifar data file required to run the test

    Args:
        test(str): name of the test, e.g. resnet20_cifar10
        cifar10_dir(str): path of the dir that holds the cifar10 data file
        cifar100_dir(str): path of the dir that holds the cifar100 data file

    Returns:
        res(str): path of the cifar data file
    '''
    res = None
    if test.find('cifar100') != -1:
        if cifar100_dir is None:
            print('-cf100 option is not specified for test:', test)
            sys.exit(-1)
        res = os.path.join(os.path.abspath(cifar100_dir), 'test.bin')
        if not os.path.exists(res):
            print(res, 'does not exist')
            sys.exit(-1)
    elif test.find('cifar10') != -1:
        if cifar10_dir is None:
            print('-cf100 option is not specified for test:', test)
            sys.exit(-1)
        res = os.path.join(os.path.abspath(cifar10_dir), 'test_batch.bin')
        if not os.path.exists(res):
            print(res, 'does not exist')
            sys.exit(-1)
    return res


def check_required_memory(rq_mem):
    '''
    Check if the system has enough memory to run performance tests

    Args:
        rq_mem(int): memory required in Gigabytes

    Returns:
        pass(bool): if memory is enough
    '''
    mem = psutil.virtual_memory()
    total_mem = mem.total / 1000000
    if total_mem < rq_mem:
        return False
    return True


def calculate_process_num(image_num, test_set, log):
    '''
    Calculate parallel process number

    Args:
        image_num(int): number of images to test
        test_set(list[str]): tests to run
        log(file): log file

    Returns:
        (num_cpus, num_process)(int, int): number of cpu required and process to run in parallel
    '''
    num_cpus = 1
    num_process = 1
    all_cpus = psutil.cpu_count(logical=False)
    if all_cpus is None:
        info = 'CPU info unavailable, proceed with 1 CPU and 1 Process\n'
        write_log(info, log)
    else:
        num_batch = math.ceil(image_num / all_cpus)
        num_cpus = int(image_num / num_batch)
        if (image_num % num_batch) != 0:
            num_cpus += 1
        num_process = int(all_cpus / num_cpus)
        info = 'Available CPU: %d, Required CPU: %d, Process: %d\n' % (
            all_cpus, num_cpus, num_process)
        write_log(info, log)
        # Adjust according to tests and memory requirements
        mlist = []
        for exec_file in test_set:
            test = get_test_name(exec_file)
            rq_mem = get_exec_mem(exec_file)
            # If single test exceed memory available
            if not check_required_memory(rq_mem):
                print('Not enough memory, %s requires %s GB' % (test, rq_mem))
                sys.exit(-1)
            # Memory allowed for parallelism
            mlist.append(rq_mem)
            if len(mlist) > num_process:
                mlist.remove(min(mlist))
            while len(mlist) > 0 and not check_required_memory(sum(mlist)):
                mlist.remove(min(mlist))
        if len(mlist) != num_process:
            num_process = len(mlist)
            info = 'Adjust to Process: %d, due to task number and memory required\n' % num_process
            write_log(info, log)
    return num_cpus, num_process


def run_cmd(cmd):
    '''
    Function used in multiprocessing

    Args:
        cmd(list[str]): command line to run

    Returns:
        ret(CompletedProcess[bytes]): execution results of the command
    '''
    ret = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return ret


def get_test_onnx_files(onnx_file, model_dir, paper):
    '''
    Collect onnx model files that are to run in the test

    Args:
        onnx_file(str): path of the onnx file
        model_dir(str): path of the dir that holds onnx files to test
        paper(str): conference paper configuration

    Returns:
        ret(list[str]): onnx files to run
    '''
    ret = []
    #import pdb; pdb.set_trace()
    if onnx_file is not None:
        onnx_file = os.path.abspath(onnx_file)
        if not os.path.exists(onnx_file):
            print(onnx_file, 'does not exist')
            sys.exit(-1)
        ret.append(onnx_file)
    else:
        model_dir = os.path.abspath(model_dir)
        model_files = [f for f in os.listdir(
            model_dir) if os.path.isfile(os.path.join(model_dir, f)) and f.endswith('.onnx')]
        model_files.sort()
        if paper is not None:
            test_set = CGO25_MODEL
            if paper == 'asplos25':
                test_set = ASPLOS25_MODEL
            elif paper == 'oopsla25':
                test_set = OOPSLA25_MODEL
            for test in test_set:
                found = False
                for model_file in model_files:
                    if get_test_name(model_file) == test:
                        ret.append(os.path.join(model_dir, model_file))
                        found = True
                        break
                if not found:
                    print('Pre-train onnx model for %s is missing under %s' % (test, model_dir))
                    sys.exit(-1)
        else:
            for model_file in model_files:
                ret.append(os.path.join(model_dir, model_file))
    return ret


def check_compile_env(build_dir, cmplr_dir, src_dir):
    '''
    Check ACE compilation environment

    Args:
        build_dir(str): path of the build dir of the ACE compiler
        cmplr_dir(str): path of the install dir of the ACE compiler
        src_dir(str): path that holds air-infra, nn-addon & fhe-cmplr

    Returns:
        (cmplr_dir, installed)(str, bool): path of the ACE compiler & if it is an installed one
    '''
    # check ACE compiler
    installed = False
    if build_dir is None and cmplr_dir is None:
        cmplr_dir = './build'
    elif cmplr_dir is None:
        cmplr_dir = build_dir
    else:
        installed = True
    cmplr_dir = os.path.abspath(cmplr_dir)
    if not os.path.exists(cmplr_dir):
        print(cmplr_dir, 'does not exist')
        sys.exit(-1)
    ace_cmplr = get_ace_cmplr(cmplr_dir)
    if ace_cmplr is None:
        print('Failed to locate fhe_cmplr under %s, please build/install ACE compiler first' % cmplr_dir)
        sys.exit(-1)
    # check repo source
    src_path = os.path.abspath(os.path.join(src_dir, 'fhe-cmplr'))
    if not os.path.exists(src_path):
        print(src_path, 'does not exist')
        sys.exit(-1)
    return cmplr_dir, installed


def get_ace_option(test, paper, lib, extra, acc, trace):
    '''
    Return compilation options for ACE compiler according to tests selected

    Args:
        test(str): name of the test
        paper(str): conference paper configuration
        lib(str): target library to link
        extra(list[str]): extra ACE compilation options
        acc(bool): compile for accuracy test
        trace(bool): if ACE trace is enabled

    Returns:
        res(list[str]): ACE compilation options
    '''
    res = []
    config = DEFAULT_CONFIG
    # Basic ACE compilation options
    if paper == 'cgo25':
        config = CGO25_CONFIG
    elif paper == 'asplos25':
        config = ASPLOS25_CONFIG
    elif paper == 'oopsla25':
        config = OOPSLA25_CONFIG
    if acc:
        res.extend(config['accuracy'])
    else:
        res.extend(config['perf'])
    # Relu accuracy tuning options
    if test in config['test']:
        relu_opt = config.get(test + '_relu')
        if relu_opt is not None:
            res.extend(relu_opt)
    # Target library option
    res.append('-P2C:lib=' + lib)
    # Trace option
    if trace:
        res.extend(['-O2A:ts', '-FHE_SCHEME:ts', '-VEC:ts:rtt', '-SIHE:ts:rtt'])
        res.extend(['-CKKS:ts:rtt', '-POLY:ts:rtt', '-P2C:ts'])
    # Extra ACE compilation options from command line
    if extra is not None:
        res.extend(extra)
    return res


def get_onnx_inc_file(test, src_dir):
    '''
    Return the onnx.inc file to be replaced with ACE generated onnx.c

    Args:
        test(str): name of the test
        src_dir(str): path that holds air-infra, nn-addon & fhe-cmplr

    Returns:
        file(str): corresponding onnx.c file for the input test
    '''
    inc_file = test + '_pre.onnx.inc'
    src_path = os.path.abspath(os.path.join(src_dir, 'fhe-cmplr'))
    for root, _, files in os.walk(src_path):
        for f in files:
            if inc_file == f:
                return os.path.abspath(os.path.join(root, f))
    return None


def get_cpp_option(cmplr_dir, installed, src_dir, omp):
    '''
    Return compile & link c++ options to generate executable

    Args:
        cmplr_dir(str): path of the dir that the ACE compiler is built or installed
        installed(bool): if the ACE compiler is installed or located in build dir
        src_dir(str): path that holds air-infra, nn-addon & fhe-cmplr
        omp(bool): if OpenMP is enabled

    Returns:
        res(list[str]): c++ options
    '''
    res = []
    ace_lib_to_link = ['libAIRutil.a', 'libFHErt_ant.a', 'libFHErt_common.a']
    res.append('-DRTLIB_SUPPORT_LINUX')
    res.extend(['-I', os.path.join(cmplr_dir, 'include')])
    # append include options
    if installed:
        res.extend(['-I', os.path.join(cmplr_dir, 'rtlib/include/')])
        res.extend(['-I', os.path.join(cmplr_dir, 'rtlib/include/ant/')])
    else:
        src_dir = os.path.abspath(src_dir)
        res.extend(['-I', os.path.join(cmplr_dir, 'rtlib/build/_deps/uthash-src/src/')])
        res.extend(['-I', os.path.join(src_dir, 'air-infra/include/')])
        res.extend(['-I', os.path.join(src_dir, 'fhe-cmplr/include/')])
        res.extend(['-I', os.path.join(src_dir, 'nn-addon/include/')])
        res.extend(['-I', os.path.join(src_dir, 'fhe-cmplr/rtlib/ant/include/')])
        res.extend(['-I', os.path.join(src_dir, 'fhe-cmplr/rtlib/include/rt_ant/')])
        res.extend(['-I', os.path.join(src_dir, 'fhe-cmplr/rtlib/include/')])
    # append link options
    res.extend(['-O3', '-DNDEBUG', '-std=gnu++17'])
    if omp:
        res.append('-fopenmp')
    for root, _, files in os.walk(cmplr_dir):
        for f in files:
            if f in ace_lib_to_link:
                res.append(os.path.join(root, f))
    return res
