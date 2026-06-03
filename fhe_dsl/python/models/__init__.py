#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""Backward compatibility shim. Use ace.model instead."""

import warnings

warnings.warn(
    "ace.models is deprecated. Use ace.model instead.",
    DeprecationWarning,
    stacklevel=2,
)

from ace.model import *  # noqa: F401,F403
