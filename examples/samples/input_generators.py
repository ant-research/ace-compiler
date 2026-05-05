#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Input tensor generators for FHE examples.
"""

import torch


def rand_input(*shape: int, dtype: torch.dtype = torch.float32) -> torch.Tensor:
    """Generate random uniform input tensor in [0, 1)."""
    return torch.rand(*shape, dtype=dtype)


def randn_input(*shape: int, dtype: torch.dtype = torch.float32) -> torch.Tensor:
    """Generate random normal input tensor with mean=0, std=1."""
    return torch.randn(*shape, dtype=dtype)


def ones_input(*shape: int, dtype: torch.dtype = torch.float32) -> torch.Tensor:
    """Generate tensor of ones."""
    return torch.ones(*shape, dtype=dtype)


def get_linear_inputs(
    batch_size: int = 1,
    input_size: int = 4,
    dtype: torch.dtype = torch.float32,
) -> list[torch.Tensor]:
    """Generate inputs for linear model examples."""
    x = randn_input(batch_size, input_size, dtype=dtype)
    return [x]


def get_conv_inputs(
    batch_size: int = 1,
    in_channels: int = 3,
    height: int = 8,
    width: int = 8,
    dtype: torch.dtype = torch.float32,
) -> list[torch.Tensor]:
    """Generate inputs for Conv2D model examples."""
    x = randn_input(batch_size, in_channels, height, width, dtype=dtype)
    return [x]


def get_add_inputs(
    batch_size: int = 1,
    shape: tuple = (4,),
    dtype: torch.dtype = torch.float32,
) -> list[torch.Tensor]:
    """Generate two inputs for add function examples."""
    x = randn_input(batch_size, *shape, dtype=dtype)
    y = randn_input(batch_size, *shape, dtype=dtype)
    return [x, y]