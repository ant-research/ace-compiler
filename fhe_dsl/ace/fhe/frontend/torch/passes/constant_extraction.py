#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Constant Extraction Pass for Torch Frontend.

This pass extracts constant tensors from the FX graph (get_attr nodes)
for use during IR generation.

Design Document: Constant Extraction Strategy
==============================================

Problem:
--------
FX graphs represent weights and biases as get_attr nodes that reference
tensors stored in the traced model. During IR generation, we need to:
1. Extract these constants with their shapes and dtypes
2. Pass them to the IR builder for serialization

Solution:
---------
Scan the FX graph for get_attr nodes and extract tensor metadata:
- Name (for symbol table)
- Shape (for type inference)
- Dtype (for serialization format)
- Data (for constant folding)

Constant Categories:
    - Weights: Conv/Linear layer weights (float32)
    - Biases: Conv/Linear layer biases (float32)
    - Shapes: Reshape/View shape constants (int64)
    - Scales: BN scale factors (float32, after folding)

Usage:
    from .constant_extraction import ConstantExtractionPass

    pass_instance = ConstantExtractionPass()
    constants = pass_instance.apply(traced_model)
"""

import torch
from typing import Dict, Any


class ConstantExtractionPass:
    """
    Constant extraction pass for FHE compilation.

    Extracts constant tensors from FX graph get_attr nodes
    for use during IR generation.
    """

    def __init__(self):
        """Initialize the constant extraction pass."""
        self.constants = {}

    def apply(self, traced_model: torch.fx.GraphModule, original_model=None) -> Dict[str, Dict[str, Any]]:
        """
        Extract constants from FX graph get_attr nodes.

        Args:
            traced_model: FX traced model
            original_model: Original model to extract weights from (optional)

        Returns:
            dict mapping constant names to metadata dicts:
            {
                'tensor': torch.Tensor,
                'shape': List[int],
                'data': List[float|int],
                'dtype': str
            }
        """
        self.constants = {}

        for node in traced_model.graph.nodes:
            if node.op == 'get_attr':
                target = node.target

                try:
                    tensor = getattr(traced_model, target)
                    if isinstance(tensor, torch.Tensor):
                        if tensor.dtype in (torch.int32, torch.int64, torch.long):
                            self.constants[target] = {
                                'tensor': tensor,
                                'shape': list(tensor.shape),
                                'data': tensor.detach().flatten().to(torch.int64).tolist(),
                                'dtype': 'int64'
                            }
                        else:
                            self.constants[target] = {
                                'tensor': tensor,
                                'shape': list(tensor.shape),
                                'data': tensor.detach().flatten().to(torch.float32).tolist(),
                                'dtype': 'float32'
                            }
                except (AttributeError, KeyError):
                    continue

        return self.constants

    def get_constants(self) -> Dict[str, Dict[str, Any]]:
        """
        Get extracted constants.

        Returns:
            Dictionary of extracted constants
        """
        return self.constants

    def get_constant_names(self) -> list:
        """
        Get names of extracted constants.

        Returns:
            List of constant names
        """
        return list(self.constants.keys())


# Convenience function for direct usage
def get_graph_constants(traced_model: torch.fx.GraphModule, original_model=None) -> Dict[str, Dict[str, Any]]:
    """
    Extract constants from FX graph get_attr nodes.

    Convenience function that applies ConstantExtractionPass.

    Args:
        traced_model: FX traced model
        original_model: Original model (optional)

    Returns:
        Dictionary of extracted constants
    """
    pass_instance = ConstantExtractionPass()
    return pass_instance.apply(traced_model, original_model)