#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from setuptools import setup, find_packages

setup(
    name='od',
    version='0.0.1',
    packages=find_packages(include=['od', 'od.*']),
    install_requires=[
    ],
)

# install td
setup(
    name='td',
    version='0.0.1',
    packages=find_packages(include=['td', 'td.*']),
    install_requires=[
        'pyyaml',
    ],
)
