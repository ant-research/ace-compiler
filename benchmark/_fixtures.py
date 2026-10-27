# benchmark/_fixtures.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Shared benchmark utilities: target params, compile helpers, dataset loaders,
# GPU tag for baseline file naming.
# Imported by benchmark/conftest.py and model-specific conftest.py files.

import re

import pytest

from ace import fhe


_gpu_available = fhe.gpu_available()

# ============================================================================
# Target Parameters — (library, device) tuples for parametrized tests
#
# Each param is a 2-tuple used as a single indirect value to fixtures.
# GPU targets carry auto-skip marks.
# ============================================================================

TARGET_PARAMS = [
    pytest.param(("ant", "cpu"), id="ant-cpu"),
    pytest.param(
        ("phantom", "cuda"),
        marks=pytest.mark.skipif(not _gpu_available, reason="GPU not available"),
        id="phantom-cuda",
    ),
    pytest.param(
        ("ace", "cuda"),
        marks=pytest.mark.skipif(not _gpu_available, reason="GPU not available"),
        id="ace-cuda",
    ),
]


# ============================================================================
# GPU tag for baseline file naming
# ============================================================================


def gpu_tag() -> str:
    """Return a short GPU model tag for baseline file naming.

    Maps torch.cuda device names to short tags:
      NVIDIA A100*      → "a100"
      NVIDIA H100*      → "h100"
      NVIDIA H20*       → "h20"
      NVIDIA A10*       → "a10"
      NVIDIA V100*      → "v100"
      NVIDIA T4*        → "t4"
      ...               → first word lowercased

    Returns "cpu" when CUDA is not available.

    Usage in test files:
        tag = gpu_tag()
        num_regression.check(data, f"resnet20_cifar10_ace_cuda_{tag}_latency", ...)
    """
    if not _gpu_available:
        return "cpu"
    try:
        import torch

        name = torch.cuda.get_device_name(0)  # e.g. "NVIDIA A100-SXM4-80GB"
        # Extract the model part: "A100", "H20", etc.
        # Pattern: "NVIDIA <MODEL>[-<SUBMODEL>...]"
        m = re.match(r"NVIDIA\s+(\w+)", name, re.IGNORECASE)
        if m:
            return m.group(1).lower()
        # Fallback: first word after stripping "NVIDIA"
        return name.split()[1].lower() if len(name.split()) > 1 else "gpu"
    except Exception:
        return "gpu"


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
# Shared compile helper
# ============================================================================


def _compile_spec(spec, library="phantom", device="cuda", relu_vr_file=None):
    """Compile a ModelSpec and return CompiledProgram.

    Args:
        spec: ModelSpec to compile
        library: Backend library ("phantom", "ace", "ant")
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
