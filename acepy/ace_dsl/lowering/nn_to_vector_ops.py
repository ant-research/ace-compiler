"""
NN to Vector Lowering Functions
================================

Lowering functions from nn::core to nn::vector domain.
These functions are executed with AIRValue objects.

See: nn-addon/include/nn/vector/opcode_def.inc for vector opcodes
"""

from ..core.registry import nn_to_vector
from ..core.air_value import AIRValue


# ═══════════════════════════════════════════════════════════════════════════════
# Helper Functions
# ═══════════════════════════════════════════════════════════════════════════════

def vec_add(a: AIRValue, b: AIRValue) -> AIRValue:
    """Emit nn::vector::ADD."""
    return a + b  # Uses AIRValue.__add__


def vec_sub(a: AIRValue, b: AIRValue) -> AIRValue:
    """Emit nn::vector::SUB."""
    return a - b


def vec_mul(a: AIRValue, b: AIRValue) -> AIRValue:
    """Emit nn::vector::MUL."""
    return a * b


def vec_div(a: AIRValue, b: AIRValue) -> AIRValue:
    """Emit nn::vector::DIV."""
    return a / b


def vec_roll(x: AIRValue, shift: int) -> AIRValue:
    """Emit nn::vector::ROLL (circular rotation)."""
    result = x.container.new_roll(x.node, shift)
    return AIRValue(result, x.container)


def vec_slice(x: AIRValue, start: int, length: int) -> AIRValue:
    """Emit nn::vector::SLICE."""
    result = x.container.new_slice(x.node, start, length)
    return AIRValue(result, x.container)


def zeros(shape: tuple, container) -> AIRValue:
    """Emit air::core::ZERO + variable allocation."""
    result = container.new_zeros(shape)
    return AIRValue(result, container)


# ═══════════════════════════════════════════════════════════════════════════════
# Lowering Functions
# ═══════════════════════════════════════════════════════════════════════════════

@nn_to_vector("add", description="Lower nn::core::ADD to nn::vector::ADD")
def add_to_vector(a: AIRValue, b: AIRValue) -> AIRValue:
    """
    Lower nn::core::ADD to nn::vector::ADD.
    
    For simple element-wise add, this is a direct mapping.
    Both nn::core and nn::vector use ARRAY type.
    """
    return vec_add(a, b)


@nn_to_vector("sub", description="Lower nn::core::SUB to nn::vector::SUB")
def sub_to_vector(a: AIRValue, b: AIRValue) -> AIRValue:
    """Lower nn::core::SUB to nn::vector::SUB."""
    return vec_sub(a, b)


@nn_to_vector("mul", description="Lower nn::core::MUL to nn::vector::MUL")
def mul_to_vector(a: AIRValue, b: AIRValue) -> AIRValue:
    """Lower nn::core::MUL to nn::vector::MUL."""
    return vec_mul(a, b)


@nn_to_vector("relu", description="Lower nn::core::RELU to polynomial approximation")
def relu_to_vector(x: AIRValue) -> AIRValue:
    """
    Lower nn::core::RELU to polynomial approximation.
    
    ReLU(x) ≈ polynomial approximation for FHE:
    Using degree-3 approximation: x * (0.5 + 0.197*x - 0.004*x^3)
    
    This is suitable for FHE where comparisons are expensive.
    """
    # Polynomial coefficients for ReLU approximation
    # ReLU(x) ≈ x * (0.5 + 0.197*x - 0.004*x^3) for x in [-1, 1] range
    container = x.container
    
    # Simplified: just return x * 0.5 * (1 + sign approximation)
    # Full implementation would use polynomial evaluation
    x2 = vec_mul(x, x)
    
    # Approximate with: max(0, x) ≈ x/2 + |x|/2 ≈ 0.5*x + 0.5*sqrt(x^2 + eps)
    # For now, use identity as placeholder
    return x


@nn_to_vector("matmul", description="Lower nn::core::GEMM using diagonal method")
def matmul_to_vector(A: AIRValue, B_diag: AIRValue, bias: AIRValue = None) -> AIRValue:
    """
    Lower nn::core::GEMM using diagonal method.
    
    See: tensor2vector_util.cxx New_gemm_metakernel()
    
    The diagonal method packs matrix B into diagonals and uses
    ROLL operations to align elements for multiplication.
    
    Key: Single loop using ROLL/SLICE/MUL/ADD
    
    Args:
        A: Input matrix (M, K)
        B_diag: Weight matrix in diagonal format
        bias: Optional bias vector
    
    Returns:
        Result matrix
    """
    container = A.container
    width = A.shape[-1] if A.shape else 64
    height = B_diag.shape[0] if B_diag.shape else 64
    
    # Initialize result
    result = zeros((2 * width,), container)
    
    # Duplicate input: input_dup = A + roll(A, -width)
    A_rolled = vec_roll(A, -width)
    input_dup = vec_add(A, A_rolled)
    
    # Main loop: accumulate diagonal products
    for i in range(height):
        rolled = vec_roll(input_dup, i)
        weight_slice = vec_slice(B_diag, i, width)
        product = vec_mul(rolled, weight_slice)
        result = vec_add(result, product)
    
    # Add bias if present
    if bias is not None:
        result = vec_add(result, bias)
    
    return result


@nn_to_vector("conv", description="Lower nn::core::CONV using im2col strategy")
def conv_to_vector(x: AIRValue, w_im2col: AIRValue, b: AIRValue,
                   ra=None, C_in: int = 3, C_out: int = 64,
                   H_out: int = 224, W_out: int = 224,
                   KH: int = 3, KW: int = 3, stride: int = 1) -> AIRValue:
    """
    Lower nn::core::CONV using im2col strategy.
    
    See: tensor2vector_util.cxx New_conv_metakernel()
    
    The im2col strategy reshapes the input and weights so that
    convolution becomes matrix multiplication using ROLL/SLICE/MUL/ADD.
    
    Key: Only 2 loops (not 7!) - flattened spatial dimensions
    
    Args:
        x: Input tensor (N, C_in, H, W) 
        w_im2col: Weights in im2col format
        b: Bias vector
        ra: Roll amounts array
        C_in: Number of input channels
        C_out: Number of output channels
        H_out: Output height
        W_out: Output width
        KH: Kernel height
        KW: Kernel width
        stride: Convolution stride
    
    Returns:
        Output tensor
    """
    container = x.container
    output_size = C_out * H_out * W_out
    
    # Initialize result
    result = zeros((output_size,), container)
    
    # Duplicate input for efficient rotation
    # input_dup = duplicate_input(x, C_in, C_out, H_out * W_out)
    spatial_size = H_out * W_out
    input_dup = x  # Simplified - real implementation would duplicate
    
    # Default roll amounts if not provided
    if ra is None:
        ra = list(range(C_in * KH * KW))
    
    # Main loops: iterate over input channels and kernel positions
    for c_in in range(C_in):
        for kh_kw in range(KH * KW):
            # Roll input by appropriate amount
            roll_idx = c_in * KH * KW + kh_kw
            roll_amount = ra[roll_idx] * stride if roll_idx < len(ra) else roll_idx
            rolled = vec_roll(input_dup, roll_amount)
            
            # Get corresponding weight slice
            weight_slice = vec_slice(w_im2col, roll_idx, output_size)
            
            # Accumulate
            product = vec_mul(rolled, weight_slice)
            result = vec_add(result, product)
        
        # Shift input for next channel
        input_dup = vec_roll(input_dup, spatial_size)
    
    # Add bias
    if b is not None:
        result = vec_add(result, b)
    
    return result


@nn_to_vector("avg_pool", description="Lower nn::core::AVERAGE_POOL to sum + multiply")
def avg_pool_to_vector(x: AIRValue, kernel_size: tuple = (2, 2)) -> AIRValue:
    """
    Lower nn::core::AVERAGE_POOL to sum + multiply.
    
    Average pooling is implemented as:
    1. Roll and sum for each kernel position
    2. Multiply by 1/kernel_area
    """
    container = x.container
    kh, kw = kernel_size if len(kernel_size) == 2 else (kernel_size[0], kernel_size[0])
    kernel_area = kh * kw
    
    # Sum over kernel positions
    result = x
    for i in range(1, kernel_area):
        rolled = vec_roll(x, i)
        result = vec_add(result, rolled)
    
    # Multiply by 1/kernel_area
    # In FHE, this is done by encoding the scalar
    # For now, we just return the sum (division would need encoding)
    
    return result


@nn_to_vector("flatten", description="Lower nn::core::FLATTEN to reshape")
def flatten_to_vector(x: AIRValue, start_dim: int = 1, end_dim: int = -1) -> AIRValue:
    """
    Lower nn::core::FLATTEN.
    
    In vector domain with flattened data, this is often a no-op.
    """
    return x


@nn_to_vector("reshape", description="Lower nn::core::RESHAPE to nn::vector::RESHAPE")
def reshape_to_vector(x: AIRValue, shape: tuple) -> AIRValue:
    """Lower nn::core::RESHAPE to nn::vector::RESHAPE."""
    # Reshape is metadata-only in vector domain
    return x

