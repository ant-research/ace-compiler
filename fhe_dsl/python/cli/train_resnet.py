# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""ace_tool train-resnet — train ResNet models subcommand.

Delegates to ace.model.train_resnet for the actual implementation.
"""


def run(args):
    """Run train-resnet subcommand."""
    import torch
    import torch.nn as nn
    import torch.optim as optim
    import torchvision
    import torchvision.transforms as transforms
    import os
    import time

    from ace.model.train_resnet import (
        get_model, get_train_transform, get_test_transform,
        train_epoch, test_epoch, adjust_learning_rate, count_parameters,
    )

    device = torch.device(args.device if torch.cuda.is_available() else "cpu")
    print(f"Using device: {device}")
    if device.type == "cuda":
        print(f"GPU: {torch.cuda.get_device_name(0)}")

    os.makedirs(args.save_dir, exist_ok=True)

    num_classes = 10 if args.dataset == "cifar10" else 100
    print(f"Dataset: {args.dataset.upper()}, Classes: {num_classes}")

    # Data loading
    print(f"\nLoading {args.dataset.upper()} dataset...")
    data_root = "./data"

    if args.dataset == "cifar10":
        trainset = torchvision.datasets.CIFAR10(
            root=data_root, train=True, download=True,
            transform=get_train_transform("cifar10"))
        testset = torchvision.datasets.CIFAR10(
            root=data_root, train=False, download=True,
            transform=get_test_transform("cifar10"))
    else:
        trainset = torchvision.datasets.CIFAR100(
            root=data_root, train=True, download=True,
            transform=get_train_transform("cifar100"))
        testset = torchvision.datasets.CIFAR100(
            root=data_root, train=False, download=True,
            transform=get_test_transform("cifar100"))

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

    # Resume from checkpoint
    start_epoch = 0
    best_acc = 0.0
    if args.resume:
        if os.path.isfile(args.resume):
            print(f"Loading checkpoint from {args.resume}")
            checkpoint = torch.load(args.resume, map_location=device)
            model.load_state_dict(checkpoint.get("state_dict", checkpoint))
            start_epoch = checkpoint.get("epoch", 0)
            best_acc = checkpoint.get("best_acc", 0.0)
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

        start_time = time.time()
        train_loss, train_acc = train_epoch(model, trainloader, criterion, optimizer, device)
        test_loss, test_acc = test_epoch(model, testloader, criterion, device)
        epoch_time = time.time() - start_time

        print(f"Train Loss: {train_loss:.4f} Acc: {train_acc:.2f}%")
        print(f"Test  Loss: {test_loss:.4f} Acc: {test_acc:.2f}%")
        print(f"Time: {epoch_time:.1f}s")

        if test_acc > best_acc:
            best_acc = test_acc
            save_name = f"{args.dataset}_resnet{args.model}.pt"
            save_path = os.path.join(args.save_dir, save_name)
            torch.save({
                "epoch": epoch + 1,
                "state_dict": model.state_dict(),
                "best_acc": best_acc,
                "optimizer": optimizer.state_dict(),
            }, save_path)
            print(f"Saved best model to {save_path} (Acc: {best_acc:.2f}%)")

    print("\n" + "=" * 70)
    print(f"Training completed!")
    print(f"Best test accuracy: {best_acc:.2f}%")
    print(f"Model saved to: {os.path.join(args.save_dir, f'{args.dataset}_resnet{args.model}.pt')}")
    print("=" * 70)
