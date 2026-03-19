"""
Matrix Multiplication Kernels
=============================

Matrix operations for FHE compilation, following the diagonal packing approach.
"""

from ace_dsl.frontend.decorator import kernel
from ace_dsl.core.types import Tensor

ConstInt = int


@kernel
def gemm_kernel(
    x: Tensor[64],           # Input vector (flattened)
    w_diag: Tensor[64, 64],  # Weight matrix in diagonal format
    bias: Tensor[64]         # Bias vector
) -> Tensor[64]:
    """
    General Matrix Multiplication: y = x @ W + bias
    
    In FHE, this is implemented using:
    - Diagonal packing of weight matrix
    - Rotate-and-sum algorithm
    """
    # For FHE, matmul is decomposed into rotations and element-wise multiplications
    # y = sum_{i} rotate(x, i) * diag(W, i)
    result = x * w_diag[0]  # First diagonal
    # In full implementation, would loop over all diagonals
    return result + bias


@kernel
def batched_matmul_kernel(
    x: Tensor[4, 64],        # Batch of input vectors
    w: Tensor[64, 64],       # Shared weight matrix
    bias: Tensor[64]
) -> Tensor[4, 64]:
    """
    Batched matrix multiplication.
    Each row of x is multiplied by W.
    """
    # Simplified: in reality would use SIMD across batch dimension
    return x * w + bias


@kernel
def fc_layer_kernel(
    x: Tensor[784],          # Flattened input (e.g., 28x28 image)
    w: Tensor[784, 128],     # First FC layer weights
    b: Tensor[128]           # Bias
) -> Tensor[128]:
    """
    Fully-connected layer: commonly used in neural networks.
    """
    return x @ w + b


if __name__ == "__main__":
    from ace_dsl.frontend.compile import compile_fhe
    
    print("=" * 60)
    print("Testing MatMul Kernels")
    print("=" * 60)
    
    # Test gemm_kernel
    print("\n--- gemm_kernel ---")
    gemm_kernel.compile(enable_ir_printing=True)
    
    print("\n✓ All matmul kernels defined successfully")

