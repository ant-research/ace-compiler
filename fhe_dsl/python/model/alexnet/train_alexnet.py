#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Train and save weights + VR profile for the AlexNet FHE smoke model.

AlexNet is the mid-depth CNN FHE test model (ant-CPU only — see alexnet.py).
FHE smoke testing needs:

1. Fixed, well-separated weights — random init produces low logit margins
   that CKKS noise flips, and the ReLU polynomial approximation needs
   realistic value ranges, not randn.
2. A per-call-site ReLU VR profile (profiles/{name}.json), consumed via
   ModelSpec::get_vr_profile() when weights_required=True, exactly like
   ResNet/LeNet/VGG16. See model/resnet/profiles/*.json for the format.

This script trains on deterministic CIFAR-shaped synthetic data with a
fixed seed (the same generator as train_vgg16.py), saves weights, then
generates the VR profile over the same normalized inputs.

Usage:
    python3 -m model.alexnet.train_alexnet    (source tree, from fhe_dsl/python)
    python3 fhe_dsl/python/model/alexnet/train_alexnet.py
"""

import argparse
import os

import torch
import torch.nn as nn

from .alexnet import AlexNet

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "weights")
_PROFILES_DIR = os.path.join(os.path.dirname(__file__), "..", "profiles")
_WEIGHTS_FILE = "alexnet_cifar10.pt"

SEED = 2026
STEPS = 1200
BATCH = 64
# AlexNet has no BatchNorm (unlike VGG16), so the lr=0.005 that trains VGG16
# diverges here (Adam → acc 0.10, chance). lr=0.001 converges cleanly to
# acc=1.0 / margin≈10, matching the VGG16 separation.
LR = 0.001


def _synthetic_cifar_like(n_per_class: int = 48, noise: float = 0.05):
    """Deterministic CIFAR-shaped data with wide class margins.

    Each class is a fixed random template [3,32,32] + small noise. A CNN can
    separate these with large margins, and values are normalized to a
    realistic range so ReLU VR profiling is meaningful. Keeps training
    self-contained (no dataset download). Same generator/seed as
    train_vgg16.py, so example inputs match across models.
    """
    g = torch.Generator().manual_seed(SEED)
    templates = torch.randn(10, 3, 32, 32, generator=g) / 7.0  # ~ ±0.15
    xs, ys = [], []
    for cls in range(10):
        t = templates[cls]
        samples = t + noise * torch.randn(n_per_class, 3, 32, 32, generator=g)
        xs.append(samples)
        ys.append(torch.full((n_per_class,), cls, dtype=torch.long))
    return torch.cat(xs), torch.cat(ys)


def _train(model, xs, ys, steps=STEPS, batch=BATCH, lr=LR):
    """Train, returning final train accuracy and minimum class margin."""
    opt = torch.optim.Adam(model.parameters(), lr=lr)
    n = xs.shape[0]
    g = torch.Generator().manual_seed(SEED + 1)
    for _ in range(steps):
        idx = torch.randperm(n, generator=g)[:batch]
        opt.zero_grad()
        loss = nn.functional.cross_entropy(model(xs[idx]), ys[idx])
        loss.backward()
        opt.step()
    with torch.no_grad():
        acc = (model(xs).argmax(dim=1) == ys).float().mean().item()
        logits = model(xs)
        top = logits.topk(2, dim=1).values
        margin = (top[:, 0] - top[:, 1]).min().item()
    return acc, margin


def _profile_vr(model, inputs, name):
    """Generate and save the ReLU VR profile via the project ReLUProfiler.

    Profiles the TRAINED model passed in: its weights are copied onto the
    fresh model the profiler creates (via model_post_init). Without this,
    ReLUProfiler instantiate a randomly initialized model and the saved
    ranges are meaningless for the trained weights shipped alongside.
    """
    from ace.fhe.config import ModelSpec, ReLUProfiler

    state = {k: v.detach().clone() for k, v in model.state_dict().items()}

    def _load_weights(m):
        m.load_state_dict(state)
        m.eval()

    spec = ModelSpec(
        name=name,
        model_class=AlexNet,
        example_inputs=(inputs[:1],),
        encrypt_inputs=["x"],
        model_init_args=(10, 3),
        model_post_init=_load_weights,
    )
    profiler = ReLUProfiler(spec)
    return profiler.profile(inputs=inputs, margin=1, save=True)


def main():
    parser = argparse.ArgumentParser(description="Train AlexNet FHE smoke weights")
    parser.add_argument(
        "--name",
        default="alexnet_cifar10",
        help="ModelSpec name (also used as VR profile filename)",
    )
    args = parser.parse_args()

    torch.manual_seed(SEED)
    xs, ys = _synthetic_cifar_like()
    print(f"data: {tuple(xs.shape)} labels=[{ys.min().item()},{ys.max().item()}]")

    model = AlexNet(num_classes=10, in_channels=3)
    n_params = sum(p.numel() for p in model.parameters())
    print(f"params: {n_params:,}")
    acc, margin = _train(model, xs, ys)
    print(f"train acc={acc:.4f} min margin={margin:.4f}")
    if acc < 0.95 or margin < 0.2:
        raise RuntimeError(
            f"Poor separation: acc={acc}, margin={margin}. Train more / widen margins."
        )

    os.makedirs(_WEIGHTS_DIR, exist_ok=True)
    path = os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE)
    torch.save(model.state_dict(), path)
    print(f"saved weights -> {path}")

    vr_path = _profile_vr(model, xs, args.name)
    print(f"VR profile -> {vr_path}")


if __name__ == "__main__":
    main()