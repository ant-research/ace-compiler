#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Import the trained VGG16 weights from the reference ONNX model.

The shipped ``weights/vgg16_cifar10.pt`` is NOT produced by
``train_vgg16.py`` (that script trains a synthetic-data rehearsal model).
It is extracted from the classic IIE reference model ``vgg16_cifar10.onnx``
in the ``ace-compiler-iiecas`` repo — the same weights the ASPLOS/IIE
accuracy table was measured on (~96% top-1 on CIFAR-10, ImageNet norm).

The reference graph has already been BN-folded by the exporter: Conv carries
the fused bias and there are no BatchNorm initializers. We populate the
VGG16 module's BN layers with an identity transform (running_var=1,
running_mean=0, gamma=1, beta=0) so the compiler's BN folding pass is a
no-op and the folded conv is exactly the reference conv.

Usage:
    python3 fhe_dsl/python/model/vgg16/import_reference_weights.py \\
        /path/to/vgg16_cifar10.onnx

Then regenerate the ReLU VR profile (needed — the old profile was fitted on
the synthetic weights):

    python3 fhe_dsl/python/model/vgg16/train_vgg16.py --profile-only
"""

import argparse
import os
import sys

import numpy as np
import onnx
import torch
from onnx import numpy_helper

from .vgg16 import VGG16

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "weights")
_WEIGHTS_FILE = "vgg16_cifar10.pt"


def _initializers(model: onnx.ModelProto) -> dict[str, torch.Tensor]:
    return {
        init.name: torch.from_numpy(numpy_helper.to_array(init).copy())
        for init in model.graph.initializer
    }


def extract_state_dict(onnx_path: str) -> dict[str, torch.Tensor]:
    """Map the reference ONNX initializers onto VGG16's BN-inclusive state_dict."""
    model = onnx.load(onnx_path)
    inits = _initializers(model)

    sd = VGG16(10, 3).state_dict()

    # Identity BatchNorm everywhere; conv params overwrite weight/bias below.
    for key in list(sd):
        if ".running_mean" in key:
            sd[key] = torch.zeros_like(sd[key])
        elif ".running_var" in key:
            sd[key] = torch.ones_like(sd[key])
        elif ".num_batches_tracked" in key:
            sd[key] = torch.zeros_like(sd[key])
        elif key.endswith(".weight") and key.count(".") == 1 and "features" in key:
            # BN weight (e.g. features.1.weight) — identity scale
            sd[key] = torch.ones_like(sd[key])
        elif key.endswith(".bias") and key.count(".") == 1 and "features" in key:
            # BN bias (e.g. features.1.bias) — zero shift
            sd[key] = torch.zeros_like(sd[key])

    convs = [n for n in model.graph.node if n.op_type == "Conv"]
    gemms = [n for n in model.graph.node if n.op_type == "Gemm"]
    if len(convs) != 13 or len(gemms) != 1:
        raise ValueError(f"Unexpected graph: {len(convs)} Conv / {len(gemms)} Gemm nodes, expected 13 / 1")

    for node in convs:
        block = node.name.split("/")[2]  # e.g. "features.0"
        w_name, b_name = node.input[1], node.input[2]
        if w_name not in inits or b_name not in inits:
            raise ValueError(f"Conv {node.name!r}: missing initializer {w_name!r}/{b_name!r}")
        key = f"{block}.weight"
        if sd[key].shape != inits[w_name].shape:
            raise ValueError(f"{key}: shape {tuple(sd[key].shape)} != ONNX {tuple(inits[w_name].shape)}")
        sd[key] = inits[w_name].clone()
        sd[f"{block}.bias"] = inits[b_name].clone()

    gemm = gemms[0]
    attrs = {a.name: onnx.helper.get_attribute_value(a) for a in gemm.attribute}
    if not attrs.get("transB", 0):
        raise ValueError(f"Gemm {gemm.name!r}: expected transB=1 (torch Linear layout)")
    sd["classifier.weight"] = inits[gemm.input[1]].clone()
    sd["classifier.bias"] = inits[gemm.input[2]].clone()

    return sd


def _verify_accuracy(state: dict[str, torch.Tensor]) -> float | None:
    """Top-1 on the CIFAR-10 test set (ImageNet normalization), if available."""
    try:
        from ace.model.dataset import _find_cifar10_test_batch

        import pickle

        with open(_find_cifar10_test_batch(), "rb") as f:
            data = pickle.load(f, encoding="bytes")
    except (ImportError, FileNotFoundError, OSError) as exc:
        print(f"[verify] CIFAR-10 unavailable ({exc}); skipping accuracy check")
        return None

    x = data[b"data"].reshape(-1, 3, 32, 32).astype(np.float32) / 255.0
    y = np.asarray(data[b"labels"])
    mean = np.array([0.485, 0.456, 0.406], dtype=np.float32).reshape(1, 3, 1, 1)
    std = np.array([0.229, 0.224, 0.225], dtype=np.float32).reshape(1, 3, 1, 1)
    images = torch.from_numpy(((x - mean) / std).astype(np.float32))

    model = VGG16(10, 3)
    model.load_state_dict(state)
    model.eval()
    with torch.no_grad():
        pred = model(images).argmax(dim=1).numpy()
    return float((pred == y).mean())


def main() -> int:
    parser = argparse.ArgumentParser(description="Import trained VGG16 weights from the reference ONNX model")
    parser.add_argument("onnx_path", help="Path to the reference vgg16_cifar10.onnx")
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
            print("[verify] ERROR: accuracy far below the reference ~0.96 — wrong ONNX file?", file=sys.stderr)
            return 1

    os.makedirs(os.path.dirname(args.output), exist_ok=True)
    torch.save(state, args.output)
    print(f"[save] {args.output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())