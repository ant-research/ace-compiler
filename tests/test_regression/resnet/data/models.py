# test_regression/resnet/cases/test_cases.py
"""
ResNet model test cases for CIFAR-10 classification.

Note: Model definitions and weights are imported from ace.models.resnet
"""
import torch

from test_cases.base import ModelTestCase
from ace.models.resnet.resnet20 import ResNet_CIFAR, BasicBlock
from ace.models.resnet import (
    resnet20_cifar10, load_resnet20_pretrained, RESNET20_COMPILE_OPTIONS,
    resnet32_cifar10, load_resnet32_pretrained,
    resnet44_cifar10, load_resnet44_pretrained,
    resnet56_cifar10, load_resnet56_pretrained,
)

MODEL_RESNET20_TEST_CASES = [
    ModelTestCase(
        name="resnet20_cifar10",
        model_class=ResNet_CIFAR,
        example_inputs=(torch.randn(1, 3, 32, 32),),
        encrypt_inputs=["x"],
        model_init_args=(BasicBlock, [3, 3, 3]),
        model_init_kwargs={"num_classes": 10},
        expected_ops=["Conv", "Relu", "Add", "AvgPool", "Gemm"],
        model_post_init=load_resnet20_pretrained,
        compile_options=RESNET20_COMPILE_OPTIONS,
    ),
]

MODEL_RESNET32_TEST_CASES = [
    ModelTestCase(
        name="resnet32_cifar10",
        model_class=ResNet_CIFAR,
        example_inputs=(torch.randn(1, 3, 32, 32),),
        encrypt_inputs=["x"],
        model_init_args=(BasicBlock, [5, 5, 5]),
        model_init_kwargs={"num_classes": 10},
        expected_ops=["Conv", "Relu", "Add", "AvgPool", "Gemm"],
        model_post_init=load_resnet32_pretrained,
    ),
]

MODEL_RESNET44_TEST_CASES = [
    ModelTestCase(
        name="resnet44_cifar10",
        model_class=ResNet_CIFAR,
        example_inputs=(torch.randn(1, 3, 32, 32),),
        encrypt_inputs=["x"],
        model_init_args=(BasicBlock, [7, 7, 7]),
        model_init_kwargs={"num_classes": 10},
        expected_ops=["Conv", "Relu", "Add", "AvgPool", "Gemm"],
        model_post_init=load_resnet44_pretrained,
    ),
]

MODEL_RESNET56_TEST_CASES = [
    ModelTestCase(
        name="resnet56_cifar10",
        model_class=ResNet_CIFAR,
        example_inputs=(torch.randn(1, 3, 32, 32),),
        encrypt_inputs=["x"],
        model_init_args=(BasicBlock, [9, 9, 9]),
        model_init_kwargs={"num_classes": 10},
        expected_ops=["Conv", "Relu", "Add", "AvgPool", "Gemm"],
        model_post_init=load_resnet56_pretrained,
    ),
]

RESNET_MODEL_TEST_CASES = (
    MODEL_RESNET20_TEST_CASES
    + MODEL_RESNET32_TEST_CASES
    + MODEL_RESNET44_TEST_CASES
    + MODEL_RESNET56_TEST_CASES
)