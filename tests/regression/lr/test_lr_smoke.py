"""
Logistic Regression FHE inference smoke tests.

Follows the ResNet smoke test route
(tests/regression/resnet/test_resnet_smoke.py) but for the minimal LR
model: a single GEMM with no hidden activation.

Backends: ant (CPU) and phantom (CUDA). ace-CUDA is intentionally not
covered — like other single-GEMM ops it hits "ace-library inference error"
(see test_sample_smoke_ace.py BROKEN_SMOKE_OPS).

Run examples:
    pytest tests/regression/lr/test_lr_smoke.py -k "ant"         # ant-cpu
    pytest tests/regression/lr/test_lr_smoke.py -k "phantom"     # phantom-cuda
"""

import pytest
import torch
from utils import requires_gpu

from ace import fhe
from ace.model.spec_lr import ALL_LR_SPECS


def _phantom_available():
    try:
        from ace.fhe.backend import get_library_impl

        return get_library_impl("phantom", device="cuda").check_available()
    except Exception:
        return False


def _compile_lr(spec, library="ant", device="cpu"):
    """Compile an LR model on the given backend.

    LR has no ReLU, so no VR profile is needed — lean on backend defaults.

    IMPORTANT: LR weights are random at init and are NOT re-instantiated the
    same way twice. The plaintext reference MUST use the exact model instance
    that was compiled, so this helper returns (prog, model).

    NOTE: use an explicit FHERuntime for ciphertext inference (the strongest
    evidence that the encrypted path runs and matches plaintext), like the
    sample smoke tests. The compile-wrapper call (compiled(x)) can disagree
    with plaintext for a single GEMM (prog path), so we drive FHERuntime.run.
    """
    model = spec.create_model()
    model.eval()

    kwargs = {}
    if spec.compile_options and library in spec.compile_options:
        kwargs.update(spec.compile_options[library])

    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        **kwargs,
    )(model)

    prog = compiled.fhe_compile(spec.example_inputs)
    return prog, model


def _assert_fhe_matches_plaintext(prog, model, sample_input, spec_name):
    """Run ciphertext (FHERuntime) and plaintext inference, assert agreement.

    ``model`` must be the SAME instance used at compile time (LR weights are
    random each create_model()).
    """
    with torch.no_grad():
        plain_output = model(sample_input)
    plain_pred = plain_output.argmax(dim=1).item()

    runner = fhe.FHERuntime(prog.package)
    fhe_output = runner.run(sample_input)
    runner.finalize()

    assert fhe_output is not None, f"{spec_name}: FHE inference returned None"
    assert isinstance(fhe_output, torch.Tensor), f"{spec_name}: expected Tensor, got {type(fhe_output)}"

    fhe_pred = fhe_output.flatten().argmax().item()
    print(f"[{spec_name}] plaintext={plain_pred}, fhe={fhe_pred}")
    assert fhe_pred == plain_pred, f"{spec_name}: FHE prediction {fhe_pred} != plaintext {plain_pred}"


# ============================================================================
# Ant (CPU) - slow
# ============================================================================


@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_LR_SPECS, ids=lambda s: s.name)
def test_smoke_ant(spec):
    """LR FHE inference on ant-cpu (slow)."""
    prog, model = _compile_lr(spec, library="ant", device="cpu")
    _assert_fhe_matches_plaintext(prog, model, spec.example_inputs[0], spec.name)


# ============================================================================
# Phantom (CUDA)
# ============================================================================


@requires_gpu
@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_LR_SPECS, ids=lambda s: s.name)
def test_smoke_phantom(spec):
    """LR FHE inference on phantom-cuda."""
    if not _phantom_available():
        pytest.skip("phantom-cuda compiler not available")
    prog, model = _compile_lr(spec, library="phantom", device="cuda")
    _assert_fhe_matches_plaintext(prog, model, spec.example_inputs[0], spec.name)


# ============================================================================
# Ace (CUDA)
# ============================================================================


