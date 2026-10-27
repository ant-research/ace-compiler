#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Train and save fixed weights for the Logistic Regression FHE smoke model.

LR is a single nn.Linear with no hidden activation. With random init the
class logits are only ~0.04 apart, which CKKS noise can flip — the FHE
smoke test needs a *fixed, well-separated* weight set so plaintext and
ciphertext always agree. This script produces that weight set on a small
synthetic linearly-separable dataset (deterministic).

Usage:
    python3 fhe_dsl/python/model/lr/train_lr.py
"""

import os

import torch
import torch.nn as nn

from .lr import LogisticRegression

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "weights")
_WEIGHTS_FILE = "lr_16x10_toy.pt"

INPUT_SIZE = 16
NUM_CLASSES = 10
SEED = 2026
N_PER_CLASS = 200
NOISE = 0.1
STEPS = 2000
LR = 0.1


def _make_dataset():
    """Deterministic linearly-separable dataset: class i is a fixed center + noise."""
    g = torch.Generator().manual_seed(SEED)
    # Distinct class centers (unit-ish vectors) so the separating margins are wide.
    centers = torch.randn(NUM_CLASSES, INPUT_SIZE, generator=g)
    centers = centers / centers.norm(dim=1, keepdim=True)

    xs, ys = [], []
    for cls in range(NUM_CLASSES):
        center = centers[cls]
        samples = center + NOISE * torch.randn(N_PER_CLASS, INPUT_SIZE, generator=g)
        xs.append(samples)
        ys.append(torch.full((N_PER_CLASS,), cls, dtype=torch.long))
    return torch.cat(xs), torch.cat(ys), centers


def main():
    os.makedirs(_WEIGHTS_DIR, exist_ok=True)
    torch.manual_seed(SEED)

    xs, ys, _ = _make_dataset()
    model = LogisticRegression(INPUT_SIZE, NUM_CLASSES, bias=True)
    opt = torch.optim.SGD(model.parameters(), lr=LR)

    for step in range(STEPS):
        opt.zero_grad()
        logits = model(xs)
        loss = nn.functional.cross_entropy(logits, ys)
        loss.backward()
        opt.step()

    with torch.no_grad():
        acc = (model(xs).argmax(dim=1) == ys).float().mean().item()
        # Min margin between the winning logit and the runner-up, over a clean sample set.
        centers = _make_dataset()[2]
        logits = model(centers)
        top = logits.topk(2, dim=1).values
        margins = (top[:, 0] - top[:, 1]).min().item()

    print(f"train acc={acc:.4f}  min class margin (on centers)={margins:.4f}")
    if acc < 1.0 or margins < 0.2:
        raise RuntimeError(
            f"Model did not converge to well-separated weights (acc={acc}, margin={margins}). "
            "Try more steps / larger margins."
        )

    path = os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE)
    torch.save(model.state_dict(), path)
    print(f"saved weights -> {path}")


if __name__ == "__main__":
    main()