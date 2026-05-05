# test_cases/models/__init__.py
"""
Model test cases organized by category.

Note: Model definitions are now in:
    - ace.samples.ops.linear: LinearOp, MLP, etc.
    - ace.samples.ops.conv: Conv2dOp, etc.
    - ace.samples.ops.pool: AvgPool2dOp, etc.
    - ace.models.resnet: ResNet_CIFAR

Usage:
    # Import test cases for parametrized tests
    from test_cases import MODEL_TEST_CASES

    # Import model classes for direct use (from new locations)
    from ace.samples.ops import LinearOp, Conv2dOp
"""
from .ops import MODEL_OPS_TEST_CASES
from .conv import MODEL_CONV_TEST_CASES
from .gemm import MODEL_GEMM_TEST_CASES
from .pool import MODEL_POOL_TEST_CASES

# All model test cases combined (ResNet cases are in test_regression/resnet/cases/)
MODEL_TEST_CASES = (
    MODEL_OPS_TEST_CASES
    + MODEL_CONV_TEST_CASES
    + MODEL_GEMM_TEST_CASES
    + MODEL_POOL_TEST_CASES
)

__all__ = [
    # Test cases
    "MODEL_TEST_CASES",
    "MODEL_OPS_TEST_CASES",
    "MODEL_CONV_TEST_CASES",
    "MODEL_GEMM_TEST_CASES",
    "MODEL_POOL_TEST_CASES",
]