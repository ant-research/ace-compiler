# benchmark/lenet/test_accuracy.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# LeNet FHE accuracy regression benchmarks.
#
# Uses pytest-regressions data_regression to store and compare accuracy baselines.
# Usage:
#   pytest benchmark/lenet/test_accuracy.py -v
#   pytest benchmark/lenet/test_accuracy.py -k phantom -v
#   pytest benchmark/lenet/test_accuracy.py --force-regen   # update baselines

import pytest
import torch

from _fixtures import _load_images
from ace.model.spec_lenet import LENET_CIFAR10


# SMALL_TARGET_PARAMS is defined in benchmark/lenet/conftest.py (auto-loaded by pytest)
# and parametrizes over [("ant", "cpu"), ("phantom", "cuda")].


def _compute_accuracy_data(program, model, images, labels):
    """Run FHE inference and compute accuracy comparison dict."""
    n = len(labels)
    result = program.run_dataset(images[:n], labels[:n], top_k=1, verbose=False)

    plain_correct = 0
    fhe_match_count = 0
    mismatches = []
    with torch.no_grad():
        for i in range(n):
            plain_pred = model(images[i : i + 1]).argmax(dim=1).item()
            fhe_pred = result.predictions[i]
            if plain_pred == labels[i]:
                plain_correct += 1
            if fhe_pred == plain_pred:
                fhe_match_count += 1
            else:
                mismatches.append(i)

    data = {
        "fhe_correct": result.num_correct_top1,
        "plaintext_correct": plain_correct,
        "fhe_match_count": fhe_match_count,
        "top1_accuracy": round(result.top1_accuracy, 4),
        "num_images": n,
        "fhe_mismatch": sorted(mismatches),
    }
    print(
        f"\n  accuracy: top1={data['top1_accuracy']:.4f}  "
        f"fhe_correct={data['fhe_correct']}/{n}  "
        f"plain_correct={data['plaintext_correct']}/{n}  "
        f"mismatch={data['fhe_mismatch'] or 'none'}"
    )
    return data


@pytest.fixture(scope="session")
def lenet_cifar10_images():
    """Load CIFAR-10 test images (up to 1000)."""
    images, labels, class_names = _load_images("cifar10", 1000)
    return images, labels, class_names


@pytest.mark.slow
class TestLeNetAccuracy:
    """LeNet CIFAR-10 accuracy tests across backends."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    @pytest.mark.parametrize("compiled_lenet", [
        pytest.param(("ant", "cpu"), id="ant-cpu"),
        pytest.param(("phantom", "cuda"), id="phantom-cuda"),
    ], indirect=True)
    def test_accuracy(self, num_images, compiled_lenet, lenet_model, lenet_cifar10_images, data_regression):
        program, library, device = compiled_lenet
        images, labels, _ = lenet_cifar10_images
        data = _compute_accuracy_data(program, lenet_model, images[:num_images], labels[:num_images])
        data_regression.check(data, f"lenet_cifar10_{library}_{device}_{num_images}img")
