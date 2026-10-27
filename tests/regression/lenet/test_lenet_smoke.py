"""
LeNet FHE inference smoke tests.

Follows the ResNet smoke test route (tests/regression/resnet/test_resnet_smoke.py)
for the LeNet CNN: uses the pre-profiled ReLU VR data (lenet/profiles/*.json)
via ModelSpec::get_vr_profile(), compiles on ant (CPU) and phantom (CUDA),
then asserts FHE argmax matches plaintext on the SAME model instance.

Backends: ant (CPU) and phantom (CUDA). ace-CUDA is intentionally not
covered (single-GEMM/conv inference hits known ace errors).

Run examples:
    pytest tests/regression/lenet/test_lenet_smoke.py -k "phantom"     # phantom only
    pytest tests/regression/lenet/test_lenet_smoke.py -k "ant"         # ant-cpu (slow)
    pytest tests/regression/lenet/test_lenet_smoke.py -m "not slow"    # skip slow (ant) tests
"""

import copy
import pytest
import torch
from utils import requires_gpu

from ace import fhe
from ace.model.spec_lenet import ALL_LENET_SPECS


def _phantom_available():
    try:
        from ace.fhe.backend import get_library_impl

        return get_library_impl("phantom", device="cuda").check_available()
    except Exception:
        return False


def _compile_lenet(spec, library="ant", device="cpu"):
    """Compile a LeNet model, reusing the pre-profiled VR data from ModelSpec.

    Follows the ResNet route: weights_required=True so get_vr_profile()
    auto-discovers lenet/profiles/{name}.json; falls back to profile_relu
    if no profile file is available.

    Returns (prog, model) — model is the SAME instance compiled, so
    plaintext uses identical weights.
    """
    model = spec.create_model()
    model.eval()

    kwargs = {}
    if spec.compile_options and library in spec.compile_options:
        kwargs = copy.deepcopy(spec.compile_options[library])

    # Built-in ReLU VR profiling: the compiler measures per-node ranges from
    # the example inputs at build time, so node-name matching is guaranteed.
    # (The bundled lenet/profiles/*.json file exists for reference, but its
    # node names do not line up with the compiled IR, so file-based VR makes
    # phantom disagree with plaintext — see git history.)
    kwargs["profile_relu"] = True

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
    """Run ciphertext (FHERuntime) and plaintext inference, assert class match."""
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
# Phantom (CUDA) — user's priority
# ============================================================================


@requires_gpu
@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_LENET_SPECS, ids=lambda s: s.name)
def test_smoke_phantom(spec):
    """LeNet FHE inference on phantom-cuda."""
    if not _phantom_available():
        pytest.skip("phantom-cuda compiler not available")
    prog, model = _compile_lenet(spec, library="phantom", device="cuda")
    _assert_fhe_matches_plaintext(prog, model, spec.example_inputs[0], spec.name)


# ============================================================================
# Ant (CPU) - slow
# ============================================================================


@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_LENET_SPECS, ids=lambda s: s.name)
def test_smoke_ant(spec):
    """LeNet FHE inference on ant-cpu (slow)."""
    prog, model = _compile_lenet(spec, library="ant", device="cpu")
    _assert_fhe_matches_plaintext(prog, model, spec.example_inputs[0], spec.name)