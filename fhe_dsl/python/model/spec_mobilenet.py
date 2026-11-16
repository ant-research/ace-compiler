#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
MobileNet ModelSpec instances for FHE compilation.

MobileNet is the depthwise-separable CNN (3×3 stem + 13 depthwise-separable
blocks + 2×2 avg-pool → linear; 27 conv / 27 relu) and follows the
AlexNet/VGG16/SqueezeNet route: weights_required=True makes
ModelSpec::get_vr_profile() auto-discover profiles/{name}.json for the ReLU
polynomial approximation, and model_post_init loads the fixed weights in
weights/mobilenet_cifar10.pt.

Those weights are trained on the REAL CIFAR-10 train set by
``model/mobilenet/train_mobilenet.py`` (there is no reference mobilenet ONNX
in this repo to import from, unlike AlexNet/VGG16/SqueezeNet). Plaintext
top-1 is 88.95% on the full CIFAR-10 test set.

Backend scope is **ant (CPU) only**: pooling (and concat) is supported by the
antlib CPU runtime but not by the phantom / ace GPU executors, same as
AlexNet/VGG16/SqueezeNet.

Frontend note: compile this model with ``frontend="torch-via-onnx"``, not the
default ``"torch"``. The direct torch→AIR conv vectorizer rejects deeper CNN
graphs (``vector_gen.cxx:196: roll num attribute should not be empty``);
routing the torch model through ONNX avoids it (the depthwise convs'
``groups`` attribute round-trips through ONNX fine — verified: group=32/64/
128/256/512/1024 on the 13 depthwise convs).
"""

import os

import torch

from ace.fhe.config.spec import ModelSpec

from .mobilenet import MobileNet


_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "mobilenet", "weights")
_MOBILENET_WEIGHTS = os.path.join(_WEIGHTS_DIR, "mobilenet_cifar10.pt")

# Deterministic CIFAR-10 test image used as the compile/probe example.
# offset=0 is class 3 with a ~4.5-logit plaintext margin, so CKKS noise /
# ReLU approximation cannot flip the argmax.
_MOBILENET_EXAMPLE_OFFSET = 0


def _mobilenet_example_input() -> torch.Tensor:
    """A real, normalized CIFAR-10 image as the compile/probe example input.

    The shipped weights were trained on real CIFAR-10 with the same
    normalization as ``load_cifar10_images`` (CIFAR statistics).
    """
    from ace.model.dataset import load_cifar10_images

    images, _ = load_cifar10_images(1, offset=_MOBILENET_EXAMPLE_OFFSET)
    return images[0:1]


def _load_mobilenet_weights(model: torch.nn.Module) -> None:
    """Load the fixed MobileNet weights into the model (in-place) and eval."""
    if not os.path.exists(_MOBILENET_WEIGHTS):
        raise FileNotFoundError(
            f"MobileNet weights not found: {_MOBILENET_WEIGHTS}. Generate them: "
            "python3 -m model.mobilenet.train_mobilenet "
            "(from fhe_dsl/python)"
        )
    state = torch.load(_MOBILENET_WEIGHTS, map_location="cpu", weights_only=True)
    model.load_state_dict(state)
    model.eval()


# =============================================================================
# ReLU value ranges
# =============================================================================

# Per-node ReLU VR values, profiled over the shipped weights
# (weights/mobilenet_cifar10.pt) on the full CIFAR-10 test set (10k images)
# as PRE-ReLU input abs_max; VR = int(abs_max) + 1.
#
# KEY NAMING: the torch-via-onnx frontend wraps the module's forward in
# functools.wraps(model.forward) and exports with the legacy TorchScript ONNX
# exporter (dynamo=False). That does NOT preserve torch module scopes — Relu
# nodes come out as "/Relu", "/Relu_1", ... in graph order. These ARE the AIR
# node names the SIHE backend matches against
# (compiler/fhe-cmplr/include/fhe/sihe/config.h Parse_relu_vr →
# tensor2sihe_impl.h ctx.Has_relu_vr(node->Attr("name"))). Same scheme as
# SqueezeNet (spec_squeezenet.py).
#
# These values MUST match profiles/mobilenet_cifar10.json (regenerate both
# at once: `python3 -m model.mobilenet.train_mobilenet --profile-only`).
# node          site                             pre-ReLU abs_max (10k)
_ORIGIN_RELU_VR = {
    "/Relu": 5.0,      # conv1 (stem)                4.42
    "/Relu_1": 10.0,   # block0 depthwise BN          9.76
    "/Relu_2": 8.0,    # block0 pointwise BN          7.83
    "/Relu_3": 12.0,   # block1 depthwise BN         11.54
    "/Relu_4": 8.0,    # block1 pointwise BN          7.71
    "/Relu_5": 12.0,   # block2 depthwise BN         11.16
    "/Relu_6": 10.0,   # block2 pointwise BN          9.84
    "/Relu_7": 9.0,    # block3 depthwise BN          8.97
    "/Relu_8": 7.0,    # block3 pointwise BN          6.14
    "/Relu_9": 9.0,    # block4 depthwise BN          8.79
    "/Relu_10": 5.0,   # block4 pointwise BN          4.45
    "/Relu_11": 7.0,   # block5 depthwise BN          6.29
    "/Relu_12": 5.0,   # block5 pointwise BN          4.01
    "/Relu_13": 7.0,   # block6 depthwise BN          6.28
    "/Relu_14": 4.0,   # block6 pointwise BN          3.75
    "/Relu_15": 7.0,   # block7 depthwise BN          6.46
    "/Relu_16": 5.0,   # block7 pointwise BN          4.08
    "/Relu_17": 7.0,   # block8 depthwise BN          6.44
    "/Relu_18": 5.0,   # block8 pointwise BN          4.27
    "/Relu_19": 7.0,   # block9 depthwise BN          7.00
    "/Relu_20": 5.0,   # block9 pointwise BN          4.42
    "/Relu_21": 7.0,   # block10 depthwise BN         6.59
    "/Relu_22": 5.0,   # block10 pointwise BN         4.77
    "/Relu_23": 8.0,   # block11 depthwise BN         7.07
    "/Relu_24": 5.0,   # block11 pointwise BN         4.42
    "/Relu_25": 6.0,   # block12 depthwise BN         5.49
    "/Relu_26": 3.0,   # block12 pointwise BN         2.66
}
_ORIGIN_RELU_VR_DEF = 3.0


# =============================================================================
# MobileNet Specs
# =============================================================================

# CIFAR-shaped input [1, 3, 32, 32] — canonical CIFAR MobileNet.
MOBILENET_CIFAR10 = ModelSpec(
    name="mobilenet_cifar10",
    model_class=MobileNet,
    example_inputs=(_mobilenet_example_input(),),
    encrypt_inputs=["x"],
    model_init_args=(10, 3),
    model_init_kwargs={},
    model_post_init=_load_mobilenet_weights,
    compile_options={
        # ant (CPU) only — pooling is unsupported on the GPU executors.
        # Same CKKS recipe as AlexNet/VGG16/SqueezeNet (N=131072, q0=60,
        # sf=56) with sbm + ts + rtt.
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "N": 131072, "icl": 17, "sbm": True, "ts": True, "rtt": True},
            # relu_vr: per-node VR map (see _ORIGIN_RELU_VR above) → fhe_cmplr
            # '-SIHE:relu_vr_def=3:relu_vr=<name>=<vr>;...' when generating .c.
            "sihe": {"relu_vr_def": _ORIGIN_RELU_VR_DEF, "relu_vr": _ORIGIN_RELU_VR, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
            "o2a": {"ts": True},
            "fhe_scheme": {"ts": True},
            "vec": {"conv_parl": True, "ssf": True, "ts": True, "rtt": True},
            "poly": {"ts": True, "rtt": True},
        },
    },
    expected_ops=["Conv", "Relu", "AveragePool", "Gemm"],
    weights_required=True,
    dataset="cifar10",
)


# =============================================================================
# Spec Collections
# =============================================================================

MOBILENET_SPECS = [MOBILENET_CIFAR10]
ALL_MOBILENET_SPECS = MOBILENET_SPECS

__all__ = [
    "ALL_MOBILENET_SPECS",
    "MOBILENET_CIFAR10",
    "MOBILENET_SPECS",
]
