#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ReLU VR profile tool — generate per-call-site ReLU value range profiles.

Produces per-call-site VR values matching AIR IR node names exactly,
using FX Interpreter-based profiling from ace.fhe.config.profiler.

Usage:
    # Recommended (after pip install):
    ace_tool relu-profile --model resnet20
    ace_tool relu-profile --model resnet20 --inputs my_data.pt
    ace_tool relu-profile --dry-run
    ace_tool relu-profile --compare

    # Also available as:
    python -m ace.model.relu_profile --model resnet20

For custom models, use the Python API directly:
    from ace.fhe.config import ReLUProfiler, ModelSpec
    spec = ModelSpec(name="my_model", model_class=MyModel, ...)
    profiler = ReLUProfiler(spec)
    vr_data = profiler.profile(inputs=my_data, margin=1, save=True)
"""
import argparse
import json
import logging
import sys
from pathlib import Path
from typing import List

import torch

logging.basicConfig(level=logging.INFO, format="%(levelname)s %(name)s: %(message)s")
logger = logging.getLogger(__name__)


def _load_dataset(dataset_name: str, num_samples: int):
    """Load dataset images by name."""
    if dataset_name == "cifar10":
        from ace.model.dataset import load_cifar10_images
        return load_cifar10_images(num_samples)
    elif dataset_name == "cifar100":
        from ace.model.dataset import load_cifar100_images
        return load_cifar100_images(num_samples)
    else:
        raise ValueError(
            f"Unknown dataset: {dataset_name}. "
            "Add support in _load_dataset() or use ReLUProfiler directly."
        )


def _get_builtin_specs() -> List:
    """Get all built-in ModelSpec instances with weights_required=True."""
    try:
        from ace.model.spec_resnet import ALL_RESNET_SPECS
        return list(ALL_RESNET_SPECS)
    except ImportError:
        return []


def profile_spec(spec, num_samples=10000, margin=1, inputs=None):
    """Profile a ModelSpec using FX Interpreter and return result dict.

    Args:
        spec: ModelSpec instance.
        num_samples: Number of samples when using built-in dataset.
        margin: Safety margin for VR calculation.
        inputs: Optional tensor (N, C, H, W). If provided, overrides built-in dataset.
    """
    from ace.fhe.config.profiler import profile_relu_vr_fx

    model = spec.create_model()

    if inputs is not None:
        images = inputs
    else:
        images, _ = _load_dataset(spec.dataset, num_samples)

    logger.info("Profiling %s with %d samples (margin=%d)", spec.name, len(images), margin)
    result = profile_relu_vr_fx(model, images, margin=margin)
    return result


def _get_default_output_dir(spec) -> Path:
    """Resolve default output directory for profile JSON files.

    Strategy:
      1. Git repository root: profiles live at fhe_dsl/python/model/resnet/profiles/
         This ensures writes go to the source tree when running from a checkout.
      2. Current working directory: fallback for pip-install users who don't
         have a source checkout. Writes to ./profiles/{spec.name}.json.

    We intentionally do NOT fall back to the install directory (__file__),
    because writing to site-packages is unreliable (permissions, pip upgrades)
    and invisible to the user.
    """
    # Try git repository root
    try:
        import subprocess
        git_root = subprocess.check_output(
            ["git", "rev-parse", "--show-toplevel"],
            stderr=subprocess.DEVNULL,
        ).decode().strip()
        source_path = Path(git_root) / "fhe_dsl" / "python" / "model" / "resnet" / "profiles"
        if source_path.exists():
            return source_path
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass

    # Fallback: current working directory
    return Path.cwd() / "profiles"


def write_profile(spec, result, dry_run=False, output_dir=None):
    """Write profile result to JSON file.

    Args:
        spec: ModelSpec instance.
        result: Profile result dict from profile_spec().
        dry_run: If True, log what would be written without writing.
        output_dir: Explicit output directory. If None, auto-detected:
            - Git checkout → fhe_dsl/python/model/resnet/profiles/
            - Pip install  → ./profiles/
    """
    if output_dir is not None:
        base = Path(output_dir)
    else:
        base = _get_default_output_dir(spec)
    path = base / f"{spec.name}.json"

    if dry_run:
        logger.info("[DRY RUN] Would write %s (%d nodes)", path, len(result.get("per_node", {})))
        return path

    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w") as f:
        json.dump(result, f, indent=2)
    logger.info("Wrote %s (%d nodes)", path, len(result.get("per_node", {})))
    return path


def compare_profiles(spec, new_result):
    """Compare new FX profile with existing profile."""
    vr_file = spec.get_vr_profile()
    if vr_file is None:
        logger.info("No existing profile for %s", spec.name)
        return

    path = Path(vr_file)
    if not path.exists():
        logger.info("No existing profile file for %s", spec.name)
        return

    with open(path) as f:
        old = json.load(f)

    old_nodes = old.get("per_node", {})
    new_nodes = new_result.get("per_node", {})
    old_keys = set(old_nodes.keys())
    new_keys = set(new_nodes.keys())

    added = new_keys - old_keys
    removed = old_keys - new_keys
    common = old_keys & new_keys

    print(f"\n{'='*70}")
    print(f"  {spec.name}: old={len(old_keys)} nodes, new={len(new_keys)} nodes")
    print(f"{'='*70}")

    if added:
        print(f"\n  NEW nodes ({len(added)}):")
        for k in sorted(added):
            print(f"    + {k}: abs_max={new_nodes[k]['abs_max']:.4f}, vr={new_nodes[k]['vr']}")

    if removed:
        print(f"\n  REMOVED nodes ({len(removed)}):")
        for k in sorted(removed):
            print(f"    - {k}: abs_max={old_nodes[k]['abs_max']:.4f}, vr={old_nodes[k]['vr']}")

    if common:
        print(f"\n  CHANGED nodes ({len(common)}):")
        changed = 0
        for k in sorted(common):
            old_vr = old_nodes[k]["vr"]
            new_vr = new_nodes[k]["vr"]
            if old_vr != new_vr:
                changed += 1
                print(f"    ~ {k}: vr {old_vr} -> {new_vr} "
                      f"(abs_max {old_nodes[k]['abs_max']:.4f} -> {new_nodes[k]['abs_max']:.4f})")
        if changed == 0:
            print("    (no VR changes in common nodes)")

    print()


def main():
    parser = argparse.ArgumentParser(
        description="ReLU VR profile tool — generate per-call-site ReLU value range profiles",
        epilog="Examples:\n"
               "  python -m ace.model.relu_profile --model resnet20\n"
               "  python -m ace.model.relu_profile --model resnet20 --inputs my_data.pt\n"
               "  python -m ace.model.relu_profile --compare\n"
               "\nFor custom models, use the Python API:\n"
               "  from ace.fhe.config import ReLUProfiler, ModelSpec\n"
               "  profiler = ReLUProfiler(spec)\n"
               "  vr_data = profiler.profile(inputs=my_data, margin=1, save=True)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--model", nargs="+", default=None,
                        help="Model names to profile (e.g. resnet20 resnet110). Default: all built-in.")
    parser.add_argument("--inputs", type=str, default=None,
                        help="Path to .pt file with input tensor (N, C, H, W). "
                             "Overrides built-in dataset for the model.")
    parser.add_argument("--num-samples", type=int, default=10000,
                        help="Number of dataset samples when using built-in dataset (default: 10000)")
    parser.add_argument("--margin", type=int, default=1,
                        help="Safety margin for VR calculation (default: 1)")
    parser.add_argument("--dry-run", action="store_true",
                        help="Show what would be done without writing files")
    parser.add_argument("--compare", action="store_true",
                        help="Compare new profiles with existing ones")
    parser.add_argument("--output", "-o", type=str, default=None,
                        help="Output directory for profile JSON files. "
                             "Default: source tree (git checkout) or ./profiles/ (pip install)")
    args = parser.parse_args()

    specs = _get_builtin_specs()

    if not specs:
        print("No built-in specs found. Ensure ace.model.resnet is available.")
        sys.exit(1)

    if args.model:
        specs = [s for s in specs if s.name in args.model
                 or any(m in s.name for m in args.model)]
        if not specs:
            print(f"No matching models. Available: {[s.name for s in _get_builtin_specs()]}")
            sys.exit(1)

    # Load user-provided inputs if specified
    user_inputs = None
    if args.inputs:
        user_inputs = torch.load(args.inputs, map_location="cpu")
        if not isinstance(user_inputs, torch.Tensor):
            print(f"Error: --inputs file must contain a single tensor, got {type(user_inputs)}")
            sys.exit(1)
        logger.info("Loaded user inputs from %s, shape=%s", args.inputs, user_inputs.shape)

    print(f"Models to profile: {[s.name for s in specs]}")
    if user_inputs is not None:
        print(f"Inputs: {args.inputs} (shape={user_inputs.shape})")
    else:
        print(f"Samples: {args.num_samples}")
    print(f"Margin: {args.margin}")
    print()

    for spec in specs:
        try:
            result = profile_spec(
                spec,
                num_samples=args.num_samples,
                margin=args.margin,
                inputs=user_inputs,
            )

            if args.compare:
                compare_profiles(spec, result)

            write_profile(spec, result, dry_run=args.dry_run, output_dir=args.output)
        except Exception as e:
            logger.error("Failed to profile %s: %s", spec.name, e)
            import traceback
            traceback.print_exc()

    print("\nDone.")


if __name__ == "__main__":
    main()