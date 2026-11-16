#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Train and save weights + VR profile for the MobileNet FHE model.

Unlike AlexNet/VGG16/SqueezeNet, there is NO reference mobilenet ONNX in this
repo to import weights from (the compiler-side phase2 catalog lists
``alinm/mobilenet_cifar10.onnx`` but the blob was never added). The shipped
weights/mobilenet_cifar10.pt are therefore trained here on the REAL CIFAR-10
train set — 88.95% top-1 on the 10k test set after 80 epochs (SGD + cosine,
4-pixel pad/random-crop + horizontal flip). Without the augmentation the net
memorizes the train set and stalls at ~73% test.

The main deliverable besides the weights is the per-node ReLU VR profile:

    python3 -m model.mobilenet.train_mobilenet --profile-only

WHY ONNX-LEVEL PROFILING: MobileNet is compiled with
``frontend="torch-via-onnx"``, and that frontend wraps the module's forward
in ``functools.wraps(model.forward)`` before calling the legacy ONNX
exporter — the resulting node names the SIHE backend sees are ``/Relu``,
``/Relu_1``, ... in graph order (NOT torch module scopes). This script
profiles the EXACT ONNX the compiler produces, using ONNX Runtime with each
Relu input exposed as a graph output. Same method as train_squeezenet.py.

Usage (from fhe_dsl/python):
    python3 -m model.mobilenet.train_mobilenet              # train + profile
    python3 -m model.mobilenet.train_mobilenet --profile-only
    python3 -m model.mobilenet.train_mobilenet --epochs 60
"""

import argparse
import functools
import json
import os

import numpy as np
import onnx
import onnxruntime as ort
import torch
import torch.nn as nn

from .mobilenet import MobileNet


_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "weights")
_PROFILES_DIR = os.path.join(os.path.dirname(__file__), "profiles")
_WEIGHTS_FILE = "mobilenet_cifar10.pt"

SEED = 2026
EPOCHS = 80
BATCH = 128
LR = 0.01
MOMENTUM = 0.9
WEIGHT_DECAY = 5e-4

# CIFAR-10 normalization used by ace.model.dataset (CIFAR statistics).
_MEAN = np.array([0.4914, 0.4822, 0.4465], dtype=np.float32).reshape(1, 3, 1, 1)
_STD = np.array([0.229, 0.224, 0.225], dtype=np.float32).reshape(1, 3, 1, 1)


# =============================================================================
# Real CIFAR-10 data (the 50k train set + 10k test set, python batches)
#
# The train split is kept UN-normalized (0..1) so augmentation (pad+crop, hflip)
# operates on raw pixels; normalization is applied on-GPU after augmentation
# (the classic pytorch-cifar recipe). The test split is stored normalized,
# matching ace.model.dataset.load_cifar10_images exactly.
# =============================================================================


def _load_cifar10_split(which: str = "train"):
    """Load a CIFAR-10 split; train → (raw 0..1), test → (normalized)."""
    import pickle

    from ace.model.dataset import _find_cifar10_test_batch, _get_data_root

    if which == "test":
        files = [_find_cifar10_test_batch()]
    else:
        root = _get_data_root() / "cifar-10-batches-py"
        files = [str(root / f"data_batch_{i}") for i in range(1, 6)]

    data, labels = [], []
    for fp in files:
        with open(fp, "rb") as f:
            batch = pickle.load(f, encoding="bytes")
        data.append(batch[b"data"])
        labels.extend(batch[b"labels"])
    x = np.concatenate(data).reshape(-1, 3, 32, 32).astype(np.float32) / 255.0
    y = torch.tensor(labels, dtype=torch.long)
    if which == "test":
        x = (x - _MEAN) / _STD
    return torch.from_numpy(x), y


@torch.no_grad()
def _evaluate(model: nn.Module, images: torch.Tensor, labels: torch.Tensor, batch: int = 500):
    correct = 0
    for i in range(0, images.shape[0], batch):
        pred = model(images[i : i + batch]).argmax(dim=1)
        correct += (pred == labels[i : i + batch]).sum().item()
    return correct / labels.shape[0]


def _train(model: nn.Module, images: torch.Tensor, labels: torch.Tensor,
           test_images: torch.Tensor, test_labels: torch.Tensor,
           epochs: int = EPOCHS, batch: int = BATCH, lr: float = LR):
    device = "cuda" if torch.cuda.is_available() else "cpu"
    model.to(device)
    opt = torch.optim.SGD(model.parameters(), lr=lr, momentum=MOMENTUM,
                         weight_decay=WEIGHT_DECAY)
    sched = torch.optim.lr_scheduler.CosineAnnealingLR(opt, T_max=epochs)

    # Classic pytorch-cifar augmentation: 4-pixel reflect pad + random crop,
    # random horizontal flip. Without it MobileNet memorizes the 50k train set
    # (train loss → 0) and stalls at ~73% test — augmentation buys ~15 points.
    aug = nn.Sequential(
        nn.ReplicationPad2d(4),
        _RandomCrop(32),
        _RandomHFlip(),
    ).to(device)

    mean = torch.tensor(_MEAN, device=device).flatten()
    std = torch.tensor(_STD, device=device).flatten()

    n = images.shape[0]
    g = torch.Generator().manual_seed(SEED + 1)
    for epoch in range(epochs):
        perm = torch.randperm(n, generator=g)
        ep_x, ep_y = images[perm].to(device), labels[perm].to(device)
        model.train()
        for i in range(0, n, batch):
            xb = aug(ep_x[i : i + batch])
            xb = (xb - mean.view(1, 3, 1, 1)) / std.view(1, 3, 1, 1)
            yb = ep_y[i : i + batch]
            opt.zero_grad()
            loss = nn.functional.cross_entropy(model(xb), yb)
            loss.backward()
            opt.step()
        sched.step()
        model.eval()
        test_acc = _evaluate(model, test_images.to(device), test_labels.to(device))
        print(f"epoch {epoch + 1}/{epochs}  loss={loss.item():.4f}  test_acc={test_acc:.4f}",
              flush=True)
    model.to("cpu").eval()


class _RandomCrop(nn.Module):
    """Random 32×32 crop from a 40×40 padded input (torch.randint offsets)."""

    def __init__(self, size: int = 32):
        super().__init__()
        self.size = size

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        h = self.size
        t = int(torch.randint(0, x.shape[2] - h + 1, (1,)).item())
        left = int(torch.randint(0, x.shape[3] - h + 1, (1,)).item())
        return x[:, :, t : t + h, left : left + h]


class _RandomHFlip(nn.Module):
    """Random horizontal flip, decided per batch element on the device RNG."""

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        mask = torch.rand(x.shape[0], 1, 1, 1, device=x.device) < 0.5
        return torch.where(mask, x.flip(-1), x)


# =============================================================================
# VR profiling (ONNX level — matches the compiler's node names)
# =============================================================================


def profile_vr_onnx(model: nn.Module, inputs: torch.Tensor, margin: int = 1,
                     relu_vr_def: int = 3) -> dict:
    """Profile pre-ReLU abs_max on the exact ONNX the torch-via-onnx frontend emits.

    Args:
        model: MobileNet with the shipped weights loaded, in eval mode.
        inputs: (N, 3, 32, 32) normalized images.
        margin: added to int(abs_max) → VR = int(abs_max) + margin.
        relu_vr_def: fallback VR for nodes whose abs_max is 0.

    Returns:
        dict with "relu_vr_def", "relu_vr" (string) and "per_node" entries.
    """
    import tempfile

    from ace.fhe.ir.io.onnx_tools import export_function_to_onnx

    model.eval()

    # Reproduce the compiler's wrapper: _create_wrapped_function uses
    # functools.wraps(model.forward), then export_function_to_onnx wraps it
    # in an inner FunctionWrapper module. This is what yields /Relu, /Relu_1...
    @functools.wraps(model.forward)
    def wrapped_func(*args, **kwargs):
        with torch.no_grad():
            return model(*args, **kwargs)

    wrapped_func._original_model = model

    with tempfile.TemporaryDirectory() as tmpdir:
        onnx_path = os.path.join(tmpdir, "mobilenet.onnx")
        export_function_to_onnx(wrapped_func, (inputs[:1],), onnx_path, ["x"])

        graph = onnx.load(onnx_path)
        relu_nodes = [n for n in graph.graph.node if n.op_type == "Relu"]
        if not relu_nodes:
            raise RuntimeError("No Relu nodes in exported ONNX")

        # Expose every Relu input tensor as a graph output so ORT returns it.
        probes = []
        for node in relu_nodes:
            graph.graph.output.append(onnx.ValueInfoProto(name=node.input[0]))
            probes.append(node.input[0])
        probe_path = os.path.join(tmpdir, "mobilenet_probe.onnx")
        onnx.save(graph, probe_path)

        session = ort.InferenceSession(probe_path)
        out_names = [o.name for o in session.get_outputs()]
        probe_set = set(probes)

        abs_max = dict.fromkeys(probes, 0.0)
        for i in range(inputs.shape[0]):
            outs = session.run(out_names, {"x": inputs[i : i + 1].numpy()})
            for name, val in zip(out_names, outs):
                if name in probe_set:
                    abs_max[name] = max(abs_max[name], float(np.abs(val).max()))

    per_node = {}
    entries = []
    for node in relu_nodes:
        a = abs_max[node.input[0]]
        vr = int(a) + margin if a > 0 else relu_vr_def
        per_node[node.name] = {"abs_max": a, "vr": vr}
        entries.append(f"{node.name}={vr}")

    return {
        "relu_vr_def": relu_vr_def,
        "relu_vr": ";".join(entries),
        "per_node": per_node,
    }


def _profile_real(name: str) -> str:
    """Regenerate the VR profile from the shipped weights on real CIFAR-10."""
    from ace.model.dataset import load_cifar10_images

    path = os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE)
    if not os.path.exists(path):
        raise FileNotFoundError(
            f"{path} not found. Train first:\n"
            "  python3 -m model.mobilenet.train_mobilenet   (from fhe_dsl/python)"
        )
    model = MobileNet(num_classes=10, in_channels=3)
    model.load_state_dict(torch.load(path, map_location="cpu", weights_only=True))
    model.eval()

    images, _ = load_cifar10_images(10000)
    result = profile_vr_onnx(model, images)

    os.makedirs(_PROFILES_DIR, exist_ok=True)
    out_path = os.path.join(_PROFILES_DIR, f"{name}.json")
    with open(out_path, "w") as f:
        json.dump(result, f, indent=2)
    print(f"VR profile -> {out_path} ({len(result['per_node'])} nodes)")
    print(result["relu_vr"])
    return out_path


# =============================================================================
# Main
# =============================================================================

def main():
    parser = argparse.ArgumentParser(description="MobileNet FHE weights / VR profile")
    parser.add_argument("--name", default="mobilenet_cifar10",
                        help="ModelSpec name (also used as VR profile filename)")
    parser.add_argument("--epochs", type=int, default=EPOCHS, help="Training epochs")
    parser.add_argument("--batch", type=int, default=BATCH, help="Batch size")
    parser.add_argument("--seed", type=int, default=SEED, help="Random seed")
    parser.add_argument("--profile-only", action="store_true",
                        help="Only regenerate the ReLU VR profile from the shipped weights (real CIFAR-10)")
    args = parser.parse_args()

    out_path = os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE)
    if args.profile_only:
        _profile_real(args.name)
        return

    torch.manual_seed(args.seed)
    train_x, train_y = _load_cifar10_split("train")
    test_x, test_y = _load_cifar10_split("test")
    print(f"data: train={tuple(train_x.shape)} test={tuple(test_x.shape)}")

    model = MobileNet(num_classes=10, in_channels=3)
    n_params = sum(p.numel() for p in model.parameters())
    print(f"params: {n_params:,}")

    _train(model, train_x, train_y, test_x, test_y, epochs=args.epochs, batch=args.batch)
    model.eval()
    test_acc = _evaluate(model, test_x, test_y)
    print(f"final test acc = {test_acc:.4f}")
    if test_acc < 0.85:
        raise RuntimeError(
            f"Trained accuracy {test_acc:.4f} below the 0.85 gate — train longer "
            "(--epochs 60) or investigate before shipping weights."
        )

    os.makedirs(_WEIGHTS_DIR, exist_ok=True)
    torch.save(model.state_dict(), out_path)
    print(f"saved weights -> {out_path}")

    _profile_real(args.name)


if __name__ == "__main__":
    main()
