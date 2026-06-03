#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""Dump sample images from datasets for testing and benchmarking.

Usage:
    # Recommended (after pip install):
    ace_tool dump-sample
    ace_tool dump-sample --num 5
    ace_tool dump-sample --dataset cifar100
    ace_tool dump-sample -o /path/to/sample.npz

    # Also available as:
    python -m ace.model.dump_sample

Output:
    npz file with keys:
        image: [N, 3, 32, 32] float32, normalized
        label: [N] int
"""
import argparse
import os

import numpy as np


def dump_sample(dataset="cifar10", num=1, offset=0, output=None):
    """Dump sample images from a dataset to npz.

    Args:
        dataset: Dataset name ("cifar10" or "cifar100").
        num: Number of images to dump.
        offset: Start index in the dataset (default 0).
        output: Output path. Defaults to <dataset>_sample.npz in current directory.
    """
    if dataset == "cifar10":
        from ace.model.dataset import load_cifar10_images
        images, labels = load_cifar10_images(num, offset=offset)
    elif dataset == "cifar100":
        from ace.model.dataset import load_cifar100_images
        images, labels = load_cifar100_images(num, offset=offset)
    else:
        raise ValueError(f"Unknown dataset: {dataset}. Supported: cifar10, cifar100")

    arr = images[:num].numpy()
    label_arr = np.array(labels[:num])

    if output is None:
        output = f"{dataset}_sample.npz"

    np.savez_compressed(output, image=arr, label=label_arr)
    print(f"Saved: {output} ({os.path.getsize(output)} bytes)")
    print(f"Images: {num}, Labels: {labels[:num]}")


def main():
    parser = argparse.ArgumentParser(description="Dump sample images from dataset")
    parser.add_argument("--dataset", default="cifar10", choices=["cifar10", "cifar100"],
                        help="Dataset to sample from (default: cifar10)")
    parser.add_argument("--num", "-n", type=int, default=1,
                        help="Number of images to dump (default: 1)")
    parser.add_argument("--offset", type=int, default=0,
                        help="Start index in the dataset (default: 0)")
    parser.add_argument("--output", "-o", default=None,
                        help="Output npz path (default: <dataset>_sample.npz)")
    args = parser.parse_args()
    dump_sample(args.dataset, args.num, args.offset, args.output)


if __name__ == "__main__":
    main()