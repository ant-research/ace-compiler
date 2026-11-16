"""
MobileNet FHE inference smoke tests.

MobileNet is the depthwise-separable CNN (3×3 stem + 13 depthwise-separable
blocks + 2×2 avg-pool → linear; 27 conv / 27 relu). Same two deviations from
the ResNet/LeNet smoke route as AlexNet/VGG16/SqueezeNet:

- **ant (CPU) only.** MobileNet's 2×2 AveragePool2d is unsupported by the
  phantom / ace GPU executors.
- **frontend="torch-via-onnx".** The direct torch→AIR path hits
  ``vector_gen.cxx:196: roll num attribute should not be empty`` on deeper
  conv graphs; routing the torch model through ONNX (torch → .onnx → AIR)
  compiles the full model. The depthwise convs' ``groups`` attribute
  (groups=32..1024) round-trips through ONNX fine.

ant-CPU inference is heavy, so this test is ``@pytest.mark.slow``.

Run examples:
    pytest tests/regression/mobilenet/test_mobilenet_smoke.py -v
"""

import copy

import pytest
import torch

from ace import fhe
from ace.model.spec_mobilenet import ALL_MOBILENET_SPECS


def _compile_mobilenet(spec, library="ant", device="cpu"):
    """Compile a MobileNet model via the torch-via-onnx frontend.

    Returns (compiled_program, model) — model is the SAME instance compiled,
    so plaintext uses identical weights.
    """
    model = spec.create_model()
    model.eval()

    kwargs = {}
    if spec.compile_options and library in spec.compile_options:
        kwargs = copy.deepcopy(spec.compile_options[library])

    # The torch-via-onnx frontend ignores relu_vr_file, so the per-node VR map
    # must already be in compile_options["ant"]["sihe"]["relu_vr"] (see
    # spec_mobilenet.py). Passing the profile path here keeps the call shape
    # identical to the AlexNet/VGG16/SqueezeNet smoke tests and is a harmless
    # no-op.
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
# Ant (CPU) — only supported backend for avg-pool MobileNet
# ============================================================================


@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_MOBILENET_SPECS, ids=lambda s: s.name)
def test_smoke_ant(spec):
    """MobileNet FHE inference on ant-cpu."""
    prog, model = _compile_mobilenet(spec, library="ant", device="cpu")
    _assert_fhe_matches_plaintext(prog, model, spec.example_inputs[0], spec.name)
