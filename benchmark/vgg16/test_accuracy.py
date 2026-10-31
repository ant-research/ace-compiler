# benchmark/vgg16/test_accuracy.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# VGG16 FHE accuracy regression benchmarks.
#
# Mirrors benchmark/resnet/test_accuracy.py. VGG16 uses
# ``frontend="torch-via-onnx"`` and is ant-cpu only (conftest defines
# VGG16_TARGET_PARAMS with GPU backends skipped at collection time).
#
# VGG16 is very deep (13 conv + 5 avg-pool + linear), so
# ant-CPU inference is extremely slow (the reference build declares ~730 GB
# RAM for vgg16_cifar10). Tests are marked @pytest.mark.slow and may take
# many minutes.
#
# Uses pytest-regressions data_regression to store and compare accuracy
# baselines. Only ant-cpu generates real baseline data.
#
# VGG16_TARGET_PARAMS is defined in _fixtures.py so test files can
# import it via the standard ``from _fixtures import ...`` pattern
# (same as ResNet imports TARGET_PARAMS from _fixtures).
#
# Usage:
#   pytest benchmark/vgg16/test_accuracy.py -v
#   pytest benchmark/vgg16/test_accuracy.py -k "ant-cpu-100 and not 1000" -v
#   pytest benchmark/vgg16/test_accuracy.py --force-regen   # update baselines

import pytest
import torch

from _fixtures import VGG16_TARGET_PARAMS


# ============================================================================
# Helper
# ============================================================================


def _compute_accuracy_data(program, model, images, labels, class_names=None):
    """Run FHE inference and compute accuracy comparison dict.

    Returns a dict suitable for data_regression.check().
    """
    n = len(labels)
    result = program.run_dataset(images[:n], labels[:n], top_k=1, verbose=False)

    # Compute plaintext predictions
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


# ============================================================================
# VGG16 accuracy tests — same structure as ResNet test classes.
#
# compiled_vgg16 fixture is defined in conftest.py with session scope.
# We re-parameterize with VGG16_TARGET_PARAMS (ant-cpu + skipped GPU).
# The fixture uses indirect=True so pytest passes (library, device) to it.
#
# pytest-regression's cifar10_images fixture (from benchmark/conftest.py)
# provides the shared dataset across all model benchmarks.
# ============================================================================


@pytest.mark.slow
class TestVGG16Accuracy:
    """VGG16 CIFAR-10 accuracy tests. Ant-cpu only; GPU targets skipped."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    @pytest.mark.parametrize("compiled_vgg16", VGG16_TARGET_PARAMS, indirect=True)
    def test_accuracy(self, num_images, compiled_vgg16, vgg16_model, cifar10_images, data_regression):
        program, library, device = compiled_vgg16
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(program, vgg16_model, images[:num_images], labels[:num_images])
        data_regression.check(data, f"vgg16_cifar10_{library}_{device}_{num_images}img")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.parametrize("compiled_vgg16", VGG16_TARGET_PARAMS, indirect=True)
    @pytest.mark.slow
    def test_accuracy_slow(self, num_images, compiled_vgg16, vgg16_model, cifar10_images, data_regression):
        program, library, device = compiled_vgg16
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(program, vgg16_model, images[:num_images], labels[:num_images])
        data_regression.check(data, f"vgg16_cifar10_{library}_{device}_{num_images}img")