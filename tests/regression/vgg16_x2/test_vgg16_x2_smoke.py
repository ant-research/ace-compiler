# tests/regression/vgg16_x2/test_vgg16_x2_smoke.py
"""
VGG16 x^2 (square activation) FHE precision smoke test (ant-cpu).

1:1 mirror of the reference validation (cti vgg16_cifar10_x2
via vgg_cifar_x2_model.py): the model carries UNTRAINED weights
(BN running stats deterministically calibrated) and a seeded
random input is fed to both the plaintext and the ant ciphertext
paths.

The precision metric is ABSOLUTE per-logit error
(|fhe_logits - plain_logits|), NOT relative error (|diff|/|expect|):
x^2 turns each ReLU into a cipher*cipher MUL, and with
untrained+BN-calibrated random init the logits land at
|expect|≈0.01–0.04 — small enough that the cti reference rel_err
formula amplifies the CKKS absolute noise floor (~1e-4) into
"noise" that does not exist. The x^2 experiment measures PURE
CKKS noise, so we use the absolute error directly.

The x^2 activation is a plain cipher*cipher MUL, so no ReLU
polynomial approximation is in the path — the residual error is
the CKKS absolute noise floor.

Compile options are exactly the standard VGG16 ant recipe minus the
ReLU VR knobs (relu_vr_file / relu_vr) — x^2 is a pure activation
swap.

Frontend is "torch-via-onnx", matching the standard VGG16 route
(the direct torch->AIR vectorizer fails from the 4th conv block
onward).

ant-cpu inference of VGG16 depth is very heavy (the reference
build declares ~730 GB RAM class), so this test is
@pytest.mark.slow.

Run:
    pytest tests/regression/vgg16_x2/test_vgg16_x2_smoke.py -v -s
"""

import copy

import pytest
import torch

from ace import fhe
from ace.model.spec_vgg16_x2 import VGG16_X2


# Absolute error tolerance. VGG16 x² has 13 Squares → mul_depth=29,
# slightly higher CKKS noise than ResNet20 (=19) / LeNet (=3).
# 1e-3 gives ~10x margin over the empirical floor and matches the
# cti harness tolerance intended for trained logits (~±10); the
# same floor shows as rel_err ≈ 1e-5 against trained-output scale.
ABS_ERROR_TOL = 1e-3


@pytest.mark.slow
def test_vgg16_x2_precision_ant():
    """Ciphertext vs plaintext logit ABS err on random input (ant-cpu).

    See module docstring: x² measures CKKS ABSOLUTE noise; we use
    abs err rather than |diff|/|expect| so the tiny BN-calibrated
    untrained logits don't dominate the metric.
    """
    spec = VGG16_X2
    model = spec.create_model()
    model.eval()

    probe = spec.example_inputs[0]

    compile_kwargs = copy.deepcopy(spec.compile_options["ant"])
    # No relu_vr_file: the model has no ReLU.

    compiled = fhe.compile(
        frontend="torch-via-onnx",
        library="ant",
        device="cpu",
        encrypt_inputs=spec.encrypt_inputs,
        **compile_kwargs,
    )(model)
    prog = compiled.fhe_compile((probe,))

    runner = fhe.FHERuntime(prog.package)
    runner.init()

    # Plaintext reference on the same model instance (identical weights)
    with torch.no_grad():
        plain_logits = model(probe)

    fhe_out = runner.run(probe)
    runner.finalize()

    assert fhe_out is not None, "FHE inference returned None"
    assert isinstance(fhe_out, torch.Tensor), f"expected Tensor, got {type(fhe_out)}"

    fhe_logits = fhe_out.flatten().float()
    expect = plain_logits.flatten().float()

    assert not torch.isnan(expect).any(), "plaintext output contains NaN (BN calibration broken?)"
    assert not torch.isnan(fhe_logits).any(), "FHE output contains NaN"

    abs_errs = (fhe_logits - expect).abs().tolist()
    for i, (r, e, ae) in enumerate(zip(fhe_logits.tolist(), expect.tolist(), abs_errs)):
        status = "ok" if ae < ABS_ERROR_TOL else "FAILED"
        print(f"index: {i}, result: {r:+.6f}, expect: {e:+.6f}, abs err={ae:.2e}, {status}")

    max_abs = max(abs_errs)
    print(f"\n[summary] max abs err={max_abs:.2e} (tol {ABS_ERROR_TOL:g})")

    assert all(ae < ABS_ERROR_TOL for ae in abs_errs), (
        f"max abs err {max_abs:.2e} exceeds tolerance {ABS_ERROR_TOL:g}"
    )
