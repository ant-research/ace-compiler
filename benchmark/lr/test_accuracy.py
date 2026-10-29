# benchmark/lr/test_accuracy.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# LR FHE accuracy regression benchmarks.
#
# Uses pytest-regressions data_regression to store and compare accuracy baselines.
# Usage:
#   pytest benchmark/lr/test_accuracy.py -v
#   pytest benchmark/lr/test_accuracy.py -k phantom -v
#   pytest benchmark/lr/test_accuracy.py --force-regen   # update baselines

import pytest
import torch

from _fixtures import TARGET_PARAMS
from ace.model.spec_lr import LR_1D


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
def lr_model():
    """Session-scoped LR model for plaintext inference."""
    model = LR_1D.create_model()
    model.eval()
    return model


@pytest.mark.slow
class TestLRAccuracy:
    """LR accuracy tests across backends."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    @pytest.mark.parametrize("compiled_lr", TARGET_PARAMS, indirect=True)
    def test_accuracy(self, num_images, compiled_lr, lr_model, data_regression):
        program, library, device = compiled_lr
        x = LR_1D.example_inputs[0]
        images = x.repeat(num_images, 1)
        labels = [0] * num_images
        data = _compute_accuracy_data(program, lr_model, images, labels)
        data_regression.check(data, f"lr_1d_{library}_{device}_{num_images}img")
