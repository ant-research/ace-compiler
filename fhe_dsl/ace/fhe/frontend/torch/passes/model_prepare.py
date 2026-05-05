#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Model Prepare Pass for Torch Frontend.

This pass prepares the PyTorch model for FHE compilation by:
1. Setting model to eval mode
2. Fusing BatchNorm layers into Conv/Linear layers

Design Document: Model Preparation Strategy
============================================

Problem:
--------
BatchNorm layers introduce non-linear operations that are expensive in FHE.
During inference (eval mode), BatchNorm can be folded into the preceding
Conv or Linear layer by adjusting weights and biases.

Solution:
---------
1. Set model to eval mode (disables dropout, fixes BN statistics)
2. Find Conv-BN and Linear-BN pairs
3. Fold BN parameters into the preceding layer
4. Replace BN module with nn.Identity

Usage:
    from .model_prepare import ModelPreparePass

    pass_instance = ModelPreparePass()
    prepared_model = pass_instance.apply(model)
"""

import torch
import torch.nn as nn
import copy
from typing import Tuple, Optional


class ModelPreparePass:
    """
    Model preparation pass for FHE compilation.

    Applies transformations to make the model FHE-friendly:
    - Eval mode (fixes BatchNorm statistics)
    - BatchNorm folding (reduces operation count)
    """

    def __init__(self, inplace: bool = False):
        """
        Initialize the model prepare pass.

        Args:
            inplace: If True, modify the model in-place. If False, return a copy.
        """
        self.inplace = inplace

    def apply(self, model: nn.Module) -> nn.Module:
        """
        Apply model preparation transformations.

        Args:
            model: PyTorch model to prepare

        Returns:
            Prepared model with BatchNorm layers folded
        """
        if not self.inplace:
            model = copy.deepcopy(model)

        # Set to eval mode
        model.eval()

        # Fuse BatchNorm layers
        self._fuse_batchnorm(model)

        return model

    def _fuse_batchnorm(self, model: nn.Module) -> nn.Module:
        """
        Fuse BatchNorm layers into preceding Conv/Linear layers.

        Iterates through all named modules and finds Conv-BN and Linear-BN pairs.

        Args:
            model: PyTorch model

        Returns:
            Model with BatchNorm layers fused
        """
        # Get all named modules
        named_modules = list(model.named_modules())

        # Find Conv-BN and Linear-BN pairs
        for i, (name, module) in enumerate(named_modules):
            # Check if this is a Conv or Linear layer
            if isinstance(module, (nn.Conv2d, nn.Linear)):
                # Look for following BatchNorm
                if i + 1 < len(named_modules):
                    next_name, next_module = named_modules[i + 1]
                    if isinstance(next_module, nn.BatchNorm2d):
                        # Check if they share the same parent
                        parent_name = ".".join(name.split(".")[:-1])
                        next_parent_name = ".".join(next_name.split(".")[:-1])

                        if parent_name == next_parent_name:
                            # Same parent, can fuse
                            self._fuse_conv_bn(module, next_module)

                            # Replace BN with Identity
                            parent_module = model.get_submodule(parent_name) if parent_name else model
                            bn_name = next_name.split(".")[-1]
                            setattr(parent_module, bn_name, nn.Identity())

        return model

    def _fuse_conv_bn(self, conv: nn.Conv2d, bn: nn.BatchNorm2d) -> Tuple[torch.Tensor, Optional[torch.Tensor]]:
        """
        Fuse Conv2d + BatchNorm2d into a single Conv2d.

        Math:
            BN: y = (x - mean) / sqrt(var + eps) * scale + bias
            Conv: x = w * input + bias

            Fused: y = (w * input + bias - mean) / sqrt(var + eps) * scale + bias
                 = (w / sqrt(var + eps) * scale) * input + ...

        Args:
            conv: Conv2d layer
            bn: BatchNorm2d layer following conv

        Returns:
            Tuple of (new_weight, new_bias)
        """
        # Get BN statistics
        bn_weight = bn.weight
        bn_bias = bn.bias
        bn_mean = bn.running_mean
        bn_var = bn.running_var
        bn_eps = bn.eps

        # Calculate scaling factor
        scale = bn_weight / torch.sqrt(bn_var + bn_eps)

        # Fold into Conv weight
        conv_weight = conv.weight * scale.view(-1, 1, 1, 1)
        conv.weight.data = conv_weight

        # Fold into Conv bias
        if conv.bias is not None:
            conv_bias = (conv.bias - bn_mean) * scale + bn_bias
            conv.bias.data = conv_bias
        else:
            conv_bias = (0 - bn_mean) * scale + bn_bias
            conv.register_parameter('bias', nn.Parameter(conv_bias))

        return conv_weight, conv_bias

    def _fuse_linear_bn(self, linear: nn.Linear, bn: nn.BatchNorm1d) -> Tuple[torch.Tensor, Optional[torch.Tensor]]:
        """
        Fuse Linear + BatchNorm1d into a single Linear.

        Args:
            linear: Linear layer
            bn: BatchNorm1d layer following linear

        Returns:
            Tuple of (new_weight, new_bias)
        """
        # Get BN statistics
        bn_weight = bn.weight
        bn_bias = bn.bias
        bn_mean = bn.running_mean
        bn_var = bn.running_var
        bn_eps = bn.eps

        # Calculate scaling factor
        scale = bn_weight / torch.sqrt(bn_var + bn_eps)

        # Fold into Linear weight
        linear_weight = linear.weight * scale.view(-1, 1)
        linear.weight.data = linear_weight

        # Fold into Linear bias
        if linear.bias is not None:
            linear_bias = (linear.bias - bn_mean) * scale + bn_bias
            linear.bias.data = linear_bias
        else:
            linear_bias = (0 - bn_mean) * scale + bn_bias
            linear.register_parameter('bias', nn.Parameter(linear_bias))

        return linear_weight, linear_bias


# Convenience function for direct usage
def prepare_model_for_fhe(model: nn.Module, inplace: bool = False) -> nn.Module:
    """
    Prepare a PyTorch model for FHE compilation.

    Convenience function that applies ModelPreparePass.

    Args:
        model: PyTorch model to prepare
        inplace: If True, modify in-place

    Returns:
        Prepared model
    """
    pass_instance = ModelPreparePass(inplace=inplace)
    return pass_instance.apply(model)