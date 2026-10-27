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
    bench           Benchmark FHE inference (latency, memory)
    kms             Key Management Service (generate, list, inspect FHE keys)
    gds             GDS (GPUDirect Storage) verification and diagnostics
    doctor          Diagnose ACE FHE environment readiness
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
    p_relu.add_argument(
        "--model",
        nargs="+",
        default=None,
        help="Model names to profile (e.g. resnet20 resnet110). Default: all built-in models.",
    )
    p_relu.add_argument(
        "--inputs",
        type=str,
        default=None,
        help="Path to .pt file with input tensor (N, C, H, W). Overrides built-in dataset.",
    )
    p_relu.add_argument("--num-samples", type=int, default=10000, help="Number of dataset samples (default: 10000)")
    p_relu.add_argument("--margin", type=int, default=1, help="Safety margin for VR calculation (default: 1)")
    p_relu.add_argument("--dry-run", action="store_true", help="Show what would be done without writing files")
    p_relu.add_argument("--compare", action="store_true", help="Compare new profiles with existing ones")
    p_relu.add_argument(
        "--output",
        "-o",
        type=str,
        default=None,
        help="Output directory for profile JSON files. "
        "Default: source tree (git checkout) or ./profiles/ (pip install)",
    )

    # --- dump-sample ---
    p_dump = subparsers.add_parser(
        "dump-sample",
        help="Dump sample images from datasets for testing and benchmarking",
        description="Export sample images from CIFAR-10/100 datasets to .npz files for testing and benchmarking.",
    )
    p_dump.add_argument(
        "--dataset",
        default="cifar10",
        choices=["cifar10", "cifar100"],
        help="Dataset to sample from (default: cifar10)",
    )
    p_dump.add_argument("--num", "-n", type=int, default=1, help="Number of images to dump (default: 1)")
    p_dump.add_argument("--offset", type=int, default=0, help="Start index in the dataset (default: 0)")
    p_dump.add_argument("--output", "-o", default=None, help="Output npz path (default: <dataset>_sample.npz)")

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
    p_train.add_argument("--model", type=int, required=True, choices=[20, 32, 44, 56, 110], help="ResNet model depth")
    p_train.add_argument(
        "--dataset", type=str, default="cifar10", choices=["cifar10", "cifar100"], help="Dataset to use"
    )
    p_train.add_argument("--epochs", type=int, default=200, help="Number of training epochs (default: 200)")
    p_train.add_argument("--batch-size", type=int, default=128, help="Batch size (default: 128)")
    p_train.add_argument("--lr", type=float, default=0.1, help="Initial learning rate (default: 0.1)")
    p_train.add_argument("--momentum", type=float, default=0.9, help="SGD momentum (default: 0.9)")
    p_train.add_argument("--weight-decay", type=float, default=5e-4, help="Weight decay (default: 5e-4)")
    p_train.add_argument(
        "--lr-schedule",
        type=str,
        default="standard",
        choices=["standard", "cosine", "step"],
        help="Learning rate schedule (default: standard)",
    )
    p_train.add_argument("--device", type=str, default="cuda", help="Device to use (default: cuda)")
    p_train.add_argument("--num-workers", type=int, default=4, help="Number of data loading workers (default: 4)")
    p_train.add_argument("--save-dir", type=str, default="weights", help="Directory to save weights (default: weights)")
    p_train.add_argument("--resume", type=str, default="", help="Resume from checkpoint path")

    # --- bench (subcommand group) ---
    p_bench = subparsers.add_parser(
        "bench",
        help="Benchmark FHE inference performance",
        description="Run FHE inference benchmarks with detailed timing, "
        "memory, and accuracy measurements across models and backends.",
    )
    bench_subparsers = p_bench.add_subparsers(dest="bench_command", help="Benchmark subcommands")
    from .bench import register_parsers

    register_parsers(bench_subparsers)

    # --- kms (subcommand group) ---
    p_kms = subparsers.add_parser(
        "kms",
        help="Key Management Service — generate, inspect, and manage FHE keys",
        description="Key Management Service for FHE encryption and evaluation keys. "
        "Generate encryption keys (SK + PK) for client-side data encryption, "
        "and evaluation keys (Relin + Galois + Bootstrap) for server-side computation.",
    )
    kms_subparsers = p_kms.add_subparsers(dest="kms_command", help="KMS subcommands")
    from .kms import register_parsers

    register_parsers(kms_subparsers)

    # --- gds (subcommand group) ---
    p_gds = subparsers.add_parser(
        "gds",
        help="GDS (GPUDirect Storage) verification and diagnostics",
        description="Verify GDS/cuFile I/O or diagnose environment issues. "
        "Use 'check' to test full I/O round-trip (requires GPU + cuFile), "
        "or 'env-check' to diagnose prerequisites (no GPU I/O, safe for containers).",
    )
    gds_subparsers = p_gds.add_subparsers(dest="gds_command", help="GDS subcommands")
    from .gds import register_parsers as register_gds_parsers

    register_gds_parsers(gds_subparsers)

    # --- doctor ---
    subparsers.add_parser(
        "doctor",
        help="Diagnose ACE FHE environment readiness",
        description="Check Python, compiled binaries, GPU/CUDA, "
        "FHE backends, model assets, and dataset availability. "
        "Reports pass/fail for each category with remediation hints. "
        "Exit code 0 if all critical checks pass, 1 otherwise.",
    )

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
    elif args.command == "bench":
        from .bench import run

        run(args)
    elif args.command == "kms":
        from .kms import run

        run(args)
    elif args.command == "gds":
        from .gds import run

        run(args)
    elif args.command == "doctor":
        from .doctor import run

        run(args)


if __name__ == "__main__":
    main()
