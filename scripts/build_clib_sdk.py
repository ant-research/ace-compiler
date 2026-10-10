#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Install the SEAL-compatible CMake package backed by the clib SDK.
#
# This mirrors scripts/build_phantom-fhe.py's role in the GPU pipeline: it
# prepares the backend the FHE-MP-CNN build links against. The clib SDK ships
# prebuilt (baselines/fpga/clib-sdk: include/clib.h + lib/libclib.a); the
# SEAL compatibility layer lives in baselines/fpga/clib-seal. This script
# assembles both into a CMake package under
# baselines/fpga/build/clib-seal-pkg:
#
#   include/seal/seal.h   shim entry header (-> seal_api.h)
#   include/seal_api.h    seal:: API declarations
#   include/clib.h        clib SDK backend header
#   lib/libclib.a         clib SDK backend library (prebuilt)
#   src/seal_api.cxx      seal:: implementation, compiled by the consumer
#   lib/cmake/SEAL-3.6/   SEALConfig.cmake + SEALConfigVersion.cmake
#
# find_package(SEAL 3.6) on that package compiles src/seal_api.cxx into a
# static library and aliases SEAL::seal to it (linking libclib.a), because
# the prebuilt libclib.a carries only the clib:: symbols.
#
# Note: the message-mode (golden-model) bootstrap output level of 34 Q limbs
# that FHE-MP-CNN's level budget expects is baked into the seal_api.cxx
# template (CipherOps::bootstrap); no source patching step is needed here
# anymore.
#
# scripts/build_cnn_clib.py invokes this script before configuring the CNN.

import os
import shutil

CWD = '/app'

FHE_ROOT = os.path.join(CWD, 'baselines', 'fpga')
CLIB_SDK = os.path.join(FHE_ROOT, 'clib-sdk')
CLIB_INCLUDE = os.path.join(CLIB_SDK, 'include')
CLIB_LIB = os.path.join(CLIB_SDK, 'lib', 'libclib.a')

TEMPLATES = os.path.join(FHE_ROOT, 'clib-seal')
SEAL_PKG = os.path.join(FHE_ROOT, 'build', 'clib-seal-pkg')
SEAL_PKG_CMAKE = os.path.join(SEAL_PKG, 'lib', 'cmake', 'SEAL-3.6')

# Compatibility-layer files: (source under TEMPLATES, destination under SEAL_PKG).
TEMPLATE_FILES = {
    'seal/seal.h':              'include/seal/seal.h',
    'seal_api.h':               'include/seal_api.h',
    'seal_api.cxx':             'src/seal_api.cxx',
    'SEALConfig.cmake':         'lib/cmake/SEAL-3.6/SEALConfig.cmake',
    'SEALConfigVersion.cmake':  'lib/cmake/SEAL-3.6/SEALConfigVersion.cmake',
}

# clib backend files: (absolute source, destination under SEAL_PKG).
BACKEND_FILES = {
    CLIB_LIB:                   'lib/libclib.a',
    os.path.join(CLIB_INCLUDE, 'clib.h'): 'include/clib.h',
}


def install_seal_package():
    """Generate the SEAL-compatible package backed by the clib SDK."""
    if not os.path.isfile(CLIB_LIB):
        raise SystemExit(f"{CLIB_LIB} not found; the clib SDK is expected at {CLIB_SDK}")

    print(f"\ninstall SEAL-compatible package into {SEAL_PKG}")
    for sub in ('include/seal', 'lib', 'lib/cmake/SEAL-3.6', 'src'):
        os.makedirs(os.path.join(SEAL_PKG, sub), exist_ok=True)

    for src, dst in TEMPLATE_FILES.items():
        shutil.copyfile(os.path.join(TEMPLATES, src), os.path.join(SEAL_PKG, dst))
    for src, dst in BACKEND_FILES.items():
        shutil.copyfile(src, os.path.join(SEAL_PKG, dst))
    # The prebuilt libclib.a has no seal:: symbols; the compatibility layer
    # is compiled from src/seal_api.cxx by SEALConfig.cmake (target
    # seal_clib_shim) at the consumer's find_package(SEAL 3.6) time.


def main():
    install_seal_package()
    print(f"\nSEAL-compatible package installed: {SEAL_PKG}")


if __name__ == '__main__':
    main()