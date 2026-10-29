#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 x^2 (square activation) ModelSpec for FHE precision testing.

1:1 mirror of the reference implementation (cti
verify/onnx2c/cnn/resnet_cifar10_x2_model.py): ReLU -> x^2 everywhere
(19 squares incl. post-add), conv biases kept, UNTRAINED weights, and
compile options EXACTLY the standard RESNET20_CIFAR10 ant recipe,
unmodified — x^2 is a pure activation swap and must not drag parameter
changes with it.

The FHE precision test feeds a random (non-image) input to both the
plaintext and the ant ciphertext paths and compares the per-logit
absolute error (|fhe - plain|, NOT |fhe - plain|/|plain|): the x²
chain measures PURE CKKS noise, and the cti reference rel_err
formula `|diff|/|expect|` was designed against trained logits
(~±10) — it explodes into noise on the BN-calibrated untrained
logits the x² setup produces.

Key differences vs the standard ResNet spec:
- No ReLU -> no VR profile (relu_vr_file) is used or needed.
- No weights file: the model runs with its seeded random init; both
  execution paths see identical parameters.
- ant (CPU) backend only, per the reference flow. (The cti reference
  result this experiment mirrors was measured on the CPU stack.)

The standard resnet specs (spec_resnet.py) are untouched.
"""

import torch

from ace.fhe.config.spec import ModelSpec

from .resnet.resnet20_x2 import BasicBlock as BasicBlockX2
from .resnet.resnet20_x2 import ResNetX2 as ResNet20X2


# Init seed fixed so both plaintext and ciphertext paths instantiate the
# SAME random parameters.
_X2_INIT_SEED = 2026


def _calibrate_and_eval(model: torch.nn.Module) -> None:
    """Deterministic BN calibration + eval mode.

    The model runs UNTRAINED, and its BatchNorms still carry the default
    running stats (mean=0, var=1) — which do nothing to tame the x^2
    magnitude explosion: with 19 squares the float32 forwards blow up to
    inf/NaN (measured). Pushing a few seeded batches through in train
    mode (no grads — running statistics only) captures the true
    activation distribution, after which eval forwards are ~ +-0.1
    logits, matching the reference harness's expect-value scale.

    Weight initialization is already seeded by init_seed in ResNetX2.__init__;
    this hook only calibrates BN running stats.
    """
    g = torch.Generator().manual_seed(_X2_INIT_SEED + 1)
    model.train()
    with torch.no_grad():
        for _ in range(16):
            model(torch.randn(16, 3, 32, 32, generator=g))
    model.eval()


def _x2_example_inputs() -> tuple[torch.Tensor, ...]:
    """Deterministic random (non-image) example input.

    Seeded so compile tracing is reproducible; the smoke test reuses the
    same tensor as its probe.
    """
    g = torch.Generator().manual_seed(_X2_INIT_SEED)
    return (torch.randn(1, 3, 32, 32, generator=g).float(),)


RESNET20_X2 = ModelSpec(
    name="resnet20_x2",
    model_class=ResNet20X2,
    example_inputs=_x2_example_inputs(),
    encrypt_inputs=["x"],
    model_init_args=(BasicBlockX2, [3, 3, 3]),
    model_init_kwargs={"num_classes": 10, "init_seed": _X2_INIT_SEED + 2},
    # untrained random init (seeded via init_seed for reproducibility),
    # BN running-stats calibrated deterministically (see _calibrate_and_eval)
    # so x^2 magnitudes stay finite.
    model_post_init=_calibrate_and_eval,
    compile_options={
        # ant (CPU) — EXACTLY the standard RESNET20_CIFAR10 ant recipe,
        # unmodified (see spec_resnet.py). x^2 is a pure activation swap.
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "N": 65536, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"ts": True, "rtt": True},
            "sihe": {"ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
        "phantom": {
            "vec": {"conv_parl": True, "ssf": True},
            "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 17, "sbm": True},
        },
    },
    expected_ops=["Conv", "BatchNormalization", "Mul", "Add", "GlobalAveragePool", "Gemm"],
    weights_required=False,
    relu_vr_file=None,
)

__all__ = [
    "RESNET20_X2",
]