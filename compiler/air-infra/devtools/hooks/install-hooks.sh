#!/bin/sh

#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

current_dir=$(pwd)
script_dir=$(cd "$(dirname "$0")"; pwd)
# echo "Found install-hooks.sh in        : $script_dir"
# echo "Running install-hooks.sh in      : $current_dir"

# Set git commit template
if [ -d "$current_dir/.git" ]; then
    git config commit.template $script_dir/.commit-template.txt
else
    exit 0
fi

if [ -d "$current_dir/.git/hooks" ]; then
    echo "-- Insatlling git hooks ..."
    cp $script_dir/prepare-commit-msg $current_dir/.git/hooks/prepare-commit-msg
    cp $script_dir/commit-msg $current_dir/.git/hooks/commit-msg
    chmod +x $current_dir/.git/hooks/prepare-commit-msg
    chmod +x $current_dir/.git/hooks/commit-msg
else
    echo "-- Checking git hook             : directory does not exist"
fi
