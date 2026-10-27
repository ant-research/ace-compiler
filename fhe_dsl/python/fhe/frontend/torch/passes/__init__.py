#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Torch Frontend Passes - Model and graph transformation passes.

These passes are applied during the prepare() phase to transform
PyTorch models into a form suitable for FHE compilation.

Pass Categories:
    - Model Prepare: BN folding, eval mode, etc.
    - Graph Transform: Custom op substitution, node removal
    - Constant Extraction: Extract weights and biases from FX graph

Usage:
    from ace.fhe.frontend.torch.passes import (
        ModelPreparePass,
        GraphTransformPass,
        ConstantExtractionPass,
    )

    # Or use convenience functions:
    from ace.fhe.frontend.torch.passes import (
        prepare_model_for_fhe,
        rewrite_graph_to_custom_ops,
        get_graph_constants,
    )
"""

from .constant_extraction import ConstantExtractionPass, get_graph_constants
from .graph_transform import GraphTransformPass, rewrite_graph_to_custom_ops
from .model_prepare import ModelPreparePass, prepare_model_for_fhe


__all__ = [
    # Constant extraction pass
    "ConstantExtractionPass",
    # Graph transform pass
    "GraphTransformPass",
    # Model preparation pass
    "ModelPreparePass",
    "get_graph_constants",
    "prepare_model_for_fhe",
    "rewrite_graph_to_custom_ops",
]
