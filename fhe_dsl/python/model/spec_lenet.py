#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
LeNet ModelSpec instances for FHE compilation.

LeNet is a small CNN with ReLUs, so it follows the ResNet route rather than
LR: weights_required=True makes ModelSpec::get_vr_profile() auto-discover
profiles/{name}.json for the ReLU polynomial approximation, and model_post_init
loads the fixed weights produced by train_lenet.py.
"""

import os

import torch

from ace.fhe.config.spec import ModelSpec

from .lenet import LeNet

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "lenet", "weights")
_LENET_WEIGHTS = os.path.join(_WEIGHTS_DIR, "lenet_cifar10.pt")

_LENET_DATA_SEED = 2026


def _lenet_example_input() -> torch.Tensor:
    """Deterministic class-0 template input, matching train_lenet.py.

    A fixed template from the training distribution (same generator seed)
    keeps the logit margins wide, so CKKS noise / ReLU approximation never
    flips the argmax. Random torch.randn inputs land where margins are small.
    """
    g = torch.Generator().manual_seed(_LENET_DATA_SEED)
    templates = torch.randn(10, 3, 32, 32, generator=g) / 7.0
    return templates[0:1].float()          # [1, 3, 32, 32]


def _load_lenet_weights(model: torch.nn.Module) -> None:
    """Load the fixed LeNet weights into the model (in-place) and eval."""
    if not os.path.exists(_LENET_WEIGHTS):
        raise FileNotFoundError(
            f"LeNet weights not found: {_LENET_WEIGHTS}. Generate them: "
            "python -m model.lenet.train_lenet (from fhe_dsl/python)"
        )
    state = torch.load(_LENET_WEIGHTS, map_location="cpu", weights_only=True)
    # LeNet.forward only uses fc1/fc2/conv/relu/pool; keys match.
    model.load_state_dict(state)
    model.eval()


# =============================================================================
# LeNet Specs
# =============================================================================

# CIFAR-shaped input [1, 3, 32, 32] — canonical LeNet.
LENET_CIFAR10 = ModelSpec(
    name="lenet_cifar10",
    model_class=LeNet,
    example_inputs=(_lenet_example_input(),),
    encrypt_inputs=["x"],
    model_init_args=(10, 3),
    model_init_kwargs={},
    model_post_init=_load_lenet_weights,
    compile_options={
        "phantom": {
            "vec": {"conv_parl": True, "ssf": True},
            "ckks": {"q0": 60, "sf": 56, "N": 65536, "icl": 17},
        },
        "ant": {
            "ckks": {"hw": 192, "q0": 60, "sf": 56, "sbm": True, "ts": True, "rtt": True},
            "p2c": {"fp": True, "lib": "ant", "ts": True},
        },
    },
    expected_ops=["Conv", "Relu", "Gemm"],
    weights_required=True,
    dataset="cifar10",
)


# =============================================================================
# Spec Collections
# =============================================================================

LENET_SPECS = [LENET_CIFAR10]
ALL_LENET_SPECS = LENET_SPECS

__all__ = [
    "ALL_LENET_SPECS",
    "LENET_CIFAR10",
    "LENET_SPECS",
]