"""
PyACE Lowering Implementations

This package contains lowering implementations that transform high-level
nn::core operations into lower-level nn::vector operations.

Each lowering follows the pattern from ACE's nn-addon:
1. Transform input tensors (reshape, pad, etc.)
2. Transform weights (im2col, transpose_diagonal, etc.)
3. Generate vector metakernel (roll + mul + add)

Available lowerings:
- conv_lowering: Conv2D to vector operations (detailed im2col implementation)
- conv_full_lowering: Full loop structure using control flow DSL
"""

from .conv_lowering import (
    ConvLowering,
    ConvParams,
    get_im2col_kernel,
    expand_bias,
    conv_metakernel_compute,
)

from .conv_full_lowering import (
    conv_metakernel_full,
    conv_kernel_3x3,
    generate_conv_ir,
)

__all__ = [
    # Detailed conv lowering
    'ConvLowering',
    'ConvParams',
    'get_im2col_kernel',
    'expand_bias',
    'conv_metakernel_compute',
    # Full loop structure conv
    'conv_metakernel_full',
    'conv_kernel_3x3',
    'generate_conv_ir',
]

