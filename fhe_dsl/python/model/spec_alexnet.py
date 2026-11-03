#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AlexNet ModelSpec instances for FHE compilation.

AlexNet is a mid-depth CNN (5 conv + 5 relu + 3 avg-pool → flatten → 3 linear)
and follows the VGG16 route: weights_required=True makes
ModelSpec::get_vr_profile() auto-discover profiles/{name}.json for the ReLU
polynomial approximation, and model_post_init loads the fixed weights in
weights/alexnet_cifar10.pt.

Those weights are the real trained CIFAR-10 AlexNet, imported from the
reference ``alexnet_cifar10.onnx`` (ace-compiler-iiecas) by
``model/alexnet/import_reference_weights.py`` — NOT the synthetic rehearsal
weights ``train_alexnet.py`` produces. Plaintext top-1 is 86.94% on the full
CIFAR-10 test set, matching the ASPLOS/IIE accuracy table (85–86%).

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

# Deterministic CIFAR-10 test image used as the compile/probe example.
# offset=1 is class 8 with a ~12-logit plaintext margin, so CKKS noise / ReLU
# approximation cannot flip the argmax (offset=0 has a marginal ~1.2 margin).
_ALEXNET_EXAMPLE_OFFSET = 1


def _alexnet_example_input() -> torch.Tensor:
    """A real, normalized CIFAR-10 image as the compile/probe example input.

    The shipped weights are trained on real CIFAR-10 (imported from the
    reference ``alexnet_cifar10.onnx``), so the example input must come from
    the same distribution as ``load_cifar10_images`` — the earlier synthetic
    ``randn/7`` template is meaningless to these weights.
    """
    from ace.model.dataset import load_cifar10_images

    images, _ = load_cifar10_images(1, offset=_ALEXNET_EXAMPLE_OFFSET)
    return images[0:1]


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

# Per-node ReLU VR values, profiled over the shipped weights
# (weights/alexnet_cifar10.pt) on the full CIFAR-10 test set (10k images,
# CIFAR mean/std normalization) as PRE-ReLU input abs_max; VR = ceil(1) + 1.
#
# KEY NAMING: the torch-via-onnx frontend exports via the legacy
# torch.onnx.export (dynamo=False), which does NOT preserve torch module
# scopes — Relu nodes come out as "/Relu", "/Relu_1", ... in graph order.
# These ARE the AIR node names the SIHE backend matches against
# (compiler/fhe-cmplr/include/fhe/sihe/config.h Parse_relu_vr →
# tensor2sihe_impl.h ctx.Has_relu_vr(node->Attr("name"))). Earlier keys
# like "/features/features.1/Relu" NEVER matched (verified: generated .c
# contained only 1/relu_vr_def scale constants) — every node silently fell
# back to relu_vr_def.
#
# These values MUST match profiles/alexnet_cifar10.json (regenerate both at
# once: `python3 -m model.alexnet.train_alexnet --profile-only`).
# node           profile site  pre-ReLU abs_max  (10k test set)
_ORIGIN_RELU_VR = {
    "/Relu": 8.0,     # conv0  features.1     7.72
    "/Relu_1": 6.0,   # conv1  features.4     5.77
    "/Relu_2": 4.0,   # conv2  features.7     3.54
    "/Relu_3": 4.0,   # conv3  features.9     3.83
    "/Relu_4": 4.0,   # conv4  features.11    3.96
    "/Relu_5": 2.0,   # fc1    classifier.1   1.91
    "/Relu_6": 16.0,  # fc2    classifier.3  15.21
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