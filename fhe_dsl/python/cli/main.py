#!/usr/bin/env python3
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
ACE CLI entry point.

Usage:
    ace_tool <subcommand> [options]
    python -m ace.cli <subcommand> [options]

Subcommands:
    relu-profile    Profile ReLU value ranges for FHE models
    dump-sample     Dump sample images from datasets
    train-resnet    Train ResNet models on CIFAR-10/100
"""

import argparse
import sys


def main():
    parser = argparse.ArgumentParser(
        prog="ace_tool",
        description="ACE FHE Compiler & Runtime — command-line tools",
    )
    subparsers = parser.add_subparsers(dest="command", help="Available subcommands")

    # --- relu-profile ---
    p_relu = subparsers.add_parser(
        "relu-profile",
        help="Profile ReLU value ranges for FHE polynomial approximation",
        description="Generate per-call-site ReLU VR profiles for FHE models. "
                    "Uses FX Interpreter to track pre-ReLU activation ranges "
                    "across a dataset, producing VR values that match AIR IR "
                    "node names exactly.",
    )
    p_relu.add_argument("--model", nargs="+", default=None,
                        help="Model names to profile (e.g. resnet20 resnet110). "
                             "Default: all built-in models.")
    p_relu.add_argument("--inputs", type=str, default=None,
                        help="Path to .pt file with input tensor (N, C, H, W). "
                             "Overrides built-in dataset.")
    p_relu.add_argument("--num-samples", type=int, default=10000,
                        help="Number of dataset samples (default: 10000)")
    p_relu.add_argument("--margin", type=int, default=1,
                        help="Safety margin for VR calculation (default: 1)")
    p_relu.add_argument("--dry-run", action="store_true",
                        help="Show what would be done without writing files")
    p_relu.add_argument("--compare", action="store_true",
                        help="Compare new profiles with existing ones")
    p_relu.add_argument("--output", "-o", type=str, default=None,
                        help="Output directory for profile JSON files. "
                             "Default: source tree (git checkout) or ./profiles/ (pip install)")

    # --- dump-sample ---
    p_dump = subparsers.add_parser(
        "dump-sample",
        help="Dump sample images from datasets for testing and benchmarking",
        description="Export sample images from CIFAR-10/100 datasets to .npz files "
                    "for testing and benchmarking.",
    )
    p_dump.add_argument("--dataset", default="cifar10",
                        choices=["cifar10", "cifar100"],
                        help="Dataset to sample from (default: cifar10)")
    p_dump.add_argument("--num", "-n", type=int, default=1,
                        help="Number of images to dump (default: 1)")
    p_dump.add_argument("--offset", type=int, default=0,
                        help="Start index in the dataset (default: 0)")
    p_dump.add_argument("--output", "-o", default=None,
                        help="Output npz path (default: <dataset>_sample.npz)")

    # --- train-resnet ---
    p_train = subparsers.add_parser(
        "train-resnet",
        help="Train ResNet models on CIFAR-10/100",
        description="Train ResNet-20/32/44/56/110 on CIFAR-10 or CIFAR-100.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  ace_tool train-resnet --model 20 --epochs 200 --dataset cifar10
  ace_tool train-resnet --model 110 --epochs 200 --dataset cifar100
  ace_tool train-resnet --model 32 --epochs 10 --batch-size 256
        """,
    )
    p_train.add_argument("--model", type=int, required=True,
                         choices=[20, 32, 44, 56, 110],
                         help="ResNet model depth")
    p_train.add_argument("--dataset", type=str, default="cifar10",
                         choices=["cifar10", "cifar100"],
                         help="Dataset to use")
    p_train.add_argument("--epochs", type=int, default=200,
                         help="Number of training epochs (default: 200)")
    p_train.add_argument("--batch-size", type=int, default=128,
                         help="Batch size (default: 128)")
    p_train.add_argument("--lr", type=float, default=0.1,
                         help="Initial learning rate (default: 0.1)")
    p_train.add_argument("--momentum", type=float, default=0.9,
                         help="SGD momentum (default: 0.9)")
    p_train.add_argument("--weight-decay", type=float, default=5e-4,
                         help="Weight decay (default: 5e-4)")
    p_train.add_argument("--lr-schedule", type=str, default="standard",
                         choices=["standard", "cosine", "step"],
                         help="Learning rate schedule (default: standard)")
    p_train.add_argument("--device", type=str, default="cuda",
                         help="Device to use (default: cuda)")
    p_train.add_argument("--num-workers", type=int, default=4,
                         help="Number of data loading workers (default: 4)")
    p_train.add_argument("--save-dir", type=str, default="weights",
                         help="Directory to save weights (default: weights)")
    p_train.add_argument("--resume", type=str, default="",
                         help="Resume from checkpoint path")

    args = parser.parse_args()

    if args.command is None:
        parser.print_help()
        sys.exit(1)

    if args.command == "relu-profile":
        from .relu_profile import run
        run(args)
    elif args.command == "dump-sample":
        from .dump_sample import run
        run(args)
    elif args.command == "train-resnet":
        from .train_resnet import run
        run(args)


if __name__ == "__main__":
    main()
