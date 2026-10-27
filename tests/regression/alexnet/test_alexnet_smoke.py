"""
AlexNet FHE inference smoke tests.

AlexNet is the mid-depth CNN FHE test model (5 conv + 3 avg-pool → flatten →
3 linear). Two deliberate deviations from the LeNet smoke route, both shared
with VGG16:

- **ant (CPU) only.** AlexNet's 2×2 AveragePool2d layers are unsupported by
  the phantom / ace GPU executors (they segfault on pool ops — see
  tests/regression/sample/test_sample_smoke_{phantom,ace}.py). AlexNet was
  chosen over the reference torch model as-is; the GPU-unfriendly pool stays.
- **frontend="torch-via-onnx".** The direct torch→AIR path fails in the conv
  vectorizer (``vector_gen.cxx:196: roll num attribute should not be empty``).
  Routing the torch model through ONNX (torch → .onnx → AIR) compiles the
  full model; the reference alexnet_cifar10 build also goes through ONNX.

ant-CPU inference is heavy (deep/wide first Gemm 1024→4096), so this test is
``@pytest.mark.slow`` and may take several minutes.

Run examples:
    pytest tests/regression/alexnet/test_alexnet_smoke.py -v
"""

import copy

import pytest
import torch

from ace import fhe
from ace.model.spec_alexnet import ALL_ALEXNET_SPECS


def _compile_alexnet(spec, library="ant", device="cpu"):
    """Compile an AlexNet model via the torch-via-onnx frontend.

    Returns (compiled_program, model) — model is the SAME instance compiled,
    so plaintext uses identical weights.
    """
    model = spec.create_model()
    model.eval()

    kwargs = {}
    if spec.compile_options and library in spec.compile_options:
        kwargs = copy.deepcopy(spec.compile_options[library])

    # Pre-profiled VR file (from train_alexnet.py), matching the VGG16 route.
    vr_file = spec.get_vr_profile()
    if vr_file is not None:
        kwargs["relu_vr_file"] = vr_file

    compiled = fhe.compile(
        frontend="torch-via-onnx",
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
# Ant (CPU) — only supported backend for pooling-based AlexNet
# ============================================================================


@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_ALEXNET_SPECS, ids=lambda s: s.name)
def test_smoke_ant(spec):
    """AlexNet FHE inference on ant-cpu (slow; deep first Gemm 1024→4096)."""
    prog, model = _compile_alexnet(spec, library="ant", device="cpu")
    _assert_fhe_matches_plaintext(prog, model, spec.example_inputs[0], spec.name)