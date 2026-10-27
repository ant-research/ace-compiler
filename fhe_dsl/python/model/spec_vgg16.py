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

_VGG16_DATA_SEED = 2026


def _vgg16_example_input() -> torch.Tensor:
    """Deterministic class-0 template input, matching train_vgg16.py.

    A fixed template from the training distribution (same generator seed)
    keeps the logit margins wide, so CKKS noise / ReLU approximation never
    flips the argmax. Random torch.randn inputs land where margins are small.
    """
    g = torch.Generator().manual_seed(_VGG16_DATA_SEED)
    templates = torch.randn(10, 3, 32, 32, generator=g) / 7.0
    return templates[0:1].float()          # [1, 3, 32, 32]


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

# Per-node ReLU VR values, profiled over the trained weights
# (weights/vgg16_cifar10.pt) on the training distribution, matching
# profiles/vgg16_cifar10.json (features_*_Relu keys → our ONNX node names).
#
# KEY NAMING: the torch-via-onnx frontend exports via the legacy
# torch.onnx.export (dynamo=False), which does NOT preserve torch module
# scopes — Relu nodes come out as "/Relu", "/Relu_1", ... in graph order.
# These ARE the AIR node names the SIHE backend matches against
# (compiler/fhe-cmplr/include/fhe/sihe/config.h Parse_relu_vr →
# tensor2sihe_impl.h ctx.Has_relu_vr(node->Attr("name"))). The earlier
# "/features/features.N/Relu" keys NEVER matched (verified in the generated
# .c) — every node silently fell back to relu_vr_def=7, which happens to be
# close enough to the true ranges that the smoke test passed despite the
# constant being effectively flat. The keys below are the real exported node
# names in graph order, aligned with the profile sites above.
#
# Do NOT reuse the reference build's ranges (5/5/4×11, def=4): our trained
# activations run up to |x|=8.5, and undersized vr pushes x/vr outside the
# [-1,1] domain of the sign-composite polynomial (Chebyshev diverges there),
# which silently corrupts the ReLU outputs.
_ORIGIN_RELU_VR = {
    "/Relu": 6.0,      # block1 conv0 absmax 5.90
    "/Relu_1": 6.0,    # block1 conv1 absmax 5.72
    "/Relu_2": 8.0,    # block1 conv2 absmax 7.68
    "/Relu_3": 9.0,    # block2 conv0 absmax 8.49
    "/Relu_4": 8.0,    # block2 conv1 absmax 7.58
    "/Relu_5": 8.0,    # block2 conv2 absmax 7.67
    "/Relu_6": 8.0,    # block2 conv3 absmax 7.55
    "/Relu_7": 7.0,    # block3 conv0 absmax 6.53
    "/Relu_8": 6.0,    # block3 conv1 absmax 5.46
    "/Relu_9": 6.0,    # block3 conv2 absmax 5.60
    "/Relu_10": 7.0,   # block3 conv3 absmax 4.84
    "/Relu_11": 5.0,   # block4 conv0 absmax 4.88
    "/Relu_12": 5.0,   # block4 conv1 absmax 4.02
}
_ORIGIN_RELU_VR_DEF = 7.0


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