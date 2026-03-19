"""
Simple Add Kernel
=================

Basic element-wise addition kernel for FHE compilation.
"""

from ace_dsl.frontend.decorator import kernel
from ace_dsl.core.types import Tensor

ConstInt = int  # Constant integer type


@kernel
def add_kernel(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
    """Simple element-wise addition."""
    return a + b


@kernel
def add_mul_kernel(a: Tensor[64], b: Tensor[64], c: Tensor[64]) -> Tensor[64]:
    """Addition followed by multiplication: (a + b) * c"""
    t = a + b
    return t * c


@kernel
def fused_add_sub_kernel(a: Tensor[64], b: Tensor[64], c: Tensor[64]) -> Tensor[64]:
    """Fused add and subtract: (a + b) - c"""
    t = a + b
    return t - c


@kernel  
def polynomial_kernel(x: Tensor[64], a0: Tensor[64], a1: Tensor[64], a2: Tensor[64]) -> Tensor[64]:
    """Polynomial evaluation: a0 + a1*x + a2*x^2"""
    x2 = x * x
    t1 = a1 * x
    t2 = a2 * x2
    return a0 + t1 + t2


if __name__ == "__main__":
    from ace_dsl.frontend.compile import compile_fhe
    
    print("=" * 60)
    print("Testing Simple Add Kernels")
    print("=" * 60)
    
    # Test add_kernel
    print("\n--- add_kernel ---")
    add_kernel.compile(enable_ir_printing=True)
    
    # Compile to FHE
    print("\n--- Compiling to FHE ---")
    c_code = compile_fhe(add_kernel, enable_ir_printing=True)
    print("\nGenerated C code (excerpt):")
    print(c_code[:500] if len(c_code) > 500 else c_code)
    
    print("\n✓ All simple add kernels defined successfully")

