# test_cases/models/conv.py
"""
Convolutional neural network test cases.

Note: Model definitions are imported from ace.samples.ops.conv
"""
import torch

from ..base import ModelTestCase
from ace.samples.ops import (
    Conv2dOp,
    Conv2dReluOp,
    Conv2dBnReluOp,
    DepthwiseConv2dOp,
    SeparableConv2dOp,
    ConvTranspose2dOp,
)


# ============================================================================
# Test Cases
# ============================================================================

MODEL_CONV_TEST_CASES = [
    ModelTestCase(
        name="conv2d",
        model_class=Conv2dOp,
        example_inputs=(torch.randn(1, 3, 16, 16),),
        encrypt_inputs=["x"],
        expected_ops=["Conv"]
    ),
    # TODO: Skip conv2d_relu due to runtime ReLU implementation issue
    # ModelTestCase(
    #     name="conv2d_relu",
    #     model_class=Conv2dReluOp,
    #     example_inputs=(torch.randn(1, 3, 16, 16),),
    #     encrypt_inputs=["x"],
    #     expected_ops=["Conv", "Relu"]
    # ),
    # TODO: Skip conv2d_bn_relu due to runtime ReLU implementation issue
    # ModelTestCase(
    #     name="conv2d_bn_relu",
    #     model_class=Conv2dBnReluOp,
    #     example_inputs=(torch.randn(1, 3, 16, 16),),
    #     encrypt_inputs=["x"],
    #     expected_ops=["BatchNorm", "Conv", "Relu"]
    # ),
]