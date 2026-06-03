# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""ace_tool dump-sample — dump sample images subcommand.

Delegates to ace.model.dump_sample for the actual implementation.
"""


def run(args):
    """Run dump-sample subcommand."""
    from ace.model.dump_sample import dump_sample
    dump_sample(args.dataset, args.num, args.offset, args.output)
