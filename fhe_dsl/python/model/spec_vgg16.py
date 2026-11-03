#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
VGG16 ModelSpec instances for FHE compilation.

VGG16 is a deep CNN (13 conv + 13 relu + 5 avg-pool → flatten → linear) and
follows the ResNet/LeNet route: weights_required=True makes
ModelSpec::get_vr_profile() auto-discover profiles/{name}.json for the ReLU
polynomial approximation, and model_post_init loads the fixed weights
produced by train_vgg16.py.

Backend scope is **ant (CPU) only**: the 2×2 AveragePool2d blocks that define
VGG16 are supported by the antlib CPU runtime but segfault on the phantom /
ace GPU executors (see tests/regression/sample/test_sample_smoke_*.py). CKKS
parameters (N=65536, q0=60, sf=56) mirror the ResNet/LeNet family. LeNet
instead used stride-2 convs to dodge pooling and run on GPU.

Frontend note: compile this model with ``frontend="torch-via-onnx"``, not the
default ``"torch"``. The direct torch→AIR path fails for VGG16 depth in the
conv vectorizer (``vector_gen.cxx:196: roll num attribute should not be
empty``) — blocks 1–3 compile, block 4 onward does not. Routing the torch
model through ONNX (torch model → .onnx → AIR) avoids it; the reference
vgg16_cifar10 build likewise goes through ONNX. Inference on ant-CPU is very
heavy (the reference build declares ~730 GB RAM for vgg16_cifar10).
"""

import os

import torch

from ace.fhe.config.spec import ModelSpec

from .vgg16 import VGG16

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "vgg16", "weights")
_VGG16_WEIGHTS = os.path.join(_WEIGHTS_DIR, "vgg16_cifar10.pt")

# Deterministic CIFAR-10 test image used as the compile/probe example.
_VGG16_EXAMPLE_OFFSET = 0


def _vgg16_example_input() -> torch.Tensor:
    """A real, normalized CIFAR-10 image as the compile/probe example input.

    The shipped weights are trained on real CIFAR-10 (imported from the
    reference ``vgg16_cifar10.onnx``), so the example input must come from the
    same distribution as ``load_cifar10_images`` — the earlier synthetic
    ``randn/7`` template is meaningless to these weights.
    """
    from ace.model.dataset import load_cifar10_images

    images, _ = load_cifar10_images(1, offset=_VGG16_EXAMPLE_OFFSET)
    return images[0:1]


def _load_vgg16_weights(model: torch.nn.Module) -> None:
    """Load the fixed VGG16 weights into the model (in-place) and eval."""
    if not os.path.exists(_VGG16_WEIGHTS):
        raise FileNotFoundError(
            f"VGG16 weights not found: {_VGG16_WEIGHTS}. Generate them: "
            "python -m model.vgg16.train_vgg16 (from fhe_dsl/python)"
        )
    state = torch.load(_VGG16_WEIGHTS, map_location="cpu", weights_only=True)
    # VGG16.forward uses features (conv/bn/relu/pool) + classifier; keys match.
    model.load_state_dict(state)
    model.eval()


# =============================================================================
# ReLU value ranges
# =============================================================================

# Per-node ReLU VR values, profiled over the shipped weights
# (weights/vgg16_cifar10.pt) on the full CIFAR-10 test set (10k images,
# CIFAR mean/std normalization) as PRE-ReLU input abs_max; VR = ceil(abs) + 1.
#
# KEY NAMING: the torch-via-onnx frontend exports via the legacy
# torch.onnx.export (dynamo=False), which does NOT preserve torch module
# scopes — Relu nodes come out as "/Relu", "/Relu_1", ... in graph order.
# These ARE the AIR node names the SIHE backend matches against
# (compiler/fhe-cmplr/include/fhe/sihe/config.h Parse_relu_vr →
# tensor2sihe_impl.h ctx.Has_relu_vr(node->Attr("name"))). The earlier
# "/features/features.N/Relu" keys NEVER matched (verified in the generated
# .c) — every node silently fell back to relu_vr_def.
#
# Do NOT reuse the reference build's ranges (5/5/4×11, def=4): they were
# calibrated on the BN-equipped reference net; undersized vr pushes x/vr
# outside the [-1,1] domain of the sign-composite polynomial (Chebyshev
# diverges there), silently corrupting the ReLU outputs.
#
# These values MUST match profiles/vgg16_cifar10.json (regenerate both at
# once: `python3 -m model.vgg16.train_vgg16 --profile-only`).
# node        site              pre-ReLU abs_max (10k)
_ORIGIN_RELU_VR = {
    "/Relu": 5.0,      # features.2  4.56
    "/Relu_1": 6.0,    # features.5  5.19
    "/Relu_2": 4.0,    # features.9  3.44
    "/Relu_3": 4.0,    # features.12 3.77
    "/Relu_4": 3.0,    # features.16 2.12
    "/Relu_5": 3.0,    # features.19 2.28
    "/Relu_6": 4.0,    # features.22 3.85
    "/Relu_7": 2.0,    # features.26 1.80
    "/Relu_8": 2.0,    # features.29 1.48
    "/Relu_9": 1.0,    # features.32 0.73
    "/Relu_10": 1.0,   # features.36 0.45
    "/Relu_11": 1.0,   # features.39 0.47
    "/Relu_12": 3.0,   # features.42 2.63
}
_ORIGIN_RELU_VR_DEF = 3.0


# =============================================================================
# VGG16 Specs
# =============================================================================

# CIFAR-shaped input [1, 3, 32, 32] — canonical CIFAR VGG16.
VGG16_CIFAR10 = ModelSpec(
    name="vgg16_cifar10",
    model_class=VGG16,
    example_inputs=(_vgg16_example_input(),),
    encrypt_inputs=["x"],
    model_init_args=(10, 3),
    model_init_kwargs={},
    model_post_init=_load_vgg16_weights,
    compile_options={
        # ant (CPU) only — pooling is unsupported on the GPU executors.
        # N=131072 (not the ResNet/LeNet 65536): the ant runtime encoder
        # rejects N=65536 for VGG16 with "slot size is too small", and the
        # reference vgg16_cifar10 build recipe also uses N=131072.
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "N": 131072, "icl": 17, "sbm": True, "ts": True, "rtt": True},
            # relu_vr: per-node VR map (see _ORIGIN_RELU_VR above) → fhe_cmplr
            # '-SIHE:relu_vr_def=4:relu_vr=<name>=<vr>;...' when generating .c.
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

VGG16_SPECS = [VGG16_CIFAR10]
ALL_VGG16_SPECS = VGG16_SPECS

__all__ = [
    "ALL_VGG16_SPECS",
    "VGG16_CIFAR10",
    "VGG16_SPECS",
]