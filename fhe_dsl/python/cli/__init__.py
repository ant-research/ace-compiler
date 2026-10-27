# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
ACE command-line tools.

Usage:
    ace_tool relu-profile --model resnet20
    ace_tool dump-sample --num 5
    ace_tool train-resnet --model 20 --epochs 200
    ace_tool bench latency --model resnet20 --num-images 10
    ace_tool kms gen-enc-keys --ckks N=65536,q0=60,sf=56 --output ./keys/global/
    ace_tool kms gen-eval-keys --compiled-dir ./compiled/resnet20/ --output ./keys/resnet20-v1/
    ace_tool kms list
    ace_tool kms info --key-id resnet20-v1
    python -m ace.cli relu-profile --model resnet20
"""

from .main import main


__all__ = ["main"]
