"""
Conv2D Lowering Implementation

This module implements the lowering of nn::core::CONV to nn::vector operations,
following the pattern from nn-addon's tensor2vector_util.cxx.

The full algorithm from New_conv_metakernel:

1. Duplicate input channel_in times using roll+add
   input_dup = input + roll(input, -h*w) + roll(input, -2*h*w) + ...

2. Two-level loop nest:
   for cin in 0..channel_in:
       for khw in 0..kernel_hw:
           # Roll input by roll_amount[khw]
           input_rolled = roll(input_dup, ra[khw])
           
           # Slice weight at position [cin*kernel_hw + khw]
           weight_slice = slice(weight_im2col, cin*kernel_hw + khw, cout*h*w)
           
           # Multiply and accumulate
           tmp_result += input_rolled * weight_slice
       
       # Roll input_dup by h*w for next channel
       input_dup = roll(input_dup, h*w)

3. Add bias
   result = tmp_result + bias_expand

Reference: nn-addon/vector/src/tensor2vector_util.cxx:New_conv_metakernel
"""

import numpy as np
from typing import List, Tuple, Optional
from dataclasses import dataclass
from math import ceil

from ace_dsl.frontend.lowering_registry import register_lowering
from ace_dsl.frontend.domain_kernels import vector_kernel, VectorTensor


# ═══════════════════════════════════════════════════════════════════════════════
# Vector Operations (from nn::vector)
# ═══════════════════════════════════════════════════════════════════════════════

def vector_roll(x: np.ndarray, amount: int) -> np.ndarray:
    """VECTOR.roll - Rotate vector elements."""
    return np.roll(x, -amount)  # Negative because FHE rotation direction


def vector_add(a: np.ndarray, b: np.ndarray) -> np.ndarray:
    """VECTOR.add - Element-wise add."""
    return a + b


def vector_mul(a: np.ndarray, b: np.ndarray) -> np.ndarray:
    """VECTOR.mul - Element-wise multiply."""
    return a * b


def vector_slice(arr: np.ndarray, idx: int, length: int) -> np.ndarray:
    """VECTOR.slice - Extract slice from 2D array."""
    if arr.ndim == 2:
        return arr[idx]
    return arr[idx * length:(idx + 1) * length]


# ═══════════════════════════════════════════════════════════════════════════════
# Helper Functions (from tensor2vector_util.cxx)
# ═══════════════════════════════════════════════════════════════════════════════

def get_array_nchw(shape: List[int]) -> Tuple[int, int, int, int]:
    """Extract NCHW dimensions from shape."""
    if len(shape) == 4:
        return shape[0], shape[1], shape[2], shape[3]
    elif len(shape) == 3:
        return 1, shape[0], shape[1], shape[2]
    elif len(shape) == 2:
        return 1, 1, shape[0], shape[1]
    else:
        return 1, 1, 1, shape[0] if shape else 1


def gen_dup_input(input_vec: np.ndarray, dup_num: int, input_len: int) -> np.ndarray:
    """
    Generate duplicated input vector.
    
    input_dup = input + roll(input, -input_len) + roll(input, -2*input_len) + ...
    
    This ensures the rolled values don't wrap around incorrectly during
    the convolution computation.
    """
    result = input_vec.copy()
    for i in range(1, dup_num):
        rolled = vector_roll(input_vec, i * input_len)
        result = vector_add(result, rolled)
    return result


def get_im2col_kernel(
    weight: np.ndarray,
    channel_in: int,
    input_height: int,
    input_width: int,
    channel_out: int,
    kernel_height: int,
    kernel_width: int,
    padding: int = 1,
    stride: int = 1,
) -> Tuple[np.ndarray, List[int]]:
    """
    Transform convolution weights to im2col format.
    
    Returns:
        weight_im2col: Transformed weight [channel_in * kernel_hw, channel_out * h * w]
        ra: Roll amounts for each kernel position
    """
    kernel_hw = kernel_height * kernel_width
    output_height = input_height  # With padding=same
    output_width = input_width
    
    # Compute roll amounts for each kernel position
    # These are the offsets needed to align input with each kernel element
    ra = []
    kh_center = kernel_height // 2
    kw_center = kernel_width // 2
    
    for kh in range(kernel_height):
        for kw in range(kernel_width):
            # Roll amount: (kh - center) * width + (kw - center)
            roll_amt = (kh - kh_center) * input_width + (kw - kw_center)
            ra.append(roll_amt * stride)
    
    # Reshape weight for im2col:
    # Original: [channel_out, channel_in, kernel_h, kernel_w]
    # Target: [channel_in * kernel_hw, channel_out * output_h * output_w]
    
    weight_flat = weight.reshape(channel_out, channel_in * kernel_hw).T
    
    # Expand each kernel position to full output size
    output_size = channel_out * output_height * output_width
    weight_im2col = np.zeros((channel_in * kernel_hw, output_size))
    
    for cin_kpos in range(channel_in * kernel_hw):
        cout_idx = 0
        for cout in range(channel_out):
            # Each output channel gets the weight value repeated
            for oh in range(output_height):
                for ow in range(output_width):
                    # The weight value for this kernel position and output channel
                    cin = cin_kpos // kernel_hw
                    kpos = cin_kpos % kernel_hw
                    w_idx = cout * channel_in * kernel_hw + cin * kernel_hw + kpos
                    if w_idx < weight.size:
                        weight_im2col[cin_kpos, cout * output_height * output_width + oh * output_width + ow] = \
                            weight.flatten()[w_idx]
    
    return weight_im2col, ra


def expand_bias(
    bias: np.ndarray,
    channel_out: int,
    output_height: int,
    output_width: int
) -> np.ndarray:
    """Expand bias to match output shape [channel_out * output_h * output_w]."""
    bias_expand = np.zeros(channel_out * output_height * output_width)
    for c in range(channel_out):
        start = c * output_height * output_width
        end = (c + 1) * output_height * output_width
        bias_expand[start:end] = bias[c]
    return bias_expand


# ═══════════════════════════════════════════════════════════════════════════════
# Conv Metakernel (from New_conv_metakernel)
# ═══════════════════════════════════════════════════════════════════════════════

@dataclass
class ConvParams:
    """Parameters for convolution operation."""
    channel_in: int
    channel_out: int
    input_height: int
    input_width: int
    kernel_height: int
    kernel_width: int
    stride: int = 1
    padding: int = 1


def conv_metakernel_compute(
    input_1d: np.ndarray,
    weight_im2col: np.ndarray,
    bias_expand: np.ndarray,
    ra: List[int],
    channel_in: int,
    channel_out: int,
    output_height: int,
    output_width: int,
    kernel_hw: int,
    stride: int = 1
) -> np.ndarray:
    """
    Full conv metakernel implementation following nn-addon.
    
    This is the actual computation, not DSL - used for validation.
    
    Algorithm:
    1. Duplicate input dup_num times
    2. Loop over channel_in and kernel_hw
    3. Roll, slice, multiply, accumulate
    4. Add bias
    """
    output_size = channel_out * output_height * output_width
    input_len = channel_in * output_height * output_width
    
    # Calculate duplication number
    dup_num = int(ceil(channel_out / channel_in)) + 1
    
    # Step 1: Duplicate input
    # Pad input to handle duplication
    input_padded = np.zeros(input_len * dup_num)
    input_padded[:len(input_1d)] = input_1d
    input_dup = gen_dup_input(input_padded, dup_num, input_len)
    
    # Step 2: Initialize result
    tmp_result = np.zeros(output_size)
    
    # Step 3: Two-level loop
    for cin in range(channel_in):
        for khw in range(kernel_hw):
            # Roll input by ra[khw]
            input_rolled = vector_roll(input_dup, ra[khw])
            
            # Slice weight at position [cin * kernel_hw + khw]
            slice_idx = cin * kernel_hw + khw
            weight_slice = weight_im2col[slice_idx] if slice_idx < len(weight_im2col) else np.zeros(output_size)
            
            # Multiply and accumulate
            tmp_result = vector_add(tmp_result, vector_mul(input_rolled[:output_size], weight_slice))
        
        # Roll input_dup by h*w for next channel
        input_dup = vector_roll(input_dup, output_height * output_width)
    
    # Step 4: Add bias
    result = vector_add(tmp_result, bias_expand)
    
    return result


# ═══════════════════════════════════════════════════════════════════════════════
# Conv Lowering Class
# ═══════════════════════════════════════════════════════════════════════════════

class ConvLowering:
    """
    Full convolution lowering that matches nn-addon's implementation.
    
    The lowering converts:
        output = conv2d(input, weight, bias)
    
    Into vector operations:
        1. input_1d = reshape(input, [-1])
        2. input_dup = dup(input_1d, dup_num)
        3. for cin in channel_in:
               for khw in kernel_hw:
                   tmp += roll(input_dup, ra[khw]) * weight_slice[cin*khw+khw]
               input_dup = roll(input_dup, h*w)
        4. result = tmp + bias_expand
    """
    
    def __init__(
        self,
        channel_in: int,
        channel_out: int,
        input_height: int,
        input_width: int,
        kernel_height: int = 3,
        kernel_width: int = 3,
        stride: int = 1,
        padding: int = 1
    ):
        self.channel_in = channel_in
        self.channel_out = channel_out
        self.input_height = input_height
        self.input_width = input_width
        self.kernel_height = kernel_height
        self.kernel_width = kernel_width
        self.stride = stride
        self.padding = padding
        
        # Output dimensions (assuming padding='same')
        self.output_height = input_height
        self.output_width = input_width
        self.kernel_hw = kernel_height * kernel_width
        
        # Precompute im2col transformation
        self._weight_im2col = None
        self._ra = None
        
    def _compute_roll_amounts(self) -> List[int]:
        """Compute roll amounts for each position in the kernel."""
        ra = []
        kh_center = self.kernel_height // 2
        kw_center = self.kernel_width // 2
        
        for kh in range(self.kernel_height):
            for kw in range(self.kernel_width):
                roll = (kh - kh_center) * self.input_width + (kw - kw_center)
                ra.append(roll * self.stride)
        return ra
    
    def transform_weight(self, weight: np.ndarray) -> np.ndarray:
        """
        Transform weight tensor to im2col format.
        
        Input shape: [channel_out, channel_in, kernel_h, kernel_w]
        Output shape: [channel_in * kernel_hw, channel_out * output_h * output_w]
        """
        self._weight_im2col, self._ra = get_im2col_kernel(
            weight,
            self.channel_in,
            self.input_height,
            self.input_width,
            self.channel_out,
            self.kernel_height,
            self.kernel_width,
            self.padding,
            self.stride
        )
        return self._weight_im2col
    
    def transform_bias(self, bias: np.ndarray) -> np.ndarray:
        """Expand bias to match output shape."""
        return expand_bias(
            bias,
            self.channel_out,
            self.output_height,
            self.output_width
        )
    
    @property
    def roll_amounts(self) -> List[int]:
        """Get roll amounts for kernel positions."""
        if self._ra is None:
            self._ra = self._compute_roll_amounts()
        return self._ra
    
    def compute(
        self,
        input_1d: np.ndarray,
        weight_im2col: np.ndarray,
        bias_expand: np.ndarray
    ) -> np.ndarray:
        """Execute the conv metakernel computation."""
        return conv_metakernel_compute(
            input_1d,
            weight_im2col,
            bias_expand,
            self.roll_amounts,
            self.channel_in,
            self.channel_out,
            self.output_height,
            self.output_width,
            self.kernel_hw,
            self.stride
        )


# ═══════════════════════════════════════════════════════════════════════════════
# DSL Kernels for Lowering
# ═══════════════════════════════════════════════════════════════════════════════

# The DSL kernel represents the structure that will be emitted as AIR
# The actual complex loops are generated by the C++ lowering

@vector_kernel
def conv_vector_kernel(
    input_dup: VectorTensor,
    weight_slice: VectorTensor,
    partial_sum: VectorTensor
) -> VectorTensor:
    """
    Inner loop body of conv metakernel.
    
    This represents ONE iteration of the khw loop:
        partial_sum += input_rolled * weight_slice
    
    In full lowering, this is inside:
        for cin in channel_in:
            for khw in kernel_hw:
                <this kernel body>
    """
    result = input_dup * weight_slice   # VECTOR.mul
    result = result + partial_sum       # VECTOR.add
    return result


@vector_kernel
def conv_bias_add(
    conv_result: VectorTensor,
    bias_expand: VectorTensor
) -> VectorTensor:
    """Add expanded bias to conv result."""
    return conv_result + bias_expand  # VECTOR.add


# Register lowerings
register_lowering("nn::core", "conv")(conv_vector_kernel)
register_lowering("nn::core", "conv2d")(conv_vector_kernel)


# ═══════════════════════════════════════════════════════════════════════════════
# Example Usage
# ═══════════════════════════════════════════════════════════════════════════════

if __name__ == "__main__":
    print("=" * 70)
    print("Conv Lowering - Full Implementation from nn-addon")
    print("=" * 70)
    
    # Create a LeNet-style conv
    lowering = ConvLowering(
        channel_in=1,
        channel_out=6,
        input_height=28,
        input_width=28,
        kernel_height=5,
        kernel_width=5,
        stride=1,
        padding=2
    )
    
    print(f"\n1. Conv Parameters:")
    print(f"   Input:  [1, {lowering.channel_in}, {lowering.input_height}, {lowering.input_width}]")
    print(f"   Weight: [{lowering.channel_out}, {lowering.channel_in}, {lowering.kernel_height}, {lowering.kernel_width}]")
    print(f"   Output: [1, {lowering.channel_out}, {lowering.output_height}, {lowering.output_width}]")
    
    # Create sample data
    np.random.seed(42)
    weight = np.random.randn(
        lowering.channel_out,
        lowering.channel_in,
        lowering.kernel_height,
        lowering.kernel_width
    ).astype(np.float32)
    bias = np.random.randn(lowering.channel_out).astype(np.float32)
    input_1d = np.random.randn(lowering.channel_in * lowering.input_height * lowering.input_width).astype(np.float32)
    
    # Transform
    weight_im2col = lowering.transform_weight(weight)
    bias_expand = lowering.transform_bias(bias)
    
    print(f"\n2. Transformed Shapes:")
    print(f"   weight_im2col: {weight_im2col.shape}")
    print(f"   bias_expand:   {bias_expand.shape}")
    print(f"   roll_amounts:  {lowering.roll_amounts[:5]}... ({len(lowering.roll_amounts)} total)")
    
    print(f"\n3. Algorithm (from New_conv_metakernel):")
    print(f"   dup_num = ceil({lowering.channel_out}/{lowering.channel_in}) + 1 = {int(ceil(lowering.channel_out/lowering.channel_in)) + 1}")
    print(f"   Loop structure:")
    print(f"     for cin in 0..{lowering.channel_in}:")
    print(f"       for khw in 0..{lowering.kernel_hw}:")
    print(f"         input_rolled = roll(input_dup, ra[khw])")
    print(f"         weight_slice = slice(weight, cin*{lowering.kernel_hw}+khw)")
    print(f"         tmp_result += input_rolled * weight_slice")
    print(f"       input_dup = roll(input_dup, {lowering.output_height * lowering.output_width})")
    print(f"     result = tmp_result + bias_expand")
    
    # Execute computation
    result = lowering.compute(input_1d, weight_im2col, bias_expand)
    print(f"\n4. Computation Result:")
    print(f"   Output shape: {result.shape}")
    print(f"   Output[0:5]:  {result[:5]}")
    
    # Show DSL kernel IR
    print(f"\n5. DSL Kernel IR (inner loop body):")
    conv_vector_kernel.compile()
    ir = conv_vector_kernel.dump_ir()
    for line in ir.split('\n'):
        if line.strip():
            print(f"   {line}")
    
    print("\n" + "=" * 70)
    print("✓ Conv lowering example complete")
    print("=" * 70)
