#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Train and save weights + VR profile for the SqueezeNet FHE model.

The shipped SqueezeNet weights are the real reference-trained ones imported
by ``import_reference_weights.py`` (plaintext ~93% top-1 on CIFAR-10), NOT
the synthetic rehearsal model this script can train. The main job here is
regenerating the ReLU VR profile:

    python3 -m model.squeezenet.train_squeezenet --profile-only

WHY ONNX-LEVEL PROFILING: ``ReLUProfiler`` uses FX tracing, which names
ReLU nodes after torch modules (``fire2_relu_Relu``). SqueezeNet is compiled
with ``frontend="torch-via-onnx"``, and that frontend wraps the module's
forward in ``functools.wraps(model.forward)`` before calling the legacy
ONNX exporter — the resulting node names the SIHE backend sees are
``/Relu``, ``/Relu_1``, ... in graph order. Those FX names never match, and
every ReLU would silently fall back to ``relu_vr_def`` (verified in the
generated .c). This script therefore profiles the EXACT ONNX the compiler
produces, using ONNX Runtime with each Relu input exposed as a graph output.

Synthetic training (rehearsal path, requires --force-synthetic):

    python3 -m model.squeezenet.train_squeezenet --force-synthetic

Usage (from fhe_dsl/python):
    python3 -m model.squeezenet.train_squeezenet --profile-only
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

from .squeezenet import SqueezeNet


_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "weights")
_PROFILES_DIR = os.path.join(os.path.dirname(__file__), "profiles")
_WEIGHTS_FILE = "squeezenet_cifar10.pt"

SEED = 2026
STEPS = 1200
BATCH = 64
LR = 0.005


# =============================================================================
# VR profiling (ONNX level — matches the compiler's node names)
# =============================================================================


def profile_vr_onnx(model: nn.Module, inputs: torch.Tensor, margin: int = 1, relu_vr_def: int = 3) -> dict:
    """Profile pre-ReLU abs_max on the exact ONNX the torch-via-onnx frontend emits.

    Args:
        model: SqueezeNet with the shipped weights loaded, in eval mode.
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
        onnx_path = os.path.join(tmpdir, "squeezenet.onnx")
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
        probe_path = os.path.join(tmpdir, "squeezenet_probe.onnx")
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
            f"{path} not found. Import it first:\n"
            "  python3 -m model.squeezenet.import_reference_weights <squeezenet_cifar10.onnx>"
        )
    model = SqueezeNet(num_classes=10, in_channels=3)
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
# Synthetic rehearsal training (fallback / smoke path)
# =============================================================================


def _synthetic_cifar_like(n_per_class: int = 48, noise: float = 0.05):
    """Deterministic CIFAR-shaped data with wide class margins."""
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
        logits = model(xs)
        acc = (logits.argmax(dim=1) == ys).float().mean().item()
        top = logits.topk(2, dim=1).values
        margin = (top[:, 0] - top[:, 1]).min().item()
    return acc, margin


def main():
    parser = argparse.ArgumentParser(description="SqueezeNet FHE weights / VR profile")
    parser.add_argument("--name", default="squeezenet_cifar10", help="ModelSpec name (VR profile filename)")
    parser.add_argument(
        "--profile-only",
        action="store_true",
        help="Only regenerate the ReLU VR profile from the shipped weights (real CIFAR-10)",
    )
    parser.add_argument(
        "--force-synthetic",
        action="store_true",
        help="Allow overwriting the real reference weights with the synthetic rehearsal model",
    )
    args = parser.parse_args()

    if args.profile_only:
        _profile_real(args.name)
        return

    # The committed weights are the real reference-trained SqueezeNet (~93%
    # CIFAR-10). Training the synthetic model below would silently replace
    # them with a network that is useless on real images.
    if not args.force_synthetic:
        parser.error(
            "Refusing to overwrite the shipped real-CIFAR weights with the synthetic "
            "rehearsal model. Use --profile-only to regenerate the VR profile, or "
            "--force-synthetic to intentionally overwrite the weights."
        )

    torch.manual_seed(SEED)
    xs, ys = _synthetic_cifar_like()
    print(f"data: {tuple(xs.shape)} labels=[{ys.min().item()},{ys.max().item()}]")

    model = SqueezeNet(num_classes=10, in_channels=3)
    n_params = sum(p.numel() for p in model.parameters())
    print(f"params: {n_params:,}")
    acc, margin = _train(model, xs, ys)
    print(f"train acc={acc:.4f} min margin={margin:.4f}")
    if acc < 0.95 or margin < 0.2:
        raise RuntimeError(f"Poor separation: acc={acc}, margin={margin}. Train more / widen margins.")

    os.makedirs(_WEIGHTS_DIR, exist_ok=True)
    path = os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE)
    torch.save(model.state_dict(), path)
    print(f"saved weights -> {path}")

    result = profile_vr_onnx(model, xs)
    os.makedirs(_PROFILES_DIR, exist_ok=True)
    out_path = os.path.join(_PROFILES_DIR, f"{args.name}.json")
    with open(out_path, "w") as f:
        json.dump(result, f, indent=2)
    print(f"VR profile -> {out_path}")


if __name__ == "__main__":
    main()
