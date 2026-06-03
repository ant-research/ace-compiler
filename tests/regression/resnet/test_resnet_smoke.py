# tests/regression/resnet/test_resnet_smoke.py
"""
ResNet FHE inference smoke tests.

Uses pre-profiled VR data (from profiles/*.json) for ReLU polynomial
approximation, then validates FHE output against plaintext.

Run examples:
    pytest test_smoke.py -k "ant"                                   # antlib-cpu all
    pytest test_smoke.py -k "phantom"                               # phantom-cuda all
    pytest test_smoke.py -k "acelib"                                 # acelib-cuda all
    pytest test_smoke.py -k "resnet20 and phantom"                  # resnet20 on phantom
    pytest test_smoke.py -k "resnet20 and acelib"                    # resnet20 on acelib
    pytest test_smoke.py -k "[phantom-cuda-resnet32_cifar100]"      # specific model+backend
    pytest test_smoke.py -m "not slow"                              # skip slow (antlib) tests
"""
import copy
from pathlib import Path

import numpy as np
import pytest
import torch

from ace import fhe
from ace.model.spec_resnet import ALL_RESNET_SPECS
from utils import requires_gpu


def _phantom_available():
    try:
        from ace.fhe.backend import get_library_impl
        return get_library_impl("phantom", device="cuda").check_available()
    except Exception:
        return False

def _acelib_available():
    try:
        from ace.fhe.backend import get_library_impl
        return get_library_impl("acelib", device="cuda").check_available()
    except Exception:
        return False

# Bundled sample images per dataset: [1, 3, 32, 32] float32, normalized
_SAMPLE_DATA = {
    "cifar10": Path(__file__).parent.parent / "data" / "cifar10_sample.npz",
    "cifar100": Path(__file__).parent.parent / "data" / "cifar100_sample.npz",
}


def _load_sample_image(spec):
    """Load the bundled sample image matching the spec's dataset."""
    dataset = spec.dataset
    if dataset not in _SAMPLE_DATA:
        raise ValueError(f"No sample data for dataset: {dataset}")
    data = np.load(_SAMPLE_DATA[dataset])
    return torch.from_numpy(data["image"])  # [1, 3, 32, 32]


def _compile_resnet(spec, sample_input, library="antlib", device="cpu"):
    """Compile a ResNet model with pre-profiled VR data from ModelSpec.

    Uses the full-dataset VR profile (profiles/*.json) which provides
    adequate headroom for FHE noise accumulation. Falls back to
    profile_relu=True only if no profile file is available.
    """
    model = spec.create_model()

    # Build compile kwargs from spec's library-specific options
    kwargs = {}
    if spec.compile_options and library in spec.compile_options:
        kwargs = copy.deepcopy(spec.compile_options[library])

    # Use pre-profiled VR data from ModelSpec (full-dataset profile)
    vr_file = spec.get_vr_profile()
    if vr_file is not None:
        kwargs["relu_vr_file"] = vr_file
    else:
        # Fallback: built-in profiling (less reliable for deep networks)
        kwargs["profile_relu"] = True

    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        **kwargs,
    )(model)

    return compiled.fhe_compile((sample_input,))


def _assert_fhe_matches_plaintext(compiled_model, spec, sample_input):
    """Run FHE and plaintext inference on the same input, assert same predicted class."""
    model = spec.create_model()
    model.eval()

    with torch.no_grad():
        plain_output = model(sample_input)
    plain_pred = plain_output.argmax(dim=1).item()

    fhe_output = compiled_model(sample_input)
    assert fhe_output is not None, f"{spec.name}: FHE inference returned None"
    assert isinstance(fhe_output, torch.Tensor), f"{spec.name}: expected Tensor, got {type(fhe_output)}"

    fhe_pred = fhe_output.flatten().argmax().item()
    print(f"[{spec.name}] plaintext={plain_pred}, fhe={fhe_pred}")
    assert fhe_pred == plain_pred, (
        f"{spec.name}: FHE prediction {fhe_pred} != plaintext {plain_pred}"
    )


# ============================================================================
# Antlib (CPU) — slow
# ============================================================================

@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_RESNET_SPECS, ids=lambda s: s.name)
def test_smoke_antlib(spec):
    """ResNet FHE inference on antlib-cpu (slow)."""
    sample = _load_sample_image(spec)
    compiled = _compile_resnet(spec, sample, library="antlib", device="cpu")
    _assert_fhe_matches_plaintext(compiled, spec, sample)


# ============================================================================
# Phantom (CUDA)
# ============================================================================

@requires_gpu
@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_RESNET_SPECS, ids=lambda s: s.name)
def test_smoke_phantom(spec):
    """ResNet FHE inference on phantom-cuda."""
    if not _phantom_available():
        pytest.skip("phantom-cuda compiler not available")
    sample = _load_sample_image(spec)
    compiled = _compile_resnet(spec, sample, library="phantom", device="cuda")
    _assert_fhe_matches_plaintext(compiled, spec, sample)


# ============================================================================
# Acelib (CUDA)
# ============================================================================

@requires_gpu
@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_RESNET_SPECS, ids=lambda s: s.name)
def test_smoke_acelib(spec):
    """ResNet FHE inference on acelib-cuda."""
    if not _acelib_available():
        pytest.skip("acelib-cuda compiler not available")
    sample = _load_sample_image(spec)
    compiled = _compile_resnet(spec, sample, library="acelib", device="cuda")
    _assert_fhe_matches_plaintext(compiled, spec, sample)