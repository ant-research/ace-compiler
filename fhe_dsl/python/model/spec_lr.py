#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Logistic Regression ModelSpec instances for FHE compilation.

LR is a single GEMM (nn.Linear) with no hidden activation — the simplest
end-to-end FHE model. No ReLU means no VR profiling is required, so
compile_options lean on backend defaults (default N=65536 is ample for a
single Gemm) and weights_required=False.

The smoke test needs a *fixed, well-separated* weight set (random LR init
has ~0.04 logit margins that CKKS noise can flip). model_post_init loads
the pretrained weights produced by train_lr.py.
"""

import os

import torch

from ace.fhe.config.spec import ModelSpec

from .lr import LogisticRegression

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "lr", "weights")
_LR_WEIGHTS = os.path.join(_WEIGHTS_DIR, "lr_16x10_toy.pt")


def _load_lr_weights(model: torch.nn.Module) -> None:
    """Load the fixed LR weights into the model (in-place) and eval."""
    if not os.path.exists(_LR_WEIGHTS):
        raise FileNotFoundError(
            f"LR weights not found: {_LR_WEIGHTS}. Generate them: "
            "python -m model.lr.train_lr (from fhe_dsl/python)"
        )
    state = torch.load(_LR_WEIGHTS, map_location="cpu", weights_only=True)
    model.load_state_dict(state)
    model.eval()


def _lr_example_input() -> torch.Tensor:
    """Deterministic near-class-0 input with wide logit margins.

    Random ``torch.randn`` inputs land where class logits are only ~0.04 apart
    and CKKS noise flips the argmax. Reusing the same generator seed as
    train_lr.py yields the clean center of class 0, whose logit margin is
    >4.0 — FHE always agrees with plaintext.
    """
    g = torch.Generator().manual_seed(2026)
    centers = torch.randn(10, 16, generator=g)
    centers = centers / centers.norm(dim=1, keepdim=True)
    return centers[0:1].float()          # [1, 16]


def _make_lr_spec(name):
    """Build an LR ModelSpec sharing the fixed weight set."""
    return ModelSpec(
        name=name,
        model_class=LogisticRegression,
        example_inputs=(_lr_example_input(),),
        encrypt_inputs=["x"],
        model_init_args=(16, 10),
        model_init_kwargs={"bias": True},
        model_post_init=_load_lr_weights,
        expected_ops=["Gemm"],
        dataset="toy",
    )


# =============================================================================
# LR Specs
# =============================================================================

# Flat input: [1, in_size] (canonical LR — a single Gemm).
LR_1D = _make_lr_spec("lr_1d")

# =============================================================================
# Spec Collections
# =============================================================================

LR_SPECS = [LR_1D]
ALL_LR_SPECS = LR_SPECS

__all__ = [
    "ALL_LR_SPECS",
    "LR_1D",
    "LR_SPECS",
]