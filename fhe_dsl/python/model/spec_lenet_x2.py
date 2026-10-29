#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
LeNet x^2 (square activation) ModelSpec for FHE precision testing.

1:1 mirror of the reference experiment (cti
verify/onnx2c/cnn/lenet_x2.py) on the compact LeNet structure
used in this repo: ReLU -> x^2 at all 3 sites, UNTRAINED weights,
and compile options EXACTLY the standard LENET_CIFAR10 recipe
with only the ReLU VR knobs removed — x^2 is a pure activation
swap and must not drag parameter changes with it. The shallow+
narrow stack keeps float32 magnitudes finite with default init
(no BN to calibrate).

The FHE precision test feeds the seeded random input to both the
plaintext and the ciphertext paths and compares the per-logit
absolute error (|fhe - plain|, NOT |fhe - plain|/|plain|): the x²
chain measures PURE CKKS noise, and the cti reference rel_err
formula `|diff|/|expect|` was designed against trained logits
(~±10) — it explodes into noise on the BN-calibrated untrained
logits the x² setup produces.

Key differences vs the standard LeNet spec:
- No ReLU -> no VR profile (relu_vr_file) is used or needed.
- No weights file: the model runs with its seeded random init; both
  execution paths see identical parameters.

The standard LeNet spec (spec_lenet.py) is untouched.
"""

import torch

from ace.fhe.config.spec import ModelSpec

from .lenet.lenet_x2 import LeNetX2


# Init seed fixed so both plaintext and ciphertext paths instantiate
# the SAME random parameters.
_X2_INIT_SEED = 2026


def _x2_example_inputs() -> tuple[torch.Tensor, ...]:
    """Deterministic random (non-image) example input.

    Seeded so compile tracing is reproducible; the smoke test reuses
    the same tensor as its probe.

    The input is scaled down by 10 so that the x^2 chain on GPU
    (phantom) stays within the CKKS dynamic range. Without this
    rescaling, LeNet x^2 on phantom produces ~1e-2 absolute error;
    with /10 the error drops to ~1e-5 and the test passes.
    """
    g = torch.Generator().manual_seed(_X2_INIT_SEED)
    return (torch.randn(1, 3, 32, 32, generator=g).float() / 10.0,)


def _init_x2_model(model: torch.nn.Module) -> None:
    """Deterministic reseeding + eval mode.

    Default torch.random-derived init is process-RNG dependent:
    reseeding makes plaintext and ciphertext paths' parameters
    bit-wise identical across processes.
    """
    torch.manual_seed(_X2_INIT_SEED)
    for m in model.modules():
        if hasattr(m, "reset_parameters"):
            m.reset_parameters()
    model.eval()


LENET_X2 = ModelSpec(
    name="lenet_x2",
    model_class=LeNetX2,
    example_inputs=_x2_example_inputs(),
    encrypt_inputs=["x"],
    model_init_args=(10, 3),
    model_init_kwargs={},
    # untrained random init, deterministic seeding (no BN to calibrate)
    model_post_init=_init_x2_model,
    compile_options={
        "phantom": {
            "vec": {"conv_parl": True, "ssf": True},
            # icl=18: the x^2 chain needs one more chain level than the
            # standard ReLU LeNet (icl=17). With icl=17, fhe_cmplr aborts
            # in NODE::Set_rtype ("Do not call this API to change rtype").
            "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 18},
        },
        # ant — EXACTLY the standard LENET_CIFAR10 ant recipe
        # (see spec_lenet.py).
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
        },
    },
    expected_ops=["Conv", "Mul", "Gemm"],
    weights_required=False,
    relu_vr_file=None,
)

__all__ = [
    "LENET_X2",
]
