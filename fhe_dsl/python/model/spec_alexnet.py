#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AlexNet ModelSpec instances for FHE compilation.

AlexNet is a mid-depth CNN (5 conv + 5 relu + 3 avg-pool → flatten → 3 linear)
and follows the VGG16 route: weights_required=True makes
ModelSpec::get_vr_profile() auto-discover profiles/{name}.json for the ReLU
polynomial approximation, and model_post_init loads the fixed weights
produced by train_alexnet.py.

Backend scope is **ant (CPU) only**: the 2×2 AveragePool2d layers that
AlexNet uses for downsampling are supported by the antlib CPU runtime but
segfault on the phantom / ace GPU executors (see
tests/regression/sample/test_sample_smoke_*.py). LeNet instead used stride-2
convs to dodge pooling and run on GPU.

Frontend note: compile this model with ``frontend="torch-via-onnx"``, not the
default ``"torch"``. The direct torch→AIR conv vectorizer fails on deeper CNN
graphs (``vector_gen.cxx:196: roll num attribute should not be empty``) and
routing the torch model through ONNX (torch model → .onnx → AIR) avoids it;
the reference alexnet_cifar10 build likewise goes through ONNX.
"""

import os

import torch

from ace.fhe.config.spec import ModelSpec

from .alexnet import AlexNet

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "alexnet", "weights")
_ALEXNET_WEIGHTS = os.path.join(_WEIGHTS_DIR, "alexnet_cifar10.pt")

_ALEXNET_DATA_SEED = 2026


def _alexnet_example_input() -> torch.Tensor:
    """Deterministic class-0 template input, matching train_alexnet.py.

    A fixed template from the training distribution (same generator seed)
    keeps the logit margins wide, so CKKS noise / ReLU approximation never
    flips the argmax. Random torch.randn inputs land where margins are small.
    """
    g = torch.Generator().manual_seed(_ALEXNET_DATA_SEED)
    templates = torch.randn(10, 3, 32, 32, generator=g) / 7.0
    return templates[0:1].float()          # [1, 3, 32, 32]


def _load_alexnet_weights(model: torch.nn.Module) -> None:
    """Load the fixed AlexNet weights into the model (in-place) and eval."""
    if not os.path.exists(_ALEXNET_WEIGHTS):
        raise FileNotFoundError(
            f"AlexNet weights not found: {_ALEXNET_WEIGHTS}. Generate them: "
            "python -m model.alexnet.train_alexnet (from fhe_dsl/python)"
        )
    state = torch.load(_ALEXNET_WEIGHTS, map_location="cpu", weights_only=True)
    # AlexNet.forward uses features (conv/relu/pool) + classifier; keys match.
    model.load_state_dict(state)
    model.eval()


# =============================================================================
# ReLU value ranges
# =============================================================================

# Per-node ReLU VR values, profiled over the trained weights
# (weights/alexnet_cifar10.pt) on the training distribution, matching
# profiles/alexnet_cifar10.json (features_*_Relu / classifier_*_Relu keys →
# our ONNX node names here).
#
# KEY NAMING: the torch-via-onnx frontend exports via the legacy
# torch.onnx.export (dynamo=False), which does NOT preserve torch module
# scopes — Relu nodes come out as "/Relu", "/Relu_1", ... in graph order.
# These ARE the AIR node names the SIHE backend matches against
# (compiler/fhe-cmplr/include/fhe/sihe/config.h Parse_relu_vr →
# tensor2sihe_impl.h ctx.Has_relu_vr(node->Attr("name"))). Earlier keys
# like "/features/features.1/Relu" NEVER matched (verified: generated .c
# contained only 1/relu_vr_def scale constants) — every node silently fell
# back to relu_vr_def, which is why the smoke test failed with a shuffled
# argmax each time the def changed (fhe=8 at def=2, fhe=7 at def=3).
# Graph order ↔ profile site mapping (verified against the exported .onnx):
_ORIGIN_RELU_VR = {
    "/Relu": 1.0,    # conv0  absmax 0.65
    "/Relu_1": 1.0,  # conv1  absmax 0.72
    "/Relu_2": 1.0,  # conv2  absmax 0.36
    "/Relu_3": 4.0,  # conv3  absmax 3.03
    "/Relu_4": 6.0,  # conv4  absmax 5.48
    "/Relu_5": 16.0,  # fc1    absmax 15.46
    "/Relu_6": 42.0,  # fc2    absmax 41.51
}
_ORIGIN_RELU_VR_DEF = 3.0


# =============================================================================
# AlexNet Specs
# =============================================================================

# CIFAR-shaped input [1, 3, 32, 32] — canonical CIFAR AlexNet.
ALEXNET_CIFAR10 = ModelSpec(
    name="alexnet_cifar10",
    model_class=AlexNet,
    example_inputs=(_alexnet_example_input(),),
    encrypt_inputs=["x"],
    model_init_args=(10, 3),
    model_init_kwargs={},
    model_post_init=_load_alexnet_weights,
    compile_options={
        # ant (CPU) only — pooling is unsupported on the GPU executors.
        # Same CKKS recipe as the VGG16 spec (N=131072, q0=60, sf=56); the
        # deep/wide first Gemm (1024→4096) needs the larger slot count too.
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "N": 131072, "icl": 17, "sbm": True, "ts": True, "rtt": True},
            # relu_vr: per-node VR map (see _ORIGIN_RELU_VR above) → fhe_cmplr
            # '-SIHE:relu_vr_def=2:relu_vr=<name>=<vr>;...' when generating .c.
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

ALEXNET_SPECS = [ALEXNET_CIFAR10]
ALL_ALEXNET_SPECS = ALEXNET_SPECS

__all__ = [
    "ALEXNET_CIFAR10",
    "ALEXNET_SPECS",
    "ALL_ALEXNET_SPECS",
]