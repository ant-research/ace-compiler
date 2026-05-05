# test_cases/models/ops.py
"""
Basic operation test cases (add, mul, relu, etc.)

Note: Model definitions are imported from ace.samples.ops
"""
import torch

from ..base import ModelTestCase
from ace.samples.ops import (
    AddOp,
    MulOp,
    ReluOp,
    SigmoidOp,
    TanhOp,
    FlattenOp,
)


# ============================================================================
# Constant Operation Models (defined locally)
# ============================================================================

class AddConstModel(torch.nn.Module):
    """Model that adds a constant tensor."""
    def forward(self, x):
        return x + torch.ones(1, 1, 2, 2)


class MultConstModel(torch.nn.Module):
    """Model that multiplies by a constant tensor."""
    def forward(self, x):
        return x * torch.ones(1, 1, 2, 2)


# ============================================================================
# Test Cases
# ============================================================================

MODEL_OPS_TEST_CASES = [
    # Add operations
    ModelTestCase(
        name="add_model",
        model_class=AddOp,
        example_inputs=(torch.randn(1, 1, 3, 3), torch.randn(1, 1, 3, 3)),
        encrypt_inputs=["x", "y"],
        expected_ops=["Add"]
    ),
    ModelTestCase(
        name="add_1_dimension",
        model_class=AddOp,
        example_inputs=(torch.randn(1, 1, 1, 3), torch.randn(1, 1, 1, 3)),
        encrypt_inputs=["x", "y"],
        expected_ops=["Add"]
    ),
    ModelTestCase(
        name="add_const",
        model_class=AddConstModel,
        example_inputs=(torch.randn(1, 1, 2, 2),),
        encrypt_inputs=["x"],
        expected_ops=["Add"]
    ),

    # Multiply operations
    ModelTestCase(
        name="mult_model",
        model_class=MulOp,
        example_inputs=(torch.randn(1, 1, 3, 3), torch.randn(1, 1, 3, 3)),
        encrypt_inputs=["x", "y"],
        expected_ops=["Mul"]
    ),
    ModelTestCase(
        name="mult_const",
        model_class=MultConstModel,
        example_inputs=(torch.randn(1, 1, 2, 2),),
        encrypt_inputs=["x"],
        expected_ops=["Mul", "Constant"]
    ),

    # Activation functions
    ModelTestCase(
        name="relu",
        model_class=ReluOp,
        example_inputs=(torch.randn(1, 1, 3, 3),),
        encrypt_inputs=["x"],
        expected_ops=["Relu"]
    ),

    # Flatten
    ModelTestCase(
        name="flatten",
        model_class=FlattenOp,
        example_inputs=(torch.randn(1, 1, 3, 3),),
        encrypt_inputs=["x"],
        expected_ops=["Flatten"]
    ),
]