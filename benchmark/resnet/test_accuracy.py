# benchmark/resnet/test_accuracy.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ResNet FHE accuracy regression benchmarks.
#
# Uses pytest-regressions data_regression to store and compare accuracy baselines.
# Each (model, backend, image_count) combination gets its own YAML baseline file.
#
# Usage:
#   pytest benchmark/resnet/test_accuracy.py -v
#   pytest benchmark/resnet/test_accuracy.py -k resnet20 -v
#   pytest benchmark/resnet/test_accuracy.py --force-regen   # update baselines
#   pytest benchmark/resnet/test_accuracy.py -m "not slow"   # skip 1000-img tests

import pytest
import torch

from conftest import requires_torch_fx, requires_gpu


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
            plain_pred = model(images[i:i+1]).argmax(dim=1).item()
            fhe_pred = result.predictions[i]
            if plain_pred == labels[i]:
                plain_correct += 1
            if fhe_pred == plain_pred:
                fhe_match_count += 1
            else:
                mismatches.append(i)

    return {
        "fhe_correct": result.num_correct_top1,
        "plaintext_correct": plain_correct,
        "fhe_match_count": fhe_match_count,
        "top1_accuracy": round(result.top1_accuracy, 4),
        "num_images": n,
        "fhe_mismatch": sorted(mismatches),
    }


# ============================================================================
# ResNet-20 accuracy tests
# ============================================================================

@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet20Accuracy:
    """ResNet-20 CIFAR-10 accuracy tests."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    @pytest.mark.parametrize("compiled_resnet_20", ["phantom"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_20,
                      resnet20_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_20, resnet20_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet20_cifar10_phantom_cuda_{num_images}img")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.parametrize("compiled_resnet_20", ["phantom"], indirect=True)
    @pytest.mark.slow
    def test_accuracy_slow(self, num_images, compiled_resnet_20,
                          resnet20_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_20, resnet20_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet20_cifar10_phantom_cuda_{num_images}img")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet32Accuracy:
    """ResNet-32 CIFAR-10 accuracy tests."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    @pytest.mark.parametrize("compiled_resnet_32", ["phantom"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_32,
                      resnet32_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_32, resnet32_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet32_cifar10_phantom_cuda_{num_images}img")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.parametrize("compiled_resnet_32", ["phantom"], indirect=True)
    @pytest.mark.slow
    def test_accuracy_slow(self, num_images, compiled_resnet_32,
                           resnet32_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_32, resnet32_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet32_cifar10_phantom_cuda_{num_images}img")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet44Accuracy:
    """ResNet-44 CIFAR-10 accuracy tests."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    @pytest.mark.parametrize("compiled_resnet_44", ["phantom"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_44,
                      resnet44_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_44, resnet44_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet44_cifar10_phantom_cuda_{num_images}img")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.parametrize("compiled_resnet_44", ["phantom"], indirect=True)
    @pytest.mark.slow
    def test_accuracy_slow(self, num_images, compiled_resnet_44,
                           resnet44_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_44, resnet44_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet44_cifar10_phantom_cuda_{num_images}img")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet56Accuracy:
    """ResNet-56 CIFAR-10 accuracy tests."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    @pytest.mark.parametrize("compiled_resnet_56", ["phantom"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_56,
                      resnet56_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_56, resnet56_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet56_cifar10_phantom_cuda_{num_images}img")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.parametrize("compiled_resnet_56", ["phantom"], indirect=True)
    @pytest.mark.slow
    def test_accuracy_slow(self, num_images, compiled_resnet_56,
                           resnet56_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_56, resnet56_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet56_cifar10_phantom_cuda_{num_images}img")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet110Accuracy:
    """ResNet-110 CIFAR-10 accuracy tests."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    @pytest.mark.parametrize("compiled_resnet_110", ["phantom"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_110,
                      resnet110_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_110, resnet110_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet110_cifar10_phantom_cuda_{num_images}img")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.parametrize("compiled_resnet_110", ["phantom"], indirect=True)
    @pytest.mark.slow
    def test_accuracy_slow(self, num_images, compiled_resnet_110,
                           resnet110_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_110, resnet110_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet110_cifar10_phantom_cuda_{num_images}img")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet32Cifar100Accuracy:
    """ResNet-32 CIFAR-100 accuracy tests."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    @pytest.mark.parametrize("compiled_resnet_32_cifar100", ["phantom"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_32_cifar100,
                      resnet32_cifar100_model, cifar100_images, data_regression):
        images, labels, _ = cifar100_images
        data = _compute_accuracy_data(
            compiled_resnet_32_cifar100, resnet32_cifar100_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet32_cifar100_phantom_cuda_{num_images}img")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.parametrize("compiled_resnet_32_cifar100", ["phantom"], indirect=True)
    @pytest.mark.slow
    def test_accuracy_slow(self, num_images, compiled_resnet_32_cifar100,
                           resnet32_cifar100_model, cifar100_images, data_regression):
        images, labels, _ = cifar100_images
        data = _compute_accuracy_data(
            compiled_resnet_32_cifar100, resnet32_cifar100_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet32_cifar100_phantom_cuda_{num_images}img")


# ============================================================================
# Ant (CPU) accuracy tests
# ============================================================================

@requires_torch_fx
@pytest.mark.slow
class TestResNet20AccuracyAnt:
    """ResNet-20 CIFAR-10 accuracy on ant/cpu."""

    @pytest.mark.parametrize("num_images", [1, 10])
    @pytest.mark.parametrize("compiled_resnet_20", ["ant"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_20,
                      resnet20_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_20, resnet20_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet20_cifar10_ant_cpu_{num_images}img")


@requires_torch_fx
@pytest.mark.slow
class TestResNet32AccuracyAnt:
    """ResNet-32 CIFAR-10 accuracy on ant/cpu."""

    @pytest.mark.parametrize("num_images", [1, 10])
    @pytest.mark.parametrize("compiled_resnet_32", ["ant"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_32,
                      resnet32_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_32, resnet32_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet32_cifar10_ant_cpu_{num_images}img")


@requires_torch_fx
@pytest.mark.slow
class TestResNet44AccuracyAnt:
    """ResNet-44 CIFAR-10 accuracy on ant/cpu."""

    @pytest.mark.parametrize("num_images", [1, 10])
    @pytest.mark.parametrize("compiled_resnet_44", ["ant"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_44,
                      resnet44_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_44, resnet44_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet44_cifar10_ant_cpu_{num_images}img")


@requires_torch_fx
@pytest.mark.slow
class TestResNet56AccuracyAnt:
    """ResNet-56 CIFAR-10 accuracy on ant/cpu."""

    @pytest.mark.parametrize("num_images", [1, 10])
    @pytest.mark.parametrize("compiled_resnet_56", ["ant"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_56,
                      resnet56_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_56, resnet56_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet56_cifar10_ant_cpu_{num_images}img")


@requires_torch_fx
@pytest.mark.slow
class TestResNet110AccuracyAnt:
    """ResNet-110 CIFAR-10 accuracy on ant/cpu."""

    @pytest.mark.parametrize("num_images", [1, 10])
    @pytest.mark.parametrize("compiled_resnet_110", ["ant"], indirect=True)
    def test_accuracy(self, num_images, compiled_resnet_110,
                      resnet110_model, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_accuracy_data(
            compiled_resnet_110, resnet110_model,
            images[:num_images], labels[:num_images],
        )
        data_regression.check(data, f"resnet110_cifar10_ant_cpu_{num_images}img")