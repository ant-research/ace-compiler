#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Import the trained SqueezeNet weights from the reference ONNX model.

The shipped ``weights/squeezenet_cifar10.pt`` is extracted from the
reference model ``model/squeezenet_cifar10.onnx`` (ace-compiler-iiecas) —
the same weights behind the IIE accuracy table (plaintext ~93% top-1 on
CIFAR-10).

The reference graph has already been BN-folded by ``onnxsim``/the exporter:
every Conv carries its fused bias and there are no BatchNorm initializers.
Our torch module therefore uses ``bias=True`` convs and no BN at all, so the
mapping is a straight 1:1 copy of the 26 Conv weight/bias pairs plus Gemm.

Reference graph (opset 10, 59 nodes):
    26 Conv / 18 Relu / 8 Concat / 4 AveragePool / 1 GlobalAveragePool
    / 1 Reshape / 1 Gemm (transB=1)

Usage:
    python3 fhe_dsl/python/model/squeezenet/import_reference_weights.py \\
        /path/to/squeezenet_cifar10.onnx

Then regenerate the ReLU VR profile (needed — VR values are weight- and
data-dependent):

    python3 fhe_dsl/python/model/squeezenet/train_squeezenet.py --profile-only
"""

import argparse
import os
import pickle
import sys

import numpy as np
import onnx
import torch
from onnx import numpy_helper

from .squeezenet import SqueezeNet


_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "weights")
_WEIGHTS_FILE = "squeezenet_cifar10.pt"

# ImageNet mean/std — the reference model was trained with this normalization
# (the classic IIE pipeline normalizes CIFAR-10 with ImageNet statistics).
_MEAN = np.array([0.485, 0.456, 0.406], dtype=np.float32).reshape(1, 3, 1, 1)
_STD = np.array([0.229, 0.224, 0.225], dtype=np.float32).reshape(1, 3, 1, 1)


def _initializers(model: onnx.ModelProto) -> dict[str, torch.Tensor]:
    return {
        init.name: torch.from_numpy(numpy_helper.to_array(init).copy())
        for init in model.graph.initializer
    }


def _conv_modules(model: SqueezeNet) -> list[torch.nn.Conv2d]:
    """Every Conv2d in reference graph order (conv1 → fire2..fire9 → conv10)."""
    mods = [model.conv1]
    for fire in (model.fire2, model.fire3, model.fire4, model.fire5,
                 model.fire6, model.fire7, model.fire8, model.fire9):
        mods += [fire.squeeze, fire.expand1x1, fire.expand3x3]
    mods.append(model.conv10)
    return mods


def extract_state_dict(onnx_path: str) -> dict[str, torch.Tensor]:
    """Map the reference ONNX initializers onto SqueezeNet's state_dict."""
    graph = onnx.load(onnx_path)
    inits = _initializers(graph)

    model = SqueezeNet(10, 3)
    conv_mods = _conv_modules(model)
    convs = [n for n in graph.graph.node if n.op_type == "Conv"]
    gemms = [n for n in graph.graph.node if n.op_type == "Gemm"]

    if len(convs) != 26 or len(gemms) != 1:
        raise ValueError(
            f"Unexpected graph: {len(convs)} Conv / {len(gemms)} Gemm nodes, expected 26 / 1"
        )

    state = model.state_dict()
    # Map each Conv module to its dotted state_dict prefix (e.g. "fire2.squeeze").
    prefix_of = {
        id(module): name
        for name, module in model.named_modules()
        if isinstance(module, torch.nn.Conv2d)
    }

    for module, node in zip(conv_mods, convs):
        if len(node.input) < 3:
            raise ValueError(f"Conv {node.name!r}: expected fused bias, got inputs {list(node.input)}")
        w_name, b_name = node.input[1], node.input[2]
        if w_name not in inits or b_name not in inits:
            raise ValueError(f"Conv {node.name!r}: missing initializer {w_name!r}/{b_name!r}")
        w = inits[w_name]
        b = inits[b_name]
        prefix = prefix_of[id(module)]
        if tuple(module.weight.shape) != tuple(w.shape):
            raise ValueError(
                f"{prefix}.weight: torch {tuple(module.weight.shape)} != ONNX {tuple(w.shape)} ({node.name!r})"
            )
        state[f"{prefix}.weight"] = w.clone()
        state[f"{prefix}.bias"] = b.clone()

    gemm = gemms[0]
    attrs = {a.name: onnx.helper.get_attribute_value(a) for a in gemm.attribute}
    if not attrs.get("transB", 0):
        raise ValueError(f"Gemm {gemm.name!r}: expected transB=1 (torch Linear layout)")
    w = inits[gemm.input[1]]
    b = inits[gemm.input[2]]
    if tuple(model.fc.weight.shape) != tuple(w.shape):
        raise ValueError(f"fc.weight: torch {tuple(model.fc.weight.shape)} != ONNX {tuple(w.shape)}")
    state["fc.weight"] = w.clone()
    state["fc.bias"] = b.clone()

    return state


def _load_cifar10_test() -> tuple[torch.Tensor, np.ndarray] | None:
    try:
        from ace.model.dataset import _find_cifar10_test_batch

        with open(_find_cifar10_test_batch(), "rb") as f:
            data = pickle.load(f, encoding="bytes")
    except (ImportError, FileNotFoundError, OSError) as exc:
        print(f"[verify] CIFAR-10 unavailable ({exc}); skipping accuracy check")
        return None

    x = data[b"data"].reshape(-1, 3, 32, 32).astype(np.float32) / 255.0
    y = np.asarray(data[b"labels"])
    images = torch.from_numpy(((x - _MEAN) / _STD).astype(np.float32))
    return images, y


def _verify_accuracy(state: dict[str, torch.Tensor]) -> float | None:
    """Top-1 on the CIFAR-10 test set (ImageNet normalization), if available."""
    loaded = _load_cifar10_test()
    if loaded is None:
        return None
    images, y = loaded

    model = SqueezeNet(10, 3)
    model.load_state_dict(state)
    model.eval()
    with torch.no_grad():
        pred = model(images).argmax(dim=1).numpy()
    return float((pred == y).mean())


def main() -> int:
    parser = argparse.ArgumentParser(description="Import trained SqueezeNet weights from the reference ONNX model")
    parser.add_argument("onnx_path", help="Path to the reference squeezenet_cifar10.onnx")
    parser.add_argument(
        "--output",
        default=os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE),
        help=f"Output .pt path (default: {os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE)})",
    )
    args = parser.parse_args()

    state = extract_state_dict(args.onnx_path)

    acc = _verify_accuracy(state)
    if acc is not None:
        print(f"[verify] CIFAR-10 top-1 (plaintext, ImageNet norm): {acc:.4f}")
        if acc < 0.90:
            print("[verify] ERROR: accuracy far below the reference ~0.93 — wrong ONNX file?", file=sys.stderr)
            return 1

    os.makedirs(os.path.dirname(args.output), exist_ok=True)
    torch.save(state, args.output)
    print(f"[save] {args.output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
