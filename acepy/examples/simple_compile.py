#!/usr/bin/env python3
"""
Simple compilation example - no pass specification needed.

This demonstrates the high-level ace_compile() API that automatically
selects and runs the appropriate pipeline.
"""

import sys
sys.path.insert(0, '.')

from ace_dsl import kernel, ace_compile, CompilerOptions, Target
from ace_dsl.frontend.domain_kernels import nn_kernel, vector_kernel, NNTensor
from ace_dsl.frontend.lowering_registry import register_lowering

# ============================================================================
# Example 1: Simple kernel - defaults work
# ============================================================================

@kernel
def add_kernel(a, b):
    """Simple add - no passes specified, compiler handles it."""
    return a + b

print("=" * 70)
print("Example 1: Simple kernel with default pipeline")
print("=" * 70)

result = ace_compile(add_kernel)
print(f"Domain: {result.domain}")
print(f"Pipeline: {result.pipeline}")
print()
print(result.dump())


# ============================================================================
# Example 2: NN kernel with verbose output
# ============================================================================

@nn_kernel
def conv_relu(x: NNTensor, w: NNTensor, b: NNTensor) -> NNTensor:
    """Conv + ReLU - auto pipeline for nn::core domain."""
    h = x * w  # NN.mul
    h = h + b  # NN.add  
    return h

print()
print("=" * 70)
print("Example 2: NN kernel with verbose compilation")
print("=" * 70)

result = ace_compile(conv_relu, CompilerOptions(verbose=True))
print()
print(result.dump())


# ============================================================================
# Example 3: Compile to specific target
# ============================================================================

print()
print("=" * 70)
print("Example 3: Compile to specific target (VECTOR only)")
print("=" * 70)

result = ace_compile(conv_relu, CompilerOptions(
    target=Target.VECTOR,
    verbose=True
))
print()
print(f"Pipeline used: {result.pipeline}")


# ============================================================================
# Example 4: With custom lowerings (auto-detected)
# ============================================================================

@register_lowering("nn::core", "custom_op")
@vector_kernel
def my_custom_lowering(a, b):
    """Custom lowering - will be auto-inlined."""
    return a * b + a

@nn_kernel  
def model_with_custom_op(x: NNTensor, w: NNTensor) -> NNTensor:
    return x * w

print()
print("=" * 70)
print("Example 4: With custom lowerings")
print("=" * 70)

result = ace_compile(model_with_custom_op, CompilerOptions(
    verbose=True,
    inline_lowerings=True
))


# ============================================================================
# Summary
# ============================================================================

print()
print("=" * 70)
print("Summary: User Experience")
print("=" * 70)
print("""
Before (explicit passes):
    result = compile_with_python_lowering(
        kernel, 
        cpp_passes=["tensor2vector", "sihe2ckks", "ckks2poly", "poly2c"]
    )

After (automatic pipeline):
    result = ace_compile(kernel)  # That's it!
    
    # Or with options:
    result = ace_compile(kernel, CompilerOptions(verbose=True))
    
    # Or to specific target:
    result = ace_compile(kernel, CompilerOptions(target=Target.VECTOR))
""")

