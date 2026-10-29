#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
"""
Train and save weights + VR profile for the LeNet FHE smoke model.

This script trains LeNet on the real CIFAR-10 dataset (not synthetic templates)
so that the resulting weights give meaningful accuracy on CIFAR-10 benchmarks.
The saved weights and ReLU VR profile are consumed by LENET_CIFAR10 in
spec_lenet.py.

Usage:
    python3 -m model.lenet.train_lenet    (source tree, from fhe_dsl/python)
    python3 fhe_dsl/python/model/lenet/train_lenet.py
"""

import argparse
import os

import torch
import torch.nn as nn
import torchvision
import torchvision.transforms as transforms

from .lenet import LeNet

_WEIGHTS_DIR = os.path.join(os.path.dirname(__file__), "weights")
_PROFILES_DIR = os.path.join(os.path.dirname(__file__), "..", "profiles")
_WEIGHTS_FILE = "lenet_cifar10.pt"

SEED = 2026
EPOCHS = 50
BATCH = 128
LR = 0.001
DATASET_ROOT = "/opt/dataset"


def _load_cifar10():
    """Load normalized CIFAR-10 train/test sets matching ace.model.dataset."""
    mean = (0.4914, 0.4822, 0.4465)
    std = (0.2470, 0.2435, 0.2616)

    transform_train = transforms.Compose([
        transforms.RandomCrop(32, padding=4),
        transforms.RandomHorizontalFlip(),
        transforms.ToTensor(),
        transforms.Normalize(mean, std),
    ])
    transform_test = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize(mean, std),
    ])

    trainset = torchvision.datasets.CIFAR10(
        root=DATASET_ROOT, train=True, download=False, transform=transform_train
    )
    testset = torchvision.datasets.CIFAR10(
        root=DATASET_ROOT, train=False, download=False, transform=transform_test
    )
    trainloader = torch.utils.data.DataLoader(
        trainset, batch_size=BATCH, shuffle=True, num_workers=4
    )
    testloader = torch.utils.data.DataLoader(
        testset, batch_size=100, shuffle=False, num_workers=4
    )
    return trainloader, testloader


def _train(model, trainloader, testloader, epochs=EPOCHS, lr=LR):
    """Train with Adam + cosine annealing; return final test accuracy."""
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model.to(device)

    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=lr)
    scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=epochs)

    best_acc = 0.0
    for epoch in range(epochs):
        model.train()
        for inputs, labels in trainloader:
            inputs, labels = inputs.to(device), labels.to(device)
            optimizer.zero_grad()
            outputs = model(inputs)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()
        scheduler.step()

        model.eval()
        correct = 0
        total = 0
        with torch.no_grad():
            for inputs, labels in testloader:
                inputs, labels = inputs.to(device), labels.to(device)
                outputs = model(inputs)
                _, predicted = outputs.max(1)
                total += labels.size(0)
                correct += predicted.eq(labels).sum().item()
        acc = 100.0 * correct / total
        best_acc = max(best_acc, acc)
        print(f"Epoch {epoch + 1}/{epochs}: test accuracy = {acc:.2f}%")

    # Move model back to CPU before saving
    model.to("cpu")
    return best_acc


def _profile_vr(model, inputs, name):
    """Generate and save the ReLU VR profile via the project ReLUProfiler."""
    from ace.fhe.config import ReLUProfiler, ModelSpec

    # Use a real CIFAR-10 image as the spec example input
    from ace.model.dataset import load_cifar10_images

    example, _ = load_cifar10_images(1, offset=0)
    spec = ModelSpec(
        name=name,
        model_class=LeNet,
        example_inputs=(example,),
        encrypt_inputs=["x"],
        model_init_args=(10, 3),
    )
    profiler = ReLUProfiler(spec)
    return profiler.profile(inputs=inputs, margin=1, save=True)


def main():
    parser = argparse.ArgumentParser(description="Train LeNet FHE smoke weights")
    parser.add_argument(
        "--name",
        default="lenet_cifar10",
        help="ModelSpec name (also used as VR profile filename)",
    )
    args = parser.parse_args()

    torch.manual_seed(SEED)
    trainloader, testloader = _load_cifar10()
    print("Training LeNet on real CIFAR-10...")

    model = LeNet(num_classes=10, in_channels=3)
    best_acc = _train(model, trainloader, testloader)
    print(f"Best test accuracy: {best_acc:.2f}%")

    os.makedirs(_WEIGHTS_DIR, exist_ok=True)
    path = os.path.join(_WEIGHTS_DIR, _WEIGHTS_FILE)
    torch.save(model.state_dict(), path)
    print(f"saved weights -> {path}")

    # Build a tensor of 256 normalized CIFAR-10 test images for VR profiling
    test_images_list = []
    for inputs, _ in testloader:
        test_images_list.append(inputs)
        if sum(len(t) for t in test_images_list) >= 256:
            break
    test_images = torch.cat(test_images_list)[:256]

    vr_path = _profile_vr(model, test_images, args.name)
    print(f"VR profile -> {vr_path}")


if __name__ == "__main__":
    main()
