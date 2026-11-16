"""
SqueezeNet FHE inference smoke tests.

SqueezeNet is the lightweight fire-module CNN (26 conv + 18 relu + 8 concat +
4 avg-pool + global-avg-pool → linear). Same two deviations from the
ResNet/LeNet smoke route as AlexNet/VGG16:

- **ant (CPU) only.** SqueezeNet's 2×2 AveragePool2d blocks and Concat ops are
  unsupported by the phantom / ace GPU executors.
- **frontend="torch-via-onnx".** The direct torch→AIR path hits
  ``vector_gen.cxx:196: roll num attribute should not be empty`` on deeper
  conv graphs; routing the torch model through ONNX (torch → .onnx → AIR)
  compiles the full model, and the reference squeezenet_cifar10 build also
  goes through ONNX.

ant-CPU inference is heavy, so this test is ``@pytest.mark.slow``.

Run examples:
    pytest tests/regression/squeezenet/test_squeezenet_smoke.py -v
"""

import copy

import pytest
import torch

from ace import fhe
from ace.model.spec_squeezenet import ALL_SQUEEZENET_SPECS


def _compile_squeezenet(spec, library="ant", device="cpu"):
    """Compile a SqueezeNet model via the torch-via-onnx frontend.

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
    # spec_squeezenet.py). Passing the profile path here keeps the call shape
    # identical to the AlexNet/VGG16 smoke tests and is a harmless no-op.
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
# Ant (CPU) — only supported backend for pooling/concat SqueezeNet
# ============================================================================


@pytest.mark.slow
@pytest.mark.parametrize("spec", ALL_SQUEEZENET_SPECS, ids=lambda s: s.name)
def test_smoke_ant(spec):
    """SqueezeNet FHE inference on ant-cpu."""
    prog, model = _compile_squeezenet(spec, library="ant", device="cpu")
    _assert_fhe_matches_plaintext(prog, model, spec.example_inputs[0], spec.name)
