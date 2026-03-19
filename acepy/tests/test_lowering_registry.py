#!/usr/bin/env python3
"""
Test for Custom Lowering Registry

Demonstrates how to register custom lowering functions that expand
high-level operations into sequences of lower-level operations.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.lowering_registry import (
    register_lowering, get_lowering, has_lowering, 
    list_lowerings, clear_lowerings, LoweringContext
)
from ace_dsl.frontend.domain_kernels import (
    kernel, nn_kernel, vector_kernel, sihe_kernel,
    Tensor, VectorTensor, SiheCiphertext
)
from ace_dsl.bindings import air_builder


# ═══════════════════════════════════════════════════════════════════════════════
# Module-level kernel definitions (required for inspect.getsourcelines)
# ═══════════════════════════════════════════════════════════════════════════════

@vector_kernel
def lower_relu_impl(x: VectorTensor) -> VectorTensor:
    """Lower relu to max(x, 0)."""
    return x * x  # Simplified for test


@vector_kernel
def lower_conv2d_impl(input: VectorTensor, weight: VectorTensor, bias: VectorTensor) -> VectorTensor:
    """Lower conv2d to vector multiply-accumulate."""
    result = input * weight
    result = result + bias
    return result


@sihe_kernel
def lower_vector_add_impl(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    """Lower vector add to SIHE add."""
    return a + b


@sihe_kernel
def lower_vector_mul_impl(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    """Lower vector mul to SIHE mul."""
    return a * b


@vector_kernel
def lower_matmul_impl(a: VectorTensor, b: VectorTensor) -> VectorTensor:
    return a * b


@sihe_kernel
def lower_vmul_impl(a: SiheCiphertext, b: SiheCiphertext) -> SiheCiphertext:
    return a * b


@vector_kernel
def lower_softmax_impl(x: VectorTensor) -> VectorTensor:
    """Softmax = exp(x) / sum(exp(x))"""
    return x * x  # Placeholder


@nn_kernel
def attention_kernel_impl(q: Tensor, k: Tensor, v: Tensor) -> Tensor:
    """Simplified attention."""
    scores = q * k
    return scores * v


@vector_kernel
def lower_conv_impl(x: VectorTensor) -> VectorTensor:
    return x


@vector_kernel
def lower_pool_impl(x: VectorTensor) -> VectorTensor:
    return x


@sihe_kernel
def lower_reduce_impl(x: SiheCiphertext) -> SiheCiphertext:
    return x


# ═══════════════════════════════════════════════════════════════════════════════
# Tests
# ═══════════════════════════════════════════════════════════════════════════════

def test_basic_registration():
    """Test basic lowering registration."""
    print("\n" + "="*60)
    print("Test 1: Basic Lowering Registration")
    print("="*60)
    
    clear_lowerings()
    
    # Register the pre-defined kernel
    register_lowering("nn::core", "relu")(lower_relu_impl)
    
    # Verify registration
    assert has_lowering("nn::core", "relu"), "relu lowering should be registered"
    
    lowering = get_lowering("nn::core", "relu")
    assert lowering is not None
    assert lowering.source_domain == "nn::core"
    assert lowering.op_name == "relu"
    assert lowering.target_domain == "nn::vector"
    
    print("✓ Basic registration test passed")
    print(f"  Registered: {lowering.source_domain}::{lowering.op_name} → {lowering.target_domain}")


def test_conv_lowering():
    """Test convolution lowering to vector operations."""
    print("\n" + "="*60)
    print("Test 2: Conv Lowering Registration")
    print("="*60)
    
    clear_lowerings()
    
    # Register convolution lowering using pre-defined kernel
    register_lowering("nn::core", "conv2d", description="2D convolution using vector ops")(lower_conv2d_impl)
    
    # Compile and check IR
    lower_conv2d_impl.compile()
    ir = lower_conv2d_impl.dump_ir()
    
    print("  Conv2d lowering IR:")
    for line in ir.split('\n')[:15]:
        if line.strip():
            print(f"    {line}")
    
    assert "VECTOR.mul" in ir, "Should have VECTOR.mul"
    assert "VECTOR.add" in ir, "Should have VECTOR.add"
    
    lowering = get_lowering("nn::core", "conv2d")
    print(f"\n✓ Conv lowering registered: {lowering.description}")


def test_sihe_lowering():
    """Test lowering to SIHE operations."""
    print("\n" + "="*60)
    print("Test 3: SIHE Lowering Registration")
    print("="*60)
    
    clear_lowerings()
    
    # Register SIHE lowering for vector add
    register_lowering("nn::vector", "add")(lower_vector_add_impl)
    
    # Register SIHE lowering for vector mul
    register_lowering("nn::vector", "mul")(lower_vector_mul_impl)
    
    # List all lowerings
    all_lowerings = list_lowerings()
    print(f"  Registered {len(all_lowerings)} lowerings:")
    for l in all_lowerings:
        print(f"    {l.source_domain}::{l.op_name} → {l.target_domain}")
    
    # Compile one
    lower_vector_add_impl.compile()
    ir = lower_vector_add_impl.dump_ir()
    
    print("\n  Vector add → SIHE IR:")
    for line in ir.split('\n')[:12]:
        if line.strip():
            print(f"    {line}")
    
    assert "SIHE.add" in ir, "Should have SIHE.add"
    print("\n✓ SIHE lowering test passed")


def test_chained_lowering():
    """Test looking up lowerings in a chain."""
    print("\n" + "="*60)
    print("Test 4: Chained Lowering Lookup")
    print("="*60)
    
    clear_lowerings()
    
    # Register chain: nn::core → nn::vector → fhe::sihe
    register_lowering("nn::core", "matmul")(lower_matmul_impl)
    register_lowering("nn::vector", "mul")(lower_vmul_impl)
    
    # Check chain
    assert has_lowering("nn::core", "matmul"), "nn::core::matmul should exist"
    assert has_lowering("nn::vector", "mul"), "nn::vector::mul should exist"
    
    l1 = get_lowering("nn::core", "matmul")
    l2 = get_lowering("nn::vector", "mul")
    
    print(f"  Chain: nn::core::matmul → {l1.target_domain}")
    print(f"         nn::vector::mul → {l2.target_domain}")
    print("\n✓ Chained lowering lookup passed")


def test_lowering_with_kernel():
    """Test using lowering inside a kernel compilation."""
    print("\n" + "="*60)
    print("Test 5: Lowering with Kernel Integration")
    print("="*60)
    
    clear_lowerings()
    
    # Register a custom softmax lowering
    register_lowering("nn::core", "softmax")(lower_softmax_impl)
    
    # Compile attention kernel
    attention_kernel_impl.compile()
    ir = attention_kernel_impl.dump_ir()
    
    print("  Attention kernel IR:")
    for line in ir.split('\n')[:15]:
        if line.strip():
            print(f"    {line}")
    
    # Verify the lowering is available for the pass
    assert has_lowering("nn::core", "softmax")
    print("\n✓ Kernel with lowering test passed")


def test_list_domain_lowerings():
    """Test listing lowerings by domain."""
    print("\n" + "="*60)
    print("Test 6: List Lowerings by Domain")
    print("="*60)
    
    clear_lowerings()
    
    # Register multiple lowerings using pre-defined kernels
    register_lowering("nn::core", "conv")(lower_conv_impl)
    register_lowering("nn::core", "pool")(lower_pool_impl)
    register_lowering("nn::vector", "reduce")(lower_reduce_impl)
    
    # List by domain
    nn_lowerings = list_lowerings("nn::core")
    vec_lowerings = list_lowerings("nn::vector")
    
    print(f"  nn::core lowerings ({len(nn_lowerings)}):")
    for l in nn_lowerings:
        print(f"    - {l.op_name}")
    
    print(f"  nn::vector lowerings ({len(vec_lowerings)}):")
    for l in vec_lowerings:
        print(f"    - {l.op_name}")
    
    assert len(nn_lowerings) == 2
    assert len(vec_lowerings) == 1
    print("\n✓ Domain listing test passed")


def main():
    print("="*60)
    print("Testing Custom Lowering Registry")
    print("="*60)
    
    test_basic_registration()
    test_conv_lowering()
    test_sihe_lowering()
    test_chained_lowering()
    test_lowering_with_kernel()
    test_list_domain_lowerings()
    
    print("\n" + "="*60)
    print("All lowering registry tests passed!")
    print("="*60)


if __name__ == "__main__":
    main()

