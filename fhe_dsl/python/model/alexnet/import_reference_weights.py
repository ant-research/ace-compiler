#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Import the trained AlexNet weights from the reference ONNX model.

The shipped ``weights/alexnet_cifar10.pt`` is NOT produced by
``train_alexnet.py`` (that script trains a synthetic-data rehearsal model).
It is extracted from the classic IIE reference model
``model/alexnet_cifar10.onnx`` in the ``ace-compiler-iiecas`` repo — the same
weights the ASPLOS/IIE accuracy table was measured on (85/86% top-1 on
CIFAR-10).

Why a separate script: ``alexnet_cifar10.onnx`` stores the parameters as
graph initializers with exporter-generated names (``onnx::Conv_72``,
``classifier.1.weight``, ...) and uses ``Gemm(transB=1)``. This script maps
them back onto our ``AlexNet`` state_dict layout by graph order.

Usage:
    python3 fhe_dsl/python/model/alexnet/import_reference_weights.py \\
        /path/to/alexnet_cifar10.onnx

Then regenerate the ReLU VR profile (needed — the old profile was fitted on
the synthetic weights):

    python3 fhe_dsl/python/model/alexnet/train_alexnet.py --profile-only
"""

import argparse
import os
import sys

import numpy as np
import onnx
import torch
from onnx import numpy_helper

from .alexnet import AlexNet

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "weights")
_WEIGHTS_FILE = "alexnet_cifar10.pt"

# AlexNet module indices for the 5 convs / 3 linears, in graph order.
_CONV_TARGETS = ["features.0", "features.3", "features.6", "features.8", "features.10"]
_LINEAR_TARGETS = ["classifier.0", "classifier.2", "classifier.4"]


def _initializers(model: onnx.ModelProto) -> dict[str, torch.Tensor]:
    return {
        init.name: torch.from_numpy(numpy_helper.to_array(init).copy())
        for init in model.graph.initializer
    }


def extract_state_dict(onnx_path: str) -> dict[str, torch.Tensor]:
    """Map the reference ONNX initializers onto AlexNet's state_dict."""
    model = onnx.load(onnx_path)
    inits = _initializers(model)

    def param(node, slot: int, kind: str) -> torch.Tensor:
        name = node.input[slot]
        if name not in inits:
            raise ValueError(f"{kind} node {node.name!r}: missing initializer {name!r}")
        return inits[name]

    convs = [n for n in model.graph.node if n.op_type == "Conv"]
    gemms = [n for n in model.graph.node if n.op_type == "Gemm"]
    if len(convs) != len(_CONV_TARGETS) or len(gemms) != len(_LINEAR_TARGETS):
        raise ValueError(
            f"Unexpected graph: {len(convs)} Conv / {len(gemms)} Gemm nodes, "
            f"expected {len(_CONV_TARGETS)} / {len(_LINEAR_TARGETS)}"
        )

    state: dict[str, torch.Tensor] = {}
    for target, node in zip(_CONV_TARGETS, convs):
        state[f"{target}.weight"] = param(node, 1, "Conv").clone()
        state[f"{target}.bias"] = param(node, 2, "Conv").clone()

    for target, node in zip(_LINEAR_TARGETS, gemms):
        attrs = {a.name: onnx.helper.get_attribute_value(a) for a in node.attribute}
        if not attrs.get("transB", 0):
            raise ValueError(f"Gemm {node.name!r}: expected transB=1 (torch Linear layout)")
        # transB=1 means the initializer is already (out_features, in_features).
        state[f"{target}.weight"] = param(node, 1, "Gemm").clone()
        state[f"{target}.bias"] = param(node, 2, "Gemm").clone()

    return state


def _verify_accuracy(state: dict[str, torch.Tensor]) -> float | None:
    """Top-1 on the CIFAR-10 test set, if it is available."""
    try:
        from ace.model.dataset import load_cifar10_images

        images, labels = load_cifar10_images(10000)
    except (ImportError, FileNotFoundError, OSError) as exc:
        print(f"[verify] CIFAR-10 unavailable ({exc}); skipping accuracy check")
        return None

    model = AlexNet(10, 3)
    model.load_state_dict(state)
    model.eval()
    with torch.no_grad():
        pred = model(images).argmax(dim=1).numpy()
    return float((pred == np.asarray(labels)).mean())


def main() -> int:
    parser = argparse.ArgumentParser(description="Import trained AlexNet weights from the reference ONNX model")
    parser.add_argument("onnx_path", help="Path to the reference alexnet_cifar10.onnx")
    parser.add_argument(
        "--output",
        default=os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE),
        help=f"Output .pt path (default: {os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE)})",
    )
    args = parser.parse_args()

    state = extract_state_dict(args.onnx_path)

    acc = _verify_accuracy(state)
    if acc is not None:
        print(f"[verify] CIFAR-10 top-1 (plaintext): {acc:.4f}")
        if acc < 0.80:
            print("[verify] ERROR: accuracy far below the reference ~0.86 — wrong ONNX file?", file=sys.stderr)
            return 1

    os.makedirs(os.path.dirname(args.output), exist_ok=True)
    torch.save(state, args.output)
    print(f"[save] {args.output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())