#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Custom Operators Registry

This module contains mapping tables for PyTorch custom operators.
These mappings are used by the CustomTracer to identify custom ops
during FX tracing.
"""

from typing import Dict, Set, Tuple

# ============================================================================
# Standard Op Mapping - Maps torch functions to custom op names
# ============================================================================

STANDARD_OP_MAPPING: Dict[str, str] = {
    # torch functions
    'torch.add': 'tensor.add',
    'torch.sub': 'tensor.sub',
    'torch.mul': 'tensor.mul',
    'torch.div': 'tensor.div',
    'torch.matmul': 'tensor.matmul',
    'torch.nn.functional.relu': 'tensor.relu',
    'torch.nn.functional.softmax': 'tensor.softmax',
    'torch.nn.functional.max_pool2d': 'tensor.max_pool',
    'torch.nn.functional.adaptive_avg_pool2d': 'tensor.global_average_pool',
    'torch.flatten': 'tensor.flatten',
    'torch.sqrt': 'tensor.sqrt',
    'torch.nn.functional.silu': 'tensor.silu',

    # torch.ops.tensor (custom ops)
    'torch.ops.tensor.add': 'tensor.add',
    'torch.ops.tensor.sub': 'tensor.sub',
    'torch.ops.tensor.mul': 'tensor.mul',
    'torch.ops.tensor.div': 'tensor.div',
    'torch.ops.tensor.matmul': 'tensor.matmul',
    'torch.ops.tensor.relu': 'tensor.relu',
    'torch.ops.tensor.softmax': 'tensor.softmax',
    'torch.ops.tensor.max_pool': 'tensor.max_pool',
    'torch.ops.tensor.average_pool': 'tensor.average_pool',
    'torch.ops.tensor.global_average_pool': 'tensor.global_average_pool',
    'torch.ops.tensor.flatten': 'tensor.flatten',
    'torch.ops.tensor.sqrt': 'tensor.sqrt',
    'torch.ops.tensor.silu': 'tensor.silu',
    'torch.ops.tensor.conv': 'tensor.conv',
    'torch.ops.tensor.gemm': 'tensor.gemm',
}

# Set of custom operator names for quick lookup
CUSTOM_OPERATORS: Set[str] = set(STANDARD_OP_MAPPING.values())

# ============================================================================
# Op to AIR_GEN mapping - Maps custom op names to IRBuilder operation names
# ============================================================================

OP_TO_AIR_GEN: Dict[str, str] = {
    'tensor.add': 'add',
    'tensor.sub': 'sub',
    'tensor.mul': 'mul',
    'tensor.div': 'div',
    'tensor.matmul': 'matmul',
    'tensor.concat': 'concat',
    'tensor.relu': 'relu',
    'tensor.softmax': 'softmax',
    'tensor.max_pool': 'max_pool',
    'tensor.average_pool': 'average_pool',
    'tensor.global_average_pool': 'global_average_pool',
    'tensor.flatten': 'flatten',
    'tensor.sqrt': 'sqrt',
    'tensor.silu': 'silu',
    'tensor.conv': 'conv',
    'tensor.gemm': 'gemm',
    # reshape uses flatten in AIR (for ResNet flattening to 2D)
    'tensor.reshape': 'flatten',

    # Backward compatibility with old naming
    'torch.ops.tensor.add': 'add',
    'torch.ops.tensor.sub': 'sub',
    'torch.ops.tensor.mul': 'mul',
    'torch.ops.tensor.div': 'div',
    'torch.ops.tensor.matmul': 'matmul',
    'torch.ops.tensor.concat': 'concat',
    'torch.ops.tensor.relu': 'relu',
    'torch.ops.tensor.softmax': 'softmax',
    'torch.ops.tensor.max_pool': 'max_pool',
    'torch.ops.tensor.average_pool': 'average_pool',
    'torch.ops.tensor.global_average_pool': 'global_average_pool',
    'torch.ops.tensor.flatten': 'flatten',
    'torch.ops.tensor.sqrt': 'sqrt',
    'torch.ops.tensor.silu': 'silu',
    'torch.ops.tensor.conv': 'conv',
    'torch.ops.tensor.gemm': 'gemm',
}

# ============================================================================
# Hardware Map - Maps backend/device to C++ function suffix
# ============================================================================

HARDWARE_MAP: Dict[Tuple[str, str], str] = {
    ('antlib', 'cpu'): '_cpu',
    ('antlib', 'cuda'): '_gpu',
    ('phantom', 'cuda'): '_gpu',
    ('acelib', 'cuda'): '_gpu',
}


def get_cpp_function_name(op_name: str, backend: str, device: str) -> str:
    """
    Get the C++ function name for a given operator.

    Args:
        op_name: Operator name (e.g., 'tensor.add')
        backend: Backend name (e.g., 'antlib')
        device: Device name (e.g., 'cpu')

    Returns:
        C++ function name (e.g., 'tensor_add_cpu')
    """
    # Convert format: tensor.add -> tensor_add
    cpp_op_name = op_name.replace('.', '_')

    # Get hardware suffix
    suffix = HARDWARE_MAP.get((backend, device), '')

    return f"{cpp_op_name}{suffix}"