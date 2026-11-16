#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
SqueezeNet ModelSpec instances for FHE compilation.

SqueezeNet is the lightweight fire-module CNN (26 conv + 18 relu + 8 concat +
4 avg-pool + global-avg-pool + linear) and follows the AlexNet/VGG16 route:
weights_required=True makes ModelSpec::get_vr_profile() auto-discover
profiles/{name}.json for the ReLU polynomial approximation, and
model_post_init loads the fixed weights in weights/squeezenet_cifar10.pt.

Those weights are the real CIFAR-10 SqueezeNet, imported from the reference
``squeezenet_cifar10.onnx`` (ace-compiler-iiecas) by
``model/squeezenet/import_reference_weights.py`` — NOT the synthetic
rehearsal weights ``train_squeezenet.py`` can produce. Plaintext top-1 is
92.65% on the full CIFAR-10 test set, matching the reference ~93%.

Backend scope is **ant (CPU) only**: pooling (and concat) is supported by the
antlib CPU runtime but not by the phantom / ace GPU executors, same as
AlexNet/VGG16.

Frontend note: compile this model with ``frontend="torch-via-onnx"``, not the
default ``"torch"``. The direct torch→AIR conv vectorizer rejects deeper CNN
graphs (``vector_gen.cxx:196: roll num attribute should not be empty``);
routing the torch model through ONNX avoids it, and the reference
squeezenet_cifar10 build likewise goes through ONNX.
"""

import os

import torch

from ace.fhe.config.spec import ModelSpec

from .squeezenet import SqueezeNet


_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "squeezenet", "weights")
_SQUEEZENET_WEIGHTS = os.path.join(_WEIGHTS_DIR, "squeezenet_cifar10.pt")

# Deterministic CIFAR-10 test image used as the compile/probe example.
# offset=0 is class 3 with a ~14.6-logit plaintext margin, so CKKS noise /
# ReLU approximation cannot flip the argmax.
_SQUEEZENET_EXAMPLE_OFFSET = 0


def _squeezenet_example_input() -> torch.Tensor:
    """A real, normalized CIFAR-10 image as the compile/probe example input.

    The shipped weights were trained on real CIFAR-10 (imported from the
    reference ``squeezenet_cifar10.onnx``), so the example input must come
    from the same distribution as ``load_cifar10_images``.
    """
    from ace.model.dataset import load_cifar10_images

    images, _ = load_cifar10_images(1, offset=_SQUEEZENET_EXAMPLE_OFFSET)
    return images[0:1]


def _load_squeezenet_weights(model: torch.nn.Module) -> None:
    """Load the fixed SqueezeNet weights into the model (in-place) and eval."""
    if not os.path.exists(_SQUEEZENET_WEIGHTS):
        raise FileNotFoundError(
            f"SqueezeNet weights not found: {_SQUEEZENET_WEIGHTS}. Generate them: "
            "python3 -m model.squeezenet.import_reference_weights <squeezenet_cifar10.onnx> "
            "(from fhe_dsl/python)"
        )
    state = torch.load(_SQUEEZENET_WEIGHTS, map_location="cpu", weights_only=True)
    model.load_state_dict(state)
    model.eval()


# =============================================================================
# ReLU value ranges
# =============================================================================

# Per-node ReLU VR values, profiled over the shipped weights
# (weights/squeezenet_cifar10.pt) on the full CIFAR-10 test set (10k images)
# as PRE-ReLU input abs_max; VR = int(abs_max) + 1.
#
# KEY NAMING: the torch-via-onnx frontend wraps the module's forward in
# functools.wraps(model.forward) and exports with the legacy TorchScript ONNX
# exporter (dynamo=False). That does NOT preserve torch module scopes — Relu
# nodes come out as "/Relu", "/Relu_1", ... in graph order. These ARE the AIR
# node names the SIHE backend matches against
# (compiler/fhe-cmplr/include/fhe/sihe/config.h Parse_relu_vr →
# tensor2sihe_impl.h ctx.Has_relu_vr(node->Attr("name"))). Verified in the
# generated fhe_ant.cpp: with these keys every node uses its own 1/vr
# constant instead of relu_vr_def.
#
# Do NOT reuse the reference build's ranges (/conv1/conv1.2/Relu=2, def=2):
# they are keyed by the pre-onnxsim node names and were calibrated on the
# BN-equipped net; they never match the exporter's "/Relu_N" names.
#
# These values MUST match profiles/squeezenet_cifar10.json (regenerate both
# at once: `python3 -m model.squeezenet.train_squeezenet --profile-only`).
# node        site                       pre-ReLU abs_max (10k)
_ORIGIN_RELU_VR = {
    "/Relu": 2.0,      # conv1                     1.96
    "/Relu_1": 2.0,    # fire2 squeeze             1.85
    "/Relu_2": 3.0,    # fire2 expand              2.38
    "/Relu_3": 4.0,    # fire3 squeeze             3.30
    "/Relu_4": 4.0,    # fire3 expand              3.41
    "/Relu_5": 5.0,    # fire4 squeeze             4.36
    "/Relu_6": 3.0,    # fire4 expand              2.33
    "/Relu_7": 4.0,    # fire5 squeeze             3.19
    "/Relu_8": 3.0,    # fire5 expand              2.12
    "/Relu_9": 4.0,    # fire6 squeeze             3.12
    "/Relu_10": 3.0,   # fire6 expand              2.22
    "/Relu_11": 5.0,   # fire7 squeeze             4.20
    "/Relu_12": 3.0,   # fire7 expand              2.54
    "/Relu_13": 6.0,   # fire8 squeeze             5.80
    "/Relu_14": 2.0,   # fire8 expand              1.90
    "/Relu_15": 7.0,   # fire9 squeeze             6.07
    "/Relu_16": 18.0,  # fire9 expand             17.32
    "/Relu_17": 14.0,  # conv10 (classifier)      13.08
}
_ORIGIN_RELU_VR_DEF = 3.0


# =============================================================================
# SqueezeNet Specs
# =============================================================================

# CIFAR-shaped input [1, 3, 32, 32] — canonical CIFAR SqueezeNet.
SQUEEZENET_CIFAR10 = ModelSpec(
    name="squeezenet_cifar10",
    model_class=SqueezeNet,
    example_inputs=(_squeezenet_example_input(),),
    encrypt_inputs=["x"],
    model_init_args=(10, 3),
    model_init_kwargs={},
    model_post_init=_load_squeezenet_weights,
    compile_options={
        # ant (CPU) only — pooling/concat are unsupported on the GPU executors.
        # Same CKKS recipe as AlexNet/VGG16 (N=131072, q0=60, sf=56) with
        # sbm + ts + rtt; the reference squeezenet_cifar10 recipe also uses
        # N=131072 (hw=192, q0=60, sf=57).
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
    expected_ops=["Conv", "Relu", "AveragePool", "Concat", "Gemm"],
    weights_required=True,
    dataset="cifar10",
)


# =============================================================================
# Spec Collections
# =============================================================================

SQUEEZENET_SPECS = [SQUEEZENET_CIFAR10]
ALL_SQUEEZENET_SPECS = SQUEEZENET_SPECS

__all__ = [
    "ALL_SQUEEZENET_SPECS",
    "SQUEEZENET_CIFAR10",
    "SQUEEZENET_SPECS",
]
