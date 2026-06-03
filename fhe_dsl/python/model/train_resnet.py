#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Train ResNet models (20/32/44/56/110) on CIFAR-10/100 dataset.

Usage:
  # Recommended (after pip install):
  ace_tool train-resnet --model 20 --epochs 200 --dataset cifar10
  ace_tool train-resnet --model 110 --epochs 200 --dataset cifar100 --batch-size 128

  # Also available as:
  python -m ace.model.train_resnet --model 20 --epochs 200 --dataset cifar10

Available models:
  - ResNet-20:  [3, 3, 3]  layers, ~0.27M params
  - ResNet-32:  [5, 5, 5]  layers, ~0.46M params
  - ResNet-44:  [7, 7, 7]  layers, ~0.66M params
  - ResNet-56:  [9, 9, 9]  layers, ~0.85M params
  - ResNet-110: [18, 18, 18] layers, ~1.7M params
"""
import argparse
import os
import time

import torch
import torch.nn as nn
import torch.optim as optim
import torchvision
import torchvision.transforms as transforms

from ace.model.resnet.resnet20 import resnet20_cifar10
from ace.model.resnet.resnet32 import resnet32_cifar10
from ace.model.resnet.resnet44 import resnet44_cifar10
from ace.model.resnet.resnet56 import resnet56_cifar10
from ace.model.resnet.resnet110 import resnet110_cifar10


# Model registry
MODEL_REGISTRY = {
    20: resnet20_cifar10,
    32: resnet32_cifar10,
    44: resnet44_cifar10,
    56: resnet56_cifar10,
    110: resnet110_cifar10,
}


def get_model(model_depth: int, num_classes: int = 10):
    """Get ResNet model by depth."""
    if model_depth not in MODEL_REGISTRY:
        raise ValueError(f"Unsupported model depth: {model_depth}. "
                        f"Available: {list(MODEL_REGISTRY.keys())}")
    return MODEL_REGISTRY[model_depth](num_classes=num_classes)


def get_train_transform(dataset: str = 'cifar10'):
    """Get training data augmentation."""
    if dataset == 'cifar10':
        mean = (0.4914, 0.4822, 0.4465)
        std = (0.2023, 0.1994, 0.2010)
    else:  # cifar100
        mean = (0.5071, 0.4867, 0.4408)
        std = (0.2675, 0.2565, 0.2761)

    return transforms.Compose([
        transforms.RandomCrop(32, padding=4),
        transforms.RandomHorizontalFlip(),
        transforms.ToTensor(),
        transforms.Normalize(mean, std),
    ])


def get_test_transform(dataset: str = 'cifar10'):
    """Get test data transform."""
    if dataset == 'cifar10':
        mean = (0.4914, 0.4822, 0.4465)
        std = (0.2023, 0.1994, 0.2010)
    else:  # cifar100
        mean = (0.5071, 0.4867, 0.4408)
        std = (0.2675, 0.2565, 0.2761)

    return transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize(mean, std),
    ])


def train_epoch(model, trainloader, criterion, optimizer, device):
    """Train for one epoch."""
    model.train()
    running_loss = 0.0
    correct = 0
    total = 0

    for batch_idx, (inputs, targets) in enumerate(trainloader):
        inputs, targets = inputs.to(device), targets.to(device)

        optimizer.zero_grad()
        outputs = model(inputs)
        loss = criterion(outputs, targets)
        loss.backward()
        optimizer.step()

        running_loss += loss.item()
        _, predicted = outputs.max(1)
        total += targets.size(0)
        correct += predicted.eq(targets).sum().item()

        if (batch_idx + 1) % 100 == 0:
            print(f"  Batch [{batch_idx + 1}/{len(trainloader)}] "
                  f"Loss: {running_loss / (batch_idx + 1):.4f} "
                  f"Acc: {100. * correct / total:.2f}%")

    return running_loss / len(trainloader), 100. * correct / total


def test_epoch(model, testloader, criterion, device):
    """Test for one epoch."""
    model.eval()
    test_loss = 0
    correct = 0
    total = 0

    with torch.no_grad():
        for inputs, targets in testloader:
            inputs, targets = inputs.to(device), targets.to(device)
            outputs = model(inputs)
            loss = criterion(outputs, targets)

            test_loss += loss.item()
            _, predicted = outputs.max(1)
            total += targets.size(0)
            correct += predicted.eq(targets).sum().item()

    acc = 100. * correct / total
    avg_loss = test_loss / len(testloader)
    return avg_loss, acc


def adjust_learning_rate(optimizer, epoch, initial_lr, schedule='standard'):
    """Adjust learning rate according to epoch."""
    lr = initial_lr
    if schedule == 'standard':
        # Drop at 100 and 150 epochs (for 200 epoch training)
        if epoch >= 100:
            lr = initial_lr * 0.1
        if epoch >= 150:
            lr = initial_lr * 0.01
    elif schedule == 'cosine':
        # Cosine annealing
        import math
        lr = initial_lr * 0.5 * (1 + math.cos(math.pi * epoch / 200))
    elif schedule == 'step':
        # Drop every 60 epochs
        lr = initial_lr * (0.1 ** (epoch // 60))

    for param_group in optimizer.param_groups:
        param_group['lr'] = lr
    return lr


def count_parameters(model):
    """Count trainable parameters."""
    return sum(p.numel() for p in model.parameters() if p.requires_grad)


def main():
    parser = argparse.ArgumentParser(
        description='Train ResNet models on CIFAR-10/100',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Train ResNet-20 on CIFAR-10
  python -m ace.model.train_resnet --model 20 --epochs 200 --dataset cifar10

  # Train ResNet-110 on CIFAR-100
  python -m ace.model.train_resnet --model 110 --epochs 200 --dataset cifar100

  # Quick test run (10 epochs)
  python -m ace.model.train_resnet --model 32 --epochs 10 --batch-size 256

  # Use cosine learning rate schedule
  python -m ace.model.train_resnet --model 56 --lr-schedule cosine
        """)

    # Model arguments
    parser.add_argument('--model', type=int, required=True,
                        choices=[20, 32, 44, 56, 110],
                        help='ResNet model depth')
    parser.add_argument('--dataset', type=str, default='cifar10',
                        choices=['cifar10', 'cifar100'],
                        help='Dataset to use')

    # Training arguments
    parser.add_argument('--epochs', type=int, default=200,
                        help='Number of training epochs (default: 200)')
    parser.add_argument('--batch-size', type=int, default=128,
                        help='Batch size (default: 128)')
    parser.add_argument('--lr', type=float, default=0.1,
                        help='Initial learning rate (default: 0.1)')
    parser.add_argument('--momentum', type=float, default=0.9,
                        help='SGD momentum (default: 0.9)')
    parser.add_argument('--weight-decay', type=float, default=5e-4,
                        help='Weight decay (default: 5e-4)')
    parser.add_argument('--lr-schedule', type=str, default='standard',
                        choices=['standard', 'cosine', 'step'],
                        help='Learning rate schedule (default: standard)')

    # System arguments
    parser.add_argument('--device', type=str, default='cuda',
                        help='Device to use (default: cuda)')
    parser.add_argument('--num-workers', type=int, default=4,
                        help='Number of data loading workers (default: 4)')
    parser.add_argument('--save-dir', type=str, default='weights',
                        help='Directory to save weights (default: weights)')
    parser.add_argument('--resume', type=str, default='',
                        help='Resume from checkpoint path')

    args = parser.parse_args()

    # Setup device
    device = torch.device(args.device if torch.cuda.is_available() else 'cpu')
    print(f"Using device: {device}")
    if device.type == 'cuda':
        print(f"GPU: {torch.cuda.get_device_name(0)}")

    # Create save directory
    os.makedirs(args.save_dir, exist_ok=True)

    # Dataset configuration
    num_classes = 10 if args.dataset == 'cifar10' else 100
    print(f"Dataset: {args.dataset.upper()}, Classes: {num_classes}")

    # Data loading
    print(f"\nLoading {args.dataset.upper()} dataset...")
    data_root = './data'

    if args.dataset == 'cifar10':
        trainset = torchvision.datasets.CIFAR10(
            root=data_root, train=True, download=True,
            transform=get_train_transform('cifar10'))
        testset = torchvision.datasets.CIFAR10(
            root=data_root, train=False, download=True,
            transform=get_test_transform('cifar10'))
    else:
        trainset = torchvision.datasets.CIFAR100(
            root=data_root, train=True, download=True,
            transform=get_train_transform('cifar100'))
        testset = torchvision.datasets.CIFAR100(
            root=data_root, train=False, download=True,
            transform=get_test_transform('cifar100'))

    trainloader = torch.utils.data.DataLoader(
        trainset, batch_size=args.batch_size, shuffle=True,
        num_workers=args.num_workers, pin_memory=True)
    testloader = torch.utils.data.DataLoader(
        testset, batch_size=100, shuffle=False,
        num_workers=args.num_workers, pin_memory=True)

    print(f"Train samples: {len(trainset)}, Test samples: {len(testset)}")

    # Model
    print(f"\nCreating ResNet-{args.model} model...")
    model = get_model(args.model, num_classes=num_classes)
    model = model.to(device)

    num_params = count_parameters(model)
    print(f"Model parameters: {num_params / 1e6:.2f}M")

    # Resume from checkpoint if specified
    start_epoch = 0
    best_acc = 0.0
    if args.resume:
        if os.path.isfile(args.resume):
            print(f"Loading checkpoint from {args.resume}")
            checkpoint = torch.load(args.resume, map_location=device)
            model.load_state_dict(checkpoint.get('state_dict', checkpoint))
            start_epoch = checkpoint.get('epoch', 0)
            best_acc = checkpoint.get('best_acc', 0.0)
            print(f"Resumed from epoch {start_epoch}, best_acc={best_acc:.2f}%")
        else:
            print(f"Warning: Resume file not found: {args.resume}")

    # Loss and optimizer
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.SGD(model.parameters(), lr=args.lr,
                          momentum=args.momentum, weight_decay=args.weight_decay)

    # Training loop
    print("\n" + "=" * 70)
    print(f"Starting training ResNet-{args.model} on {args.dataset.upper()}")
    print(f"Epochs: {args.epochs}, Batch size: {args.batch_size}, LR: {args.lr}")
    print("=" * 70)

    for epoch in range(start_epoch, args.epochs):
        lr = adjust_learning_rate(optimizer, epoch, args.lr, args.lr_schedule)
        print(f"\nEpoch: [{epoch + 1}/{args.epochs}] LR: {lr:.4f}")

        # Train
        start_time = time.time()
        train_loss, train_acc = train_epoch(model, trainloader, criterion, optimizer, device)

        # Test
        test_loss, test_acc = test_epoch(model, testloader, criterion, device)

        epoch_time = time.time() - start_time

        print(f"Train Loss: {train_loss:.4f} Acc: {train_acc:.2f}%")
        print(f"Test  Loss: {test_loss:.4f} Acc: {test_acc:.2f}%")
        print(f"Time: {epoch_time:.1f}s")

        # Save best model
        if test_acc > best_acc:
            best_acc = test_acc
            save_name = f"{args.dataset}_resnet{args.model}.pt"
            save_path = os.path.join(args.save_dir, save_name)

            torch.save({
                'epoch': epoch + 1,
                'state_dict': model.state_dict(),
                'best_acc': best_acc,
                'optimizer': optimizer.state_dict(),
            }, save_path)
            print(f"Saved best model to {save_path} (Acc: {best_acc:.2f}%)")

    print("\n" + "=" * 70)
    print(f"Training completed!")
    print(f"Best test accuracy: {best_acc:.2f}%")
    print(f"Model saved to: {os.path.join(args.save_dir, f'{args.dataset}_resnet{args.model}.pt')}")
    print("=" * 70)


if __name__ == '__main__':
    main()