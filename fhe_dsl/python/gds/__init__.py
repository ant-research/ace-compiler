# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
GDS (GPUDirect Storage / cuFile) verification and diagnostics.

Submodules:
    cufile      — ctypes bindings for libcufile.so + CUDA helpers
    check_io    — full cuFile I/O round-trip verification
    check_env   — environment diagnostics (no GPU I/O required)
"""
