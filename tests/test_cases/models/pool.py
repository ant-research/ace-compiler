# test_cases/models/pool.py
"""
Pooling operation test cases.

Note: Model definitions are imported from ace.samples.ops.pool
"""
import torch

from ..base import ModelTestCase
from ace.samples.ops import (
    AvgPool2dOp,
    MaxPool2dOp,
    GlobalAvgPool2dOp,
    GlobalMaxPool2dOp,
    AvgPoolConv2dOp,
    Conv2dAvgPool2dOp,
    ReluAvgPoolOp,
    AvgPoolFlattenOp,
)


# ============================================================================
# Test Cases
# ============================================================================

MODEL_POOL_TEST_CASES = [
    # Average pooling
    ModelTestCase(
        name="avg_pool_2d",
        model_class=AvgPool2dOp,
        example_inputs=(torch.randn(1, 1, 4, 4),),
        encrypt_inputs=["x"],
        expected_ops=["AveragePool"]
    ),
    ModelTestCase(
        name="avg_pool_2d_with_stride",
        model_class=AvgPool2dOp,
        example_inputs=(torch.randn(1, 1, 8, 8),),
        encrypt_inputs=["x"],
        model_init_args=(2, 2, False),
        expected_ops=["AveragePool"]
    ),

    # Max pooling
    # ModelTestCase(
    #     name="max_pool_2d",
    #     model_class=MaxPool2dOp,
    #     example_inputs=(torch.randn(1, 1, 4, 4),),
    #     encrypt_inputs=["x"],
    #     expected_ops=["MaxPool"]
    # ),

    # Global pooling
    ModelTestCase(
        name="global_avg_pool",
        model_class=GlobalAvgPool2dOp,
        example_inputs=(torch.randn(1, 3, 8, 8),),
        encrypt_inputs=["x"],
        expected_ops=["GlobalAveragePool"]
    ),

    # Combined operations
    # TODO: Skip avg_pool_conv2d due to runtime accuracy issue (mul_depth=8)
    # ModelTestCase(
    #     name="avg_pool_conv2d",
    #     model_class=AvgPoolConv2dOp,
    #     example_inputs=(torch.randn(1, 3, 16, 16),),
    #     encrypt_inputs=["x"],
    #     expected_ops=["AveragePool", "Conv"]
    # ),
    # ModelTestCase(
    #     name="conv2d_avg_pool",
    #     model_class=Conv2dAvgPool2dOp,
    #     example_inputs=(torch.randn(1, 3, 16, 16),),
    #     encrypt_inputs=["x"],
    #     expected_ops=["Conv", "AveragePool"]
    # ),

    ModelTestCase(
        name="relu_avg_pool",
        model_class=ReluAvgPoolOp,
        example_inputs=(torch.randn(1, 3, 4, 4),),
        encrypt_inputs=["x"],
        expected_ops=["Relu", "AveragePool"]
    ),
    ModelTestCase(
        name="avg_pool_flatten",
        model_class=AvgPoolFlattenOp,
        example_inputs=(torch.randn(1, 3, 4, 4),),
        encrypt_inputs=["x"],
        expected_ops=["AveragePool", "Flatten"]
    ),
]