#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

# Lazy loading: Load only on the first visit
_torch_ext = None

def _load_torch_ext():
    global _torch_ext
    if _torch_ext is None:
        try:
            # Primary: import from ace.runtime (new flat package structure)
            from ace import runtime as torch_ext
            _torch_ext = torch_ext
        except ImportError as e:
            raise ImportError(
                "Failed to import C++ extension 'runtime'. "
                "Please ensure it is installed correctly."
            ) from e
    return _torch_ext

# Provide a clean interface.
def get_global_config():
    return _load_torch_ext().GlobalConfig()

def set_glob_config(config):
    return _load_torch_ext().set_glob_config(config)

# Expose module interfaces
def get_module():
    return _load_torch_ext()
