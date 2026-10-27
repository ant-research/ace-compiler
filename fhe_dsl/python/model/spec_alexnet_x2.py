#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AlexNet x^2 (square activation) ModelSpec for FHE precision testing.

1:1 mirror of the reference experiment (cti
verify/onnx2c/cnn/alexnet_cifar10_x2.py): ReLU -> x^2 at all 7
sites, UNTRAINED weights (default torch init — measured to keep
float32 activations finite, no BN calibration needed for a
BN-less network), and compile options EXACTLY the standard
ALEXNET_CIFAR10 ant recipe, with only the ReLU VR knobs removed —
x^2 is a pure activation swap and must not drag parameter changes
with it.

The FHE precision test feeds the seeded random input to both the
plaintext and the ant ciphertext paths and compares the per-logit
absolute error (|fhe - plain|, NOT |fhe - plain|/|plain|): the x²
chain measures PURE CKKS noise, and the cti reference rel_err
formula `|diff|/|expect|` was designed against trained logits
(~±10) — it explodes into noise on the BN-calibrated untrained
logits the x² setup produces.

Key differences vs the standard AlexNet spec:
- No ReLU -> no VR profile (relu_vr_file / relu_vr) is used or needed.
- No weights file: the model runs with its seeded random init; both
  execution paths see identical parameters.

The standard AlexNet spec (spec_alexnet.py) is untouched.
"""

import torch

from ace.fhe.config.spec import ModelSpec

from .alexnet.alexnet_x2 import AlexNetX2


# Init seed fixed so both plaintext and ciphertext paths instantiate
# the SAME random parameters.
_X2_INIT_SEED = 2026


def _x2_example_inputs() -> tuple[torch.Tensor, ...]:
    """Deterministic random (non-image) example input.

    Seeded so compile tracing is reproducible; the smoke test reuses
    the same tensor as its probe.
    """
    g = torch.Generator().manual_seed(_X2_INIT_SEED)
    return (torch.randn(1, 3, 32, 32, generator=g).float(),)


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


ALEXNET_X2 = ModelSpec(
    name="alexnet_x2",
    model_class=AlexNetX2,
    example_inputs=_x2_example_inputs(),
    encrypt_inputs=["x"],
    model_init_args=(10, 3),
    model_init_kwargs={},
    # untrained random init, deterministic seeding (no BN to calibrate)
    model_post_init=_init_x2_model,
    compile_options={
        # ant (CPU) — EXACTLY the standard ALEXNET_CIFAR10 ant recipe
        # (see spec_alexnet.py) minus the ReLU VR knobs (no ReLU).
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "N": 131072, "icl": 17, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"conv_parl": True, "ssf": True, "ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
    },
    expected_ops=["Conv", "AveragePool", "Mul", "Gemm"],
    weights_required=False,
    relu_vr_file=None,
)

__all__ = [
    "ALEXNET_X2",
]
