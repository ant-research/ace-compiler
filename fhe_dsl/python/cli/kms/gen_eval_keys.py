# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
KMS gen-eval-keys subcommand — generate evaluation keys.

Usage:
    ace_tool kms gen-eval-keys --compiled-dir ./compiled/resnet20/ --output ./keys/resnet20-v1/
    ace_tool kms gen-eval-keys --compiled-dir ./compiled/resnet20/ --output ./keys/resnet20-v1/ --key-id resnet20-v1
"""


def register(subparsers):
    """Register the gen-eval-keys subcommand parser."""
    p = subparsers.add_parser(
        "gen-eval-keys",
        help="Generate evaluation keys (Relin + Galois + Bootstrap)",
        description="Generate CKKS evaluation keys from a compiled model's .conf file. "
        "Evaluation keys are model-specific and include relin keys, galois keys, "
        "and bootstrap keys.",
    )
    p.add_argument(
        "--compiled-dir",
        type=str,
        required=True,
        help="Path to compiled model directory containing .conf file",
    )
    p.add_argument(
        "--output",
        "-o",
        type=str,
        required=True,
        help="Output directory for generated key files",
    )
    p.add_argument(
        "--key-id",
        type=str,
        default=None,
        help="Optional key identifier (defaults to timestamp-based ID)",
    )


def run(args):
    """Execute the gen-eval-keys subcommand."""
    from ace.kms import generate_evaluation_keys

    generate_evaluation_keys(
        compiled_dir=args.compiled_dir,
        output_dir=args.output,
        key_id=args.key_id,
    )
