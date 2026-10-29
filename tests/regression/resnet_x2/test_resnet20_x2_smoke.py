# tests/regression/resnet_x2/test_resnet20_x2_smoke.py
"""
ResNet-20 x^2 (square activation) FHE precision smoke test (ant-cpu).

1:1 mirror of the reference validation (cti resnet20_cifar10_x2): the
model carries UNTRAINED weights (BN running stats deterministically
calibrated) and a random input is fed to both the plaintext and the ant
ciphertext paths.

The precision metric is ABSOLUTE per-logit error
(|fhe_logits - plain_logits|), NOT relative error (|diff|/|expect|):
x^2 turns each ReLU into a cipher*cipher MUL, and with untrained
random init the BN-calibrated logits land at |expect|≈0.1 — small
enough that the cti reference rel_err formula amplifies the CKKS
absolute noise floor (~1e-4 absolute) into "noise" that does not
exist. The x^2 experiment measures PURE CKKS noise, so we use the
absolute error directly.

The x^2 activation is a plain cipher*cipher MUL, so no ReLU
polynomial approximation is in the path — the residual error is
the CKKS absolute noise floor.

Compile options are exactly the standard RESNET20_CIFAR10 ant recipe —
x^2 is a pure activation swap.

ant-cpu inference of the un-bootstrapped x^2 chain is heavy, so this
test is @pytest.mark.slow.

Run:
    pytest tests/regression/resnet_x2/test_resnet20_x2_smoke.py -v -s
"""

import copy

import pytest
import torch
from utils import requires_gpu

from ace import fhe
from ace.model.spec_resnet_x2 import RESNET20_X2


def _phantom_available():
    try:
        from ace.fhe.backend import get_library_impl
        return get_library_impl("phantom", device="cuda").check_available()
    except Exception:
        return False


# Absolute error tolerance. The x^2 forward is pure cipher*cipher MUL
# chain, so the per-logit residual is the CKKS absolute noise floor
# (~1e-4 absolute on this Mul depth / q0 / sf / N). 1e-3 gives ~10x
# margin and matches the cti reference harness's REL_ERROR=1e-3
# default intended against trained logits (~±10), where the same
# absolute floor shows as rel_err ≈ 1e-5.
ABS_ERROR_TOL = 1e-3


@pytest.mark.slow
def test_resnet20_x2_precision_ant():
    """Ciphertext vs plaintext logit ABS err on random input (ant-cpu).

    See module docstring: x^2 measures CKKS ABSOLUTE noise; we use
    abs err rather than |diff|/|expect| so the tiny BN-calibrated
    untrained logits don't dominate the metric.
    """
    spec = RESNET20_X2
    model = spec.create_model()
    model.eval()

    probe = spec.example_inputs[0]

    compile_kwargs = copy.deepcopy(spec.compile_options["ant"])
    # No relu_vr_file: the model has no ReLU.

    compiled = fhe.compile(
        frontend="torch",
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


@pytest.mark.slow
@requires_gpu
def test_resnet20_x2_precision_phantom():
    """Ciphertext vs plaintext logit ABS err on random input (phantom-cuda)."""
    if not _phantom_available():
        pytest.skip("phantom-cuda compiler not available")

    spec = RESNET20_X2
    model = spec.create_model()
    model.eval()

    probe = spec.example_inputs[0]

    compile_kwargs = copy.deepcopy(spec.compile_options["phantom"])

    compiled = fhe.compile(
        frontend="torch",
        library="phantom",
        device="cuda",
        encrypt_inputs=spec.encrypt_inputs,
        **compile_kwargs,
    )(model)
    prog = compiled.fhe_compile((probe,))

    runner = fhe.FHERuntime(prog.package)
    runner.init()

    with torch.no_grad():
        plain_logits = model(probe)

    fhe_out = runner.run(probe)
    runner.finalize()

    assert fhe_out is not None, "FHE inference returned None"
    assert isinstance(fhe_out, torch.Tensor), f"expected Tensor, got {type(fhe_out)}"

    fhe_logits = fhe_out.flatten().float()
    expect = plain_logits.flatten().float()

    assert not torch.isnan(expect).any(), "plaintext output contains NaN"
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