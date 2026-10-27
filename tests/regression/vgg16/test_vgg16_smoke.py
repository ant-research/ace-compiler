"""
VGG16 FHE inference smoke tests.

VGG16 is the deep-CNN FHE test model (13 conv + 5 avg-pool → flatten → linear).
Two deliberate deviations from the ResNet/LeNet smoke route:

- **ant (CPU) only.** VGG16's 2×2 AveragePool2d blocks are unsupported by the
  phantom / ace GPU executors (they segfault on pool ops — see
  tests/regression/sample/test_sample_smoke_{phantom,ace}.py). GPU VGG16 in
  this repo would have to drop pooling, which no longer makes it VGG16.
- **frontend="torch-via-onnx".** The direct torch→AIR path hits
  ``vector_gen.cxx:196: roll num attribute should not be empty`` from VGG16's
  4th conv block onward (blocks 1–3 compile; block 4+ does not). Routing the
  torch model through ONNX (torch → .onnx → AIR) compiles the full model; the
  reference vgg16_cifar10 build also goes through ONNX.

ant-CPU inference is very heavy (the reference build declares ~730 GB RAM for
vgg16_cifar10), so this test is ``@pytest.mark.slow`` and may take many minutes.

Run examples:
    pytest tests/regression/vgg16/test_vgg16_smoke.py -v
"""

import copy

import pytest
import torch

from ace import fhe
from ace.model.spec_vgg16 import ALL_VGG16_SPECS


def _compile_vgg16(spec, library="ant", device="cpu"):
    """Compile a VGG16 model via the torch-via-onnx frontend.

    Returns (compiled_program, model) — model is the SAME instance compiled,
    so plaintext uses identical weights.
    """
    model = spec.create_model()
    model.eval()

    kwargs = {}
    if spec.compile_options and library in spec.compile_options:
        kwargs = copy.deepcopy(spec.compile_options[library])

    # Pre-profiled VR file (from train_vgg16.py), matching the ResNet route.
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
# Ant (CPU) — only supported backend for pooling-based VGG16
# ============================================================================


@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_VGG16_SPECS, ids=lambda s: s.name)
def test_smoke_ant(spec):
    """VGG16 FHE inference on ant-cpu (very slow; ~730 GB RAM class)."""
    prog, model = _compile_vgg16(spec, library="ant", device="cpu")
    _assert_fhe_matches_plaintext(prog, model, spec.example_inputs[0], spec.name)