#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
VGG16 x^2 (square activation) ModelSpec for FHE precision testing.

1:1 mirror of the reference experiment (cti
verify/onnx2c/cnn/vgg16_cifar10_x2.py via vgg_cifar_x2_model.py):
ReLU -> x^2 at all 13 conv sites, UNTRAINED weights, and compile
options EXACTLY the standard VGG16_CIFAR10 ant recipe with only the
ReLU VR knobs removed — x^2 is a pure activation swap and must not
drag parameter changes with it.

The FHE precision test feeds the seeded random input to both the
plaintext and the ant ciphertext paths and compares the per-logit
absolute error (|fhe - plain|, NOT |fhe - plain|/|plain|): the x²
chain measures PURE CKKS noise, and the cti reference rel_err
formula `|diff|/|expect|` was designed against trained logits
(~±10) — it explodes into noise on the BN-calibrated untrained
logits the x² setup produces (VGG16 is the worst: |expect|·0.01).

Key differences vs the standard VGG16 spec:
- No ReLU -> no VR profile (relu_vr_file / relu_vr) is used or needed.
- No weights file: untrained seeded random init; both execution
  paths see identical parameters.
- Untrained BN running stats blow the 13-square float32 forward
  to inf/NaN — model_post_init runs the deterministic seeded BN
  calibration (same approach as the resnet20_x2 spec), after which
  eval logits are finite (~±0.1 scale observed for resnet20_x2).

The standard VGG16 spec (spec_vgg16.py) is untouched.
"""

import torch

from ace.fhe.config.spec import ModelSpec

from .vgg16.vgg16_x2 import VGG16X2


# Init seed fixed so both plaintext and ciphertext paths instantiate
# the SAME random parameters.
_X2_INIT_SEED = 2026


def _calibrate_and_eval(model: torch.nn.Module) -> None:
    """Deterministic reseeding + BN calibration + eval mode.

    Same approach as the resnet20_x2 spec: the model runs UNTRAINED
    and its BatchNorms still carry the default running stats
    (mean=0, var=1), which do nothing to tame the x^2 magnitude
    explosion — with 13 squares the float32 forward blows up.
    Pushing seeded batches through in train mode (running
    statistics only) captures the true activation distribution, after
    which eval forwards are bounded. Seeding makes plaintext and
    ciphertext paths see bit-wise identical parameters.
    """
    torch.manual_seed(_X2_INIT_SEED)
    for m in model.modules():
        if hasattr(m, "reset_parameters"):
            m.reset_parameters()
    g = torch.Generator().manual_seed(_X2_INIT_SEED + 1)
    model.train()
    with torch.no_grad():
        # 16 batches of 16 — keep this Shallow. Deeper BN calib
        # (256 batches) inflates some BN running_vars by 1.28e+01
        # (std lifts from 0.5–0.9 to 3.4–5.5), which scales up the
        # intermediate activations and therefore the CKKS absolute
        # noise floor across all 13 Squares. 16 batches is the right
        # depth — finite logits, smallest working-set fit, and the
        # smallest absolute error band.
        for _ in range(16):
            model(torch.randn(16, 3, 32, 32, generator=g))
    model.eval()


def _x2_example_inputs() -> tuple[torch.Tensor, ...]:
    """Deterministic random (non-image) example input.

    Seeded so compile tracing is reproducible; the smoke test reuses
    the same tensor as its probe.
    """
    g = torch.Generator().manual_seed(_X2_INIT_SEED)
    return (torch.randn(1, 3, 32, 32, generator=g).float(),)


VGG16_X2 = ModelSpec(
    name="vgg16_x2",
    model_class=VGG16X2,
    example_inputs=_x2_example_inputs(),
    encrypt_inputs=["x"],
    model_init_args=(),
    model_init_kwargs={"num_classes": 10},
    # untrained random init + deterministic BN calibration (see
    # _calibrate_and_eval) so x^2 magnitudes stay finite.
    model_post_init=_calibrate_and_eval,
    compile_options={
        # ant (CPU) — EXACTLY the standard VGG16_CIFAR10 ant recipe
        # (N=131072 — the ant runtime encoder rejects N=65536 here,
        # same as the standard VGG16) minus the ReLU VR knobs.
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "N": 131072, "icl": 17, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"conv_parl": True, "ssf": True, "ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
    },
    expected_ops=["Conv", "BatchNormalization", "AveragePool", "Mul", "Gemm"],
    weights_required=False,
    relu_vr_file=None,
)

__all__ = [
    "VGG16_X2",
]
