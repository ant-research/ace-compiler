#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet CompileSpec instances using three-layer architecture.

Layer 1: ModelEntity - describes the ResNet model
Layer 2: CompileConfig - describes how to compile
Layer 3: RuntimeConfig - describes how to run/validate
"""
import torch

from ace.fhe.spec import (
    CompileSpec, ModelEntity, CompileConfig, InputSpec,
    RuntimeConfig, DatasetSource
)
# Import compile options directly from config
from ace.models.resnet.config import (
    RESNET20_COMPILE_OPTIONS,
    RESNET32_COMPILE_OPTIONS,
    RESNET32_CIFAR100_COMPILE_OPTIONS,
    RESNET44_COMPILE_OPTIONS,
    RESNET56_COMPILE_OPTIONS,
    RESNET110_COMPILE_OPTIONS,
)


def _get_resnet_ir_ops(n_layers):
    """Get expected IR ops for ResNet model."""
    # Basic ops in ResNet: Conv, BatchNorm, Relu, Add, GlobalAveragePool, Gemm
    base_ops = ["Conv", "BatchNormalization", "Relu", "Add", "GlobalAveragePool", "Gemm"]
    return base_ops


def _create_resnet_creator(n_layers, num_classes):
    """Create a function that builds ResNet model."""
    def create():
        from ace.models.resnet import create_pretrained_resnet
        return create_pretrained_resnet(n_layers, num_classes=num_classes)
    return create


# =============================================================================
# Helper to build ResNet CompileSpec
# =============================================================================

def _make_resnet_spec(n_layers, num_classes, compile_options, dataset="cifar10"):
    """Build a ResNet CompileSpec with three-layer structure."""
    entity = ModelEntity(
        name=f"resnet{n_layers}_cifar{num_classes}",
        model_class=torch.nn.Module,  # Base class, actual creation via create()
        weights_required=True,
        ir_ops=_get_resnet_ir_ops(n_layers),
    )

    compile_config = CompileConfig(
        input_spec=[InputSpec(shape=(1, 3, 32, 32), dtype=torch.float32)],
        compile_options=dict(compile_options),
        encrypt_inputs=["x"],
    )

    runtime_config = RuntimeConfig(
        input_source=DatasetSource(dataset=dataset, num_samples=1),
    )

    # Create spec and override create method
    spec = CompileSpec(
        entity=entity,
        compile=compile_config,
        runtime=runtime_config,
    )

    # Override create to use create_pretrained_resnet
    def create_fn():
        from ace.models.resnet import create_pretrained_resnet
        model = create_pretrained_resnet(n_layers, num_classes=num_classes)
        model._fhe_name = entity.name
        return model

    spec.create = create_fn

    return spec


# =============================================================================
# CIFAR-10 Specs
# =============================================================================

RESNET20_CIFAR10 = _make_resnet_spec(20, 10, RESNET20_COMPILE_OPTIONS, "cifar10")
RESNET32_CIFAR10 = _make_resnet_spec(32, 10, RESNET32_COMPILE_OPTIONS, "cifar10")
RESNET44_CIFAR10 = _make_resnet_spec(44, 10, RESNET44_COMPILE_OPTIONS, "cifar10")
RESNET56_CIFAR10 = _make_resnet_spec(56, 10, RESNET56_COMPILE_OPTIONS, "cifar10")
RESNET110_CIFAR10 = _make_resnet_spec(110, 10, RESNET110_COMPILE_OPTIONS, "cifar10")

# =============================================================================
# CIFAR-100 Specs
# =============================================================================

RESNET32_CIFAR100 = _make_resnet_spec(32, 100, RESNET32_CIFAR100_COMPILE_OPTIONS, "cifar100")

# =============================================================================
# All Specs
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
]