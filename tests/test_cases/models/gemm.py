# test_cases/models/gemm.py
"""
Linear/GEMM (General Matrix Multiply) test cases.

Note: Model definitions are imported from ace.samples.ops.linear
"""
import torch

from ..base import ModelTestCase
from ace.samples.ops import (
    LinearOp as GemmModel,
    LinearReluOp as GemmReluOp,
    ReluLinearOp as ReluGemmOp,
    LinearOp as LinearBiasOp,
    MLP as MultiOpPerceptron,
)


# ============================================================================
# Test Cases
# ============================================================================

MODEL_GEMM_TEST_CASES = [
    # Basic GEMM
    ModelTestCase(
        name="gemm_49x3",
        model_class=GemmModel,
        example_inputs=(torch.flatten(torch.randn(1, 1, 7, 7), 1),),
        encrypt_inputs=["x"],
        model_init_args=(49, 3),
        expected_ops=["Gemm"]
    ),

    # GEMM + ReLU
    # ModelTestCase(
    #     name="gemm_relu",
    #     model_class=GemmReluOp,
    #     example_inputs=(torch.flatten(torch.randn(1, 1, 1, 3), 1),),
    #     encrypt_inputs=["x"],
    #     model_init_args=(3, 2),
    #     expected_ops=["Gemm", "Relu"]
    # ),

    # ReLU + GEMM
    ModelTestCase(
        name="relu_gemm",
        model_class=ReluGemmOp,
        example_inputs=(torch.flatten(torch.randn(1, 1, 1, 3), 1),),
        encrypt_inputs=["x"],
        model_init_args=(3, 2),
        expected_ops=["Gemm", "Relu"]
    ),

    # MLP
    # ModelTestCase(
    #     name="mlp",
    #     model_class=MultiOpPerceptron,
    #     example_inputs=(torch.randn(1, 10),),
    #     encrypt_inputs=["x"],
    #     model_init_args=(10, 20, 5),
    #     expected_ops=["Gemm", "Relu", "Gemm"]
    # ),
]