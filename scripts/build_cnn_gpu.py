#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
import subprocess

dir = f'/app/baselines/gpu'
pfhe = f'{dir}'

# build command
config_phantom=f'cmake -S {pfhe} -B {pfhe}/build -DCMAKE_BUILD_TYPE=Release;'
build_phantom=f'cmake --build {dir}/build -- -j'

# Configure phantom-fhe
print("\configure phantom-fhe")
ret = os.system(config_phantom)
assert(ret == 0)

# Build phantom-fhe
print("\nbuild phantom-fhe")
ret = os.system(build_phantom)
assert(ret == 0)
