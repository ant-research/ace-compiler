#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet FHE compile configurations.

These options are model-specific (tuned for ResNet depth) but library-agnostic.
The p2c.lib field is injected by the library handler at compile time,
not stored here.
"""

RESNET20_COMPILE_OPTIONS = {
    "vec": {"conv_parl": True, "ssf": True},
    "sihe": {
        "relu_vr_def": 3,
        "relu_vr": (
            "/relu/Relu=4;"
            "/layer1/layer1.0/relu_1/Relu=4;/layer1/layer1.1/relu/Relu=4;"
            "/layer1/layer1.1/relu_1/Relu=5;/layer1/layer1.2/relu_1/Relu=5;"
            "/layer2/layer2.0/relu_1/Relu=5;/layer2/layer2.1/relu_1/Relu=5;"
            "/layer2/layer2.2/relu_1/Relu=7;/layer3/layer3.0/relu_1/Relu=4;"
            "/layer3/layer3.1/relu_1/Relu=6;/layer3/layer3.2/relu/Relu=4;"
            "/layer3/layer3.2/relu_1/Relu=20"
        ),
    },
    "ckks": {"q0": 60, "sf": 56, "N": 65536},
}

RESNET32_COMPILE_OPTIONS = {
    "vec": {"conv_parl": True, "ssf": True},
    "sihe": {
        "relu_vr_def": 2,
        "relu_vr": (
            "/relu/Relu=4;"
            "/layer1/layer1.0/relu/Relu=3;/layer1/layer1.0/relu_1/Relu=5;"
            "/layer1/layer1.1/relu/Relu=3;/layer1/layer1.1/relu_1/Relu=5;"
            "/layer1/layer1.2/relu/Relu=3;/layer1/layer1.2/relu_1/Relu=5;"
            "/layer1/layer1.3/relu/Relu=3;/layer1/layer1.3/relu_1/Relu=5;"
            "/layer1/layer1.4/relu/Relu=3;/layer1/layer1.4/relu_1/Relu=5;"
            "/layer2/layer2.0/relu/Relu=3;/layer2/layer2.0/relu_1/Relu=5;"
            "/layer2/layer2.1/relu_1/Relu=5;/layer2/layer2.2/relu_1/Relu=5;"
            "/layer2/layer2.3/relu_1/Relu=6;/layer2/layer2.4/relu_1/Relu=6;"
            "/layer3/layer3.0/relu/Relu=3;/layer3/layer3.0/relu_1/Relu=5;"
            "/layer3/layer3.1/relu_1/Relu=4;/layer3/layer3.2/relu/Relu=3;"
            "/layer3/layer3.2/relu_1/Relu=6;/layer3/layer3.3/relu/Relu=4;"
            "/layer3/layer3.3/relu_1/Relu=10;/layer3/layer3.4/relu_1/Relu=11"
        ),
    },
    "ckks": {"q0": 60, "sf": 56, "N": 65536},
}

RESNET32_CIFAR100_COMPILE_OPTIONS = {
    "vec": {"conv_parl": True, "ssf": True},
    "sihe": {
        "relu_vr_def": 3,
        "relu_vr": (
            "/relu/Relu=5;"
            "/layer1/layer1.0/relu_1/Relu=6;/layer1/layer1.1/relu_1/Relu=7;"
            "/layer1/layer1.2/relu_1/Relu=8;/layer1/layer1.3/relu_1/Relu=10;"
            "/layer1/layer1.4/relu/Relu=4;/layer1/layer1.4/relu_1/Relu=7;"
            "/layer2/layer2.0/relu/Relu=4;/layer2/layer2.0/relu_1/Relu=6;"
            "/layer2/layer2.1/relu_1/Relu=8;/layer2/layer2.2/relu/Relu=4;"
            "/layer2/layer2.2/relu_1/Relu=8;/layer2/layer2.3/relu_1/Relu=9;"
            "/layer2/layer2.4/relu_1/Relu=11;"
            "/layer3/layer3.0/relu/Relu=4;/layer3/layer3.0/relu_1/Relu=8;"
            "/layer3/layer3.1/relu_1/Relu=9;/layer3/layer3.2/relu/Relu=4;"
            "/layer3/layer3.2/relu_1/Relu=11;/layer3/layer3.3/relu/Relu=4;"
            "/layer3/layer3.3/relu_1/Relu=26;/layer3/layer3.4/relu/Relu=5;"
            "/layer3/layer3.4/relu_1/Relu=46"
        ),
    },
    "ckks": {"q0": 60, "sf": 56, "N": 65536},
}
RESNET44_COMPILE_OPTIONS = {
    "vec": {"conv_parl": True, "ssf": True},
    "sihe": {
        "relu_vr_def": 2,
        "relu_vr": (
            "/relu/Relu=4;"
            "/layer1/layer1.0/relu_1/Relu=5;/layer1/layer1.1/relu_1/Relu=5;"
            "/layer1/layer1.2/relu_1/Relu=5;/layer1/layer1.3/relu_1/Relu=6;"
            "/layer1/layer1.4/relu_1/Relu=6;/layer1/layer1.5/relu/Relu=2;"
            "/layer1/layer1.5/relu_1/Relu=7;/layer1/layer1.6/relu_1/Relu=6;"
            "/layer2/layer2.0/relu/Relu=2;/layer2/layer2.0/relu_1/Relu=5;"
            "/layer2/layer2.1/relu/Relu=2;/layer2/layer2.1/relu_1/Relu=5;"
            "/layer2/layer2.2/relu/Relu=2;/layer2/layer2.2/relu_1/Relu=5;"
            "/layer2/layer2.3/relu/Relu=2;/layer2/layer2.3/relu_1/Relu=5;"
            "/layer2/layer2.4/relu/Relu=2;/layer2/layer2.4/relu_1/Relu=5;"
            "/layer2/layer2.5/relu/Relu=2;/layer2/layer2.5/relu_1/Relu=6;"
            "/layer2/layer2.6/relu/Relu=2;/layer2/layer2.6/relu_1/Relu=7;"
            "/layer3/layer3.0/relu_1/Relu=5;/layer3/layer3.1/relu/Relu=2;"
            "/layer3/layer3.1/relu_1/Relu=5;/layer3/layer3.2/relu/Relu=2;"
            "/layer3/layer3.2/relu_1/Relu=6;/layer3/layer3.3/relu_1/Relu=7;"
            "/layer3/layer3.4/relu_1/Relu=9;/layer3/layer3.5/relu_1/Relu=15;"
            "/layer3/layer3.6/relu_1/Relu=16"
        ),
    },
    "ckks": {"q0": 60, "sf": 56, "N": 65536},
}
RESNET56_COMPILE_OPTIONS = {
    "vec": {"conv_parl": True, "ssf": True},
    "sihe": {
        "relu_vr_def": 2,
        "relu_vr": (
            "/relu/Relu=4;"
            "/layer1/layer1.0/relu_1/Relu=6;/layer1/layer1.1/relu_1/Relu=5;"
            "/layer1/layer1.2/relu/Relu=3;/layer1/layer1.2/relu_1/Relu=6;"
            "/layer1/layer1.3/relu/Relu=3;/layer1/layer1.3/relu_1/Relu=7;"
            "/layer1/layer1.4/relu_1/Relu=6;/layer1/layer1.5/relu_1/Relu=6;"
            "/layer1/layer1.6/relu_1/Relu=6;/layer1/layer1.7/relu_1/Relu=6;"
            "/layer1/layer1.8/relu_1/Relu=5;"
            "/layer2/layer2.0/relu_1/Relu=4;/layer2/layer2.1/relu_1/Relu=4;"
            "/layer2/layer2.2/relu_1/Relu=5;/layer2/layer2.3/relu_1/Relu=5;"
            "/layer2/layer2.4/relu_1/Relu=6;/layer2/layer2.5/relu_1/Relu=8;"
            "/layer2/layer2.6/relu_1/Relu=11;/layer2/layer2.7/relu_1/Relu=11;"
            "/layer2/layer2.8/relu_1/Relu=12;"
            "/layer3/layer3.0/relu/Relu=3;/layer3/layer3.0/relu_1/Relu=5;"
            "/layer3/layer3.1/relu_1/Relu=5;/layer3/layer3.2/relu_1/Relu=5;"
            "/layer3/layer3.3/relu_1/Relu=5;/layer3/layer3.4/relu_1/Relu=5;"
            "/layer3/layer3.5/relu_1/Relu=6;/layer3/layer3.6/relu_1/Relu=8;"
            "/layer3/layer3.7/relu/Relu=3;/layer3/layer3.7/relu_1/Relu=10;"
            "/layer3/layer3.8/relu_1/Relu=12"
        ),
    },
    "ckks": {"q0": 60, "sf": 56, "N": 65536},
}

RESNET110_COMPILE_OPTIONS = {
    "vec": {"conv_parl": True, "ssf": True},
    "sihe": {
        "relu_vr_def": 3,
        "relu_vr": (
            "/relu/Relu=14;"
            "/layer1/layer1.0/relu/Relu=4;/layer1/layer1.0/relu_1/Relu=14;"
            "/layer1/layer1.1/relu/Relu=4;/layer1/layer1.1/relu_1/Relu=15;"
            "/layer1/layer1.2/relu/Relu=3;/layer1/layer1.2/relu_1/Relu=15;"
            "/layer1/layer1.3/relu/Relu=6;/layer1/layer1.3/relu_1/Relu=20;"
            "/layer1/layer1.4/relu/Relu=5;/layer1/layer1.4/relu_1/Relu=17;"
            "/layer1/layer1.5/relu/Relu=5;/layer1/layer1.5/relu_1/Relu=17;"
            "/layer1/layer1.6/relu/Relu=5;/layer1/layer1.6/relu_1/Relu=17;"
            "/layer1/layer1.7/relu/Relu=4;/layer1/layer1.7/relu_1/Relu=17;"
            "/layer1/layer1.8/relu/Relu=4;/layer1/layer1.8/relu_1/Relu=17;"
            "/layer1/layer1.9/relu/Relu=5;/layer1/layer1.9/relu_1/Relu=18;"
            "/layer1/layer1.10/relu/Relu=5;/layer1/layer1.10/relu_1/Relu=18;"
            "/layer1/layer1.11/relu/Relu=5;/layer1/layer1.11/relu_1/Relu=17;"
            "/layer1/layer1.12/relu/Relu=4;/layer1/layer1.12/relu_1/Relu=21;"
            "/layer1/layer1.13/relu/Relu=7;/layer1/layer1.13/relu_1/Relu=22;"
            "/layer1/layer1.14/relu/Relu=7;/layer1/layer1.14/relu_1/Relu=23;"
            "/layer1/layer1.15/relu/Relu=7;/layer1/layer1.15/relu_1/Relu=25;"
            "/layer1/layer1.16/relu/Relu=6;/layer1/layer1.16/relu_1/Relu=22;"
            "/layer1/layer1.17/relu/Relu=6;/layer1/layer1.17/relu_1/Relu=22;"
            "/layer2/layer2.0/relu/Relu=6;/layer2/layer2.0/relu_1/Relu=14;"
            "/layer2/layer2.1/relu/Relu=4;/layer2/layer2.1/relu_1/Relu=14;"
            "/layer2/layer2.2/relu/Relu=3;/layer2/layer2.2/relu_1/Relu=15;"
            "/layer2/layer2.3/relu/Relu=4;/layer2/layer2.3/relu_1/Relu=16;"
            "/layer2/layer2.4/relu/Relu=4;/layer2/layer2.4/relu_1/Relu=16;"
            "/layer2/layer2.5/relu/Relu=3;/layer2/layer2.5/relu_1/Relu=17;"
            "/layer2/layer2.6/relu/Relu=5;/layer2/layer2.6/relu_1/Relu=17;"
            "/layer2/layer2.7/relu/Relu=3;/layer2/layer2.7/relu_1/Relu=17;"
            "/layer2/layer2.8/relu/Relu=3;/layer2/layer2.8/relu_1/Relu=18;"
            "/layer2/layer2.9/relu/Relu=4;/layer2/layer2.9/relu_1/Relu=19;"
            "/layer2/layer2.10/relu/Relu=3;/layer2/layer2.10/relu_1/Relu=19;"
            "/layer2/layer2.11/relu/Relu=3;/layer2/layer2.11/relu_1/Relu=19;"
            "/layer2/layer2.12/relu/Relu=3;/layer2/layer2.12/relu_1/Relu=22;"
            "/layer2/layer2.13/relu/Relu=3;/layer2/layer2.13/relu_1/Relu=21;"
            "/layer2/layer2.14/relu/Relu=3;/layer2/layer2.14/relu_1/Relu=23;"
            "/layer2/layer2.15/relu/Relu=3;/layer2/layer2.15/relu_1/Relu=22;"
            "/layer2/layer2.16/relu/Relu=4;/layer2/layer2.16/relu_1/Relu=23;"
            "/layer2/layer2.17/relu/Relu=3;/layer2/layer2.17/relu_1/Relu=22;"
            "/layer3/layer3.0/relu/Relu=5;/layer3/layer3.0/relu_1/Relu=14;"
            "/layer3/layer3.1/relu/Relu=4;/layer3/layer3.1/relu_1/Relu=15;"
            "/layer3/layer3.2/relu/Relu=3;/layer3/layer3.2/relu_1/Relu=15;"
            "/layer3/layer3.3/relu/Relu=4;/layer3/layer3.3/relu_1/Relu=15;"
            "/layer3/layer3.4/relu/Relu=3;/layer3/layer3.4/relu_1/Relu=15;"
            "/layer3/layer3.5/relu/Relu=3;/layer3/layer3.5/relu_1/Relu=16;"
            "/layer3/layer3.6/relu/Relu=3;/layer3/layer3.6/relu_1/Relu=16;"
            "/layer3/layer3.7/relu/Relu=3;/layer3/layer3.7/relu_1/Relu=16;"
            "/layer3/layer3.8/relu/Relu=3;/layer3/layer3.8/relu_1/Relu=17;"
            "/layer3/layer3.9/relu/Relu=3;/layer3/layer3.9/relu_1/Relu=18;"
            "/layer3/layer3.10/relu/Relu=3;/layer3/layer3.10/relu_1/Relu=20;"
            "/layer3/layer3.11/relu/Relu=4;/layer3/layer3.11/relu_1/Relu=20;"
            "/layer3/layer3.12/relu/Relu=4;/layer3/layer3.12/relu_1/Relu=20;"
            "/layer3/layer3.13/relu/Relu=4;/layer3/layer3.13/relu_1/Relu=24;"
            "/layer3/layer3.14/relu/Relu=4;/layer3/layer3.14/relu_1/Relu=27;"
            "/layer3/layer3.15/relu/Relu=4;/layer3/layer3.15/relu_1/Relu=30;"
            "/layer3/layer3.16/relu/Relu=4;/layer3/layer3.16/relu_1/Relu=27;"
            "/layer3/layer3.17/relu/Relu=9;/layer3/layer3.17/relu_1/Relu=33"
        ),
    },
    "ckks": {"q0": 60, "sf": 56, "N": 65536},
}