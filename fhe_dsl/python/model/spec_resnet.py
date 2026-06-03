#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet ModelSpec instances for FHE compilation.

Default compile options tuned for phantom (GPU) backend.
For other backends (e.g., antlib), override options in your test/benchmark case.

Usage:
    from ace.model.spec_resnet import RESNET20_CIFAR10
    model = RESNET20_CIFAR10.create_model()
"""
import os
import torch

from ace.fhe.config.spec import ModelSpec

# Import model classes from respective files (with aliases for clarity)
from ace.model.resnet.resnet20 import ResNet_CIFAR as ResNet20, BasicBlock as BasicBlock20
from ace.model.resnet.resnet32 import ResNet_CIFAR as ResNet32, BasicBlock as BasicBlock32
from ace.model.resnet.resnet44 import ResNet_CIFAR as ResNet44, BasicBlock as BasicBlock44
from ace.model.resnet.resnet56 import ResNet_CIFAR as ResNet56, BasicBlock as BasicBlock56
from ace.model.resnet.resnet110 import ResNet_CIFAR as ResNet110, BasicBlock as BasicBlock110


# =============================================================================
# Weights Directory
# =============================================================================

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "resnet", "weights")


# =============================================================================
# Weight Loading Functions
# =============================================================================

def _load_checkpoint(weights_path: str, model: torch.nn.Module):
    """Load checkpoint from file into model."""
    if os.path.exists(weights_path):
        checkpoint = torch.load(weights_path, map_location='cpu')
        # Support both formats: direct state_dict or checkpoint dict with state_dict
        if isinstance(checkpoint, dict) and 'state_dict' in checkpoint:
            state_dict = checkpoint['state_dict']
        else:
            state_dict = checkpoint
        model.load_state_dict(state_dict)
    model.eval()


def load_resnet_pretrained(model: torch.nn.Module, n_layers: int, num_classes: int = 10):
    """Load pre-trained weights into ResNet model.

    Args:
        model: ResNet model instance
        n_layers: ResNet depth (20, 32, 44, 56, 110)
        num_classes: Number of output classes (10 or 100)

    Raises:
        FileNotFoundError: If weights file not found
    """
    weights_file = f"resnet{n_layers}_cifar{num_classes}.pt"
    weights_path = os.path.join(_WEIGHTS_DIR, weights_file)

    if not os.path.exists(weights_path):
        raise FileNotFoundError(f"Weights file not found: {weights_path}")

    _load_checkpoint(weights_path, model)


# =============================================================================
# Model Post-Init Factory
# =============================================================================

def _load_weights(n_layers: int, num_classes: int = 10):
    def post_init(model):
        load_resnet_pretrained(model, n_layers=n_layers, num_classes=num_classes)
    return post_init


# =============================================================================
# Common Constants
# =============================================================================

_RESNET_IR_OPS = ["Conv", "BatchNormalization", "Relu", "Add", "GlobalAveragePool", "Gemm"]


# =============================================================================
# ResNet-20 CIFAR-10
# =============================================================================

RESNET20_CIFAR10 = ModelSpec(
    name="resnet20_cifar10",
    model_class=ResNet20,
    example_inputs=(torch.randn(1, 3, 32, 32, dtype=torch.float32),),
    encrypt_inputs=["x"],
    model_init_args=(BasicBlock20, [3, 3, 3]),
    model_init_kwargs={"num_classes": 10},
    model_post_init=_load_weights(20, 10),
    compile_options={
        "phantom": {
            "vec": {"conv_parl": True, "ssf": True},
            "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 17},
        },
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"ts": True, "rtt": True},
            "sihe": {"ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
        "acelib": {
            "ckks": {"sbm": True, "hw": 192, "q0": 56, "sf": 51, "N": 65536, "icl": 17, "mcl": 34},
            "p2c": {"fp": True},
        },
    },
    expected_ops=_RESNET_IR_OPS,
    weights_required=True,
    dataset="cifar10",
)


# =============================================================================
# ResNet-32 CIFAR-10
# =============================================================================

RESNET32_CIFAR10 = ModelSpec(
    name="resnet32_cifar10",
    model_class=ResNet32,
    example_inputs=(torch.randn(1, 3, 32, 32, dtype=torch.float32),),
    encrypt_inputs=["x"],
    model_init_args=(BasicBlock32, [5, 5, 5]),
    model_init_kwargs={"num_classes": 10},
    model_post_init=_load_weights(32, 10),
    compile_options={
        "phantom": {
            "vec": {"conv_parl": True, "ssf": True},
            "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 17},
        },
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"ts": True, "rtt": True},
            "sihe": {"ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
    },
    expected_ops=_RESNET_IR_OPS,
    weights_required=True,
    dataset="cifar10",
)


# =============================================================================
# ResNet-32 CIFAR-100
# =============================================================================

RESNET32_CIFAR100 = ModelSpec(
    name="resnet32_cifar100",
    model_class=ResNet32,
    example_inputs=(torch.randn(1, 3, 32, 32, dtype=torch.float32),),
    encrypt_inputs=["x"],
    model_init_args=(BasicBlock32, [5, 5, 5]),
    model_init_kwargs={"num_classes": 100},
    model_post_init=_load_weights(32, 100),
    compile_options={
        "phantom": {
            "vec": {"conv_parl": True, "ssf": True},
            "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 17},
        },
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"ts": True, "rtt": True},
            "sihe": {"ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
    },
    expected_ops=_RESNET_IR_OPS,
    weights_required=True,
    dataset="cifar100",
)


# =============================================================================
# ResNet-44 CIFAR-10
# =============================================================================

RESNET44_CIFAR10 = ModelSpec(
    name="resnet44_cifar10",
    model_class=ResNet44,
    example_inputs=(torch.randn(1, 3, 32, 32, dtype=torch.float32),),
    encrypt_inputs=["x"],
    model_init_args=(BasicBlock44, [7, 7, 7]),
    model_init_kwargs={"num_classes": 10},
    model_post_init=_load_weights(44, 10),
    compile_options={
        "phantom": {
            "vec": {"conv_parl": True, "ssf": True},
            "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 17},
        },
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"ts": True, "rtt": True},
            "sihe": {"ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
    },
    expected_ops=_RESNET_IR_OPS,
    weights_required=True,
    dataset="cifar10",
)


# =============================================================================
# ResNet-56 CIFAR-10
# =============================================================================

RESNET56_CIFAR10 = ModelSpec(
    name="resnet56_cifar10",
    model_class=ResNet56,
    example_inputs=(torch.randn(1, 3, 32, 32, dtype=torch.float32),),
    encrypt_inputs=["x"],
    model_init_args=(BasicBlock56, [9, 9, 9]),
    model_init_kwargs={"num_classes": 10},
    model_post_init=_load_weights(56, 10),
    compile_options={
        "phantom": {
            "vec": {"conv_parl": True, "ssf": True},
            "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 17},
        },
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"ts": True, "rtt": True},
            "sihe": {"ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
    },
    expected_ops=_RESNET_IR_OPS,
    weights_required=True,
    dataset="cifar10",
)


# =============================================================================
# ResNet-110 CIFAR-10
# =============================================================================

RESNET110_CIFAR10 = ModelSpec(
    name="resnet110_cifar10",
    model_class=ResNet110,
    example_inputs=(torch.randn(1, 3, 32, 32, dtype=torch.float32),),
    encrypt_inputs=["x"],
    model_init_args=(BasicBlock110, [18, 18, 18]),
    model_init_kwargs={"num_classes": 10},
    model_post_init=_load_weights(110, 10),
    compile_options={
        "phantom": {
            "vec": {"conv_parl": True, "ssf": True},
            "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 17},
        },
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"ts": True, "rtt": True},
            "sihe": {"ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
    },
    expected_ops=_RESNET_IR_OPS,
    weights_required=True,
    dataset="cifar10",
)


# =============================================================================
# Spec Collections
# =============================================================================

RESNET_CIFAR10_SPECS = [
    RESNET20_CIFAR10,
    RESNET32_CIFAR10,
    RESNET44_CIFAR10,
    RESNET56_CIFAR10,
    RESNET110_CIFAR10,
]

RESNET_CIFAR100_SPECS = [
    RESNET32_CIFAR100,
]

ALL_RESNET_SPECS = RESNET_CIFAR10_SPECS + RESNET_CIFAR100_SPECS

__all__ = [
    "RESNET20_CIFAR10",
    "RESNET32_CIFAR10",
    "RESNET32_CIFAR100",
    "RESNET44_CIFAR10",
    "RESNET56_CIFAR10",
    "RESNET110_CIFAR10",
    "RESNET_CIFAR10_SPECS",
    "RESNET_CIFAR100_SPECS",
    "ALL_RESNET_SPECS",
    # Export weight loading function for external use
    "load_resnet_pretrained",
]