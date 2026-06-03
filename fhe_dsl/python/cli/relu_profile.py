# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""ace_tool relu-profile — ReLU VR profiling subcommand.

Delegates to ace.model.relu_profile for the actual implementation.
"""

import logging

logging.basicConfig(level=logging.INFO, format="%(levelname)s %(name)s: %(message)s")


def run(args):
    """Run relu-profile subcommand."""
    from ace.model.relu_profile import profile_spec, write_profile, compare_profiles, _get_builtin_specs
    import torch

    specs = _get_builtin_specs()

    if not specs:
        print("No built-in specs found. Ensure ace.model.resnet is available.")
        return 1

    if args.model:
        specs = [s for s in specs if s.name in args.model
                 or any(m in s.name for m in args.model)]
        if not specs:
            print(f"No matching models. Available: {[s.name for s in _get_builtin_specs()]}")
            return 1

    # Load user-provided inputs if specified
    user_inputs = None
    if args.inputs:
        user_inputs = torch.load(args.inputs, map_location="cpu")
        if not isinstance(user_inputs, torch.Tensor):
            print(f"Error: --inputs file must contain a single tensor, got {type(user_inputs)}")
            return 1
        print(f"Loaded user inputs from {args.inputs}, shape={user_inputs.shape}")

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
            logging.getLogger(__name__).error("Failed to profile %s: %s", spec.name, e)
            import traceback
            traceback.print_exc()

    print("\nDone.")
    return 0
