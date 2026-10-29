# tests/regression/lenet_x2/test_lenet_x2_smoke.py
"""
LeNet x^2 (square activation) FHE precision smoke tests.

1:1 mirror of the resnet20_x2 precision validation, against
the reference experiment (cti lenet_x2.py): the model
carries UNTRAINED seeded weights and a seeded random input is
fed to both the plaintext and the ciphertext paths.

The precision metric is ABSOLUTE per-logit error
(|fhe_logits - plain_logits|), NOT relative error (|diff|/|expect|):
x^2 turns each ReLU into a cipher*cipher MUL, and with
untrained+BN-calibrated random init the logits land at
|expect|≈0.1 — small enough that the cti reference rel_err
formula amplifies the CKKS absolute noise floor (~1e-4) into
"noise" that does not exist. The x^2 experiment measures PURE
CKKS noise, so we use the absolute error directly.

The x^2 activation is a plain cipher*cipher MUL, so no ReLU
polynomial approximation is in the path — the residual
error is the CKKS absolute noise floor.

Compile options are exactly the standard LeNet recipe minus the
ReLU VR knobs (relu_vr_file) — x^2 is a pure activation swap.

Backends: ant (CPU) — mirror of the resnet20_x2 route.
phantom (CUDA) is NOT covered: the pool-free LeNet
structure compiles standalone on phantom, but the
cipher*cipher Mul chain behind x^2 does not reach the
executor yet — phantom compilation fails today with
"air_infra node.cxx: Do not call this API to change
rtype" + "ctx_param_ana.h: Input cipher level must be
at least 18". Add a phantom test once the Mul chain
is supported there.

Run:
    pytest tests/regression/lenet_x2/test_lenet_x2_smoke.py -v -s
"""

import copy

import pytest
import torch
from utils import requires_gpu

from ace import fhe
from ace.model.spec_lenet_x2 import LENET_X2


def _phantom_available():
    try:
        from ace.fhe.backend import get_library_impl
        return get_library_impl("phantom", device="cuda").check_available()
    except Exception:
        return False


# Absolute error tolerance. 1e-3 gives ~10x margin over the empirical
# CKKS noise floor (~1e-4 absolute on this Mul depth / q0 / sf / N),
# matching the cti harness tolerance intended against trained logits
# (~±10) where the same absolute floor shows as rel_err ≈ 1e-5.
ABS_ERROR_TOL = 1e-3


def _run_precision(spec, library="ant", device="cpu"):
    """Compile at x^2, run ciphertext + plaintext, print per-logit
    abs err, assert all below the harness tolerance."""
    model = spec.create_model()
    model.eval()

    probe = spec.example_inputs[0]

    compile_kwargs = copy.deepcopy(spec.compile_options[library])
    # No relu_vr_file: the model has no ReLU.

    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
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


@pytest.mark.slow
@requires_gpu
def test_lenet_x2_precision_phantom():
    """Ciphertext vs plaintext logit ABS err on random input (phantom-cuda)."""
    if not _phantom_available():
        pytest.skip("phantom-cuda compiler not available")
    _run_precision(LENET_X2, library="phantom", device="cuda")


@pytest.mark.slow
def test_lenet_x2_precision_ant():
    """Ciphertext vs plaintext logit ABS err on random input (ant-cpu).

    See module docstring: x² measures CKKS ABSOLUTE noise; we use
    abs err rather than |diff|/|expect| so the tiny BN-calibrated
    untrained logits don't dominate the metric.
    """
    _run_precision(LENET_X2)