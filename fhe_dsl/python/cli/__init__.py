# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
ACE command-line tools.

Usage:
    ace_tool relu-profile --model resnet20
    ace_tool dump-sample --num 5
    ace_tool train-resnet --model 20 --epochs 200
    python -m ace.cli relu-profile --model resnet20
"""

from .main import main

__all__ = ["main"]
