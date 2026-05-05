# test_regression/resnet/test_inference.py
"""
ResNet FHE inference tests.

Organization: one test class per library, methods per (model, dataset) variant.
- Primary library (phantom): full model coverage on CIFAR-10
- Other libraries: same structure
- CIFAR-100: only ResNet-32 supports it for now
- Adding a new library = adding a new class
- Adding a new model/dataset = adding a method to each relevant class

Run examples:
    pytest test_inference.py -k TestResNetPhantom           # phantom-cuda all
    pytest test_inference.py -k "resnet20 and phantom"      # resnet20 on phantom
    pytest test_inference.py -k cifar100                    # cifar-100 tests
    pytest test_inference.py -k TestResNetAntlib             # antlib-cpu all
"""
import pytest
import torch

from ace import fhe
from test_utils import TORCH_FX_AVAILABLE


requires_torch_fx = pytest.mark.skipif(
    not TORCH_FX_AVAILABLE,
    reason="torch.fx not available"
)

requires_gpu = pytest.mark.skipif(
    not fhe.gpu_available(),
    reason="GPU not available"
)

# CIFAR-100 support pending: pretrained weights and data loader not yet available
requires_cifar100 = pytest.mark.skipif(
    True,
    reason="CIFAR-100 pretrained weights not yet available"
)


# ============================================================================
# Helpers
# ============================================================================

# Dataset loaders keyed by dataset name (matches spec.runtime.input_source.dataset)
_DATASET_LOADERS = {
    "cifar10": ("ace.models.cifar10", "load_cifar10_images", "CIFAR10_CLASSES"),
    "cifar100": ("ace.models.cifar100", "load_cifar100_images", "CIFAR100_CLASSES"),
}


def _get_resnet_spec(n_layers, num_classes=10):
    """Get CompileSpec for the given ResNet variant."""
    from ace.models.resnet.specs import (
        RESNET20_CIFAR10,
        RESNET32_CIFAR10,
        RESNET32_CIFAR100,
        RESNET44_CIFAR10,
        RESNET56_CIFAR10,
        RESNET110_CIFAR10,
    )
    spec_map = {
        (20, 10): RESNET20_CIFAR10,
        (32, 10): RESNET32_CIFAR10,
        (32, 100): RESNET32_CIFAR100,
        (44, 10): RESNET44_CIFAR10,
        (56, 10): RESNET56_CIFAR10,
        (110, 10): RESNET110_CIFAR10,
    }
    key = (n_layers, num_classes)
    if key not in spec_map:
        raise ValueError(f"No spec for ResNet-{n_layers} with {num_classes} classes")
    return spec_map[key]


def _compile_resnet(n_layers, library=None, device=None, num_classes=10):
    """Compile a ResNet model using CompileSpec (Driver handles caching automatically).

    Reads frontend/library/device defaults from spec.compile, with optional overrides.
    """
    spec = _get_resnet_spec(n_layers, num_classes)
    model = spec.create()

    example_inputs = tuple(
        torch.randn(inp.shape, dtype=inp.dtype)
        for inp in spec.compile.input_spec
    )

    # Use spec.compile defaults, allow caller overrides
    frontend = spec.compile.frontend
    lib = library or spec.compile.library
    dev = device or spec.compile.device

    compile_options = dict(spec.compile.compile_options)
    compile_options.setdefault("p2c", {})["lib"] = lib

    compiled = fhe.compile(
        frontend=frontend,
        library=lib,
        device=dev,
        encrypt_inputs=spec.compile.encrypt_inputs,
        **compile_options,
    )(model)

    return compiled.fhe_compile(example_inputs)


def _load_dataset(spec):
    """Load dataset from spec.runtime.input_source."""
    if spec.runtime is None or spec.runtime.input_source is None:
        raise ValueError(f"Spec '{spec.name}' has no runtime.input_source configured")

    source = spec.runtime.input_source
    dataset_name = source.dataset

    if dataset_name not in _DATASET_LOADERS:
        raise ValueError(f"Unknown dataset: {dataset_name}. "
                         f"Available: {list(_DATASET_LOADERS.keys())}")

    module_name, load_fn_name, classes_name = _DATASET_LOADERS[dataset_name]
    import importlib
    mod = importlib.import_module(module_name)
    load_fn = getattr(mod, load_fn_name)
    class_names = getattr(mod, classes_name)

    images, labels = load_fn(source.num_samples, offset=source.offset)
    return images, labels, class_names


def _assert_fhe_matches_plaintext(compiled_model, n_layers, num_classes=10):
    """Run single-image FHE inference and assert prediction matches plaintext."""
    spec = _get_resnet_spec(n_layers, num_classes)
    model = spec.create()

    images, labels, class_names = _load_dataset(spec)

    with torch.no_grad():
        plain_output = model(images[0:1])
    plain_pred = plain_output.argmax(dim=1).item()

    result = compiled_model(images[0:1])
    assert result is not None
    assert isinstance(result, torch.Tensor)

    fhe_pred = result.flatten().argmax().item()
    dataset = spec.runtime.input_source.dataset.upper()
    print(f"\n[ResNet-{n_layers}/{dataset}] plaintext={plain_pred} ({class_names[plain_pred]}), "
          f"fhe={fhe_pred} ({class_names[fhe_pred]}), "
          f"label={labels[0]} ({class_names[labels[0]]})")

    assert fhe_pred == plain_pred, (
        f"ResNet-{n_layers}/{dataset}: FHE prediction {fhe_pred} != plaintext {plain_pred}"
    )

# ============================================================================
# Antlib (CPU) - Primary CPU backend
# ============================================================================

@requires_torch_fx
@pytest.mark.slow
class TestResNetAntlib:
    """ResNet inference on antlib-cpu."""

    # --- CIFAR-10 ---
    def test_inference_resnet20_cifar10(self):
        compiled = _compile_resnet(20, library="antlib", device="cpu")
        _assert_fhe_matches_plaintext(compiled, 20)

    def test_inference_resnet32_cifar10(self):
        compiled = _compile_resnet(32, library="antlib", device="cpu")
        _assert_fhe_matches_plaintext(compiled, 32)

    def test_inference_resnet44_cifar10(self):
        compiled = _compile_resnet(44, library="antlib", device="cpu")
        _assert_fhe_matches_plaintext(compiled, 44)

    def test_inference_resnet56_cifar10(self):
        compiled = _compile_resnet(56, library="antlib", device="cpu")
        _assert_fhe_matches_plaintext(compiled, 56)

    # --- CIFAR-100 ---
    @requires_cifar100
    def test_inference_resnet32_cifar100(self):
        compiled = _compile_resnet(32, library="antlib", device="cpu", num_classes=100)
        _assert_fhe_matches_plaintext(compiled, 32, num_classes=100)


# ============================================================================
# Phantom (CUDA) - Primary GPU backend
# ============================================================================

@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNetPhantom:
    """ResNet inference on phantom-cuda."""

    # --- CIFAR-10 ---
    def test_inference_resnet20_cifar10(self):
        compiled = _compile_resnet(20, library="phantom", device="cuda")
        _assert_fhe_matches_plaintext(compiled, 20)

    def test_inference_resnet32_cifar10(self):
        compiled = _compile_resnet(32, library="phantom", device="cuda")
        _assert_fhe_matches_plaintext(compiled, 32)

    def test_inference_resnet44_cifar10(self):
        compiled = _compile_resnet(44, library="phantom", device="cuda")
        _assert_fhe_matches_plaintext(compiled, 44)

    def test_inference_resnet56_cifar10(self):
        compiled = _compile_resnet(56, library="phantom", device="cuda")
        _assert_fhe_matches_plaintext(compiled, 56)

    # --- CIFAR-100 ---
    @requires_cifar100
    def test_inference_resnet32_cifar100(self):
        compiled = _compile_resnet(32, library="phantom", device="cuda", num_classes=100)
        _assert_fhe_matches_plaintext(compiled, 32, num_classes=100)


# ============================================================================
# Additional libraries - Uncomment and extend as backends become available
# ============================================================================

# @requires_torch_fx
# @requires_gpu
# @pytest.mark.slow
# class TestResNetHyperfhe:
#     """ResNet inference on hyperfhe-cuda."""
#
#     # --- CIFAR-10 ---
#     def test_inference_resnet20_cifar10(self):
#         compiled = _compile_resnet(20, library="hyperfhe", device="cuda")
#         _assert_fhe_matches_plaintext(compiled, 20)
#
#     # --- CIFAR-100 ---
#     @requires_cifar100
#     def test_inference_resnet32_cifar100(self):
#         compiled = _compile_resnet(32, library="hyperfhe", device="cuda", num_classes=100)
#         _assert_fhe_matches_plaintext(compiled, 32, num_classes=100)

# @requires_torch_fx
# @pytest.mark.slow
# class TestResNetSeal:
#     """ResNet inference on seal-cpu."""
#
#     # --- CIFAR-10 ---
#     def test_inference_resnet20_cifar10(self):
#         compiled = _compile_resnet(20, library="seal", device="cpu")
#         _assert_fhe_matches_plaintext(compiled, 20)
#
#     # --- CIFAR-100 ---
#     @requires_cifar100
#     def test_inference_resnet32_cifar100(self):
#         compiled = _compile_resnet(32, library="seal", device="cpu", num_classes=100)
#         _assert_fhe_matches_plaintext(compiled, 32, num_classes=100)

# @requires_torch_fx
# @pytest.mark.slow
# class TestResNetOpenfhe:
#     """ResNet inference on openfhe-cpu."""
#
#     # --- CIFAR-10 ---
#     def test_inference_resnet20_cifar10(self):
#         compiled = _compile_resnet(20, library="openfhe", device="cpu")
#         _assert_fhe_matches_plaintext(compiled, 20)
#
#     # --- CIFAR-100 ---
#     @requires_cifar100
#     def test_inference_resnet32_cifar100(self):
#         compiled = _compile_resnet(32, library="openfhe", device="cpu", num_classes=100)
#         _assert_fhe_matches_plaintext(compiled, 32, num_classes=100)