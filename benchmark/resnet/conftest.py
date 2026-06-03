# benchmark/resnet/conftest.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Shared fixtures for ResNet FHE benchmarks.

import os

import pytest
import torch

from ace import fhe


# ============================================================================
# Skip markers
# ============================================================================

requires_torch_fx = pytest.mark.skipif(
    not getattr(torch, "fx", None),
    reason="torch.fx not available",
)

requires_gpu = pytest.mark.skipif(
    not fhe.gpu_available(),
    reason="GPU not available",
)


# ============================================================================
# Dataset loaders
# ============================================================================

_DATASET_LOADERS = {
    "cifar10": ("ace.model.dataset", "load_cifar10_images", "CIFAR10_CLASSES"),
    "cifar100": ("ace.model.dataset", "load_cifar100_images", "CIFAR100_CLASSES"),
}

_MAX_IMAGES = 1000


def _load_images(dataset_name, num_images, offset=0):
    """Load dataset images and labels."""
    import importlib
    module_name, load_fn_name, classes_name = _DATASET_LOADERS[dataset_name]
    mod = importlib.import_module(module_name)
    load_fn = getattr(mod, load_fn_name)
    class_names = getattr(mod, classes_name)
    images, labels = load_fn(num_images, offset=offset)
    return images, labels, class_names


# ============================================================================
# Session-scoped dataset fixtures
# ============================================================================

@pytest.fixture(scope="session")
def cifar10_images():
    """Load CIFAR-10 test images (up to 1000)."""
    images, labels, class_names = _load_images("cifar10", _MAX_IMAGES)
    return images, labels, class_names


@pytest.fixture(scope="session")
def cifar100_images():
    """Load CIFAR-100 test images (up to 1000)."""
    images, labels, class_names = _load_images("cifar100", _MAX_IMAGES)
    return images, labels, class_names


# ============================================================================
# Model / compile fixtures
# ============================================================================

# Direct imports — no lookup table needed
from ace.model.spec_resnet import (
    RESNET20_CIFAR10,
    RESNET32_CIFAR10,
    RESNET32_CIFAR100,
    RESNET44_CIFAR10,
    RESNET56_CIFAR10,
    RESNET110_CIFAR10,
)


def _compile_spec(spec, library="phantom", device="cuda", relu_vr_file=None):
    """Compile a ModelSpec and return CompiledProgram.

    Args:
        spec: ModelSpec to compile
        library: Backend library ("phantom", "ant")
        device: Device ("cuda", "cpu")
        relu_vr_file: Optional path to ReLU VR profile JSON
    """
    model = spec.create_model()
    model.eval()

    # Get backend-specific compile options
    compile_opts = dict(spec.compile_options.get(library, {})) if spec.compile_options else {}

    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        relu_vr_file=relu_vr_file,
        **compile_opts,
    )(model)

    return compiled.fhe_compile(spec.example_inputs)


# ============================================================================
# Compiled model fixtures (parameterized by target)
# ============================================================================

_TARGET_CONFIG = {
    "phantom": {"library": "phantom", "device": "cuda"},
    "ant": {"library": "ant", "device": "cpu"},
}


@pytest.fixture(scope="module")
def compiled_resnet_20(request):
    """Module-scoped compiled ResNet-20. Parametrized by target ('phantom' or 'ant')."""
    target = request.param
    config = _TARGET_CONFIG[target]
    vr_file = RESNET20_CIFAR10.get_vr_profile()
    return _compile_spec(RESNET20_CIFAR10, **config, relu_vr_file=vr_file)


@pytest.fixture(scope="module")
def compiled_resnet_32(request):
    target = request.param
    config = _TARGET_CONFIG[target]
    vr_file = RESNET32_CIFAR10.get_vr_profile()
    return _compile_spec(RESNET32_CIFAR10, **config, relu_vr_file=vr_file)


@pytest.fixture(scope="module")
def compiled_resnet_44(request):
    target = request.param
    config = _TARGET_CONFIG[target]
    vr_file = RESNET44_CIFAR10.get_vr_profile()
    return _compile_spec(RESNET44_CIFAR10, **config, relu_vr_file=vr_file)


@pytest.fixture(scope="module")
def compiled_resnet_56(request):
    target = request.param
    config = _TARGET_CONFIG[target]
    vr_file = RESNET56_CIFAR10.get_vr_profile()
    return _compile_spec(RESNET56_CIFAR10, **config, relu_vr_file=vr_file)


@pytest.fixture(scope="module")
def compiled_resnet_110(request):
    target = request.param
    config = _TARGET_CONFIG[target]
    vr_file = RESNET110_CIFAR10.get_vr_profile()
    return _compile_spec(RESNET110_CIFAR10, **config, relu_vr_file=vr_file)


@pytest.fixture(scope="module")
def compiled_resnet_32_cifar100(request):
    target = request.param
    config = _TARGET_CONFIG[target]
    vr_file = RESNET32_CIFAR100.get_vr_profile()
    return _compile_spec(RESNET32_CIFAR100, **config, relu_vr_file=vr_file)


# ============================================================================
# Model fixture for plaintext comparison
# ============================================================================

@pytest.fixture(scope="module")
def resnet20_model():
    """Module-scoped ResNet-20 model for plaintext inference."""
    model = RESNET20_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="module")
def resnet32_model():
    model = RESNET32_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="module")
def resnet44_model():
    model = RESNET44_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="module")
def resnet56_model():
    model = RESNET56_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="module")
def resnet110_model():
    model = RESNET110_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="module")
def resnet32_cifar100_model():
    model = RESNET32_CIFAR100.create_model()
    model.eval()
    return model


# ============================================================================
# Pytest configuration
# ============================================================================

def pytest_configure(config):
    config.addinivalue_line("markers", "slow: Slow tests (1000-image benchmarks)")
    config.addinivalue_line("markers", "resnet: ResNet benchmark tests")
    config.addinivalue_line("markers", "requires_gpu: Tests requiring GPU")