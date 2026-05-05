# test_regression/resnet/data/__init__.py
"""
ResNet model test cases and IR baselines for regression testing.

Note: Model definitions are imported from ace.models.resnet
"""
from ace.models.resnet.resnet20 import ResNet_CIFAR, BasicBlock
from ace.models.resnet import resnet20_cifar10
from .models import (
    MODEL_RESNET20_TEST_CASES,
    MODEL_RESNET32_TEST_CASES,
    MODEL_RESNET44_TEST_CASES,
    MODEL_RESNET56_TEST_CASES,
    RESNET_MODEL_TEST_CASES,
)
from .ir_baselines import (
    RESNET20_IR_BASELINE,
    RESNET32_IR_BASELINE,
    RESNET44_IR_BASELINE,
    RESNET56_IR_BASELINE,
    IR_BASELINES,
    extract_ir_structure,
    compare_ir_structure,
    validate_resnet_ir,
)

__all__ = [
    "ResNet_CIFAR",
    "BasicBlock",
    "resnet20_cifar10",
    "RESNET_MODEL_TEST_CASES",
    "MODEL_RESNET20_TEST_CASES",
    "MODEL_RESNET32_TEST_CASES",
    "MODEL_RESNET44_TEST_CASES",
    "MODEL_RESNET56_TEST_CASES",
    "IR_BASELINES",
    "RESNET20_IR_BASELINE",
    "RESNET32_IR_BASELINE",
    "RESNET44_IR_BASELINE",
    "RESNET56_IR_BASELINE",
    "extract_ir_structure",
    "compare_ir_structure",
    "validate_resnet_ir",
]